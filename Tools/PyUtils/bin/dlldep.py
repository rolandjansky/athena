#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# @file:    dlldep.py
#
# @purpose: Create a graph of the runtime dependencies of a dynamic library using ldd.
#           The dependencies are printed in DOT (graphviz) language.
#
# @author:  Frank Winklmeier (CERN)
#           based on Dominik Seichter's 'dependencies.sh':
#           http://domseichter.blogspot.com/2008/02/visualize-dependencies-of-binaries-and.html
#

from __future__ import print_function

import sys
from os.path import basename
import subprocess as sp
import re

import PyUtils.Dso as Dso

import six

class Cache:
   """Global cache of already processed files"""
   
   files = {}   # Global cache of already processed libs
   stats = []   # Global statistics
   
   def __init__(self):
      self.myfiles = {}    # Dependencies of the currently processed lib
      self.dotsrc = []     # DOT source code
      
   def add(self, shlib):
      """Add file to local and global cache"""
      self.myfiles[shlib.lib] = shlib
      Cache.files[shlib.lib] = shlib

   def writeDOT(self, file):      
      for d in self.dotsrc: print (file, d, file=file)

   def dot(self, code, style={}):
      """Output a line of dot code"""   
      if len(style)>0:
         code += ' ['
         for k,v in six.iteritems(style):
            code += '%s="%s" ' % (k,v)
         code += ']'
      
      self.dotsrc.append(code)
      return
   
      
class SharedLib:
   """Represent a shared library with name, dependencies and other stats"""
   
   def __init__(self, distance, lib):
      import os.path as osp
      lib = osp.expanduser(osp.expandvars(lib))
      if not osp.exists(lib):
         l = Dso.find_library(lib)
         if l:
            lib = l
      assert osp.exists(lib), "no such path [%s]" % (lib,)
      self.lib = lib              # path of library
      self.distance = distance    # distance from root lib
      self.deplibs = self._getLibs(lib)   # direct dependencies

   def _getLibs(self, lib):
      """Get direct dependencies of shared library"""

      # First check if already in global cache
      cachedlib = Cache.files.get(lib)
      if cachedlib: return cachedlib.deplibs
      
      # Run readelf to find direct dependencies
      # Note: ldd itself does recursions so we cannot use it here
      encargs = {} if six.PY2 else {'encoding' : 'utf-8'}
      p = sp.Popen(["readelf","-d",lib], stdout=sp.PIPE, **encargs)
      output = p.communicate()[0]
      if p.returncode != 0:
         print ("Cannot run 'readelf' on",lib)
         return []

      libs = []
      for l in output.split("\n"):
         if l.find("NEEDED")==-1: continue
         libs += [l.split()[-1].strip("[]")]

      # Run ldd to find full path of libraries
      p = sp.Popen(["ldd",lib], stdout=sp.PIPE, **encargs)
      output = p.communicate()[0]
      if p.returncode != 0:
         print ("Cannot run 'ldd' on",lib)
         return []

      libpaths = []
      for l in output.split("\n"):
         fields = l.strip().split()
         if len(fields)!=4: continue
         path = fields[2]
         if (fields[0] in libs) and len(path)>0:
            libpaths += [path]

      return libpaths

               
class Color:
   """Helper class for colored nodes"""
   
   default = "white"
   scheme = "rdbu8"
   projects = {"DetCommon" : 2,
               "AtlasCore" : 1,
               "AtlasEvent" : 3,
               "AtlasConditions" : 4,
               "AtlasReconstruction" : 5,
               "AtlasSimulation" : 6,
               "AtlasTrigger" : 7,
               "AtlasAnalysis" : 8
               }

   @classmethod
   def get(cls, lib):
      for p,c in six.iteritems(cls.projects):
         if lib.find(p)!=-1: return "/%s/%s" % (cls.scheme, c)
      return cls.default


class Stats:
   """Statistics calculated from Cache object"""
   
   def __init__(self, lib, cache):
      self.lib = lib
      self.depTotal = len(cache.myfiles)-1   # do not count ourselves
      self.depDirect = 0
      
      for lib in cache.myfiles.values():
         if lib.distance==1: self.depDirect += 1
      return

      
def anaLib(lib, opt, cache, select = [], ignore = [], depth = 0):
   """Get dependencies of shared library recursively"""

   def process(path):
      """Should this lib be processed?"""
      for regexp in select:
         if regexp.match(path): return True
      if len(select)>0: return False
      
      for regexp in ignore:
         if regexp.match(path): return False
      return True

   if opt.maxdepth and depth>=opt.maxdepth: return
   
   # Check if we analyzed this lib already
   cachedlib = cache.myfiles.get(lib)
   if cachedlib:
      # Always save minimum distance
      if depth<cachedlib.distance: cachedlib.distance=depth
      return

   shlib = SharedLib(depth, lib)
   cache.add(shlib)
   
   for l in shlib.deplibs:
      if process(l):
         cache.dot('  "%s" -> "%s"' % (basename(lib), basename(l)))
         anaLib(l, opt, cache, select, ignore, depth+1)
         
   return


def processLib(lib, opt, dotFileName = None):
   """Process one library"""

   cache = Cache()
   dot = cache.dot   # shortcut
   
   dot('digraph DependencyTree {')
   dot('  ratio=0.9 nodesep=0.05')         # some reasonable default values
   dot('  "%s" [shape=box]' % basename(lib))

   select = []
   ignore = []   # currently not used
   if opt.filter:
      for f in opt.filter: select += [re.compile(f)]
   else:
      select = [re.compile(".*atlas/software.*")]

   anaLib(lib, opt, cache, select, ignore)

   # Declare style of all nodes
   for l,v in six.iteritems(cache.myfiles):
      style = {}
      # Special style for direct dependencies
      if v.distance==1:
         style["shape"] = "box"
         
      if not opt.nocolor:
         style["style"] = "filled"
         style["fillcolor"] = Color.get(l)
         
      dot('  "%s"' % (basename(l)), style)
      
   dot('}')
   
   # Write output to file
   if dotFileName: outFile = open(dotFileName, "w")
   else: outFile = open(basename(lib)+".dot", "w")
   
   cache.writeDOT(outFile)

   # Calculate statistics
   if opt.stats:
      st = Stats(lib, cache)
      Cache.stats += [st]
      return st

   return None


   
def printStats():
   """Print statistics"""
   import operator
   
   print ("%-50s %7s %7s" % ("Library dependencies","Direct","Total"))
   print ("-"*70)
   for s in sorted(Cache.stats, key=operator.attrgetter("depDirect"), reverse=True):
      print ("%-50s %7d %7d" % (basename(s.lib), s.depDirect, s.depTotal))

   return

      
def main():

   import optparse
   parser = optparse.OptionParser(description="Create runtime dependecy graph for shared library. The output is a graph in DOT language. To visualize it use, e.g. 'dot -O -Tps mygraph.dot'. The rectangular nodes represent direct dependencies. Nodes belonging to the same project have the same color.",
                                  usage="%prog [OPTIONS] LIB [LIB...]")

   parser.add_option("-o", "--output",
                     help="File for DOT source code (default is LIB.dot)")

   parser.add_option("-d", "--maxdepth", type="int",
                     help="Maximum depth of dependency tree [1..]")

   parser.add_option("-f", "--filter", action="append",
                     help="Only analyze libraries matching regular expression (can be specified multiple times) [default: .*atlas/software.*]")
   
   parser.add_option("--nocolor", action="store_true",
                     help="Do not use colors")

   parser.add_option("-s", "--stats", action="store_true",
                     help="Print statistics")

   (opt, args) = parser.parse_args()
   if len(args)==0:
      parser.error("Invalid number of arguments specified")
      

   if len(args)>1 and opt.output:
      print ("Multiple libraries specified. Ignoring output file name.")
      opt.output = None
      
   for lib in args:
      processLib(lib, opt, opt.output)

   if opt.stats:
      printStats()
      
   return 0


if __name__ == "__main__":
   try:
      sys.exit(main()) 
   except KeyboardInterrupt:
      sys.exit(1)
      
