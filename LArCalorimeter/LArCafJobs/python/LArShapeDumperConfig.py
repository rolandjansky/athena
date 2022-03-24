# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory 

def LArShapeDumperCfg(flags):

    result=ComponentAccumulator()

    
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(flags))
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(TileGMCfg(flags))

    #Setup cabling
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(flags))

    from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg
    result.merge(LArRawDataReadingCfg(flags))

    from LArROD.LArRawChannelBuilderAlgConfig import LArRawChannelBuilderAlgCfg
    result.merge(LArRawChannelBuilderAlgCfg(flags))
    result.getEventAlgo("LArRawChannelBuilder").TimingContainerKey="LArOFIterResult"
    
    from LArCellRec.LArTimeVetoAlgConfig import LArTimeVetoAlgCfg
    result.merge(LArTimeVetoAlgCfg(flags))

    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    result.merge(BunchCrossingCondAlgCfg(flags))
    
    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge(CaloNoiseCondAlgCfg(flags,"totalNoise"))

    from LArROD.LArFebErrorSummaryMakerConfig import LArFebErrorSummaryMakerCfg
    result.merge(LArFebErrorSummaryMakerCfg(flags))
    result.getEventAlgo("LArFebErrorSummaryMaker").CheckAllFEB=False

    from IOVDbSvc.IOVDbSvcConfig import addFolders 
    result.merge(addFolders(flags,
                            '/LAR/ElecCalibOfl/Shape/RTM/5samples3bins17phases<tag>LARElecCalibOflShapeRTM5samples3bins17phases-RUN2-UPD3-00</tag><key>LArShape17phases</key>',
                            'LAR_OFL'))

    result.getService("PoolSvc").ReadCatalog += ["apcfile:poolcond/PoolCat_comcond_castor.xml"]

    if flags.LArShapeDump.doTrigger:
        from TrigDecisionTool.TrigDecisionToolConfig import getTrigDecisionTool
        result.merge(getTrigDecisionTool(flags))

        from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1TriggerByteStreamDecoderCfg
        result.merge(L1TriggerByteStreamDecoderCfg(flags))

    
    result.merge(addFolders(flags,'/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<tag>LARElecCalibOflAutoCorrsAutoCorr-RUN2-UPD3-00</tag>','LAR_OFL'))
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/Shape/RTM/5samples1phase</prefix><tag>LARElecCalibOflShapeRTM5samples1phase-RUN2-UPD1-04</tag>']
    # for splashes: FIXME later
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples3bins17phases</prefix><tag>LARElecCalibOflOFCPhysWaveRTM4samples3bins17phases-RUN2-UPD3-00</tag>']
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases</prefix><tag>LARElecCalibOflShapeRTM4samples3bins17phases-RUN2-UPD3-00</tag>']

    
    print("Dumping flags: ")
    flags.dump()
    dumperAlg=CompFactory.LArShapeDumper("LArShapeDumper")
    dumperAlg.CaloType = flags.LArShapeDump.caloType
    dumperAlg.Prescale = flags.LArShapeDump.prescale
    dumperAlg.NoiseSignifCut = flags.LArShapeDump.noiseSignifCut
    dumperAlg.DoTrigger = flags.LArShapeDump.doTrigger
    dumperAlg.DoStream = flags.LArShapeDump.doStream
    dumperAlg.DoOFCIter = flags.LArShapeDump.doOFCIter
    dumperAlg.DumpChannelInfos = flags.LArShapeDump.dumpChannelInfos
    dumperAlg.DumpDisconnected = False
    dumperAlg.DigitsKey = flags.LArShapeDump.digitsKey
    dumperAlg.ProblemsToMask=['deadReadout', 'deadPhys','almostDead', 'short',
                              'highNoiseHG','highNoiseMG','highNoiseLG']
    dumperAlg.LArShapeDumperTool=CompFactory.LArShapeDumperTool(DoShape=True)
    dumperAlg.FileName=flags.LArShapeDump.outputNtup
    dumperAlg.TriggerNames = flags.LArShapeDump.triggerNames
    if flags.LAr.RawChannelSource == "calculated":
       dumperAlg.ChannelsKey = "LArRawChannels_FromDigits"

    result.addEventAlgo(dumperAlg)

    if (flags.LArShapeDump.HECNoiseNtup!=""):
        result.addEventAlgo(CompFactory.LArHECNoise())
        result.addService(CompFactory.THistSvc(Output=["HEC DATAFILE='"+flags.LArShapeDump.HECNoiseNtup+"' OPT='RECREATE'",]))

    return result


if __name__=="__main__":
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArShapeDumperFlags import addShapeDumpFlags
    addShapeDumpFlags(ConfigFlags)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RAW
    ConfigFlags.LAr.ROD.forceIter=True

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(LArShapeDumperCfg(ConfigFlags))

    cfg.run(10)
