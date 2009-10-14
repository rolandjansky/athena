#!/usr/bin/env python
#
# @file: callgrind.py
# @purpose: Show and compare callgrind profiles
# @author: Frank Winklmeier

__author__ = "Frank Winklmeier"
__version__ = "$Revision: 1.1 $".split()[1]

import sys
import os
import re
import logging
import operator
import string

class Header:
   """
   The header of a callgrind profile
   """
   def __init__(self):
      self.pid = -1;
      self.totalCost = -1
      
   def read(self,filename):
      """Read header of callgrind profile"""      
      f = open(filename)
      for line in f:
         fields = line.split(":",1)
         if len(fields)==2:
            if fields[0]=="pid":
               self.pid = int(fields[1])
            elif fields[0]=="summary":
               self.totalCost = int(fields[1])
               
class Cost:
   """
   Cost entity (function/symbol) that holds costs and number of calls.
   """
   def __init__(self):
      self.selfCost = 0
      self.inclCost = 0
      self.calls = 0
   
   def clone(self):
      c = Cost()
      c.selfCost = self.selfCost
      c.inclCost = self.inclCost
      c.calls = self.calls
      return c
   
   def __str__(self):
      return "%d %d %d" % (self.inclCost,self.selfCost,self.calls)
   
   def __iadd__(self,other):
      self.selfCost += other.selfCost
      self.inclCost += other.inclCost
      self.calls += other.calls
      return self

   def __eq__(self,other):
      return ((type(other)==type(self)) and \
              (self.selfCost==other.selfCost) and \
              (self.inclCost==other.inclCost) and \
              (self.calls==other.calls))

   def __ne__(self,other):
      return not self.__eq__(other)


class Context:
   """
   Context information consisting of function name, object file and (source) file.
   Two contexts are identical if function and object name are the same.
   
   Possible assignments to obj/func/file are:
     func = 'MyFunc.cc() const'        -> assign value directly
     func = '(122) MyFunc.cc() const'  -> assign value and create dictionary entry
     func = '(122)'                    -> lookup value of key 122 in dictionary and assign
   """

   _idNameDict = {}
   _idNameDict["obj"] = {}    # id -> name dictionary for object files
   _idNameDict["func"] = {}   # id -> name dictionary for functions
   _idNameDict["file"] = {}   # id -> name dictionary for files
   
   _dictRE = re.compile("\(([0-9]+)\)(.*)")  # Regexp to extract key/value
   
   def __init__(self):
      self.obj = ""
      self.func = ""
      self.file = ""
      self.cost = Cost()
      self.callees = {}   #: functions called by this context
      self.callers = {}   #: functions calling this context
      
   def create(self):
      """Create copy of this Context but with zero costs"""
      c = Context()
      c.__dict__["obj"] = self.obj
      c.__dict__["func"] = self.func
      c.__dict__["file"] = self.file
      c.__dict__["callees"] = self.callees.copy()
      c.__dict__["callers"] = self.callers.copy()
      c.cost = Cost()
      return c

   def clone(self):
      """Create a clone of this Context (including costs)"""
      c = self.create()
      c.cost = self.cost.clone()
      return c
   
   def _doAdd(self,name,list):
      # Add elements to dictionary 'name'
      if list==None: return
      for l in list:
         # If exists already, just add the costs
         c = self.__dict__[name].get(l)
         if c: c.cost += l.cost
         else: self.__dict__[name][l] = l
      
   def addCallers(self,callers):
      self._doAdd("callers",callers)

   def addCallees(self,callees):
      self._doAdd("callees",callees)
      
   def isCycleOf(self,other):
      """
      Check if self is a cycle of other. Return cycle number or 0 if not a cycle.
      Example: MyFunc'2 is a cycle of MyFunc (cycle = 2)
               (but MyFunc is not a cycle of MyFunc'2)
      """
      sep = "'"  # separator
      cycle = 0
      p1 = self.func.split(sep,1)
      p2 = other.func.split(sep,1)
      if p1[0]==p2[0]:   # name is the same
         if len(p1)==2: cycle = p1[1]
         #if p1[1]==sep: cycle = p1[2]
         try: cycle = int(cycle)
         except: cycle = 0

      return cycle
      
   def __setattr__(self,name,value):
      # Handles the ID -> name dictionary for obj, func and file
      
      if type(value)==type("") and len(value)>0 and value[0]=="(" and name in ["obj","func","file"]:
         m = self._dictRE.match(value)
         if m!=None:
            id = m.group(1)
            value = m.group(2).strip()
            if value=="":
               value = self._idNameDict[name][id]      # get value out of dict
            else:
               self._idNameDict[name][id] = value      # put value in dict
         else:
            print "Cannot set '%s' to '%s'" % (name,value)
            raise AttributeError

      self.__dict__[name] = value         
         
   def __str__(self):
      return "%s in %s (%s)" % (self.func,self.file,self.obj)
   
   def __eq__(self,other):
      # Do not use file for equality since functions may spread over
      # several files due to inlining (same for __hash__)
      return (type(other)==type(self) and \
              self.obj==other.obj and \
              self.func==other.func)

   def __ne__(self,other):
      return not self.__eq__(other)

   def __hash__(self):
      # See also __eq__
      return hash(self.func+self.obj)

      
class Profile:
   """
   One callgrind profile
   """
   def __init__(self,filename):
      self.header = Header()
      self.filename = filename
      self.cost = {}
      self.locations = {}

      self.pos = Context()
      self.callPos = Context()
      self.jumpPos = Context()
      self.spec = ""
      self.log = logging.getLogger("Profile::")
      
      self.read()

   def read(self):

      log = self.log
      # Read header
      self.header.read(self.filename)

      headerRE = re.compile("\w+: .*")
      f = open(self.filename)
      lineno = 0
      for line in f:
         lineno += 1
         line = line.strip()
         if len(line)==0: continue
         if headerRE.match(line): continue

         #log.debug(line)
         try:
            ok = self.parseLine(line)
         except:
            log.fatal("Exception parsing: '%s' (%s:%d)" % (line,self.filename,lineno))
            raise
         
         if not ok:
            log.error("Could not understand: '%s' (%s:%d)" % (line,self.filename,lineno))

      self._cleanup()
      
            
   def parseLine(self,line):

      log = self.log
      fields = line.split("=",1)

      if len(fields)==2:
         self.spec = fields[0]
         #log.debug("Found specifier '%s'" % self.spec)
         if self.spec in ["fl","fi","fe"]: self.pos.file = fields[1]
         elif self.spec=="fn":
            self.pos.func = fields[1]
            if not self.cost.has_key(self.pos):
               #log.debug("Adding new position to dictionary: %s '%s'" % (hash(self.pos),self.pos))
               newpos = self.pos.create()
               self.cost[newpos] = newpos

            if not self.locations.has_key(self.pos):         
               newpos = self.pos.create()
               #log.debug("Adding new lib '%s' to locations" % newpos)
               self.locations[newpos] = newpos
               
         elif self.spec=="ob": self.pos.obj = fields[1]
         
         elif self.spec in ["cfl","cfi"]: self.callPos.file = fields[1]
         elif self.spec=="cfn": self.callPos.func = fields[1]
         elif self.spec=="cob": self.callPos.obj = fields[1]

         elif self.spec in ["jfl","jfi"]: self.jumpPos.file = fields[1]
         elif self.spec=="jfn": self.jumpPos.func = fields[1]
         elif self.spec in ["jcnd","jump"]: pass    # not supported yet
         
         elif self.spec=="calls":    # calls=100 0
            (calls,src) = fields[1].split(" ",1)
            calls = int(calls)
            if not self.cost.has_key(self.callPos):
               #log.debug("Adding new call position to dictionary: %s '%s'" % (hash(self.callPos),self.callPos))
               newpos = self.callPos.create()
               self.cost[newpos] = newpos
               
            #log.debug("Adding %s calls to %s" % (calls,self.callPos.func))
            self.cost[self.callPos].cost.calls += calls

            # Create Context for the called function (callee)
            callee = self.callPos.create()
            callee.cost.calls = calls
            self.cost[self.pos].addCallees([callee])

            # Create Context for the caller
            caller = self.pos.create()
            caller.cost.calls = calls
            self.cost[self.callPos].addCallers([caller])
            
         else: return False

      else:
         fields = line.split(" ")[:2]
         if len(fields)==2:
            # cost line: '0 123'
            # source line positions not supported
                     
            costs = int(fields[1])
            self.cost[self.pos].cost.inclCost += costs
            if self.spec=="calls":     # specifier on previous line
               # Add costs to caller of callee
               # This shows up as "Ir" costs in KCachegrind in the "Callers" tab
               self.cost[self.callPos].callers[self.pos].cost.inclCost += costs
               #            ^^^^^               ^^^^^
               #        I call this         that's me (the caller)
            else:
               self.cost[self.pos].cost.selfCost += costs
               
            self.spec=""
            
            #log.debug("Adding costs %s for '%s' total=%d" % (fields[1],self.pos,self.cost[self.pos].cost.inclCost))
         elif line=="*": return True  # not supported
         else: return False

      return True
            

   def _doMerge(self,merge):

      log = self.log
      func = merge[0].func
      origin = filter(lambda x:x.func==func,self.locations)
      if len(origin)==0:
         log.error("Cannot determine original library for '%s':" % func)
         return merge
      elif len(origin)==1:
         pos = origin[0]
      else:
         log.debug("Several libraries for '%s' (probably ok):" % func)
         for o in origin: log.debug(o)
         return merge

      # Create new context and add up all costs and callees in merge list
      newpos = pos.create()
      for m in merge:
         newpos.cost += m.cost
         newpos.addCallees(m.callees)
         newpos.addCallers(m.callers)

      return [newpos]

               
   def _cleanup(self,removeCycle=True):
      """
      Remove duplicates. These are function symbols with different assigned
      object files. The one that initially got inserted via the 'fn' specifier
      is the correct one. By default also remove any cycles.
      """
      cost = []
      merge = []
      func = ""
      for c in sorted(self.cost.values(),key=operator.attrgetter("func")):
         if c.func!=func:
            if len(merge)==0:
               pass            
            elif len(merge)==1:
               cost += [merge[0]]
               merge = []
            else:               
               cost += self._doMerge(merge)
               merge = []

            func = c.func
            
         merge += [c]

      # Do one final merge for the remaining elements
      cost += self._doMerge(merge)

      # Replace original profiles with the merged ones
      self.cost = {}
      for c in cost:
         self.cost[c] = c

      # Remove cycles
      if removeCycle:
         li = sorted(self.cost.values(),key=operator.attrgetter("func"))
         for i in range(len(li)-1):
            c1 = self.cost[li[i]]
            c2 = self.cost[li[i+1]]
            if c1.isCycleOf(c2):
               try: del self.cost[c2]
               except: pass  # don't bother if it's already deleted
            elif c2.isCycleOf(c1):
               try: del self.cost[c1]
               except: pass
         
   def calcRelativeCosts(self):
      """Overwrites the absolute costs with relative costs"""
      log = self.log

      total = self.header.totalCost
      if total<=0:
         log.error("Cannot calculate relative costs. Total cost = %d",total)
         return

      total /= float(100)   # in percent
      for c in self.cost.values():
         c.cost.inclCost /= total
         c.cost.selfCost /= total
         for l in c.callers.values()+c.callees.values():
            l.cost.inclCost /= total
            l.cost.selfCost /= total
      
   def costByLib(self):
      """Cumulative costs of all functions in library"""

      libcost = Context()
      libs = {}
      first = True
      for c in sorted(self.cost.values(),key=operator.attrgetter("obj")):
         if libcost.obj==c.obj:
            #print("Adding costs of '%s' (%s) to %s" % (c,c.cost,c.obj))
            libcost.cost += c.cost
         else:
            if not first: libs[libcost] = libcost
            libcost = c.clone()
            libcost.func = ""   # obsolete
            libcost.file = ""
            
            first = False
            
      return libs

   
   def show(self, showFileLib = False):
      for c in sorted(self.cost.values(),key=operator.attrgetter("func")):
         print c,c.cost.inclCost,c.cost.selfCost,c.cost.calls
         for f in c.callers.values():
            if showFileLib: where = "%s" % f
            else: where = f.func
            print "%10s %d %d %s" % ("<--",f.cost.inclCost,f.cost.calls,where)
            
         for f in c.callees.values():
            if showFileLib: where = "%s" % f
            else: where = f.func
            print "%10s %d %d %s" % ("-->",f.cost.inclCost,f.cost.calls,where)
         
         print
      print "PID: %d" % self.header.pid
      print "Total costs: %d" % self.header.totalCost


def doDiff(cost1,cost2,equal):
   """
   Return list of tuples that differ between cost1 and cost2
   'equal' is a lambda expression that is used to compare the elements in cost1/2
   """

   def oneSidedDiff(cost1,cost2,diff,reverse=False):
      for c1 in cost1.keys():
         tpl = None
         c2 = cost2.get(c1)
         if c2!=None:
            if not equal(c1.cost,c2.cost):
               if reverse: tpl = (c2,c1)
               else: tpl = (c1,c2)
         else:
            null = c1.create()  # create dummy context with zero costs
            if reverse: tpl = (null,c1)
            else: tpl = (c1,null)
            
         if tpl and (not tpl in diff): diff += [tpl]
      return
   
   diff = []
   oneSidedDiff(cost1,cost2,diff,reverse=False)
   oneSidedDiff(cost2,cost1,diff,reverse=True)
   return diff


def maxChar(name,maxLen):
   """Return name truncated at maxLen characters"""
   
   if maxLen==None or len(name)<=maxLen: return name
   return (name[:maxLen]+"...")


def sign(x):
   if x==0: return 0
   elif x>0: return 1
   else: return -1
   
def profDiff(cost1,cost2,opts):
   """Show difference between profiles"""

   import os.path
   
   # Full diff or calls only?
   if opts.callsOnly:      
      equal = lambda x,y: x.calls==y.calls
   else:
      equal = lambda x,y: x==y

   diff = doDiff(cost1,cost2,equal)

   if opts.maxChar==None:
      fmt = "%-50s"
   else:
      fmt = "%%-%ds" % (opts.maxChar+4)

   if opts.relative:
      fmt += " %10.2f %10.2f %10d %10.2f %10.2f %10d"
   else:
      fmt += " %10d %10d %10d %10d %10d %10d"

   # Replace all spaces with user defined separator
   fmt = string.replace(fmt," ",opts.delim)
   
   callChar = opts.maxChar
   if callChar: callChar -= 6

   # Optionally sort the list of differences
   if opts.sort:
      if opts.sort=="incl": key = operator.attrgetter("inclCost")
      elif opts.sort=="self": key = operator.attrgetter("selfCost")
      else: key = operator.attrgetter("calls")
      cmpFct = lambda x,y: sign((key(y[0].cost) - key(y[1].cost)) -\
                                (key(x[0].cost) - key(x[1].cost)))      
      diff.sort(cmp=cmpFct)

   # Print differences
   for d in diff:
      if opts.libs: name = os.path.basename(d[0].obj)
      else: name = d[0].func
      print fmt % (maxChar(name,opts.maxChar),\
                   d[0].cost.inclCost,d[0].cost.selfCost,d[0].cost.calls,
                   d[1].cost.inclCost,d[1].cost.selfCost,d[1].cost.calls)

      # Print difference in callers/callees
      if opts.callers and not opts.libs:
         callerDiff = doDiff(d[0].callers,d[1].callers,equal)
         calleeDiff = doDiff(d[0].callees,d[1].callees,equal)
         
         if opts.sort:
            key = operator.attrgetter("calls")
            callerDiff.sort(cmp=cmpFct)
            calleeDiff.sort(cmp=cmpFct)
            
         for c in callerDiff[:opts.callers]:
            print fmt % ("  <-- "+maxChar(c[0].func,callChar),\
                         c[0].cost.inclCost, c[0].cost.selfCost, c[0].cost.calls,
                         c[1].cost.inclCost, c[1].cost.selfCost, c[1].cost.calls)

         for c in calleeDiff[:opts.callers]:
            print fmt % ("  --> "+maxChar(c[0].func,callChar),\
                         c[0].cost.inclCost, c[0].cost.selfCost, c[0].cost.calls,
                         c[1].cost.inclCost, c[1].cost.selfCost, c[1].cost.calls)
   
def main():


   logging.basicConfig(level=logging.INFO,
                       format="%(name)s%(funcName)-30s %(levelname)s %(message)s")
   
   import optparse
   parser = optparse.OptionParser(description="Show and compare callgrind profiles",
                                  usage="%prog [Options] profile1 [profile2]",
                                  version="%prog "+ __version__+"\nAuthor: "+__author__)

      
   parser.add_option("-c", "--callsOnly",
                     action = "store_true",
                     help = "only show difference if number of calls differs")

   parser.add_option("-l", "--libs",
                     action = "store_true",
                     help = "show cumulative costs per library")

   parser.add_option("-r", "--relative",
                     action = "store_true",
                     help = "show relative costs in % of total costs")

   parser.add_option("--callers",
                     action = "store", type = "int",
                     help = "maximum number of callers/callees to show [none]")

   parser.add_option("--sort",
                     action = "store", type = "choice", choices = ["self","incl","call"],
                     help = "sort by change in [self,incl,call] costs")

   parser.add_option("-m","--maxChar",
                     action = "store", type = "int",
                     help = "Maximum number of characters printed for symbols [all]")

   parser.add_option("--delim",
                     action = "store", type = "string", default = " ",
                     help = "Field separator to use in printout ['%default']")

   parser.add_option("--showCycles",
                     action = "store_true",
                     help = "do not remove function cycles (not recommended)")
  

   (opts, args) = parser.parse_args()

   if len(args)==0 or len(args)>2:
      parser.print_help()
      return 1
   
   prof1 = Profile(args[0])
   if opts.relative: prof1.calcRelativeCosts()
   if len(args)==1:
      prof1.show();
      return
   
   prof2 = Profile(args[1])
   if opts.relative: prof2.calcRelativeCosts()
   
   if opts.libs:
      lib1 = prof1.costByLib()
      lib2 = prof2.costByLib()
      profDiff(lib1,lib2,opts)
   else:
      profDiff(prof1.cost,prof2.cost,opts)
   

   
if __name__ == "__main__":
   try:
      sys.exit(main())
   except IOError, e:
      (code, msg) = e
      if (code==32): pass   # ignore broken pipe exception
      else: raise e
   except KeyboardInterrupt:
      sys.exit(1)
      
