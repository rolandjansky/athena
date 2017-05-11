#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#

#
# get_files LVL1config_Physics_pp_v5.xml
# ln -s /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/atn-test/data15_13TeV.00266904.physics_EnhancedBias.merge.RAW._lb0452._SFO-1._0001.1 input.data
# exact config for this data is: https://atlas-trigconf.cern.ch/run2/smkey/2142/l1key/1077/hltkey/765/

import os.path
assert os.path.isfile('input.data'), 'No input file: see the JO to see how to get it'

## @file L1Topo_ReadBS_test.py
## @brief Example job options file to read BS file to test a converter
## $Id: decodeBS.parallel.py 717359 2016-01-12 14:40:21Z bwynne $
###############################################################
#
# This Job option:
# ----------------
# 1. Read ByteStream test data file and decode the L1Topo part
#
#==============================================================

## basic job configuration
import AthenaCommon.AtlasUnixStandardJob
#import AthenaCommon.AtlasThreadedJob

include( "ByteStreamCnvSvc/BSEventStorageEventSelector_jobOptions.py" )

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

# older file, has wrong ROD id: svcMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/m/mzinser/public/l1calo-event.sim"]
#svcMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/m/mzinser/public/InputSimon/Mode7/l1calo-event.sim" ]
svcMgr.ByteStreamInputSvc.FullFileName = [ "./input.data" ]
#svcMgr.ByteStreamInputSvc.FullFileName = [ "/afs/cern.ch/user/s/sgeorge/atlaspublic/L1TopoCnv/mergedsim._0001.data" ]

svcMgr.ByteStreamInputSvc.ValidateEvent = True
svcMgr.EventSelector.ProcessBadEvent = True



if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
    from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
    svcMgr += ByteStreamAddressProviderSvc()


from TrigConfigSvc.TrigConfigSvcConf import TrigConf__LVL1ConfigSvc
l1svc = TrigConf__LVL1ConfigSvc("LVL1ConfigSvc")
l1svc.XMLMenuFile = "LVL1config_Physics_pp_v5.xml"
svcMgr += l1svc


# This is the list of proxies to set up so that retrieval attempt will trigger the BS conversion
svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
    "ROIB::RoIBResult/RoIBResult" ]

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm" top algorithms to be run, and the libraries that house them


if nThreads >= 1:
  #Retrieve input data
  from SGComps.SGCompsConf import SGInputLoader
  topSequence += SGInputLoader( OutputLevel=INFO, ShowEventDump=False )
  topSequence.SGInputLoader.Load = [ ('ROIB::RoIBResult','RoIBResult') ]

from L1Decoder.L1DecoderConf import CTPUnpackingTool, EMRoIsUnpackingTool, L1Decoder, MURoIsUnpackingTool
l1Decoder = L1Decoder( OutputLevel=DEBUG )
l1Decoder.ctpUnpacker = CTPUnpackingTool( OutputLevel =  DEBUG, ForceEnableAllChains=True )

l1Decoder.ctpUnpacker.CTPToChainMapping = ["0:HLT_e3",  "0:HLT_g5", "1:HLT_e7", "15:HLT_mu6", "33:HLT_2mu6", "15:HLT_mu6idperf", "42:HLT_e15mu4"] # this are real IDs of L1_* items in pp_v5 menu

emUnpacker = EMRoIsUnpackingTool( OutputLevel=DEBUG )
emUnpacker.ThresholdToChainMapping = ["EM3 : HLT_e3", "EM3 : HLT_g5",  "EM7 : HLT_e7", "EM15 : HLT_e15mu4" ]


muUnpacker = MURoIsUnpackingTool( OutputLevel=DEBUG )
muUnpacker.ThresholdToChainMapping = ["MU6 : HLT_mu6", "MU6 : HLT_mu6idperf", "MU4 : HLT_e15mu4"] 
# do not know yet how to configure the services for it

l1Decoder.roiUnpackers = [emUnpacker]
l1Decoder.Chains="HLTChainsResult"
topSequence += l1Decoder
#Run calo decoder

from DecisionHandling.DecisionHandlingConf import DumpDecisions
emDecisionsDumper = DumpDecisions("DumpEML1RoIs", OutputLevel=DEBUG)
emDecisionsDumper.Decisions = "EMRoIDecisions"
topSequence += emDecisionsDumper

chainSeedingDumper = DumpDecisions("ChainSeedingDumper", OutputLevel=DEBUG)
chainSeedingDumper.Decisions = "HLTChainsResult"
topSequence += chainSeedingDumper



# caloDecoder = L1CaloDecoder() # by default it is steered towards the RoIBResult of the name above
# caloDecoder.OutputLevel=VERBOSE
# topSequence += caloDecoder

# #Dumper
# from ViewAlgs.ViewAlgsConf import DumpDecisions
# dumper = DumpDecisions("L1CaloDecisions")
# dumper.OutputLevel=VERBOSE
# topSequence += dumper


#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = INFO
svcMgr.MessageSvc.Format = "% F%42W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.verboseLimit = 0
svcMgr.StoreGateSvc.OutputLevel = INFO
svcMgr.StoreGateSvc.Dump=False #This is required to avoid a bug in bytestream decoding in AthenaMT mode
svcMgr.ByteStreamAddressProviderSvc.OutputLevel = INFO
svcMgr.ByteStreamCnvSvc.OutputLevel = INFO
svcMgr.ByteStreamInputSvc.OutputLevel = INFO
svcMgr.AthDictLoaderSvc.OutputLevel = INFO
svcMgr.EventPersistencySvc.OutputLevel = INFO
svcMgr.ROBDataProviderSvc.OutputLevel = INFO
print svcMgr


theApp.EvtMax = 100


print "topSequence dump:", topSequence
#
# End of job options file
#
###############################################################
