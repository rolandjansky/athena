# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
LAr+Tile detector description for the ATLAS CTB(2004) Geant4 Simulation.

 contacts:
       LAr:  Guillaume Unal <guillaume.unal@cern.ch>
       Tile: Vakho Tsulaia  <Vakhtang.Tsulaia@cern.ch>
             Gia Khoriauli  <gia.khoriauli@cern.ch>
"""

__author__= 'M. Gallas'

import PyG4Atlas, AtlasG4Eng


class LAr(object):
    """ Implements the LAr CTB description.
    """
    def __init__(self):
        AtlasG4Eng.G4Eng.load_Lib("LArG4HitManagement")
        self.ctb_lar=PyG4Atlas.DetFacility("GeoDetector","LArMgr:LArMgr")
        self.ctb_lar_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-10.,0.,0.)
        self.ctb_lar.df.MoveTo(self.ctb_lar_position)
    def _initSD(self,mode):
        """ Describes the sensitive detector

            modes:

            0 --> Normal LAr sensitive detector
            1 --> LAr sensitive detector for calibration hits
        """
        # sensitive detector with GeoModel:
        if(mode==0):
            # normal mode
            self.ctb_lar.add_SenDetector('LArG4SD',\
                                'LAr::EMB::STAC',\
                                'LAr::EMB::STAC',\
                                'LArMgr::LAr::EMB::STAC')
            self.ctb_lar.add_SenDetector('LArG4SD',\
                                'LAr::Barrel::Presampler::Module',\
                                'LAr::Barrel::Presampler::Module',\
                                'LArMgr::LAr::Barrel::Presampler::Module')
        elif(mode==1):
            # calibration mode
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'TBSTACMult',\
                                       'TBSTACMult',\
                                       'LArMgr::LAr::EMB::STAC')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                'TBBarrelPresamplerMult',\
                                'TBBarrelPresamplerMult',\
                                'LArMgr::LAr::Barrel::Presampler::Module')
            # Calibration SDs
            """ LAr::TBBarrelCryostat::Dead
            """
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrelCryostat::Dead',\
                                       'LAr::TBBarrelCryostat::Dead',\
                                       'LArMgr::LAr::TBBarrel::Cryostat::*')
            """ LAr::Barrel::Presampler::Inactive
            """
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LArMgr::LAr::Barrel::Presampler::Prep*')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LArMgr::LAr::Barrel::Presampler::Anode*')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LAr::TBBarrel::Presampler::Inactive',\
                                           'LArMgr::LAr::Barrel::Presampler::Cathode*')
            """ LAr::Barrel::Presampler::Dead
            """
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::Sector')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::MotherBoard')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::ProtectionShell')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::MotherBoard')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::Connectics')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::Rail')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LAr::TBBarrel::Presampler::Dead',\
                                       'LArMgr::LAr::Barrel::Presampler::ProtectionPlate')
            """ LAr::Barrel::Inactive
            """
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Inactive',\
                                       'LAr::TBBarrel::Inactive',\
                                       'LArMgr::LAr::EMB::*Abs::*')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Inactive',\
                                       'LAr::TBBarrel::Inactive',\
                                       'LArMgr::LAr::EMB::Electrode::*')
            """ LAr::Barrel::Dead
            """
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::ECAM')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::TELF')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::TELFB')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::MOAC')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::CAAC')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::SUMB')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::GTENF')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::GTENB')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::FrontBack::Electrode')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::FrontBack::G10')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::FrontBack::Steel')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::FrontBack::Absorber2')
            self.ctb_lar.add_SenDetector('LArG4H8SD',\
                                       'LAr::TBBarrel::Dead',\
                                       'LAr::TBBarrel::Dead',\
                                       'LArMgr::LAr::EMB::FrontBack::Absorber')
    def _initPR(self):
        """ Describes the physics regions
        """
        self.ctb_lar_PhysReg=PyG4Atlas.PhysicsReg('EMB')
        self.ctb_lar_PhysReg.add_Volumes('LArMgr::LAr::EMB::STAC')
        self.ctb_lar_PhysReg.add_Cuts('e-',0.03)
        self.ctb_lar_PhysReg.add_Cuts('e+',0.03)
        self.ctb_lar_PhysReg.add_Cuts('gamma',0.03)
        self.ctb_lar.add_PhysicsReg(self.ctb_lar_PhysReg)

# -- TILE --
# envelope: CALO

class TileDet(object):
    """ Tile:Tile GeoModel detector

        to be added to the CALO envelope

        CTB-3B   --> Mode for CTB with a z-shift = 0.0 (only 3B)
        CTB-3B3E --> Mode for CTB with a z-shift = 1780.25 (July 2004)
                     3 barrels + 3 extended barrels
        TB --> Mode for old Tile TB  with a z-shift = 0.0
    """
    # detector geometry:
    def __init__(self,mode):
        self.ctb_tile = PyG4Atlas.DetFacility("GeoDetector","Tile:Tile")
        if(mode=='CTB-3B3E'):
            self.ctb_tile_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,1780.25)
        elif(mode=='CTB-3B'):
            self.ctb_tile_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
        elif(mode=='TB'):
            self.ctb_tile_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
        self.ctb_tile.df.MoveTo(self.ctb_tile_position)
    # sensitive detector:
    def SD(self,mode):
        """
           Implements the Tile SD.

           0 --> Normal Tile sensitive detector
           1 --> Tile sensitive detector for calibration hits
        """
        if(mode==0):
            # normal mode
            self.ctb_tile.add_SenDetector('TileGeoG4SD','TileCTBGeoG4SD',\
                               'TileCTBGeoG4SD','Tile::Scintillator')
        elif(mode==1):
            # calibration mode
            self.ctb_tile.add_SenDetector('TileGeoG4SD:TileGeoG4Calib',\
             'TileCTBGeoG4CalibSD','TileCTBGeoG4CalibSD','Tile::Scintillator')
            self.ctb_tile_SenDetectorCalib=AtlasG4Eng.G4Eng.Dict_SenDetector.get('TileCTBGeoG4CalibSD')
            tile_volumes=['Tile::TileTBEnv','Tile::Barrel',
            'Tile::BarrelModule','Tile::FrontPlate',
            'Tile::EndPlate1','Tile::EPHole1','Tile::EndPlate2','Tile::EPHole2',
            'Tile::GirderMother','Tile::GirderIron','Tile::GirderAluminium',
            'Tile::GirderElectronics','Tile::Absorber','Tile::AbsorberChild',
            'Tile::Period','Tile::Glue','Tile::Wrapper',
            'Tile::EBarrel','Tile::EBarrelModule','Tile::EndPlateSh',
            'Tile::ITC','Tile::ITCModule','Tile::Plug1Module',
            'Tile::FrontPlateSh','Tile::Plug2Module',
            'Tile::Finger','Tile::FingerModule','Tile::FingerIron',
            'Tile::FingerAluminum','Tile::FingerElectronics']
            for i_v in tile_volumes:
                self.ctb_tile_SenDetectorCalib.add_Volume(i_v)

#=======================================================================
