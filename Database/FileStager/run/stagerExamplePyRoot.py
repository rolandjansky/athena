# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# PyRoot example, by Till Eifert

import os
import sys, string, commands
from datetime import datetime

## Needed to correct ROOT behavior; see below
CWD = os.getcwd()
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem, gROOT
os.chdir(CWD)
## Import the ROOT library for reading han results
gSystem.Load('libFileStagerLib')
from ROOT import TStageManager, TCopyFile, TCopyChain



def init():
  # Load the filestager dictionary
  gSystem.Load("../${CMTCONFIG}/libFileStagerLib.so")
  # make sure ROOT picks up TCopyFile when filename starts with gridcopy://
  gROOT.GetPluginManager().AddHandler("TFile", "^gridcopy:", "TCopyFile","TCopyFile_cxx", "TCopyFile(const char*,Option_t*,const char*,Int_t)")
  pass



def Example(sample, doStaging=True, treeName="CollectionTree" ) :
  # load the filestager library, configure root
  init()
  #
  # EXAMPLE: RFIO
  if (doStaging) :
    # turn on staging functionality
    TCopyChain.SetOriginalTChain(False)
    TCopyFile.SetOriginalTFile(False)
    # stager settings
    manager = TStageManager.instance()
    manager.setInfilePrefix("gridcopy://")
    manager.setCpCommand("rfcp")
    # by default manager stores in $TMPDIR, or else /tmp ifndef
    #manager.setBaseTmpdir("/tmpdir")
    #manager.setPipeLength(1)
    # turn on verbosity
    # manager.verbose()     # lots of output
    manager.verboseWait()   # useful to see if your next has not yet finished staging
    pass
  #
  # --- Find and open sample definition file ---
  #
  f = file(sample, "r")
  if (not f) :
    print "Error opening sample definition file <%s>" % (defFileName)
    exit()
    pass
  #
  ntupleChain = TCopyChain(treeName)
  #
  for line in f:
    ls  = line.strip()
    if ls.find("#") == 0 : continue
    if ls.find("//") ==0 : continue
    if (len(ls) == 0) : continue
    if ls.find("TITLE") != -1 : continue
    if ls.find("FLAGS") != -1 : continue
    print "Adding file <%s>"  %(ls)
    ntupleChain.Add(ls)
    pass
  #
  print "Done adding samples."
  #
  # Loop over chain
  nentries = ntupleChain.GetEntriesFast()
  jentry=0
  nb=0  
  #
  for i in xrange(nentries) :
     ientry = ntupleChain.LoadTree(jentry) #in case of a TChain, ientry is the entry number in the current file
     isNewTree = (ientry==0)
     if (ientry < 0) : break
     if jentry % 1000 == 0 :
       print  "Now processing event %i"  % jentry    
     ## use this to retrieve events.
     #size = ntupleChain.GetEntry(jentry)
     #if (size > 0) :  
     #  nb += size
     #else :       
     #  print "Framework: end of tree reached, stopping event processing loop"
     #  break
     #
     jentry += 1;
     pass

