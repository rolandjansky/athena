# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDCoeffsCondAlgSCDefault.py
# Created: Mar 2020, sss
# Purpose: Configure CaloBCIDCoeffsCondAlg.


from AthenaCommon.AlgSequence import AthSequencer
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.CondDB import conddb

def addLArFlatFolder (db, obj, calg, folder_base='/LAR/ElecCalibFlat/',qual=''):
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    folder = folder_base + obj
    if not conddb.folderRequested(folder):
      conddb.addFolder(db, folder + qual,
                     className = 'CondAttrListCollection')
      condSequence += calg (ReadKey=folder, WriteKey='LAr'+obj+'SC')
    return



def CaloBCIDCoeffsCondAlgSCDefault():
    from AthenaCommon.GlobalFlags import globalflags

    name = 'CaloBCIDCoeffsCondAlgSC'
    condSeq = AthSequencer ('AthCondSeq')

    if hasattr (condSeq, name):
        return getattr (condSeq, name)

    if globalflags.DataSource()=='data':
        print("Not yet available")
        return None
    else: #MC case
        from LArRecUtils.LArOFCSCCondAlgDefault import LArOFCSCCondAlgDefault
        from LArRecUtils.LArAutoCorrTotalSCCondAlgDefault import  LArAutoCorrTotalSCCondAlgDefault
        from LArRecUtils.LArADC2MeVSCCondAlgDefault import LArADC2MeVSCCondAlgDefault
        from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeSC_ as LArShapeSCCondAlg
        LArADC2MeVSCCondAlgDefault()
        LArAutoCorrTotalSCCondAlgDefault()
        LArOFCSCCondAlgDefault()
        conddb.addFolder("LAR_OFL","/LAR/ElecCalibMCSC/LArPileupAverage<tag>LARElecCalibMCSCLArPileupAverage-IOVDEP-00</tag>",className="CondAttrListCollection")

        from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMinBiasAverageSC_ as LArPileupAverageAlgSC
        addLArFlatFolder ('LAR_OFL', 'Shape', LArShapeSCCondAlg,'/LAR/ElecCalibMCSC/')
        if not hasattr (condSeq, 'LArPileupAverageAlgSC'):
            condSeq+=LArPileupAverageAlgSC("LArPileupAverageAlgSC",ReadKey="/LAR/ElecCalibMCSC/LArPileupAverage",WriteKey="LArPileupAverageSC")

        # we might need to add the condition algorithm for shape here
        ShapeKey = 'LArShapeSC'

    CaloBCIDCoeffsCondAlg = CompFactory.CaloBCIDCoeffsCondAlg # CaloRec
    alg = CaloBCIDCoeffsCondAlg (name,
                                 MCSymKey = '',
                                 OFCKey = 'LArOFCSC',
                                 ShapeKey = ShapeKey,
                                 MinBiasAvgKey = 'LArPileupAverageSC',
                                 OutputCoeffsKey = 'CaloBCIDCoeffsSC',
                                 IsSuperCell=True)
    condSeq += alg
    return alg

    
