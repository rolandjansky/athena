# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path, string, sys
from time import time
from GaudiPython import PyAlgorithm
from AthenaCommon import Constants, Logging, SystemOfUnits
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
import PyCintex
from ISF_Geant4Config.iGeant4 import G4Eng
from ISF_Geant4Config.SenDetector import SenDetector

class DetFacility:
    """ DetFacility is the entry point for a sub-detector's geometry.

    Sets up the geometry of the sub-detector envelope and its contains.

    The available types are:
    - Atlas envelopes: AtlasDetector, BeamPipe, InnerDetector
                       Calorimeter, Muon, MuonSystemEntryLayer
    - Generic (resizable) envelopes: ResizeableBox, ResizeableTubs,
                                     GenericPCON, GenericPGON
    - GeoDetectors from the GeoModel interface.
    - Other detector descriptions loading the appropiate library

    DetFacility can be positioned within other DetFacilities, moved,
    rotated... And they must be added to the simulation engine:
    G4Eng.add_DetFacility(self, det_obj, det_obj_father)
    """

    def __init__(self, type_name, name, allowMods=False):
        self.Description = 'Detector facility of type %s; more information from the user follows:\n'
        self.Dict_DetFacility = dict()
        self.Dict_Regions = dict()
        self.Dict_SenDetector = dict()
        self.Dict_MaxStep = dict()
        self.Dict_MaxTime = dict()
        self.Father = '' # TODO: use None instead?
        self.Name = name
        self.Type = type_name
        self.df = G4Eng._ctrl.geometryMenu.GetDetectorFacility(type_name, name)
        self._Built = True
        self._PostBuilt = False
        self._BuiltAssoc = False
        self._AllowMods = allowMods


    def _build(self):
        # Fake method in order to keep compatibility in between DetFacility
        # and DetFacilityT
        pass


    def _postbuild(self):
        """ We set here the MaxStep and MaxTime per volume
        """
        for v in self.Dict_MaxStep.keys():
            G4Eng._ctrl.geometryMenu.SetMaxStep(v,self.Dict_MaxStep[v])
        for v in self.Dict_MaxTime.keys():
            G4Eng._ctrl.geometryMenu.SetMaxTime(v,self.Dict_MaxTime[v])


    def add_MaxStepInVolumes(self,vol_name,max_step):
        """ Adds volumes in which the MaxStep is limited to a given value
        """
        self.Dict_MaxStep[vol_name]=max_step
        G4Eng.log.debug(' DetFacility:add_MaxStepInVolumes'+\
          ' adding to '+ vol_name +' maxstep = '+str(max_step))


    def add_MaxTimeInVolumes(self,vol_name,max_time):
        """ Adds volumes in which the MaxTime is limited to a given value
        """
        self.Dict_MaxTime[vol_name]=max_time
        G4Eng.log.debug(' DetFacility:add_MaxTimeInVolumes'+\
          ' adding to '+ vol_name +' maxtime = '+str(max_time))


    def add_PhysicsReg(self,region_obj):
        """ Adds physics regions (PhysicsReg) to volumes in the DetFacility.

             We check that the name of the region was not taken by somebody else
            before. You should care about the volumes included in each region.
        """
        from ISF_Geant4Config.PhysicsReg import PhysicsReg
        if isinstance(region_obj,PhysicsReg):
            if region_obj.Name not in self.Dict_Regions and \
                    region_obj.Name not in G4Eng.Dict_Regions.get('fastSimModels') and \
                    region_obj.Name not in G4Eng.Dict_Regions.get('physicsReg'):
                self.Dict_Regions[region_obj.Name] = region_obj
                if self.Name in G4Eng.Dict_DetFacility:
                    ## If the DetFacility is already in G4Eng, we add the region immediately
                    G4Eng.Dict_Regions.get('physicsReg').update({region_obj.Name : region_obj})
                G4Eng.log.debug(' DetFacility: add_PhysicsReg: adding to %s the PhysRegion %s' % (self.Name, region_obj.Name))
            elif region_obj.Name in G4Eng.Dict_Regions.get('fastSimModels') or \
                 region_obj.Name in G4Eng.Dict_Regions.get('physicsReg'):
                G4Eng.log.warning(' DetFacility: add_PhysicsReg: %s is already used by somebody else -- nothing done' % region_obj.Name)
            else:
                G4Eng.log.warning(' DetFacility: add_PhysicsReg: the %s'++' model already has the %s region' % (self.Name, region_obj.Name))
        else:
            G4Eng.log.error(' FastSimModel: add_PhysicsReg: This is not a PhysicsReg object!!!')


    def add_SenDetector(self,lib,sdname,name,volume):
        """ Association of a sensitive detector (SenDetector) to a
            given volume.

           lib    = library that contains the sensitive
                    detector "sdname".
           name   = local name for "sdname".
           volume = volume to which "sdname" is associated.
        """
        if name in G4Eng.Dict_SenDetector:
            if name not in self.Dict_SenDetector:
                self.Dict_SenDetector[name] = G4Eng.Dict_SenDetector.get(name)
                G4Eng.log.debug(' DetFacility:'+\
                'add_SenDetector: adding to '+self.Name +' the SD '+\
                name+' into the volume '+volume)
            else:
                dfsd = self.Dict_SenDetector.get(name)
                if volume not in dfsd.List_Volumes:
                    dfsd.add_Volume(volume)
                    G4Eng.log.debug(' DetFacility:'+\
                    'add_SenDetector: adding to '+self.Name +\
                    ' the existing SD '+name+' into the volume '+volume)
        else:
            sd=SenDetector(lib,sdname,name,volume,self._AllowMods)
            G4Eng.Dict_SenDetector[name]=sd
            self.Dict_SenDetector[name]=sd
            G4Eng.log.debug(' DetFacility:'+\
            'add_SenDetector: adding to '+self.Name +\
            ' the new SD '+name+' into the volume '+volume)


    def list_DetFacility(self):
        """ Prints the detector facilities asssociated with a given
            detector facility as daugthers.
        """
        for i in self.Dict_DetFacility.values():
           print i.Name


    def listrecursive_DetFacility(self,space=None):
        """ Prints the recursive list of detector facilities
            associated with this detector facility.
        """
        if space is None:
            space = '.'
        for i in self.Dict_DetFacility.values():
            print space, i.Name
            if i.Dict_DetFacility.values():
                DetFacility.listrecursive_DetFacility(i, space+'.')
            else:
                'No more daughters'



class DetFacilityT(DetFacility):
    """ DetFacility is the entry point for a sub-detector's geometry.

        Sets up the geometry of the sub-detector envelope and its contains.
       The available types are:
        - Atlas envelopes: AtlasDetector, BeamPipe, InnerDetector
                           Calorimeter, Muon, MuonSystemEntryLayer
        - Generic (resizable) envelopes: ResizeableBox, ResizeableTubs,
                                         GenericPCON, GenericPGON
        - GeoDetectors from the GeoModel interface.
        - Other detector descriptions loading the appropiate library

       DetFacility can be positioned within other DetFacilities, moved,
       rotated... And they must be added to the simulation engine:
         G4Eng.add_DetFacility(self, det_obj, det_obj_father)
    """

    def __init__(self, type_name, name):
        self.Description = 'This is a detector facility of type "%s"; more information from the user follows here:\n' % type_name
        self.Dict_DetFacility = dict()
        self.Dict_Regions = dict()
        self.Dict_SenDetector = dict()
        self.Father = ''
        self._Built = False
        self._BuiltAssoc = False
        self.Name = name
        self.Type = type_name


    #@classmethod
    def _build(self):
        self.df = G4Eng._ctrl.geometryMenu.GetDetectorFacility(self.Type, self.Name)
        self._Built = True
        G4Eng.log.debug('DetFacilityT: _build: init geometry of the DetFacility')



class DetConfigurator:
    """ DetConfigurator is a hook for the specific sub-detector configuration.

         It provides access to the C++ classes which configure a given
        sub-detector and that have been exported to the Python layer
        using the LCG dictionaries. The corresponding dictionary is
        loaded and the  corresponding python "DetConfigurator" object
        is kept in the:
                AtlasG4Eng.G4Eng.Dict_DetConfig
        Python dictionary.
         In this way the detector configuration is done by the simulation
        core infrastructure following the user requirements, in terms of
        detector flags ("DetFlags") and simulation flags ("simFlags") but
        the end-user can access to the same object and modify the
        configuration.
         Of course the end-user modification will be taken in account
        depending on the time in which is done and in relation with the
        different initialization phases of the G4Eng.
    """

    def __init__(self,name,lcgdict_name,mode=''):
        """
          name = name that will appear in AtlasG4Eng.G4Eng.Dict_DetConfig
                 as a key.
          lcgdict_name = name of the LCG dictionary
          mode = (optional) string to be use in order to select diferent
                "modes" at the time the DetConfigurator hook is built. The
                 LCG dictionary provided by the sub-detector can be generic
                 and only a set of configuration classes make sense in a
                 given simulation job.
        """
        self.Name = name
        self.LCGDictionaryName = lcgdict_name
        self.Mode = mode
        self._Built = False
        G4Eng.load_Dict(self.LCGDictionaryName)
        if self.Name not in G4Eng.Dict_DetConfig:
            G4Eng.Dict_DetConfig[self.Name] = self
            self.build(self.Mode)
            self._Built = True
        else:
            G4Eng.log.info(' DetConfigurator: with name '+self.Name+ ' is already in the Dict_DetConfig: please use it!')


    def build(self,mode):
        """ You should re_write this method to decide which C++ configuration
            classes you want to make available to the users
        """
        G4Eng.log.warning(' DetConfigurator:_build: nothing is done!!!, you have to overwrite it!')

