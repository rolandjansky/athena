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
    return result,None


def LArOnOffIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArOnOffMappingAlg,"/LAR/Identifier/OnOffIdMap")

def LArFebRodMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArFebRodMappingAlg,"/LAR/Identifier/FebRodMap")

def LArCalibIdMappingCfg(configFlags):
    return _larCablingCfg(configFlags,LArCalibLineMappingAlg,"/LAR/Identifier/CalibIdMap")


if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

    acc = LArOnOffIdMappingCfg( ConfigFlags )[0]
    acc.merge(LArFebRodMappingCfg(ConfigFlags)[0])
    acc.merge(LArCalibIdMappingCfg(ConfigFlags)[0])
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
