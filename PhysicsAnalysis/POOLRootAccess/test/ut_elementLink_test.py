# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


#Tests the element links are correctly available
import sys
import ROOT
evt = ROOT.POOL.TEvent(ROOT.POOL.TEvent.kClassAccess)
evt.readFrom("$ASG_TEST_FILE_MC")

#FIXME This relies on electrons being present in the first event!
evt.getEntry(0)
els = evt.retrieve("xAOD::ElectronContainer","Electrons")
for el in els:
    if not el.trackParticle():
        print "ERROR: Element link is invalid for kClassAccess .. exit"
        sys.exit(-1)


print "Element links were valid. Test passed"

