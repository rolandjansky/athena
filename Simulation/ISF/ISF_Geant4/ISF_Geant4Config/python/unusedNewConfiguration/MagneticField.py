# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, os.path, string, sys
from time import time
from GaudiPython import PyAlgorithm
from AthenaCommon import Constants, Logging, SystemOfUnits
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
from ISF_Geant4Config.iGeant4 import G4Eng
import PyCintex

class MagneticField:
    """ Magnetic field.

    The Python object "MagneticField". It can be 'Constant' or
    magnetic field map 'MapField' type (by default 'Constant'),
    unless the last argument will be set to 'MapField'.
    """

    # TODO: change signature so that lib can have a default value (i.e. swap the order of lib & name)
    # TODO: reinstate stepper="StepperDispatcher"):
    def __init__(self, lib, name, volume="", typefield="Constant", stepper="AtlasRK4"):
        """\
        lib    = library (normally 'G4Field')
        name   = name of the field map
        volume = Geant4 volume with the syntax: 'name::name' For the Mapfield is not needed and it MUST be an empty string.
        typefield = Constant | MapField
        """
        self._Built = False
        self.List_Volumes = list()
        if volume:
            self.add_Volume(volume)
        self.Name = name
        self.Library = lib
        self.Type = typefield
        if typefield == 'Constant':
            self.Bx = 0.0
            self.By = 0.0
            self.Bz = 0.0
        elif typefield == 'MapField':
            self.DeltaIntersection = dict()
            self.DeltaOneStep = dict()
            self.MaximumEpsilonStep = dict()
            self.MinimumEpsilonStep = dict()
        self.FieldStepper = stepper


    def add_Volume(self, vol):
        """\
        Adds more volumes to the List_Volumes in which the
        MagneticField will be applied.The syntax for the
        volume names is: 'name::name'.
        """
        self.List_Volumes.append(vol)


    def set_FieldMapFileName(self, fieldmapfilename):
        """\
        Sets the name of the file that contains the field map.
        """
        if self.Type == 'MapField':
            self.MapFieldFileName = fieldmapfilename


    def set_G4FieldTrackParameters(self, param_name, volume, value):
        """\
        The possible parameters are:
        - DeltaIntersection
        - DeltaOneStep
        - MaximumEpsilonStep
        - MinimumEpsilonStep
        """
        if volume in self.List_Volumes:
            if param_name == 'DeltaIntersection':
                self.DeltaIntersection[volume] = value
            elif param_name == 'DeltaOneStep':
                self.DeltaOneStep[volume] = value
            elif param_name == 'MaximumEpsilonStep':
                self.MaximumEpsilonStep[volume] = value
            elif param_name == 'MinimumEpsilonStep':
                self.MinimumEpsilonStep[volume] = value
            else:
                raise ValueError(' PyG4Atlas.MagneticField.set_G4FieldTrackParameters: invalid parameter')
        else:
            G4Eng.log.debug('PyG4Atlas.MagneticField.set_G4FieldTrackParameters: ' +
                                          'the volume is not in the list of the MagneticField volumes')


    def _build(self):
        if self.Library not in G4Eng.List_LoadedLib:
            G4Eng.load_Lib(self.Library)
        if self.Type == 'Constant':
            if self.Library+'Dict' not in G4Eng.List_LoadedDict:
                G4Eng.load_Dict(self.Library+'Dict')
            CtFieldHandler = G4Eng.gbl.ConstantFieldHandler()
            for volname in self.List_Volumes:
                G4Eng._ctrl.fldMenu.assign('ConstantField', self.Name, volname)
                G4Eng.log.debug(' MagneticField:_build: Constant field ' + self.Name +
                                              ' with components (%f, %f, %f)' % (self.Bx, self.By, self.Bz) +
                                              ' applied to volume ' + volname)
            CtFieldHandler.setFieldValues(self.Name, self.Bx, self.By, self.Bz)
            self._Built = True
        elif self.Type == 'MapField':
            G4Eng._ctrl.fldMenu.select(self.Name)
            G4Eng._ctrl.fldMenu.fieldmap(self.MapFieldFileName)
            for volname in self.List_Volumes:
                G4Eng._ctrl.fldMenu.assign(self.Name, self.Name, volname)
            for volname in self.DeltaIntersection.keys():
                G4Eng._ctrl.fldMenu.setDeltaIntersection(volname, self.DeltaIntersection.get(volname))
            for volname in self.DeltaOneStep.keys():
                G4Eng._ctrl.fldMenu.setDeltaOneStep(volname, self.DeltaOneStep.get(volname))
            for volname in self.MaximumEpsilonStep.keys():
                G4Eng._ctrl.fldMenu.setMaximumEpsilonStep(volname, self.MaximumEpsilonStep.get(volname))
            for volname in self.MinimumEpsilonStep.keys():
                G4Eng._ctrl.fldMenu.setMinimumEpsilonStep(volname, self.MinimumEpsilonStep.get(volname))
            G4Eng.log.debug(' MagneticField::set_G4FieldTrackParameters: set values')
            G4Eng._ctrl.fldMenu.initialize()
            G4Eng.log.debug(' MagneticField:_build: MapField ' + self.Name)

        ## Enable default stepper
        stepper = self.FieldStepper
        G4Eng.log.debug(' MagneticField:_build: Setting stepper = %s' % stepper)
        G4Eng._ctrl.fldMenu.UseStepper(stepper)

        self._Built = True
        G4Eng._app_profiler('  _build: MagneticField ' + self.Name)
