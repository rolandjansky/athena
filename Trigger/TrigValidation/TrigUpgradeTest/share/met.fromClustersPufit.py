#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc

svcMgr += TrigCaloDataAccessSvc()

from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()


#################################################
# Get topoclusters
#################################################
from TrigT2CaloCommon.CaloDef import HLTFSTopoRecoSequence
clusterSequence, clusterContainer = HLTFSTopoRecoSequence( RoIs="FullScanRoIs" )
topSequence += clusterSequence 

#################################################
# Add EFMissingETFrom** algorithm
#################################################

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromClustersPufitMT
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

clusterPufitTool = EFMissingETFromClustersPufitMT( name="METFromClustersPufitTool" )
clusterPufitTool.ClustersCollection = clusterContainer

metAlg = EFMissingETAlgMT( name="EFMET" )
metAlg.METTools=[ clusterPufitTool ]
metAlg.METContainerKey = "HLT_MET_ClustersPufit"
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
hypoAlg.HypoInputDecisions = "HLTNav_L1MET"
hypoAlg.HypoOutputDecisions = "EFMETDecisions"
topSequence += hypoAlg

