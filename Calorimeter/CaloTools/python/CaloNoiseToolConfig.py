from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from CaloTools.CaloToolsConf import CaloNoiseToolDB
from AthenaCommon.Logging import logging

def CaloNoiseToolCfg(configFlags):

    log = logging.getLogger(__name__)
    result=ComponentAccumulator()

    isMC=configFlags.get("global.isMC")
    fixedLumi=configFlags.get("Calo.Noise.fixedLumiForNoise")
    useCaloLumi=configFlags.get("Calo.Noise.useCaloNoiseLumi")
    
    caloNoiseToolDB=CaloNoiseToolDB("CaloNoiseTool")


    if configFlags.get("global.isOnline"):
        #online mode:
        folder  = "/CALO/Noise/CellNoise"
        result.addConfig(addFolders,configFlags,inputFlags,folder,'CALO_ONL')
        caloNoiseToolDB.FolderNames=[folder,]
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity = fixedLumi
            log.info("online mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Noise/PileUpNoiseLumi'
                result.addConfig(addFolders,configFlags,lumiFolder,'CALO')
                caloNoiseToolDB.LumiFolderName = lumiFolder
                caloNoiseToolDB.Luminosity = -1.
                log.info("online mode: use luminosity from /CALO/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                caloNoiseToolDB.Luminosity = 0.
                log.info("online mode: ignore pileup noise")
                pass
        result.addPublicTool(caloNoiseToolDB)
        return result

    #The not-online case:
    if isMC:
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity=fixedLumi
            log.info("Luminosity (in 10**33) units used for pileup noise from CaloNoiseFlags : %f", fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.addConfig(addFolders,configFlags,lumiFolder,'CALO_OFL')
                log.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileuUpNoiseLumi to scale pileup noise")
                caloNoiseToolDB.LumiFolderName = lumiFolder
                caloNoiseToolDB.Luminosity=-1.
            else:
                estimatedLumi=configFlags.get("global.estimatedLuminosity")
                caloNoiseToolDB.Luminosity=estimatedLumi/1e+33
                log.info("  Luminosity (in 10**33) units used for pileup noise from global flags: %f", caloNoiseToolDB.Luminosity)


        folders  = (("CALO_OFL","/CALO/Ofl/Noise/CellNoise"),
                    ("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                    ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                    ) 
        pass
    else: # Real data case:
        # for luminosity
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity = fixedLumi
            log.info("offline mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else :
            caloNoiseToolDB.Luminosity = -1
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.addConfig(addFolders,configFlags,lumiFolder,'CALO_OFL')
                log.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
                result.addConfig(addFolders,configFlags,lumiFolder,'TRIGGER_ONL')
                log.info("offline mode: use luminosity = f(Lumiblock) to scale pileup noise")
                caloNoiseToolDB.LumiFolderName = lumiFolder


        folders=[("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                 ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                 ]
        
        if configFlags.get("IOVDb.DatabaseInstance")=="COMP200":
            folders.append(("CALO_OFL","/CALO/Ofl/Noise/CellNoise")),
        
        #Fixme: Add rescaling of noise based on HV! 


        pass #end of real data case
    
    for (db,fldr) in folders:
        result.addConfig(addFolders,configFlags,fldr,db)
    
    caloNoiseToolDB.FolderNames=[f[1] for f in folders]    

    result.addPublicTool(caloNoiseToolDB)
    
    return result
