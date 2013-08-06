/* --COPYRIGHT--,BSD
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 * The --position and --input_position arguments must always have even
 * horizontal settings.
 *
 * These platform-specific requirements also apply:
 *
 * dm6467: Does not have support for --crop due to H/W limitations. Also
 *         --smooth is not supported due to no Smooth module implementation on
 *         this platform. -a is not supported since there is no accelerated
 *         framecopy available. The width of --position, --input_position and
 *         -r need to be even. Example usage:
 *
 *         ./video_loopback_copy_dm6467.x470MV -r 642x481 --position 430x345 \
 *             --input_position 232x345
 *
 * dm6446: When -a or --smooth is used, the width of --position and
 *         --input_position needs to be a multiple of 16. When -r is used,
 *         the width needs to be an even number, and when --smooth is used
 *         both the width and the height of -r need to be a multiple of 16.
 *         Example usage:
 *
 *         ./video_loopback_copy_dm6446.x470MV -a -r 318x239 \
 *             --position 160x149 --input_position 80x73
 *
 *         ./video_loopback_copy_dm6446.x470MV -r 320x208 \
 *             --position 160x149 --input_position 80x73 --smooth
 *
 * dm355:  When --smooth is used, all IPIPE hardware restrictions apply, and
 *         the application will fail if any property is violated.  
 *
 *         The main thing to be aware of is that buffer accesses must start
 *         on 32-byte boundaries.  For the most part, this means your
 *         horizontal argument to --position must be a multiple of 16.
 *         Example usage:
 *
 *         ./video_loopback_copy_dm355.x470MV -a -r 318x239 \
 *             --position 160x149 --input_position 80x73
 *
 *         ./video_loopback_copy_dm355.x470MV -r 320x208 \
 *             --position 160x149 --input_position 80x73 --smooth
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <xdc/std.h>

#include "../appMain.h"

/* Default arguments for app */
#define DEFAULT_ARGS { FALSE, FALSE, FALSE, 0, 0, 0, 0, -1, -1, 0, 0 , Display_Output_COMPOSITE }

/*
 * Argument IDs for long options. They must not conflict with ASCII values,
 * so start them at 256.
 */
typedef enum
{
   ArgID_BENCHMARK = 256,
   ArgID_FCOPY_ACCEL,
   ArgID_POSITION,
   ArgID_INPUT_POSITION,
   ArgID_RESOLUTION,
   ArgID_SMOOTH,
   ArgID_CROP,
   ArgID_HELP,
   ArgID_NUMFRAMES,
   ArgID_DISPLAY_OUTPUT,
   ArgID_NUMARGS
} ArgID;

/******************************************************************************
 * usage
 ******************************************************************************/
static Void usage(void)
{
    fprintf(stderr, "Usage: video_loopback_copy_<platform> [options]\n\n"
        "Options:\n"
        "-a | --fcopy_accel       Use hardware acceleration if available for\n"
        "                         copying frames\n"
        "     --position          Output position ('x-offset'x'y-offset')\n"
        "                         position origin [Default: 0x0] is the\n"
        "                         upper-left corner of the screen\n"
        "                         Only used with --resolution.\n"
        "-r | --resolution        Image resolution ('width'x'height')\n"
        "                         [Defaults to same as display standard]\n"
        "     --input_position    Input position ('x-offset'x'y-offset')\n"
        "                         position origin [Default: 0x0] is the\n"
        "                         upper-left corner of the screen.\n"
        "                         Only used with --input_resolution.\n"
        "     --smooth            Enable the smooth module instead of a\n"
        "                         normal copy.\n"
        "     --crop              If -r is set, use cropping on the video\n"
        "                         input to set this resolution.\n"
        "     --benchmark         Print benchmarking information\n"
        "-O | --display_output    Video output to use (see below)\n"
        "-h | --help              Print usage information (this message)\n"
        "-n | --numframes         Number of frames to process [Default: 0 "
                                  "(infinite)]\n"
        "\nVideo output available:\n"
        "\tcomposite [Default]\n"
        "\tsvideo \n"
        "\tcomponent\n"
        "\tauto (select video output by reading sysfs)\n");
}


/******************************************************************************
 * parseArgs
 ******************************************************************************/
static Void parseArgs(Int argc, Char *argv[], Args *argsp)
{
    const char shortOptions[] = "ar:O:n:h";

    const struct option longOptions[] = {
        {"fcopy_accel",      no_argument,       NULL, ArgID_FCOPY_ACCEL      },
        {"position",         required_argument, NULL, ArgID_POSITION         },
        {"resolution",       required_argument, NULL, ArgID_RESOLUTION       },
        {"display_output",   required_argument, NULL, ArgID_DISPLAY_OUTPUT   },
        {"input_position",   required_argument, NULL, ArgID_INPUT_POSITION   },
        {"smooth",           no_argument,       NULL, ArgID_SMOOTH           },
        {"crop",             no_argument,       NULL, ArgID_CROP             },
        {"benchmark",        no_argument,       NULL, ArgID_BENCHMARK        },
        {"help",             no_argument,       NULL, ArgID_HELP             },
        {"numframes",        required_argument, NULL, ArgID_NUMFRAMES        },
        {0, 0, 0, 0}
    };

    Int  index;
    Int  argID;

    for (;;) {
        argID = getopt_long(argc, argv, shortOptions, longOptions, &index);

        if (argID == -1) {
            break;
        }

        switch (argID) {

            case ArgID_FCOPY_ACCEL:
            case 'a':
                argsp->accel = TRUE;
                break;

            case ArgID_POSITION:
                if (sscanf(optarg, "%dx%d", &argsp->xOut,
                                            &argsp->yOut) != 2) {
                    fprintf(stderr, "Invalid position supplied (%s)\n",
                            optarg);
                    usage();
                    exit(EXIT_FAILURE);
                }
                break;

            case ArgID_INPUT_POSITION:
                if (sscanf(optarg, "%dx%d", &argsp->xIn,
                                            &argsp->yIn) != 2) {
                    fprintf(stderr, "Invalid position supplied (%s)\n",
                            optarg);
                    usage();
                    exit(EXIT_FAILURE);
                }
                break;

            case ArgID_RESOLUTION:
            case 'r':
                if (sscanf(optarg, "%dx%d", &argsp->width,
                                            &argsp->height) != 2) {
                    fprintf(stderr, "Invalid resolution supplied (%s)\n",
                            optarg);
                    usage();
                    exit(EXIT_FAILURE);
                }
                break;

            case ArgID_DISPLAY_OUTPUT:
            case 'O':
                if (strcmp(optarg, "composite") == 0) {
                    argsp->videoOutput = Display_Output_COMPOSITE;
                } else if (strcmp(optarg, "svideo") == 0) {
                    argsp->videoOutput = Display_Output_SVIDEO;
                } else if (strcmp(optarg, "component") == 0) {
                    argsp->videoOutput = Display_Output_COMPONENT;
                } else if (strcmp(optarg, "auto") == 0) {
                    argsp->videoOutput = Display_Output_SYSTEM;
                } else {
                    fprintf(stderr, "Unknown display output\n");
                    usage();
                    exit(EXIT_FAILURE);
                }          
                break;
    
            case ArgID_SMOOTH:
                argsp->smooth = TRUE;
                break;

            case ArgID_CROP:
                argsp->crop = TRUE;
                break;

            case ArgID_BENCHMARK:
                argsp->benchmark = TRUE;
                break;

            case ArgID_HELP:
            case 'h':
                usage();
                exit(EXIT_SUCCESS);

            case ArgID_NUMFRAMES:
            case 'n':
                argsp->numFrames = atoi(optarg);
                break;

            default:
                usage();
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        usage();
        exit(EXIT_FAILURE);
    }
}

/******************************************************************************
 * main
 ******************************************************************************/
Int main(Int argc, Char *argv[])
{
    Args args = DEFAULT_ARGS;
    
    /* Parse the arguments given to the app */
    parseArgs(argc, argv, &args);    

    appMain(&args);
    
    return 0;
}
