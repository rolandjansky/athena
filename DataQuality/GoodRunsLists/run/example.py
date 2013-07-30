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
gSystem.Load('../StandAlone/libGoodRunsLists.so')
from ROOT import Root


## read the goodrunslist xml file(s)
reader = Root.TGoodRunsListReader('LumiRangeCollectionA.xml')
#reader.AddXMLFile('LumiRangeCollectionB.xml')
reader.Interpret()
goodrunslist = reader.GetMergedGRLCollection()

## show summary of content
goodrunslist.Summary() ## Detailed: Summary(True)

## constains run and/or lumiblock ?
if goodrunslist.HasRun (90210) : print "Foo!"    # contains run 90210 ?
if goodrunslist.HasRunLumiBlock (90210,7) : print "Bar!"  # run 90210, lb 7 ?

## Now write out xml again ... 
writer = Root.TGoodRunsListWriter()
writer.SetGRLCollection(goodrunslist)
writer.SetFilename('test2.xml')
writer.WriteXMLFile()

