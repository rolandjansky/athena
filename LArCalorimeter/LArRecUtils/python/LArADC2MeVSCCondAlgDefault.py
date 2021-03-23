# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
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

def LArADC2MeVSCCondAlgDefault(isMC=True):

    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArADC2MeVSCCondAlg"):
        return getattr(condSeq,"LArADC2MeVSCCondAlg")

    LArOnOffIdMappingSC()
    condSeq = AthSequencer("AthCondSeq")

    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArRampSC_ as LArRampCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LAruA2MeVSC_ as LAruA2MeVCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArDAC2uASC_ as LArDAC2uACondAlg
    addLArFlatFolder ('LAR_OFL', 'Ramp', LArRampCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'uA2MeV', LAruA2MeVCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'DAC2uA', LArDAC2uACondAlg,'/LAR/ElecCalibMCSC/')


    theADC2MeVCondAlg=LArADC2MeVCondAlg("LArADC2MeVSCCondAlg",LArADC2MeVKey = 'LArADC2MeVSC',isSuperCell=True,LArOnOffIdMappingKey='LArOnOffIdMapSC')
 
    if isMC:
        theADC2MeVCondAlg.LArMphysOverMcalKey="" #No MphysOVerMcal

        theADC2MeVCondAlg.LArHVScaleCorrKey=""
      
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSC"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASC"
        theADC2MeVCondAlg.LArRampKey="LArRampSC"


        theADC2MeVCondAlg.UseFEBGainTresholds=False
        
    condSeq+=theADC2MeVCondAlg
    return theADC2MeVCondAlg
