# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""ATLAS-specific flake8 plugins

Documentation: http://flake8.pycqa.org/en/latest/plugin-development
"""

from PyUtils.flake8_atlas import utils
import ast
import re

# Inspired by: https://github.com/openstack-dev/hacking/blob/master/hacking/checks/other.py
RE_LOGGING = re.compile(r".*\.(?:error|warn|warning|info|debug)"
                        r"\([^,]*(%)[^,]*[,)]")
@utils.flake8_atlas
def delayed_string_interpolation(logical_line):
    r"""String interpolation should be delayed at logging calls.
    ATL101: log.debug('Example: %s' % 'bad')
    Okay:   log.debug('Example: %s', 'good')
    """
    msg = ("ATL100: use lazy string formatting in logging calls (',' instead of '%')")

    m = RE_LOGGING.match(logical_line)
    if m is not None:
       col = m.start(1)
       yield (col-1, msg)


######################################################################
# ATL9xy: Specialized plugins (disabled by default)
######################################################################

@utils.flake8_atlas
@utils.off_by_default
class OutputLevel(object):
   """Check if an explicit OutputLevel is set
   ATL900: myalg.OutputLevel = DEBUG
   """
   msg = ('ATL900: Do not assign an explicit OutputLevel', 'ATL900')

   def __init__(self, tree):
      self.tree = tree

   def run(self):
      # Walk AST and find assignment to OutputLevel
      for node in ast.walk(self.tree):
         # Find: c.OutputLevel = DEBUG
         if isinstance(node, ast.Assign):
            for t in node.targets:
               if isinstance(t,ast.Attribute) and t.attr=='OutputLevel':
                  yield (node.lineno, node.col_offset) + self.msg

         # Find: setattr(c,'OutputLevel',DEBUG)
         if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id=='setattr':
            a = node.args[1]
            if isinstance(a, ast.Str) and a.s=='OutputLevel':
               yield (node.lineno, node.col_offset) + self.msg


RE_PRINT = re.compile(r"\bprint")
@utils.flake8_atlas
@utils.off_by_default
def print_for_logging(logical_line):
    """Check for occurences of plain 'print'"""

    for match in RE_PRINT.finditer(logical_line):
        yield match.start(0), (
            "ATL901: use 'AthenaCommon.Logging' instead of 'print'")
