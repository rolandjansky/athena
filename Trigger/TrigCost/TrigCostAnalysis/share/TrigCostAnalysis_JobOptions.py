#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

#TODO: Reowrk into new-JO style. Similar to RatesAnalysisFullMenu.py

# Define top sequence and output sequence
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
topSequence = AlgSequence()

from AthenaConfiguration.AllConfigFlags import ConfigFlags
ConfigFlags.Input.Files = [\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-1._0001.data",\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-2._0001.data",\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-3._0001.data",\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-4._0001.data",\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-5._0001.data",\
"/eos/atlas/atlascerngroupdisk/trig-daq/validation/test_data/data20_test/data_test.00379158.calibration_CostMonitoring.daq.RAW/data_test.00379158.calibration_CostMonitoring.daq.RAW._lb0000._SFO-6._0001.data"\
]

# Use new-style config of ByteStream reading and import here into old-style JO
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
CAtoGlobalWrapper(ByteStreamReadCfg, ConfigFlags)

# Define the decoding sequence
from TrigHLTResultByteStream.TrigHLTResultByteStreamConf import HLTResultMTByteStreamDecoderAlg
from TrigOutputHandling.TrigOutputHandlingConf import TriggerEDMDeserialiserAlg
from AthenaCommon.CFElements import seqAND
decoder = HLTResultMTByteStreamDecoderAlg()
costDataDeserialiser = TriggerEDMDeserialiserAlg("CostDataTrigDeserialiser")

from TrigEDMConfig import DataScoutingInfo
costDataDeserialiser.ModuleID = DataScoutingInfo.DataScoutingIdentifiers["CostMonDS"]

decodingSeq = seqAND("Decoding")
decodingSeq += decoder
decodingSeq += costDataDeserialiser
topSequence += decodingSeq

# Define the TrigConfigSvc to place the menu JSON in the DetStore
# TODO: Configure to load from the DB.
from TrigConfigSvc.TrigConfigSvcConf import TrigConf__HLTConfigSvc
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
hltConfigSvc = TrigConf__HLTConfigSvc( "HLTConfigSvc" )
hltConfigSvc.ConfigSource = "None"
hltConfigSvc.XMLMenuFile = "None"
hltConfigSvc.InputType = "file"
hltConfigSvc.JsonFileName = "HLTMenu_9.json"
theApp.CreateSvc += [ "TrigConf::HLTConfigSvc/HLTConfigSvc" ]
ServiceMgr += hltConfigSvc

# Cost Monitoring
from EnhancedBiasWeighter.EnhancedBiasWeighterConf import EnhancedBiasWeighter
enhancedBiasWeighter = EnhancedBiasWeighter()
enhancedBiasWeighter.RunNumber = 379158
enhancedBiasWeighter.UseBunchCrossingTool = False

from AthenaCommon import CfgMgr
trigCostAnalysis = CfgMgr.TrigCostAnalysis()
trigCostAnalysis.OutputLevel = DEBUG
trigCostAnalysis.RootStreamName = "COSTSTREAM"
trigCostAnalysis.BaseEventWeight = 10.0
trigCostAnalysis.EnhancedBiasTool = enhancedBiasWeighter
trigCostAnalysis.UseEBWeights = False
trigCostAnalysis.MaxFullEventDumps = 100
trigCostAnalysis.FullEventDumpProbability = 1 # X. Where probability is 1 in X
topSequence += trigCostAnalysis

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'THistSvc'): 
  svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["COSTSTREAM DATAFILE='TrigCostRoot_Results.root' OPT='RECREATE'"]
