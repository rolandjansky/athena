# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ISF_Geant4Config.iGeant4 import G4Eng

class PhysicsReg:
    """
    Physics regions.

    Provides the possibility of setting cuts in energy on a volume
    base and depending on the type of particle. The physics regions
    are identified by name and they have a dictionary with the
    cuts and a list with all the volumes in which they area applied.

    Attributes:
      .Dict_Cuts
      .Name
      .List_Volumes

    The physics regions are attached to the DetFacility objects.
    """

    def __init__(self, nameRegion):
        self._Built = False
        self.Dict_Cuts = dict()
        self.Name = nameRegion
        self.List_Volumes = list()


    def _construct_Region(self):
        """This is an internal function and in any case must be
           call after the construction of the Physics and after
           G4Eng.init_G4()
        """
        self._pr = G4Eng._ctrl.physicsMenu.CreateNewRegion(self.Name)
        for i in self.Dict_Cuts.keys():
            self._pr.setCut(self.Dict_Cuts.get(i), i)
            G4Eng.log.debug(' PhysicsReg:' +
                                    '_construct_Region: %s' % self.Name +
                                    ' with cut %s=%s ' % (i, str(self.Dict_Cuts.get(i))) )
        for i in self.List_Volumes:
            G4Eng.log.debug(' PhysicsReg:' +
                                    '_construct_Region: %s and added volume %s' % (self.Name, i))
            self._pr.addVolume(i)
        self._Built = True
        G4Eng._app_profiler('_construct_Region: %s' % self.Name)


    def add_Cuts(self, name_particle, cut_value):
        """
        Adds cuts for different particles.

        Cuts are stored in .Dict_Cuts
        """
        if name_particle not in self.Dict_Cuts:
            self.Dict_Cuts[name_particle] = cut_value
        else:
            G4Eng.log.warning('PhysicsReg:add_Cuts: Cut value for %s redefined to %s' % (name_particle, str(cut_value)))


    def add_Volumes(self, name_vol):
        """
        Adds volumes to the region.

        Volumes are stored in List_Volumes.
        """
        if name_vol not in self.List_Volumes:
            self.List_Volumes.append(name_vol)
        else:
            G4Eng.log.warning('PhysicsReg:add_Volumes: ' +
                                      'The region %s already exists.' % name_vol)
