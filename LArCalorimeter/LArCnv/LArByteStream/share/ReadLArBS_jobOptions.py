# jobOption fragment for Accessing LArRawChannel from ByteStream 
# DLLs 

from LArROD.LArRODConf import ReadLArRaw

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
readLArRaw = ReadLArRaw()
topSequence += readLArRaw
readLArRaw.LArRawChannelContainerName = "LArRawChannels" 
readLArRaw.DumpFile="lar_rawchannel.txt"
