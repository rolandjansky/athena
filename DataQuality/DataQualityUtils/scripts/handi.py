#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import DataQualityUtils.handimod as mod

import sys

import getopt

def handi( name, resultsFile, htmlDir, browserMenu=False, allDirsScriptLoc="https://atlasdqm.cern.ch/static/js/" ):

  ## Instructions for the compare button and the browser menu:

  ## Compare buttons only work with the typical run/stream structure. See DQWebDisplay.py for more info
  compare = False  
  javaScriptLoc =""
  HjavaScriptLoc =""
  runListLoc = ""
  
  ## If you want a browser menu instead of the default directory listing, put browserMenu = True ...
  #browserMenu = False
  ## ... and give in the url of the script AllDirs.js, eg:
  ##  allDirsScriptLoc="http://atlasdqm.web.cern.ch/atlasdqm/tier0/AllDirs.js"
  #allDirsScriptLoc=""
  
  mod.handiWithComparisons( name, resultsFile, htmlDir, runListLoc, compare, browserMenu, allDirsScriptLoc )


def usage():
  cmdi = sys.argv[0].rfind("/")
  cmd = sys.argv[0][cmdi+1:]
  print("Usage: ", cmd, "[-m|--browser-menu] [-s|--script-loc=<URL>] <name_of_system> <results_file_name> <html_output_directory>")


########################################
## main()
########################################

short_options = "hms:"
long_options = ["help","browser-menu","script-loc="]

if __name__ == "__main__":

  # Default
  browserMenu      = False
  allDirsScriptLoc = ""

  try:
    opts, args = getopt.getopt(sys.argv[1:], short_options, long_options)
  except getopt.GetoptError:
    usage()
    sys.exit(1)
  for cmd, arg in opts:
    if(cmd in ('--help',) or cmd in ('-h',)):
      usage()
      sys.exit(0)
    if (cmd in ('--browser-menu',) or cmd in ('-m',)):
      browserMenu = True
    if (cmd in ('--script-loc',) or cmd in ('-s',)):
      allDirsScriptLoc = arg

  if len(args) != 3:
    usage()
    sys.exit(0)
  
  # Input
  name        = args[0] # name of the subdetector
  resultsFile = args[1] # root file with output
  html_dir    = args[2] # destination directory for html files
  handi( name, resultsFile, html_dir, browserMenu, allDirsScriptLoc )
