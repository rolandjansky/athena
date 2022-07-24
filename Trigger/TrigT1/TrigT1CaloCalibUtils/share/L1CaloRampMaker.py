# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

import glob, os, sys
from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
from AthenaConfiguration.ComponentFactory import CompFactory

###################################    
## Example for run TILE/LAr + L1Calo  calibration
##################################
#  python L1CaloRampMaker.py --doTile -i /eos/atlas/atlastier0/rucio/data22_calib/calibration_L1CaloEnergyScan/00419258/data22_calib.00419258.calibration_L1CaloEnergyScan.daq.RAW/data22_calib.00419258.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data
#######################################
### To run a directory just do:
#######################################
#  python L1CaloRampMaker.py --doTile -i /eos/atlas/atlastier0/rucio/data22_calib/calibration_L1CaloEnergyScan/00419258/data22_calib.00419258.calibration_L1CaloEnergyScan.daq.RAW/*
#######################################



# ===============================================================
#  main()
# ===============================================================
def main():
    from optparse import OptionParser
    parser = OptionParser(usage = "usage: %prog arguments", version="%prog")
    parser.add_option("-i","--InputFiles",        dest="InputFiles",                      help="Input raw data (default: %default)")
    parser.add_option("-l","--doLAr",            dest="doLAr",    action="store_true",  help="Do L1Calo+LAr calibration(default: %default)")
    parser.add_option("-t","--doTile",           dest="doTile",   action="store_true",  help="Do L1Calo+Tile calibration(default: %default)")
    parser.set_defaults(InputFiles="/eos/atlas/atlastier0/rucio/data22_calib/calibration_L1CaloEnergyScan/00419051/data22_calib.00419051.calibration_L1CaloEnergyScan.daq.RAW/data22_calib.00419051.calibration_L1CaloEnergyScan.daq.RAW._lb0000._SFO-1._0001.data", doLAr=False, doTile=False)
    (options,args) = parser.parse_args()
    

    flags.Input.Files = glob.glob(options.InputFiles)
    
    flags.Exec.MaxEvents = -1
    flags.Detector.EnableCalo = True
    flags.Trigger.enableL1CaloLegacy = True
    flags.Calo.Cell.doPileupOffsetBCIDCorr=False
    flags.LAr.doBadFebMasking = False
    flags.LAr.doHVCorr = False
    flags.LAr.OFCShapeFolder="4samples1phase"
    from LArConfiguration.LArConfigFlags import RawChannelSource
    flags.LAr.RawChannelSource = RawChannelSource.Calculated
    flags.Tile.doOverflowFit=True
    flags.Tile.doOptATLAS=True
    flags.Tile.RunType='PHY'
    flags.Tile.BestPhaseFromCOOL=True
    flags.GeoModel.AtlasVersion="ATLAS-R2-2015-03-01-00"
    flags.IOVDb.DBConnection = "sqlite://;schema=energyscanresults.sqlite;dbname=L1CALO"
    flags.IOVDb.GlobalTag = 'CONDBR2-BLKPA-2022-02'


    flags.lock() 

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
    from TrigT1CaloByteStream.LVL1CaloRun2ByteStreamConfig import LVL1CaloRun2ReadBSCfg
    
    acc = MainServicesCfg(flags)
    acc.merge( ByteStreamReadCfg(flags) )
    acc.merge( L1TriggerByteStreamDecoderCfg(flags) )
    acc.merge( LVL1CaloRun2ReadBSCfg(flags))
    
    
    from TrigT1CaloCalibUtils.CaloRecoCalibConfig import CaloRecoCalibCfg
    acc.merge(CaloRecoCalibCfg(flags))
    


    from LArCabling.LArCablingConfig import LArFebRodMappingCfg, LArCalibIdMappingCfg 
    acc.merge(LArFebRodMappingCfg(flags))
    acc.merge(LArCalibIdMappingCfg(flags))
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg     
    acc.merge(LArOnOffIdMappingCfg(flags))

    from CaloConditions.CaloConditionsConfig import CaloTriggerTowerCfg
    acc.merge(CaloTriggerTowerCfg(flags))
    
    
    from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
    LArADC2MeVCondAlg= CompFactory.LArADC2MeVCondAlg()
    LArADC2MeVCondAlg.LArHVScaleCorrKey=""
    acc.addCondAlgo(LArADC2MeVCondAlg, 'AthAlgSeq')


    # To be inplemented: 
    # PostConfigureLAr.py: postInclude script that configures LAr (calibration OFCs)
    #conddb.blockFolder('/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase')
    #conddb.addFolderWithTag('LAR_OFL','/LAR/ElecCalibOfl/OFC/CaliWave','LARElecCalibOflOFCCaliWave-RUN2-UPD3-00');
    from IOVDbSvc.IOVDbSvcConfig import addOverride
    acc.merge(addOverride(flags, "/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples1phase","LARElecCalibOflOFCPhysWaveRTM4samples1phase-RUN2-UPD3-00"))
    acc.merge(addOverride(flags,"/LAR/ElecCalibOfl/Shape/RTM/4samples1phase","LARElecCalibOflShapeRTM4samples1phase-RUN2-UPD3-00"))
    acc.getService("PoolSvc").ReadCatalog+=['file:PoolCat_comcond_castor.xml', 'prfile:poolcond/PoolCat_comcond_castor.xml', 'apcfile:poolcond/PoolCat_comcond_castor.xml']
    

    
    from TrigT1CaloCondSvc.L1CaloCondConfig import L1CaloCondAlgCfg
    acc.merge(L1CaloCondAlgCfg(flags, Physics=False, Calib1=True, Calib2=False))

    
    decorator = CompFactory.LVL1.L1CaloTriggerTowerDecoratorAlg()
    decorator.TriggerTowerTools = CompFactory.LVL1.L1CaloxAODOfflineTriggerTowerTools()
    decorator.DecorName_caloCellEnergy = "CaloCellEnergy"
    decorator.DecorName_caloCellET = "CaloCellET"
    acc.addEventAlgo(decorator, 'AthAlgSeq')
    
    RampMaker = CompFactory.L1CaloRampMaker()
    RampMaker.L1TriggerTowerToolRun3 = CompFactory.LVL1.L1TriggerTowerToolRun3()
    RampMaker.DoTile = options.doTile
    RampMaker.DoLAr = options.doLAr 
    RampMaker.EventsPerEnergyStep = 200
    RampMaker.NumberOfEnergySteps = 11
    RampMaker.IsGain1 = True
    RampMaker.CheckProvenance = True
    RampMaker.TileSaturationCut = 255.
    # special region 1.3 < |eta| < 1.5, saturation on tile side.
    RampMaker.SpecialChannelRange = { 0x6130f02 : 150, 0x7100003 : 150, 0x7180f03 : 150, 0x7180303 : 150, 0x7100200 : 150,
                                      0x6130601 : 150, 0x6130302 : 150, 0x61f0303 : 150, 0x71c0e00 : 150, 0x71c0a00 : 150, 0x7180501 : 150, 0x6130003 : 150, 0x7140d01 : 150,
                                      0x7140600 : 150, 0x7100d00 : 150, 0x6170900 : 150, 0x61b0901 : 150, 0x7180002 : 150, 0x7140c03 : 150, 0x6170901 : 150, 0x6130702 : 150,
                                      0x7180a00 : 150, 0x61b0403 : 150, 0x6130e00 : 150, 0x7180601 : 150, 0x61f0200 : 150, 0x6130002 : 150, 0x61b0601 : 150, 0x71c0e01 : 150,
                                      0x7100900 : 150, 0x7100901 : 150, 0x7100501 : 150, 0x6170100 : 150, 0x7140802 : 150, 0x7140003 : 150, 0x7140803 : 150, 0x7180c02 : 150,
                                      0x7100f02 : 150, 0x61b0b03 : 150, 0x6170302 : 150, 0x6170303 : 150, 0x7180703 : 150, 0x6170b02 : 150, 0x71c0402 : 150, 0x61f0803 : 150,
                                      0x6170b03 : 150, 0x6130101 : 150, 0x71c0601 : 150, 0x7140702 : 150, 0x61f0500 : 150, 0x71c0403 : 150, 0x71c0501 : 150, 0x7140e00 : 150,
                                      0x7140703 : 150, 0x7140402 : 150, 0x61f0501 : 150, 0x7140403 : 150, 0x61b0402 : 150, 0x7140e01 : 150, 0x6130703 : 150, 0x7180302 : 150,
                                      0x61b0a00 : 150, 0x61b0f02 : 150, 0x61b0f03 : 150, 0x7180100 : 150, 0x61b0303 : 150, 0x61f0e01 : 150, 0x71c0b03 : 150, 0x6130d00 : 150,
                                      0x7180101 : 150, 0x7100c03 : 150, 0x61b0a01 : 150, 0x6170802 : 150, 0x7100d01 : 150, 0x6130500 : 150, 0x7100403 : 150, 0x6130d01 : 150,
                                      0x7180702 : 150, 0x6170601 : 150, 0x61f0302 : 150, 0x71c0302 : 150, 0x61f0a01 : 150, 0x7180d00 : 150, 0x6130901 : 150, 0x7180d01 : 150,
                                      0x71c0303 : 150, 0x61f0901 : 150, 0x7140d00 : 150, 0x71c0a01 : 150, 0x7180c03 : 150, 0x6170c03 : 150, 0x6130201 : 150, 0x61b0702 : 150,
                                      0x71c0b02 : 150, 0x7100b02 : 150, 0x71c0600 : 150, 0x61f0600 : 150, 0x7140901 : 150, 0x61f0f02 : 150, 0x6170702 : 150, 0x6130803 : 150,
                                      0x6170403 : 150, 0x6170e00 : 150, 0x7180803 : 150, 0x6170703 : 150, 0x71c0c02 : 150, 0x7140f02 : 150, 0x71c0c03 : 150, 0x7100500 : 150,
                                      0x7140f03 : 150, 0x6130e01 : 150, 0x61b0b02 : 150, 0x6130c02 : 150, 0x6170101 : 150, 0x7100302 : 150, 0x61b0100 : 150, 0x7180003 : 150,
                                      0x7140501 : 150, 0x7100a00 : 150, 0x6130c03 : 150, 0x71c0900 : 150, 0x7100303 : 150, 0x6170002 : 150, 0x61b0101 : 150, 0x7180802 : 150,
                                      0x7100b03 : 150, 0x61f0402 : 150, 0x61f0403 : 150, 0x61f0f03 : 150, 0x7180e00 : 150, 0x7100a01 : 150, 0x7100201 : 150, 0x6130402 : 150,
                                      0x71c0101 : 150, 0x6170d01 : 150, 0x7140c02 : 150, 0x61f0a00 : 150, 0x6130403 : 150, 0x61b0c03 : 150, 0x6170d00 : 150, 0x71c0702 : 150,
                                      0x6130a01 : 150, 0x71c0d01 : 150, 0x6170c02 : 150, 0x61b0803 : 150, 0x7100600 : 150, 0x6170500 : 150, 0x61f0201 : 150, 0x6130600 : 150,
                                      0x61b0002 : 150, 0x7180900 : 150, 0x6170501 : 150, 0x7180901 : 150, 0x61b0003 : 150, 0x6130a00 : 150, 0x61f0900 : 150, 0x6170803 : 150,
                                      0x7140303 : 150, 0x7140100 : 150, 0x71c0200 : 150, 0x7180f02 : 150, 0x7140500 : 150, 0x71c0201 : 150, 0x6170003 : 150, 0x6130200 : 150,
                                      0x7140601 : 150, 0x6170e01 : 150, 0x61f0b02 : 150, 0x61f0b03 : 150, 0x71c0f02 : 150, 0x61b0e00 : 150, 0x61b0703 : 150, 0x71c0002 : 150,
                                      0x61b0e01 : 150, 0x7140a01 : 150, 0x6130b02 : 150, 0x71c0802 : 150, 0x7140b02 : 150, 0x71c0803 : 150, 0x7100100 : 150, 0x61f0100 : 150,
                                      0x61b0900 : 150, 0x7140b03 : 150, 0x71c0003 : 150, 0x6130f03 : 150, 0x7100101 : 150, 0x7140a00 : 150, 0x7140200 : 150, 0x7140201 : 150,
                                      0x61f0702 : 150, 0x7100802 : 150, 0x61b0d00 : 150, 0x61b0600 : 150, 0x61b0d01 : 150, 0x7180402 : 150, 0x61f0c02 : 150, 0x61f0c03 : 150,
                                      0x7100703 : 150, 0x61f0002 : 150, 0x6130900 : 150, 0x71c0703 : 150, 0x7180a01 : 150, 0x7180e01 : 150, 0x61f0601 : 150, 0x7140002 : 150,
                                      0x61f0802 : 150, 0x7100002 : 150, 0x7100c02 : 150, 0x7100f03 : 150, 0x61b0200 : 150, 0x6130100 : 150, 0x6170f02 : 150, 0x6170200 : 150,
                                      0x61b0201 : 150, 0x6170f03 : 150, 0x6170600 : 150, 0x6130501 : 150, 0x7140900 : 150, 0x61b0501 : 150, 0x71c0901 : 150, 0x7100702 : 150,
                                      0x61b0500 : 150, 0x7100803 : 150, 0x7180403 : 150, 0x61b0802 : 150, 0x71c0d00 : 150, 0x6130b03 : 150, 0x6130303 : 150, 0x6170201 : 150,
                                      0x7180600 : 150, 0x61f0003 : 150, 0x7100e01 : 150, 0x7180500 : 150, 0x71c0f03 : 150, 0x6170a00 : 150, 0x61b0c02 : 150, 0x61f0101 : 150,
                                      0x6170402 : 150, 0x7100402 : 150, 0x6130802 : 150, 0x7100e00 : 150, 0x7140302 : 150, 0x61f0e00 : 150, 0x7180b02 : 150, 0x7180b03 : 150,
                                      0x71c0500 : 150, 0x7140101 : 150, 0x6170a01 : 150, 0x7180200 : 150, 0x7180201 : 150, 0x61b0302 : 150, 0x61f0703 : 150, 0x71c0100 : 150,
                                      0x7100601 : 150, 0x61f0d00 : 150, 0x61f0d01 : 150,
                                      # saturating channels
                                      0x7120203 : 100
                                  }
    
    
    acc.addEventAlgo(RampMaker, 'AthAlgSeq')


    L1CaloCalib = CompFactory.L1CaloLinearCalibration()
    acc.addEventAlgo(L1CaloCalib, 'AthAlgSeq')

    from RegistrationServices.OutputConditionsAlgConfig import OutputConditionsAlgCfg
    conditionStream =  OutputConditionsAlgCfg(flags, "L1CaloRampDataContainer", outputFile="L1CaloRampData.pool.root", WriteIOV = False)
    acc.merge(conditionStream)


    
    outputCondAlgCA = OutputConditionsAlgCfg(flags, "EnergyScanResultOutput", outputFile="dummy.root", WriteIOV = True, ObjectList = ["CondAttrListCollection#/TRIGGER/L1Calo/V1/Results/EnergyScanResults","AthenaAttributeList#/TRIGGER/L1Calo/V1/Results/EnergyScanRunInfo"])
    acc.merge(outputCondAlgCA)



    L1CaloDumpRampData = CompFactory.L1CaloDumpRampData(RootStreamName="RAMPDATA")
    acc.addEventAlgo(L1CaloDumpRampData, 'AthAlgSeq')


    histSvc = CompFactory.THistSvc( Output = ["AANT DATAFILE='output.root' OPT='RECREATE'"] )
    histSvc.Output +=  ["RAMPDATA DATAFILE='graphs.root' OPT='RECREATE'"]
    acc.addService(histSvc)




    sys.exit(acc.run().isFailure())


       
# ===============================================================
#  __main__
# ===============================================================
if __name__ == '__main__':
    main()


