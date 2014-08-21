# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS calorimeter detector description
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. Di Simone '

from ISF_Geant4Config.ISF_SimFlags import simFlags

from ISF_Geant4Config.iGeant4 import G4Eng
from ISF_Geant4Config.DetFacility import DetFacility
from ISF_Geant4Config.PhysicsReg import PhysicsReg
from ISF_Geant4Config.DetConfigurator import DetConfigurator

# -- LAr --
# detector configurator
class PyLArG4RunControler(DetConfigurator):

    """
    Class that inherits from G4AtlasApps.PyG4Atlas.DetConfigurator
    and it builds the PyLArG4RunControler in accordance with the
    user requirements.

    It uses the LArG4RunControlerDict dictionary.
    """
    def __init__(self,name,lcgdict_name,mode='',allowMods=False):
        self.allowMods=allowMods
        DetConfigurator.__init__(self,name,lcgdict_name,mode)


    def build(self, mode):
        """
        Builds the LArG4RunControl configurator.

        All the information is saved in StoreGate. The options given
        here can act only over the SD.
        """
        import ROOT
        self.LArG4GlobalOptions = G4Eng.gbl.LArG4GlobalOptions()

        self.LArG4GlobalOptions.AllowHitModification(self.allowMods)

        self.LArG4GlobalOptions.saveMe()
        ## We need the following line because otherwise StoreGate will
        ## delete the object first and pylcgdict will complain
        ROOT.SetOwnership(self.LArG4GlobalOptions, 0)

        ## Enable cond db reading in case we need HV maps
        if hasattr(simFlags, 'LArHighVoltage') and simFlags.LArHighVoltage.statusOn:
            if simFlags.LArHighVoltage.get_Value() is not None:
                for calo in ["EMB", "EMEC", "HEC", "FCAL"]:
                    if calo in simFlags.LArHighVoltage.get_Value():
                        from IOVDbSvc.CondDB import conddb
                        conddb.addFolder("", "/LAR/DCS/HV/BARREl/I16")
                        conddb.addFolder("", "/LAR/DCS/HV/BARREL/I8" )
                        break

        ## TODO: Tidy this... lots of reducible repetition of the same checks
        self.LArG4BarrelOptions = G4Eng.gbl.LArG4BarrelOptions()
        self.LArG4BarrelOptions.EMBBirksLaw(simFlags.DoLArBirk())
        self.LArG4EMECOptions = G4Eng.gbl.LArG4EMECOptions()
        self.LArG4EMECOptions.EMECBirksLaw(simFlags.DoLArBirk())
        self.LArG4HECOptions = G4Eng.gbl.LArG4HECOptions()
        self.LArG4HECOptions.HECBirksLaw(simFlags.DoLArBirk())
        self.LArG4FCALOptions=G4Eng.gbl.LArG4FCALOptions()
        self.LArG4FCALOptions.FCALBirksLaw(simFlags.DoLArBirk())
        if hasattr(simFlags, 'LArHighVoltage') and simFlags.LArHighVoltage.statusOn and simFlags.LArHighVoltage.get_Value() is not None:
            self.LArG4BarrelOptions.EMBHVEnable("EMB" in simFlags.LArHighVoltage.get_Value())
            self.LArG4EMECOptions.EMECHVEnable("EMEC" in simFlags.LArHighVoltage.get_Value())
            self.LArG4HECOptions.HECHVEnable("HEC" in simFlags.LArHighVoltage.get_Value())
            self.LArG4FCALOptions.FCALEnableHV("FCAL" in simFlags.LArHighVoltage.get_Value())
        self.LArG4BarrelOptions.saveMe()
        self.LArG4EMECOptions.saveMe()
        self.LArG4HECOptions.saveMe()
        self.LArG4FCALOptions.saveMe()
        import logging
        if G4Eng.log.level <= logging.INFO:
            self.LArG4BarrelOptions.printMe()
            self.LArG4EMECOptions.printMe()
            self.LArG4HECOptions.printMe()
            self.LArG4FCALOptions.printMe()
        ROOT.SetOwnership(self.LArG4HECOptions, 0)
        ROOT.SetOwnership(self.LArG4EMECOptions, 0)
        ROOT.SetOwnership(self.LArG4BarrelOptions, 0)
        ROOT.SetOwnership(self.LArG4FCALOptions, 0)


class LAr(object):
    """
    Describes the ATLAS LAr detector.
    """

    def __init__(self,allowMods=False):
        """ Describes the geometry.
        """
        G4Eng.load_Lib("LArG4HitManagement")
        self.atlas_lar = DetFacility("GeoDetector", "LArMgr:LArMgr",allowMods)

    def _initSD(self, mode):
        """
        Describes the sensitive detector.

        Modes:
          0 --> Normal LAr sensitive detector
          1 --> LAr sensitive detector for calibration hits
          2 --> LAr active + dead regions

        TODO: Identify common elements: don't repeat everything for each mode. And use loops.
        """
        # sensitive detector with GeoModel:
        if mode == 0:
            # normal mode
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMB::STAC',
                                           'LAr::EMB::STAC',
                                           'LArMgr::LAr::EMB::STAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Module',
                                           'LAr::Barrel::Presampler::Module',
                                           'LArMgr::LAr::Barrel::Presampler::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Endcap::Presampler::LiquidArgon',
                                           'LAr::Endcap::Presampler::LiquidArgon',
                                           'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel',
                                           'LAr::EMEC::Pos::InnerWheel',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel',
                                           'LAr::EMEC::Pos::OuterWheel',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel',
                                           'LAr::EMEC::Neg::InnerWheel',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel',
                                           'LAr::EMEC::Neg::OuterWheel',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::BackOuterBarrette::Module::Phidiv',
                                           'LAr::EMEC::BackOuterBarrette::Module::Phidiv',
                                           'LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module1::Gap',
                                           'LAr::FCAL::Module1::Gap',
                                           'LArMgr::LAr::FCAL::Module1::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module2::Gap',
                                           'LAr::FCAL::Module2::Gap',
                                           'LArMgr::LAr::FCAL::Module2::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module3::Gap',
                                           'LAr::FCAL::Module3::Gap',
                                           'LArMgr::LAr::FCAL::Module3::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Module::Depth::Slice::Wheel',
                                           'LAr::HEC::Module::Depth::Slice::Wheel',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCAL::Wafer',
                                           'MiniFCAL::Wafer',
                                           'LArMgr::MiniFCAL::Wafer')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS1')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS2')

        elif mode == 1:
            # calibration mode
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'STACMult',
                                           'STACMult',
                                           'LArMgr::LAr::EMB::STAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'BarrelPresamplerMult',
                                           'BarrelPresamplerMult',
                                           'LArMgr::LAr::Barrel::Presampler::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EndcapPresamplerMult',
                                           'EndcapPresamplerMult',
                                           'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EMECPosInnerWheelMult',
                                           'EMECPosInnerWheelMult',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EMECPosOuterWheelMult',
                                           'EMECPosOuterWheelMult',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EMECNegInnerWheelMult',
                                           'EMECNegInnerWheelMult',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EMECNegOuterWheelMult',
                                           'EMECNegOuterWheelMult',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'EMECBackOuterBarretteMult',
                                           'EMECBackOuterBarretteMult',
                                           'LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'FCALModule1Mult',
                                           'FCALModule1Mult',
                                           'LArMgr::LAr::FCAL::Module1::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'FCALModule2Mult',
                                           'FCALModule2Mult',
                                           'LArMgr::LAr::FCAL::Module2::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'FCALModule3Mult',
                                           'FCALModule3Mult',
                                           'LArMgr::LAr::FCAL::Module3::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'HECWheelMult',
                                           'HECWheelMult',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCALMult',
                                           'MiniFCALMult',
                                           'LArMgr::MiniFCAL::Wafer')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS1')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS2')

            # Calibration SDs
            """ LAr::BarrelCryostat::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Dead',
                                           'LAr::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Cylinder::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Dead',
                                           'LAr::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::InnerWall')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Dead',
                                           'LAr::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Sector::*')
            """ LAr::BarrelCryostat::Mixed::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Mixed::Cylinder::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::OuterWall')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LAr::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::InnerEndWall')
            """ LAr::BarrelCryostat::LAr::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::MotherVolume')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::TotalLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::HalfLAr*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LAr::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::ExtraMat*')
            """ LAr::DM  dead matter in crack barrel-endCap
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::DM::Dead',
                                           'LAr::DM::Dead',
                                           'LArMgr::LAr::DM::*')
            """ LAr::Barrel::Presampler::Inactive
            LArG4Barrel/LArBarrelPresamplerSDConsultant there is one strange name --Catho3-- ??
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LArMgr::LAr::Barrel::Presampler::Prep*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LArMgr::LAr::Barrel::Presampler::Anode*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LAr::Barrel::Presampler::Inactive',
                                           'LArMgr::LAr::Barrel::Presampler::Cathode*')
            """ LAr::Barrel::Presampler::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::ProtectionShell')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::MotherBoard')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Connectics')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Rail')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LAr::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::ProtectionPlate')
            """ LAr::Barrel::Inactive
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Inactive',
                                           'LAr::Barrel::Inactive',
                                           'LArMgr::LAr::EMB::*::Straight')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Inactive',
                                           'LAr::Barrel::Inactive',
                                           'LArMgr::LAr::EMB::*::*Fold')
            """ LAr::Barrel::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::ECAM')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::TELF')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::TELB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::MOAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::CAAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::SUMB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::GTENF')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::GTENB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::G10')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Steel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Absorber2')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Dead',
                                           'LAr::Barrel::Dead',
                                           'LArMgr::LAr::EMB::ExtraMat*')
            """ LAr::EndcapCryostat::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cylinder')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cone')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Cylinder')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Dead',
                                           'LAr::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::FcalLAr::Cylinder')
            """ LAr::EndcapCryostat::LAr::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::MotherVolume')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::FcalLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LAr::EndcapCryostat::LAr::Dead',
                                           'LArMgr::Moderator*')
            """ LAr::EndcapCryostat::Mixed::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cone::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Sector::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LAr::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::FCAL::LiquidArgonC')
            """ LAr::EMEC::InnerWheel::Inactive
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel::Glue')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LAr::EMEC::Pos::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel::Lead')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel::Glue')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LAr::EMEC::Neg::InnerWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel::Lead')

            """ LAr::EMEC::OuterWheel::Inactive
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel::Glue')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LAr::EMEC::Pos::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel::Lead')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel::Glue')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LAr::EMEC::Neg::OuterWheel::Inactive',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel::Lead')
            """ LAr::EMECSupport::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Mother*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Front*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::BackSupport*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Barrettes')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Barrette::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Abs')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Ele')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Ring')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Bar')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Hole')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*GTen')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Top*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::*Stretchers')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::OuterSupportMother')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::OuterTransversalBars')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::InnerAluCone::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMECSupport::Dead',
                                           'LAr::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::InnerTransversalBars')
            """ LAr::HEC::Inactive
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice::TieRod')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice::TieRodDead')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LAr::HEC::Wheel::Inactive',
                                           'LArMgr::LAr::HEC::Module::Depth::Absorber::TieRod')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCAL::Inactive',
                                           'MiniFCAL::Inactive',
                                           'LArMgr::MiniFCAL')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCALMother::Inactive',
                                           'MiniFCALMother::Inactive',
                                           'LArMgr::MiniFCALMother')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCAL::Layer::Inactive',
                                           'MiniFCAL::Layer::Inactive',
                                           'LArMgr::MiniFCAL::Layer')
            """ LAr::HEC::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Mother')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Clamp')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Clamp::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::FrontModule')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::RearModule')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth::FirstAbsorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Wheel::Dead',
                                           'LAr::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth::FirstAbsorber::TieRod')
            """ LAr::FCAL::Inactive1
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive1',
                                           'LAr::FCAL::Inactive1',
                                           'LArMgr::LAr::FCAL::Module1::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive1',
                                           'LAr::FCAL::Inactive1',
                                           'LArMgr::LAr::FCAL::Module1::CableTrough')
            """ LAr::FCAL::Inactive2
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive2',
                                           'LAr::FCAL::Inactive2',
                                           'LArMgr::LAr::FCAL::Module2::Rod')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive2',
                                           'LAr::FCAL::Inactive2',
                                           'LArMgr::LAr::FCAL::Module2::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive2',
                                           'LAr::FCAL::Inactive2',
                                           'LArMgr::LAr::FCAL::Module2::CableTrough')
            """ LAr::FCAL::Inactive3
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive3',
                                           'LAr::FCAL::Inactive3',
                                           'LArMgr::LAr::FCAL::Module3::Rod')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive3',
                                           'LAr::FCAL::Inactive3',
                                           'LArMgr::LAr::FCAL::Module3::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Inactive3',
                                           'LAr::FCAL::Inactive3',
                                           'LArMgr::LAr::FCAL::Module3::CableTrough')
        elif mode == 2:
            # dead matter mode
            # active LAr => normal hits only
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMB::STAC',
                                           'LAr::EMB::STAC',
                                           'LArMgr::LAr::EMB::STAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Barrel::Presampler::Module',
                                           'LAr::Barrel::Presampler::Module',
                                           'LArMgr::LAr::Barrel::Presampler::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::Endcap::Presampler::LiquidArgon',
                                           'LAr::Endcap::Presampler::LiquidArgon',
                                           'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::InnerWheel',
                                           'LAr::EMEC::Pos::InnerWheel',
                                           'LArMgr::LAr::EMEC::Pos::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Pos::OuterWheel',
                                           'LAr::EMEC::Pos::OuterWheel',
                                           'LArMgr::LAr::EMEC::Pos::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::InnerWheel',
                                           'LAr::EMEC::Neg::InnerWheel',
                                           'LArMgr::LAr::EMEC::Neg::InnerWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::Neg::OuterWheel',
                                           'LAr::EMEC::Neg::OuterWheel',
                                           'LArMgr::LAr::EMEC::Neg::OuterWheel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::EMEC::BackOuterBarrette::Module::Phidiv',
                                           'LAr::EMEC::BackOuterBarrette::Module::Phidiv',
                                           'LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module1::Gap',
                                           'LAr::FCAL::Module1::Gap',
                                           'LArMgr::LAr::FCAL::Module1::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module2::Gap',
                                           'LAr::FCAL::Module2::Gap',
                                           'LArMgr::LAr::FCAL::Module2::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::FCAL::Module3::Gap',
                                           'LAr::FCAL::Module3::Gap',
                                           'LArMgr::LAr::FCAL::Module3::Gap')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LAr::HEC::Module::Depth::Slice::Wheel',
                                           'LAr::HEC::Module::Depth::Slice::Wheel',
                                           'LArMgr::LAr::HEC::Module::Depth::Slice')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'MiniFCAL::Wafer',
                                           'MiniFCAL::Wafer',
                                           'LArMgr::MiniFCAL::Wafer')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS1')
            self.atlas_lar.add_SenDetector('MinBiasScintillator',
                                           'MinBiasScintillatorSD',
                                           'MBTS',
                                           'LArMgr::MBTS2')
            """ LAr::BarrelCryostat::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Cylinder::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::InnerWall')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArDead::BarrelCryostat::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Sector::*')
            """ LAr::BarrelCryostat::Mixed::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::Mixed::Cylinder::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::OuterWall')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArDead::BarrelCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::InnerEndWall')
            """ LAr::BarrelCryostat::LAr::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::MotherVolume')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::TotalLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::HalfLAr*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArDead::BarrelCryostat::LAr::Dead',
                                           'LArMgr::LAr::Barrel::Cryostat::ExtraMat*')
            """ LAr::DM  dead matter in crack barrel-endCap
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::DM::Dead',
                                           'LArDead::DM::Dead',
                                           'LArMgr::LAr::DM::*')
            """ LAr::Barrel::Presampler::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::ProtectionShell')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::MotherBoard')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Connectics')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::Rail')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArDead::Barrel::Presampler::Dead',
                                           'LArMgr::LAr::Barrel::Presampler::ProtectionPlate')
            """ LAr::Barrel::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::ECAM')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::TELF')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::TELB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::MOAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::CAAC')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::SUMB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::GTENF')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::GTENB')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Electrode')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::G10')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Steel')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Absorber2')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::FrontBack::Absorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::Barrel::Dead',
                                           'LArDead::Barrel::Dead',
                                           'LArMgr::LAr::EMB::ExtraMat*')
            """ LAr::EndcapCryostat::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cylinder')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cone')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Cylinder')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArDead::EndcapCryostat::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::FcalLAr::Cylinder')
            """ LAr::EndcapCryostat::LAr::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::MotherVolume')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::FcalLAr')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArDead::EndcapCryostat::LAr::Dead',
                                           'LArMgr::Moderator*')
            """ LAr::EndcapCryostat::Mixed::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Cone::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::Sector::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::Endcap::Cryostat::EmecHecLAr::Sector::Mixed')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArDead::EndcapCryostat::Mixed::Dead',
                                           'LArMgr::LAr::FCAL::LiquidArgonC')
            """ LAr::EMECSupport::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Mother*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Front*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::BackSupport*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Barrettes')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Barrette::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Abs')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Ele')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Ring')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Bar')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*Hole')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Back*GTen')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::Top*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::*Stretchers')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::OuterSupportMother')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::OuterTransversalBars')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::InnerAluCone::*')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::EMECSupport::Dead',
                                           'LArDead::EMECSupport::Dead',
                                           'LArMgr::LAr::EMEC::InnerTransversalBars')
            """ LAr::HEC::Dead
            """
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Mother')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Clamp')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Clamp::LiquidArgon')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::FrontModule')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::RearModule')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth::FirstAbsorber')
            self.atlas_lar.add_SenDetector('LArG4SD',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArDead::HEC::Wheel::Dead',
                                           'LArMgr::LAr::HEC::Module::Depth::FirstAbsorber::TieRod')

    def _initPR(self):
        """
        Describe the physics regions.
        """
        # Only two sets of range cuts have been explored : one for 'default' simulation, and one for QGSP_BERT
        # BERT which, because of the new multiple scattering model, can have bigger range cuts.
        # But those bigger range cuts have not been validated with any other physics list, so we leave the
        # default as the much smaller ones.
        rangeEMB  = 0.03
        rangeEMEC = 0.03
        rangeHEC  = 0.03
        rangeFCAL = 0.03
        if simFlags.PhysicsList() == 'QGSP_BERT':
            rangeEMB  = 0.1
            rangeEMEC = 0.1
            rangeHEC  = 1.
            rangeFCAL = 0.03

        self.atlas_lar_PhysReg=PhysicsReg('EMB')
        self.atlas_lar_PhysReg.add_Volumes('LArMgr::LAr::EMB::STAC')
        self.atlas_lar_PhysReg.add_Cuts('e-',rangeEMB)
        self.atlas_lar_PhysReg.add_Cuts('e+',rangeEMB)
        self.atlas_lar_PhysReg.add_Cuts('gamma',rangeEMB)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg)

        self.atlas_lar_PhysReg1=PhysicsReg('EMEC')
        self.atlas_lar_PhysReg1.add_Volumes('LArMgr::LAr::EMEC::Mother')
        self.atlas_lar_PhysReg1.add_Cuts('e-',rangeEMEC)
        self.atlas_lar_PhysReg1.add_Cuts('e+',rangeEMEC)
        self.atlas_lar_PhysReg1.add_Cuts('gamma',rangeEMEC)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg1)

        self.atlas_lar_PhysReg2=PhysicsReg('HEC')
        self.atlas_lar_PhysReg2.add_Volumes('LArMgr::LAr::HEC::LiquidArgon')
        self.atlas_lar_PhysReg2.add_Cuts('e-',rangeHEC)
        self.atlas_lar_PhysReg2.add_Cuts('e+',rangeHEC)
        self.atlas_lar_PhysReg2.add_Cuts('gamma',rangeHEC)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg2)

        self.atlas_lar_PhysReg3=PhysicsReg('FCAL')
        self.atlas_lar_PhysReg3.add_Volumes('LArMgr::LAr::FCAL::LiquidArgonC')
        self.atlas_lar_PhysReg3.add_Cuts('e-',rangeFCAL)
        self.atlas_lar_PhysReg3.add_Cuts('e+',rangeFCAL)
        self.atlas_lar_PhysReg3.add_Cuts('gamma',rangeFCAL)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg3)

    def _initParameterization(self):
        """  Creates the needed regions and applies the FastSimulation
            models.
             The usual LAr physics regions are re-created here for the
            case we use the parametrization models. The EMB region will
            be used as well to trigger the fast simulation models. The
            parametrization regions: EMECPara,FCALPara,FCAL2Para will
            have the usual 30 microns cuts
        """
        # new stuff with G4.8
        # create the physical regions

        rangeEMB  = 0.03
        rangeEMEC = 0.03
        rangeHEC  = 0.03
        rangeFCAL = 0.03
        # Only two sets of range cuts have been explored : one for 'default' simulation, and one for QGSP_BERT
        # BERT which, because of the new multiple scattering model, can have bigger range cuts.
        # But those bigger range cuts have not been validated with any other physics list, so we leave the
        # default as the much smaller ones.
        if simFlags.PhysicsList() == 'QGSP_BERT':
            rangeEMB  = 0.1
            rangeEMEC = 0.1
            rangeHEC  = 1.
            rangeFCAL = 0.03

        self.atlas_lar_PhysReg=PhysicsReg('EMB')
        self.atlas_lar_PhysReg.add_Volumes('LArMgr::LAr::EMB::STAC')
        self.atlas_lar_PhysReg.add_Cuts('e-',rangeEMB)
        self.atlas_lar_PhysReg.add_Cuts('e+',rangeEMB)
        self.atlas_lar_PhysReg.add_Cuts('gamma',rangeEMB)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg)

        self.atlas_lar_PhysReg1=PhysicsReg('EMEC')
        self.atlas_lar_PhysReg1.add_Volumes('LArMgr::LAr::EMEC::Mother')
        self.atlas_lar_PhysReg1.add_Cuts('e-',rangeEMEC)
        self.atlas_lar_PhysReg1.add_Cuts('e+',rangeEMEC)
        self.atlas_lar_PhysReg1.add_Cuts('gamma',rangeEMEC)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg1)

        self.atlas_lar_PhysReg2=PhysicsReg('HEC')
        self.atlas_lar_PhysReg2.add_Volumes('LArMgr::LAr::HEC::LiquidArgon')
        self.atlas_lar_PhysReg2.add_Cuts('e-',rangeHEC)
        self.atlas_lar_PhysReg2.add_Cuts('e+',rangeHEC)
        self.atlas_lar_PhysReg2.add_Cuts('gamma',rangeHEC)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg2)

        self.atlas_lar_PhysReg3=PhysicsReg('FCAL')
        self.atlas_lar_PhysReg3.add_Volumes('LArMgr::LAr::FCAL::LiquidArgonC')
        self.atlas_lar_PhysReg3.add_Cuts('e-',rangeFCAL)
        self.atlas_lar_PhysReg3.add_Cuts('e+',rangeFCAL)
        self.atlas_lar_PhysReg3.add_Cuts('gamma',rangeFCAL)
        self.atlas_lar.add_PhysicsReg(self.atlas_lar_PhysReg3)

        # The main STAC Physics region does double duty:
        self.atlas_EMBPara = self.atlas_lar_PhysReg;

        # Other physics regions must be created:
        self.atlas_EMECPara = PhysicsReg("EMECPara")
        self.atlas_EMECPara.add_Volumes("LArMgr::LAr::EMEC::Pos::InnerWheel")
        self.atlas_EMECPara.add_Volumes("LArMgr::LAr::EMEC::Pos::OuterWheel")
        self.atlas_EMECPara.add_Volumes("LArMgr::LAr::EMEC::Neg::InnerWheel")
        self.atlas_EMECPara.add_Volumes("LArMgr::LAr::EMEC::Neg::OuterWheel")
        self.atlas_EMECPara.add_Cuts('e-',rangeEMEC)
        self.atlas_EMECPara.add_Cuts('e+',rangeEMEC)
        self.atlas_EMECPara.add_Cuts('gamma',rangeEMEC)

        self.atlas_FCALPara = PhysicsReg("FCALPara")
        self.atlas_FCALPara.add_Volumes("LArMgr::LAr::FCAL::Module1::Absorber")
        self.atlas_FCALPara.add_Cuts('e-',rangeFCAL)
        self.atlas_FCALPara.add_Cuts('e+',rangeFCAL)
        self.atlas_FCALPara.add_Cuts('gamma',rangeFCAL)

        self.atlas_FCAL2Para = PhysicsReg("FCAL2Para")
        self.atlas_FCAL2Para.add_Volumes("LArMgr::LAr::FCAL::Module2::Absorber")
        self.atlas_FCAL2Para.add_Volumes("LArMgr::LAr::FCAL::Module3::Absorber")
        self.atlas_FCAL2Para.add_Cuts('e-',rangeFCAL)
        self.atlas_FCAL2Para.add_Cuts('e+',rangeFCAL)
        self.atlas_FCAL2Para.add_Cuts('gamma',rangeFCAL)

        # create the FastSimModel obj
        self.atlas_EMBFastSimModel=FastSimModel('LArG4FastSimulation','EMBFastShower')
        self.atlas_EMECFastSimModel=FastSimModel('LArG4FastSimulation','EMECFastShower')
        self.atlas_FCALFastSimModel=FastSimModel('LArG4FastSimulation','FCALFastShower')
        self.atlas_FCAL2FastSimModel=FastSimModel('LArG4FastSimulation','FCAL2FastShower')

        # add regions to the FastSimModel obj
        self.atlas_EMBFastSimModel.add_Region(self.atlas_EMBPara)
        self.atlas_EMECFastSimModel.add_Region(self.atlas_EMECPara)
        self.atlas_FCALFastSimModel.add_Region(self.atlas_FCALPara)
        self.atlas_FCAL2FastSimModel.add_Region(self.atlas_FCAL2Para)

        # add the FastSimModel to the G4Eng
        G4Eng.add_FastSimModel(self.atlas_EMBFastSimModel)
        G4Eng.add_FastSimModel(self.atlas_EMECFastSimModel)
        G4Eng.add_FastSimModel(self.atlas_FCALFastSimModel)
        G4Eng.add_FastSimModel(self.atlas_FCAL2FastSimModel)

        if simFlags.LArParameterization.get_Value() > 1:
            self.PreSampLAr = PhysicsReg('PreSampLAr')
            self.PreSampLAr.add_Volumes('LArMgr::LAr::Endcap::Presampler::LiquidArgon')
            self.atlas_lar.add_PhysicsReg(self.PreSampLAr)

            self.atlas_DeadMat = PhysicsReg('DeadMaterial')
            sectionList = range(16,49) # does not include 49
            sectionList += [ 51, 52, 53, 54 ]
            for section in sectionList:
                self.atlas_DeadMat.add_Volumes('BeamPipe::SectionF'+str(section))
            self.atlas_DeadMat.add_Volumes('LArMgr::LAr::Endcap::Cryostat::Cylinder')
            self.atlas_DeadMat.add_Volumes('LArMgr::LAr::Endcap::Cryostat::Cylinder::Mixed')
            self.atlas_DeadMat.add_Volumes('LArMgr::LAr::Endcap::Cryostat::Cone::Mixed')
            self.atlas_DeadMat.add_Volumes('LArMgr::LAr::Endcap::Cryostat::Cone')
            self.atlas_DeadMat.add_Volumes('DiskShieldingPlugs')
            self.atlas_DeadMat.add_Volumes('ToroidShieldingInnerPlugs')
            self.atlas_DeadMat.add_Volumes('ForwardShieldingMainCylinder')
            self.atlas_DeadMat.add_Cuts('e-',1.0)
            self.atlas_DeadMat.add_Cuts('e+',1.0)
            self.atlas_DeadMat.add_Cuts('gamma',1.0)
            self.atlas_DeadMatFastSimModel=FastSimModel('LArG4FastSimulation','DeadMaterialShower')
            self.atlas_DeadMatFastSimModel.add_Region(self.atlas_DeadMat)
            G4Eng.add_FastSimModel(self.atlas_DeadMatFastSimModel)


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# -- Tile --


class PyTileSimUtils(DetConfigurator):
    """
    Class that inherits from G4AtlasApps.DetConfigurator
    and it builds the TileSimUtils in accordance with the
    user requirements.

    It uses the TileSimUtilsDict dictionary.
    """

    ## __init__() method is not needed

    def build(self, mode):
        """
        Builds the TileSimUtils configurator in different modes
        Current modes are:
          standard

        All the information is saved in StoreGate. The options given
        here can act only over the SD.
        """
        import ROOT
        if mode == 'standard':
            self.TileG4SimOptions = G4Eng.gbl.TileG4SimOptions()
            self.TileG4SimOptions.saveMe()
            ## We need the following line because otherwise StoreGate will
            ## delete the object first and pylcgdict will complain.
            ROOT.SetOwnership(self.TileG4SimOptions, 0)



class Tile(object):
    """
    Describes the ATLAS Tile detector.
    """

    def __init__(self,allowMods=False):
        """
        Describes the geometry of the Tile.
        """
        self.atlas_tile=DetFacility("GeoDetector","Tile:Tile",allowMods)

    def _initSD(self,mode):
        """
        Describes the sensitive detector.

          0 --> Normal Tile sensitive detector
          1 --> Tile sensitive detector for calibration hits
        """
        if mode == 0:
            # normal mode
            self.atlas_tile.add_SenDetector('TileGeoG4SD','TileGeoG4SD', 'TileGeoG4SD', 'Tile::Scintillator')
        elif mode == 1:
            # calibration mode
            self.atlas_tile.add_SenDetector('TileGeoG4SD:TileGeoG4Calib', 'TileGeoG4CalibSD',
                                            'TileGeoG4CalibSD', 'Tile::Scintillator')
            self.atlas_tile_SenDetectorCalib = G4Eng.Dict_SenDetector.get('TileGeoG4CalibSD')
            tile_volumes = ['Tile::Tile',
                            'Tile::TileCentralBarrel',
                            'Tile::TileEndcapPos',
                            'Tile::TileEndcapNeg',
                            'Tile::Barrel',
                            'Tile::BarrelModule',
                            'Tile::FrontPlate',
                            'Tile::EndPlate1',
                            'Tile::EPHole1',
                            'Tile::EndPlate2',
                            'Tile::EPHole2',
                            'Tile::GirderMother',
                            'Tile::GirderIron',
                            'Tile::GirderAluminium',
                            'Tile::GirderElectronics',
                            'Tile::Absorber',
                            'Tile::AbsorberChild',
                            'Tile::Period',
                            'Tile::Glue',
                            'Tile::Wrapper',
                            'Tile::EBarrel',
                            'Tile::EBarrelModule',
                            'Tile::EndPlateSh',
                            'Tile::ITC',
                            'Tile::ITCModule',
                            'Tile::Plug1Module',
                            'Tile::FrontPlateSh',
                            'Tile::Plug2Module',
                            'Tile::Gap',
                            'Tile::GapModule',
                            'Tile::IrUp',
                            'Tile::IrDw',
                            'Tile::Iron4',
                            'Tile::Iron3',
                            'Tile::Iron2',
                            'Tile::Iron1',
                            'Tile::IrBox',
                            'Tile::SaddleModule',
                            'Tile::LArService',
                            'Tile::LArCables',
                            'Tile::ExtBarrelSaddleSupport',
                            'Tile::Crack',
                            'Tile::CrackModule',
                            'Tile::FingerModule',
                            'Tile::FingerIron',
                            'Tile::FingerAluminum',
                            'Tile::FingerElectronics',
                            'Tile::EFinger',
                            'Tile::EFingerModule',
                            'Tile::FingerPos',
                            'Tile::FingerNeg',
                            'Tile::SaddlePos',
                            'Tile::SaddleNeg',
                            'Tile::ESaddlePos',
                            'Tile::ESaddleNeg']
            for i_v in tile_volumes:
                self.atlas_tile_SenDetectorCalib.add_Volume(i_v)
