# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
# DerivationFrameworkMaster.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------
# Contains all basic includes for running the derivation framework
#-------------------------------------------------------------

from __future__ import print_function

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
    if (inputFileSummary['evt_type'][0] == 'IS_SIMULATION') and (inputFileSummary['stream_names'][0] != 'StreamEVGEN'):
        svcMgr.IOVDbSvc.Folders += ['/Simulation/Parameters']
    
# Set up the metadata tool:
if not globalflags.InputFormat=="bytestream":
    # Extra config: make sure if we are using EVNT that we don't try to check sim/digi/reco metadata 
    from RecExConfig.ObjKeyStore import objKeyStore
#    ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool( "FileMetaDataCreatorTool",
#                                  isEVNT = objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ),
#                                  OutputLevel = 2 )
#    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.FileMetaDataCreatorTool ]

# Set up stream auditor
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True

# Trigger decision tool
# SUPERFLUOUS
#if rec.doTrigger() or TriggerFlags.doTriggerConfigOnly():
#    from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
#    cfg = TriggerConfigGetter('ReadPool')
#    from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
#    tdt = Trig__TrigDecisionTool("TrigDecisionTool")
#    ToolSvc += tdt

# Centrally setting  flags
jetFlags.useTracks = True
# MC-related flags
DerivationFrameworkIsMonteCarlo=False
DerivationFrameworkSimBarcodeOffset = int(200e3)
if globalflags.DataSource()=='geant4':
    print ("Switching on jetFlags.useTruth")
    jetFlags.useTruth = True
    DerivationFrameworkIsMonteCarlo = True
    try:
        # Extra config: make sure if we are using EVNT that we don't try to check sim metadata 
        from RecExConfig.ObjKeyStore import objKeyStore
        # Make sure input file is not EVNT
        if not objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
            DerivationFrameworkSimBarcodeOffset = int(inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset'])
    except:
        print ('Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k')

def buildFileName(derivationStream):
    return derivationStream.FileName
