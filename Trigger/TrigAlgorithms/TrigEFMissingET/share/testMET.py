#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

isData = False
if globalflags.InputFormat.is_bytestream():
  isData = True



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

from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker
cellMakerAlgo =  HLTCaloCellMaker("CellMakerMT", roiMode=True)
cellMakerAlgo.RoIs="METRoIs" 
cellMakerAlgo.OutputLevel=VERBOSE
cellMakerAlgo.CellsName="cells"
topSequence += cellMakerAlgo

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromCellsMT


# cellTool = EFMissingETFromCellsMT( name="METFromCellsTool", CaloNoiseTool=ToolSvc.CaloNoiseToolDefault )
# cellTool.CellsCollection = "cells"

# metAlg = EFMissingETAlgMT( name="EFMET" )
# metAlg.METTools=[ cellTool ]
# metAlg += cellTool
# topSequence += metAlg

#   from TrigCaloRec.TrigCaloRecConfig import TrigCaloClusterMakerMT_topo
  
# #  clusterMakerAlgo = TrigCaloClusterMakerMT_topo(name="ClustermakerMT_topo",cells="cells",doMoments=True,doLC=True)
# #  Get conditions error using TrigLocalCalib tool, so set doLC=False
#   clusterMakerAlgo = TrigCaloClusterMakerMT_topo(name="ClustermakerMT_topo",cells="cells",doMoments=True,doLC=False)
#   clusterMakerAlgo.OutputLevel=VERBOSE
#   topSequence += clusterMakerAlgo
  
# print ToolSvc


