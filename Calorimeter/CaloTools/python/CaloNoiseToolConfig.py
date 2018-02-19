from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from CaloTools.CaloToolsConf import CaloNoiseToolDB
from AthenaConfiguration.CfgLogMsg import cfgLogMsg

def CaloNoiseToolCfg(inputFlags):

    result=ComponentAccumulator()

    isMC=inputFlags.get("AthenaConfiguration.GlobalFlags.isMC")
    fixedLumi=inputFlags.get("CaloRec.CaloRecConfigFlags.fixedLumiForNoise")
    useCaloLumi=inputFlags.get("CaloRec.CaloRecConfigFlags.useCaloNoiseLumi")
    
    caloNoiseToolDB=CaloNoiseToolDB("CaloNoiseTool")


    if inputFlags.get("AthenaConfiguration.GlobalFlags.isOnline"):
        #online mode:
        folder  = "/CALO/Noise/CellNoise"
        result.executeModule(addFolders,inputFlags,folder,'CALO_ONL')
        caloNoiseToolDB.FolderNames=[folder,]
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity = fixedLumi
            cfgLogMsg.info("online mode: use fixed luminosity for scaling pileup noise: %f"%fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Noise/PileUpNoiseLumi'
                result.executeModule(addFolders,inputFlags,lumiFolder,'CALO')
                caloNoiseToolDB.LumiFolderName = lumiFolder
                caloNoiseToolDB.Luminosity = -1.
                cfgLogMsg.info("online mode: use luminosity from /CALO/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                caloNoiseToolDB.Luminosity = 0.
                mlog.info("online mode: ignore pileup noise")
                pass
        result.addPublicTool(caloNoiseToolDB)
        return result

    #The not-online case:
    if isMC:
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity=fixedLumi
            cfgLogMsg.info("Luminosity (in 10**33) units used for pileup noise from CaloNoiseFlags : %f" % fixedLumi)
        else:
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.executeModule(addFolders,inputFlags,lumiFolder,'CALO_OFL')
                cfgLogMsg.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileuUpNoiseLumi to scale pileup noise")
                caloNoiseToolDB.LumiFolderName = lumiFolder
                caloNoiseToolDB.Luminosity=-1.
            else:
                estimatedLumi=inputFlags.get("AthenaConfiguration.GlobalFlags.estimatedLuminosity")
                caloNoiseToolDB.Luminosity=estimatedLumi/1e+33
                cfgLogMsg.info("  Luminosity (in 10**33) units used for pileup noise from global flags: %f"%caloNoiseToolDB.Luminosity)


        folders  = (("CALO_OFL","/CALO/Ofl/Noise/CellNoise"),
                    ("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                    ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                    ) 
        pass
    else: # Real data case:
        # for luminosity
        if fixedLumi >= 0 :
            caloNoiseToolDB.Luminosity = fixedLumi
            mlog.info("offline mode: use fixed luminosity for scaling pileup noise: %f"%fixedLumi)
        else :
            caloNoiseToolDB.Luminosity = -1
            if useCaloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                result.executeModule(addFolders,inputFlags,lumiFolder,'CALO_OFL')
                cfgLogMsg.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
                result.executeModule(addFolders,inputFlags,lumiFolder,'TRIGGER_ONL');
                cfgLogMsg.info("offline mode: use luminosity = f(Lumiblock) to scale pileup noise")
                caloNoiseToolDB.LumiFolderName = lumiFolder


        folders=[("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                 ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                 ]
        
        if inputFlags.get("IOVDbSvc.IOVDbConfigFlags.DatabaseInstance")=="COMP200":
            folders.append(("CALO_OFL","/CALO/Ofl/Noise/CellNoise")),
        
        #Fixme: Add rescaling of noise based on HV! 


        pass #end of real data case
    
    for (db,fldr) in folders:
        result.executeModule(addFolders,inputFlags,fldr,db)
    
    caloNoiseToolDB.FolderNames=[f[1] for f in folders]    

    result.addPublicTool(caloNoiseToolDB)
    
    return result
