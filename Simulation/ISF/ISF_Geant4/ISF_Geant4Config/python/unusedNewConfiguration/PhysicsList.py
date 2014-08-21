# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ISF_Geant4Config.iGeant4 import G4Eng

class PhysicsList:
    """ Geant4 physics lists.

       Constructs any of the physics lists provided by Geant4. The
       customization is possible in terms of general cuts for the
       whole detector and defining "cut in range" if it is needed
       for a particular sub-detector/region (see PhysicsReg).

       The physics list is constructed when it is added to the
       simulation engine G4AtlasEngine.
    """

    def __init__(self, PhysicsListName, GeneralCut):
        """ The PhysicsListName availables can be found in the
            menu_Physics at the simulation engine.

            The general cut is expressed in mm.
        """
        self._Built = False
        self.Name = PhysicsListName
        self.Value_gen_cut = GeneralCut


    # TODO: Where is this called?! ---> ELLI: from _init_Physics in PyG4Atlas
    def _construct(self):
        if "G4PhysicsLists" not in G4Eng.List_LoadedLib:
            G4Eng._ctrl.load("G4PhysicsLists")
            G4Eng.List_LoadedLib.append("G4PhysicsLists")
        G4Eng._ctrl.physicsMenu.UsePhysicsList(self.Name)
        G4Eng._ctrl.physicsMenu.SetGeneralCut(self.Value_gen_cut)
        if not self._Built:
            G4Eng._ctrl.physicsMenu.ConstructPhysicsList()
            G4Eng.log.debug('PhysicsList:_construct: %s with a general cut of %e mm' % (self.Name, self.Value_gen_cut))
            self._Built = True
