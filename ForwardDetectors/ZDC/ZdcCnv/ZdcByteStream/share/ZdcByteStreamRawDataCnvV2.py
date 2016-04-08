from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamRawDataV2
job += ZdcByteStreamRawDataV2( "ZdcByteStreamRawDataV2" )

job.ZdcByteStreamTester = DEBUG
