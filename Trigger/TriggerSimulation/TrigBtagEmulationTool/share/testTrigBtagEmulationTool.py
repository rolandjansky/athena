# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from AthenaPoolCnvSvc import ReadAthenaPool
from AthenaCommon.AppMgr import ToolSvc
import glob

from TrigBjetHypo.TrigBjetFexTuningGrade1_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade1_IP3D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP1D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP2D import *
from TrigBjetHypo.TrigBjetFexTuningGrade2_IP3D import *
from TrigBjetHypo.TrigBjetFexTuning_EVTX import *
from TrigBjetHypo.TrigBjetFexTuning_MVTX import *
from TrigBjetHypo.TrigBjetFexTuning_NVTX import *


rec.readAOD=True


#Skip first 47000 events
#acf.SkipEvents=47000 
#
#Run on the firsts 47000 events only
#acf.EvtMax=47000

#Run in all events
acf.EvtMax=-1

#complete EOS sample (do "eosmount eos" in ~/ before using it)
#lista = glob.glob('/afs/cern.ch/user/g/gmarceca/eos/atlas/atlasdatadisk/rucio/valid1/*/*/AOD.01575399._*.pool.root.1')

#Local file:
# REAL DATA
#lista = glob.glob('/afs/cern.ch/work/c/cschiavi/public/DATA-VALIDATION/data15_13TeV.00280500.physics_EnhancedBias.merge.AOD.r7806_p2565_tid08010770_00/AOD.08010770._000198.pool.root.1')
# MC DATA: TTBAR
#lista = glob.glob('/afs/cern.ch/work/c/cschiavi/public/DATA-VALIDATION/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r6630_r6264/AOD.05382618._000251.pool.root.1')
lista = glob.glob('/afs/cern.ch/work/c/cschiavi/public/DATA-VALIDATION/mc15_13TeV.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.merge.AOD.e3698_s2608_s2183_r7773_r7676/AOD.07981831._000076.pool.root.1')
# MC DATA: VBF Hbb
#lista = glob.glob('/afs/cern.ch/work/c/cschiavi/public/DATA-VALIDATION/mc15_13TeV.341566.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_VBFH125_bb.merge.AOD.e3988_a766_a767_r6264/AOD.05739017._000075.pool.root.1')
# REPROCESSED DATA
#lista = glob.glob('/afs/cern.ch/work/c/cschiavi/public/DATA-VALIDATION/data15_13TeV.00282625.physics_Main.recon.AOD.r7734/AOD.07908618._001196.pool.root.1')
# HIGGS DERIVED SAMPLE
#lista = glob.glob('root://eosatlas//eos/atlas/user/c/cvarni/VBF/Run2/VBFH125_bb_merge_DAOD_HIGG5D3/DAOD_HIGG5D3.05894664._000001.pool.root.1')

#####acf.FilesInput.set_Value_and_Lock(['root://eosatlas//eos/atlas/user/c/cvarni/VBF/Run2/VBFH125_bb_merge_DAOD_HIGG5D3/DAOD_HIGG5D3.05894664._000001.pool.root.1'])
acf.FilesInput.set_Value_and_Lock(lista) 

rec.doCBNT=False

from RecExConfig.RecFlags import rec
rec.doTrigger=True
from RecExConfig.RecAlgsFlags  import recAlgs
recAlgs.doTrigger=True
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.doTriggerConfigOnly=True

rec.doWriteAOD=False
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doAOD=False
rec.doDPD=False 
rec.doESD=False
rec.doCBNT = False
rec.doHist = False  #default (True)

doTAG=False

rec.doTruth=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

# abort when there is an unchecked status code
StatusCodeSvc.AbortOnError=False

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()

# TDT
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
ToolSvc.TrigDecisionTool.TrigDecisionKey='xTrigDecision'

# OFFLINE BTAGGING
from BTagging.BTaggingFlags import BTaggingFlags
from BTagging.BTaggingConfiguration import getConfiguration
## Comment for untuned offline tools / uncomment for tuned ones
# # BTaggingFlags.TrigCalibrationFolderRoot = '/GLOBAL/TrigBTagCalib/'
# # BTaggingFlags.TrigCalibrationTag = 'TrigBTagCalibRUN12Onl-08-10'
BTagConfig = getConfiguration("Trig")
BTagConfig.PrefixxAODBaseName(False)
BTagConfig.PrefixVertexFinderxAODBaseName(False)
BTagConfig.setupJetBTaggerTool(ToolSvc, "AntiKt4EMTopo", SetupScheme="Trig", TaggerList=BTaggingFlags.TriggerTaggers)

# ONLINE EMULATOR
from TrigBtagEmulationTool.TrigBtagEmulationToolConf import Trig__TrigBtagEmulationTool
emulator = Trig__TrigBtagEmulationTool()
emulator.BTagTrackAssocTool = BTagConfig.getJetCollectionMainAssociatorTool("AntiKt4EMTopo")
emulator.BTagTool           = BTagConfig.getJetCollectionTool("AntiKt4EMTopo")
emulator.BTagSecVertexing   = BTagConfig.getJetCollectionSecVertexingTool("AntiKt4EMTopo")
emulator.UseTriggerNavigation = True
emulator.TagOfflineWeights = False
emulator.TagOnlineWeights = False
emulator.EmulatedChainDefinitions = [
    ["HLT_2j35_bmv2c2070_2j35_L13J25.0ETA23",
     "L1_3J25.0ETA23",
     "EMUL_HLT_4j35",
     "EMUL_HLT_2j35_bmv2c2070"],
    ["HLT_2j45_bmv2c2077_2j45_L13J25.0ETA23",
     "L1_3J25.0ETA23",
     "EMUL_HLT_4j45",
     "EMUL_HLT_2j45_bmv2c2077"],
]
ToolSvc += emulator

# TEST ALGORITHM
from TrigBtagEmulationTool.TrigBtagEmulationToolConf import Trig__TrigBtagEmulationToolTest
test = Trig__TrigBtagEmulationToolTest()
test.TrigBtagEmulationTool = emulator
test.OutputLevel = 0
theJob += test

## Define your output root file using MultipleStreamManager
## ====================================================================
#from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
#MyFirstHistoXAODStream = MSMgr.NewRootStream( rootStreamName, rootFileName )

ServiceMgr.MessageSvc.defaultLimit = 9999999999
