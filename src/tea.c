/* tea.c: main file
opyright © 2015 Michael Zhu <boot2linux@gmail.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
must display the following acknowledgement:
This product includes software developed by the Tea.
4. Neither the name of the Tea nor the
names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Michael Zhu <boot2linux@gmail.com> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Michael Zhu <boot2linux@gmail.com> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include "terminal.h"
#include "tea.h"
#include "telnet.h"

static char *ver = "developing";

tea_t  tea = {
    .speed = B115200,
    .cs = 0,
    .stopbits = 1,
    .p = SER_PARITY_NONE,
    .flow = SER_FLOW_NONE,
};

static
void usage()
{
    fprintf(stderr,
            "Usage: tea [options]\n\n"
            "--version:                Show version\n"
            "--help|-h:                Help info\n"
            "--device|-d:              Serial port name or path. If no, try to open one automatically\n"
            "--speed|-s:               Serial port speed. Default is 115200\n"
            "--bits|-b <5|6|7|8>:      The number of data bits. Default is 8\n"
            "--stopbits|-t <1|2>:      The number of stop bit. Default is 1\n"
            "--parity|-p <odd|even>:   Parity setting. Default is none\n"
            "--flow|-f <xon>:          Flow control type. Default is none\n"
    );

}

int main(int argc, char *argv[])
{
    int number;
    int fd;

    int c;
    int option_index = 0;

    int version = 0;
    char *device = NULL;

    struct terminal *tm;
    struct serial *ser;

    struct option long_options[] = {

        {"version", no_argument,       &version, 1},
        {"help",    no_argument,       0, 'h'},
        {"device",  required_argument, 0, 'd'},
        {"speed",   required_argument, 0, 's'},
        {"bits",   required_argument, 0, 'b'},
        {"stopbits",   required_argument, 0, 't'},
        {"parity",   required_argument, 0, 'p'},
        {"flow",   required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };


    while (1) {

         c = getopt_long (argc, argv, "hd:s:b:t:p:f:",
                          long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 0:
        fprintf (stderr, "Version is %s\n", ver);
        exit(1);

        case 'h':
        usage();
        exit(1);

        case 'd':
        device = optarg;
        break;

        case 's':
        tea.speed = baudrate_to_speed(atoi(optarg));
        if ( tea.speed == 0 ) {
            fprintf(stderr, "Illegal baudrate\n");
            exit(1);
        }
        break;

        case 'b':
        tea.cs = atoi(optarg);
        if ( tea.cs < 5 || tea.cs> 8 ) {
            fprintf(stderr, "number of data bits is illegal\n");
            exit(1);
        }
        break;

        case 't':
        tea.stopbits = atoi(optarg);
        if ( tea.stopbits !=1 && tea.stopbits != 2 ) {
            fprintf(stderr, "number of stop bits is illegal\n");
            exit(1);
        }
        break;

        case 'p':
        if (0 == strcmp(optarg, "even"))
            tea.p = SER_PARITY_EVEN;
        else if (0 == strcmp(optarg, "odd"))
            tea.p = SER_PARITY_ODD;
        else {

            fprintf(stderr, "parity type is illegal\n");
            exit(1);
        }
        break;

        case 'f':
        if (0 == strcmp(optarg, "xon"))
            tea.flow = SER_FLOW_XON;
        else if (0 == strcmp(optarg, "none"))
            tea.flow = SER_FLOW_NONE;
        else {
            fprintf(stderr, "flow control type is illegal\n");
            return 0;
        }
        break;

        default:
        exit(1);
        }
    }

    number = scan_serial();
    if ( number == 0 )
    {
        fprintf(stderr, "No serial port or Permission denied\n");
        exit(1);
    }

    aev_loop_init(&tea.loop);
    /* tm = new_terminal(&tea, 0, 1); */
    start_telnet_server(&tea, NULL, "6000");
    aev_run(&tea.loop);
    /* delete_terminal(tm); */

    return 0;
}
