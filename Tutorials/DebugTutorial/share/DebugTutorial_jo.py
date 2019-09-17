#
# File: DebugTutorial/share/DebugTutorial_jo.py
# Author: snyder@bnl.gov
# Date: Sep 2019
# Purpose: debugging tutorial
#

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

#--------------------------------------------------------------
# Define input
#--------------------------------------------------------------
#/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/aod/
svcMgr.EventSelector.InputCollections = [
    "/eos/atlas/atlascerngroupdisk/data-art/grid-input/aod/AOD-20.1.6.4/AOD-20.1.6.4-full.pool.root"
    ]

from DebugTutorial.DebugTutorialConf import Tutorial__TestAlg
topSequence += Tutorial__TestAlg ("TestAlg", Jets = 'AntiKt4LCTopoJets',
                                  OutputLevel = VERBOSE)
