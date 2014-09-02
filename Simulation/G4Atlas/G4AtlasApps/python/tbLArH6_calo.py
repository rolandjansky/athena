# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
 LAr H6 Calo detector description.

"""

__author__ = 'Joe Boudreau, Mohsen Kakhzad, Margret Fincke-Keeler, \
              J.P. Archambault, M. Gallas, A. Soukharev'

import math
from G4AtlasApps import PyG4Atlas, AtlasG4Eng
from G4AtlasApps.PyG4Atlas import  DetFacility, PhysicsReg


# -- World volume --
class LArWorld(object):
    """ Describes the world volume for the LArH6 TBs
    """
    def __init__(self,mode):
        """ Depending on thee LArH6 TB the world volume box will have
           different dimensions

            + mode can be:
                 'tb_LArH6_2003', 'tb_LArH6_2002', 'tb_LArH6EC_2002', 'tb_LArH6_2004'
        """
        self.world=PyG4Atlas.DetFacility("ResizeableBox","LARTBWORLD")
        if (mode=='tb_LArH6_2003'):
            self.world.df.SetDx(25500.)
            self.world.df.SetDy(25500.)
            self.world.df.SetDz(50500.)
        elif (mode=='tb_LArH6_2002' or mode=='tb_LArH6_2004'):
            self.world.df.SetDx(14500.)
            self.world.df.SetDy(14500.)
            self.world.df.SetDz(50500.)
        elif (mode=='tb_LArH6EC_2002'):
            self.world.df.SetDx(5500.)
            self.world.df.SetDy(5500.)
            self.world.df.SetDz(15500.)

# -- Position of the LAr modules --
class Det_Layouts:
    """ Describes the position of the LAr modules in the diffeerent
       test beams
    """
    def LArTBpositions(tb_case):

        AtlasG4Eng.G4Eng.load_Dict('LArG4RunControlDict')
        import ROOT
        import __main__
        from G4AtlasApps.SimFlags import simFlags
        if (simFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
            pd=AtlasG4Eng.G4Eng.gbl.LArGeoTBH1GeoOptions()
            if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : pd.printMe()
            if simFlags.LArTB_H1CryoXPos.statusOn:
                pd.CryoXPosition(simFlags.LArTB_H1CryoXPos())
            if simFlags.LArTB_H1TableYPos.statusOn:
                pd.TableYPosition(simFlags.LArTB_H1TableYPos())
            ROOT.SetOwnership(pd,0)
            pd.saveMe()
            if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : pd.printMe()
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
            pd=AtlasG4Eng.G4Eng.gbl.LArGeoTBGeometricOptions()
            if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : pd.printMe()
            if simFlags.LArTBEC_CryoEtaPos.statusOn:
                pd.CryoEtaPosition(simFlags.LArTBEC_CryoEtaPos.get_Value())
            if simFlags.LArTBEC_CryoPhiPos.statusOn:
                pd.CryoPhiPosition(simFlags.LArTBEC_CryoPhiPos.get_Value())
            ROOT.SetOwnership(pd,0)
            pd.saveMe()
            if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : pd.printMe()
        elif(simFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
            pd=AtlasG4Eng.G4Eng.gbl.LArGeoTB2004Options()
            if simFlags.LArTB_H1CryoXPos.statusOn:
                pd.CryoXPosition(simFlags.LArTB_H1CryoXPos())
            if simFlags.LArTB_H1TableYPos.statusOn:
                pd.TableYPosition(simFlags.LArTB_H1TableYPos())
            if simFlags.LArTB_H6Run1.statusOn:
                pd.Run1(simFlags.LArTB_H6Run1())
            if simFlags.LArTB_H6Hec.statusOn:
                pd.Hec(simFlags.LArTB_H6Hec())
            if simFlags.LArTB_H6Emec.statusOn:
                pd.Emec(simFlags.LArTB_H6Emec())
            if simFlags.LArTB_H6Fcal.statusOn:
                pd.Fcal(simFlags.LArTB_H6Fcal())
            if simFlags.LArTB_H6Coldnose.statusOn:
                pd.Coldnose(simFlags.LArTB_H6Coldnose())
            ROOT.SetOwnership(pd,0)
            pd.saveMe()
            if (AtlasG4Eng.G4Eng.log.level <= AtlasG4Eng.G4Eng.log.info) : pd.printMe()

    from G4AtlasApps.SimFlags import simFlags
    if (simFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
        LArTBpositions(simFlags.SimLayout.get_Value())
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
        LArTBpositions(simFlags.SimLayout.get_Value())
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2003"):
        LArTBpositions(simFlags.SimLayout.get_Value())
    elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
        LArTBpositions(simFlags.SimLayout.get_Value())

    del LArTBpositions

# MCTruth in the volume LArMgr::LArTB::H62002 for H62002

# -- LAr --
class LArH6(object):
    """ Describes the LAr for the H6 test beam.
    """
    def __init__(self):
        """ Inits the geometry of the LAr for the H6 test beam.
        """
        AtlasG4Eng.G4Eng.load_Lib("LArG4HitManagement")
        self.H6_lar=PyG4Atlas.DetFacility("GeoDetector","LArMgr:LArMgr")
    def _initSD(self, mode):
        """ Describes the sensitive detector for LAr H6

            modes:
            0-->Regular Hits
            1-->Calibration Run
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.DetFlags import DetFlags
        if (simFlags.SimLayout.get_Value()=="tb_LArH6_2003"):
            if (DetFlags.FCal_on()):
                if(mode==0):
                    # Regular Hits
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'LAr::FCAL::Module1::Gap',\
                                          'LAr::FCAL::Module1::Gap',\
                                          'LArMgr::LAr::FCAL::Module1::Gap')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'LAr::FCAL::Module2::Gap',\
                                          'LAr::FCAL::Module2::Gap',\
                                          'LArMgr::LAr::FCAL::Module2::Gap')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'LAr::FCAL::Module3::Gap',\
                                          'LAr::FCAL::Module3::Gap',\
                                          'LArMgr::LAr::FCAL::Module3::Gap')
                elif(mode==1):
                    # Calibration mode
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'FCALModule1Mult',\
                                          'FCALModule1Mult',\
                                          'LArMgr::LAr::FCAL::Module1::Gap')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'FCALModule2Mult',\
                                          'FCALModule2Mult',\
                                          'LArMgr::LAr::FCAL::Module2::Gap')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                          'FCALModule3Mult',\
                                          'FCALModule3Mult',\
                                          'LArMgr::LAr::FCAL::Module3::Gap')
                    # Calibration SDs
                    """ LAr::FCAL::Inactive1
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive1',\
                                      'LAr::FCAL::Inactive1',\
                                      'LArMgr::LAr::FCAL::Module1::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive1',\
                                      'LAr::FCAL::Inactive1',\
                                      'LArMgr::LAr::FCAL::Module1::CableTrough')
                    """ LAr::FCAL::Inactive2
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive2',\
                                      'LAr::FCAL::Inactive2',\
                                      'LArMgr::LAr::FCAL::Module2::Rod')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive2',\
                                      'LAr::FCAL::Inactive2',\
                                      'LArMgr::LAr::FCAL::Module2::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive2',\
                                      'LAr::FCAL::Inactive2',\
                                      'LArMgr::LAr::FCAL::Module2::CableTrough')
                    """ LAr::FCAL::Inactive3
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive3',\
                                      'LAr::FCAL::Inactive3',\
                                      'LArMgr::LAr::FCAL::Module3::Rod')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive3',\
                                      'LAr::FCAL::Inactive3',\
                                      'LArMgr::LAr::FCAL::Module3::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                      'LAr::FCAL::Inactive3',\
                                      'LAr::FCAL::Inactive3',\
                                      'LArMgr::LAr::FCAL::Module3::CableTrough')
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
            if (DetFlags.HEC_on()):
                if(mode==0):
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                        'LAr::HEC::Module::Depth::Slice::Local',\
                                        'LAr::HEC::Module::Depth::Slice::Local',\
                                        'LArMgr::LAr::HEC::Module::Depth::Slice')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::Endcap::Presampler::LiquidArgon',\
                                       'LAr::Endcap::Presampler::LiquidArgon',\
                                       'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::InnerWheel',\
                                       'LAr::EMEC::Pos::InnerWheel',\
                                       'LArMgr::LAr::EMEC::Pos::InnerWheel')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::OuterWheel',\
                                       'LAr::EMEC::Pos::OuterWheel',\
                                       'LArMgr::LAr::EMEC::Pos::OuterWheel')
                elif(mode==1):
                    # Calibration mode
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'EndcapPresamplerMult',\
                                       'EndcapPresamplerMult',\
                                       'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'EMECPosInnerWheelMult',\
                                       'EMECPosInnerWheelMult',\
                                       'LArMgr::LAr::EMEC::Pos::InnerWheel')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'EMECPosOuterWheelMult',\
                                       'EMECPosOuterWheelMult',\
                                       'LArMgr::LAr::EMEC::Pos::OuterWheel')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'HECMult',\
                                       'HECMult',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice')
                    # Calibration SDs
                    """ LAr::HEC::Inactive
                    """
                    AtlasG4Eng.G4Eng.log.info(' tbLArH6_calo:'+\
                    'SD calibration hits for tb_LArH6_2002 are being tried out.')
                    """ LAr::EMEC::InnerWheel::Inactive
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::InnerWheel::Inactive',\
                                       'LAr::EMEC::Pos::InnerWheel::Inactive',\
                                       'LArMgr::LAr::EMEC::Pos::InnerWheel::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::InnerWheel::Inactive',\
                                       'LAr::EMEC::Pos::InnerWheel::Inactive',\
                                       'LArMgr::LAr::EMEC::Pos::InnerWheel::Electrode')
                    """ LAr::EMEC::OuterWheel::Inactive
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::OuterWheel::Inactive',\
                                       'LAr::EMEC::Pos::OuterWheel::Inactive',\
                                       'LArMgr::LAr::EMEC::Pos::OuterWheel::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::EMEC::Pos::OuterWheel::Inactive',\
                                       'LAr::EMEC::Pos::OuterWheel::Inactive',\
                                       'LArMgr::LAr::EMEC::Pos::OuterWheel::Electrode')
                    """ LAr::HEC::Inactive
                    """
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LArMgr::LAr::HEC::Module::Depth::Absorber')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper')
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LAr::HEC::Local::Inactive',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::TieRod')
                    AtlasG4Eng.G4Eng.log.warning(' tbLArH6_calo:'+\
                    'Specific DM calibration hits for tb_LArH6_2002 not implemented yet.')
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
            if (DetFlags.em_on()):
                if(mode==0):
                    self.H6_lar.add_SenDetector('LArG4SD',\
                                       'LAr::Endcap::Presampler::LiquidArgon',\
                                       'LAr::Endcap::Presampler::LiquidArgon',\
                                       'LArMgr::LAr::Endcap::Presampler::LiquidArgon')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                        'TBECInnerModuleMult',\
                                        'TBECInnerModuleMult',\
                                        'LArMgr::LAr::EMEC::InnerModule')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                        'TBECOuterModuleMult',\
                                        'TBECOuterModuleMult',\
                                        'LArMgr::LAr::EMEC::OuterModule')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                       'TBECBackOuterBarretteMult',\
                                       'TBECBackOuterBarretteMult',\
                                       'LArMgr::LAr::EMEC::BackOuterBarrette::Module::Phidiv')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                       'LArG4TBECBeamCh',
                                       'LArG4TBECBeamCh',
                                       'LArMgr::LAr::TBEC::BeamChamber0')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                       'LArG4TBECBeamCh',
                                       'LArG4TBECBeamCh',
                                       'LArMgr::LAr::TBEC::BeamChamber1')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                       'LArG4TBECBeamCh',
                                       'LArG4TBECBeamCh',
                                       'LArMgr::LAr::TBEC::BeamChamber2')
                    self.H6_lar.add_SenDetector('LArG4H6SD',\
                                       'LArG4TBECBeamCh',
                                       'LArG4TBECBeamCh',
                                       'LArMgr::LAr::TBEC::BeamChamber3')
                elif(mode==1):
                    AtlasG4Eng.G4Eng.log.warning(' tbLArH6_calo:'+\
                    'SD calibration hits for tb_LArH6EC_2002 not implemented yet.')
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
            if (simFlags.LArTB_H6Hec.get_Value()):
                # HEC
                if(mode==0):
                    # Regular Hits
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LAr::HEC::Module::Depth::Slice::Local::H6',\
                                        'LAr::HEC::Module::Depth::Slice::Local::H6',\
                                        'LArMgr::LAr::HEC::Module::Depth::Slice')
                elif(mode==1):
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'HECMult::H6',\
                                       'HECMult::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice')
                    # Calibration SDs
                    AtlasG4Eng.G4Eng.log.info(' tbLArH6_calo:'+\
                    'SD calibration hits for tb_LArH6_2004 are being tried out.')
                    """ LAr::HEC::Inactive
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Absorber')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::Electrode::Copper')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LAr::HEC::Local::Inactive::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::TieRod')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::HEC::Module')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::Slice::TieRod')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::HEC::Module::Depth::FirstAbsorber')
            if (simFlags.LArTB_H6Emec.get_Value()):
                # EMEC
                if(mode==0):
                    # Regular Hits
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::EMEC::InnerModule::H6',\
                                      'LAr::EMEC::InnerModule::H6',\
                                      'LArMgr::LAr::EMEC::InnerWheel')
                elif(mode==1):
                    # Calibration mode
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'EMECInnerModuleMult::H6',\
                                       'EMECInnerModuleMult::H6',\
                                       'LArMgr::LAr::EMEC::InnerWheel')
                    AtlasG4Eng.G4Eng.log.info(' tbLArH6_calo:'+\
                    'SD calibration hits for tb_LArH6_2004 are being tried out.')
                    """ LAr::EMEC::InnerWheel::Inactive
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::EMEC::InnerModule::Inactive::H6',\
                                       'LAr::EMEC::InnerModule::Inactive::H6',\
                                       'LArMgr::LAr::EMEC::InnerWheel::Absorber')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LAr::EMEC::InnerModule::Inactive::H6',\
                                       'LAr::EMEC::InnerModule::Inactive::H6',\
                                       'LArMgr::LAr::EMEC::InnerWheel::Electrode')
                    """ LAr::EMECSupport::Dead
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Mother*')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Front*')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::BackSupport*')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Barrettes')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Barrette::Module')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::BackInnerBarrette::Module::Phidiv')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Abs')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Ele')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Ring')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Bar')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*Hole')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Back*GTen')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::Top*')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::*Stretchers')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::InnerAluCone*')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LAr::EMECSupport::Dead::H6',\
                                               'LArMgr::LAr::EMEC::InnerTransversalBars')

            if (simFlags.LArTB_H6Fcal.get_Value()):
                # FCAL
                if(mode==0):
                    # Regular Hits
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                          'LAr::FCAL::Module1::Gap::H6',\
                                          'LAr::FCAL::Module1::Gap::H6',\
                                          'LArMgr::LAr::FCAL::Module1::Gap')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                          'LAr::FCAL::Module2::Gap::H6',\
                                          'LAr::FCAL::Module2::Gap::H6',\
                                          'LArMgr::LAr::FCAL::Module2::Gap')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                          'LAr::FCAL::ColdTC::H6',\
                                          'LAr::FCAL::ColdTC::H6',\
                                          'LArMgr::LAr::FCAL::ColdTC::Gap')
                elif(mode==1):
                    # Calibration mode
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                          'FCALModule1Mult::H6',\
                                          'FCALModule1Mult::H6',\
                                          'LArMgr::LAr::FCAL::Module1::Gap')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                          'FCALModule2Mult::H6',\
                                          'FCALModule2Mult::H6',\
                                          'LArMgr::LAr::FCAL::Module2::Gap')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                           'FCALColdTCMult::H6',\
                                           'FCALColdTCMult::H6',\
                                           'LArMgr::LAr::FCAL::ColdTC::Gap')
                    # Calibration SDs
                    """ LAr::FCAL::Inactive1
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::FCAL::Inactive1::H6',\
                                      'LAr::FCAL::Inactive1::H6',\
                                      'LArMgr::LAr::FCAL::Module1::Absorber')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::FCAL::Inactive1::H6',\
                                      'LAr::FCAL::Inactive1::H6',\
                                      'LArMgr::LAr::FCAL::Module1::CableTrough')
                    """ LAr::FCAL::Inactive2
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LArMgr::LAr::FCAL::Module2::Rod')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LArMgr::LAr::FCAL::Module2::Absorber')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LAr::FCAL::Inactive2::H6',\
                                      'LArMgr::LAr::FCAL::Module2::CableTrough')
                    """ LAr::FCAL::Inactive3
                    """
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::FCAL::LiquidArgonC')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::FCAL::ColdTC::Absorber')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::FCAL::ColdTC::Active')
                    self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                       'LArDead::Dead::H6',\
                                       'LArDead::Dead::H6',\
                                       'LArMgr::LAr::FCAL::ColdTC::Electrode')
            """ Other H6 TB sensitive detectors
            """
            if(mode==0):
            # Regular Hits
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6WarmTC::X',\
                                        'LArG4H6WarmTC::X',\
                                        'LArMgr::LAr::WarmTC::Sci::X')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6WarmTC::Y',\
                                        'LArG4H6WarmTC::Y',\
                                        'LArMgr::LAr::WarmTC::Sci::Y')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6WarmTC::Abs',\
                                        'LArG4H6WarmTC::Abs',\
                                        'LArMgr::LAr::WarmTC::Absorber')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Front',\
                                        'LArG4H6BeamSD::Front',\
                                        'LArMgr::LAr::TBH62004::FrontBeam::Scintillator')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TBH62004::MovableTable::Scintillator')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TB::BPCOLD::bpco_div')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TB::BPC::bpc_xdiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TB::BPC::bpc_ydiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TB::MWPC::XDiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArG4H6BeamSD::Movable',\
                                        'LArMgr::LAr::TB::MWPC::YDiv')
            elif(mode==1):
             # Calibration mode
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'WarmTCMult::X::H6',\
                                        'WarmTCMult::X::H6',\
                                        'LArMgr::LAr::WarmTC::Sci::X')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'WarmTCMult::Y::H6',\
                                        'WarmTCMult::Y::H6',\
                                        'LArMgr::LAr::WarmTC::Sci::Y')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'WarmTCMult::Abs::H6',\
                                        'WarmTCMult::Abs::H6',\
                                        'LArMgr::LAr::WarmTC::Absorber')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'FrontMult::H6',\
                                        'FrontMult::H6',\
                                        'LArMgr::LAr::TBH62004::FrontBeam::Scintillator')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TBH62004::MovableTable::Scintillator')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpco_div')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_xdiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_ydiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TB::MWPC::XDiv')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'MoveMult::H6',\
                                        'MoveMult::H6',\
                                        'LArMgr::LAr::TB::MWPC::YDiv')
            """ Only H6 TB dead material
            """
            if(mode==1):
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LARTBWORLD::LARTBWORLD')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::ExpHallLog')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::LeakageDet::ColdNose')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::H6::Cryostat::MotherVolume')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArTB::H6::Cryostat::WarmWall')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArTB::H6::Cryostat::Vacuum')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArTB::H6::Cryostat::ColdWall')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArTB::H6::Cryostat::LAr')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::LeakageDet::LAr')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::EMEC::Excluder')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::FCAL::Excluder')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::Front::Excluder')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LArGeoTB::Back::Excluder')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::WarmTC::MuonWall')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TBH62004::FrontBeam')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC::Mylar')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC::AlFoil')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC::XPlane')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC::YPlane')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::MWPC::Wire')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpcalmylar')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_mylar')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_xplane')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_wire')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_cwire')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPC::bpc_yplane')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TBH62004::MiddleBeam')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpcalmylar')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpc_mylar')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpco_plane')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpco_wire')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TB::BPCOLD::bpco_cwire')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::LAr::TBH62004::MovableTable')
                self.H6_lar.add_SenDetector('LArG4H62004SD',\
                                        'LArDead::Dead::H6',\
                                        'LArDead::Dead::H6',\
                                        'LArMgr::WarmTC_Mother')

    def _initPR(self):
        """ Describes the physics regions for LAr H6.
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.DetFlags    import DetFlags

        if (simFlags.SimLayout.get_Value()=="tb_LArH6_2003"):
            if (DetFlags.FCal_on()):
                self.H6_lar_PhysReg=PyG4Atlas.PhysicsReg('FCAL')
                self.H6_lar_PhysReg.add_Volumes('LArMgr::LAr::FCAL::LiquidArgonC')
                self.H6_lar_PhysReg.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg)
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2002"):
            if (DetFlags.HEC_on()):
                self.H6_lar_PhysReg=PyG4Atlas.PhysicsReg('HEC')
                self.H6_lar_PhysReg.add_Volumes('LArMgr::LAr::HEC::LiquidArgon')
                self.H6_lar_PhysReg.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg)
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6EC_2002"):
            if (DetFlags.em_on()):
                self.H6_lar_PhysReg=PyG4Atlas.PhysicsReg('EMEC')
                self.H6_lar_PhysReg.add_Volumes('LArMgr::LAr::EMEC::Mother')
                self.H6_lar_PhysReg.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg)
        elif (simFlags.SimLayout.get_Value()=="tb_LArH6_2004"):
            if (simFlags.LArTB_H6Hec.get_Value()):
                self.H6_lar_PhysReg=PyG4Atlas.PhysicsReg('HEC')
                self.H6_lar_PhysReg.add_Volumes('LArMgr::LAr::HEC::LiquidArgon')
                self.H6_lar_PhysReg.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg)
            if (simFlags.LArTB_H6Emec.get_Value()):
                self.H6_lar_PhysReg1=PyG4Atlas.PhysicsReg('EMEC')
                self.H6_lar_PhysReg1.add_Volumes('LArMgr::LAr::EMEC::Mother')
                self.H6_lar_PhysReg1.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg1.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg1.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg1)
            if (simFlags.LArTB_H6Fcal.get_Value()):
                self.H6_lar_PhysReg2=PyG4Atlas.PhysicsReg('FCAL')
                self.H6_lar_PhysReg2.add_Volumes('LArMgr::LAr::FCAL::LiquidArgonC')
                self.H6_lar_PhysReg2.add_Cuts('e-',0.03)
                self.H6_lar_PhysReg2.add_Cuts('e+',0.03)
                self.H6_lar_PhysReg2.add_Cuts('gamma',0.03)
                self.H6_lar.add_PhysicsReg(self.H6_lar_PhysReg2)

#=======================================================================


