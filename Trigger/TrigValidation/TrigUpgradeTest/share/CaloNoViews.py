#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

if not 'doHLTCaloTopo' in dir() :
  doHLTCaloTopo=True
if not 'doL2Egamma' in dir():
  doL2Egamma=True

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer

if TriggerFlags.doCalo:

  if ( doHLTCaloTopo ) :

     from TrigT2CaloCommon.CaloDef import addHLTCaloCell, addHLTTopoCluster
     topSequence+=addHLTCaloCell()
     topSequence+=addHLTTopoCluster()

  if ( doL2Egamma ) :
     from TrigT2CaloCommon.CaloDef import addL2Egamma
     topSequence+=addL2Egamma()

  from AthenaCommon.AlgSequence import dumpMasterSequence
  dumpMasterSequence()
