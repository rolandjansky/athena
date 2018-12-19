#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

if not 'doHLTCaloTopo' in dir() :
  doHLTCaloTopo=True
if not 'doL2Egamma' in dir():
  doL2Egamma=True

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

isData = False
if globalflags.InputFormat.is_bytestream():
  isData = True

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer

if TriggerFlags.doCalo:

  if ( doHLTCaloTopo ) :

     from TrigT2CaloCommon.CaloDef import addHLTCaloCell, addHLTTopoCluster
     topSequence+=addHLTCaloCell(OutputLevel=DEBUG)    
     topSequence+=addHLTTopoCluster(OutputLevel=DEBUG)    

  if ( doL2Egamma ) :
     from TrigT2CaloCommon.CaloDef import addL2Egamma
     topSequence+=addL2Egamma(OutputLevel=DEBUG)

  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()
