# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ISF_Geant4Config.iGeant4 import G4Eng

class SenDetector:
    """ Sensitive detector.

       A sensitive detector object when assigned to a volume, makes it
       sensitive, the simulation hits are created. Sensitive detectors
       can be built directly using a given method in the DetFacility
       object.
    """

    def __init__(self, lib, sdname, name, volume='', allowMods=False):
        """   The initial list of volumes in which the SD will be
            applied can be empty, but later it has to be populated
            with the add_Volume method.
        """
        self._Built = False
        self.Name = name
        self.Library = lib
        self.List_Volumes = list()
        if volume:
            self.List_Volumes.append(volume)
        self.SenDetectorFacility = sdname
        self.AllowMods = allowMods


    def add_Volume(self, vol):
        """ Adds more volumes to the List_Volumes in which the
           SenDetector will be applied.
        """
        self.List_Volumes.append(vol)


    def _build(self):
        if self.Library not in G4Eng.List_LoadedLib:
            G4Eng.load_Lib(self.Library)
        G4Eng._ctrl.sendetectorMenu.useSensitiveDetector(self.SenDetectorFacility, self.Name,
                                                                 self.AllowMods)
        for v_n in self.List_Volumes:
            G4Eng._ctrl.geometryMenu.AssignSD(v_n, self.Name)
            G4Eng.log.debug('SenDetector:_build: %s associated to the volume %s' % (self.Name, v_n))
        self._Built=True
        G4Eng._app_profiler('_build SenDetector: %s' % self.Name)
        if (len(self.List_Volumes)==0):
            G4Eng.log.warning(' SenDetector:_build: %s is not associated to any volume' % self.Name)
