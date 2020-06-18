# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# python fragment to configure LAr HV conditions chain

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg,addFolders

def LArHVDBCfg(configFlags):

   result=ComponentAccumulator()
   result.merge(IOVDbSvcCfg(configFlags))

   isMC=configFlags.Input.isMC
   isOnline=configFlags.Common.isOnline

   if not isMC and not isOnline:
      result.merge(addFolders(configFlags,["/LAR/DCS/HV/BARREl/I16","/LAR/DCS/HV/BARREL/I8"],detDb="DCS_OFL",className="CondAttrListCollection"))
      result.merge(addFolders(configFlags,["/LAR/IdentifierOfl/HVLineToElectrodeMap","/LAR/HVPathologiesOfl/Pathologies"],detDb="LAR_OFL",className="AthenaAttributeList"))
 
      from LArRecUtils.LArRecUtilsConf import LArHVIdMappingAlg
      result.addCondAlgo(LArHVIdMappingAlg(ReadKey="/LAR/IdentifierOfl/HVLineToElectrodeMap",WriteKey="LArHVIdMap"))
 
      from LArRecUtils.LArRecUtilsConf import LArHVPathologyDbCondAlg
      result.addCondAlgo(LArHVPathologyDbCondAlg(PathologyFolder="/LAR/HVPathologiesOfl/Pathologies",HVMappingKey="LArHVIdMap", HVPAthologyKey="LArHVPathology"))
 
      from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
      result.addCondAlgo(LArHVCondAlg(HVPathologies="LArHVPathology",OutputHVData="LArHVData"))
 
      from LArRecUtils.LArRecUtilsConf import LArHVScaleCorrCondAlg
      hvscale = LArHVScaleCorrCondAlg(keyHVdata="LArHVData",keyOutputCorr="LArHVScaleCorrRecomputed")
      hvscale.UndoOnlineHVCorr=True
      result.addCondAlgo(hvscale)

   if isMC:
      result.merge(addFolders(configFlags,detDb="LAR_OFL",folders=["/LAR/Identifier/HVLineToElectrodeMap<tag>LARHVLineToElectrodeMap-001</tag>"],className="AthenaAttributeList"))

   return result

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    cfg=ComponentAccumulator()
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    cfg.merge( ByteStreamReadCfg(ConfigFlags) )

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
 

    cfg.merge(LArHVDBCfg(ConfigFlags))
    f=open("LArHVDBConfig.pkl","wb")
    cfg.store(f)
    f.close()
