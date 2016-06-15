# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Inner detector description for the ATLAS CTB(2004) Geant4 Simulation

 contacts:
       Pixel geometry
           Sebastien Correard <correard@cppm.in2p3.fr>
           Jean-Baptiste de Vivie <devivie@lal.in2p3.fr>
       SCT geometry
            Carlos Escobar <cescobar@hal.ific.uv.es>
            Peter Kodys <kodys@ific.uv.es>
            Thijs Cornelissen <tgcornel@nikhef.nl>
            Grant Gorfine <gorfine@nikhef.nl>
       TRT geometry
            Thomas Kittelman <kittel@nbi.dk>
       Inner detector digitization:
            Thijs Cornelissen <tgcornel@nikhef.nl>

"""

__author__= 'M. Gallas'

import PyG4Atlas, AtlasG4Eng

#=======================================================================
# -- PIXEL --
class Pixel(object):
    """ Describes the ATLAS CTB Pixel detector.

        to be added to the MBPSID from ctb_common.mbpsid envelope in
        two different positions:

        ctb_pixel_position1 -> position valid for tag InnerDetector-CTB-01
                               in this case the Pixel is in the CTB volume
                               and in the MBPSID envelope
        ctb_pixel_position2 -> position valid for tags
                               InnerDetector-CTB-03->09
    """
    def __init__(self):
        """ Describes the geometry.
        """
        self.ctb_pixel=PyG4Atlas.DetFacility("GeoDetector","Pixel:Pixel")
        self.ctb_pixel_position1=\
                  AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-4045.,0.,8)
        self.ctb_pixel_position2=\
                  AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-354,0.,8)
    def _initSD(self):
        """ Describes the sensitive detector.
        """
        pass

#-----------------------------------------------------------------------
# -- SCT --
class SCT(object):
    """ Describes the ATLAS CTB SCT detector.

        to be added to the MBPSID envelope from ctb_common.mbpsid
    """
    def __init__(self):
        """ Describes the geometry.

             Sets the max step size to 50 micron for consistency with G3
            in: SCT::ECSensor0
        """
        self.ctb_sct=PyG4Atlas.DetFacility("GeoDetector","SCT:SCT")
        self.ctb_sct.add_MaxStepInVolumes('SCT::ECSensor0', 0.050)
        self.ctb_sct_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(12.,0.,-9)
        self.ctb_sct.df.MoveTo(self.ctb_sct_position)

    def _initSD(self):
        """ Describes the sensitive detector.
        """
        pass

    def _initPR(self):
        """ Describes the physics regions
        """
        self.ctb_sct_PhysReg=PyG4Atlas.PhysicsReg('SCTSiliconRegion')
        self.ctb_sct_PhysReg.add_Volumes('SCT::ECSensor0')
        self.ctb_sct_PhysReg.add_Cuts('e-',0.01)
        self.ctb_sct.add_PhysicsReg(self.ctb_sct_PhysReg)

#-----------------------------------------------------------------------
# -- TRT --
class TRT(object):
    """ Describes the ATLAS CTB TRT detector.
    """
    def __init__(self):
        """ Describes the geometry.

            to be added to the CTBTRT envelope from ctb_common.ctbtrt
        """
        self.ctb_trt=PyG4Atlas.DetFacility("GeoDetector","TRT:TRT")

    def _initSD(self):
        """ Describes the sensitive detector.
        """
        pass

    def _initTRTProcess(self):
        """Activates the TRT process.
        """
        AtlasG4Eng.G4Eng.load_Lib('TRT_TR_Process')
        AtlasG4Eng.G4Eng.read_XML('TR_CTBgeomodelgeometry.xml')

#=======================================================================
