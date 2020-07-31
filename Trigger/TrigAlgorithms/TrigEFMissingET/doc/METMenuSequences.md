# MET Menu Sequences

This documentation serves as an overview for how the MET sequences work.  
The code presented here is based on the tests that were run early in the AthenaMT migration process.
Now these "flattened" sequences will hopefully be helpful to anyone who wishes to make sense of 
the MET trigger code in the menu.

This file was written in Nov 2019, when the Run-3 trigger menu was coming together.  
Note bene things may have changed by the time you read this.  
However the core ideas should still be valid throughout Run-3.


## InputMaker, reco, and hypo

```python
include("TriggerJobOpts/runHLT_standalone.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection

from DecisionHandling.DecisionHandlingConf import InputMakerForRoI, ViewCreatorInitialROITool
InputMakerAlg = InputMakerForRoI("MetCellInputMaker", RoIsLink="initialRoI")
InputMakerAlg.RoITool = ViewCreatorInitialROITool()
InputMakerAlg.RoIs='METCellRoI'
InputMakerAlg.InputMakerInputDecisions=[mapThresholdToL1DecisionCollection("XE")]
InputMakerAlg.InputMakerOutputDecisions="InputMaker_from_L1MET"
topSequence += InputMakerAlg

from TriggerMenuMT.HLTMenuConfig.MET.METSequences import metCellRecoSequence
cellRecoSeq, METkey = metCellRecoSequence(InputMakerAlg.RoIs)
topSequence += cellRecoSeq

from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, MissingETHypoToolMT

metHypoTool = MissingETHypoToolMT("METHypoTool")
metHypoTool.metThreshold=50

metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
metHypoAlg.HypoTools = [metHypoTool]
metHypoAlg.METContainerKey="HLT_MET"
metHypoAlg.HypoInputDecisions = InputMakerAlg.InputMakerOutputDecisions
metHypoAlg.HypoOutputDecisions = "EFMETDecisions"

topSequence += metHypoAlg

topSequence.L1Decoder.Chains="HLTChains"
```


## Reco sequence and hypo

```python 

include("TriggerJobOpts/runHLT_standalone.py")

METalgorithm = "cell" # or tc, mht, pufit

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import math
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

mon = GenericMonitoringTool("CaloDataAccessSvcMon")
mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", path='EXPERT', title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiROBs_LAr", path='EXPERT', title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                   defineHistogram( "TIME_locking_LAr_FullDet", path='EXPERT', title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", path='EXPERT', title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-math.pi, ymax=math.pi )]

svcMgr += TrigCaloDataAccessSvc()
svcMgr.TrigCaloDataAccessSvc.MonTool = mon

from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

#################################################
# Get inputs for the MET reconstruction         #
#################################################

if METalgorithm == "cell":
  from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
  cellMakerAlgo =  HLTCaloCellMaker("CellMakerMT")
  cellMakerAlgo.roiMode=True
  cellMakerAlgo.RoIs="FullScanRoIs" 
  cellMakerAlgo.CellsName="cells"
  topSequence += cellMakerAlgo
elif METalgorithm == "pufit" or METalgorithm == "tc":
  from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
  clusterSequence, clusterContainer = HLTFSTopoRecoSequence( RoIs="FullScanRoIs" )
  topSequence += clusterSequence 
elif METalgorithm == "mht":
  from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoSequence
  from TrigUpgradeTest.jetMenuHelper import jetDictFromString
  from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool
  jetRecoDict=jetDictFromString("a4_tc_em_subjes")
  (jetSequence, jetsKey) = RecoFragmentsPool.retrieve( jetRecoSequence, None, dataSource="data", RoIs="FullScanRoIs", **jetRecoDict )
  topSequence += jetSequence

#################################################
# Add EFMissingETFrom** algorithm
#################################################

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

metAlg = EFMissingETAlgMT( name="EFMET" )
metAlg.METContainerKey="HLT_MET_{}".format(METalgorithm)
metAlg.MonTool = getMETMonTool()

if METalgorithm == "cell":
  from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromCellsMT, EFMissingETFlagsMT
  cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
  cellTool.CellsCollection = "cells"
  flagTool = EFMissingETFlagsMT("theFlagsTool") 
  metAlg.METTools=[ cellTool, flagTool ]
elif METalgorithm == "tc":
  from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersMT
  clusterTool = EFMissingETFromClustersMT( name="METFromClustersTool" )
  clusterTool.ClustersCollection = clusterContainer
  metAlg.METTools=[ clusterTool ]
elif METalgorithm == "mht":
  from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromJetsMT
  mhtTool = EFMissingETFromJetsMT( name="METFromJetsTool" )
  mhtTool.JetsCollection = jetsKey
  metAlg.METTools=[ mhtTool ]
elif METalgorithm == "pufit":
  from TrigEFMissingET.TrigEFMissingETConf import EFMissingETFromClustersPufitMT
  clusterPufitTool = EFMissingETFromClustersPufitMT( name="METFromClustersPufitTool" )
  clusterPufitTool.ClustersCollection = clusterContainer
  metAlg.METTools=[ clusterPufitTool ]

topSequence += metAlg

#################################################
# Add TrigMissingETHypo algorithm and tool
#################################################
from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, MissingETHypoToolMT

def makeMETHypoTool():
    hypoTool = MissingETHypoToolMT("HLT_xe10")
    hypoTool.metThreshold = 10
    return hypoTool

hypoAlg = MissingETHypoAlgMT("METHypoAlg")
hypoAlg.HypoTools=[makeMETHypoTool()]
hypoAlg.METContainerKey=metAlg.METContainerKey

from L1Decoder.L1DecoderConfig import mapThresholdToL1DecisionCollection

hypoAlg.OutputLevel = DEBUG
hypoAlg.HypoInputDecisions = mapThresholdToL1DecisionCollection("XE")
hypoAlg.HypoOutputDecisions = "EFMETDecisions"
topSequence += hypoAlg

```

