# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Geant4 simulation Python facilities for the ATLAS experiment.

- General-purpose Python classes & functions for the Atlas G4 simulation.
- The attributes, methods and functions which start with "_" are private
  and intended only for internal use.

"""

__author__ = 'A. Dell`Acqua, M. Gallas, A. Buckley'

import os, os.path, string, sys
from time import time
from GaudiPython import PyAlgorithm
from AthenaCommon import Constants, Logging, SystemOfUnits
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
import PyCintex


class FastSimModel:
    """ FastSimulation model.

    This object contains the name of the FastSimulation model,
    and the C++ lib which is needed to load, together with the
    regions in which it will be applied. These regions (class
    PhysicsReg) must be added after the instantiation using the
    method add_Region.

    The FastSimulation object becomes active if it is added to
    the simulation engine.

    The regions associated to a given FastSimulation model object
    are automatically added to the G4AtlasEngine at the time the
    FastSimulation model is added to the G4AtlasEngine (except if
    the model is already associated to G4Eng, in this case the
    region is added inmediatelly to the G4Eng).

    The construction of the regions here described is automatically
    done at the time the FastSimModels are initialized
    """

    def __init__(self,lib,modelname):
        self._Built=False
        self.Name=modelname
        self.Library=lib
        self.Dict_Regions = dict()


    def add_Region(self,region_obj):
        """ Adds more region objects to the Dict_Regions in which the
        FastSimModel will be applied.
        """
        if isinstance(region_obj,PhysicsReg):
            if region_obj.Name not in self.Dict_Regions and \
                    region_obj.Name not in G4AtlasEngine.Dict_Regions.get('fastSimModels') and \
                    region_obj.Name not in G4AtlasEngine.Dict_Regions.get('physicsReg'):
                self.Dict_Regions[region_obj.Name] = region_obj
                ## We associated to the G4Eng at the time we add the FastSimModel, except if the
                ## FastSimodel is already in G4Eng
                if self.Name in G4AtlasEngine.Dict_FastSimModel:
                    G4AtlasEngine.Dict_Regions.get('fastSimModels').update({region_obj.Name : region_obj})
                G4AtlasEngine.log.debug(' FastSimModel: adding the region '+region_obj.Name+' to the FastSimModel' + self.Name + ' from lib '+self.Library)
            elif region_obj.Name in self.Dict_Regions:
                G4AtlasEngine.log.warning(' FastSimModel: add_PhysicsReg: the '+self.Name+' model already has the '+ region_obj.Name+' region')
            elif region_obj.Name in G4AtlasEngine.Dict_Regions.get('fastSimModels') or \
                    region_obj.Name in G4AtlasEngine.Dict_Regions.get('physicsReg'):
                G4AtlasEngine.log.warning(' FastSimModel: add_PhysicsReg: the '+ region_obj.Name+' is already used by somebody else, nothing done ')
        else:
            G4AtlasEngine.log.error(' FastSimModel: add_PhysicsReg: This is not a PhysicsReg object!!!')


    def _build(self):
        pass




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
            G4AtlasEngine.log.debug('PyG4Atlas.MagneticField.set_G4FieldTrackParameters: ' +
                                          'the volume is not in the list of the MagneticField volumes')


    def _build(self):
        if self.Library not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine.load_Lib(self.Library)
        if self.Type == 'Constant':
            if self.Library+'Dict' not in G4AtlasEngine.List_LoadedDict:
                G4AtlasEngine.load_Dict(self.Library+'Dict')
            CtFieldHandler = G4AtlasEngine.gbl.ConstantFieldHandler()
            for volname in self.List_Volumes:
                G4AtlasEngine._ctrl.fldMenu.assign('ConstantField', self.Name, volname)
                G4AtlasEngine.log.debug(' MagneticField:_build: Constant field ' + self.Name +
                                              ' with components (%f, %f, %f)' % (self.Bx, self.By, self.Bz) +
                                              ' applied to volume ' + volname)
            CtFieldHandler.setFieldValues(self.Name, self.Bx, self.By, self.Bz)
            self._Built = True
        elif self.Type == 'MapField':
            G4AtlasEngine._ctrl.fldMenu.select(self.Name)
            G4AtlasEngine._ctrl.fldMenu.fieldmap(self.MapFieldFileName)
            for volname in self.List_Volumes:
                G4AtlasEngine._ctrl.fldMenu.assign(self.Name, self.Name, volname)
            for volname in self.DeltaIntersection.keys():
                G4AtlasEngine._ctrl.fldMenu.setDeltaIntersection(volname, self.DeltaIntersection.get(volname))
            for volname in self.DeltaOneStep.keys():
                G4AtlasEngine._ctrl.fldMenu.setDeltaOneStep(volname, self.DeltaOneStep.get(volname))
            for volname in self.MaximumEpsilonStep.keys():
                G4AtlasEngine._ctrl.fldMenu.setMaximumEpsilonStep(volname, self.MaximumEpsilonStep.get(volname))
            for volname in self.MinimumEpsilonStep.keys():
                G4AtlasEngine._ctrl.fldMenu.setMinimumEpsilonStep(volname, self.MinimumEpsilonStep.get(volname))
            G4AtlasEngine.log.debug(' MagneticField::set_G4FieldTrackParameters: set values')
            G4AtlasEngine._ctrl.fldMenu.initialize()
            G4AtlasEngine.log.debug(' MagneticField:_build: MapField ' + self.Name)

        ## Enable default stepper
        stepper = self.FieldStepper
        G4AtlasEngine.log.debug(' MagneticField:_build: Setting stepper = %s' % stepper)
        G4AtlasEngine._ctrl.fldMenu.UseStepper(stepper)

        self._Built = True
        G4AtlasEngine._app_profiler('  _build: MagneticField ' + self.Name)




class FieldIntegrationParameters:
    """ Field integration parameters.

       The Python object "FieldIntegrationParameters".
    """
    def __init__(self,region):
        """
            name   = name of the field integration parameters object
             based on the region from which it is constructed
        """
        self._Built = False
        self.List_Regions = []
        if region:
            self.List_Regions.append(region)
        # TODO: Calc name dynamically by catting sorted regions list
        self.Name = '%s_Parameters' % region
        self.KineticEnergyThreshold = -1
        self.MaxLengthForConstField = dict()
        self.MissDistance = dict()
        self.TypicalLongStep = dict()
        self.TolerableBiasError = dict()
        self.TolerableIntegrationError = dict()
        self.ExpectedNumBoundaries = dict()
        self.ExpectedTrackLength = dict()

    def add_Region(self, reg):
        """  Adds more regions to the List_Regions in which the
            FieldIntegrationParameters will be applied.
        """
        if reg:
            self.List_Regions.append(reg)

    def set_KineticEnergyThreshold(self,val):
        """ Sets kinetic energy threshold for the region.  Below this threshold
            electrons and positrons will be treated differently
        """
        self.KineticEnergyThreshold = val

    def show_Parameters(self):
        """ Shows the current values for the parameters of the object
        """
        print 'FieldIntegrationParameters with name '+self.Name+' parameters:'
        if self.KineticEnergyThreshold >= 0:
            print 'KineticEnergyThreshold : %f MeV' % self.KineticEnergyThreshold
        else : print 'KineticEnergyThreshold : default'
        print 'For the following parameters:'
        print '   idx=0 for e+/e- track below E-threshold'
        print '   idx=1 for e+/e- track above E-threshold and all other charged track except mu+/mu- of any kinetic energy'
        print '   idx=2 for mu+/mu- track of any kinetic energy'
        print 'Any items or entries not listed will take the default values'
        if len(self.MaxLengthForConstField)>0 : print 'MaxLengthForConstField :',self.MaxLengthForConstField
        if len(self.MissDistance)>0 : print 'MissDistance :',self.MissDistance
        if len(self.TypicalLongStep)>0 : print 'TypicalLongStep :',self.TypicalLongStep
        if len(self.TolerableBiasError)>0 : print 'TolerableBiasError :',self.TolerableBiasError
        if len(self.TolerableIntegrationError)>0 : print 'TolerableIntegrationError :',self.TolerableIntegrationError
        if len(self.ExpectedNumBoundaries)>0 : print 'ExpectedNumBoundaries :',self.ExpectedNumBoundaries
        if len(self.ExpectedTrackLength)>0 : print 'ExpectedTrackLength :',self.ExpectedTrackLength
        print '\n\n'

    def set_Parameter(self,param_name,index,val):
        """ Set the value of one of the field integration parameters.  The
            possible parameter names are:
            - MaxLengthForConstField : maximum length of a step for which
               the magnetic field will be considered to be constant
            - MissDistance : Allowable distance for missing a volume
            - TypicalLongStep : Length of a long step in the region
            - TolerableBiasError : The allowable bias that can accumulate
               during stepping
            - TolerableIntegrationError : The allowable error on integration
               during stepping
            - ExpectedNumBoundaries : The expected number of boundaries that
               will be crossed by a track traversing the region
            - ExpectedTrackLength : The expected track length for a track
               traversing the region

            Index should be 0, 1, or 2 :
            - idx=0 for e+/e- track below E-threshold
            - idx=1 for e+/e- track above E-threshold
               and all other charged track except mu+/mu-
               of any kinetic energy
            - idx=2 for mu+/mu- track of any kinetic energy
        """
        if (int(index) == 0 or int(index) == 1 or int(index) == 2):
            if(param_name=='MaxLengthForConstField'):
                self.MaxLengthForConstField[int(index)]=val
            elif(param_name=='MissDistance'):
                self.MissDistance[int(index)]=val
            elif(param_name=='TypicalLongStep'):
                self.TypicalLongStep[int(index)]=val
            elif(param_name=='TolerableBiasError'):
                self.TolerableBiasError[int(index)]=val
            elif(param_name=='TolerableIntegrationError'):
                self.TolerableIntegrationError[int(index)]=val
            elif(param_name=='ExpectedNumBoundaries'):
                self.ExpectedNumBoundaries[int(index)]=val
            elif(param_name=='ExpectedTrackLength'):
                self.ExpectedTrackLength[int(index)]=val
            else:
                raise ValueError,(' PyG4Atlas.FieldIntegrationParameters::'+\
                'set_Parameter: invalid parameter '+param_name)
        else:
            G4AtlasEngine.log.debug(' FieldIntegrationParameters::'+\
              'set_Parameter the index is not one of 0, 1, or 2 - '+index)

    def _build(self):
        if not self._Built:
            for reg in self.List_Regions :
                if self.KineticEnergyThreshold >= 0:
                    G4AtlasEngine._ctrl.fldIntMenu.Set_kineticEnergyThreshold( reg , self.KineticEnergyThreshold )
                for idx in self.MaxLengthForConstField.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_maxLengthForConstField( reg , idx , self.MaxLengthForConstField[idx] )
                for idx in self.MissDistance.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_missDistance( reg , idx , self.MissDistance[idx] )
                for idx in self.TypicalLongStep.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_typicalLongStep( reg , idx , self.TypicalLongStep[idx] )
                for idx in self.TolerableBiasError.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_tolerableBiasError( reg , idx , self.TolerableBiasError[idx] )
                for idx in self.TolerableIntegrationError.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_tolerableIntegrationError( reg , idx , self.TolerableIntegrationError[idx] )
                for idx in self.ExpectedNumBoundaries.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_expectedNumBoundaries( reg , idx , self.ExpectedNumBoundaries[idx] )
                for idx in self.ExpectedTrackLength.keys() :
                    G4AtlasEngine._ctrl.fldIntMenu.Set_expectedTrackLength( reg , idx , self.ExpectedTrackLength[idx] )
            G4AtlasEngine.log.debug(' FieldIntegrationParameters:_build: FieldIntegrationParameters '+self.Name)
            self._Built = True
        G4AtlasEngine._app_profiler('  _build: FieldIntegrationParameters '+self.Name)



class MCTruthStrg:
    """ MCTruth strategy.

        Different MCTruth strategies can be defined and applied to
       several volumes (typically to the DetFacilities that are the
       envelopes of the sub-detectors).

       The G4AtlasEngine.menu_MCTruth menu is the way to deal with
       the MCTruthStrg.
    """
    def __init__(self,G4TruthStrgLib,NameStrg,volume_name,volume_level):
        """
           volume_level = position of the volume in the G4 hierarchy.
        """
        self._Built=False
        self.Lib=G4TruthStrgLib
        self.Name=NameStrg
        self.Dict_Volumes = dict()
        self.Dict_Volumes[volume_name]=volume_level


    def add_Volumes(self,vol_name, vol_level):
        """ Adds volumes to the list of volumes in which the strategy
            will be used.

        """
        if vol_name not in self.Dict_Volumes:
            self.Dict_Volumes[vol_name] = vol_level
        else:
            G4AtlasEngine.log.warning(' MCTruthStrg: add_Volumes: The the volume '+vol_name+'is already in the list')


    def _construct(self):
        if self.Lib not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine._ctrl.load(self.Lib)
            G4AtlasEngine.List_LoadedLib.append(self.Lib)
        if not(self._Built):
           for i in self.Dict_Volumes.keys():
               G4AtlasEngine._ctrl.mctruthMenu.activateStrategy(self.Name, i, self.Dict_Volumes.get(i))
               G4AtlasEngine.log.debug(' MCTruthStrg:'+
                                             '_construct: '+self.Name +' and apply it to volume '+i)
           self._Built=True
           G4AtlasEngine._app_profiler('  _build MCTruthStrg: '+self.Name)



class RecEnvelope:
    """ Recording envelope.


    """

    def __init__(self, name, volume_name, volume_level,allowMods=False):
        self._Built = False
        self.Name = name
        self.Dict_Volumes = dict()
        self.Dict_Volumes[volume_name] = volume_level
        self._AllowMods=allowMods
        
    def _construct(self):
        if not self._Built:
           for i in self.Dict_Volumes.keys():
               G4AtlasEngine._ctrl.mctruthMenu.addRecordingEnvelope(i, self.Dict_Volumes.get(i), self.Name)
               G4AtlasEngine._ctrl.sendetectorMenu.setAllowMods(self.Name,self._AllowMods)
               
               G4AtlasEngine.log.debug(' RecEnvelope:'+
                                             '_construct: '+self.Name+' and store at the exit of volume  '+i)
           self._Built = True
           G4AtlasEngine._app_profiler('  _build RecEnvelope: '+self.Name)



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
        if self.Library not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine.load_Lib(self.Library)
        G4AtlasEngine._ctrl.sendetectorMenu.useSensitiveDetector(self.SenDetectorFacility, self.Name,
                                                                 self.AllowMods)
        for v_n in self.List_Volumes:
            G4AtlasEngine._ctrl.geometryMenu.AssignSD(v_n, self.Name)
            G4AtlasEngine.log.debug('SenDetector:_build: %s associated to the volume %s' % (self.Name, v_n))
        self._Built=True
        G4AtlasEngine._app_profiler('_build SenDetector: %s' % self.Name)
        if (len(self.List_Volumes)==0):
            G4AtlasEngine.log.warning(' SenDetector:_build: %s is not associated to any volume' % self.Name)



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


    # TODO: Where is this called?!
    def _construct(self):
        if "G4PhysicsLists" not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine._ctrl.load("G4PhysicsLists")
            G4AtlasEngine.List_LoadedLib.append("G4PhysicsLists")
        G4AtlasEngine._ctrl.physicsMenu.UsePhysicsList(self.Name)
        G4AtlasEngine._ctrl.physicsMenu.SetGeneralCut(self.Value_gen_cut)
        if not self._Built:
            G4AtlasEngine._ctrl.physicsMenu.ConstructPhysicsList()
            G4AtlasEngine.log.debug('PhysicsList:_construct: %s with a general cut of %e mm' % (self.Name, self.Value_gen_cut))
            self._Built = True



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
           G4AtlasEngine.init_G4()
        """
        self._pr = G4AtlasEngine._ctrl.physicsMenu.CreateNewRegion(self.Name)
        for i in self.Dict_Cuts.keys():
            self._pr.setCut(self.Dict_Cuts.get(i), i)
            G4AtlasEngine.log.debug(' PhysicsReg:' +
                                    '_construct_Region: %s' % self.Name +
                                    ' with cut %s=%s ' % (i, str(self.Dict_Cuts.get(i))) )
        for i in self.List_Volumes:
            G4AtlasEngine.log.debug(' PhysicsReg:' +
                                    '_construct_Region: %s and added volume %s' % (self.Name, i))
            self._pr.addVolume(i)
        self._Built = True
        G4AtlasEngine._app_profiler('_construct_Region: %s' % self.Name)


    def add_Cuts(self, name_particle, cut_value):
        """
        Adds cuts for different particles.

        Cuts are stored in .Dict_Cuts
        """
        if name_particle not in self.Dict_Cuts:
            self.Dict_Cuts[name_particle] = cut_value
        else:
            G4AtlasEngine.log.warning('PhysicsReg:add_Cuts: Cut value for %s redefined to %s' % (name_particle, str(cut_value)))


    def add_Volumes(self, name_vol):
        """
        Adds volumes to the region.

        Volumes are stored in List_Volumes.
        """
        if name_vol not in self.List_Volumes:
            self.List_Volumes.append(name_vol)
        else:
            G4AtlasEngine.log.warning('PhysicsReg:add_Volumes: ' +
                                      'The region %s already exists.' % name_vol)




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
        engine G4AtlasEngine.
        """
        self._Built = False
        self.Lib = userActionLib
        self.Name = actionName
        self.ListStatus = listStatus
        self.Properties = dict()


    def set_Properties(self, propDict):
        self.Properties = propDict


    def _construct(self):
        if self.Lib not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine._ctrl.load(self.Lib)
            G4AtlasEngine.List_LoadedLib.append(self.Lib)
        for i in self.ListStatus:
            G4AtlasEngine._ctrl.actMenu.actionRegister(self.Name,i)
            G4AtlasEngine.log.debug('UserAction: constructing user action %s and applying it to: %s' % (self.Name, i))
        for k in self.Properties.keys():
            G4AtlasEngine._ctrl.actMenu.actionProperty(self.Name, k, self.Properties[k])
        self._Built = True
        G4AtlasEngine._app_profiler('_construct UserAction: %s' % self.Name)




class SimSkeleton(object):
    """ Skeleton for a simulation entity.

       This class defines a skeleton for a simulation entity. The different
        do_something  methods will be automatically executed at the method
       initialize() of the PyG4AtlasAlg (Python Algorithm) which steers all
       the simulation jobs. You can re-write the do_something methods here
       defined in order to start a simulation entity from scracth or to
       modify in deep one of the existing simulation entities (see the user
       manual, or ask for more help in how to use this class)

       Note: all the do_something methods you re-write must be declared as
             classmethod. Apart from the existing do_something methods here
             defined you can define your own ones.
    """

    def __init__(self):
        """
        The skeleton will be in the G4AtlasEng.G4Eng.Dict() and it will be
        accessible at any point.
        """
        if 'simu_skeleton' not in G4AtlasEngine.Dict:
            G4AtlasEngine.Dict['simu_skeleton'] = self
        else:
            G4AtlasEngine.log.warning('SimSkeleton.__init__: the simulation has already a skeleton: '
                                      'you can find it in the G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(cls):
        """ Place to handle JobProperties.
        """
        G4AtlasEngine.log.info('SimSkeleton._do_jobproperties ::  nothing done')


    @classmethod
    def _do_external(cls):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        G4AtlasEngine.log.info('SimSkeleton._do_external ::  nothing done')


    @classmethod
    def _do_persistency(cls):
        """ Place to handle the persistency.
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.PoolHitsOutput.statusOn:
            ## Write hits in POOL
            G4AtlasEngine.log.verbose('SimSkeleton._do_persistency :: starting')

            ## The following used to be in G4AtlasApps/HitAthenaPoolWriteOptions
            from AthenaCommon.DetFlags import DetFlags
            from AthenaCommon.Configurable import Configurable
            from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

            ## Default setting for one output stream
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN = '100'"]
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += ["DEFAULT_BUFFERSIZE = '2048'"]

            ## Instantiate StreamHITS
            ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
            stream1 = None
            if athenaCommonFlags.PoolHitsOutput.statusOn:
                stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput())

                ## Write geometry tag info
                include("EventInfoMgt/EventInfoMgt_jobOptions.py")

                ## EventInfo & TruthEvent always written by default
                stream1.ForceRead=True
                stream1.ItemList = ["EventInfo#*",
                                    "McEventCollection#TruthEvent",
                                    "JetCollection#*"]

                ## Make stream aware of aborted events
                stream1.AcceptAlgs = ["G4AtlasAlg"]

                ## Detectors

                ## Inner Detector
                if DetFlags.ID_on():
                    stream1.ItemList += ["SiHitCollection#*",
                                         "TRTUncompressedHitCollection#*",
                                         "TrackRecordCollection#CaloEntryLayer"]
                ## Calo
                if DetFlags.Calo_on():
                    stream1.ItemList += ["CaloCalibrationHitContainer#*",
                                         "LArHitContainer#*",
                                         "TileHitVector#*",
                                         "SimpleScintillatorHitCollection#*",
                                         "TrackRecordCollection#MuonEntryLayer"]
                ## Muon
                if DetFlags.Muon_on():
                    stream1.ItemList += ["RPCSimHitCollection#*",
                                         "TGCSimHitCollection#*",
                                         "CSCSimHitCollection#*",
                                         "MDTSimHitCollection#*",
                                         "TrackRecordCollection#MuonExitLayer"]
                    if hasattr(simFlags, 'SimulateNewSmallWheel'):
                        if simFlags.SimulateNewSmallWheel():
                            stream1.ItemList += ["GenericMuonSimHitCollection#*"]
                ## Lucid
                if DetFlags.Lucid_on():
                    stream1.ItemList += ["LUCID_SimHitCollection#*"]
                ## ZDC
                if DetFlags.ZDC_on():
                    stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                         "ZDC_SimStripHit_Collection#*"]
                ## ALFA
                if DetFlags.ALFA_on():
                    stream1.ItemList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*"]
                ## Ancillary scintillators
                stream1.ItemList += ["ScintillatorHitCollection#*"]

                ## Add cosmics and test beam configuration hit persistency if required cf. geom tag
                layout = simFlags.SimLayout.get_Value()
                if "tb" not in layout:
                    from AthenaCommon.BeamFlags import jobproperties
                    if jobproperties.Beam.beamType() == 'cosmics' or \
                            (hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn) or \
                            (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
                        stream1.ItemList += ["TrackRecordCollection#CosmicRecord", "TrackRecordCollection#CosmicPerigee"]
                else:
                    ## CTB-specific
                    if layout.startswith("ctb"):
                        if simFlags.LArFarUpstreamMaterial.statusOn and simFlags.LArFarUpstreamMaterial.get_Value():
                            stream1.ItemList.append("TrackRecordCollection#LArFarUpstreamMaterialExitLayer")
                    ## Persistency of test-beam layout
                    if layout.startswith('ctb') or layout.startswith('tb_Tile2000_'):
                        stream1.ItemList += ["TBElementContainer#*"]

            ## AthenaPool converters
            # TODO: Remove? Indentation?
            include("EventAthenaPool/EventAthenaPool_joboptions.py")
            include("G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py")
            theApp.Dlls += [ "GeneratorObjectsAthenaPoolPoolCnv"]

            ## StreamEVGEN: needed for cosmic simulations and cavern BG
            ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
            # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
            layout = simFlags.SimLayout.get_Value()
            if "tb" not in layout:
                if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
                    stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.WriteTR.get_Value())
                    if simFlags.CavernBG.statusOn and 'Write' in simFlags.CavernBG.get_Value():
                        stream2.ItemList += ["TrackRecordCollection#NeutronBG"]
                    else:
                        stream2.ItemList += ["TrackRecordCollection#CosmicRecord"]
                    stream2.AcceptAlgs = ["G4AtlasAlg"]
                if hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                    stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.StoppedParticleFile.get_Value())
                    stream2.ItemList += ["IOVMetaDataContainer#*"]
                    stream2.ItemList += ["TrackRecordCollection#StoppingPositions"]
                    stream2.AcceptAlgs = ["G4AtlasAlg"]

            G4AtlasEngine.log.verbose('SimSkeleton._do_persistency :: done')
        else:
            G4AtlasEngine.log.info('SimSkeleton._do_persistency :: nothing done')


    @classmethod
    def _do_readevgen(cls):
        """ Place to read evgen events.
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: starting')

        myRunNumber = 1
        myFirstLB = 1
        if hasattr(simFlags, "RunNumber") and simFlags.RunNumber.statusOn:
            myRunNumber = simFlags.RunNumber.get_Value()

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        ## ReadTR is only present in simFlags for ATLAS geometries with cosmics switched on
        if hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn:
            include("CosmicGenerator/SetCosmicGenerator.py")

        if athenaCommonFlags.PoolEvgenInput.statusOn:
            ## Tell the event selector about the evgen input files and event skipping
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaPoolCnvSvc.ReadAthenaPool
            svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
            if athenaCommonFlags.SkipEvents.statusOn:
                svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

            ## Read input file metadata
            import PyUtils.AthFile as af
            hitfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
            f = af.fopen(hitfile)

            ## Check that event type is SIMULATION (as it must be)
            if "evt_type" in f.infos.keys():
                evttypes = f.infos["evt_type"]
                evttype0 = str(evttypes[0])
                if not evttype0.startswith("IS_SIMULATION"):
                    msg = "SimSkeleton._do_readevgen :: This input file has incorrect evt_type: %s\n" % str(evttypes)
                    msg += "Please make sure you have set input file metadata correctly - "
                    msg += "consider using the job transforms for earlier steps if you aren't already doing so."
                    G4AtlasEngine.log.error(msg)
                    raise SystemExit("Input file evt_type is incorrect: please check your evgen jobs.")
            else:
                G4AtlasEngine.log.warning("SimSkeleton._do_readevgen :: Could not find 'evt_type' key in athfile.infos. Unable to that check evt_type is correct.")

            ## Get evgen run number and lumi block
            if len(f.run_numbers) > 0:
                myRunNumber = f.run_numbers[0]
                G4AtlasEngine.log.info('SimSkeleton._do_readevgen :: Found run number %d in hits file metadata.', myRunNumber)
            else:
                G4AtlasEngine.log.warning("SimSkeleton._do_readevgen :: Failed to find run number in hits file metadata.")
            if f.lumi_block:
                myFirstLB = f.lumi_block[0]
        else:
            ## No input file so assume that we are running a Generator in the same job
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaCommon.AtlasUnixGeneratorJob
            # TODO: Check that there is at least one algorithm already in the AlgSequence?
            ## Warn if attempting to skip events in a generator job
            if athenaCommonFlags.SkipEvents.statusOn:
                msg = "SimSkeleton._do_readevgen :: athenaCommonFlags.SkipEvents set in a job without an active "
                msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
                G4AtlasEngine.log.warning(msg)


        ## Set event selector details based on evgen metadata
        svcMgr.EventSelector.OverrideRunNumber = True
        svcMgr.EventSelector.OverrideEventNumber = True
        svcMgr.EventSelector.RunNumber = myRunNumber
        svcMgr.EventSelector.FirstLB = myFirstLB
        svcMgr.EventSelector.InitialTimeStamp  = 0

        G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: done')


    @classmethod
    def _do_G4AtlasAlg(cls):
        """ Place to handle the G4AtlasAlg service
        """
        G4AtlasEngine.log.verbose('SimSkeleton._doG4AtlasAlg :: starting')
        from AthenaCommon.AlgSequence import AlgSequence
        job = AlgSequence()
        from G4AtlasAlg.G4AtlasAlgConf import G4AtlasAlg
        job += G4AtlasAlg()

        ## Killing neutrinos
        from G4AtlasApps.SimFlags import simFlags
        sLayout = simFlags.SimLayout()
        if "ATLAS" in sLayout:
            job.G4AtlasAlg.KillAllNeutrinos = True

        if hasattr(simFlags, 'ReleaseGeoModel') and simFlags.ReleaseGeoModel.statusOn:
            ## Save the fast simulation
            job.G4AtlasAlg.ReleaseGeoModel = simFlags.ReleaseGeoModel.get_Value()

        if hasattr(simFlags, 'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn:
            ## Save the fast simulation
            job.G4AtlasAlg.IncludeParentsInG4Event = simFlags.IncludeParentsInG4Event.get_Value()

        ## G4AtlasAlg verbosities (available domains = Navigator, Propagator, Tracking, Stepping, Stacking, Event)
        ## Set stepper verbose = 1 if the Athena logging level is <= DEBUG
        # TODO: Why does it complain that G4AtlasAlgConf.G4AtlasAlg has no "Verbosities" object? Fix.
        #from AthenaCommon.AppMgr import ServiceMgr
        #if ServiceMgr.MessageSvc.OutputLevel <= 2:
        #    print dir(job.G4AtlasAlg)
        #    print job.G4AtlasAlg.Verbosities
        #    print job.G4AtlasAlg.Verbosities["Tracking"]
        #    job.G4AtlasAlg.Verbosities["Tracking"] = 1

        # TODO: Add physics list setting from simFlags to the alg here?

        G4AtlasEngine.log.verbose('SimSkeleton._doG4AtlasAlg :: done')


    @classmethod
    def do_GeoSD(cls):
        """ Place to define the geometry and sensitive detectors.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_GeoSD :: nothing done')


    @classmethod
    def _do_MagField(cls):
        """ Place to define the existing magnetic fields.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_MagField :: nothing done')

    @classmethod
    def _do_FieldIntegrationParameters(cls):
        """ Place to define the settings for default field integration parameters.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_FieldIntegrationParameters :: nothing done')

    @classmethod
    def do_MCtruth(cls):
        """ Place to define the MCTruth strategies.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_MCtruth :: nothing done')


    @classmethod
    def do_Physics(cls):
        """ Place to define the Physics list.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_Physics :: nothing done')


    @classmethod
    def do_UserActions(cls):
        """ Place to define default user actions.

           In most of the cases this will be empty unless a well
         defined user action will be applied in all the jobs for a
         given simulation entity.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_UserActions :: nothing done')


    @classmethod
    def _do_DefaultUserActions(cls):
        """
           Default user actions: G4SimMem, G4SimTimer.

           Do not re-write this method. The memory and time
          checkers must be on always.
        """
        import AtlasG4Eng
        G4AtlasEngine.log.verbose('SimSkeleton._do_DefaultUserActions :: starting')
        from atlas_utilities import G4SimMem, G4SimTimer
        actions = AtlasG4Eng.G4Eng.menu_UserActions()
        actions.add_UserAction(G4SimTimer)
        actions.add_UserAction(G4SimMem)
        G4AtlasEngine.log.verbose('SimSkeleton._do_DefaultUserActions :: done')


    @classmethod
    def _do_metadata(cls):
        """
        Default metadata: empty
        Storage controlled by either AtlasKernel or CTBKernel
        """
        G4AtlasEngine.log.info('SimSkeleton._do_metadata :: nothing done')


    @classmethod
    def __checks(cls):
        """
        Internals.

        Do not re-write this method. Here it is checked if the
        user has defined a world volume and a physics list. If
        this is not the case a default world_volume is created
        and the QGSP_BERT is selected.
        """
        import AtlasG4Eng
        G4AtlasEngine.log.verbose('SimSkeleton.__checks :: starting')
        if 'volume_world' not in G4AtlasEngine.Dict:
             G4AtlasEngine.log.debug('SimSkeleton.__checks :: defined a default world volume 50x50x50 meters')
             worldvolume = DetFacility("ResizeableBox", "WorldVolume")
             worldvolume.df.SetDx(50000.)
             worldvolume.df.SetDy(50000.)
             worldvolume.df.SetDz(50000.)
             AtlasG4Eng.G4Eng.add_DetFacility(worldvolume, worldvolume)
        if 'physics' not in G4AtlasEngine.Dict:
             G4AtlasEngine.log.debug('SimSkeleton.__checks :: defined the default Physics List: QGSP_BERT ')
             physics = PhysicsList('QGSP_BERT', 1)
             AtlasG4Eng.G4Eng.add_PhysicsList(physics)
        G4AtlasEngine.log.verbose('SimSkeleton.__checks :: done')


    @classmethod
    def _do_PreInit(cls):
        """
        Execute all the do_something methods in the phase before Athena initialization.

        Do not overload this method.
        """
        G4AtlasEngine.log.verbose('SimSkeleton._do_PreInit :: starting')
        known_methods = ['_do_jobproperties', '_do_external', '_do_metadata',
                         '_do_readevgen', '_do_persistency', '_do_G4AtlasAlg',
                         '_do_MagField', '_do_FieldIntegrationParameters']
        ## Execute the known methods from the known_methods in pre_init
        for k in known_methods:
            try:
                G4AtlasEngine.log.debug('SimSkeleton._do_PreInit :: evaluating method ' + k)
                getattr(cls, k).__call__()
            except Exception, err:
                print "Error: %s" % str(err)
                import traceback,sys
                traceback.print_exc(file=sys.stdout)
                raise RuntimeError('SimSkeleton._do_PreInit :: found problems with the method  %s' % k)

        ## Run pre-init callbacks
        G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + "preInit")
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.InitFunctions.statusOn and "preInit" in simFlags.InitFunctions.get_Value():
            for callback_fn in simFlags.InitFunctions.get_Value()["preInit"]:
                    callback_fn.__call__()
                    G4AtlasEngine._callback_counter += 1
        G4AtlasEngine.log.verbose('SimSkeleton._do_PreInit :: done')


    @classmethod
    def _do_All(cls):
        """
        Executes all the do_something methods at the Athena initialization time.

        Do not overload this method.
        """
        G4AtlasEngine.log.verbose('SimSkeleton._do_All :: starting')
        known_methods = ['do_GeoSD', 'do_MCtruth', 'do_UserActions', 'do_Physics']
        ## Execute the known methods from the known_methods list
        for k in known_methods:
            try:
                G4AtlasEngine.log.debug('SimSkeleton :: evaluating method ' +k)
                getattr(cls, k).__call__()
            except Exception, err:
                print "Error: %s" % str(err)
                import traceback, sys
                traceback.print_exc(file = sys.stdout)
                raise RuntimeError('SimSkeleton: found problems with the method  %s' % k)
        ## Execute the unknown methods created by user
        for i in dir(cls):
            if i.find('do_') == 0 and i not in known_methods:
               try:
                   G4AtlasEngine.log.debug('SimSkeleton :: evaluating method %s' % i)
                   getattr(cls, i).__call__()
               except Exception, err:
                   print "Error: %s" % str(err)
                   import traceback, sys
                   traceback.print_exc(file=sys.stdout)
                   raise RuntimeError('SimSkeleton: found problems with the method %s' % i)
        cls._do_DefaultUserActions()
        cls.__checks()
        G4AtlasEngine.log.verbose('SimSkeleton._do_All :: done')




## Python algorithm to set up the Python side of the G4 sim
from AthenaPython import PyAthena
class PyG4AtlasAlg(PyAthena.Alg):
    """
    Python algorithm which runs the ATLAS simulation.
    """

    def __init__(self, name="PyG4AtlasAlg", sim_module=None):
        """
        name                  --> must be 'PyG4AtlasAlg'
        sim_module            --> name of a Python module which describes the simulation

        NB. If sim_module is an empty string or None, the sim skeleton contained
        in G4AtlasEng.G4Eng.Dict() will be run.
        """
        super(PyG4AtlasAlg, self).__init__(name=name)
        self.doFirstEventG4SeedsCheck = True
        self.sim_module = sim_module
        self.RndG4Menu = None

        ## If the random number service hasn't been set up already, do it now.
        from G4AtlasApps.SimFlags import simFlags
        simFlags.RandomSeedList.useDefaultSeeds()

        ## Set up the appropriate sim skeleton in the G4 engine
        import AtlasG4Eng
        if "ATLAS" in simFlags.SimLayout():
            from SimAtlasKernel import AtlasSimSkeleton
            AtlasG4Eng.G4Eng.log.info('Configuring ATLAS simulations')
            dummy = AtlasSimSkeleton()
        elif "tb_Tile2000_2003" in simFlags.SimLayout():
            from SimCtbKernel import Tile2000_2003
            AtlasG4Eng.G4Eng.log.info('Configuring old standalone Tile test beams')
            dummy = Tile2000_2003()
        elif "tb_LArH6" in simFlags.SimLayout():
            AtlasG4Eng.G4Eng.log.info('Configuring LAr H6 test beam')
            from SimCtbKernel import LArH6_TB
            dummy = LArH6_TB()
        elif "ctbh8" in simFlags.SimLayout():
            from SimCtbKernel import CtbSim
            AtlasG4Eng.G4Eng.log.info('Configuring CTB H8 (2004) test beam')
            dummy = CtbSim()
        elif "simu_skeleton" not in AtlasG4Eng.G4Eng.Dict:
            AtlasG4Eng.G4Eng.log.error('No sim skeleton registered by time of PyG4AtlasAlg construction: STOP!!')
            raise ValueError('Unknown sim setup: STOP')

        ## Import sim module if requested
        # TODO: is this ever used?
        if self.sim_module:
            G4AtlasEngine.log.info("The kernel simulation Python module which describes the simulation is: %s" % self.sim_module)
            try:
                __import__(self.sim_module, globals(), locals())
            except:
                G4AtlasEngine.log.fatal("The kernel simulation Python module '%s' was not found!" % self.sim_module)
                raise RuntimeError('PyG4Atlas: PyG4AtlasAlg: initialize()')

        ## Call the sim skeleton pre-init method
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_PreInit()

        ## Start profiler
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg before initialize')


    def initialize(self):
        import AtlasG4Eng
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('PyG4AtlasAlg starting at (UTC): %s' % timestr)
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg begin of initialize')
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_All()
        AtlasG4Eng.G4Eng._init_Simulation()
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg end of initialize')
        self.RndG4Menu = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
        from G4AtlasApps.SimFlags import simFlags
        if "atlas_flags" in simFlags.extra_flags:
            beamcondsvc = PyAthena.py_svc('BeamCondSvc/BeamCondSvc', createIf=True, iface=PyCintex.gbl.IBeamCondSvc)
            assert(type(beamcondsvc) ==  PyCintex.gbl.IBeamCondSvc)
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
            theApp.EvtMax = athenaCommonFlags.EvtMax()
        return True


    def execute(self):
        import PyG4Atlas, AtlasG4Eng
        from G4AtlasApps.SimFlags import simFlags

        if self.doFirstEventG4SeedsCheck :
            if simFlags.SeedsG4.statusOn:
                rnd = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
                rnd.set_Seed(simFlags.SeedsG4.get_Value())
                self.doFirstEventG4SeedsCheck = False
                if self.RndG4Menu.SaveStatus:
                    self.RndG4Menu.Menu.saveStatus('G4Seeds.txt')
        return True


    def finalize(self):
        import AtlasG4Eng
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg at finalize  ')
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('PyG4AtlasAlg ending at (UTC): %s' % timestr)
        return True
