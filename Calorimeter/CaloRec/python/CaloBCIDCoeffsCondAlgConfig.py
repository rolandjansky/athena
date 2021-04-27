# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDCoeffsCondAlgConfig.py
# Created: Mar 2020, sss
# Purpose: Configure CaloBCIDCoeffsCondAlg.

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolderList


def CaloBCIDCoeffsCondAlgCfg (flags):
    result = ComponentAccumulator()

    from LArRecUtils.LArRecUtilsConfig import LArMCSymCondAlgCfg
    result.merge (LArMCSymCondAlgCfg (flags))

    if flags.Input.isMC is False:
        #For data, the regular shape is the 4-sample one used to Q-factor computation by LArRawChannelBuilder
        #Here we need a 32-sample, symmetrized shape. Therfore the re-key'ing and the dedicated LArPileUpShapeSymCondAlg

        if flags.Common.isOnline:
            result.merge(addFolderList(flags, (('/LAR/LArPileup/LArPileupShape<key>LArShape32</key>', 'LAR_ONL', 'LArShape32MC'),
                                               ('/LAR/LArPileup/LArPileupAverage','LAR_ONL','LArMinBiasAverageMC')) ))
        else:
            result.merge(addFolderList(flags, (('/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>','LAR_OFL','LArShape32MC'),
                                               ('/LAR/ElecCalibOfl/LArPileupAverage','LAR_OFL','LArMinBiasAverageMC')) ))

        LArMinBiasAverageSymAlg=CompFactory.getComp("LArSymConditionsAlg<LArMinBiasAverageMC,LArMinBiasAverageSym>")
        result.addCondAlgo(LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym"))

        LArShapeSymAlg=CompFactory.getComp("LArSymConditionsAlg<LArShape32MC,LArShape32Sym>")
        result.addCondAlgo(LArShapeSymAlg("LArPileUpShapeSymCondAlg",ReadKey="LArShape32",WriteKey="LArShape32Sym"))

        ShapeKey = 'LArShape32Sym'

        from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
        result.merge (LArElecCalibDbCfg (flags, ['OFC']))
    else:
        from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
        from LArRecUtils.LArRecUtilsConfig import LArOFCCondAlgCfg, LArAutoCorrTotalCondAlgCfg

        result.merge (LArADC2MeVCondAlgCfg (flags))
        result.merge (LArOFCCondAlgCfg (flags))
        result.merge (LArAutoCorrTotalCondAlgCfg (flags))

        result.merge(addFolderList(flags, (('/LAR/ElecCalibMC/Shape','LAR_OFL','LArShape32MC'), 
                                           ('/LAR/ElecCalibMC/LArPileupAverage', 'LAR_OFL', 'LArMinBiasAverageMC')) ))
                               
        LArMinBiasAverageSymAlg=CompFactory.getComp("LArSymConditionsAlg<LArMinBiasAverageMC,LArMinBiasAverageSym>")
        result.addCondAlgo(LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym"))

        ShapeKey = 'LArShapeSym'


    CaloBCIDCoeffsCondAlg = CompFactory.CaloBCIDCoeffsCondAlg # CaloRec
    alg = CaloBCIDCoeffsCondAlg ('CaloBCIDCoeffsCondAlg',
                                 MCSymKey = 'LArMCSym',
                                 OFCKey = 'LArOFC',
                                 ShapeKey = ShapeKey,
                                 MinBiasAvgKey = 'LArPileupAverageSym',
                                 OutputCoeffsKey = 'CaloBCIDCoeffs')
    result.addCondAlgo (alg)

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    only = ['CaloBCIDCoeffsCondAlg',
            'LArPileUpAvgSymCondAlg',
            'LArPileUpShapeSymCondAlg']

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloBCIDCoeffsCondAlgCfg (flags1)
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- mc')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.ESD
    flags2.lock()
    acc2 = CaloBCIDCoeffsCondAlgCfg (flags2)
    acc2.printConfig(summariseProps=True, onlyComponents=only)
    print ('IOVDbSvc:', acc2.getService('IOVDbSvc').Folders)
    acc2.wasMerged()
