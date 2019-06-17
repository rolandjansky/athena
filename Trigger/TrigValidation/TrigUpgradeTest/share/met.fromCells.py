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

from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

from TrigCaloRec.TrigCaloRecConfig import HLTCaloCellMaker
cellMakerAlgo =  HLTCaloCellMaker("CellMakerMT")
cellMakerAlgo.roiMode=True
cellMakerAlgo.RoIs="FullScanRoIs" 

cellMakerAlgo.CellsName="cells"
topSequence += cellMakerAlgo

#################################################
# Add EFMissingETFrom** algorithm
#################################################

from TrigEFMissingET.TrigEFMissingETConf import EFMissingETAlgMT, EFMissingETFromCellsMT, EFMissingETFlagsMT
from TrigEFMissingET.TrigEFMissingETMTConfig import getMETMonTool

cellTool = EFMissingETFromCellsMT( name="METFromCellsTool" )
cellTool.CellsCollection = "cells"

flagTool = EFMissingETFlagsMT("theFlagsTool") 

metAlg = EFMissingETAlgMT( name="EFMET" )
metAlg.METContainerKey="HLT_MET"
metAlg.METTools=[ cellTool, flagTool ]
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

