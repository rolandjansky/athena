# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

""" Helper wrapper to argparse.ArgumentParser for use in athena

    The user's options are separated from athena's command line arguments by a 
    single '-' character when athena is called.

    Public classes:
        AthArgumentParser

    Usage:
        from AthenaCommon.AthArgumentParser import AthArgumentParser
        parser = AthArgumentParser()
        parser.add_argument("-a", "--arg", action="store", help="Argument")

        args = parser.parse_args()
        print(args.arg) # etc
"""

import argparse
import sys
import inspect
import os
from subprocess import Popen, PIPE

# import the command line options back from the main process.
# This means that this code only runs in something like an athena call.
from __main__ import opts


class AthArgumentParser(argparse.ArgumentParser):
    """ Parse user commandline arguments in a job option. """

    def __init__(self, jo_name=None, prog=None, print_caller_help=True, **kwargs):
        """ Create the parser
            ---------
            Arguments
            ---------
            jo_name: 
                The name of the calling job option, used to format the help
                text. Will attempt to deduce it if possible
            prog:
                The start of the usage message, usually can be deduced
            print_caller_help:
                Whether or not to print the help text from the calling program
                (usually athena)

            All other keyword arguments are forwarded to the base class
            constructor
        """

        caller = sys.argv[0]
        if prog is None:
            if jo_name is None:
                # Use inspect to find which job option we're in. This isn't
                # completely foolproof but it's just a cosmetic detail so
                # doesn't need to be
                for frame in inspect.stack():
                    # For each frame in the stack, check the file name and keep
                    # it if it matches one of the scripts supplied
                    fname = inspect.getfile(frame[0])
                    try:
                        jo_name = next(
                            script for script in opts.scripts if fname.endswith(script)
                        )
                        break
                    except StopIteration:
                        pass
                else:
                    # fallback name
                    jo_name = "User job option"
            prog = "{0} {1} [{0} options] -".format(os.path.basename(caller), jo_name)
        if print_caller_help:
            # Get the help options for the caller
            caller_help, _ = Popen(
                [caller, "--help"], stdout=PIPE, stderr=PIPE
            ).communicate()
            # decode from bytes
            caller_help = caller_help.decode("utf-8")
            if caller.endswith("athena.py"):
                # If this is being called from athena, we know that there are
                # some lines to strip off
                caller_help = caller_help.split("\n", 3)[3]
            self.caller_help = "{} options:\n{}".format(
                os.path.basename(caller), caller_help
            )
        else:
            self.caller_help = ""
        super().__init__(prog=prog, **kwargs)
        super().add_argument(
            "remaining scripts",
            nargs="*",
            help="Any remaining scripts to be run by athena after this job option. Mainly used to run the 'post' script on the grid.",
        )

    def add_argument(self, *args, **kwargs):
        """ Add a new argument

        The difference between this and the base class is that it disallows
        adding positional arguments which are reserved by athena for job
        options. In particular, even if user code uses only one job option
        (which is common) pathena will add more when running on the grid
        """
        if not args or len(args) and args[0][0] not in self.prefix_chars:
            raise ValueError(
                "Positional arguments are not allowed! Defining them could mess up grid running!"
            )
        return super().add_argument(*args, **kwargs)

    def parse_args(self, args=None, namespace=None):
        """ Override the base class to use the leftover athena argumnets by
        default
        """
        if args is None:
            args = opts.user_opts
        return super().parse_args(args, namespace)

    def parse_known_args(self, args=None, namespace=None):
        """ Override the base class to use the leftover athena argumnets by
        default
        """
        if args is None:
            args = opts.user_opts
        return super().parse_known_args(args, namespace)

    def format_help(self):
        text = super().format_help()
        return "\n".join([text, self.caller_help])
