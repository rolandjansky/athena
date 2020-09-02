#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
CaloNoiseCondAlg=CompFactory.CaloNoiseCondAlg
from AthenaCommon.Logging import logging

def CaloNoiseCondAlgCfg(configFlags,noisetype="totalNoise"):
    if noisetype not in ("electronicNoise","pileupNoise","totalNoise"):
        raise RuntimeError("Requested noise of unknown type %s" % noisetype)

    noiseAlgName="Calo_"+noisetype+"Alg"
    

    log = logging.getLogger("CaloNoiseToolCfg")
    result=ComponentAccumulator()

    isMC=configFlags.Input.isMC
    fixedLumi=configFlags.Calo.Noise.fixedLumiForNoise
    useCaloLumi=configFlags.Calo.Noise.useCaloNoiseLumi

    #CaloNoiseCondAlg needs Identifiers ... 
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    gms=GeoModelCfg(configFlags)
    result.merge(gms)

    #... and cabling:
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(configFlags))

    theCaloNoiseAlg=CaloNoiseCondAlg(noiseAlgName,OutputKey=noisetype)

    if configFlags.Common.isOnline:
        log.info("Configuring CaloNoiseCondAlg for online case")
        #online mode:
        result.merge(addFolders(configFlags,"/CALO/Noise/CellNoise",'CALO_ONL',className="CondAttrListCollection"))
        theCaloNoiseAlg.CaloNoiseFolder="/CALO/Noise/CellNoise"
        theCaloNoiseAlg.LArNoiseFolder=""
        theCaloNoiseAlg.TileNoiseFolder=""
        if fixedLumi >= 0 :
            theCaloNoiseAlg.Luminosity = fixedLumi
            log.info("online mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Noise/PileUpNoiseLumi'
                result.merge(addFolders(configFlags,lumiFolder,'CALO',className="CondAttrListCollection"))
                theCaloNoiseAlg.LumiFolder = lumiFolder
                theCaloNoiseAlg.Luminosity = -1.
                log.info("online mode: use luminosity from /CALO/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                theCaloNoiseAlg.Luminosity = 0.
                log.info("online mode: ignore pileup noise")
                pass
        result.addCondAlgo(theCaloNoiseAlg)
        return result

    #The not-online case:
    if isMC:
        log.info("Configuring CaloNoiseCondAlg for MC data processing")
        if fixedLumi >= 0 :
            theCaloNoiseAlg.Luminosity=fixedLumi
            log.info("Luminosity (in 10**33) units used for pileup noise from CaloNoiseFlags : %f", fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.merge(addFolders(configFlags,lumiFolder,'CALO_OFL',className="CondAttrListCollection"))
                log.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileuUpNoiseLumi to scale pileup noise")
                theCaloNoiseAlg.LumiFolder = lumiFolder
                theCaloNoiseAlg.Luminosity=-1.
            else:
                estimatedLumi=configFlags.Beam.estimatedLuminosity
                theCaloNoiseAlg.Luminosity=estimatedLumi/1e+33
                log.info("  Luminosity (in 10**33) units used for pileup noise from Beam flags: %f", theCaloNoiseAlg.Luminosity)

        theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
        result.merge(addFolders(configFlags,"/LAR/NoiseOfl/CellNoise","LAR_OFL",className="CondAttrListCollection"))
        theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
        result.merge(addFolders(configFlags,"/TILE/OFL02/NOISE/CELL","TILE_OFL",className="CondAttrListCollection"))
        theCaloNoiseAlg.CaloNoiseFolder="/CALO/Ofl/Noise/CellNoise"
        result.merge(addFolders(configFlags,"/CALO/Ofl/Noise/CellNoise","CALO_OFL",className="CondAttrListCollection"))

        pass
    else: # Real data case:
        # for luminosity
        if fixedLumi >= 0 :
            theCaloNoiseAlg.Luminosity = fixedLumi
            log.info("offline mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else :
            theCaloNoiseAlg.Luminosity = -1
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.merge(addFolders(configFlags,lumiFolder,'CALO_OFL',className="CondAttrListCollection"))
                log.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
                result.merge(addFolders(configFlags,lumiFolder,'TRIGGER_ONL',className="CondAttrListCollection"))
                log.info("offline mode: use luminosity = f(Lumiblock) to scale pileup noise")
            theCaloNoiseAlg.LumiFolder = lumiFolder

        if configFlags.IOVDb.DatabaseInstance=="CONDBR2": # Run2 case:
            log.info("Configuring CaloNoiseCondAlg for Run2 real data processing")
            theCaloNoiseAlg.CaloNoiseFolder=""
            theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
            theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
            result.merge(addFolders(configFlags,"/LAR/NoiseOfl/CellNoise","LAR_OFL",className="CondAttrListCollection"))
            result.merge(addFolders(configFlags,"/TILE/OFL02/NOISE/CELL","TILE_OFL",className="CondAttrListCollection"))


            if configFlags.LAr.doHVCorr:
                log.info("Run2 & doLArHVCorr=True: Will rescale noise automatically for HV trips")
                theCaloNoiseAlg.useHVCorr=True
                from LArCalibUtils.LArHVScaleConfig import LArHVScaleCfg
                result.merge(LArHVScaleCfg(configFlags))
                pass
            pass
        else: #COMP200 case:
            log.info("Configuring CaloNoiseCondAlg for Run1 real data processing")
            #The noise for runs before 2012 is a different folder:
            theCaloNoiseAlg.CaloNoiseFolder="/CALO/Ofl/Noise/CellNoise"
            theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
            theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
            result.merge(addFolders(configFlags,"/CALO/Ofl/Noise/CellNoise","CALO_OFL",className="CondAttrListCollection"))
            result.merge(addFolders(configFlags,"LAR_OFL","/LAR/NoiseOfl/CellNoise",className="CondAttrListCollection"))
            result.merge(addFolders(configFlags,"TILE_OFL","/TILE/OFL02/NOISE/CELL",className="CondAttrListCollection"))

            
        pass #end of real data case
    
    result.addCondAlgo(theCaloNoiseAlg)
    return result

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.ESD
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    acc=MainServicesCfg(ConfigFlags)
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    acc.merge(PoolReadCfg(ConfigFlags))

    acc.merge(CaloNoiseCondAlgCfg(ConfigFlags))

    acc.run(10)

    #f=open('test.pkl','w')
    #acc.store(f)
    #f.close()
