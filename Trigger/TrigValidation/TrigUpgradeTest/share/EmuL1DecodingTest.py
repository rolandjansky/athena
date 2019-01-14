#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

import os.path

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob
#import AthenaCommon.AtlasThreadedJob

#include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#Determine whether we're running in threaded mode (threads= >=1)
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
nThreads = jp.ConcurrencyFlags.NumThreads()

if nThreads >= 1:
  ## get a handle on the Scheduler
  from AthenaCommon.AlgScheduler import AlgScheduler
  AlgScheduler.CheckDependencies( True )

# Use McEventSelector so we can run with AthenaMP
#import AthenaCommon.AtlasUnixGeneratorJob

## get a handle on the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

    
from L1Decoder.L1DecoderConf import CTPUnpackingTool, EMRoIsUnpackingTool, L1Decoder, MURoIsUnpackingTool
from L1Decoder.L1DecoderConf import CTPUnpackingEmulationTool, RoIsUnpackingEmulationTool
l1Decoder = L1Decoder( OutputLevel=DEBUG )
l1Decoder.RoIBResult=""
l1Decoder.prescaler.EventInfo=""

data = {}
data['CTPEmulation'] = ['HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4', 
                        'HLT_e3 HLT_g5 HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4', 
                        'HLT_e3 HLT_g5 HLT_e7 HLT_2e3 HLT_mu6 HLT_2mu6', 
                        'HLT_mu6 HLT_2mu6 HLT_mu6idperf HLT_e15mu4', ]   # just to see some change   
data['RoIEmulation'] = ['1.3,2.9,2704088841,EM3,EM7; 1.2,3.1,2972524297,EM3,EM7,EM10,EM15; -3.2,-2.0,3103727387,MU0,MU4,MU6,MU8',
                        '1.2,1.9,2733969453,MU0,MU4,MU6,MU8,MU10;2.2,1.0,2733969453,MU0,MU4,MU6',
                        '-3.2,3.0,2704088841,MU0,MU4,MU6,MU8;3.0,1.6,2972524297,MU0,MU4',
                        '1.3,1.9,3103727387,MU0,MU10;1.2,2.6,2733969453,MU6;-1.1,2.6,2972524297,MU6; -1.2,2.6,2704088842,MU20']

from TrigUpgradeTest.TestUtils import writeEmulationFiles
writeEmulationFiles(data)

ctpUnpacker = CTPUnpackingEmulationTool( OutputLevel =  DEBUG, ForceEnableAllChains=True )

l1Decoder.ctpUnpacker = ctpUnpacker


emUnpacker = RoIsUnpackingEmulationTool("EMRoIsUnpackingTool",
                                        Decisions = "EMRoIDecisions",
                                        OutputLevel=DEBUG )
emUnpacker.ThresholdToChainMapping = ["EM3 : HLT_e3", "EM3 : HLT_g5",  "EM7 : HLT_e7", "EM15 : HLT_e15mu4" ]

muUnpacker = RoIsUnpackingEmulationTool("MURoIsUnpackingTool", 
                                        Decisions = "MURoIDecisions",
                                        OutputLevel=DEBUG )
muUnpacker.ThresholdToChainMapping = ["MU6 : HLT_mu6", "MU6 : HLT_mu6idperf", "MU4 : HLT_e15mu4"] 
# do not know yet how to configure the services for it

l1Decoder.roiUnpackers = [emUnpacker]
l1Decoder.L1DecoderSummaryKey = "L1DecoderSummary"
topSequence += l1Decoder
#Run calo decoder

from DecisionHandling.DecisionHandlingConf import DumpDecisions
emDecisionsDumper = DumpDecisions("DumpEML1RoIs", OutputLevel=DEBUG)
emDecisionsDumper.Decisions = "EMRoIDecisions"
topSequence += emDecisionsDumper

chainSeedingDumper = DumpDecisions("ChainSeedingDumper", OutputLevel=DEBUG)
chainSeedingDumper.Decisions = "L1DecoderSummary"
topSequence += chainSeedingDumper





#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=False #This is required to avoid a bug in bytestream decoding in AthenaMT mode
# svcMgr.ByteStreamAddressProviderSvc.OutputLevel = INFO
# svcMgr.ByteStreamCnvSvc.OutputLevel = INFO
# svcMgr.ByteStreamInputSvc.OutputLevel = INFO
svcMgr.AthDictLoaderSvc.OutputLevel = INFO
svcMgr.EventPersistencySvc.OutputLevel = INFO
#svcMgr.ROBDataProviderSvc.OutputLevel = INFO
print svcMgr

theApp.EvtMax = len(data['RoIEmulation'])

print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
