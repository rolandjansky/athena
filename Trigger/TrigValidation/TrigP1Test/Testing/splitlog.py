#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# @file:    splitlog.py
# @purpose: Slice file into several pieces using regular expressions
# @author:  Frank Winklmeier

__version__ = "$Revision: 1.1 $"

import sys
import re

def sliceLog(beginRE, endRE, filename):
      
   file = open(filename)
   fSlice = None

   class Slice:
      n = 0
      def new(self):
         Slice.n += 1
         f = open("%s.%d" % (filename,Slice.n), "w")
         print f.name
         return f
   
   for line in file:
      if fSlice:
         if beginRE.match(line):
            fSlice.close()
            fSlice = Slice().new()
         elif endRE and endRE.match(line):
            fSlice.write(line)
            fSlice.close()
            fSlice = None
      else:
         if beginRE.match(line):
            fSlice = Slice().new()

      if fSlice: fSlice.write(line)
   return

     
def main():

   import logging
   logging.basicConfig()
   
   import optparse
   parser = optparse.OptionParser(description="Slice log file into several pieces",
                                  version="%prog: "+ __version__,
                                  usage="%prog -b PATTERN [...] FILE")

   parser.add_option("-b", "--begin", action="store", type="string",
                     help="Regular expression for start of slice")

   parser.add_option("-e", "--end", action="store", type="string",
                     help="Regular expression for end of slice")

   (opts, args) = parser.parse_args()
   if len(args)!=1:
      parser.error("Need to specify one FILE name")

   if not opts.begin:
      parser.error("Need to specify PATTERN where the slice starts")
      
   beginRE = re.compile(opts.begin)
   if opts.end: endRE = re.compile(opts.end)
   else: endRE = None
   
   sliceLog(beginRE,endRE,args[0])


if __name__ == "__main__":
   try:
      sys.exit(main())
   except IOError, e:
      (code, msg) = e
      if (code==32): pass   # ignore broken pipe exception
      else: raise e
   except KeyboardInterrupt:
      sys.exit(1)
      
