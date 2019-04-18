#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import math
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

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

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromClustersPufitMT, EFMissingETFromHelper

clusterTool = EFMissingETFromClustersPufitMT( name="METFromClustersPufitTool" )
clusterTool.ClustersCollection = clusterContainer

helperTool = EFMissingETFromHelper("theHelperTool") 
metAlg = EFMissingETAlgMT( name="EFMET" )


metAlg.METTools=[ clusterTool ]
metAlg.HelperTool= helperTool 
metAlg.METContainerKey = "HLT_MET_ClustersPufit"

metMon = GenericMonitoringTool("METMonTool")
metMon.Histograms = [ defineHistogram( "TIME_Total", path='EXPERT', title="Time spent Alg", xbins=100, xmin=0, xmax=100 ),
                      defineHistogram( "TIME_Loop", path='EXPERT', title="Time spent in Tools loop", xbins=100, xmin=0, xmax=100 )]
from TrigEFMissingET.TrigEFMissingETMonitoring import ( hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log, 
                                                 hMET_lin, hSumEt_lin, 
                                                 hXS, hMETPhi, hMETStatus,
                                                 hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE,
                                                 hCompEt_lin, hCompSumEt_lin )

metMon.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
metMon.Histograms += [ hMET_lin, hSumEt_lin ]
metMon.Histograms += [ hXS, hMETPhi, hMETStatus]
metMon.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
metMon.Histograms += [ hCompEt_lin, hCompSumEt_lin ]

metAlg.MonTool = metMon
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

