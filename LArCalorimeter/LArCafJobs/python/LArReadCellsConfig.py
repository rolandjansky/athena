# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory 

def LArReadCellsCfg(flags):

    result=ComponentAccumulator()

    #setup Calo reco
    from CaloRec.CaloRecoConfig import CaloRecoCfg
    result.merge(CaloRecoCfg(flags))
    result.getEventAlgo("LArRawChannelBuilder").TimingContainerKey="LArOFIterResult"
    
    from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
    result.merge(BunchCrossingCondAlgCfg(flags))
    
    from IOVDbSvc.IOVDbSvcConfig import addFolders 
    result.merge(addFolders(flags,
                            '/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases<tag>LARElecCalibOflShapeRTM4samples3bins17phases-RUN2-UPD3-00</tag><key>LArShape17phases</key>',
                            'LAR_OFL'))

    result.getService("PoolSvc").ReadCatalog += ["apcfile:poolcond/PoolCat_comcond_castor.xml"]

    result.merge(addFolders(flags,'/LAR/ElecCalibOfl/AutoCorrs/AutoCorr<tag>LARElecCalibOflAutoCorrsAutoCorr-RUN2-UPD3-00</tag>','LAR_OFL'))
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/Shape/RTM/5samples1phase</prefix><tag>LARElecCalibOflShapeRTM5samples1phase-RUN2-UPD1-04</tag>']
    # for splashes: FIXME later
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/OFC/PhysWave/RTM/4samples3bins17phases</prefix><tag>LARElecCalibOflOFCPhysWaveRTM4samples3bins17phases-RUN2-UPD3-00</tag>']
    result.getService("IOVDbSvc").overrideTags+=['<prefix>/LAR/ElecCalibOfl/Shape/RTM/4samples3bins17phases</prefix><tag>LARElecCalibOflShapeRTM4samples3bins17phases-RUN2-UPD3-00</tag>']

    print("Dumping flags: ")
    flags.dump()
    dumperAlg=CompFactory.LArReadCells("LArReadCells")
    dumperAlg.output = flags.LArShapeDump.outputNtup
    dumperAlg.etCut = -1500.

    result.addEventAlgo(dumperAlg)

    return result


if __name__=="__main__":
    
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from LArShapeDumperFlags import addShapeDumpFlags
    addShapeDumpFlags(ConfigFlags)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files=defaultTestFiles.RAW
    ConfigFlags.LAr.ROD.forceIter=True
    ConfigFlags.LArShapeDump.outputNtup="SPLASH"

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    
    cfg=MainServicesCfg(ConfigFlags)
    cfg.addService(CompFactory.THistSvc(Output=["SPLASH DATAFILE='ntuple.root' OPT='RECREATE'",]))
    cfg.merge(LArReadCellsCfg(ConfigFlags))


    cfg.run(10)
