#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
OutputLevel=WARNING
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
svcMgr.TrigCaloDataAccessSvc.OutputLevel=WARNING

from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
cellMakerAlgo =  HLTCaloCellMaker("CellMakerMT")
cellMakerAlgo.roiMode=True
cellMakerAlgo.RoIs="FullScanRoIs" 

cellMakerAlgo.OutputLevel=WARNING
cellMakerAlgo.CellsName="cells"
topSequence += cellMakerAlgo

#################################################
# Add EFMissingETFrom** algorithm
#################################################

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromCellsMT, EFMissingETFromHelper

cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
cellTool.CellsCollection = "cells"

helperTool = EFMissingETFromHelper("theHelperTool") 

metAlg = EFMissingETAlgMT( name="EFMET" )


metAlg.METTools=[ cellTool ]
metAlg.HelperTool= helperTool 
metAlg.OutputLevel=WARNING

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
for t in hypoAlg.HypoTools:
    t.OutputLevel=VERBOSE
hypoAlg.METContainerKey="HLT_MET"

# Not sure how to implement monitoring at the moment. 
# Left here in case will be useful.
# Will be removed if not
'''
from TrigMissingETHypo.TrigMissingETHypoMonitoringTool import TrigMissingETHypoMonitoringTool
hypoMon = TrigMissingETHypoMonitoringTool()
hypoAlg.onlineMonitoring()
'''

hypoAlg.OutputLevel=DEBUG
hypoAlg.HypoInputDecisions = "L1MET"
hypoAlg.HypoOutputDecisions = "EFMETDecisions"
topSequence += hypoAlg

