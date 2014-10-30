# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS volume envelopes.
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. Di Simone'

import math
import PyG4Atlas, AtlasG4Eng

class AtlasEnvelope(object):
    """
    ATLAS envelope.

    ATLAS detector facility that implements the envelope in which all the
    sub-detectors are placed. The dimensions are set in accordance with the
    setup (simFlags.SimLayout) and simFlags.WorldZRange
    """

    def __init__(self, layout):
        """
        Describes the geometry.
        """

        self.AtlasForwardOuterR = 2751.
        self.AtlasOuterR1 = 13911.
        self.AtlasOuterR2 = 13911.
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
        AtlasG4Eng.G4Eng.log.debug('Creating Atlas envelope with outer radii %d, %d' % (self.AtlasOuterR1, self.AtlasOuterR2))

        self.atlas = PyG4Atlas.DetFacility("GenericPCON", "Atlas")
        self.atlas.df.SetPhiMin(math.radians(0))
        self.atlas.df.SetDeltaPhi(math.radians(360))
        self.atlas.df.SetNSurfaces(18)
        self.atlas.df.SetZSurface(0, -26046.)
        self.atlas.df.SetZSurface(1, -23001.)
        self.atlas.df.SetZSurface(2, -23001.)
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
        self.atlas.df.SetZSurface(15, 23001.)
        self.atlas.df.SetZSurface(16, 23001.)
        self.atlas.df.SetZSurface(17, 26046.)

        if simFlags.ForwardDetectors.statusOn:
            self.atlas.df.SetZSurface(0, -400000.)
            self.atlas.df.SetZSurface(17, 400000.)

        if simFlags.WorldZRange.statusOn:
            if simFlags.WorldZRange.get_Value() < 26046.:
                  AtlasG4Eng.G4Eng.log.error('WorldZRange must be > 26046.')
                  raise RuntimeError('atlas_common: found problems with the Z World volume range: %f' % simFlags.WorldZRange.get_Value())
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
            AtlasG4Eng.G4Eng.log.debug('Increasing the Z World ATLAS envelope to %f' % simFlags.WorldZRange.get_Value())

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
                AtlasG4Eng.G4Eng.log.debug('Increasing the ATLAS World envelope R value to %f' % routValue)
            else:
                AtlasG4Eng.G4Eng.log.error('WorldRRange must be > %f' % max(self.AtlasOuterR1, self.AtlasOuterR2))
                raise RuntimeError('atlas_common: found problems with the R World volume range %f' % routValue)


class InDetEnvelope(object):
    """
    ATLAS envelope for the inner detector.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        AtlasG4Eng.G4Eng.log.debug('Creating inner detector envelope')
        self.idet = PyG4Atlas.DetFacility("ResizeableTubs", "IDET")
        self.idet.df.SetDz(3475.)
        from G4AtlasApps.SimFlags import simFlags
        ## NB. Special treatment of Rmin for upgrade layouts with insertable B-layer (innermost pixel layer)
        if 'ATLAS-IBL' in simFlags.SimLayout.get_Value() or 'ATLAS-R2' in simFlags.SimLayout.get_Value():
            self.idet.df.SetRmin(29.15) ## NB Beampipe starts at 29.1

            IBL_GeoVersion = 0
            try:
                IBL_GeoVersion = int(simFlags.SimLayout.get_Value().split('-')[2])
            except:
                IBL_GeoVersion = 0
            if IBL_GeoVersion >= 4:
                self.idet.df.SetRmin(28.9) ## NB Beampipe starts at 28.9
            del IBL_GeoVersion
        elif 'ATLAS-SLHC' in simFlags.SimLayout.get_Value() or 'ATLAS-P' in simFlags.SimLayout.get_Value():
            self.idet.df.SetRmin(32.15) ##AK: Cartigny beampipe starts at 32.1
        else:
            self.idet.df.SetRmin(37.)
        self.idet.df.SetRmax(1148.)


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

        AtlasG4Eng.G4Eng.log.debug('Creating CosmicShortcut envelope')
        self.ttr_barrel = PyG4Atlas.DetFacility("GenericPCON", "TTR_BARREL")
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

    def __init__(self, layout):
        """
        Describe the calo geometry.
        """
        AtlasG4Eng.G4Eng.log.debug('Creating Calo envelope')
        self.caloMuonBoundary = 6550
        self.calo = PyG4Atlas.DetFacility("GenericPCON", "CALO")
        ri_ro_z_list = [
            (  41.,  415., -6781.),
            (  41.,  415., -6735.),
            (  41., 3795., -6735.),
            (  41., 3795., -6530.),
            (  41., 4251., -6530.),
            (  41., 4251., -4587.),
            ( 120., 4251., -4587.),
            ( 120., 4251., -3475.),
            (1148., 4251., -3475.),
            (1148., 4251.,  3475.),
            ( 120., 4251.,  3475.),
            ( 120., 4251.,  4587.),
            (  41., 4251.,  4587.),
            (  41., 4251.,  6530.),
            (  41., 3795.,  6530.),
            (  41., 3795.,  6735.),
            (  41.,  415.,  6735.),
            (  41.,  415.,  6781.)]
        self.calo.df.SetPhiMin(math.radians(0))
        self.calo.df.SetDeltaPhi(math.radians(360))
        self.calo.df.SetNSurfaces(len(ri_ro_z_list))
        for i, ri_ro_z in enumerate(ri_ro_z_list):
            ri, ro, z = ri_ro_z
            self.calo.df.SetRinSurface(i, ri)
            self.calo.df.SetRouSurface(i, ro)
            self.calo.df.SetZSurface(i, z)


class MuonEnvelope(object):
    """
    ATLAS Muon envelope.

    Sets the Muon envelope in accordance with the
    simFlags.SimLayout
    """

    def __init__(self, layout):
        """
        Describe the muon geometry.
        """
        AtlasG4Eng.G4Eng.log.debug('Creating Muon envelope')
        self.muonq02 = PyG4Atlas.DetFacility("GenericPCON", "MUONQ02")
        ri_ro_z_list = [
            (1050.,   1500., -26046.),
            (1050.,   1500., -23001.),
            (1050.,   2750., -23001.),
            (1050.,   2750., -22030.),
            ( 436.7, 12650., -22030.),
            ( 436.7, 12650., -18650.),
            ( 279.,  13400., -18650.),
            ( 279.,  13400., -12900.),
            (  70.,  13910., -12900.),
            (  70.,  13910.,  -6783.),
            ( 420.,  13910.,  -6783.),
            ( 420.,  13910.,  -6736.),
            (3800.,  13910.,  -6736.),
            (3800.,  13910.,  -6550.),
            (4255.,  13000.,  -6550.),
            (4255.,  13000.,   6550.),
            (3800.,  13910.,   6550.),
            (3800.,  13910.,   6736.),
            ( 420.,  13910.,   6736.),
            ( 420.,  13910.,   6783.),
            (  70.,  13910.,   6783.),
            (  70.,  13910.,  12900.),
            ( 279.,  13400.,  12900.),
            ( 279.,  13400.,  18650.),
            ( 436.7, 12650.,  18650.),
            ( 436.7, 12650.,  22030.),
            (1050.,   2750.,  22030.),
            (1050.,   2750.,  23001.),
            (1050.,   1500.,  23001.),
            (1050.,   1500.,  26046.)]

        self.muonq02.df.SetPhiMin(math.radians(0))
        self.muonq02.df.SetDeltaPhi(math.radians(360))
        self.muonq02.df.SetNSurfaces(len(ri_ro_z_list))
        for i, ri_ro_z in enumerate(ri_ro_z_list):
            ri, ro, z = ri_ro_z
            self.muonq02.df.SetRinSurface(i, ri)
            self.muonq02.df.SetRouSurface(i, ro)
            self.muonq02.df.SetZSurface(i, z)


class BeamPipe(object):
    """
    Describes the beam pipe.
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.beampipe = PyG4Atlas.DetFacility("GeoDetector", "BeamPipe:BeamPipe")

    def _initPR(self):
        """ Parameterization of the forward region (if fwd detectors on)
            creates the needed regions and applies the relevant fast sim model
        """
        from G4AtlasApps.SimFlags import simFlags

        ## Speed up simulation of forward plugs on the beam pipe
        if simFlags.BeamPipeSimMode.statusOn and simFlags.BeamPipeSimMode() != "Normal":
            self.atlas_beampipe_PhysReg = PyG4Atlas.PhysicsReg('BeampipeFwdCut')
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
                self.atlas_SimpleFastKiller = PyG4Atlas.FastSimModel('LArG4FastSimulation', 'SimpleFastKiller')
                self.atlas_SimpleFastKiller.add_Region(self.atlas_beampipe_PhysReg)
                AtlasG4Eng.G4Eng.add_FastSimModel(self.atlas_SimpleFastKiller)
                AtlasG4Eng.G4Eng.log.info('Adding fast sim model to the beampipe!')
            else:
                assert simFlags.BeamPipeCut.statusOn
                if simFlags.BeamPipeCut() < 1:
                    msg = "Setting the forward beam pipe range cuts to %e mm " % simFlags.BeamPipeCut()
                    msg += "-- cut is < 1 mm, I hope you know what you're doing!"
                    AtlasG4Eng.G4Eng.log.info(msg)
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
            self.atlas_bPipe_FwdTransport = PyG4Atlas.FastSimModel('ForwardTransport','ForwardTransport')
            self.atlas_bPipe_PhysReg = PyG4Atlas.PhysicsReg('FWDBeamLine')
            self.atlas_bPipe_PhysReg.add_Volumes('BeamPipe::SectionF46')
            self.beampipe.add_PhysicsReg(self.atlas_bPipe_PhysReg)
            ## Add region to fwd transport
            self.atlas_bPipe_FwdTransport.add_Region(self.atlas_bPipe_PhysReg)
            ## Add fwd transport to the G4Eng
            AtlasG4Eng.G4Eng.add_FastSimModel(self.atlas_bPipe_FwdTransport)


class AtlasCavernGeoModel(object):
    """
    ATLAS cavern (as defined in GeoModel)
    """

    def __init__(self):
        """
        Describes the geometry.
        """
        self.point1 = PyG4Atlas.DetFacility("GeoDetector", "CavernInfra:CavernInfra")
        self.world = PyG4Atlas.DetFacility("ResizeableBox", "World")

        bedrockDX = 302700
        bedrockDZ = 301000
        from G4AtlasApps.SimFlags import simFlags
        if not (hasattr(simFlags,'CavernBG') and simFlags.CavernBG.statusOn ):
            ## Be ready to resize bedrock if the cosmic generator needs more space
            if simFlags.ISFRun:
                from AthenaCommon.CfgGetter import getAlgorithm
                if simFlags.ReadTR.statusOn:
                    cg = getAlgorithm('TrackRecordCosmicGenerator')
                else:
                    cg = getAlgorithm('EvgenCosmicGenerator')
                # for ISF cosmics simulation, set world volume to biggest possible case
                bedrockDX = 1000.*3000 # 3 km
                bedrockDZ = 1000.*3000 # 3 km
            else:
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
        BelowCavernDim = AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(bedrockDX, 41000, bedrockDZ)
        BelowCavernPos = AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0, -58300, 0)
        self.world.df.SetSubtractionBoxDimensions('BelowCavern', BelowCavernDim)
        self.world.df.SetSubtractionBoxPosition('BelowCavern', BelowCavernPos)

    def DeclarePhysReg(self):
        """
        Describes physics regions for the cavern
        """
        point1_PhysReg1 = PyG4Atlas.PhysicsReg('SX1')
        point1_PhysReg1.add_Volumes('CavernInfra::SX1Air')
        point1_PhysReg1.add_Cuts('e-', 2000)
        point1_PhysReg1.add_Cuts('e+', 2000)
        point1_PhysReg1.add_Cuts('gamma', 2000)
        point1_PhysReg2 = PyG4Atlas.PhysicsReg('BEDROCK')
        point1_PhysReg2.add_Volumes('CavernInfra::BEDROCK')
        point1_PhysReg2.add_Cuts('e-', 1000000)
        point1_PhysReg2.add_Cuts('e+', 1000000)
        point1_PhysReg2.add_Cuts('gamma', 1000000)
        point1_PhysReg3 = PyG4Atlas.PhysicsReg('CAV_SHAFTS_CONC')
        point1_PhysReg3.add_Volumes('CavernInfra::CAV_SHAFTS_CONC')
        point1_PhysReg3.add_Cuts('e-', 150)
        point1_PhysReg3.add_Cuts('e+', 150)
        point1_PhysReg3.add_Cuts('gamma', 150)
        #point1_PhysReg4=PyG4Atlas.PhysicsReg('CAV_SHAFTS_AIR')
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

        self.px16o = PyG4Atlas.DetFacility("ResizeableTubs", "PX16outer")
        self.px16o.df.SetDz(pitHeight)
        self.px16o.df.SetRmin(0.)
        self.px16o.df.SetRmax(px16OR)
        self.px16o.df.SetMaterial("Concrete")
        self.px16i = PyG4Atlas.DetFacility("ResizeableTubs", "PX16inner")
        self.px16i.df.SetDz(pitHeight)
        self.px16i.df.SetRmin(0.)
        self.px16i.df.SetRmax(px16IR)
        self.px16i.df.SetMaterial("Air")

        AtlasG4Eng.G4Eng.add_DetFacility(self.px16i, self.px16o)
        self.px14o = PyG4Atlas.DetFacility("ResizeableTubs", "PX14outer")
        self.px14o.df.SetDz(pitHeight)
        self.px14o.df.SetRmin(0.)
        self.px14o.df.SetRmax(px14OR)
        self.px14o.df.SetMaterial("Concrete")
        self.px14i = PyG4Atlas.DetFacility("ResizeableTubs", "PX14inner")
        self.px14i.df.SetDz(pitHeight)
        self.px14i.df.SetRmin(0.)
        self.px14i.df.SetRmax(px14IR)
        self.px14i.df.SetMaterial("Air")

        AtlasG4Eng.G4Eng.add_DetFacility(self.px14i, self.px14o)
        self.ux15o = PyG4Atlas.DetFacility("ResizeableBox","UX15outer")
        self.ux15o.df.SetDz(27500.)
        self.ux15o.df.SetDy(25000.)
        self.ux15o.df.SetDx(17000.)
        #self.ux15o.df.SetDx(25000.)
        self.ux15o.df.SetMaterial("Concrete")
        self.ux15i = PyG4Atlas.DetFacility("ResizeableBox","UX15inner")
        self.ux15i.df.SetDz(26500.)
        self.ux15i.df.SetDy(24000.)
        self.ux15i.df.SetDx(16000.)
        #self.ux15i.df.SetDx(24000.)
        self.ux15i.df.SetMaterial("Air")

        ## Need two small volumes to cut away the concrete from the roof of the cavern
        px16cap = PyG4Atlas.DetFacility("ResizeableTubs","PX16cap")
        px16cap.df.SetMaterial("Air")
        px16cap.df.SetDz(500.)
        px16cap.df.SetRmin(0.)
        px16cap.df.SetRmax(px16OR)
        px14cap = PyG4Atlas.DetFacility("ResizeableTubs","PX14cap")
        px14cap.df.SetMaterial("Air")
        px14cap.df.SetDz(500.)
        px14cap.df.SetRmin(0.)
        px14cap.df.SetRmax(px14OR)

        AtlasG4Eng.G4Eng.add_DetFacility(self.ux15i, self.ux15o)
        AtlasG4Eng.G4Eng.add_DetFacility(px16cap, self.ux15o)
        px16cap.df.RotateX(math.radians(90))
        px16cap.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0., 25000 - 250, 11000.))
        AtlasG4Eng.G4Eng.add_DetFacility(px14cap, self.ux15o)
        px14cap.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0., 25000 - 250, -10000.))
        px14cap.df.RotateX(math.radians(90))

        self.cavern = PyG4Atlas.DetFacility("ResizeableBox","Cavern")
        self.cavern.df.SetDz(300000.)
        self.cavern.df.SetDx(300000.)
        self.cavern.df.SetDy(pitHeight + 25000.)
        self.cavern.df.SetMaterial("Rock")

        AtlasG4Eng.G4Eng.add_DetFacility(self.cavern, self.cavern)
        AtlasG4Eng.G4Eng.add_DetFacility(self.ux15o, self.cavern)
        self.ux15o.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.0, (-104149 + 50000)/2.0 , 0.0))

        AtlasG4Eng.G4Eng.add_DetFacility(self.px16o, self.cavern)
        self.px16o.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.0, -104149/2.0 + 50000 + pitHeight, 11000.))
        self.px16o.df.RotateX(math.radians(90))

        AtlasG4Eng.G4Eng.add_DetFacility(self.px14o, self.cavern)
        self.px14o.df.MoveTo(AtlasG4Eng.G4Eng.gbl.CLHEP.Hep3Vector(0.0, -104149/2.0 + 50000 + pitHeight, -10000.))
        self.px14o.df.RotateX(math.radians(90))
