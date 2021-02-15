# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
- This module contains the common volumes, sub-detector envelopes,
  magnets, beam pipes, volumes for MCTruth and other common stuff
  (ancillary scintillators and dead material) for the CTB(2004)
  simulation.
- The test beam layout here described follows the Note:
  "ATLAS Barrel Combined Run in 2004 Test Beam Setup and its evolutions"
         https://edms.cern.ch/file/406980/3/TB2004_layout_v06.pdf
- Some classes are also provided to setup in a easy way the different
  test beam layouts.

"""

__author__ = 'M. Gallas'


import math
import PyG4Atlas, AtlasG4Eng
from PyG4Atlas import DetFacilityT

#=======================================================================
# - CTB top volumes  ---------------------------------------------------
__doc__+="""\n\n         -- CTB TOP VOLUMES --  \n"""
__doc__+="""\n ctb      = World volume CTB"""
ctb=PyG4Atlas.DetFacility("ResizeableBox","CTB")
ctb.df.SetDx(58000.)
ctb.df.SetDy(7000.)
ctb.df.SetDz(7000.)
#
__doc__+="""\n idet     = common IDET volume for Pixel, SCT and TRT
                          MCTruth strategies."""
idet=PyG4Atlas.DetFacility("ResizeableBox","IDET")
idet.df.SetDx(2380.)
idet.df.SetDy(1000.)
idet.df.SetDz(1300.)

#=======================================================================
# - CTB subdetector envelopes   ---------------------------------------
__doc__+="""\n\n         -- CTB SUB-DETECTOR ENVELOPES --  \n"""
# - TRT envelope
__doc__+="""\n ctbtrt   = envelope CTBTRT for the TRT detector."""
ctbtrt=PyG4Atlas.DetFacility("ResizeableTubs","CTBTRT")
ctbtrt.df.SetRmin(550.)
ctbtrt.df.SetRmax(1090.)
ctbtrt.df.SetDz(712.75)
ctbtrt.df.SetStartPhi(math.radians(-19.0))
ctbtrt.df.SetDeltaPhi(math.radians(31.0))
# InnerDetector-CTB-04/05/06 Oracle tags use position1
ctbtrt_position1=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1152.,-8.,550.)
# InnerDetector-CTB-03/09 Oracle tags use position2
ctbtrt_position2=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1152.,112.,550.)
# InnerDetector-CTB-01 Oracle tag uses position3
ctbtrt_position3=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1152.,96.,550.)
#ctbtrt.df.MoveTo(ctbtrt_position) # We do it in CtbSim.py

# - CALO envelope (LAr and Tile)
__doc__+="""\n calo     = envelope CALO for LAr and Tile detector."""
calo=PyG4Atlas.DetFacility("GenericPCON","CALO")
calo.df.SetPhiMin(math.radians(-27))
calo.df.SetDeltaPhi(math.radians(60))
calo.df.SetNSurfaces(3)
calo.df.SetZSurface(0,-2830.)
calo.df.SetZSurface(1,-1050.)
calo.df.SetZSurface(2,6310.)
calo.df.SetRinSurface(0,2269.)
calo.df.SetRinSurface(1,950.)
calo.df.SetRinSurface(2,950.)
calo.df.SetRouSurface(0,5140.)
calo.df.SetRouSurface(1,5140.)
calo.df.SetRouSurface(2,5140.)
# - MUON envelope
__doc__+="""\n muon     = MUON envelope for the Muon system."""
muon=PyG4Atlas.DetFacility("ResizeableBox","MUON")
muon.df.SetDx(22702)
muon.df.SetDy(4100.)
muon.df.SetDz(2800.)
muon_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(34900.,0.,0.)
muon.df.MoveTo(muon_position)


class ctb_top_volumes:
    """ Implements the World volume (CTB), together with the
        IDET and MUON volumes.

        These three volumes must be present in any configuration.
    """
    def __init__(self):
        AtlasG4Eng.G4Eng.add_DetFacility(ctb,ctb)    # Added as World
        AtlasG4Eng.G4Eng.add_DetFacility(idet,ctb)   # Added to World
        AtlasG4Eng.G4Eng.add_DetFacility(muon,ctb)

#=======================================================================
# - Beam-pipes  --------------------------------------------------------
__doc__+="""\n\n         -- BEAM-PIPES and MYLAR EQUIVALENT -- \n"""
# - Vacuum beam pipe 1 -
__doc__+="""\n beampipe1     = BEAMPIPE1 vacuum tube."""
beampipe1=PyG4Atlas.DetFacility("ResizeableTubs","BEAMPIPE1")
beampipe1.df.SetRmin(0.)
beampipe1.df.SetRmax(50.)
beampipe1.df.SetDz(1500.)
beampipe1.df.SetMaterial('Vacuum')
beampipe1_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-15486.,0.,0.)
beampipe1.df.RotateY(math.radians(-90.))

# - Vacuum beam pipe 2 -
__doc__+="""\n beampipe2     = BEAMPIPE2 vacuum tube."""
beampipe2=PyG4Atlas.DetFacility("ResizeableTubs","BEAMPIPE2")
beampipe2.df.SetRmin(0.)
beampipe2.df.SetRmax(50.)
beampipe2.df.SetDz(1500.)
beampipe2.df.SetMaterial('Vacuum')
beampipe2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-10755.,0.,0.)
beampipe2.df.RotateY(math.radians(-90.))

# - Mylar equivalent (14 windows with 120 micras)
__doc__+="""\n mylarequiv    = MYLAREQUIV (14 windows with 120 micras each)."""
mylarequiv=PyG4Atlas.DetFacility("ResizeableTubs","MYLAREQUIV")
mylarequiv.df.SetRmin(0.)
mylarequiv.df.SetRmax(100.)
mylarequiv.df.SetDz(0.00168)
mylarequiv.df.SetMaterial('Mylar')
mylarequiv.df.RotateY(math.radians(-90.))
mylarequiv_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-13980.,0.,0.)
mylarequiv.df.MoveTo(mylarequiv_position)

# - Far upstream material by Stathes
class LArFarUpstreamMaterial(object):
    """ This represents the far upstream material under estimateted
        and found during the LAr studies.

        (13.35mm ~0.15RadLengths) Al Rad Length=8.9cm

        Option 0 --> the material is placed at -20000 mm inside the CTB
                     volume.
                      This option is used if:
                          SimFlags.LArFarUpstreamMaterial  is True
                     and
                          SimFlags.BeamConditions is False

        Option 1 --> the material is placed at -1020 mm inside the IDET
                     volume and in front of the EquivUpstreamMaterial
                     which represents the beam line material when the
                     beam profiles are used for the ctbh8_combined layout.
                      This option is used if:
                          SimFlags.LArFarUpstreamMaterial  is True
                     and
                          SimFlags.BeamConditions is True

        In both cases we record the particles and momenta in
        TrackRecordCollection#LArFarUpstreamMaterialExitLayer

    """
    def __init__(self,option):
        self.material=PyG4Atlas.DetFacility("ResizeableBox",
                                            "LARFARUPSTREAMMATERIAL")
        self.material.df.SetDx(6.675)
        self.material.df.SetDy(50.)
        self.material.df.SetDz(50.)
        self.material.df.SetMaterial("Aluminum")
        if(option==0):
            self.material.position=AtlasG4Eng.G4Eng.gbl.Hep3Vector(-20000.,0.,0.)
            self.material.df.MoveTo(self.material.position)
            AtlasG4Eng.G4Eng.add_DetFacility(self.material,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.log.info(' LArFarUpstreamMaterial is placed inside CTB '
                                      'at -20000 mm. ')
        elif(option==1):
            self.material.position=AtlasG4Eng.G4Eng.gbl.Hep3Vector(-1020.,0.,0.)
            self.material.df.MoveTo(self.material.position)
            AtlasG4Eng.G4Eng.add_DetFacility(self.material,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('IDET'))
            AtlasG4Eng.G4Eng.log.info(' LArFarUpstreamMaterial is placed inside IDET '
                                      'at -1020 mm in order to use BeamConditions ')

# - Magnets  -----------------------------------------------------------
__doc__+="""\n\n         -- MAGNETS -- \n"""
# - MBPL12 magnet gap and iron
__doc__+="""\n mbpl & magboxmbpl12 = gap (MBPL12), iron (MAGBOXMBPL12)\r
               for the MBPL12 magnet in use during the photon runs."""
magboxmbpl12=PyG4Atlas.DetFacility("ResizeableBox","MAGBOXMBPL12")
magboxmbpl12.df.SetDx(1335)
magboxmbpl12.df.SetDy(870.)
magboxmbpl12.df.SetDz(565.)
magboxmbpl12.df.SetMaterial('Iron')
magboxmbpl12.df.DefineSubtractionBox('MBPL12HOLE')
mbpl12hole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(2670., 261., 56.,)
mbpl12hole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
magboxmbpl12.df.SetSubtractionBoxDimensions('MBPL12HOLE',mbpl12hole_dim)
magboxmbpl12.df.SetSubtractionBoxPosition('MBPL12HOLE',mbpl12hole_pos)
magboxmbpl12_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-11702.,0.,0.)
magboxmbpl12.df.MoveTo(magboxmbpl12_position)

mbpl12=PyG4Atlas.DetFacility("ResizeableBox","MBPL12")
mbpl12.df.SetDx(1000.)
mbpl12.df.SetDy(260.)
mbpl12.df.SetDz(55.)
mbpl12_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-11702.,0.,0.)
mbpl12.df.MoveTo(mbpl12_position)

# - MBPL13 magnet gap and iron
__doc__+="""\n mbpl & magboxmbpl13 = gap (MBPL13),iron (MAGBOXMBPL13)\r
               for the MBPL13 magnet in use during the photon runs."""
magboxmbpl13=PyG4Atlas.DetFacility("ResizeableBox","MAGBOXMBPL13")
magboxmbpl13.df.SetDx(1335)
magboxmbpl13.df.SetDy(565.)
magboxmbpl13.df.SetDz(870.)
magboxmbpl13.df.SetMaterial('Iron')
magboxmbpl13.df.DefineSubtractionBox('MBPL13HOLE')
mbpl13hole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(2670., 56.,261.)
mbpl13hole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
magboxmbpl13.df.SetSubtractionBoxDimensions('MBPL13HOLE',mbpl13hole_dim)
magboxmbpl13.df.SetSubtractionBoxPosition('MBPL13HOLE',mbpl13hole_pos)
magboxmbpl13_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-15500.,0.,0.)
magboxmbpl13.df.MoveTo(magboxmbpl13_position)

mbpl13=PyG4Atlas.DetFacility("ResizeableBox","MBPL13")
mbpl13.df.SetDx(1000.)
mbpl13.df.SetDy(55.)
mbpl13.df.SetDz(260.)
mbpl13_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-15500.,0.,0.)
mbpl13.df.MoveTo(mbpl13_position)

# - MBPSID magnet gap and iron
__doc__+="""\n mbpsid & magboxmbpsid = gap(MBPSID),iron(MAGBOXMBPSID)\r
               for the inner detector magnet."""
magboxmbpsid=PyG4Atlas.DetFacility("ResizeableBox","MAGBOXMBPSID")
magboxmbpsid.df.SetDx(845.)
magboxmbpsid.df.SetDy(870.)
magboxmbpsid.df.SetDz(565.)
magboxmbpsid.df.SetMaterial('Iron')
magboxmbpsid.df.DefineSubtractionBox('MBPSIDHOLE')
mbpsidhole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1690., 261., 116.,)
mbpsidhole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
magboxmbpsid.df.SetSubtractionBoxDimensions('MBPSIDHOLE',mbpsidhole_dim)
magboxmbpsid.df.SetSubtractionBoxPosition('MBPSIDHOLE',mbpsidhole_pos)
magboxmbpsid_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(500.,0.,0.)
magboxmbpsid.df.MoveTo(magboxmbpsid_position)

mbpsid=PyG4Atlas.DetFacility("ResizeableBox","MBPSID")
mbpsid.df.SetDx(500.)
mbpsid.df.SetDy(260.)
mbpsid.df.SetDz(115.)
mbpsid_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(500.,0.,0.)
mbpsid.df.MoveTo(mbpsid_position)

# - MBPL magnet gap and iron
__doc__+="""\n mbpl & magboxmbpl = gap (MBPL),  iron (MAGBOXMBPL)\r
               for the MBPL magnet at the Muon region"""
magboxmbpl=PyG4Atlas.DetFacility("ResizeableBox","MAGBOXMBPL")
magboxmbpl.df.SetDx(1335)
magboxmbpl.df.SetDy(565.)
magboxmbpl.df.SetDz(870.)
magboxmbpl.df.SetMaterial('Iron')
magboxmbpl.df.DefineSubtractionBox('MBPLHOLE')
mbplhole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(2670., 56., 261.,)
mbplhole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
magboxmbpl.df.SetSubtractionBoxDimensions('MBPLHOLE',mbplhole_dim)
magboxmbpl.df.SetSubtractionBoxPosition('MBPLHOLE',mbplhole_pos)
#magboxmbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(28296.,0.,0.)
#magboxmbpl.df.MoveTo(magboxmbpl_position)

mbpl=PyG4Atlas.DetFacility("ResizeableBox","MBPL")
mbpl.df.SetDx(1000.)
mbpl.df.SetDy(55.)
mbpl.df.SetDz(260.)
#mbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(28296.,0.,0.)
#mbpl.df.MoveTo(mbpl_position)

# - MBPS2 magnet gap and iron
__doc__+="""\n mbps2 & magboxmbps2 = gap (MBPS2), iron (MAGBOXMBPS2)\r
               at the Muon volumen."""
magboxmbps2=PyG4Atlas.DetFacility("ResizeableBox","MAGBOXMBPS2")
magboxmbps2.df.SetDx(845.)
magboxmbps2.df.SetDy(565.)
magboxmbps2.df.SetDz(870.)
magboxmbps2.df.SetMaterial('Iron')
magboxmbps2.df.DefineSubtractionBox('MBPS2HOLE')
mbps2hole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1690., 56., 261.,)
mbps2hole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
magboxmbps2.df.SetSubtractionBoxDimensions('MBPS2HOLE',mbps2hole_dim)
magboxmbps2.df.SetSubtractionBoxPosition('MBPS2HOLE',mbps2hole_pos)
#magboxmbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(45299.,0.,0.)
#magboxmbps2.df.MoveTo(magboxmbps2_position)

mbps2=PyG4Atlas.DetFacility("ResizeableBox","MBPS2")
mbps2.df.SetDx(500.)
mbps2.df.SetDy(55.)
mbps2.df.SetDz(260.)
#mbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(45299.,0.,0.)
#mbps2.df.MoveTo(mbps2_position)

class EquivUpstreamMaterial(object):
    """ Equivalent material

         to be used in case of the combined layout when the beam
         divergence info is loaded. It is placed at -1 m
    """
    def __init__(self):
        self.material=PyG4Atlas.DetFacility("ResizeableBox",
                                            "EquivUpstreamMaterial")
        self.material.df.SetDx(5.874)   #5.874 for 13.2
        self.material.df.SetDy(100.)
        self.material.df.SetDz(100.)
        self.material.df.SetMaterial("Aluminum")
        self.material.position=AtlasG4Eng.G4Eng.gbl.Hep3Vector(-1000.,0.,0.)
        self.material.df.MoveTo(self.material.position)
        AtlasG4Eng.G4Eng.add_DetFacility(self.material,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('IDET'))

class ctb_beampipes_and_magnetsupstream:
    """ Implements the CTB upstream magnets and beam-pipes.

       The mylar equivalent for the beam-pipes is also included.
       In a "photon run" two more magnets (MBPL12, MBPL13) are
       also implemented. In this mode the beampipes are inside
       the magnets.
    """
    def __init__(self,mode=0):
        """ mode=0 --> Standard runs
            mode=1 --> Photon runs
            mode=2 --> Standard runs using beam divergence
                       (all the upstream material is replaced
                        by equiv material)
        """
        AtlasG4Eng.G4Eng.add_DetFacility(mbpsid,idet)
        AtlasG4Eng.G4Eng.add_DetFacility(magboxmbpsid,idet)
        if(mode==0):
            beampipe1.df.MoveTo(beampipe1_position)
            AtlasG4Eng.G4Eng.add_DetFacility(beampipe1,ctb)
            beampipe2.df.MoveTo(beampipe2_position)
            AtlasG4Eng.G4Eng.add_DetFacility(beampipe2,ctb)
            AtlasG4Eng.G4Eng.add_DetFacility(mylarequiv,ctb)
        if(mode==1):
            AtlasG4Eng.G4Eng.add_DetFacility(mbpl13,ctb)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbpl13,ctb)
            AtlasG4Eng.G4Eng.add_DetFacility(mbpl12,ctb)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbpl12,ctb)
            # for the photon beams we reduce the size of the
            # beampipes and we centered them in the MBPL12 and
            # MBPL13 magnet
            beampipe1.df.SetDz(999.)
            AtlasG4Eng.G4Eng.add_DetFacility(beampipe1,mbpl13)
            beampipe2.df.SetDz(999.)
            AtlasG4Eng.G4Eng.add_DetFacility(beampipe2,mbpl12)
            AtlasG4Eng.G4Eng.add_DetFacility(mylarequiv,ctb)
        if(mode==2):
            equivupstreammaterial=EquivUpstreamMaterial()  # noqa: F841 (needed?)

# - Muon-DUMP  ---------------------------------------------------------
__doc__+="""\n\n         -- MUON DUMP -- \n"""
__doc__+="""\ndumpconcrete, dumpiron, dump= (DUMPCONCRETE),(DUMPIRON)\r
               and DUMP in the Muon region.\r
               DUMP can be present or not, depends on the run conditions
         """
dumpconcrete=PyG4Atlas.DetFacility("ResizeableBox","DUMPCONCRETE")
dumpconcrete.df.SetDx(1600.)
dumpconcrete.df.SetDy(2000.)
dumpconcrete.df.SetDz(1600.)
dumpconcrete.df.SetMaterial('Concrete')
#dumpconcrete_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(15449.7,-70.,0.)
#dumpconcrete.df.MoveTo(dumpconcrete_position)
dumpconcrete.df.DefineSubtractionBox('DUMPCONCRETEHOLE1')
dumpconcretehole1_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1601.,500.,800.,)
dumpconcretehole1_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,800.,0.)
dumpconcrete.df.SetSubtractionBoxDimensions('DUMPCONCRETEHOLE1',\
                                             dumpconcretehole1_dim)
dumpconcrete.df.SetSubtractionBoxPosition('DUMPCONCRETEHOLE1',\
                                             dumpconcretehole1_pos)
dumpconcrete.df.DefineSubtractionBox('DUMPCONCRETEHOLE2')
dumpconcretehole2_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1601.,800.,400.,)
dumpconcretehole2_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,-400.,0.)
dumpconcrete.df.SetSubtractionBoxDimensions('DUMPCONCRETEHOLE2',\
                                             dumpconcretehole2_dim)
dumpconcrete.df.SetSubtractionBoxPosition('DUMPCONCRETEHOLE2',\
                                             dumpconcretehole2_pos)
#
dumpiron=PyG4Atlas.DetFacility("ResizeableBox","DUMPIRON")
dumpiron.df.SetDx(1600.)
dumpiron.df.SetDy(600.)
dumpiron.df.SetDz(800.)
dumpiron.df.SetMaterial('Iron')
#dumpiron_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(15449.7,0.,0.)
#dumpiron.df.MoveTo(dumpiron_position)
dumpiron.df.DefineSubtractionBox('DUMPIRONHOLE1')
dumpironhole1_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1604., 400., 400.,)
dumpironhole1_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
dumpiron.df.SetSubtractionBoxDimensions('DUMPIRONHOLE1',\
                                             dumpironhole1_dim)
dumpiron.df.SetSubtractionBoxPosition('DUMPIRONHOLE1',\
                                             dumpironhole1_pos)
dumpiron.df.DefineSubtractionBox('DUMPIRONCORNER1')
dumpironcorner1_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1604., 100., 200.,)
dumpironcorner1_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,-501.,601.)
dumpiron.df.SetSubtractionBoxDimensions('DUMPIRONCORNER1',\
                                             dumpironcorner1_dim)
dumpiron.df.SetSubtractionBoxPosition('DUMPIRONCORNER1',\
                                             dumpironcorner1_pos)
dumpiron.df.SetSubtractionBoxPosition('DUMPIRONHOLE1',\
                                             dumpironhole1_pos)
dumpiron.df.DefineSubtractionBox('DUMPIRONCORNER2')
dumpironcorner2_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1604., 100., 200.,)
dumpironcorner2_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,-501.,-601.)
dumpiron.df.SetSubtractionBoxDimensions('DUMPIRONCORNER2',\
                                             dumpironcorner2_dim)
dumpiron.df.SetSubtractionBoxPosition('DUMPIRONCORNER2',\
                                             dumpironcorner2_pos)
#
dump=PyG4Atlas.DetFacility("ResizeableBox","DUMP")
dump.df.SetDx(1600.)
dump.df.SetDy(800.)
dump.df.SetDz(800.)
dump.df.SetMaterial('Iron')
#dump_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(15449.7,800.,0.)
#dump.df.MoveTo(dump_position)

class ctb_dump_and_muonmagnets:
    """ Implements the CTB DUMP and the Muon magnets.

       - If the MuonSystem is not included in the simulation
       (muosystemOn=0) dump and magnets are in the "MUON" envelope
       volumen. If(muosystemOn=1) dump and magnets are in the "Muon"
       detector volume.
       - Dump in/out ---> dumpIn=1/0 (runs with and without DUMP)
    """
    def __init__(self,muosystemOn,dumpIn):
        """
          muosystemOn=0 ---> Muon system off from DetFlags.
          muosystemOn=1 ---> Muon system on from DetFlags.
          dumpIn=1/0    ---> Dump in/out.
        """
        if(muosystemOn==0):
            magboxmbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          28296.-muon_position.getX(),0.,0.)
            magboxmbpl.df.MoveTo(magboxmbpl_position)
            mbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          28296.-muon_position.getX(),0.,0.)
            mbpl.df.MoveTo(mbpl_position)
            magboxmbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          45299.-muon_position.getX(),0.,0.)
            magboxmbps2.df.MoveTo(magboxmbps2_position)
            mbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          45299.-muon_position.getX(),0.,0.)
            mbps2.df.MoveTo(mbps2_position)
            dumpiron_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          15449.7-muon_position.getX(),0.,0.)
            dumpiron.df.MoveTo(dumpiron_position)
            dumpconcrete_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          15449.7-muon_position.getX(),-70.,0.)
            dumpconcrete.df.MoveTo(dumpconcrete_position)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbpl,muon)
            AtlasG4Eng.G4Eng.add_DetFacility(mbpl,muon)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbps2,muon)
            AtlasG4Eng.G4Eng.add_DetFacility(mbps2,muon)
            AtlasG4Eng.G4Eng.add_DetFacility(dumpconcrete,muon)
            AtlasG4Eng.G4Eng.add_DetFacility(dumpiron,muon)
            if(dumpIn==1):
                dump_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                            15449.7-muon_position.getX(),800.,0.)
                dump.df.MoveTo(dump_position)
                AtlasG4Eng.G4Eng.add_DetFacility(dump,muon)
        if(muosystemOn==1):
            from ctb_muon import ctb_muon
            magboxmbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          28296.,0.,0.)
            magboxmbpl.df.MoveTo(magboxmbpl_position)
            mbpl_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          28296.,0.,0.)
            mbpl.df.MoveTo(mbpl_position)
            magboxmbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          45299.,0.,0.)
            magboxmbps2.df.MoveTo(magboxmbps2_position)
            mbps2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          45299.,0.,0.)
            mbps2.df.MoveTo(mbps2_position)
            dumpiron_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          15449.7,0.,0.)
            dumpiron.df.MoveTo(dumpiron_position)
            dumpconcrete_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                          15449.7,-70.,0.)
            dumpconcrete.df.MoveTo(dumpconcrete_position)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbpl,ctb_muon)
            AtlasG4Eng.G4Eng.add_DetFacility(mbpl,ctb_muon)
            AtlasG4Eng.G4Eng.add_DetFacility(magboxmbps2,ctb_muon)
            AtlasG4Eng.G4Eng.add_DetFacility(mbps2,ctb_muon)
            AtlasG4Eng.G4Eng.add_DetFacility(dumpconcrete,ctb_muon)
            AtlasG4Eng.G4Eng.add_DetFacility(dumpiron,ctb_muon)
            if(dumpIn==1):
                dump_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(\
                            15449.7,800.,0.)
                dump.df.MoveTo(dump_position)
                AtlasG4Eng.G4Eng.add_DetFacility(dump,ctb_muon)

#=======================================================================

# - Ancillary scintillators  ------------------------------------------
__doc__+="""\n\n         -- ANCILLARY SCINTILLATORS -- \n"""
__doc__+="""\n              (upstream MBPSID region)   \n"""
# - Upstream MBPSID -
__doc__+="""\n smv      = SMV muon veto counter.

               To be used only during the very low energy periods to
               remove the unwanted muon halo.
         """
smv=PyG4Atlas.DetFacility("ResizeableBox","SMV")
smv.df.SetDx(10.)
smv.df.SetDy(405)
smv.df.SetDz(170)
smv.df.SetMaterial("Scintillator")
smv_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-20677.,0.,0.)
smv.df.MoveTo(smv_position)
#
__doc__+="""\n s1       = S1 scintillator.
         """
s1=PyG4Atlas.DetFacility("ResizeableBox","S1")
s1.df.SetDx(3.)
s1.df.SetDy(50.)
s1.df.SetDz(50.)
s1.df.SetMaterial("Scintillator")
s1_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-8843.,0.,0.)
s1.df.MoveTo(s1_position)
#
__doc__+="""\n smh      = SMH scintillator.

         Scintillator with a hole 30x30.
         """
smh=PyG4Atlas.DetFacility("ResizeableBox","SMH")
smh.df.SetDx(5.)
smh.df.SetDy(150.)
smh.df.SetDz(150.)
smh.df.SetMaterial("Scintillator")
smh_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-8083.,0.,0.)
smh.df.MoveTo(smh_position)
smh.df.DefineSubtractionBox('SMHHOLE')
smhhole_dim=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(30., 30., 30.,)
smhhole_pos=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)
smh.df.SetSubtractionBoxDimensions('SMHHOLE',smhhole_dim)
smh.df.SetSubtractionBoxPosition('SMHHOLE',smhhole_pos)
#
__doc__+="""\n s2       = S2 scintillator.
         """
s2=PyG4Atlas.DetFacility("ResizeableBox","S2")
s2.df.SetDx(2.5)
s2.df.SetDy(25.)
s2.df.SetDz(25.)
s2.df.SetMaterial("Scintillator")
s2_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-7558.,0.,0.)
s2.df.MoveTo(s2_position)
#
__doc__+="""\n s3       = S3 scintillator.
         """
s3=PyG4Atlas.DetFacility("ResizeableBox","S3")
s3.df.SetDx(2.5)
s3.df.SetDy(25.)
s3.df.SetDz(25.)
s3.df.SetMaterial("Scintillator")
s3_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-7523.,0.,0.)
s3.df.MoveTo(s3_position)
#
__doc__+="""\n s4       = Test scintillator (never used).
         """
class s4(DetFacilityT):
    DetFacilityT.Name="ResizeableBox"
    DetFacilityT.Type="S4"
    def __init__(self):
        DetFacilityT.__init__(self,"ResizeableBox","S4")
    def _build(self):
        self.df=PyG4Atlas.DetFacility("ResizeableBox","S4").df
        self.df.SetDx(2.5)
        self.df.SetDy(25.)
        self.df.SetDz(25.)
        self.df.SetMaterial("Scintillator")
        self.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-7533.,0.,0.))
        self._Built=True
#
__doc__+="""\n sh       = SH scintillator.
         """
sh=PyG4Atlas.DetFacility("ResizeableBox","SH")
sh.df.SetDx(2.5)
sh.df.SetDy(50.)
sh.df.SetDz(50.)
sh.df.SetMaterial("Scintillator")
sh_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-7570.,0.,0.)
sh.df.MoveTo(sh_position)
#
__doc__+="""\n snew     = SNEW scintillator.
         """
snew=PyG4Atlas.DetFacility("ResizeableBox","SNEW")
snew.df.SetDx(5.)
snew.df.SetDy(15.)
snew.df.SetDz(15.)
snew.df.SetMaterial("Scintillator")
snew_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-1675.,0.,0.)
snew.df.MoveTo(snew_position)
#
#
__doc__+="""\n leadfoil     =  LeadFoil

            It clean the beam from e- during the very low energy periods
            of type PeriodB.
         """
leadfoil=PyG4Atlas.DetFacility("ResizeableBox","LeadFoil")
leadfoil.df.SetDx(2.5)
leadfoil.df.SetDy(50.)
leadfoil.df.SetDz(50.)
leadfoil.df.SetMaterial("Lead")
leadfoil_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-7570.,0.,0.)
leadfoil.df.MoveTo(leadfoil_position)
#


class AncillaryUpstream:
    """
       Implements all the upstream (MBPSID) scintillators.

       There are three periods (A,B,C) described in the Note:
       "ATLAS Barrel Combined Run in 2004 Test Beam Setup and its evolutions"
         https://edms.cern.ch/file/406980/3/TB2004_layout_v06.pdf
    """
    def __init__(self,period):
        if(period=='PeriodA'):
            AtlasG4Eng.G4Eng.add_DetFacility(s1,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(smh,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(s2,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(s3,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
        elif(period=='PeriodB'):
            AtlasG4Eng.G4Eng.add_DetFacility(s1,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(s2,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(s3,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(snew,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('IDET'))
        elif(period=='PeriodC'):
            AtlasG4Eng.G4Eng.add_DetFacility(s1,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('CTB'))
            AtlasG4Eng.G4Eng.add_DetFacility(snew,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('IDET'))
        else:
            pass

# LAr and Tile ancillary material

class CombinedScintillator:
    """
       Implements the CombinedScintillator in between the Lar and
       the Tile Barrel.

       The SD is also implemented but the MuonWall is the 'master'

    """
    def __init__(self,eta):
        AtlasG4Eng.G4Eng.load_Lib("CombinedScintillator")
        self.ctb_combinedscintillator=PyG4Atlas.DetFacility( "CombinedScintillator","CombinedScintillator")
        AtlasG4Eng.G4Eng.read_XML("Tile_materials.xml")
        AtlasG4Eng.G4Eng.read_XML("combinedscintillator.xml")
        self.ctb_combinedscintillator.df.UseDescription('CombinedScintillator')
       #self.ctb_combinedscintillator_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-2.0,0.0,2200.)
        if (eta==0):
          self.ctb_combinedscintillator_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-2.0,0.0,0.)
        elif(eta>0 and eta<=0.95):
          self.ctb_combinedscintillator_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-2.0,0.0,\
                     2274.0/math.tan(2*math.atan(math.exp(-eta))) )
        elif(eta>0.95):
          self.ctb_combinedscintillator_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(-2.0,0.0,0.0)
        self.ctb_combinedscintillator.df.MoveTo(self.ctb_combinedscintillator_position)
        #self.ctb_combinedscintillator.add_SenDetector('CombinedScintillator',\
        #                   'CombinedScintillatorSD','CombinedScintillatorSD',\
        #                             'CombinedScintillator::CScintillatorLayer')


class LArCrate:
    """
       Implements the LArCrate in front of the Tile extended Barrel.

    """
    def __init__(self):
        AtlasG4Eng.G4Eng.read_XML("Tile_materials.xml")
        AtlasG4Eng.G4Eng.read_XML("LArCrate_materials.xml")
        AtlasG4Eng.G4Eng.load_Lib("LarCrate")
        self.ctb_larcrate=PyG4Atlas.DetFacility("LArCrate","LArCrate")
        AtlasG4Eng.G4Eng.read_XML("larcrate.xml")
        self.ctb_larcrate.df.UseDescription('LArCrate')
        self.ctb_larcrate_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(3075.0,-80.0,3255.)
        self.ctb_larcrate.df.MoveTo(self.ctb_larcrate_position)

class MuonWall:
    """
       Implements the MuonWall.

       The SD is also implemented here. The MuonWallSD is the
       'master' for the set of SD: MuonWallSD, CombinedScintillatorSD
       and PhantomBarrelSD. This means that MuonWallSD has to be
       build first.

    """
    def __init__(self,eta=0,theta=3.1415926,delta=0.0):
        AtlasG4Eng.G4Eng.read_XML("Tile_materials.xml")
        AtlasG4Eng.G4Eng.load_Lib("MuonWall")
        self.ctb_muonwall=PyG4Atlas.DetFacility("MuonWall","MuonWall")
        AtlasG4Eng.G4Eng.read_XML("muonwall.xml")
        self.ctb_muonwall.df.UseDescription('MuonWall')
        zmax=2380.22441662
        r=5115.0
        z=0.0
        if (math.fabs(theta)<1.57):
            z=r*math.tan(-theta)-delta
        elif (eta!=0):
            z=r/math.tan(2*math.atan(math.exp(-eta)))
        if (z>zmax):
            z=zmax
        elif (z<-zmax):
            z=-zmax
        self.ctb_muonwall_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(r,0.0,z)
        self.ctb_muonwall.df.MoveTo(self.ctb_muonwall_position)
        #self.ctb_muonwall.add_SenDetector('MuonWall',\
        #     'MuonWallSD','MuonWallSD','MuonWall::MuScintillatorLayer')
        # We want it built at the first place
        #AtlasG4Eng.G4Eng._List_SenDetector_dependence.append('MuonWallSD')


class PhantomCalorimeter:
    """
       Implements the PhantomCalorimeter in front of the Tile
       extended Barrel.

       The SD is also implemented but the MuonWall is the 'master'
    """
    def __init__(self):
        AtlasG4Eng.G4Eng.read_XML("Tile_materials.xml")
        AtlasG4Eng.G4Eng.load_Lib("PhantomCalorimeter")
        self.ctb_phantomcalorimeter=PyG4Atlas.DetFacility("PhantomBarrel","PhantomBarrel")
        AtlasG4Eng.G4Eng.read_XML("phantombarrel.xml")
        self.ctb_phantomcalorimeter.df.UseDescription('PhantomBarrel')
        self.ctb_phantomcalorimeter_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(1950.,0.,4350.)
        self.ctb_phantomcalorimeter.df.MoveTo(self.ctb_phantomcalorimeter_position)
        self.ctb_phantomcalorimeter.df.RotateY(math.radians(90.))
        #self.ctb_phantomcalorimeter.add_SenDetector('PhantomCalorimeter',\
        #     'PhantomBarrelSD','PhantomBarrelSD','PhantomBarrel::ScintillatorLayer')

class LArTileAncillary:
    """
    Implements all the Tile ancillary material that is included in
    in the simulation i. e.: CombinedScintillator, MuonWall,
    PhantomCalorimeter, LArCrate.

    + the eta value is used for the CombinedScintillator and MuonWall.
    + the PhantomCalorimeter is not in place in the setup with
      only 3 barrels (CTB-3B).
    """
    def __init__(self,eta,mode):
        muonwall=MuonWall(eta)
        AtlasG4Eng.G4Eng.add_DetFacility(muonwall.ctb_muonwall,calo)
        larcrate=LArCrate()
        AtlasG4Eng.G4Eng.add_DetFacility(larcrate.ctb_larcrate,calo)
        combinedscintillator=CombinedScintillator(eta)
        AtlasG4Eng.G4Eng.add_DetFacility(combinedscintillator.ctb_combinedscintillator,calo)
        if (mode=='CTB-3B3E'):
            phantomcalorimeter=PhantomCalorimeter()
            AtlasG4Eng.G4Eng.add_DetFacility(phantomcalorimeter.ctb_phantomcalorimeter,calo)


class EquivalentLArMuonChamber(object):
     """ This describes the muon chamber in front of the LAr
         approximated as a Al block

         It has to be inside the CALO envelope (after re-size)
     """
     def __init__(self):
         self.MuonChamber=PyG4Atlas.DetFacility("ResizeableBox",
                                            "LARMUONCHAMBER")
         self.MuonChamber.df.SetDx(0.5*8)
         self.MuonChamber.df.SetDy(400)
         self.MuonChamber.df.SetDz(0.5*1075)
         self.MuonChamber.df.SetMaterial("Aluminum")
         # position unknow here (see SimCtbKernel)
         self.MuonChamber.position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,
                                      0.,0.)

# - Scintillators in the Muon region
__doc__+="""\n              (Muon region)   \n"""
__doc__+="""\n scdump = SCDUMP scintillator.

            Scintillator after the dump.
         """
scdump=PyG4Atlas.DetFacility("ResizeableBox","SCDUMP")
scdump.df.SetDx(2.)
scdump.df.SetDy(70.)
scdump.df.SetDz(35.)
scdump.df.SetMaterial("Scintillator")
scdump_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(19199.,0.,0.)
scdump.df.MoveTo(scdump_position)
#
__doc__+="""\n smt = SMT scintillator.

            Scintillator used as Muon tag.
         """
smt=PyG4Atlas.DetFacility("ResizeableBox","SMT")
smt.df.SetDx(10.)
smt.df.SetDy(200.)
smt.df.SetDz(200.)
smt.df.SetMaterial("Scintillator")
smt_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(20669.,0.,0.)
smt.df.MoveTo(smt_position)
#
__doc__+="""\n muon10x10A = MUON10X10A scintillator.
         """
muon10x10A=PyG4Atlas.DetFacility("ResizeableBox","MUON10x10A")
muon10x10A.df.SetDx(5.)
muon10x10A.df.SetDy(50.)
muon10x10A.df.SetDz(50.)
muon10x10A.df.SetMaterial("Scintillator")
muon10x10A_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(26511.,0.,0.)
muon10x10A.df.MoveTo(muon10x10A_position)
#
__doc__+="""\n muon10x10B = MUON10X10B scintillator.
         """
muon10x10B=PyG4Atlas.DetFacility("ResizeableBox","MUON10x10B")
muon10x10B.df.SetDx(5.)
muon10x10B.df.SetDy(50.)
muon10x10B.df.SetDz(50.)
muon10x10B.df.SetMaterial("Scintillator")
muon10x10B_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(26796.,0.,0.)
muon10x10B.df.MoveTo(muon10x10B_position)
#
__doc__+="""\n muon10x10C = MUON10X10C scintillator.
         """
muon10x10C=PyG4Atlas.DetFacility("ResizeableBox","MUON10x10C")
muon10x10C.df.SetDx(7.5)
muon10x10C.df.SetDy(30.)
muon10x10C.df.SetDz(30.)
muon10x10C.df.SetMaterial("Scintillator")
muon10x10C_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(26926.,0.,0.)
muon10x10C.df.MoveTo(muon10x10C_position)
#
__doc__+="""\n muon10x10D = MUON10X10D scintillator.
         """
muon10x10D=PyG4Atlas.DetFacility("ResizeableBox","MUON10x10D")
muon10x10D.df.SetDx(2.5)
muon10x10D.df.SetDy(175.)
muon10x10D.df.SetDz(50.)
muon10x10D.df.SetMaterial("Scintillator")
muon10x10D_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(27046.,0.,0.)
muon10x10D.df.MoveTo(muon10x10D_position)
#
__doc__+="""\n muonhodoselector = MUONHODOSELECTOR scintillator.
         """
muonhodoselector=PyG4Atlas.DetFacility("ResizeableBox","MUONHODOSELECTOR")
muonhodoselector.df.SetDx(2.5)
muonhodoselector.df.SetDy(325.)
muonhodoselector.df.SetDz(50.)
muonhodoselector.df.SetMaterial("Scintillator")
muonhodoselector_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(31014.,0.,0.)
muonhodoselector.df.MoveTo(muonhodoselector_position)
#
__doc__+="""\n muonhodoplane1(1/6) = MUONHODOPLANE1(1/6) scintillator.
         """
class muonhodoplane1:
    """
       Implements the MuonHodo plane 1 with 6 scintillators
    """
    def __init__(self,muosystemOn):
        """
          muosystemOn=0 ---> Muon system off from DetFlags.
          muosystemOn=1 ---> Muon system on from DetFlags.
        """
        for i in range(7):
            if i>0:
                name=PyG4Atlas.DetFacility("ResizeableBox",\
                                            "MUONHODOPLANE1"+repr(i))
                name.df.SetDx(2.5)
                name.df.SetDy(50.)
                name.df.SetDz(500.)
                name.df.SetMaterial("Scintillator")
                name.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(31019.,\
                               -25+(i-1)*10.,0.))
                if(muosystemOn==0):
                    AtlasG4Eng.G4Eng.add_DetFacility(name,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('MUON'))
                elif(muosystemOn==1):
                    AtlasG4Eng.G4Eng.add_DetFacility(name,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))

__doc__+="""\n muonhodoplane2(1/6) = MUONHODOPLANE2(1/6) scintillator.
         """
class muonhodoplane2:
    """
       Implements the MuonHodo plane 2 with 6 scintillators
    """
    def __init__(self,muosystemOn):
        """
          muosystemOn=0 ---> Muon system off from DetFlags.
          muosystemOn=1 ---> Muon system on from DetFlags.
        """
        for i in range(7):
            if i>0:
                name=PyG4Atlas.DetFacility("ResizeableBox",\
                                            "MUONHODOPLANE2"+repr(i))
                name.df.SetDx(2.5)
                name.df.SetDy(50.)
                name.df.SetDz(500.)
                name.df.SetMaterial("Scintillator")
                name.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(31599.,\
                               -25+(i-1)*10.,0.))
                if(muosystemOn==0):
                    AtlasG4Eng.G4Eng.add_DetFacility(name,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('MUON'))
                elif(muosystemOn==1):
                    AtlasG4Eng.G4Eng.add_DetFacility(name,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))

class MuonAncillary:
    """ Implements all the muon ancillary detectors.
    """
    def __init__(self,muosystemOn):
        """
          muosystemOn=0 ---> Muon system off (Not implemented)
          muosystemOn=1 ---> Muon system on from DetFlags.
        """
        if(muosystemOn==0):
            print ('WARNING :: Not Implemented yet !!')
        elif(muosystemOn==1):
            AtlasG4Eng.G4Eng.add_DetFacility(scdump,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(smt,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(muon10x10A,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(muon10x10B,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(muon10x10C,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(muon10x10D,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            AtlasG4Eng.G4Eng.add_DetFacility(muonhodoselector,\
                     AtlasG4Eng.G4Eng.Dict_DetFacility.get('Muon:Muon'))
            muonhodoplane1(1)
            muonhodoplane2(1)

#=======================================================================
# LAr material studies
__doc__+="""\n\n         -- LAr MATERIAL STUDIES -- \n"""
__doc__+="""\n larmaterial      = LARMATERIAL for LAr studies.

            The dimension in x can be 25, 50 or 75 mm the default
            dimension is 25. The position must be centered in the
            beam line at 6 cm of the Lar cryo, rotated as the LAr
            to 0.4

         """
larmaterial=PyG4Atlas.DetFacility("ResizeableBox","LARMATERIAL")
larmaterial.df.SetDx(12.5)
larmaterial.df.SetDy(100)
larmaterial.df.SetDz(100)
larmaterial.df.SetMaterial("Aluminum")
#larmaterial_position=AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.,0.,0.)

#=======================================================================
## class CTBRecordingEnvelopes(object):
##     """ Defines the recording envelopes in use in the CTB simulations
##
##          The hits are recorded with the TrackRecorderSD in the
##          TrackRecordCollection collection.
##
##     """
##     class CtbCaloEntryLayer:
##         """ Defines the recording envelope CtbCaloEntryLayer
##
##             Hits in TrackRecordCollection#CaloEntryLayer
##         """
##         def __init__(self):
##             self.recenv=PyG4Atlas.RecEnvelope('CaloEntryLayer','IDET::IDET',2)
##
##     class LArFarUpstreamMaterialExitLayer:
##         """ Defines the recording envelope LArFarUpstreamMaterialExitLayer
##
##             Hits in TrackRecordCollection#LArFarUpstreamMaterialExitLayer
##         """
##         def __init__(self,option):
##             if(option==0):
##                 self.recenv=PyG4Atlas.RecEnvelope('LArFarUpstreamMaterialExitLayer',
##                              'LARFARUPSTREAMMATERIAL::LARFARUPSTREAMMATERIAL',2)
##             elif(option==1):
##                 self.recenv=PyG4Atlas.RecEnvelope('LArFarUpstreamMaterialExitLayer',
##                              'LARFARUPSTREAMMATERIAL::LARFARUPSTREAMMATERIAL',3)
##
## #=======================================================================
