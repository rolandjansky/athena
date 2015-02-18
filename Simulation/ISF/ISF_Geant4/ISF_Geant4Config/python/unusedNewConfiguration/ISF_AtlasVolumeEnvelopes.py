# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS volume envelopes.
"""

# modified by Elmar.Ritsch -at- cern.ch
__author__  = 'A. Dell`Acqua, M. Gallas, A. Di Simone'

import math
from ISF_Geant4Config.DetFacility import DetFacility
from ISF_Geant4Config.iGeant4 import G4Eng
#from G4AtlasApps import AtlasG4Eng
#import AthenaPython.PyAthena as PyAthena

class AtlasEnvelope(object):
    """
    ATLAS envelope.

    ATLAS detector facility that implements the envelope in which all the
    sub-detectors are placed. The dimensions are set in accordance with the
    setup (simFlags.SimLayout) and simFlags.WorldZRange
    """

    def __init__(self):
        """
        Describes the geometry.
        """

        self.AtlasForwardOuterR = 2751.
        self.AtlasOuterR1 = 13001.
        self.AtlasOuterR2 = 13401.
        self.AtlasOuterR3 =  1501.
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.BeamFlags import jobproperties
        from AthenaCommon.DetFlags import DetFlags
        ## Shrink the global ATLAS envelope to the activated detectors, except when running on special setups.
        if jobproperties.Beam.beamType() != 'cosmics' and not DetFlags.Muon_on() and not \
           (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            self.AtlasOuterR1 = 4251.
            self.AtlasOuterR2 = 4251.
            if not DetFlags.Calo_on():
                self.AtlasOuterR1 = 1150.
                self.AtlasOuterR2 = 1150.
        G4Eng.log.debug('Creating Atlas envelope with outer radii %d, %d' % (self.AtlasOuterR1, self.AtlasOuterR2))

        self.atlas = DetFacility("GenericPCON", "Atlas", G4Eng)
        self.atlas.df.SetPhiMin(math.radians(0))
        self.atlas.df.SetDeltaPhi(math.radians(360))
        self.atlas.df.SetNSurfaces(18)
        self.atlas.df.SetZSurface(0, -26046.)
        self.atlas.df.SetZSurface(1, -23000.)
        self.atlas.df.SetZSurface(2, -23000.)
        self.atlas.df.SetZSurface(3, -22031.)
        self.atlas.df.SetZSurface(4, -22031.)
        self.atlas.df.SetZSurface(5, -12899.)
        self.atlas.df.SetZSurface(6, -12899.)
        self.atlas.df.SetZSurface(7, -6741.)
        self.atlas.df.SetZSurface(8, -6741.)
        self.atlas.df.SetZSurface(9,  6741.)
        self.atlas.df.SetZSurface(10,  6741.)
        self.atlas.df.SetZSurface(11,  12899.)
        self.atlas.df.SetZSurface(12, 12899.)
        self.atlas.df.SetZSurface(13, 22031.)
        self.atlas.df.SetZSurface(14, 22031.)
        self.atlas.df.SetZSurface(15, 23000.)
        self.atlas.df.SetZSurface(16, 23000.)
        self.atlas.df.SetZSurface(17, 26046.)

        if simFlags.ForwardDetectors.statusOn:
            self.atlas.df.SetZSurface(0, -400000.)
            self.atlas.df.SetZSurface(17, 400000.)
 
        if simFlags.WorldZRange.statusOn:
            if simFlags.WorldZRange.get_Value() < 26046.:
                  G4Eng.log.error('WorldZRange must be > 26046.')
                  raise RuntimeError('ISF_AtlasVolumeEnvelopes: found problems with the Z World volume range: %f' % simFlags.WorldZRange.get_Value())
            self.atlas.df.SetZSurface(17,  simFlags.WorldZRange.get_Value() + 100.)
            self.atlas.df.SetZSurface(16,  simFlags.WorldZRange.get_Value() + 50. )
            self.atlas.df.SetZSurface(15,  simFlags.WorldZRange.get_Value() + 50. )
            self.atlas.df.SetZSurface(14,  simFlags.WorldZRange.get_Value())
            self.atlas.df.SetZSurface(13,  simFlags.WorldZRange.get_Value())
            self.atlas.df.SetZSurface(0,  -simFlags.WorldZRange.get_Value() - 100.)
            self.atlas.df.SetZSurface(1,  -simFlags.WorldZRange.get_Value() - 50. )
            self.atlas.df.SetZSurface(2,  -simFlags.WorldZRange.get_Value() - 50. )
            self.atlas.df.SetZSurface(3,  -simFlags.WorldZRange.get_Value() )
            self.atlas.df.SetZSurface(4,  -simFlags.WorldZRange.get_Value() )
            G4Eng.log.debug('Increasing the Z World ATLAS envelope to %f' % simFlags.WorldZRange.get_Value())

        ## Set inner (all zero) and outer radii
        for i in xrange(0, 18):
            self.atlas.df.SetRinSurface(i, 0.0)
        for i in (0, 1, 16, 17):
            self.atlas.df.SetRouSurface(i, 1501.)
        for i in (2, 3, 14, 15):
            self.atlas.df.SetRouSurface(i, self.AtlasForwardOuterR)
        for i in (4, 5, 12, 13):
            self.atlas.df.SetRouSurface(i, self.AtlasOuterR2)
        for i in xrange(6, 12):
            self.atlas.df.SetRouSurface(i, self.AtlasOuterR1)

        ## World R range
        if simFlags.WorldRRange.statusOn:
            if simFlags.WorldRRange.get_Value() > max(self.AtlasOuterR1, self.AtlasOuterR2):
                routValue = simFlags.WorldRRange.get_Value()
                for i in xrange(4, 14):
                    self.atlas.df.SetRouSurface(i, routValue)
                G4Eng.log.debug('Increasing the ATLAS World envelope R value to %f' % routValue)
            else:
                G4Eng.log.error('WorldRRange must be > %f' % max(self.AtlasOuterR1, self.AtlasOuterR2))
                raise RuntimeError('ISF_AtlasVolumeEnvelopes: found problems with the R World volume range %f' % routValue)


class InDetEnvelope(object):
    """
    ATLAS envelope for the inner detector.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        G4Eng.log.debug('Creating inner detector envelope')
        self.idet = DetFacility("EnvelopeDefSvc", "IDET", G4Eng)
        self.idet.df.SetEnvelopeSvcName( "G4EnvelopeDefSvc" )
        self.idet.df.SetEnvelopeName( "IDET" )

        ## retrieve the iGeant4EnvelopeSvc
        ## @TODO: get '..GeoIDSvc..' strings dynamically
        ##           eg via  iGeant4EnvelopeSvc.getFullName()  string from jobOptions.py
        #self.GeoIDSvc = PyAthena.py_svc( 'iGeant4::GeoIDSvc/iGeant4EnvelopeSvc',
        #                                 iface=getattr(PyAthena, 'ISF::IGeoIDSvc'))
        #if self.GeoIDSvc is None:
        #  G4Eng.log.error('Problem retrieving %s service !', 'iGeant4EnvelopeSvc')
        #  return
        #  #return StatusCode.Failure

        ## now use the iGeant4EnvelopeSvc to get the InDet boundary points
        ## @TODO: dynamic indetID
        #indetID = 0
        ##ri_ro_z_list = [ ]
        #zMax = 0.
        #rMax = 0.
        #rMin = 9999999.
        #numPoints = self.GeoIDSvc.getEnvelopeNumPoints(indetID)

        #self.idet.df.SetDz( zMax)       # = 3475.0
        #from G4AtlasApps.SimFlags import simFlags
        ### NB. Special treatment of Rmin for upgrade layouts with insertable B-layer (innermost pixel layer)
        #if 'ATLAS-IBL-' in simFlags.SimLayout.get_Value() or 'ATLAS-SLHC-' in simFlags.SimLayout.get_Value():
        #    self.idet.df.SetRmin(29.7)
        #else:
        #    self.idet.df.SetRmin( rMin) # = 37.
        #self.idet.df.SetRmax( rMax)     # = 1148.0


class CosmicShortCut(object):
    """
    Entry layer used to store cosmic rays entering the ATLAS detector.
    It will produce a collection named CosmicRecord, containing
    TrackRecords.
    """

    def __init__(self, createTTR):
        """
        Describes the geometry.

        If the createTTR arg is True, the entry layer will have a SD called
        CosmicTRSD.
        """

        G4Eng.log.debug('Creating CosmicShortcut envelope')
        self.ttr_barrel = DetFacility("GenericPCON", "TTR_BARREL", G4Eng)
        ri_ro_z_list = [
            (   70., 12501., -22031.),
            (   70., 12501., -22030.),
            (12500., 12501., -22030.),
            (12500., 12501., -12901.),
            (12500., 13001., -12901.),
            (12500., 13001., -12900.),
            (13000., 13001., -12900.),
            (13000., 13001.,  12900.),
            (12500., 13001.,  12900.),
            (12500., 13001.,  12901.),
            (12500., 12501.,  12901.),
            (12500., 12501.,  22030.),
            (   70., 12501.,  22030.),
            (   70., 12501.,  22031.)]
        self.ttr_barrel.df.SetPhiMin(math.radians(0))
        self.ttr_barrel.df.SetDeltaPhi(math.radians(360))
        self.ttr_barrel.df.SetNSurfaces(len(ri_ro_z_list))
        for i, ri_ro_z in enumerate(ri_ro_z_list):
            ri, ro, z = ri_ro_z
            self.ttr_barrel.df.SetRinSurface(i, ri)
            self.ttr_barrel.df.SetRouSurface(i, ro)
            self.ttr_barrel.df.SetZSurface(i, z)

        if createTTR:
            self.ttr_barrel.add_SenDetector('MCTruth', 'CosmicTRSD', 'CosmicRecord', 'TTR_BARREL')


class CaloEnvelope(object):
    """
    ATLAS envelope for the calorimeters
    """

    def __init__(self):
        """
        Describe the calo geometry.
        """
        G4Eng.log.debug('Creating Calo envelope')
        self.caloMuonBoundary = 6550
        self.calo = DetFacility("EnvelopeDefSvc", "CALO", G4Eng)
        self.calo.df.SetEnvelopeSvcName( "G4EnvelopeDefSvc" )
        self.calo.df.SetEnvelopeName( "CALO" )

        # retrieve the iGeant4EnvelopeSvc
        # @TODO: get '..GeoIDSvc..' strings dynamically
        #           eg via  iGeant4EnvelopeSvc.getFullName()  string from jobOptions.py
        #self.GeoIDSvc = PyAthena.py_svc( 'iGeant4::GeoIDSvc/iGeant4EnvelopeSvc',
        #                                 iface=getattr(PyAthena, 'ISF::IGeoIDSvc'))
        #if self.GeoIDSvc is None:
        #  G4Eng.log.error('Problem retrieving %s service !', 'iGeant4EnvelopeSvc')
        #  return
        #  #return StatusCode.Failure

        # now use the iGeant4EnvelopeSvc to get the Calo boundary points
        #self.GeoIDSvc.initialize()
        ## @TODO: dynamic caloID
        #caloID = 2
        #numPoints = self.GeoIDSvc.getEnvelopeNumPoints(caloID)
        #self.calo.df.SetNPoints(numPoints)
        #self.calo.df.SetPhiMin(math.radians(0))
        #self.calo.df.SetDeltaPhi(math.radians(360))

        # the old list:
        #ri_ro_z_list = [ (  41., 3800., -6750.),
        #                 (  41., 3800., -6550.),
        #                 (  41., 3851., -6550.),
        #                 (  41., 3851., -6530.),
        #                 (  41., 4251., -6530.),
        #                 (  41., 4251., -4185.),
        #                 ( 120., 4251., -4185.),
        #                 ( 120., 4251., -3475.),
        #                 (1148., 4251., -3475.),
        #                 (1148., 4251.,  3475.),
        #                 ( 120., 4251.,  3475.),
        #                 ( 120., 4251.,  4185.),
        #                 (  41., 4251.,  4185.),
        #                 (  41., 4251.,  6530.),
        #                 (  41., 3851.,  6530.),
        #                 (  41., 3851.,  6550.),
        #                 (  41., 3800.,  6550.),
        #                 (  41., 3800.,  6750.) ]


class MuonEnvelope(object):
    """
    ATLAS Muon envelope.

    Sets the Muon envelope in accordance with the
    simFlags.SimLayout
    """

    def __init__(self):
        """
        Describe the muon geometry.
        """
        G4Eng.log.debug('Creating Muon envelope')
        self.muonq02 = DetFacility("EnvelopeDefSvc", "MUONQ02", G4Eng)
        self.muonq02.df.SetEnvelopeSvcName( "G4EnvelopeDefSvc" )
        self.muonq02.df.SetEnvelopeName( "MUONQ02" )

        ## retrieve the iGeant4EnvelopeSvc
        ## @TODO: get '..GeoIDSvc..' strings dynamically
        ##           eg via  iGeant4EnvelopeSvc.getFullName()  string from jobOptions.py
        #self.GeoIDSvc = PyAthena.py_svc( 'iGeant4::GeoIDSvc/iGeant4EnvelopeSvc',
        #                                 iface=getattr(PyAthena, 'ISF::IGeoIDSvc'))
        #if self.GeoIDSvc is None:
        #  G4Eng.log.error('Problem retrieving %s service !', 'iGeant4EnvelopeSvc')
        #  return
        #  #return StatusCode.Failure

        ## now use the iGeant4EnvelopeSvc to get the InDet boundary points
        #self.GeoIDSvc.initialize()
        ## @TODO: dynamic msID
        #msID = 3
        #numPoints = self.GeoIDSvc.getEnvelopeNumPoints(msID)
        #self.muonq02.df.SetNPoints(numPoints)
        #self.muonq02.df.SetPhiMin(math.radians(0))
        #self.muonq02.df.SetDeltaPhi(math.radians(360))

        # the old list:
        #ri_ro_z_list = [
        #    ( 1050., 1500., -26045.),
        #    ( 1050., 1500., -23000.),
        #    ( 1050., 2750., -23000.),
        #    ( 1050., 2750., -22030.),
        #    ( 436., 12650., -22030.),
        #    ( 436., 12650., -18650.),
        #    ( 280., 13400., -18650.),
        #    ( 280., 13400., -12900.),
        #    (  70., 13000., -12900.),
        #    (  70., 13000.,  -6750.),
        #    (3800., 13000.,  -6750.),
        #    (3800., 13000.,  -6550),
        #    (4255., 13000.,  -6550),
        #    (4255., 13000.,   6550),
        #    (3800., 13000.,   6550),
        #    (3800., 13000.,   6750.),
        #    (  70., 13000.,   6750.),
        #    (  70., 13000.,  12900.),
        #    ( 280., 13400.,  12900.),
        #    ( 280., 13400.,  18650.),
        #    ( 436., 12650.,  18650.),
        #    ( 436., 12650.,  22030.),
        #    ( 1050., 2750.,  22030.),
        #    ( 1050., 2750.,  23000.),
        #    ( 1050., 1500.,  23000.),
        #    ( 1050., 1500.,  26045.)]

class BeamPipe(object):
    """
    Describes the beam pipe.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.beampipe = DetFacility("GeoDetector", "BeamPipe:BeamPipe", G4Eng)

    def _initPR(self):
        """ Parameterization of the forward region (if fwd detectors on)
            creates the needed regions and applies the relevant fast sim model
        """
        from G4AtlasApps.SimFlags import simFlags

        ## Speed up simulation of forward plugs on the beam pipe
        if simFlags.BeamPipeSimMode.statusOn and simFlags.BeamPipeSimMode() != "Normal":
            self.atlas_beampipe_PhysReg = ISF_PyG4Atlas.PhysicsReg('BeampipeFwdCut')
            self.atlas_beampipe_PhysReg.add_Volumes('BeamPipe::SectionF47')
            self.atlas_beampipe_PhysReg.add_Volumes('BeamPipe::SectionF48')
            self.atlas_beampipe_PhysReg.add_Volumes('BeamPipe::SectionF61')
            if simFlags.BeamPipeSimMode() == "FastSim":
                self.atlas_beampipe_PhysReg.add_Cuts('e-', 10.)
                self.atlas_beampipe_PhysReg.add_Cuts('e+', 10.)
                self.atlas_beampipe_PhysReg.add_Cuts('gamma', 10.)
                self.beampipe.add_PhysicsReg(self.atlas_beampipe_PhysReg)
                ## Add dead material
                #from AthenaCommon.AppMgr import ServiceMgr
                #if not hasattr(ServiceMgr, 'DeadMaterialFastSimSvc' ):
                #    from LArG4FastSimSvc.LArG4FastSimSvcConfig import DeadMaterialFastSimSvc
                #    ServiceMgr += DeadMaterialFastSimSvc()
                self.atlas_SimpleFastKiller = ISF_PyG4Atlas.FastSimModel('LArG4FastSimulation', 'SimpleFastKiller')
                self.atlas_SimpleFastKiller.add_Region(self.atlas_beampipe_PhysReg)
                G4Eng.add_FastSimModel(self.atlas_SimpleFastKiller)
                G4Eng.log.info('Adding fast sim model to the beampipe!')
            else:
                assert simFlags.BeamPipeCut.statusOn
                if simFlags.BeamPipeCut() < 1:
                    msg = "Setting the forward beam pipe range cuts to %e mm " % simFlags.BeamPipeCut()
                    msg += "-- cut is < 1 mm, I hope you know what you're doing!"
                    G4Eng.log.info(msg)
                if simFlags.BeamPipeSimMode() == "EGammaRangeCuts":
                    self.atlas_beampipe_PhysReg.add_Cuts('e-', simFlags.BeamPipeCut())
                    self.atlas_beampipe_PhysReg.add_Cuts('e+', simFlags.BeamPipeCut())
                    self.atlas_beampipe_PhysReg.add_Cuts('gamma', simFlags.BeamPipeCut())
                elif simFlags.BeamPipeSimMode() == "EGammaPRangeCuts":
                    self.atlas_beampipe_PhysReg.add_Cuts('e-', simFlags.BeamPipeCut())
                    self.atlas_beampipe_PhysReg.add_Cuts('e+', simFlags.BeamPipeCut())
                    self.atlas_beampipe_PhysReg.add_Cuts('gamma', simFlags.BeamPipeCut())
                    self.atlas_beampipe_PhysReg.add_Cuts('proton', simFlags.BeamPipeCut())
                self.beampipe.add_PhysicsReg(self.atlas_beampipe_PhysReg)

        ## Set up the forward transport model
        if simFlags.ForwardDetectors.statusOn and simFlags.ForwardDetectors() == 2:
            self.atlas_bPipe_FwdTransport = ISF_PyG4Atlas.FastSimModel('ForwardTransport','ForwardTransport')
            self.atlas_bPipe_PhysReg = ISF_PyG4Atlas.PhysicsReg('FWDBeamLine')
            self.atlas_bPipe_PhysReg.add_Volumes('BeamPipe::SectionF46')
            self.beampipe.add_PhysicsReg(self.atlas_bPipe_PhysReg)
            ## Add region to fwd transport
            self.atlas_bPipe_FwdTransport.add_Region(self.atlas_bPipe_PhysReg)
            ## Add fwd transport to the G4Eng
            G4Eng.add_FastSimModel(self.atlas_bPipe_FwdTransport)


class AtlasCavernGeoModel(object):
    """
    ATLAS cavern (as defined in GeoModel)
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.point1 = DetFacility("GeoDetector", "CavernInfra:CavernInfra", G4Eng)
        self.world = DetFacility("ResizeableBox", "World", G4Eng)

        bedrockDX = 302700
        bedrockDZ = 301000
        from G4AtlasApps.SimFlags import simFlags
        if not (hasattr(simFlags,'CavernBG') and simFlags.CavernBG.statusOn ):
            ## Be ready to resize bedrock if the cosmic generator needs more space
            from CosmicGenerator.CosmicGeneratorConf import CosmicGenerator
            cg = CosmicGenerator()
            if (cg.xvert_hig - cg.xvert_low)/2.0 > bedrockDX:
                bedrockDX = (cg.xvert_hig - cg.xvert_low)/2.0
            if (cg.zvert_hig - cg.zvert_low)/2.0 > bedrockDZ:
                bedrockDZ = (cg.zvert_hig - cg.zvert_low)/2.0
        else :
            self.world.add_MaxStepInVolumes('World', 20000.000)

        self.world.df.SetDx(bedrockDX)
        self.world.df.SetDz(bedrockDZ)
        self.world.df.SetDy(57300 + 41000 + 1000) # 1 extra meter to help voxelization...
        self.world.df.DefineSubtractionBox('BelowCavern')
        BelowCavernDim = G4Eng.gbl.CLHEP.Hep3Vector(bedrockDX, 41000, bedrockDZ)
        BelowCavernPos = G4Eng.gbl.CLHEP.Hep3Vector(0, -58300, 0)
        self.world.df.SetSubtractionBoxDimensions('BelowCavern', BelowCavernDim)
        self.world.df.SetSubtractionBoxPosition('BelowCavern', BelowCavernPos)

    def DeclarePhysReg(self):
        """
        Describes physics regions for the cavern
        """
        point1_PhysReg1 = ISF_PyG4Atlas.PhysicsReg('SX1')
        point1_PhysReg1.add_Volumes('CavernInfra::SX1Air')
        point1_PhysReg1.add_Cuts('e-', 2000)
        point1_PhysReg1.add_Cuts('e+', 2000)
        point1_PhysReg1.add_Cuts('gamma', 2000)
        point1_PhysReg2 = ISF_PyG4Atlas.PhysicsReg('BEDROCK')
        point1_PhysReg2.add_Volumes('CavernInfra::BEDROCK')
        point1_PhysReg2.add_Cuts('e-', 1000000)
        point1_PhysReg2.add_Cuts('e+', 1000000)
        point1_PhysReg2.add_Cuts('gamma', 1000000)
        point1_PhysReg3 = ISF_PyG4Atlas.PhysicsReg('CAV_SHAFTS_CONC')
        point1_PhysReg3.add_Volumes('CavernInfra::CAV_SHAFTS_CONC')
        point1_PhysReg3.add_Cuts('e-', 150)
        point1_PhysReg3.add_Cuts('e+', 150)
        point1_PhysReg3.add_Cuts('gamma', 150)
        #point1_PhysReg4=ISF_PyG4Atlas.PhysicsReg('CAV_SHAFTS_AIR')
        #point1_PhysReg4.add_Volumes('CavernInfra::CAV_SHAFTS_AIR')
        #point1_PhysReg4.add_Cuts('e-',2000)
        #point1_PhysReg4.add_Cuts('e+',2000)
        #point1_PhysReg4.add_Cuts('gamma',2000)
        self.point1.add_PhysicsReg(point1_PhysReg1)
        self.point1.add_PhysicsReg(point1_PhysReg2)
        self.point1.add_PhysicsReg(point1_PhysReg3)
        #self.point1.add_PhysicsReg(point1_PhysReg4)


class AtlasCavern(object):
    """
    Atlas cavern.

    This is a rough description of the ATLAS cavern (UX15) and shafts (PX16 and
    PX14)
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        pitHeight = 54149/2.
        px16OR = 13100/2.
        px16IR = 12600/2.
        px14OR = 18500/2.
        px14IR = 18000/2.

        self.px16o = DetFacility("ResizeableTubs", "PX16outer", G4Eng)
        self.px16o.df.SetDz(pitHeight)
        self.px16o.df.SetRmin(0.)
        self.px16o.df.SetRmax(px16OR)
        self.px16o.df.SetMaterial("Concrete")
        self.px16i = DetFacility("ResizeableTubs", "PX16inner", G4Eng)
        self.px16i.df.SetDz(pitHeight)
        self.px16i.df.SetRmin(0.)
        self.px16i.df.SetRmax(px16IR)
        self.px16i.df.SetMaterial("Air")

        G4Eng.add_DetFacility(self.px16i, self.px16o)
        self.px14o = ISF_PyG4Atlas.DetFacility("ResizeableTubs", "PX14outer", G4Eng)
        self.px14o.df.SetDz(pitHeight)
        self.px14o.df.SetRmin(0.)
        self.px14o.df.SetRmax(px14OR)
        self.px14o.df.SetMaterial("Concrete")
        self.px14i = ISF_PyG4Atlas.DetFacility("ResizeableTubs", "PX14inner", G4Eng)
        self.px14i.df.SetDz(pitHeight)
        self.px14i.df.SetRmin(0.)
        self.px14i.df.SetRmax(px14IR)
        self.px14i.df.SetMaterial("Air")

        G4Eng.add_DetFacility(self.px14i, self.px14o)
        self.ux15o = ISF_PyG4Atlas.DetFacility("ResizeableBox","UX15outer", G4Eng)
        self.ux15o.df.SetDz(27500.)
        self.ux15o.df.SetDy(25000.)
        self.ux15o.df.SetDx(17000.)
        #self.ux15o.df.SetDx(25000.)
        self.ux15o.df.SetMaterial("Concrete")
        self.ux15i = ISF_PyG4Atlas.DetFacility("ResizeableBox","UX15inner", G4Eng)
        self.ux15i.df.SetDz(26500.)
        self.ux15i.df.SetDy(24000.)
        self.ux15i.df.SetDx(16000.)
        #self.ux15i.df.SetDx(24000.)
        self.ux15i.df.SetMaterial("Air")

        ## Need two small volumes to cut away the concrete from the roof of the cavern
        px16cap = ISF_PyG4Atlas.DetFacility("ResizeableTubs","PX16cap", G4Eng)
        px16cap.df.SetMaterial("Air")
        px16cap.df.SetDz(500.)
        px16cap.df.SetRmin(0.)
        px16cap.df.SetRmax(px16OR)
        px14cap = ISF_PyG4Atlas.DetFacility("ResizeableTubs","PX14cap", G4Eng)
        px14cap.df.SetMaterial("Air")
        px14cap.df.SetDz(500.)
        px14cap.df.SetRmin(0.)
        px14cap.df.SetRmax(px14OR)

        G4Eng.add_DetFacility(self.ux15i, self.ux15o)
        G4Eng.add_DetFacility(px16cap, self.ux15o)
        px16cap.df.RotateX(math.radians(90))
        px16cap.df.MoveTo(G4Eng.gbl.CLHEP.Hep3Vector(0., 25000 - 250, 11000.))
        G4Eng.add_DetFacility(px14cap, self.ux15o)
        px14cap.df.MoveTo(G4Eng.gbl.CLHEP.Hep3Vector(0., 25000 - 250, -10000.))
        px14cap.df.RotateX(math.radians(90))

        self.cavern = ISF_PyG4Atlas.DetFacility("ResizeableBox","Cavern", G4Eng)
        self.cavern.df.SetDz(300000.)
        self.cavern.df.SetDx(300000.)
        self.cavern.df.SetDy(pitHeight + 25000.)
        self.cavern.df.SetMaterial("Rock")

        G4Eng.add_DetFacility(self.cavern, self.cavern)
        G4Eng.add_DetFacility(self.ux15o, self.cavern)
        self.ux15o.df.MoveTo(G4Eng.gbl.CLHEP.Hep3Vector(0.0, (-104149 + 50000)/2.0 , 0.0))

        G4Eng.add_DetFacility(self.px16o, self.cavern)
        self.px16o.df.MoveTo(G4Eng.gbl.CLHEP.Hep3Vector(0.0, -104149/2.0 + 50000 + pitHeight, 11000.))
        self.px16o.df.RotateX(math.radians(90))

        G4Eng.add_DetFacility(self.px14o, self.cavern)
        self.px14o.df.MoveTo(G4Eng.gbl.CLHEP.Hep3Vector(0.0, -104149/2.0 + 50000 + pitHeight, -10000.))
        self.px14o.df.RotateX(math.radians(90))

