# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
from LArGeoAlgsNV.LArGMConfig import LArGMCfg


def DetDescrConfig(configFlags=None):
    from DetDescrCnvSvc.DetDescrCnvSvcConf import DetDescrCnvSvc
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    acc=ComponentAccumulator()
    detDescrCnvSvc=DetDescrCnvSvc(IdDictName = "IdDictParser/ATLAS_IDS.xml",IdDictFromRDB = False,DecodeIdDict = True)
    acc.addService(detDescrCnvSvc)
    acc.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[detDescrCnvSvc.getName(),])) #No service handle yet???
    return acc

def LArRawDataReadingCfg(configFlags):
    acc=ComponentAccumulator()
    #acc.merge(LArGMCfg(configFlags)) #Needed for identifier helpers
    acc.merge(DetDescrConfig())
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
    #acc.merge(LArOnOffIdMappingCfg(ConfigFlags))
    #acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels",))

    #acc.getService("IOVDbSvc").OutputLevel=VERBOSE

    f=open("LArRawDataReading.pkl","w")
    acc.store(f)
    f.close()
