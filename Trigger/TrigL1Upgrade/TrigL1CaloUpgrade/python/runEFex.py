# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT

TestFile=""
PoolAODInput=[TestFile]

# set conditions
DetDescrVersion="ATLAS-R2-2016-01-00-01"
ConditionsTag="OFLCOND-MC16-SDR-20"
# set everything
include ("RecExCond/RecExCommon_DetFlags.py")
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
include ("RecExCond/AllDet_detDescr.py")

# Set the input file
import AthenaPoolCnvSvc.ReadAthenaPool

svcMgr.EventSelector.InputCollections = [TestFile]

# import the algorithm in python and make sure it
# is attached to the sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

topSequence += CfgMgr.TrigT1CaloEFex()

# initialize athena
theApp.initialize()
# main loop
ev=0
for i in range(0,10):
 try : 
  a=not theApp.nextEvent().isFailure() 
  print 'Event Number',ev
  ev=ev+1;
 except : 
  print "except"
  break
theApp.finalize().ignore();

print 'writing out file after',ev,'events'
theApp.exit()

