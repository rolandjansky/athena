# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""ATLAS-specific flake8 plugins

Documentation: http://flake8.pycqa.org/en/latest/plugin-development
"""

from utils import flake8_atlas, off_by_default
import ast

@flake8_atlas
@off_by_default
class OutputLevel(object):
   """Check if an explicit OutputLevel is set"""

   code = ('ATL100: Do not assign an explicit OutputLevel', 'ATL100')

   def __init__(self, tree):
      self.tree = tree

   def run(self):
      # Walk AST and find assignment to OutputLevel
      for node in ast.walk(self.tree):
         # Find: c.OutputLevel = DEBUG
         if isinstance(node, ast.Assign):
            for t in node.targets:
               if isinstance(t,ast.Attribute) and t.attr=='OutputLevel':
                  yield (node.lineno, node.col_offset) + self.code

         # Find: setattr(c,'OutputLevel',DEBUG)
         if isinstance(node, ast.Call) and isinstance(node.func, ast.Name) and node.func.id=='setattr':
            a = node.args[1]
            if isinstance(a, ast.Str) and a.s=='OutputLevel':
               yield (node.lineno, node.col_offset) + self.code
