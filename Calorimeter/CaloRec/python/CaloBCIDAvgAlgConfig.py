# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDAvgAlgConfig.py
# Created: Mar 2019, sss
# Purpose: Configure CaloBCIDAvgAlg.

from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloBCIDAvgAlgCfg (flags):
    from IOVDbSvc.IOVDbSvcConfig import addFolders
    from CaloRec.CaloRecConf import CaloBCIDAvgAlg

    result = ComponentAccumulator()

    from LArRecUtils.LArRecUtilsConfig import LArMCSymCondAlgCfg
    result.merge (LArMCSymCondAlgCfg (flags))

    if flags.Input.isMC is False:
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        result.merge (LuminosityCondAlgCfg (flags))
        lumiAlg = result.getCondAlgo ('LuminosityCondAlg')

        if flags.Common.isOnline:
            result.merge(addFolders(flags, ['/LAR/LArPileup/LArPileupShape<key>LArShape32</key>', 
                                            '/LAR/LArPileup/LArPileupAverage'], 'LAR_ONL'))
        else:
            result.merge(addFolders(flags, ['/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>',
                                            '/LAR/ElecCalibOfl/LArPileupAverage'], 'LAR_OFL'))

        alg = CaloBCIDAvgAlg (isMC = False,
                              LuminosityCondDataKey = lumiAlg.LuminosityOutputKey,
                              ShapeKey = 'LArShape32')

    else:
        from LArRecUtils.LArRecUtilsConfig import \
            LArADC2MeVCondAlgCfg, LArOFCCondAlgCfg, LArAutoCorrTotalCondAlgCfg
        result.merge (LArADC2MeVCondAlgCfg (flags))
        result.merge (LArOFCCondAlgCfg (flags))
        result.merge (LArAutoCorrTotalCondAlgCfg (flags))

        # FIXME: Convert to new config  It's also a public tool.
        from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
        theBunchCrossingTool = BunchCrossingTool()

        result.merge(addFolders(flags, ['/LAR/ElecCalibMC/Shape',
                                        '/LAR/ElecCalibMC/LArPileupAverage'], 'LAR_OFL'))

        alg = CaloBCIDAvgAlg (isMC = True,
                              BunchCrossingTool = theBunchCrossingTool,
                              ShapeKey = 'LArShape')

    result.addEventAlgo (alg)
    return result



if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags (quiet = True)

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloBCIDAvgAlgCfg (flags1)
    acc1.printConfig(summariseProps=True)
    acc1.wasMerged()

    print ('--- mc')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.RAW
    flags2.Input.isMC = True
    flags2.lock()
    acc2 = CaloBCIDAvgAlgCfg (flags2)
    acc2.printConfig(summariseProps=True)
    acc2.wasMerged()

    print ('--- online')
    flags3 = ConfigFlags.clone()
    flags3.Input.Files = defaultTestFiles.RAW
    flags3.Common.isOnline = True
    flags3.lock()
    acc3 = CaloBCIDAvgAlgCfg (flags3)
    acc3.printConfig(summariseProps=True)
    acc3.wasMerged()
