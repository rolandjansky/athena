#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

isData = False
if globalflags.InputFormat.is_bytestream():
  isData = True

# ----------------------------------------------------------------
# Setup Views
# ----------------------------------------------------------------
from AthenaCommon.AlgSequence import AthSequencer

if TriggerFlags.doCalo:

  if ( True ) :
     from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
    
     from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
     import math
     mon = GenericMonitoringTool("CaloDataAccessSvcMon")
     mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", path="EXPERT", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                      defineHistogram( "roiROBs_LAr", path="EXPERT", title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                      defineHistogram( "TIME_locking_LAr_FullDet", path="EXPERT", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                      defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", path="EXPERT", title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-math.pi, ymax=math.pi )]
    
     svcMgr += TrigCaloDataAccessSvc()
     svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR
     svcMgr.TrigCaloDataAccessSvc.MonTool = mon
    
    
    
     #topSequence += algo
     #from TrigUpgradeTest.TestUtils import L1DecoderTest
     #l1DecoderTest=L1DecoderTest()
     #topSequence+=l1DecoderTest
    
     from TrigCaloRec.TrigCaloRecConf import HLTCaloCellMaker, HLTCaloCellSumMaker
     algo1=HLTCaloCellMaker("testFastAlgo1")
     algo1.RoIs="StoreGateSvc+EMRoIs"
     algo1.TrigDataAccessMT=svcMgr.TrigCaloDataAccessSvc
     #algo1.roiMode=False
     algo1.OutputLevel=VERBOSE
     topSequence += algo1
     algo2=HLTCaloCellSumMaker("testSumFastAlgo")
     algo2.OutputLevel=VERBOSE
     #algo2.roiMode=False
     topSequence += algo2



  svcMgr.ToolSvc.TrigDataAccess.ApplyOffsetCorrection=False
  
  from TrigT2CaloEgamma.TrigT2CaloEgammaConfig import T2CaloEgamma_ReFastAlgo
  algo=T2CaloEgamma_ReFastAlgo("testReFastAlgo")
  algo.OutputLevel=VERBOSE

  algo.RoIs="StoreGateSvc+EMRoIs"
  topSequence += algo

