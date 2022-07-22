# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import sys

def L1CaloCondFoldersCfg(flags, Physics, Calib1, Calib2):

        
        L1CaloFolders = {}
        
        if Physics:
                L1CaloFolders['PprChanCalibV2Physics'] = '/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib'
                L1CaloFolders['PprChanCommonV2Physics'] = '/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCommon'
                L1CaloFolders['PprChanHighMuV2Physics'] = '/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanHighMu'
                L1CaloFolders['PprChanLowMuV2Physics'] = '/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanLowMu'
        if Calib1: 
                L1CaloFolders['PprChanCalibV2Calib1'] = '/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanCalib'
                L1CaloFolders['PprChanCommonV2Calib1'] = '/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanCommon'
                L1CaloFolders['PprChanHighMuV2Calib1'] = '/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanHighMu'
                L1CaloFolders['PprChanLowMuV2Calib1'] = '/TRIGGER/L1Calo/V2/Calibration/Calib1/PprChanLowMu'
        if Calib2:
                L1CaloFolders['PprChanCalibV2Calib2'] = '/TRIGGER/L1Calo/V2/Calibration/Calib2/PprChanCalib'
                L1CaloFolders['PprChanCommonV2Calib2'] = '/TRIGGER/L1Calo/V2/Calibration/Calib2/PprChanCommon'
                L1CaloFolders['PprChanHighMuV2Calib2'] = '/TRIGGER/L1Calo/V2/Calibration/Calib2/PprChanHighMu'
                L1CaloFolders['PprChanLowMuV2Calib2'] = '/TRIGGER/L1Calo/V2/Calibration/Calib2/PprChanLowMu'

        L1CaloFolders['PprChanDefaultsV2'] = '/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults'
        L1CaloFolders['ReadoutConfig'] =  "/TRIGGER/L1Calo/V2/Configuration/ReadoutConfig"
        L1CaloFolders['ReadoutConfigJSON'] =  "/TRIGGER/L1Calo/V2/Configuration/ReadoutConfigJSON"
        L1CaloFolders['PprChanStrategy'] = "/TRIGGER/L1Calo/V2/Configuration/PprChanStrategy"


        L1CaloFolders['FineTimeReferences'] =  "/TRIGGER/L1Calo/V1/References/FineTimeReferences"
        L1CaloFolders['RunParameters'] = "/TRIGGER/L1Calo/V1/Conditions/RunParameters"
        L1CaloFolders['Strategy'] = '/TRIGGER/Receivers/Conditions/Strategy'
        L1CaloFolders['DisabledTowers'] = '/TRIGGER/L1Calo/V1/Conditions/DisabledTowers'
        L1CaloFolders['DerivedRunPars'] =   '/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars'
        L1CaloFolders['PpmDeadChannels'] = '/TRIGGER/L1Calo/V1/Calibration/PpmDeadChannels'

        from IOVDbSvc.IOVDbSvcConfig import addFolders
        db = 'TRIGGER' 
        return addFolders(flags,list(L1CaloFolders.values()), db, className='CondAttrListCollection')



def L1CaloCondAlgCfg(flags, readTest=False, Physics=True, Calib1=True, Calib2=True):
        
        from AthenaConfiguration.ComponentFactory import CompFactory
        result = L1CaloCondFoldersCfg(flags, Physics, Calib1, Calib2)
        result.addCondAlgo(CompFactory.L1CaloCondAlg(), 'AthAlgSeq')
        
        if readTest:
                # Set True for test L1CaloCondAlg and print condition container parameters 
                L1CaloCondReader = CompFactory.L1CaloCondAlgReader()
                result.addEventAlgo(L1CaloCondReader, 'AthAlgSeq')
        

        return result
        

if __name__=="__main__":
        
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    
    flags.Input.Files = defaultTestFiles.RAW 
    flags.Exec.MaxEvents = 1
    flags.IOVDb.GlobalTag = 'CONDBR2-BLKPA-2022-02'
    flags.GeoModel.AtlasVersion="ATLAS-R2-2015-03-01-00"
    flags.Trigger.enableL1CaloLegacy = True 
    
    flags.lock() 
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
    from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
    

    acc = MainServicesCfg(flags)
    acc.merge( ByteStreamReadCfg(flags) )
    acc.merge( L1TriggerByteStreamDecoderCfg(flags) )
    acc.merge( LVL1CaloRun2ReadBSCfg(flags))

    #### Default 
    # outputlevel=1 (INFO = 1, DEBUG=2) 
    # timingRegime = "", strategy = "" are empty. Read it from DB directly 
    # readTest False, True execute L1CaloCondAlgReader  
    ####
    acc.merge(L1CaloCondAlgCfg(flags, Physics=True, Calib1=False, Calib2=False))

    #Example ...
    #acc.merge(L1CaloCondAlgCfg(flags,readTest=True, Physics=True, Calib1=False, Calib2=False))
    #acc.getCondAlgo('L1CaloCondAlg').OutputLevel = 2
    #acc.getCondAlgo('L1CaloCondAlg').timingRegime = "Calib2"
    #acc.getCondAlgo('L1CaloCondAlg').strategy = "LowMu"

    
   
    
    
    sys.exit(acc.run().isFailure())
