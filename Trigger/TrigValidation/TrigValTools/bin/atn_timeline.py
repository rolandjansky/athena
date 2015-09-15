#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
"""Analyse and create a graph of the ATN test timeline."""

__author__ = "Frank Winklmeier"

import glob
import re
import sys
import os
import operator
from datetime import datetime

import matplotlib
from matplotlib.dates import date2num
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from matplotlib.figure import Figure

class Test:
   def __init__(self):
      self.name = None
      self.t1 = None
      self.t2 = None
      self.suite = None

def getSuites(dir):
   """Read test suites from python dict dumped into NICOS log file"""

   f = open(os.path.join(dir,'nicos_test.loglog'),'r')
   for line in f:
      if line.find('atn_streams_config: dictStreams')!=-1:
         streams = eval(f.next().strip())
         return streams
   return {}

def parseTestLog(log):
   """Extract time stamps from NICOS logs"""
   
   test = Test()
   f = open(log,'r')

   # Test name from log file name
   # Example: Trigger_TrigValidation_TrigP1Test_49__TrigP1Test_TestConfiguration__AllPT_physicsV1_magField_on_off_on__x.loglog
   test.name = re.match('.*?_([0-9]+.*)\.loglog',log).group(1)
   test.alias = test.name.split('__')[2]
   
   for line in f:
      # Old NICOS version
      if line.find('qmtest.start_time')!=-1:
         test.t1 = f.next().strip()
         test.t1 = datetime.strptime(test.t1, '%Y-%m-%dT%H:%M:%SZ')
         continue
      if line.find('qmtest.end_time')!=-1:
         test.t2 = f.next().strip()
         test.t2 = datetime.strptime(test.t2, '%Y-%m-%dT%H:%M:%SZ')
         continue

      # New NICOS version without QMTest
      if line.find('ATN, test start time')!=-1:
         test.t1 = line.split('] ')[1].strip()
         test.t1 = datetime.strptime(test.t1, '%a %b %d %H:%M:%S %Z %Y')
         continue
      if line.find('ATN, test end time')!=-1:
         test.t2 = line.split('] ')[1].strip()
         test.t2 = datetime.strptime(test.t2, '%a %b %d %H:%M:%S %Z %Y')
         continue
         
   return test

def getTimes(dir, package):
   """Loop over all test log files and extract test info"""
   
   tests = {}
   suites = getSuites(dir)
   if len(suites)==0: return None
   
   for log in glob.iglob(os.path.join(dir,"*%s*.loglog" % package)):
      test = parseTestLog(log)
      if test.t1==None or test.t2==None:
         print "Missing start/stop time for %s" % test.name
      else:
         tests[test.name] = test

      for s,v in suites.iteritems():
         if test.name.lower() in v: test.suite = s
         
   return tests
   
def main():
   import optparse
   parser = optparse.OptionParser(description=__doc__,
                                  usage="%prog [Options] TESTDIR")

   parser.add_option("-p", dest="package", type="string", action="store",
                     help="Test package (e.g. TrigP1Test)")

   parser.add_option("-o", dest="output", type="string", action="store", default = 'atn_timeline.png',
                     help="File name for graphics [%default]")

   parser.add_option("--atn", action="store_true",
                     help="ATN mode for nightly tests")

   (opts, args) = parser.parse_args()

   if not opts.package:
      parser.error("Specify test package")
      return 1
   
   if len(args)==0 and not opts.atn:
      parser.print_help()
      return 1

   if opts.atn:
      testdir = '../../NICOS_TestLog%s' % os.environ['NICOS_SUFFIX']
   else:   
      testdir = args[0]
      
   tests = getTimes(testdir, opts.package)
   if tests==None:
      print 'No test suites available in',testdir
      return 0

   # Print tests and stats
   suites = []
   print '%-40s %-20s %-19s  %-19s  %s' % ('Test', 'Suite', 'Start', 'Stop', 'Duration')
   for t in sorted(tests.values(), key=operator.attrgetter('t1')):
      print '%-40s %-20s %s  %s  %s' % (t.alias, t.suite, t.t1, t.t2, t.t2-t.t1)
      if not t.suite in suites: suites += [t.suite]
   suites.sort()

   f = open('atn_timeline_order.txt','w')
   for i,t in enumerate(sorted(tests.values(), key=operator.attrgetter('suite','t1'))):
      f.write('%d %s %s\n' % (i,t.alias,t.suite))
   f.close()
      
   dmin = sorted(tests.values(), key=operator.attrgetter('t1'))[0].t1
   dmax = sorted(tests.values(), key=operator.attrgetter('t1'))[-1].t2
   print
   print 'First test started: %s' % dmin
   print 'Last test finished: %s' % dmax
   print 'Total duration:     %s' % (dmax-dmin)

   # Create graphics
   DPI = 80
   fig = Figure(figsize=(1200/DPI,800/DPI), dpi=DPI)
   canvas = FigureCanvas(fig)
   ax = fig.add_subplot(111)

   # Just to get the x-axis as time
   ax.plot_date([date2num(dmin)-0.001,date2num(dmax)+0.001], [1,1], markersize=0)

   # Plot tests   
   n = 0
   suite = None
   for t in sorted(tests.values(), key=operator.attrgetter('suite','t1')):
      t1 = date2num(t.t1)
      t2 = date2num(t.t2)
      dt = '%s' % (t.t2-t.t1).seconds
      y = suites.index(t.suite)   
      ax.hlines([y], t1, t2, color='red')
      ax.vlines(t1, y-0.1, y+0.1, color='red')
      ax.vlines(t2, y-0.1, y+0.1, color='red')

      # Labels
      if suite!=t.suite:
         ax.text(date2num(dmin), y+0.35, 'Suite: %s' % t.suite, color='blue')
         suite = t.suite
         n = 0
      else:
         n += 1
      
      if n%4==0: offset = -0.3
      elif n%4==1: offset = 0.1
      elif n%4==2: offset = -0.45
      elif n%4==3: offset = 0.25

      ax.text(t1, y+offset, '%d) %s' % (n+1,t.alias))
      ax.text(t1+0.5*(t2-t1), y+0.02, dt, color='red', ha='center')


   ax.text(0, 1, testdir, transform = ax.transAxes)
   ax.xaxis.set_major_formatter(matplotlib.dates.DateFormatter('%H:%M:%S'))
   ax.yaxis.set_ticklabels([])

   canvas.print_figure(opts.output, dpi=DPI)
   
if __name__ == "__main__":
   try:
      sys.exit(main())
   except KeyboardInterrupt:
      sys.exit(1)
