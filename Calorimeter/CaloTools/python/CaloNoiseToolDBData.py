# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# example of use of CaloNoiseToolDB returning cell noise from Cool DB

from AthenaCommon.Logging import logging 
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

from CaloRec.CaloCellFlags import jobproperties

# import the base class
from CaloTools.CaloToolsConf import CaloNoiseToolDB



class CaloNoiseToolDBData(CaloNoiseToolDB) :

    def __init__(self, name="CaloNoiseToolDBData"): 
        # call base class constructor
        CaloNoiseToolDB.__init__( self, name)

        mlog = logging.getLogger( 'CaloNoiseToolDBData::__init__ ' )
        mlog.info("entering")

        from IOVDbSvc.CondDB import conddb

        from CaloTools.CaloNoiseFlags import jobproperties
        fixedLumi = jobproperties.CaloNoiseFlags.FixedLuminosity()
        caloLumi  = jobproperties.CaloNoiseFlags.UseCaloLuminosity()

        if conddb.isOnline:
            folder  = "/CALO/Noise/CellNoise"
            conddb.addFolder('CALO_ONL',folder)
            CaloNoiseToolDB.FolderNames=[folder,]
            if fixedLumi >= 0 :
                CaloNoiseToolDB.Luminosity = fixedLumi
                mlog.info("online mode: use fixed luminosity for scaling pileup noise: %f"%fixedLumi)
            else:
                if caloLumi:
                    lumiFolder='/CALO/Noise/PileUpNoiseLumi'
                    conddb.addFolder('CALO',lumiFolder)
                    CaloNoiseToolDB.LumiFolderName = lumiFolder
                    CaloNoiseToolDB.Luminosity = -1.
                    mlog.info("online mode: use luminosity from /CALO/Noise/PileUpNoiseLumi to scale pileup noise")
                else:
                    CaloNoiseToolDB.Luminosity = 0.
                    mlog.info("online mode: ignore pileup noise")
        else:
            folders=[("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                     ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                     ]
            #The noise for runs before 2012 is a different folder:
            if conddb.dbdata=="COMP200":
                folders.append(("CALO_OFL","/CALO/Ofl/Noise/CellNoise")),
            

            for (db,fldr) in folders:
                conddb.addFolder(db,fldr)
            CaloNoiseToolDB.FolderNames=[f[1] for f in folders]

            # for luminosity
            if fixedLumi >= 0 :
                CaloNoiseToolDB.Luminosity = fixedLumi
                mlog.info("offline mode: use fixed luminosity for scaling pileup noise: %f"%fixedLumi)
            else :
                CaloNoiseToolDB.Luminosity = -1
                if caloLumi:
                    lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                    conddb.addFolder('CALO_OFL',lumiFolder)
                    mlog.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileUpNoiseLumi to scale pileup noise")
                else:
                    lumiFolder = '/TRIGGER/LUMI/LBLESTONL'
                    conddb.addFolder('TRIGGER_ONL',lumiFolder);
                    mlog.info("offline mode: use luminosity = f(Lumiblock) to scale pileup noise")
                CaloNoiseToolDB.LumiFolderName = lumiFolder

            #In run 2 we automatically rescale the noise for HV changes
            if  conddb.dbdata=="CONDBR2": #or True 
                if jobproperties.CaloCellFlags.doLArHVCorr():
                    mlog.info("Run2 & doLArHVCorr=True: Will rescale noise automatically for HV trips")
                    CaloNoiseToolDB.RescaleForHV=True
                    from LArCellRec.LArCellHVCorrDefault import LArCellHVCorrDefault
                    CaloNoiseToolDB.LArHVCellCorrTool=LArCellHVCorrDefault()


