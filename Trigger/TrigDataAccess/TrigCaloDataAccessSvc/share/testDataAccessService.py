#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

#viewTest = opt.enableViews   # from testHLT_MT.py
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram

from TrigCaloDataAccessSvc.TrigCaloDataAccessSvcConf import TrigCaloDataAccessSvc, TestCaloDataAccess
import math
mon = GenericMonitoringTool("CaloDataAccessSvcMon")
mon.Histograms += [defineHistogram( "TIME_locking_LAr_RoI", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiROBs_LAr", title="Number of ROBs unpacked in RoI requests", xbins=20, xmin=0, xmax=20 ),
                   defineHistogram( "TIME_locking_LAr_FullDet", title="Time spent in unlocking the LAr collection", xbins=100, xmin=0, xmax=100 ),
                   defineHistogram( "roiEta_LAr,roiPhi_LAr", type="TH2F", title="Geometric usage", xbins=50, xmin=-5, xmax=5, ybins=64, ymin=-math.pi, ymax=math.pi )]

svcMgr += TrigCaloDataAccessSvc()
#svcMgr.TrigCaloDataAccessSvc.OutputLevel=DEBUG
svcMgr.TrigCaloDataAccessSvc.OutputLevel=ERROR
svcMgr.TrigCaloDataAccessSvc.MonTool = mon


algo = TestCaloDataAccess(emulateFixedRoIs=True,emulateRoIs=False,nFixedRoIs=4)
#algo.OutputLevel=DEBUG
algo.OutputLevel=ERROR


#topSequence += algo
from TrigUpgradeTest.TestUtils import L1DecoderTest
l1DecoderTest=L1DecoderTest()

from TrigCaloDataAccessSvc.TrigCaloDataAccessSvcConf import HLTCaloCellMaker, HLTCaloCellSumMaker
algo1=HLTCaloCellMaker("testFastAlgo")
algo1.RoIs="StoreGateSvc+EMRoIs"
algo1.OutputLevel=VERBOSE
topSequence += algo1
algo2=HLTCaloCellSumMaker("testSumFastAlgo")
algo2.OutputLevel=VERBOSE
topSequence += algo2


