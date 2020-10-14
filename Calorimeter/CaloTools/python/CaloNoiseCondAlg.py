# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.AlgSequence import AthSequencer
from IOVDbSvc.CondDB import conddb
from AthenaCommon.Logging import logging 


def CaloNoiseCondAlg(noisetype="totalNoise"):
    if noisetype not in ("electronicNoise","pileupNoise","totalNoise"):
        raise RuntimeError("Requested noise of unknown type %s" % noisetype)


    noiseAlgName="Calo_"+noisetype+"Alg"
    condSeq = AthSequencer("AthCondSeq")

    if hasattr (condSeq,noiseAlgName):
        return getattr(condSeq,noiseAlgName)
 
    if (conddb.isMC):
        alg = _CaloNoiseCondAlgMC(noiseAlgName,noisetype)
    else: 
        alg = _CaloNoiseCondAlgData(noiseAlgName,noisetype)
    condSeq += alg
    return alg


def _CaloNoiseCondAlgMC(noiseAlgName,noisetype):
    mlog = logging.getLogger('CaloNoiseCondMC')

    from CaloTools.CaloToolsConf import CaloNoiseCondAlg 
    theCaloNoiseAlg=CaloNoiseCondAlg(noiseAlgName,OutputKey=noisetype)
    from CaloTools.CaloNoiseFlags import jobproperties
    if jobproperties.CaloNoiseFlags.FixedLuminosity() >= 0 :
        theCaloNoiseAlg.Luminosity=jobproperties.CaloNoiseFlags.FixedLuminosity()
        mlog.info("  Luminosity (in 10**33) units used for pileup noise from CaloNoiseFlags : %f", theCaloNoiseAlg.Luminosity)
    else:
        if jobproperties.CaloNoiseFlags.UseCaloLuminosity():
            lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
            conddb.addFolder('CALO_OFL',lumiFolder,className="CondAttrListCollection")
            mlog.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileuUpNoiseLumi to scale pileup noise")
            theCaloNoiseAlg.LumiFolder = lumiFolder
            theCaloNoiseAlg.Luminosity=-1.
        else:
            from AthenaCommon.BeamFlags import jobproperties
            theCaloNoiseAlg.Luminosity=jobproperties.Beam.estimatedLuminosity()/1e+33
            mlog.info("  Luminosity (in 10**33) units used for pileup noise from BeamFlags : %f", theCaloNoiseAlg.Luminosity)
            pass
        pass
    theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
    conddb.addFolder("LAR_OFL","/LAR/NoiseOfl/CellNoise",className="CondAttrListCollection")
    theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
    conddb.addFolder("TILE_OFL","/TILE/OFL02/NOISE/CELL",className="CondAttrListCollection")
    theCaloNoiseAlg.CaloNoiseFolder="/CALO/Ofl/Noise/CellNoise"
    conddb.addFolder("CALO_OFL","/CALO/Ofl/Noise/CellNoise",className="CondAttrListCollection")
    
    return theCaloNoiseAlg




def _CaloNoiseCondAlgData(noiseAlgName,noisetype):
    mlog = logging.getLogger('CaloNoiseCondData')
    from CaloTools.CaloNoiseFlags import jobproperties
    fixedLumi = jobproperties.CaloNoiseFlags.FixedLuminosity()
    caloLumi  = jobproperties.CaloNoiseFlags.UseCaloLuminosity()

    from CaloTools.CaloToolsConf import CaloNoiseCondAlg     
    theCaloNoiseAlg=CaloNoiseCondAlg(noiseAlgName,OutputKey=noisetype)
    
    if conddb.isOnline:
        folder = "/CALO/Noise/CellNoise"
        theCaloNoiseAlg.CaloNoiseFolder=folder
        theCaloNoiseAlg.LArNoiseFolder=""
        theCaloNoiseAlg.TileNoiseFolder=""
        conddb.addFolder('CALO_ONL',folder,className="CondAttrListCollection")
        if fixedLumi >= 0 :
            theCaloNoiseAlg.Luminosity = fixedLumi
            mlog.info("online mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else:
            if caloLumi:
                lumiFolder='/CALO/Noise/PileUpNoiseLumi'
                conddb.addFolder('CALO',lumiFolder,className="CondAttrListCollection")
                theCaloNoiseAlg.LumiFolder = lumiFolder
                theCaloNoiseAlg.Luminosity = -1.
                mlog.info("online mode: use luminosity from /CALO/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                theCaloNoiseAlg.Luminosity = 0.
                mlog.info("online mode: ignore pileup noise") 
                pass
    else: #regular offline mode
        if conddb.dbdata=="CONDBR2": # Run2 case:
            theCaloNoiseAlg.CaloNoiseFolder=""
            theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
            theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
            conddb.addFolder("LAR_OFL","/LAR/NoiseOfl/CellNoise",className="CondAttrListCollection")
            conddb.addFolder("TILE_OFL","/TILE/OFL02/NOISE/CELL",className="CondAttrListCollection")
            from CaloRec.CaloCellFlags import jobproperties
            if jobproperties.CaloCellFlags.doLArHVCorr():
                mlog.info("Run2 & doLArHVCorr=True: Will rescale noise automatically for HV trips")
                theCaloNoiseAlg.useHVCorr=True
                from LArConditionsCommon import LArHVDB  # noqa: F401
                pass
            pass
        else: #COMP200 case:
            #The noise for runs before 2012 is in different folders:
            theCaloNoiseAlg.CaloNoiseFolder="/CALO/Ofl/Noise/CellNoise"
            theCaloNoiseAlg.LArNoiseFolder="/LAR/NoiseOfl/CellNoise"
            theCaloNoiseAlg.TileNoiseFolder="/TILE/OFL02/NOISE/CELL"
            conddb.addFolder("CALO_OFL","/CALO/Ofl/Noise/CellNoise",className="CondAttrListCollection")
            conddb.addFolder("LAR_OFL","/LAR/NoiseOfl/CellNoise",className="CondAttrListCollection")
            conddb.addFolder("TILE_OFL","/TILE/OFL02/NOISE/CELL",className="CondAttrListCollection")
            

        # for luminosity
        if fixedLumi >= 0 :
            theCaloNoiseAlg.Luminosity = fixedLumi
            mlog.info("offline mode: use fixed luminosity for scaling pileup noise: %f", fixedLumi)
        else:
            theCaloNoiseAlg.Luminosity = -1
            if caloLumi:
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                conddb.addFolder('CALO_OFL',lumiFolder,className="CondAttrListCollection")
                mlog.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileUpNoiseLumi to scale pileup noise")
            else:
                lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
                conddb.addFolder('TRIGGER_ONL',lumiFolder,className="CondAttrListCollection")
                mlog.info("offline mode: use luminosity = f(Lumiblock) to scale pileup noise")
            theCaloNoiseAlg.LumiFolder = lumiFolder
                
    return theCaloNoiseAlg
