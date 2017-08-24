#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# art-description: Test basic xAOD reading in python
# art-type: build
# art-ci: 21.2

#This is an example of reading a POOL file in PyROOT

import ROOT
evt = ROOT.POOL.TEvent()
#evt = ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess) #for faster xAOD reading

import os
if os.environ.has_key("ASG_TEST_FILE_MC"):
    fname = "$ASG_TEST_FILE_MC"
else:
    fname = "/afs/cern.ch/atlas/maxidisk/d33/referencefiles/aod/AOD-devval-20160812/AOD-devval-20160812-full.pool.root"
evt.readFrom(fname)

for i in range(0,10): #use evt.getEntries() to read all
    evt.getEntry(i);

    #normal retrieve method types a 'type' and 'key'
    ei = evt.retrieve("xAOD::EventInfo","EventInfo")
    print ei.eventNumber()

    #possible (but slower) to retrieve just by 'key'
    els = evt.get_item("Electrons")
    for el in els: ROOT.xAOD.dump(el)
