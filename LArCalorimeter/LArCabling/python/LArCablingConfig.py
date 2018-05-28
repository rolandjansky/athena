# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from LArRecUtils.LArRecUtilsConf import LArOnOffMappingAlg, LArFebRodMappingAlg, LArCalibLineMappingAlg

def _larCablingCfg(configFlags,algo,folder):
    result=ComponentAccumulator()

    #MC folder-tag hack (See also ATCONDDB-49)
    tagsperFolder={"/LAR/Identifier/OnOffIdMap":"LARIdentifierOnOffIdMap-012",
                   "/LAR/Identifier/FebRodMap":"LARIdentifierFebRodMap-005",
                   "/LAR/Identifier/CalibIdMap":"LARIdentifierCalibIdMap-012"
                   }

    if configFlags.get("global.isMC"):
        db='LAR_OFL'
        if folder in tagsperFolder:
            ft=tagsperFolder[folder]
            folderwithtag=folder+"<tag>"+ft+"</tag>"
    else:
        db='LAR'
        folderwithtag=folder

    result.addCondAlgo(algo(ReadKey=folder))
    result.addConfig(addFolders,configFlags,folderwithtag,className="AthenaAttributeList",detDb=db)
    return result


def LArOnOffIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/Identifier/OnOffIdMap")

def LArFebRodMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArFebRodMappingAlg,"/LAR/Identifier/FebRodMap")

def LArCalibIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArCalibLineMappingAlg,"/LAR/Identifier/CalibIdMap")


