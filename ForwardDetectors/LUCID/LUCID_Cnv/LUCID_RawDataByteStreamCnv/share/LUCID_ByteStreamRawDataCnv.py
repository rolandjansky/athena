
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from LUCID_RawDataByteStreamCnv.LUCID_RawDataByteStreamCnvConf import LUCID_ByteStreamRawDataCnv
job += LUCID_ByteStreamRawDataCnv( "LUCID_ByteStreamRawDataCnv" )   

job.LUCID_ByteStreamRawDataCnv = INFO
