# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ------------------------------------------------------------
# DerivationFrameworkMaster.py
# J. Catmore (James.Catmore@cern.ch)
# ------------------------------------------------------------
# Contains all basic includes for running the derivation framework
#-------------------------------------------------------------

from AthenaCommon.AppMgr import ToolSvc
# Output file name handling
from D2PDMaker.D2PDFlags import D2PDFlags
from D2PDMaker.D2PDHelpers import buildFileName
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
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
# Trigger navigation slimming
#from TrigNavTools.TrigNavToolsConf import HLT__TrigNavigationSlimmingTool, TrigNavigationThinningTool, HLT__StreamTrigNavSlimming
#from TrigNavTools.TrigNavToolsConfig import navigationSlimming

# Sequence for the AuxStoreWrappers (for slimming)
AuxStoreWrapperSequence = CfgMgr.AthSequencer("AuxStoreWrapperSequence")

# DerivationJob is COMMON TO ALL DERIVATIONS
DerivationFrameworkJob = AlgSequence()
DerivationFrameworkJob += AuxStoreWrapperSequence

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

###ON HOLD FOR NOW
##use pileupreweighting provider to generate a df.metadata.root prw config file
##only to run if the input file is post-digitization (i.e. dont run this on EVNT)
#from RecExConfig.InputFilePeeker import inputFileSummary
#if 'metadata' in inputFileSummary and '/Digitization/Parameters' in inputFileSummary['metadata']:
#  DerivationFrameworkJob += CfgMgr.CP__PileupReweightingProvider(ConfigOutputStream="DFMETADATA")
#  if not hasattr(svcMgr,'THistSvc'):
#    svcMgr += CfgMgr.THistSvc()
#  svcMgr.THistSvc.Output += ["DFMETADATA DATAFILE='df.metadata.root' OPT='RECREATE'"]

from RecExConfig.InputFilePeeker import inputFileSummary
if (inputFileSummary['evt_type'][0] == 'IS_SIMULATION') and (inputFileSummary['stream_names'][0] != 'StreamEVGEN'):
	svcMgr.IOVDbSvc.Folders += ['/Simulation/Parameters']
	
# Set up the metadata tool:
ToolSvc += CfgMgr.xAODMaker__FileMetaDataCreatorTool( "FileMetaDataCreatorTool",
						      OutputLevel = 2 )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.FileMetaDataCreatorTool ]

# Set up stream auditor
if not hasattr(svcMgr, 'DecisionSvc'):
        svcMgr += CfgMgr.DecisionSvc()
svcMgr.DecisionSvc.CalcStats = True

# Trigger decision tool
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter('ReadPool')
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
tdt = Trig__TrigDecisionTool("TrigDecisionTool")
ToolSvc += tdt

# Centrally setting  flags
jetFlags.useTracks = True
# MC-related flags
DerivationFrameworkIsMonteCarlo=False
DerivationFrameworkSimBarcodeOffset = int(200e3)
if globalflags.DataSource()=='geant4':
    print "Switching on jetFlags.useTruth"
    jetFlags.useTruth = True
    DerivationFrameworkIsMonteCarlo = True
    try:
        DerivationFrameworkSimBarcodeOffset = int(inputFileSummary['metadata']['/Simulation/Parameters']['SimBarcodeOffset'])
    except:
        print 'Could not retrieve SimBarcodeOffset from /Simulation/Parameters, leaving at 200k'
    
