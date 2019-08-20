#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import math
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

mon = GenericMonitoringTool("CaloDataAccessSvcMon")
mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", path='EXPERT', title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiROBs_LAr", path='EXPERT', title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                   defineHistogram( "TIME_locking_LAr_FullDet", path='EXPERT', title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", path='EXPERT', title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-math.pi, ymax=math.pi )]

svcMgr += TrigCaloDataAccessSvc()
svcMgr.TrigCaloDataAccessSvc.MonTool = mon


#################################################
# Create RoI and get jets
#################################################
from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

from TrigT2CaloCommon.CaloDef import clusterFSInputMaker
topSequence += clusterFSInputMaker()

from TriggerMenuMT.HLTMenuConfig.Jet.JetRecoSequences import jetRecoSequence
from TrigUpgradeTest.jetMenuHelper import jetDictFromString
from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import RecoFragmentsPool

jetRecoDict=jetDictFromString("a4_tc_em_subjes")
(jetSequence, jetsKey) = RecoFragmentsPool.retrieve( jetRecoSequence, None, 
                                                     dataSource="data", RoIs="FullScanRoIs", **jetRecoDict )



topSequence += jetSequence

#################################################
# Add EFMissingETFrom** algorithm
#################################################

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromJetsMT
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

metAlg = EFMissingETAlgMT( name="EFMET" )

mhtTool = EFMissingETFromJetsMT( name="METFromJetsTool" )
mhtTool.JetsCollection = jetsKey

metAlg.METTools=[ mhtTool ]
metAlg.METContainerKey="HLT_MET_mht"
metAlg.MonTool = getMETMonTool()

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

hypoAlg.OutputLevel = DEBUG
hypoAlg.HypoInputDecisions = "L1MET"
hypoAlg.HypoOutputDecisions = "EFMETDecisions"
topSequence += hypoAlg

