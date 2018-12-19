#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

include("TrigUpgradeTest/testHLT_MT.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

import math
from TrigT2CaloCommon.TrigT2CaloCommonConf import TrigCaloDataAccessSvc#, TestCaloDataAccess
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram


from L1Decoder.L1DecoderConf import CreateFullScanRoI
topSequence += CreateFullScanRoI()

from TrigUpgradeTest.metDefs import metCellRecoSequence
cellRecoSeq, METkey = metCellRecoSequence("FullScanRoIs")
topSequence += cellRecoSeq

from TrigMissingETHypo.TrigMissingETHypoConfigMT import MissingETHypoAlgMT, TrigMETHypoToolFromName, MissingETHypoToolMT

metHypoTool = MissingETHypoToolMT("METHypoTool")
metHypoTool.metThreshold=50

metHypoAlg = MissingETHypoAlgMT("METHypoAlg")
metHypoAlg.HypoTools = [metHypoTool]
metHypoAlg.METContainerKey="HLT_MET"
metHypoAlg.OutputLevel=DEBUG
metHypoAlg.HypoInputDecisions = "L1MET"
metHypoAlg.HypoOutputDecisions = "EFMETDecisions"

topSequence += metHypoAlg

