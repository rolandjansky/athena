# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg

def _larCablingCfg(configFlags,algo,folder):
    result=ComponentAccumulator()

    #MC folder-tag hack (See also ATCONDDB-49)
    tagsperFolder={"/LAR/Identifier/OnOffIdMap":"LARIdentifierOnOffIdMap-012",
                   "/LAR/Identifier/FebRodMap":"LARIdentifierFebRodMap-005",
                   "/LAR/Identifier/CalibIdMap":"LARIdentifierCalibIdMap-012",
                   "/LAR/IdentifierOfl/OnOffIdMap_SC":"LARIdentifierOflOnOffIdMap_SC-000"
                   }

    if configFlags.Input.isMC:
        db='LAR_OFL'
        if folder in tagsperFolder:
            ft=tagsperFolder[folder]
            folderwithtag=folder+"<tag>"+ft+"</tag>"
    else:
        db='LAR'
        folderwithtag=folder

    result.addCondAlgo(algo(ReadKey=folder))
    result.merge(addFolders(configFlags,folderwithtag,className="AthenaAttributeList",detDb=db))
    #print result
    return result


def LArOnOffIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/Identifier/OnOffIdMap")

def LArOnOffIdMappingSCCfg(configFlags):
    return _larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/IdentifierOfl/OnOffIdMap_SC")

def LArFebRodMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArFebRodMappingAlg,"/LAR/Identifier/FebRodMap")

def LArCalibIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArCalibLineMappingAlg,"/LAR/Identifier/CalibIdMap")


if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = LArOnOffIdMappingCfg( ConfigFlags )
    acc.merge(LArFebRodMappingCfg(ConfigFlags))
    acc.merge(LArCalibIdMappingCfg(ConfigFlags))
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
