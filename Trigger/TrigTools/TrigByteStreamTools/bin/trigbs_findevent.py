#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""Find event in FILE(s).

If no event identifier(s) are specified on the command line all events are printed.
In addition one can save the selected events into a new file.
"""

__author__ = "Frank Winklmeier"

import sys
import argparse
import eformat

def fmtEvent(e,showTime=False):
   if showTime:
      time = ' time=%d:%d' % (e.bc_time_seconds(),e.bc_time_nanoseconds())
   else:
      time = ''
   return '[run=%d lb=%d l1id=%d gid=%d%s]' % (e.run_no(),e.lumi_block(),e.lvl1_id(),e.global_id(),time)

class StoreTime(argparse.Action):
    def __call__(self, parser, namespace, values, option_string=None):
       times = [tuple(map(int,v.split(':',1))) for v in values]
       setattr(namespace, self.dest, times)
        
def main():
   parser = argparse.ArgumentParser(description=__doc__)

   parser.add_argument('-f', '--file', metavar='FILE', nargs='*', default=[],
                       help='file name')
   
   parser.add_argument('-g', '--globalid', type=int, action='store', nargs='*',
                       help='Global event ID')

   parser.add_argument('-l', '--lvl1id', type=int, action='store', nargs='*',
                       help='LVL1 ID')

   parser.add_argument('-t', '--time', action=StoreTime, nargs='*',
                       help='Nanosecond time stamp (seconds:nanoseconds)')   

   parser.add_argument('-s', '--save', metavar='OUTFILE', nargs='?', action='store', const='trigbs_findevent',
                       help='Save selected events in OUTFILE')

   parser.add_argument('-v', '--verbose', action='store_true',
                       help='Be verbose')
   
   args = parser.parse_args()
   
   ofs = None
   if args.save is not None:
      ofs = eformat.ostream(core_name=args.save)

   for f in args.file:
      ifs = eformat.istream(f)
      if args.verbose:
         print('==%s' % f)
      for e in ifs:
         found = True
         if ofs:
            ofs.write(e)
         if args.globalid is not None and e.global_id() not in args.globalid:
            found = False
         if args.lvl1id is not None and e.lvl1_id() not in args.lvl1id:
            found = False
         if args.time is not None and (e.bc_time_seconds(),e.bc_time_nanoseconds()) not in args.time:
            found = False
         if found:
            print('%s %s' % (f,fmtEvent(e,args.time is not None)))
      
if __name__ == '__main__':
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)
