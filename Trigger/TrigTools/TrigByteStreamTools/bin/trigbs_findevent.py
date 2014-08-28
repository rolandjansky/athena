#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Find event in FILE(s).

FILE can be a local file or a castor path. On castor, wildcards are supported but
need to be put into single-quotes to avoid shell expansion.
If no event identifier(s) are specified on the command line all events are printed.
In addition one can save the selected events into a new file.
"""

__author__ = "Frank Winklmeier"

import sys
import os
import argparse
import eformat
from PyUtils.castor import nsls

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
                       help='local file or castor path')
   
   parser.add_argument('-g', '--globalid', type=int, action='store', nargs='*',
                       help='Global event ID')

   parser.add_argument('-l', '--lvl1id', type=int, action='store', nargs='*',
                       help='LVL1 ID')

   parser.add_argument('-t', '--time', action=StoreTime, nargs='*',
                       help='Nanosecond time stamp (seconds:nanoseconds)')   

   parser.add_argument('-d', '--debug', type=int, action='store', metavar='RUN',
                       help='Find event in debug streams of RUN')

   parser.add_argument('-s', '--save', metavar='OUTFILE', nargs='?', action='store', const='trigbs_findevent',
                       help='Save selected events in OUTFILE')

   parser.add_argument('--debugPath', action='store',
                       default='/castor/cern.ch/grid/atlas/DAQ/2012',
                       help='Path to debug stream events %(default)s')

   parser.add_argument('-v', '--verbose', action='store_true',
                       help='Be verbose')
   
   args = parser.parse_args()
   
   files = []
   for f in args.file:
      if f.find('castor')!=-1: files += nsls(f)
      else: files += [f]

   if args.debug!=None:
      # Set/reset castor environment for debug stream access
      stage_host = os.environ.get('STAGE_HOST',None)
      stage_svcclass = os.environ.get('STAGE_SVCCLASS',None)
      os.environ['STAGE_HOST'] = 'castoratlas'
      os.environ['STAGE_SVCCLASS'] = 'atlcal'
      debug_dirs = nsls(os.path.join(args.debugPath,'%08d' % args.debug,'debug_*'))
      for d in debug_dirs:
         files += nsls(os.path.join(d,'*.data'))

      if stage_host: os.environ['STAGE_HOST'] = stage_host
      if stage_svcclass: os.environ['STAGE_SVCCLASS'] = stage_svcclass

   ofs = None
   if args.save!=None:
      ofs = eformat.ostream(core_name=args.save)

   for f in files:
      ifs = eformat.istream(f)
      if args.verbose==True: print '==%s' % f
      for e in ifs:
         found = True
         if ofs: ofs.write(e)
         if args.globalid!=None and e.global_id() not in args.globalid:
            found = False
         if args.lvl1id!=None and e.lvl1_id() not in args.lvl1id:
            found = False
         if args.time!=None and (e.bc_time_seconds(),e.bc_time_nanoseconds()) not in args.time:
            found = False
         if found:
            print f,fmtEvent(e,args.time!=None)
      
if __name__ == '__main__':
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)
