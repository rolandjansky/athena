#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# $Id: root2html.py,v 1.5 2008-09-08 13:49:16 fwinkl Exp $

import sys
import os
from AthenaCommon.Utils.unixtools import FindFile

def main():
         
   # Parse arguments
   from optparse import OptionParser
   parser = OptionParser(usage = "usage: %prog [options] file1 [file2] ..." )
   
   parser.add_option("-o", "--outDir",
                     default = "./",
                     help = "write output to DIR")

   parser.add_option("-W", "--width",
                     default = 400,
                     help = "image width [400]")

   parser.add_option("-H", "--height",
                     default = 400,
                     help = "image height [400]")

   parser.add_option("-s","--skip", action="append", type="string",
                     help = "Regular expression of histograms to skip. "\
                     "Can be used multiple times.")

   parser.add_option("-i","--include", action="append", type="string",
                     help = "Regular expression of histograms to always include. "\
                     "Can be used multiple times.")
      
   parser.add_option("--drawOpt", action="append", type="string",
                     help = "Add draw options for histogram. Example: 'Error.*:text' will "\
                     "draw all histograms whose name matches 'Error.*' with the 'text' option. "\
                     "Regular expressions are allowed for the name and this option can be "\
                      "used multiple times.")
   
   parser.add_option("--classDrawOpt", action="append", type="string",
                     help = "Add class wide draw options. Example: 'TH2:colz' will "\
                     "draw all 2D histograms with the 'colz' option. Regular expressions "\
                     "are allowed for the class name and this option can be used multiple times.")

   parser.add_option("-v", "--verbose",
                     action = "store_true", default = False,
                     help = "be verbose")
   
   (opts, args) = parser.parse_args()
   
   if len(args)<1:
      parser.print_help()
      return

   from TrigValTools.Utils import findInstallDir
   installDir = findInstallDir("TrigValTools/root2html")
   if not installDir:
      print "Could not find installation directory 'TrigValTools/root2html'"
      return 1
   
   # Now import ROOT
   import PyCintex
   from PerfMonAna import PyRootLib
   ROOT = PyRootLib.importRoot( batch=True )
   
   from ROOT import TRoot2Html   
   from ROOT import gROOT, gStyle
   gROOT.SetStyle("Plain")
   gStyle.SetOptStat(1111111);
   gROOT.ForceStyle();

   # Check if output directory exists
   if not os.path.exists(opts.outDir):
      print "Cannot write to output directory "+opts.outDir
      return 1

   root2html = TRoot2Html()
   root2html.setVerbose(opts.verbose)
   root2html.setOutputDir(opts.outDir)
   
   root2html.setImageSize(opts.width, opts.height)

   if opts.skip:
      for re in opts.skip:
         root2html.addFailRegexp(re)

   if opts.include:
      for re in opts.include:
         root2html.addPassRegexp(re)

   if opts.drawOpt:
      for o in opts.drawOpt:
         tpl = o.split(":")
         if len(tpl)==2: root2html.addDrawOptions(tpl[0],tpl[1]);

   # Always draw 2D histograms as colz, except user specified something else
   if not opts.classDrawOpt: opts.classDrawOpt = ["TH2:colz"]
   for o in opts.classDrawOpt:
      tpl = o.split(":")
      if len(tpl)==2: root2html.addClassDrawOptions(tpl[0],tpl[1]);   
      
   for s in args:
      root2html.addFile(s)
      
   root2html.run();

   # Copy html files to output directory
   for s in ["index.html","tree.html"]:
      cmd = "cp %s %s" % (os.path.join(installDir,s),opts.outDir)
      os.system(cmd)


if __name__ == "__main__":
   sys.exit(main())
