from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
from LArGeoAlgsNV.LArGMConfig import LArGMCfg

def LArRawDataReadingCfg(configFlags):
    acc=ComponentAccumulator()
    acc.merge(LArGMCfg(configFlags)) #Needed for identifier helpers
    acc.merge(ByteStreamReadCfg(configFlags))    
    acc.addEventAlgo(LArRawDataReadingAlg())
    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.LAr.doAlign=False
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc=LArRawDataReadingCfg(ConfigFlags)
    
    from LArEventTest.LArEventTestConf import DumpLArRawChannels
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels",))

    #acc.getService("IOVDbSvc").OutputLevel=VERBOSE

    f=open("LArRawDataReading.pkl","w")
    acc.store(f)
    f.close()
