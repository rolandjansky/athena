# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
 LAr H6 Calo detector description.

"""

__author__ = 'Joe Boudreau, Mohsen Kakhzad, Margret Fincke-Keeler, \
              J.P. Archambault, M. Gallas, A. Soukharev'

from G4AtlasApps import PyG4Atlas, AtlasG4Eng

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
        self.H6_lar=PyG4Atlas.DetFacility("GeoDetector","LArMgr:LArMgr")
    def _initSD(self, mode):
        """ Describes the sensitive detector for LAr H6

            modes:
            0-->Regular Hits
            1-->Calibration Run
        """

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


