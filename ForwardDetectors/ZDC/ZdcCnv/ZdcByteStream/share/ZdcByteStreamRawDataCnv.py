from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from ZdcByteStream.ZdcByteStreamConf import ZdcByteStreamRawData
job += ZdcByteStreamRawData( "ZdcByteStreamRawData" )

job.ZdcByteStreamTester = DEBUG
