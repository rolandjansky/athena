#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import math
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

mon = GenericMonitoringTool("CaloDataAccessSvcMon")
mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiROBs_LAr", title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                   defineHistogram( "TIME_locking_LAr_FullDet", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-math.pi, ymax=math.pi )]

svcMgr += TrigCaloDataAccessSvc()
svcMgr.TrigCaloDataAccessSvc.MonTool = mon
svcMgr.TrigCaloDataAccessSvc.OutputLevel=INFO

from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker
cellMakerAlgo =  HLTCaloCellMaker("CellMakerMT", roiMode=True)
cellMakerAlgo.RoIs="FullScanRoIs" 

cellMakerAlgo.OutputLevel=VERBOSE
cellMakerAlgo.CellsName="cells"
topSequence += cellMakerAlgo

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromCellsMT


cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
cellTool.CellsCollection = "cells"

metAlg = EFMissingETAlgMT( name="EFMET" )


metAlg.METTools=[ cellTool ]
#metAlg.OutputLevel=DEBUG
metMon = GenericMonitoringTool("METMonTool")
metMon.Histograms = [ defineHistogram( "TIME_Total", title="Time spent Alg", xbins=100, xmin=0, xmax=100 ),
                      defineHistogram( "TIME_Loop", title="Time spent in Tools loop", xbins=100, xmin=0, xmax=100 )]
from TrigEFMissingET.TrigEFMissingETMonitoring import *
metMon.Histograms  = [ hEx_log, hEy_log, hEz_log, hMET_log, hSumEt_log ]
metMon.Histograms += [ hMET_lin, hSumEt_lin ]
metMon.Histograms += [ hXS, hMETPhi, hMETStatus]
metMon.Histograms += [ hCompEx, hCompEy, hCompEz, hCompEt, hCompSumEt, hCompSumE ]
metMon.Histograms += [ hCompEt_lin, hCompSumEt_lin ]

metAlg.MonTool = metMon
#metAlg += cellTool
topSequence += metAlg

#   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
  
# #  clusterMakerAlgo = TrigCaloClusterMakerMT_topo(name="ClustermakerMT_topo",cells="cells",doMoments=True,doLC=True)
# #  Get conditions error using TrigLocalCalib tool, so set doLC=False
#   clusterMakerAlgo = TrigCaloClusterMakerMT_topo(name="ClustermakerMT_topo",cells="cells",doMoments=True,doLC=False)
#   clusterMakerAlgo.OutputLevel=VERBOSE
#   topSequence += clusterMakerAlgo
  
# print ToolSvc


