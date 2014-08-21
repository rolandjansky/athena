# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ISF_Geant4Config.iGeant4 import G4Eng

class UserAction:
    """
    UserActions.

    Gives to the user the possibility of interact with the simulation
    at the beginning and the end of a run, at the beginning and the end
    of each event, and/or at every step.
    """

    def __init__(self, userActionLib, actionName, listStatus):
        """
        An UserAction must be instantiated with the name of the
        library (userActionLib) that contains the action (actionName)
        and a list of simulation states to apply it (BeginOfRun,
        EndOfRun, BeginOfEvent, EndOfEvent, Step)

        To activate an UserAction, it must be added to the simulation
        engine G4Eng.
        """
        self._Built = False
        self.Lib = userActionLib
        self.Name = actionName
        self.ListStatus = listStatus
        self.Properties = dict()


    def set_Properties(self, propDict):
        self.Properties = propDict


    def _construct(self):
        if self.Lib not in G4Eng.List_LoadedLib:
            G4Eng._ctrl.load(self.Lib)
            G4Eng.List_LoadedLib.append(self.Lib)
        for i in self.ListStatus:
            G4Eng._ctrl.actMenu.actionRegister(self.Name,i)
            G4Eng.log.debug('UserAction: constructing user action %s and applying it to: %s' % (self.Name, i))
        for k in self.Properties.keys():
            G4Eng._ctrl.actMenu.actionProperty(self.Name, k, self.Properties[k])
        self._Built = True
        G4Eng._app_profiler('_construct UserAction: %s' % self.Name)
