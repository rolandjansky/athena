#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""
Check message statistics.
The exceptions to the message thresholds specified on the command line
can be configured in the configuration file (regex):

# Source    Level     Max Count
IOVDbSvc$   INFO        300
MyAlg$      INFO        4
.*MyTool$   WARNING     5
"""

import sys
import os
import string
import re

# Global thresholds from command line
globalThr = {}

def parseConfig(configfile):
   """Return dictionary {Level : [(regex_source, Threshold)] }"""

   thr = {}
   for line in open(configfile):
      line = line.strip()
      if len(line)==0 or line.startswith('#'): continue
      (source,level,nmax) = line.rsplit(None,2)
      if not level in thr: thr[level] = []
      thr[level].append((re.compile(source),int(nmax)))

   return thr

def getMsgStat(logfile):
   """Return a list of tuples (Source,Level,Count)"""
   
   stats = []
   f = open(logfile)   
   for line in f:
      if line.find('Summarizing all message')==-1: continue
      for i in range(3): f.next()   # skip 3 lines
      
      for line in f:
         if line.find('======================')!=-1: return stats
         t = map(string.strip, line.rsplit('|',2))   # split line and remove whitespaces
         t[2] = int(t[2])
         stats.append(t)
         
   return stats

def findThreshold(source, thr):
   for t in thr:
      if t[0].match(source)!=None: return t[1]
   return 0

def checkStats(stats, thr):
   
   aboveThr = []
   ignored = []
   for s in stats:
      # Are we supposed to check this message level?
      if s[1] not in globalThr: continue
      
      if (s[1] in thr):
         maxCount = findThreshold(s[0],thr[s[1]])
         if s[2] > maxCount: aboveThr.append(s)
         else: ignored.append(s)
      else:
         if s[2] > globalThr[s[1]]: aboveThr.append(s)
         else: ignored.append(s)

   return (aboveThr,ignored)

def main():
   import optparse
   parser = optparse.OptionParser(description=__doc__,
                                  usage='%prog [Options] LOGFILE')

   parser.add_option('--info', type='int', action='store', default=0,
                     help='Threshold for INFO messages [%default]')

   parser.add_option('--warning', type='int', action='store', default=None,
                     help='Threshold for WARNING messages [%default]')

   parser.add_option('--error', type='int', action='store', default=None,
                     help='Threshold for ERROR messages [%default]')

   parser.add_option('--fatal', type='int', action='store', default=None,
                     help='Threshold for FATAL messages [%default]')
   
   parser.add_option('-c', '--config', action='store',
                     help='Message threshold configuration file')

   parser.add_option('--showignored', action='store_true', default=False,
                     help='Show ignored message sources')

   (opts, args) = parser.parse_args()
   
   if len(args)==0:
      parser.print_help()
      return 1

   if opts.info!=None:    globalThr['INFO'] = opts.info
   if opts.warning!=None: globalThr['WARNING'] = opts.warning
   if opts.error!=None:   globalThr['ERROR'] = opts.error
   if opts.fatal!=None:   globalThr['FATAL'] = opts.fatal
   
   thresholds = {}
   if opts.config:
      if not os.path.exists(opts.config):
         os.system("get_files -data -symlink %s &> /dev/null" % opts.config)

      try:
         thresholds = parseConfig(opts.config)
      except Exception, e:
         print "Error parsing config file '%s'\n%s" % (opts.config,e)
         return 1
      
   stats = getMsgStat(args[0])
   (aboveThr,ignored) = checkStats(stats, thresholds)
   
   print 'Message sources above threshold:'
   print '--------------------------------'
   if len(aboveThr)==0: print 'NONE'
   for s in aboveThr:
      print '%-70s %10s %10d' % tuple(s)

   if opts.showignored:
      print
      print 'Ignored message sources:'
      print '------------------------'
      for s in ignored:
         print '%-70s %10s %10d' % tuple(s)

   return (2 if len(aboveThr)>0 else 0)
   
if __name__ == '__main__':
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)
