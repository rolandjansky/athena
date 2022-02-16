# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg, addFolders
LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg,LArLATOMEMappingAlg=CompFactory.getComps("LArOnOffMappingAlg","LArFebRodMappingAlg","LArCalibLineMappingAlg","LArLATOMEMappingAlg")

def _larCablingCfg(configFlags,algo,folder,algName=None):
    result=ComponentAccumulator()

    result.merge(IOVDbSvcCfg(configFlags))

    #MC folder-tag hack (See also ATCONDDB-49)
    tagsperFolder={"/LAR/Identifier/OnOffIdMap":"LARIdentifierOnOffIdMap-012",
                   "/LAR/Identifier/FebRodMap":"LARIdentifierFebRodMap-005",
                   "/LAR/Identifier/CalibIdMap":"LARIdentifierCalibIdMap-012",
                   }

    if configFlags.Input.isMC:
        db='LAR_OFL'
        if folder in tagsperFolder:
            ft=tagsperFolder[folder]
            folderwithtag=folder+"<tag>"+ft+"</tag>"
    else:
        db='LAR_ONL'
        folderwithtag=folder

    if algName is None:
        result.addCondAlgo(algo(ReadKey=folder),primary=True)
    else:
        result.addCondAlgo(algo(name=algName, ReadKey=folder),primary=True)
    result.merge(addFolders(configFlags,folderwithtag,className="AthenaAttributeList",detDb=db))
    return result

def _larLatomeCfg(configFlags,algo,folder,outkey):
    result=ComponentAccumulator()

    result.merge(IOVDbSvcCfg(configFlags))

    #MC folder-tag hack 
    tagsperFolder={"/LAR/IdentifierSC/LatomeMapping":"LARIdentifierSCLatomeMapping-UPD1-00"
                   }

    if configFlags.Input.isMC:
        db='LAR_OFL'
        if folder in tagsperFolder:
            ft=tagsperFolder[folder]
            folderwithtag=folder+"<tag>"+ft+"</tag>"
        else:    
            folderwithtag=folder
    else:
        db='LAR_ONL'
        folderwithtag=folder

    result.addCondAlgo(algo(ReadKey=folder,WriteKey=outkey),primary=True)
    result.merge(addFolders(configFlags,folderwithtag,className="CondAttrListCollection",detDb=db))
    #print (result)
    return result


def LArOnOffIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/Identifier/OnOffIdMap")

def LArOnOffIdMappingSCCfg(configFlags):
    result = ComponentAccumulator()
    if configFlags.Input.isMC:
       result.merge(_larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/IdentifierOfl/OnOffIdMap_SC","LArOnOffMappingAlgSC"))
       from IOVDbSvc.IOVDbSvcConfig import addOverride
       result.merge(addOverride(configFlags, "/LAR/IdentifierOfl/OnOffIdMap_SC", "LARIdentifierOflOnOffIdMap_SC-000")) # FIXME temporary?
    else:
       result.merge(_larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/Identifier/OnOffIdMap_SC","LArOnOffMappingAlgSC"))
    result.getCondAlgo("LArOnOffMappingAlgSC").WriteKey = "LArOnOffIdMapSC"
    result.getCondAlgo("LArOnOffMappingAlgSC").isSuperCell = True
    return result

def LArFebRodMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArFebRodMappingAlg,"/LAR/Identifier/FebRodMap")

def LArCalibIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArCalibLineMappingAlg,"/LAR/Identifier/CalibIdMap")

def LArCalibIdMappingSCCfg(configFlags):
    result = ComponentAccumulator()
    if not configFlags.Input.isMC:
       result.merge(_larCablingCfg(configFlags,LArCalibLineMappingAlg,"/LAR/Identifier/CalibIdMap_SC","LArCalibLineMappingAlgSC"))
       result.getCondAlgo("LArCalibLineMappingAlgSC").WriteKey="LArCalibIdMapSC"
       result.getCondAlgo("LArCalibLineMappingAlgSC").isSuperCell=True
       result.getCondAlgo("LArCalibLineMappingAlgSC").MaxCL=16
       return result

def LArLATOMEMappingCfg(configFlags):
    if not configFlags.Input.isMC:
       return _larLatomeCfg(configFlags,LArLATOMEMappingAlg,"/LAR/Identifier/LatomeMapping","LArLATOMEMap")

def LArIdMapCfg(configFlags):
    """Return ComponentAccumulator configured with Identifier Map in POOL/COOL"""
    # replaces LArIdMap_MC_jobOptions.py or LArIdMap_comm_jobOptions.py
    result = LArOnOffIdMappingCfg(configFlags)
    result.merge(LArFebRodMappingCfg(configFlags))
    result.merge(LArCalibIdMappingCfg(configFlags))
    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = LArOnOffIdMappingCfg( ConfigFlags )
    acc.merge(LArFebRodMappingCfg(ConfigFlags))
    acc.merge(LArCalibIdMappingCfg(ConfigFlags))
    acc.store( open( "test.pkl", "wb" ) )
    print ("All OK")
