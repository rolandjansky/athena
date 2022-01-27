# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from IOVDbSvc.CondDB import conddb
from LArRecUtils.LArRecUtilsConf import LArAutoCorrNoiseCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
condSeq = AthSequencer("AthCondSeq")

def addLArFlatFolder (db, obj, calg, folder_base='/LAR/ElecCalibFlat/',qual=''):
    from AthenaCommon.AlgSequence import AthSequencer
    condSequence = AthSequencer("AthCondSeq")

    folder = folder_base + obj
    if not conddb.folderRequested(folder):
      conddb.addFolder(db, folder + qual,
                     className = 'CondAttrListCollection')
      condSequence += calg (ReadKey=folder, WriteKey='LAr'+obj+'SC')
    return

def LArAutoCorrNoiseSCCondAlgDefault():

    LArOnOffIdMappingSC()
    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArAutoCorrNoiseSCCondAlg"):
        return getattr(condSeq,"LArAutoCorrNoiseSCCondAlg")

    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArAutoCorrSC_ as LArAutoCorrSCCondAlg
    addLArFlatFolder ('LAR_OFL', 'AutoCorr', LArAutoCorrSCCondAlg,'/LAR/ElecCalibMCSC/')
    theAutoCorrNoiseCondAlg=LArAutoCorrNoiseCondAlg("LArAutoCorrNoiseSCCondAlg",isSuperCell=True,LArAutoCorrNoiseObjKey="LArAutoCorrNoiseSC",LArAutoCorrObjKey="LArAutoCorrSC",LArOnOffIdMappingObjKey='LArOnOffIdMapSC')

    from LArROD.LArRODFlags import larRODFlags
    # we need to keep one sample before, another after
    theAutoCorrNoiseCondAlg.nSampl = larRODFlags.nSamples() + 2 # See ATLASSIM-5483
 
    condSeq+=theAutoCorrNoiseCondAlg
    return theAutoCorrNoiseCondAlg
