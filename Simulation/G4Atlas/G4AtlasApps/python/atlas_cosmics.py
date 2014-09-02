# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS stuff for the cosmics setups
"""

__author__  = 'A. Dell`Acqua, M. Gallas'

import math
import PyG4Atlas, AtlasG4Eng


## ATLAS ancillary scintillators for the IDET cosmics
## TODO: Can this be removed now that the IDET cosmics layouts are unsupported?
class IDcosmicS1(object):
    """
      IDCS1 = top scintillator for the IDET cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","IDCS1")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1):
            DF.df.SetDx(500.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(700.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,1150.,0.))
        elif(cosmicSetup==2):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(700.)
            DF.df.RotateZ(0.3925)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(440.,1063.,0.))
        elif(cosmicSetup==3):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(70.0)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(476.,115.,0.))
        elif(cosmicSetup==4):
            DF.df.SetDx(300.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(750.)
            DF.df.RotateZ(0.3925)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(440.,1063.,0.))
        elif(cosmicSetup==5):
            DF.df.SetDx(150.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(300.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(476.,1150.,-400.))
        elif(cosmicSetup==6):
            DF.df.SetDx(200.)
            DF.df.SetDy(12.5)
            DF.df.SetDz(720.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(783.,1987.5,0.))
        elif(cosmicSetup==7):
            DF.df.SetDx(200.)
            DF.df.SetDy(12.5)
            DF.df.SetDz(720.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(783.,1987.5,0.))
        elif(cosmicSetup==8):
            DF.df.SetDx(200.)
            DF.df.SetDy(10.)
            DF.df.SetDz(700.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-390.,1370.,-3150.))

class IDcosmicS2(object):
    """
      IDCS2 = midle scintillator for the IDET cosmics.
    """
    def __init__(self,cosmicSetup):
         DF=PyG4Atlas.DetFacility("ResizeableBox","IDCS2")
         DF.df.SetMaterial("Scintillator")
         self.DF=DF
         if(cosmicSetup==1):
             DF.df.SetDx(150.)
             DF.df.SetDy(2.5)
             DF.df.SetDz(700.)
             DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.))
         elif(cosmicSetup==2):
             DF.df.SetDx(150.)
             DF.df.SetDy(2.5)
             DF.df.SetDz(700.)
             DF.df.RotateZ(0.3925)
             DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.))
         elif(cosmicSetup==3):
             DF.df.SetDx(150.)
             DF.df.SetDy(2.5)
             DF.df.SetDz(700.)
             DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.))
         elif(cosmicSetup==7):
             DF.df.SetDx(200.)
             DF.df.SetDy(12.5)
             DF.df.SetDz(720.)
             DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-1003.,-3762.5,0.))
         elif(cosmicSetup==8):
             DF.df.SetDx(200.)
             DF.df.SetDy(10.)
             DF.df.SetDz(700.)
             DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-800.,1370.,-3150.))

class IDcosmicS3(object):
    """
      IDCS3 = bottom scintillator for the IDET cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","IDCS3")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(700.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,-1150.,0.))
        elif(cosmicSetup==2):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(700.)
            DF.df.RotateZ(0.3925)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-440.,-1063.,0.))
        elif(cosmicSetup==3):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(700.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-476.,-1150.,0.))
        elif(cosmicSetup==4):
            DF.df.SetDx(300.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(750.)
            DF.df.RotateZ(0.3925)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-440.,-1063.,0.))
        elif(cosmicSetup==5):
            DF.df.SetDx(250.)
            DF.df.SetDy(2.5)
            DF.df.SetDz(300.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-476.,-1150.,-400.))
        elif(cosmicSetup==6):
            DF.df.SetDx(200.)
            DF.df.SetDy(12.5)
            DF.df.SetDz(720.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-608.,-1612.5,0.))
        elif(cosmicSetup==7):
            DF.df.SetDx(200.)
            DF.df.SetDy(12.5)
            DF.df.SetDz(720.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-608.,-1612.5,0.))
        elif(cosmicSetup==8):
            DF.df.SetDx(250.)
            DF.df.SetDy(300.)
            DF.df.SetDz(5.)
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-170.,110.,-655.))

# - ATLAS ancillary scintillators for the Pixel EndCap A cosmics   -

class PixelECcosmicS1(object):
    """
      PixelECCS1 = top scintillator for the PixelEndCap cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","PixelECCS1")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1 or cosmicSetup==2):
            DF.df.SetDx(356.)
            DF.df.SetDy(229.)
            DF.df.SetDz(5.)
            DF.df.RotateZ(math.radians(-22.5))
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,215.))

class PixelECcosmicS2(object):
    """
      PixelECCS2 = bottom right scintillator for the PixelEndCap cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","PixelECCS2")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1):
            DF.df.SetDx(254.)
            DF.df.SetDy(457.)
            DF.df.SetDz(5.)
            DF.df.RotateZ(math.radians(-22.5))
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(244.8,-101.4,1309.))

class PixelECcosmicS3(object):
    """
      PixelECCS3 = bottom scintillator for the PixelECEndCap cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","PixelECCS3")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1 or cosmicSetup==2):
            DF.df.SetDx(356.)
            DF.df.SetDy(229.)
            DF.df.SetDz(5.)
            DF.df.RotateZ(math.radians(-22.5))
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-129.3,53.6,1415.))

class PixelECcosmicS4(object):
    """
      PixelECCS4 = bottom left scintillator for the PixelEndCap cosmics.
    """
    def __init__(self,cosmicSetup):
        DF=PyG4Atlas.DetFacility("ResizeableBox","PixelECCS4")
        DF.df.SetMaterial("Scintillator")
        self.DF=DF
        if(cosmicSetup==1):
            DF.df.SetDx(254.)
            DF.df.SetDy(457.)
            DF.df.SetDz(5.)
            DF.df.RotateZ(math.radians(-22.5))
            DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-522.,216.2,1309.))

class PixelECcosmicIron(object):
    """
      PixelECCI = iron block below cooling pipes on the PP0 side of the PSQP
    """
    def __init__(self):
        DF=PyG4Atlas.DetFacility("ResizeableBox","PixelECCI")
        DF.df.SetMaterial("Iron")
        self.DF=DF
        DF.df.SetDx(170.)
        DF.df.SetDy(135.)
        DF.df.SetDz(60.)
        DF.df.RotateZ(math.radians(-22.5))
        DF.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(184.8,-76.5,1095.))
