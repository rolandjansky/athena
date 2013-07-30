#!/usr/bin/python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os
import sys, string, commands
from datetime import datetime

## Needed to correct ROOT behavior; see below
CWD = os.getcwd()
import ROOT
## Importing gSystem may change the current directory to one of the
## command-line arguments; chdir to original directory to have
## predictable behavior
from ROOT import gSystem
os.chdir(CWD)
## Import the ROOT library for reading han results
gSystem.Load('libGoodRunsListsLib')
from ROOT import Root

## read the goodrunslist xml file(s)
reader = Root.TGoodRunsListReader()
reader.SetXMLFile('Collisions_DQgood_runlist.xml')
reader.Interpret()
grl = reader.GetMergedGoodRunsList()

## show summary of content
grl.Summary() ## Detailed: Summary(True)

### read grl of all collisions
#reader.SetXMLFile('prelim_collisions.xml')
#reader.Interpret()
#grl_all = reader.GetMergedGoodRunsList()
#grl_all.Summary()

### try out the following functions
#sumgrl = grl_all.GetSumWith(grl)
#GetOverlapWith()
#GetPartOnlyIn()
#GetPartNotIn()
### Are there lumiblock appearing only only grl, that are not in grl_all ? 
### Store the overlap between grl and grl_all as the object grl_overlap

### Now write out the overlapping part between 'grl' and 'grl_all'
#writer = Root.TGoodRunsListWriter()
#writer.SetGoodRunsList(grl_overlap)
#writer.SetFilename('collisions_dqgood.xml')
#writer.WriteXMLFile()

