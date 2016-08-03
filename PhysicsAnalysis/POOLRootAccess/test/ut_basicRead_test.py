# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#This is an example of reading a POOL file in PyROOT

import ROOT
evt = ROOT.POOL.TEvent()
#evt = ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess) #for faster xAOD reading

evt.readFrom("$ASG_TEST_FILE_MC")

for i in range(0,10): #use evt.getEntries() to read all
    evt.getEntry(i);
    ei = evt.retrieve("xAOD::EventInfo","EventInfo")
    print ei.eventNumber()
