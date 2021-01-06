#!/usr/bin/env python3
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Created: Oct 2020, Frank Winklmeier
#
"""
Extract Python dependencies between packages and create DOT graph.
Both `import` and `include` dependencies are considered.
"""

import ast
import sys
import os
import argparse
import pygraphviz
from collections import defaultdict

class DependencyFinder(ast.NodeVisitor):
   """Walk an AST collecting import/include statements."""

   def __init__(self):
      self.imports = set()
      self.includes = set()

   def visit_Import(self, node):
      """import XYZ"""
      self.imports.update(alias.name.split('.',1)[0] for alias in node.names)

   def visit_ImportFrom(self, node):
      """from XYZ import ABC"""
      if node.level==0:   # ignore relative imports
         self.imports.add(node.module.split('.',1)[0])

   def visit_Call(self, node):
      """"include(XYZ/ABC.py)"""
      if isinstance(node.func, ast.Name) and node.func.id=='include' and node.args:
         if isinstance(node.args[0], ast.Str):
            self.includes.add(node.args[0].s.split('/',1)[0])


def get_dependencies(filename, print_error=False):
   """Get all the imports/includes in a file."""

   try:
      tree = ast.parse(open(filename,'rb').read(), filename=filename)
   except SyntaxError as e:
      if print_error:
         print(e, file=sys.stderr)
      return DependencyFinder()

   finder = DependencyFinder()
   finder.visit(tree)

   return finder


def walk_tree(path='./', print_error=False, filterFnc=None):
   """Walk the source tree and extract python dependencies, filtered by FilterFnc"""

   pkg = 'UNKNOWN'
   deps = defaultdict(lambda : defaultdict(set))
   for root, dirs, files in os.walk(path):
      if 'CMakeLists.txt' in files:
         pkg = os.path.basename(root)

      if (filterFnc and not filterFnc(pkg)):
         continue

      for f in filter(lambda p : os.path.splitext(p)[1]=='.py', files):
         d = get_dependencies(os.path.join(root,f), print_error)
         deps[pkg]['import'].update(d.imports)
         deps[pkg]['include'].update(d.includes)

   return deps


def make_graph(deps, filterFnc=None):
   """Save the dependencies as dot graph, nodes filtered by filterFnc"""

   graph = pygraphviz.AGraph(name='AthPyGraph', directed=True)
   for a in deps:
      for t in ['import','include']:
         graph.add_edges_from(((a,b) for b in deps[a][t]
                               if a!=b and (filterFnc is None or (filterFnc(a) and filterFnc(b)))),
                               label = t)
   return graph


def main():
   parser = argparse.ArgumentParser(description=__doc__)

   parser.add_argument('path', metavar='DIRECTORY', nargs='?', default='./',
                       help='root of source tree [%(default)s]')

   parser.add_argument('-o', '--output', metavar='FILE', type=str,
                       help='output file for DOT graph')

   parser.add_argument('-p', '--packages', metavar='FILE', type=str,
                       help='path to packages.txt file [from release]')

   parser.add_argument('-a', '--all', action='store_true',
                       help='include non-athena dependencies')

   parser.add_argument('-v', '--verbose', action='store_true',
                       help='print parse errors')

   args = parser.parse_args()

   packages = None
   if not args.all:
      package_file = args.packages or os.path.join(os.environ['AtlasArea'],'InstallArea',
                                                   os.environ['BINARY_TAG'],'packages.txt')

      try:
         with open(package_file) as f:
            packages = set(line.rstrip().split('/')[-1] for line in f if not line.startswith('#'))
      except FileNotFoundError:
         parser.error(f"Cannot read '{package_file}'. Specify via '-p/--packages' or run with '-a/--all'")

   # By default only show athena packages:
   filterFnc = None if args.all else lambda p : p in packages

   # Walk source tree and create DOT graph:
   g = make_graph(walk_tree(args.path, args.verbose, filterFnc), filterFnc)

   if args.output:
      g.write(args.output)
   else:
      print(g)

if __name__ == "__main__":
   sys.exit(main())
