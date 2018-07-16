"""Helper scripts for passing command line arguments into athena

  Command line arguments for job options should follow a single '-' character
  when the script is called.
  Public classes:
    AthArgumentParser

  Usage:
     from AthenaCommon.AthArgumentParser import AthArgumentParser
     athArgsParser = AthArgumentParser()
     athArgsParser.add_argument("--myArg",action="store",dest="myArg",help="My help")
     ...

     athArgs = athArgsParser.parse_args()
     athArgs.myArg #etc etc

"""

import inspect
import argparse
import sys
import os
from subprocess import Popen, PIPE
from __main__ import opts

# TODO - had to remove the check for pre-existing argument groups. Would be good to reinstate

class AthArgumentParser(argparse.ArgumentParser):
  """
    Parse commandline arguments in a job option.
    Extends argparse.ArgumentParser
  """

  def __init__(self, joboption_name=None, stack_depth=1, print_caller_help=True, prog=None, **kwargs):
    """
      Create the parser
        joboption_name   : The name of the calling job option. If not provided will attempt to work it out
        stack_depth      : When working out the job option name, how far deep in the stack to go to find the
                           right file. Only needed if the call to this class goes through other python files
        print_caller_help: Whether or not to print the help script from the calling function too
        prog             : The start of the usage message. Usually fine to be deduced

        The remaining args are from argparse constructor
    """
    caller = sys.argv[0]
    if prog is None:
      if joboption_name is None:
        joboption_name = inspect.getfile(inspect.stack()[stack_depth][0])
      prog = "{0} {1} [{0} options] -".format(os.path.basename(caller), joboption_name)
    if print_caller_help:
      caller_help, _ = Popen([caller, "--help"], stdout=PIPE, stderr=PIPE).communicate()
      if "athena.py" in caller:
          caller_help = caller_help.split("\n",3)[3];
      self.caller_help = "{0} options:\n{1}".format(os.path.basename(caller), caller_help)
    else:
      self.caller_help = ""
    super(AthArgumentParser, self).__init__(prog=prog, **kwargs)
    super(AthArgumentParser, self).add_argument("remaining_scripts", nargs="*", help="Any remaining scripts to be run by athena after this job option. Mainly used to run the 'post' script on the grid.")

  def add_argument(self, *args, **kwargs):
    """Block adding any positional arguments. Athena already could be using these for extra
      job options (especially on the grid)
    """
    if not args or len(args) == 1 and args[0][0] not in self.prefix_chars:
      raise ValueError("Positional arguments are not allowed! Defining them could mess up grid running!")
    return super(AthArgumentParser, self).add_argument(*args, **kwargs)

  def has_argument_group(self, group_name):
    """Check if an argument group has already been defined"""
    return any(group.title == group_name for group in self._action_groups)

  def parse_args(self, args=None, namespace=None):
    """Override base class to use the leftover athena arguments by default"""
    if args is None:
      args = opts.user_opts
    return super(AthArgumentParser, self).parse_args(args, namespace)

  def parse_known_args(self, args=None, namespace=None):
    """Override base class to use the leftover athena arguments by default"""
    if args is None:
      args = opts.user_opts
    return super(AthArgumentParser, self).parse_known_args(args, namespace)

  def format_help(self):
    text = super(AthArgumentParser, self).format_help()
    return text + "\n" + self.caller_help
