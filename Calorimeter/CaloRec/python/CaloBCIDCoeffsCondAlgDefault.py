# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDCoeffsCondAlgDefault.py
# Created: Mar 2020, sss
# Purpose: Configure CaloBCIDCoeffsCondAlg.


from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloBCIDCoeffsCondAlgDefault():
    from AthenaCommon.GlobalFlags import globalflags
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

    name = 'CaloBCIDCoeffsCondAlg'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    from LArRecUtils.LArMCSymCondAlg import LArMCSymCondAlgDefault
    LArMCSymCondAlgDefault()

    if globalflags.DataSource()=='data':
        if athenaCommonFlags.isOnline:
            conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
            conddb.addFolder("LAR_ONL","/LAR/LArPileup/LArPileupAverage",className="LArMinBiasAverageMC")
        else:
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupShape<key>LArShape32</key>",className="LArShape32MC")
            conddb.addFolder("LAR_OFL","/LAR/ElecCalibOfl/LArPileupAverage",className="LArMinBiasAverageMC")

        #For data, the regular shape is the 4-sample one used to Q-factor computation by LArRawChannelBuilder
        #Here we need a 32-sample, symmetrized shape. Therfore the re-key'ing and the dedicated LArPileUpShapeSymCondAlg

        from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
        if not hasattr (condSeq, 'LArPileUpAvgSymCondAlg'):
            condSeq+=LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym")

        from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArShape32MC_LArShape32Sym_ as LArShapeSymAlg
        if not hasattr (condSeq, 'LArPileUpShapeSymCondAlg'):
            condSeq+=LArShapeSymAlg("LArPileUpShapeSymCondAlg",ReadKey="LArShape32",WriteKey="LArShape32Sym")

        ShapeKey = 'LArShape32Sym'
    else: #MC case
        from LArRecUtils.LArOFCCondAlgDefault import LArOFCCondAlgDefault
        from LArRecUtils.LArAutoCorrTotalCondAlgDefault import  LArAutoCorrTotalCondAlgDefault
        from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
        LArADC2MeVCondAlgDefault()
        LArAutoCorrTotalCondAlgDefault()
        LArOFCCondAlgDefault()
        conddb.addFolder("LAR_OFL","/LAR/ElecCalibMC/LArPileupAverage",className="LArMinBiasAverageMC")

        from LArRecUtils.LArRecUtilsConf import LArSymConditionsAlg_LArMinBiasAverageMC_LArMinBiasAverageSym_ as LArMinBiasAverageSymAlg
        if not hasattr (condSeq, 'LArPileUpAvgSymCondAlg'):
            condSeq+=LArMinBiasAverageSymAlg("LArPileUpAvgSymCondAlg",ReadKey="LArPileupAverage",WriteKey="LArPileupAverageSym")

        ShapeKey = 'LArShapeSym'

    CaloBCIDCoeffsCondAlg = CompFactory.CaloBCIDCoeffsCondAlg # CaloRec
    alg = CaloBCIDCoeffsCondAlg (name,
                                 MCSymKey = 'LArMCSym',
                                 OFCKey = 'LArOFC',
                                 ShapeKey = ShapeKey,
                                 MinBiasAvgKey = 'LArPileupAverageSym',
                                 OutputCoeffsKey = 'CaloBCIDCoeffs')
    condSeq += alg
    return alg

    
