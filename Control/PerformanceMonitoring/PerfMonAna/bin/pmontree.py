#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__author__  = "Frank Winklmeier"
__version__ = "$Revision: 270227 $"
__doc__     = "Script to create dependency tree of perfmon stats"

import sys
import operator
import re

import PerfMonComps.PerfMonSerializer as pmon_ser

# Global for command line options
opt = None

class Resource(object):
   def __init__(self, name):
      self.name = name
      self.step = ''
      self.dvmem = 0
      self.dmalloc = 0
      self.nmalloc = 0
      self._vmem = [0,0]     # kB
      self._malloc = [0,0]   # kB 
      self._nmalloc = [0,0]  # number

   def set(self, line, i):
      """Set members via match object or stream line and index i"""

      fields = line.split()
      i = int(fields[1])
      step = fields[0][1:-1]    # slice/comp
      self.step = step.split('/')[0]
      if self.step=='dso':
         self._vmem[i] = float(fields[2])
      else:
         self._vmem[i] = float(fields[6])#*1024
         self._malloc[i] = float(fields[8])
         self._nmalloc[i] = float(fields[9])
         return
      
   def wrapup(self):      
      self.dvmem = self._vmem[1]-self._vmem[0]
      self.dmalloc = self._malloc[1]-self._malloc[0]
      self.nmalloc = self._nmalloc[1]-self._nmalloc[0]


class ResUser(Resource):
   """Resource user with dependents
   """
   def __init__(self, name):
      super(ResUser, self).__init__(name)
      self.dep = []         # Dependent users (children)
      self.dvmem_self = None
      
   def _show(self, level=0, showFct=None):
      if showFct and not showFct(self):
         s = ''
      else:
         indent = ('  '*level*(not opt.flat))
         s = '\n' + indent + self._node()
        
      for d in self.dep: 
          s += d._show(level+1, showFct)
      return s

   def calcSelf(self, children=None):
      self.dvmem_self = self.dvmem

   def show(self, showFct=None):
      print( self._show(0, showFct), )

   def _node(self):
      return self.name

   def purge(self, purgeFct):
      """Remove all objects passing purgeFct()
      """
      # Mark dependents for deletion
      for d in self.dep:
         if purgeFct(d): 
             d.name = None
      
      # Recursively call children
      for d in self.dep: 
          d.purge(purgeFct)

      # Remove from list
      self.dep = [ d for d in self.dep if d.name is not None ]
                  
class Comp(ResUser):
   """Component (Algorithm, Tool, Service, etc.)
   """
   def __init__(self, name):
      super(Comp, self).__init__(name)
      self.symbol = '++'

   def calcSelf(self, children = None):
      """Calculate self dvmem (subtract dvmem of all direct children)
         Additional classes to be considered children can be supplied as list.
      """
      self.dvmem_self = self.dvmem
      for d in self.dep:
         if isinstance(d, Comp) or (children and d.__class__ in children):
            self.dvmem_self -= d.dvmem
            
         d.calcSelf(children)
   
   def _node(self):
      # FIXME !!
      dvmem_self = self.dvmem_self
      if self.dvmem_self is None:
         dvmem_self = 0.
      #
      return '|%s %s [%.0f kB, %.0f kB]' % (
         self.symbol,self.name,self.dvmem,dvmem_self
         )


class Callback(Comp):
   """Callback
   """
   def __init__(self, name):
      super(Callback, self).__init__(name)
      self.symbol = '=='

   def _node(self):
      return '|%s %s [%.0f kB, %.0f kB]' % (self.symbol,self.name,self.dvmem,self.dvmem_self)

   
class SharedLib(ResUser):
   """Shared library
   """
   def __init__(self, name):
      super(SharedLib, self).__init__(name)
      self.symbol = '--'
      
   def _node(self):
      return '|%s %s [%.0f kB]' % (self.symbol,self.name,self.dvmem)
   

def sliceCompIdx(line):
   fields = line.split()
   # should be of the form "/step/comp-name"
   step = fields[0][1:-1]
   step = step.split('/')[0]
   comp = fields[0][len(step)+2:]
   idx = int(fields[1])
   return step,comp,idx
   
def getResUser(f, resTree, steps=['ini'], current=None):
   """Get resource user tree"""
   
   # Remember my parent
   parent = current
   
   for line in f:
      if line.startswith(('#','/io/')): 
          continue

      step,name,idx = sliceCompIdx(line)
      if step not in steps: 
          continue

      # startAud
      if idx==0:
         if step in ['cbk']:
            comp = Callback(name)
         elif step=='dso':
            comp = SharedLib(name)
         else:
            comp = Comp(name)
            
         comp.set(line, idx)
         if current:
            current.dep.append(comp)
            getResUser(f, resTree, steps, comp)
         else:
            current = comp
            resTree.append(comp)
         continue

      
      # stopAud
      if idx==1:
         if name != current.name:
            raise RuntimeError( "stop for %s within scope of %s" % (name, current.name) )
         else:
            current.set(line, idx)
            current.wrapup()
            # Remove absolute address from callback
            if step=='cbk':
               offset = current.name.split(']+')[1]
               i = current.name.find('[')
               if i>0: 
                   current.name = '%s{+%s}' % (current.name[:i],offset)

            if parent is None:
               current=None
               continue
            else: 
                return


def readEvents(f):
   """Read components for evt slice
   """
   reEvent = re.compile(r'AthenaEventLoopMgr\s*INFO\s*===>>>  done processing event.*')
   evt = None
   comps = []   # [ {name : [Comp]} ]
   for line in f:
      m = reEvent.match(line)
      if m:
         if evt: 
             evt+=1
         else: 
             evt=0
         comps.append({})
      if evt is None: 
          continue
     
      '''
      ## FIX ME : This bit needs to be checked
      ##          It's not obvious what reAud is inteded to be...
      m = reAud.match(line)
      if m and m.group('slice')!='evt': 
          continue
      
      if m and m.group('action')=='start':
         comp = Comp(m.group('comp'))
         comp.set(m, 0)
         if comp.name not in comps[evt]: 
             comps[evt][comp.name] = []
         comps[evt][comp.name].append(comp)

      if m and m.group('action')=='stop':
         comp = comps[evt][comp.name][-1]
         comp.set(m, 1)
         comp.wrapup()         
      '''
   
   return comps


def resAvg(res):
   """Calculate average of list of resources"""
   if len(res)==0: 
       return None
   a = Comp(res[0].name)
   a.step = res[0].step
   for r in res:
      a.dvmem += r.dvmem
      a.dmalloc += r.dmalloc
      a.nmalloc += r.nmalloc

   a.dvmem /= float(len(res))
   a.dmalloc /= float(len(res))
   a.nmalloc /= float(len(res))
   return a


def calcEventAvg(comps, sliceObj=slice(None)):

   tmp = {}  # { comp: [] }
   for evt in comps[sliceObj]:
      for comp in evt.keys():
         if comp not in tmp: 
             tmp[comp] = []
         tmp[comp] += evt[comp]

   avg = []
   for comp in tmp.keys():
      avg.append(resAvg(tmp[comp]))

   return avg


      
def getCompList(resTree, resList):
   """
   Create flat list of components from resource tree.
   Call with resList = [].
   """
   for r in resTree:
      if isinstance(r, ResUser): 
          resList.append(r)
      for d in r.dep: 
          getCompList([d], resList)   

   return resList[:]

   
def diff(table, opt, attrgetter=operator.attrgetter('dvmem')):
      
   tmp = {}   # {name : [mem1,mem2]}
   for i,t in enumerate(table):
      for comp in t:
         label = comp.symbol + ' ' + comp.name
         if label not in tmp: 
             tmp[label] = [0]*len(table)
         tmp[label][i] = attrgetter(comp)

   # Convert to list
   if opt.min is not None:
      limit = opt.min + 0.00001
      cmpTable = [ [k]+v for k,v in tmp.iteritems() if abs(v[1]-v[0])>limit ]
   else:
      cmpTable = [ [k]+v for k,v in tmp.iteritems() ]
      
   if opt.diff and len(table)==2:
      cmpTable.sort( lambda x,y : (int(x[2]-x[1])-int(y[2]-y[1])), reverse=True )
      for c in cmpTable:
         print( "%-60s %10.0f %10.0f %10.0f" % (c[0],c[1],c[2],c[2]-c[1]) )
   else:
      cmpTable.sort( lambda x,y : int(x[1]-y[1]), reverse=True)
      for c in cmpTable:
         print( "%-60s" % c[0], )
         print( "%10.0f "*(len(c)-1) % tuple(c[1:]) )

   return


def printTable(compList, opt):
   for c in sorted(compList,key=operator.attrgetter('dvmem'), reverse=True):
      if c.nmalloc!=0:
         avgmalloc = c.dmalloc*1024/c.nmalloc
      else:
         avgmalloc = 0
      print( "%-60s %10.0f %10.0f %10.0f %10.0f" %\
            (c.name,c.dvmem,c.dmalloc,c.nmalloc,avgmalloc) )
   
def main():
   import argparse
   parser = argparse.ArgumentParser(
      description="Create callgraph profile from log file",
      usage="%prog [Options] LOGFILE [LOGFILE]")

   arg = parser.add_argument
   arg('files',
       nargs='*',
       help='list of files to process')
   
   arg("-m", "--min", action="store", type="float",
       help="only show entries with VMem>MIN in kB")

   arg("-d", "--diff", action="store_true",
       help="show difference (only for two files)")

   arg("-s", "--self", action="store_true",
       help="use self instead of inclusive VMem for sorting/filtering")

   arg("-l", "--libself", action="store_true",
       help="include libraries into self-VMem")

   arg("-c", "--slice", action="store", default="ini",
       help="slice to analyze [ini]")

   arg("-f", "--flat", action="store_true",
       help="do not indent tree")

   global opt
   opt= parser.parse_args()
   
   if len(opt.files)==0:
      parser.print_help()
      return 1

   if opt.diff and len(opt.files)!=2:
      print( "Can only calculate difference if two files are given" )
      return 1

   slices = [opt.slice]
   if opt.slice=='ini':
      slices += ['cbk', 'dso']
   
   # evt slice
   if opt.slice=='evt':
      table = []
      for f in opt.files:
         table.append(calcEventAvg(readEvents(open(f,'r')))[:])

      #diff(table, opt, operator.attrgetter('nmalloc'))
      printTable(table[0], opt)
      return 0
      
   # Read files
   resTreeList = []
   for f in opt.files:
      z = []
      fstream = pmon_ser.extract_pmon_files(f)['data']
      getResUser(fstream, z, slices)
      del fstream
      resTreeList.append(z[:])

      # Calculate self-VMem
      if not opt.libself: 
          children = [SharedLib]
      else: 
          children = None
      for r in resTreeList[-1]: 
          r.calcSelf(children)

   # Diff
   if len(opt.files)>1:

      print( '#'*80 )
      for i,f in enumerate(opt.files):
         print( "# [%d] %s" % (i+1,f) )
      if opt.diff:
         print( "# [3] difference [2]-[1]" )
      print( '#'*80 )
            
      table = [ getCompList(t,[]) for t in resTreeList ]
      if opt.self:
         diff(table, opt, operator.attrgetter('dvmem_self'))
      else:
         diff(table, opt)      
      return 0

   # Only one file
   resTree = resTreeList[0]

   if opt.min is not None:
      # Use VMem or self-VMem for filtering
      def vmem( c ): 
          result = c.dvmem_self if (opt.self is True and hasattr(c,'dvmem_self')) else c.dvmem
          return result
      for r in resTree: 
          r.show(lambda c: vmem(c)>opt.min)
   else:
      for r in resTree: 
          r.show()

   return 0


if __name__ == "__main__":
   try:
      sys.exit(main())
   except IOError as e:
      (code, msg) = e
      if (code==32): 
          pass   # ignore broken pipe exception
      else: 
          raise e
   except KeyboardInterrupt:
      sys.exit(1)
