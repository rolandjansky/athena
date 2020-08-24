# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Job option fragment to merge input bytestream with HLT result
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf
from TriggerJobOpts.TriggerFlags import TriggerFlags

acf.BSRDOOutput='AppName=Athena, OutputDirectory=./'

from ByteStreamCnvSvc import WriteByteStream
StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")

# BS content definition
if TriggerFlags.doHLT():
    StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_HLT" ]
else:
    StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_L2" ]
    StreamBSFileOutput.ItemList   += [ "HLT::HLTResult#HLTResult_EF" ]


from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMergeOutputSvc
mergeSvc=ByteStreamMergeOutputSvc(ByteStreamOutputSvc='ByteStreamEventStorageOutputSvc',
                                  ByteStreamInputSvc='ByteStreamInputSvc',
                                  overWriteHeader=True)

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr+=mergeSvc

StreamBSFileOutput.OutputFile = "ByteStreamMergeOutputSvc"
svcMgr.ByteStreamCnvSvc.ByteStreamOutputSvcList=['ByteStreamMergeOutputSvc']
