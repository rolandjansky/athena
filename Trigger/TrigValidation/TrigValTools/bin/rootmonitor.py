#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @file:    rootcomp.py
# @purpose: Script to compare the histograms in two root files
# @author:  Frank Winklmeier, Will Buttinger
#
# $Id: rootmonitor.py 779896 2016-10-23 11:05:09Z will $

"""
Look through given file, if find any 1 bin histograms, try to append to graph from previous atn nights

Return value: 0 unless more than X% difference to average of past 7 nightlies
"""              

__author__  = "Will Buttinger"
__version__ = "$Revision: 779896 $"
__doc__     = "Script monitor single values througout nightlies running"

import sys
import os




def lsroot(dir):
   """Return list of all keys in 'dir' (recursively)"""

   import ROOT
   
   def dols(dir, keys):
      """Do the recursive traversal"""
      dirList = dir.GetListOfKeys()
      for k in dirList:
         kname = k.GetName()
         if k.GetClassName()=="TDirectoryFile" or k.GetClassName()=="TDirectory":
            dir.cd(kname)
            dols(ROOT.gDirectory, keys)
         else:
            keys += [dir.GetPath()+"/"+kname]
      
      dir.cd("..")
      return

   keys = []
   basedir = dir.GetPath().rstrip("/") + "/"
   dols(dir,keys)

   # Return sorted list with base directory removed
   return sorted([k.replace(basedir,"") for k in keys])



def main():
      
   from optparse import OptionParser
   parser = OptionParser(usage = "usage: %prog [options] reference file",
                         description = "monitor values from one atn nightly to the next")
   

   parser.add_option("-t", "--threshold",
                     action = "store", type="float",
                     help = "threshold for percentage value change wrt past 7 nightlies")

   import json
   parser.add_option("-l", "--lower",action="store",type=str,help="Dictionary of lower thresholds for 1-bin histograms")

   parser.add_option("-o", "--output",
                     action = "store", dest = "output",
                     metavar = "NAME", default = "rootmonitor",
                     help = "write output to NAME.<input> (default: rootmonitor)")
   
   parser.add_option("-v", "--verbose",
                     action = "store_true", default = False,
                     help = "be verbose")


   parser.add_option("--html",action="store_true",default=False,help="generate root html code to view results in web browser")

   (opts, args) = parser.parse_args()
   
   if len(args)!=1:
      parser.print_help()
      return 255
   
   
   #find last valid atn output
   from TrigValTools.Utils import getPreviousNightlyPath
   import os.path
   refFile = None
   for i in range(1,7):
      refFile = getPreviousNightlyPath(i) + "/" + opts.output + "." + args[0]
      if os.path.isfile(refFile): break
      print "Could not find file %s" % refFile
      refFile = None
   
   # Default thresholds
   if not opts.threshold:
      opts.threshold = 0.05 #match 5% change allowed
   

   
   print "-"*70
   print " rootmonitor.py, "+__version__
   print
   print "Command    : rootmonitor.py %s" % (" ".join(sys.argv[1:]))
   print
   print "Reference  : %s" % (refFile)
   print "File       : %s" % (args[0])
   print "Threshold  : %.2f" % (opts.threshold)

   if opts.lower:
      print "Lower      : %s" % (opts.lower)
      opts.lower = json.loads(opts.lower)

   print "-"*70

   

   # Now import ROOT
   import cppyy
   try:
      from PerfMonAna import PyRootLib
      ROOT = PyRootLib.importRoot( batch=True )
   except ImportError:
      import ROOT

   sys.stdout.flush()
   sys.stderr.flush()
   
   if refFile != None: refFile = ROOT.TFile.Open(refFile)
   
   outFile = ROOT.TFile.Open(opts.output + "." + os.path.basename(args[0]),"RECREATE")
   
   result = 0
   
   #open file and look for 1-bin histograms
   file = ROOT.TFile.Open(args[0])
   keys = lsroot(file)
   import time
   myTime = int(time.strftime("%y%m%d"))
   for key in keys:
     if key.startswith("/"): key = key[1:]
     obj = file.Get(key)
     if obj and obj.InheritsFrom("TH1") and obj.GetNbinsX()==1:
       print "Found single bin histogram: %s (%s)" % (key,obj.GetTitle())
     else: continue
     graph = None
     if refFile != None:
       graph = refFile.Get(key)
     if graph == None or not graph.InheritsFrom("TGraphErrors"):
       #make a graph
       graph = ROOT.TGraphErrors()
       graph.SetTitle(obj.GetTitle())
     #append point
     n = graph.GetN()
     graph.SetPoint(n,myTime,obj.GetBinContent(1))
     graph.SetPointError(n,0,obj.GetBinError(1))
     #graph.SetDirectory(outFile)
     outFile.cd()
     graph.Write(key)
 
     if opts.lower and key in opts.lower:
        if(obj.GetBinContent(1) < opts.lower[key]):
           print "INFO Value in %s (%s) is below limit set by lower argument: %f vs %f" % (key,obj.GetTitle(),obj.GetBinContent(1),opts.lower[key])
           result = 255
     else:
        if graph.GetN()>7:
       #calculate average of previous 7 points and result is 255 if more than opts.threshold out
           sumVal = 0.0;
           for i in range(graph.GetN()-8,graph.GetN()-1):
              sumVal += graph.GetY()[i]
           sumVal = sumVal/7.0
           if abs(sumVal-obj.GetBinContent(1))/sumVal > opts.threshold:
              print "INFO Value in %s changed by more than threshold: %f to %f" % (key,sumVal,obj.GetBinContent(1))
              result = 255
   

   if opts.html:
      import os
      os.system("root2html.py *.root")
   

   print "Overall test result: %i" % result
   return result


if __name__ == "__main__":
   sys.exit(main())
