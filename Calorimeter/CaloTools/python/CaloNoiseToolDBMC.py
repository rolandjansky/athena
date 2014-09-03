# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# example of use of CaloNoiseToolDB returning cell noise from Cool DB

from AthenaCommon.Logging import logging 
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *

# import the base class
from CaloTools.CaloToolsConf import CaloNoiseToolDB

class CaloNoiseToolDBMC(CaloNoiseToolDB) :

    def __init__(self, name="CaloNoiseToolDBMC"): 
        # call base class constructor
        CaloNoiseToolDB.__init__( self, name)

        mlog = logging.getLogger( 'CaloNoiseToolDBMC::__init__ ' )
        mlog.info("entering")

        from IOVDbSvc.CondDB import conddb

        # do the configuration
        from CaloTools.CaloNoiseFlags import jobproperties
        if jobproperties.CaloNoiseFlags.FixedLuminosity() >= 0 :
            self.Luminosity=jobproperties.CaloNoiseFlags.FixedLuminosity()
            mlog.info("  Luminosity (in 10**33) units used for pileup noise from CaloNoiseFlags : %f"%self.Luminosity)
        else:
            if jobproperties.CaloNoiseFlags.UseCaloLuminosity():
                lumiFolder='/CALO/Ofl/Noise/PileUpNoiseLumi'
                conddb.addFolder('CALO_OFL',lumiFolder)
                mlog.info("offline mode: use luminosity from /CALO/Ofl/Noise/PileuUpNoiseLumi to scale pileup noise")
                self.LumiFolderName = lumiFolder
                self.Luminosity=-1.
            else:
                from AthenaCommon.BeamFlags import jobproperties
                self.Luminosity=jobproperties.Beam.estimatedLuminosity()/1e+33
                mlog.info("  Luminosity (in 10**33) units used for pileup noise from BeamFlags : %f"%self.Luminosity)

        folders  = (("CALO_OFL","/CALO/Ofl/Noise/CellNoise"),
                    ("LAR_OFL","/LAR/NoiseOfl/CellNoise"),
                    ("TILE_OFL","/TILE/OFL02/NOISE/CELL")
                   ) 
        for (db,fldr) in folders:
            conddb.addFolder(db,fldr)
        CaloNoiseToolDB.FolderNames=[f[1] for f in folders]
