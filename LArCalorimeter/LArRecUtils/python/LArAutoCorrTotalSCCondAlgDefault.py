# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging

from LArRecUtils.LArRecUtilsConf import LArAutoCorrTotalCondAlg 
from AthenaCommon.AlgSequence import AthSequencer
from LArCabling.LArCablingAccess import LArOnOffIdMappingSC
condSeq = AthSequencer("AthCondSeq")
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

def LArAutoCorrTotalSCCondAlgDefault():

    mlog = logging.getLogger( 'LArAutoCorrTotalSCCondAlg::__init__ ' )
    mlog.info("entering LArAutoCorrTotalSCCondAlgDefault")

    condSeq = AthSequencer("AthCondSeq")
    if hasattr (condSeq,"LArAutoCorrTotalSCCondAlg"):
        return getattr(condSeq,"LArAutoCorrTotalSCCondAlg")

    LArOnOffIdMappingSC()
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArNoiseSC_ as LArNoiseSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArPedestalSC_ as LArPedestalSCFlatCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArShapeSC_ as LArShapeSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArfSamplSC_ as LArfSamplSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArAutoCorrSC_ as LArAutoCorrSCCondAlg
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArMinBiasSC_ as LArMinBiasSCCondAlg


    #include('LArConditionsCommon/LArConditionsCommon_comm_jobOptions')
    addLArFlatFolder ('LAR_OFL', 'Shape', LArShapeSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'Pedestal', LArPedestalSCFlatCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'Noise', LArNoiseSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'fSampl', LArfSamplSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'AutoCorr', LArAutoCorrSCCondAlg,'/LAR/ElecCalibMCSC/')
    addLArFlatFolder ('LAR_OFL', 'MinBias', LArMinBiasSCCondAlg,'/LAR/ElecCalibMCSC/')

    from LArRecUtils.LArADC2MeVSCCondAlgDefault import LArADC2MeVSCCondAlgDefault
    LArADC2MeVSCCondAlgDefault()

    theAutoCorrTotalCondAlg=LArAutoCorrTotalCondAlg("LArAutoCorrTotalSCCondAlg",isSuperCell=True)
    theAutoCorrTotalCondAlg.LArADC2MeVObjKey='LArADC2MeVSC'
    theAutoCorrTotalCondAlg.LArOnOffIdMappingObjKey='LArOnOffIdMapSC'
    theAutoCorrTotalCondAlg.LArShapeObjKey='LArShapeSC'
    theAutoCorrTotalCondAlg.LArAutoCorrObjKey='LArAutoCorrSC'
    theAutoCorrTotalCondAlg.LArNoiseObjKey='LArNoiseSC'
    theAutoCorrTotalCondAlg.LArPedestalObjKey='LArPedestalSC'
    theAutoCorrTotalCondAlg.LArfSamplObjKey='LArfSamplSC'
    theAutoCorrTotalCondAlg.LArMinBiasObjKey='LArMinBiasSC'
    theAutoCorrTotalCondAlg.LArAutoCorrTotalObjKey='LArAutoCorrTotalSC'


    from AthenaCommon.BeamFlags import jobproperties
    from LArROD.LArRODFlags import larRODFlags

    from AthenaCommon.SystemOfUnits import ns
    theAutoCorrTotalCondAlg.Nsamples = larRODFlags.nSamples()
    theAutoCorrTotalCondAlg.firstSample = larRODFlags.firstSample()
    theAutoCorrTotalCondAlg.deltaBunch = int(jobproperties.Beam.bunchSpacing()/( 25.*ns)+0.5)   

    if larRODFlags.doOFCPileupOptimization():
        if larRODFlags.NumberOfCollisions():
            theAutoCorrTotalCondAlg.Nminbias=larRODFlags.NumberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   deltaBunch %f", larRODFlags.NumberOfCollisions(), jobproperties.Beam.bunchSpacing())
        else:
            theAutoCorrTotalCondAlg.Nminbias=jobproperties.Beam.numberOfCollisions()
            mlog.info("  setup for  Ncollisions %f   deltaBunch %f", jobproperties.Beam.numberOfCollisions(), jobproperties.Beam.bunchSpacing())
    else:
        theAutoCorrTotalCondAlg.Nminbias=0
        mlog.info("  setup for computing total noise autocorrelation without pileup")
    
 
    condSeq+=theAutoCorrTotalCondAlg
    return theAutoCorrTotalCondAlg
