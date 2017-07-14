# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
ATLAS calorimeter detector description
"""

__author__  = 'A. Dell`Acqua, M. Gallas, A. Di Simone '

from DetConfigurator import DetConfigurator


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
        import AtlasG4Eng
        self.LArG4GlobalOptions = AtlasG4Eng.G4Eng.gbl.LArG4GlobalOptions()

        #Temporary protection to avoid branching for MC12
        if hasattr(self.LArG4GlobalOptions, 'AllowHitModification'):
            self.LArG4GlobalOptions.AllowHitModification(self.allowMods)

        self.LArG4GlobalOptions.saveMe()
        ## We need the following line because otherwise StoreGate will
        ## delete the object first and pylcgdict will complain
        ROOT.SetOwnership(self.LArG4GlobalOptions, 0)

        ## Enable cond db reading in case we need HV maps
        from SimFlags import simFlags
        if hasattr(simFlags, 'LArHighVoltage') and simFlags.LArHighVoltage.statusOn:
            if simFlags.LArHighVoltage.get_Value() is not None:
                for calo in ["EMB", "EMEC", "HEC", "FCAL"]:
                    if calo in simFlags.LArHighVoltage.get_Value():
                        from IOVDbSvc.CondDB import conddb
                        conddb.addFolder("", "/LAR/DCS/HV/BARREl/I16")
                        conddb.addFolder("", "/LAR/DCS/HV/BARREL/I8" )
                        break

        ## TODO: Tidy this... lots of reducible repetition of the same checks
        self.LArG4BarrelOptions = AtlasG4Eng.G4Eng.gbl.LArG4BarrelOptions()
        self.LArG4BarrelOptions.EMBBirksLaw(simFlags.DoLArBirk())
        self.LArG4EMECOptions = AtlasG4Eng.G4Eng.gbl.LArG4EMECOptions()
        self.LArG4EMECOptions.EMECBirksLaw(simFlags.DoLArBirk())
        self.LArG4HECOptions = AtlasG4Eng.G4Eng.gbl.LArG4HECOptions()
        self.LArG4HECOptions.HECBirksLaw(simFlags.DoLArBirk())
        self.LArG4FCALOptions=AtlasG4Eng.G4Eng.gbl.LArG4FCALOptions()
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
        if AtlasG4Eng.G4Eng.log.level <= logging.INFO:
            self.LArG4BarrelOptions.printMe()
            self.LArG4EMECOptions.printMe()
            self.LArG4HECOptions.printMe()
            self.LArG4FCALOptions.printMe()
        ROOT.SetOwnership(self.LArG4HECOptions, 0)
        ROOT.SetOwnership(self.LArG4EMECOptions, 0)
        ROOT.SetOwnership(self.LArG4BarrelOptions, 0)
        ROOT.SetOwnership(self.LArG4FCALOptions, 0)
