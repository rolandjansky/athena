#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Created: June 2020, Frank Winklmeier
#
"""
Print target/package dependencies of ATLAS releases. For a given target/package
name, the dependencies are printed as a plain list or DOT graph. The recursion
depth is configurable.
"""

import os
import re
from collections import deque
import PyUtils.acmdlib as acmdlib
import argparse
import pygraphviz


def read_package_list(package_file):
   """Read packages.txt as a source for the full package path"""

   with open(package_file) as f:
      packages = [line.rstrip() for line in f if not line.startswith('#')]

   return dict([(p.split('/')[-1],p) for p in packages])


def externals_name(lib):
   """Return a short name for an external library"""
   if '/LCG_' in lib:
      dirs = lib.split('/')
      lcg = next(d for d in dirs if d.startswith('LCG_'))
      return '%s::%s' % (dirs[dirs.index(lcg)+1], dirs[-1])
   elif lib.startswith('Gaudi'):
      return 'Gaudi::%s' % lib
   else:
      return os.path.basename(lib)


def lrstrip(s, prefix, postfix):
   """Strip `prefix` and `postfix` from string `s`"""
   if s.startswith(prefix): s = s[len(prefix):]
   if s.endswith(postfix):  s = s[:-len(postfix)]
   return s


def traverse(graph, root, reverse=False, maxdepth=None, nodegetter=lambda n:n):
   """Depth-limited BFS edge traversal of graph starting at root.

   @param graph       graph
   @param root        start node for traversal
   @param reverse     traverse graph in reverse
   @param maxdepth    maximum traversal depth (1 = only direct neighbors)
   @param nodegetter  functor returning node names
   @return            edge tuple (parent,node)

   Inspired by https://github.com/networkx/networkx/tree/master/networkx/algorithms/traversal
   """
   visited_nodes = set()
   visited_edges = set()
   queue = deque([(root,root,0)])
   neighbors = graph.iterpred if reverse else graph.itersucc
   while queue:
      parent,node,level = queue.popleft()

      if node not in visited_nodes:
         visited_nodes.add(node)

         # Add edges to neighbors into queue:
         if maxdepth is None or level < maxdepth:
            queue.extend((node,n,level+1) for n in neighbors(node))
         # For the last level only edges to already visited nodes:
         elif level==maxdepth:
            queue.extend((node,n,level+1) for n in neighbors(node) if n in visited_nodes)

      if (parent,node) not in visited_edges:
         visited_edges.add((parent,node))
         yield nodegetter(parent), nodegetter(node)


def subgraph(graph, sources, reverse=False, maxdepth=None, nodegetter=lambda n : n.attr.get('label')):
   """Extract subgraph created by traversing from one or more sources.
   Parameters are the same as in `traverse`.
   """
   g = pygraphviz.AGraph(directed=True)
   for root in sources:
      for a,b in traverse(graph, root, reverse=reverse, maxdepth=maxdepth, nodegetter=nodegetter):
         if a and b and a!=b:
            if reverse: g.add_edge(b,a)
            else: g.add_edge(a,b)

   return g


class AthGraph:
   """Class to hold dependency information for release"""

   def __init__(self, dotfile, package_paths={}):
      """Read dotfile and and optionally transform package names to full paths"""

      # Read dot file:
      self.graph = pygraphviz.AGraph(dotfile)

      # Build dictionary for node types:
      legend = self.graph.get_subgraph('clusterLegend')
      self.types = { n.attr['label'] : n.attr['shape'] for n in legend.iternodes() }

      # Build dictionary for node names:
      self.node = { n.attr['label'] : n.get_name() for n in self.graph.iternodes() }

      # Extract package dependencies:
      for e in self.graph.iteredges():
         p = e[0].attr['label']
         # Decorate target with package name:
         if p.startswith('Package_'):
            pkg = lrstrip(p, 'Package_', '_tests')
            e[0].attr['package'] = e[1].attr['package'] = package_paths.get(pkg,pkg)

      # Assign "package" names to externals if possible:
      external_nodes = filter(lambda n : 'package' not in n.attr.keys(),
                              self.graph.iternodes())
      for n in external_nodes:
         name = externals_name(n.attr['label'])
         n.attr['package'] = name.split('::')[0]
         n.attr['label'] = name
         n.attr['external'] = 'yes'

   def get_node(self, label):
      """Return graph node for label/target"""
      return self.graph.get_node(self.node[label])

   def ignore_target(self, node):
      """Check if target should be ignored"""
      label = node.attr['label']
      return True if (label.startswith('__') or   # internal targets
                      label.startswith('-') or    # compiler flags (e.g. -pthread)
                      node.attr['shape']==self.types['Custom Target']) else False

#
# Main function and command line arguments
#
@acmdlib.command(name='cmake.depends',
                 description=__doc__)

@acmdlib.argument('names', nargs='+', metavar='NAME',
                  help='package/target name or regular expression')

@acmdlib.argument('-t', '--target', action='store_true',
                  help='treat NAME as target instead of package name')

@acmdlib.argument('-c', '--clients', action='store_true',
                  help='show clients (instead of dependencies)')

@acmdlib.argument('-e', '--externals', action='store_true',
                  help='include external dependencies')

@acmdlib.argument('-l', '--long', action='store_true',
                  help='show full package names')

@acmdlib.argument('-r', '--recursive', nargs='?', metavar='DEPTH',
                  type=int, default=1, const=None,
                  help='recursively resolve dependencies up to DEPTH (default: unlimited)')

@acmdlib.argument('--regex', action='store_true',
                  help='treat NAME as regular expression')

@acmdlib.argument('--all', action='store_true',
                  help='do not apply any target filter (e.g. custom targets)')

@acmdlib.argument('-d', '--dot', action='store_true',
                  help='print DOT graph')

# Debugging/expert options:
@acmdlib.argument('--cmakedot', help=argparse.SUPPRESS)

def main(args):
   """Inspect cmake build dependencies"""

   # Find packages.dot:
   if not args.cmakedot:
      try:
         args.cmakedot = os.path.join(os.environ['AtlasArea'],'InstallArea',
                                      os.environ['BINARY_TAG'],'packages.dot')
      except KeyError:
         main.parser.error("Cannot find 'packages.dot'. Setup a release or use --cmakedot.")

   # Read packages.txt if needed:
   package_paths = {}
   if args.long:
      try:
         package_paths = read_package_list(os.path.join(os.environ['AtlasArea'],'InstallArea',
                                                        os.environ['BINARY_TAG'],'packages.txt'))
      except Exception:
         main.parser.error("Cannot read 'packages.txt'. Setup a release or run without -l/--long.")

   # In package mode we have one extra level due to the Package_ target:
   if not args.target and not args.clients and args.recursive is not None:
      args.recursive += 1

   # Read dependencies:
   d = AthGraph(args.cmakedot, package_paths)

   # Helper for graph traversal below:
   def getnode(node):
      if not args.all and d.ignore_target(node): return None
      if args.externals or not node.attr['external']:
         a = 'label' if args.target else 'package'
         return node.attr[a]

   graph = pygraphviz.AGraph(name='AthGraph', directed=True)
   for p in args.names:
      target = p.split('/')[-1]  # in case of full package path

      # With regex, find all matching targets:
      if args.regex:
         r = re.compile(target)
         targets = [getnode(n) for n in d.graph.iternodes() if r.match(n.attr['label'])]
         targets = list(filter(lambda t : t is not None, targets))
      else:
         targets = [target]

      # Find the nodes from which graph traversal starts:
      sources = []
      for l in targets:
         if not args.target:
            l = 'Package_'+l
         try:
            if d.get_node(l).attr['external'] and not args.externals:
               print(f"{l} is an external target. Run with -e/--externals.")
               return 1

            # To find clients of a package means finding clients of the targets
            # within that package. First find all targets within the package:
            if args.clients and not args.target:
               sources.extend([b for a,b in traverse(d.graph, d.get_node(l), maxdepth=1)])
            else:
               sources.extend([d.get_node(l)])
         except KeyError:
            print(f"Target with name {l} does not exist.")
            return 1

      # Extract the dependency subgraph:
      g = subgraph(d.graph, sources, reverse=args.clients,
                   maxdepth=args.recursive, nodegetter=getnode)

      graph.add_subgraph(name=target)
      graph.get_subgraph(target).add_edges_from(g.edges())

   # Print result:
   if args.dot:
      print(graph)
   else:
      nodes = [e[0] for e in graph.in_edges_iter()] if args.clients \
         else [e[1] for e in graph.out_edges_iter()]
      for p in sorted(set(nodes)):
         print(p)
