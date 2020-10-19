# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
# DerivationFrameworkMaster.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------
# Contains all basic includes for running the derivation framework
#-------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AppMgr import theApp
# Derivation names and locations of job options
from DerivationFrameworkCore.DerivationFrameworkProdFlags import derivationFlags
# Multiple stream manager
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
# Athena common properties
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon import CfgMgr 
from AthenaCommon.AlgSequence import AlgSequence 
from JetRec.JetRecFlags import jetFlags 
from AthenaCommon.GlobalFlags  import globalflags
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
# AODFix object, for checking whether it ran or not

import os
if "AthAnalysisBase" not in os.environ.get("CMTEXTRATAGS",""):
  from AODFix.AODFix import *

# Trap for ROOT6 errors
theApp.CreateSvc += ["AthROOTErrorHandlerSvc"]

from AthenaCommon import Logging
dfmasterlog = Logging.logging.getLogger('DFMaster')

# Trigger navigation slimming
#from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool, TrigNavigationThinningTool, HLT__StreamTrigNavSlimming
#from TrigNavTools.TrigNavToolsConfig import navigationSlimming

# Sequence for the AuxStoreWrappers (for slimming)
AuxStoreWrapperSequence = CfgMgr.AthSequencer("AuxStoreWrapperSequence")

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = AlgSequence()

# Aux store wrapper for expansion-to-dynamic. This is only used now for
# a handful of container types, defined in ContainersForExpansion.py 
DerivationFrameworkJob += AuxStoreWrapperSequence

# Special sequence run after the algsequence
# Being used here to reset ElementLinks
#if AODFix_willDoAODFix(): This flag doesn't work so commented for now
athOutSeq = CfgMgr.AthSequencer("AthOutSeq")
athOutSeq += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )

from RecExConfig.InputFilePeeker import inputFileSummary
if inputFileSummary is not None:
    # If we have Simulation metadata in the file or if this is simulation, add it to the IOVDbSvc
    # Might have Simulation metadata but not IS_SIMULATION if we're running data overlay
    if ('metadata' in inputFileSummary and '/Simulation/Parameters' in inputFileSummary['metadata']) or \
       (inputFileSummary['evt_type'][0] == 'IS_SIMULATION' and inputFileSummary['stream_names'][0] != 'StreamEVGEN'):
        svcMgr.IOVDbSvc.Folders += ['/Simulation/Parameters']
    
# Set up the metadata tool:
if not globalflags.InputFormat=="bytestream":
    # Extra config: make sure if we are using EVNT that we don't try to check sim/digi/reco metadata 
    from RecExConfig.ObjKeyStore import objKeyStore
    ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool( "FileMetaDataCreatorTool",
                                  isEVNT = objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ),
                                  OutputLevel = 2 )
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.FileMetaDataCreatorTool ]

# Set up stream auditor
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True

# Centrally setting  flags
jetFlags.useTracks = True
# MC-related flags
DerivationFrameworkHasTruth=False
DerivationFrameworkHasxAODTruth=False
DerivationFrameworkSimBarcodeOffset = int(200e3)
DerivationFrameworkIsDataOverlay=False
# To establish if this file has truth information, we look for the relevant collections
from RecExConfig.ObjKeyStore import objKeyStore
if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ) or \
   objKeyStore.isInInput( "McEventCollection", "TruthEvent") or \
   objKeyStore.isInInput( "xAOD::TruthEventContainer","TruthEvents"):
    DerivationFrameworkHasTruth=True
    dfmasterlog.info("Switching on jetFlags.useTruth")
    jetFlags.useTruth = True
    if 'metadata' in inputFileSummary and '/Simulation/Parameters' in inputFileSummary['metadata'] and \
       'SimBarcodeOffset' in inputFileSummary['metadata']['/Simulation/Parameters']:
        DerivationFrameworkSimBarcodeOffset = int(inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset'])
    else:
        dfmasterlog.warning('Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k')
if objKeyStore.isInInput( "xAOD::TruthEventContainer","TruthEvents"):
    DerivationFrameworkHasxAODTruth=True
if globalflags.DataSource() is not 'geant4' and DerivationFrameworkHasTruth:
    DerivationFrameworkIsDataOverlay=True

def buildFileName(derivationStream):
    return derivationStream.FileName
