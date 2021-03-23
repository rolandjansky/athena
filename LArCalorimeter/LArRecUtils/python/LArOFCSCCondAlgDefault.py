# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Include import include

include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")
from AthenaCommon.Logging import logging
from LArRecUtils.LArRecUtilsConf import LArOFCCondAlg 
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

def LArOFCSCCondAlgDefault():

    mlog = logging.getLogger('LArOFCSCCondAlg')
    mlog.info("entering LArOFCSCCondAlgDefault")

    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArOFCSCCondAlg"):
        return getattr(condSeq,"LArOFCSCCondAlg")

    LArOnOffIdMappingSC()

    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArNoiseSC_ as LArNoiseSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalSC_ as LArPedestalSCFlatCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeSC_ as LArShapeSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArfSamplSC_ as LArfSamplSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArAutoCorrSC_ as LArAutoCorrSCCondAlg


    #include('LArConditionsCommon/LArConditionsCommon_comm_jobOptions')
    addLArFlatFolder ('LAR_OFL', 'Shape', LArShapeSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'Pedestal', LArPedestalSCFlatCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'Noise', LArNoiseSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'fSampl', LArfSamplSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'AutoCorr', LArAutoCorrSCCondAlg,'/LAR/ElecCalibMCSC/')

    from LArRecUtils.LArAutoCorrTotalSCCondAlgDefault import LArAutoCorrTotalSCCondAlgDefault
    LArAutoCorrTotalSCCondAlgDefault()

    theOFCCondAlg=LArOFCCondAlg("LArOFCSCCondAlg")
    theOFCCondAlg.isMC  = True
    theOFCCondAlg.isSuperCell  = True
    from LArROD.LArRODFlags import larRODFlags
    from AthenaCommon.BeamFlags import jobproperties

    theOFCCondAlg.firstSample = larRODFlags.firstSample()
    theOFCCondAlg.useHighestGainAutoCorr = larRODFlags.useHighestGainAutoCorr()
    theOFCCondAlg.LArOnOffIdMappingObjKey='LArOnOffIdMapSC'
    theOFCCondAlg.LArShapeObjKey='LArShapeSC'
    theOFCCondAlg.LArNoiseObjKey='LArNoiseSC'
    theOFCCondAlg.LArPedestalObjKey='LArPedestalSC'
    theOFCCondAlg.LArAutoCorrTotalObjKey='LArAutoCorrTotalSC'
    theOFCCondAlg.LArOFCObjKey='LArOFCSC'


    if larRODFlags.doOFCPileupOptimization():
        if larRODFlags.NumberOfCollisions():
            theOFCCondAlg.Nminbias=larRODFlags.NumberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   ", larRODFlags.NumberOfCollisions())
        else:
            theOFCCondAlg.Nminbias=jobproperties.Beam.numberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   ", jobproperties.Beam.numberOfCollisions())
    else:
        theOFCCondAlg.Nminbias=0
        mlog.info("  setup for no pileup optimization")
      
    pass
    
    condSeq+=theOFCCondAlg
    return theOFCCondAlg
