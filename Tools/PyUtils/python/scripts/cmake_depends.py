#!/usr/bin/env python
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Created: June 2020, Frank Winklmeier
#
"""
Print target/package dependencies of ATLAS releases. For a given target/package
name, the dependencies are printed as a plain list or DOT graph. The recursion
depth is configurable.
"""

import sys
import os
import re
from collections import deque
import PyUtils.acmdlib as acmdlib
import argparse
import pygraphviz

# Style for python dependencies:
py_style = 'dashed'

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
   Parameters are the same as in `traverse`. Return list of edge tuples.
   """
   edges = set()
   for root in sources:
      for a,b in traverse(graph, root, reverse=reverse, maxdepth=maxdepth, nodegetter=nodegetter):
         if a and b and a!=b:
            if reverse: edges.add((b,a))
            else: edges.add((a,b))

   return edges


def add_legend(graph):
   """Add legend to graph"""
   graph.add_subgraph(name='clusterLegend', label='Legend')
   l = graph.subgraphs()[-1]
   for n in 'abcd':
      l.add_node(n, shape='point', style='invis')
   l.add_edge('a','b', label='C++', constraint=False)
   l.add_edge('c','d', label='Python', style=py_style, constraint=False)


def copy_graph(source, dest):
   """Copy graph nodes and edges from source to dest including attributes"""
   for e in source.edges_iter():
      dest.add_edge(e, **e.attr)
   for n in source.nodes_iter():
      dest.add_node(n, **n.attr)


class AthGraph:
   """Class to hold dependency information for release"""

   def __init__(self, dotfile):
      """Read dotfile and and optionally transform package names to full paths"""

      # Read dot file:
      self.graph = pygraphviz.AGraph(dotfile)

      # Build dictionary for node types:
      legend = self.graph.get_subgraph('clusterLegend')
      self.types = { n.attr['label'] : n.attr['shape'] for n in legend.nodes_iter() }

      # Build dictionary for node names:
      self.node = { n.attr['label'] : n.get_name() for n in self.graph.nodes_iter() }

      def decorate_package(n0, n1=None):
         """Assign package name to n0 -> n1 if n0 is a package target"""
         p = n0.attr['label']
         # Decorate target with package name:
         if p.startswith('Package_'):
            n0.attr['package'] = lrstrip(p, 'Package_', '_tests')
            if n1 is not None:
               n1.attr['package'] = n0.attr['package']

      # Extract package dependencies:
      for e in self.graph.edges_iter():
         decorate_package(e[0], e[1])

      # Another pass on nodes to cover packages without dependendencies:
      for n in self.graph.nodes_iter():
         decorate_package(n)

      # Assign "package" names to externals if possible:
      external_nodes = filter(lambda n : 'package' not in n.attr.keys(),
                              self.graph.nodes_iter())
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


def create_dep_graph(target, deps, pydeps, args):
   """Create and return dependency graph.

   @param target  name of target
   @param deps    AthGraph cmake dependencies
   @param pydeps  python dependencies
   @param args    command line arguments
   """
   # Helper for graph traversal below:
   def getnode(node):
      if not args.all and deps.ignore_target(node): return None
      if args.externals or not node.attr['external']:
         a = 'label' if args.target else 'package'
         return node.attr[a]

   target = target.split('/')[-1]  # in case of full package path

   # In package mode we have one extra level due to the Package_ target:
   depth = args.recursive
   if not args.target and not args.clients and args.recursive is not None:
      depth += 1

   # With regex, find all matching targets:
   if args.regex:
      r = re.compile(target)
      targets = [getnode(n) for n in deps.graph.nodes_iter() if r.match(n.attr['label'])]
      if args.py:
         targets += [n for n in pydeps.nodes_iter() if r.match(n)]
      targets = sorted(set(filter(lambda t : t is not None, targets)))
   else:
      targets = [target]

   # Find the nodes from which graph traversal starts:
   sources = []
   for l in targets:
      if not args.target:
         l = 'Package_'+l
      try:
         if deps.get_node(l).attr['external'] and not args.externals:
            raise RuntimeError(f"{l} is an external target. Run with -e/--externals.")

         # To find clients of a package means finding clients of the targets
         # within that package. First find all targets within the package:
         if args.clients and not args.target:
            sources.extend([b for a,b in traverse(deps.graph, deps.get_node(l), maxdepth=1)])
         else:
            sources.extend([deps.get_node(l)])
      except KeyError:
         raise RuntimeError(f"Target with name {l} does not exist.")

   # Extract the dependency subgraph:
   g = subgraph(deps.graph, sources, reverse=args.clients,
                maxdepth=depth, nodegetter=getnode)

   graph = pygraphviz.AGraph(name=target, directed=True, strict=False)
   graph.add_edges_from(g)

   # Add python dependencies:
   if args.py:
      # Here the nodes are the actual package names:
      pysources = [pydeps.get_node(t) for t in targets if pydeps.has_node(t)]
      g = subgraph(pydeps, pysources, reverse=args.clients,
                   maxdepth=args.recursive, nodegetter=lambda n : n.name)

      graph.add_edges_from(g, style=py_style)

      # Change style of nodes that have only Python dependencies:
      for n in graph.nodes_iter():
         if all(e.attr['style']==py_style for e in graph.edges_iter(n)):
            n.attr['style'] = py_style

   return graph


def print_dep_graph(graph, args, package_paths={}):
   """Output final graph"""

   # txt output
   if args.batch or not args.dot:
      f = open(graph.name+'.txt', 'w') if args.batch else sys.stdout
      nodes = [e[0] for e in graph.in_edges_iter()] if args.clients \
         else [e[1] for e in graph.out_edges_iter()]

      output = []
      for p in set(nodes):
         suffix = ':py' if p.attr['style']==py_style else ''
         output.append('%s%s' % (package_paths.get(p,p), suffix))
      print('\n'.join(sorted(output)), file=f)

   # dot output
   if args.batch or args.dot:
      f = open(graph.name+'.dot', 'w') if args.batch else sys.stdout
      if args.legend:
         add_legend(graph)
      print(graph, file=f)


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
                  help='show full package names (only for txt output)')

@acmdlib.argument('-r', '--recursive', nargs='?', metavar='DEPTH',
                  type=int, default=1, const=None,
                  help='recursively resolve dependencies up to DEPTH (default: unlimited)')

@acmdlib.argument('--py', action='store_true',
                  help=f'add Python dependencies (marked with ":py" in printout, {py_style} in graph)')

@acmdlib.argument('--regex', action='store_true',
                  help='treat NAME as regular expression')

@acmdlib.argument('--all', action='store_true',
                  help='do not apply any target filter (e.g. custom targets)')

@acmdlib.argument('-d', '--dot', action='store_true',
                  help='print DOT graph')

@acmdlib.argument('--legend', action='store_true',
                  help='add legend to graph')

@acmdlib.argument('--batch', nargs='?', metavar='N', type=int, const=1,
                  help='Batch mode using N jobs (default: 1). Create dot and txt dependencies '
                  'for all NAMEs and store them in separate files.')


# Debugging/expert options:
@acmdlib.argument('--cmakedot', help=argparse.SUPPRESS)
@acmdlib.argument('--pydot', help=argparse.SUPPRESS)


def run(args):
   """Inspect cmake build dependencies"""

   # Find packages.dot:
   if not args.cmakedot:
      try:
         args.cmakedot = os.path.join(os.environ['AtlasArea'],'InstallArea',
                                      os.environ['BINARY_TAG'],'packages.dot')
      except KeyError:
         main.parser.error("Cannot find 'packages.dot'. Setup a release or use --cmakedot.")

   # Find packages.py.dot:
   pydeps = None
   if args.py:
      if args.target:
         main.parser.error("Python dependencies not possible in target mode.")

      args.pydot = args.pydot or args.cmakedot.replace('.dot','.py.dot')
      try:
         pydeps = pygraphviz.AGraph(args.pydot)
      except Exception:
         main.parser.error(f"Cannot read '{args.pydot}'. Setup a release or use --pydot.")

   # Read packages.txt if needed:
   package_paths = {}
   if args.long:
      try:
         package_paths = read_package_list(os.path.join(os.environ['AtlasArea'],'InstallArea',
                                                        os.environ['BINARY_TAG'],'packages.txt'))
      except Exception:
         main.parser.error("Cannot read 'packages.txt'. Setup a release or run without -l/--long.")

   # Read dependencies:
   deps = AthGraph(args.cmakedot)

   # Create combined graph for all given targets:
   if not args.batch:
      subgraphs = [create_dep_graph(target, deps, pydeps, args) for target in args.names]
      if len(subgraphs)>1:
         graph = pygraphviz.AGraph(name='AthGraph', directed=True, strict=False)
         for g in subgraphs:
            graph.add_subgraph(name=g.get_name())
            copy_graph(g, graph.subgraphs()[-1])
      else:
         graph = subgraphs[0]

      print_dep_graph(graph, args, package_paths)

   # Batch mode: create separte graph for each target:
   else:
      import multiprocessing
      global doit   # required for use in multiprocessing
      def doit(target):
         graph = create_dep_graph(target, deps, pydeps, args)
         print_dep_graph(graph, args, package_paths)

      pool = multiprocessing.Pool(args.batch)
      pool.map(doit, args.names)


def main(args):
   try:
      run(args)
   except RuntimeError as e:
      print(e)
      return 1
