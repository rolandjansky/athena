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
        self.ctb_lar=PyG4Atlas.DetFacility("GeoDetector","LArMgr:LArMgr")
        self.ctb_lar_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-10.,0.,0.)
        self.ctb_lar.df.MoveTo(self.ctb_lar_position)
    def _initSD(self,mode):
        """ Describes the sensitive detector

            modes:

            0 --> Normal LAr sensitive detector
            1 --> LAr sensitive detector for calibration hits
        """
        pass # sensitive detector with GeoModel

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
    def SD(self):
        pass
#=======================================================================
