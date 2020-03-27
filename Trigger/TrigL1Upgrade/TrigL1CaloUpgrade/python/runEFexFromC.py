# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function


import ROOT

# later on we could define this in ROOT
TestFile="myAODTTv3.root"
OutputFile="test.output.root"
PoolAODInput=[TestFile]

# set conditions
DetDescrVersion="ATLAS-R2-2015-03-01-00"
ConditionsTag="OFLCOND-RUN12-SDR-31"
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
from TrigT1CaloEFex.TrigT1CaloEFexConf import TrigT1CaloEFex
a=TrigT1CaloEFex()
topSequence+=a
#from TrigT1CaloEFex.EFexAlgo import EFexAlgo
#topSequence+=EFexAlgo()

# initialize athena
theApp.initialize()
# main loop
ev=0
for i in range(0,10):
 try : 
  a=not theApp.nextEvent().isFailure() 
  print ('Event Number',ev)
  ev=ev+1;
 except : 
  print ("except")
  break
theApp.finalize().ignore();

print ('writing out file after',ev,'events')
theApp.exit()

