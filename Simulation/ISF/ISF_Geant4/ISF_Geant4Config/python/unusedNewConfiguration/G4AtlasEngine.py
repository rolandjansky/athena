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


class G4AtlasEngine:
    """
    Generic simulation engine class.

    Geometry, sensitive detectors, physics lists, physical cuts, etc. must be
    attached to this class.
    """

    _NumInstances = 0

    def __init__(self, name_simu=""):
        """
        Default simulation engine object initialization.

        Only one instance per simulation job.
        """
        if G4AtlasEngine._NumInstances > 0:
            msg = 'There is already an instance of the G4AtlasEngine class -- try to use it!'
            G4AtlasEngine.log.error('G4AtlasEngine: ' + msg)
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: __init__(): Attempted multiple instantiation')

        print 'G4AtlasEngine: setting up G4 engine'

        ## Init dictionaries and lists
        # TODO: Clean up this whole ugly "Dict*" mess
        G4AtlasEngine.Dict = dict()
        G4AtlasEngine.Dict_DetFacility = dict()
        G4AtlasEngine.Dict_DetConfig = dict()
        G4AtlasEngine.Dict['Dict_DetFacility'] = G4AtlasEngine.Dict_DetFacility
        G4AtlasEngine.Dict_Materials = dict()
        #G4AtlasEngine.Dict_Colors = dict()
        G4AtlasEngine.Dict_MCTruthStrg = dict()
        G4AtlasEngine.Dict_MCTruthStrg['SecondarySavingPolicy'] = 'All'
        G4AtlasEngine.Dict_SenDetector = dict()
        G4AtlasEngine._List_SenDetector_dependence = list()
        G4AtlasEngine.Dict_FastSimModel = dict()
        G4AtlasEngine.Dict_Fields = dict()
        G4AtlasEngine.Dict_UserAction = dict()
        G4AtlasEngine.Dict_Regions = dict()
        G4AtlasEngine.Dict_Regions['fastSimModels'] = dict()
        G4AtlasEngine.Dict_Regions['physicsReg'] = dict()
        G4AtlasEngine.Dict_RecEnvelope = dict()
        G4AtlasEngine.Dict_FieldIntegrationParameters = dict()
        G4AtlasEngine.Dict_SpecialConfiguration = dict()

        ## Web doc links
        G4AtlasEngine.Dict_WebLinks = {
            'maindoc': 'https://twiki.cern.ch/twiki/bin/view/Atlas/G4AtlasApps',
            'recipes': 'https://twiki.cern.ch/twiki/bin/view/Atlas/RecipesG4AtlasApps' }
        G4AtlasEngine.List_Elements = []
        G4AtlasEngine.List_LoadedDict = []
        G4AtlasEngine.List_LoadedLib = []
        G4AtlasEngine.List_LoadedXML = []

        ## Simulation name and number of instances control
        G4AtlasEngine.Name = name_simu
        G4AtlasEngine._NumInstances += 1

        ## Counter to track the number of callback functions that have been executed
        G4AtlasEngine._callback_counter = 0

        ## Logging service
        G4AtlasEngine.log = Logging.logging.getLogger('G4AtlasApps')
        ## Configure the logger to automatically prepend the method it's called from
        #if not G4AtlasEngine.log.handlers:
        #    G4AtlasEngine.log.addHandler(Logging.logging.StreamHandler())
        #fmt = Logging.logging.Formatter('%(name) %(module)s %(funcName)s: %(message)s')
        #G4AtlasEngine.log.handlers[0].setFormatter(fmt)
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        G4AtlasEngine.log.setLevel(10*(svcMgr.MessageSvc.OutputLevel - 1))

        from ISF_Geant4Config._app_profiler import _app_profiler
        # The profiler will work if log level = debug
        G4AtlasEngine._app_profiler = _app_profiler( G4AtlasEngine )

        # pylcgdict default dictionaries
        self.load_Dict('AtlasSealCLHEPDict')
        self.load_Dict('G4AtlasControlDict')
        #self.load_Dict('G4AtlasUtilitiesDict')
        G4AtlasEngine.gbl = PyCintex.makeNamespace("")
        G4AtlasEngine._ctrl = G4AtlasEngine.gbl.SimControl()
        self.load_Lib("G4DetectorEnvelopes")
        self.load_Lib("GeoDetectorPlugIns")
        self.load_Lib("G4StepLimitation")
        self.init_status = 0
        G4AtlasEngine._InitList = list()
        G4AtlasEngine.log.info('G4AtlasEngine: __init__ done')


    def add_CompositeMaterial(self, name_m, fraction_mass):
        """ Adds Material to the simulation.
        """
        if name_m not in G4AtlasEngine.Dict_Materials:
            G4AtlasEngine.Dict_Materials[name_m] = G4AtlasEngine.gbl.CompositeMaterial(name_m, fraction_mass)
            G4AtlasEngine.log.debug('G4AtlasEngine.add_CompositeMaterial: composite material %s added' % name_m)
        else:
            G4AtlasEngine.log.warning(' G4AtlasEngine.add_CompositeMaterial: composite material %s is already defined' % name_m)


    def add_DetFacility(self, det_obj, det_obj_father):
        """
        Adds DetFacilities to the G4AtlasEngine.

        If both DetFacilities are already built, the DetFacility will be
        immediately added. If this is not the case, the "add" procedure will
        happen at the init_DetFacility process.
        """
        from ISF_Geant4Config.DetFacility import DetFacility
        if not isinstance(det_obj, DetFacility):
            G4AtlasEngine.log.error(' G4AtlasEngine: det_obj is not a DetFacility !!!')
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_DetFacility')
        if not isinstance(det_obj_father, DetFacility):
            G4AtlasEngine.log.error(' G4AtlasEngine: det_obj_father is not a DetFacility !!!')
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_DetFacility')

        det_obj.Father = det_obj_father.Name
        if det_obj._Built and det_obj_father._Built:
            # TODO: Already done above... can we remove?
            det_obj.Father = det_obj_father.Name
            if det_obj == det_obj_father:
                # TODO: Already done above... can we remove?
                det_obj.Father = det_obj_father.Name
                det_obj.df.SetAsWorld()
                self.Dict['volume_world'] = det_obj
                self.Dict_DetFacility[det_obj.Name] = det_obj
                det_obj._BuiltAssoc = True
                G4AtlasEngine.log.debug(' G4AtlasEngine: add_DetFacility: %s as a world volume' % det_obj.Name)
            else:
                det_obj_father.df.AddDetector(det_obj.df)
                self.Dict_DetFacility[det_obj.Name] = det_obj
                det_obj_father.Dict_DetFacility[det_obj.Name] = det_obj
                det_obj._BuiltAssoc = True
                G4AtlasEngine.log.debug(' G4AtlasEngine: add_DetFacility: %s as daughter of %s' %
                                              (det_obj.Name, det_obj_father.Name))
        else:
            self.Dict_DetFacility[det_obj.Name] = det_obj
            #det_obj_father.Dict_DetFacility[det_obj.Name] = det_obj
            G4AtlasEngine.log.debug(' G4AtlasEngine: add_DetFacility: %s will be added as a daughter of %s' %
                                    (det_obj.Name, det_obj_father.Name))
            if det_obj == det_obj_father:
                if 'volume_world' not in G4AtlasEngine.Dict:
                    self.Dict['volume_world'] = det_obj
                else:
                    G4AtlasEngine.log.error(' G4AtlasEngine: add_DetFacility: there is already a volume_world')
                    raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_DetFacility')
        G4AtlasEngine._app_profiler('  add_DetFacility: ' + det_obj.Name)



    def add_FastSimModel(self, model_obj):
        """
        Adds FastSimModel objects to the G4AtlasEngine.

        The G4AtlasEngine.Dict_Regions gets also populated with the regions
        associated to the FastSimModel.
        """
        if not isinstance(model_obj, FastSimModel):
            G4AtlasEngine.log.error(' FastSimModel: add_PhysicsReg: This is not a PhysicsReg object!!!')
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_FastSimModel')

        if model_obj.Name not in G4AtlasEngine.Dict_FastSimModel:
            G4AtlasEngine.Dict_FastSimModel[model_obj.Name] = model_obj
            for i in model_obj.Dict_Regions.keys():
                i_r_obj = model_obj.Dict_Regions.get(i)
                G4AtlasEngine.Dict_Regions.get('fastSimModels').update( { i_r_obj.Name : i_r_obj } )
            G4AtlasEngine.log.debug('G4AtlasEngine: adding the FastSimModel %s with associated PhysRegions %s' %
                                    (model_obj.Name, str(model_obj.Dict_Regions.keys())) )
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: adding the FastSimModel: the %s model has been already added to G4Eng with the regions %s' %
                                      (model_obj.Name, str(model_obj.Dict_Regions.keys())) )



    def add_Element(self, name_el, symbol_el, z_el, a_el, density_el):
        """
        Add a new element to the simulation.

        If you do not need to specify the density, set it to 0.
        """
        if name_el not in G4AtlasEngine.List_Elements:
            G4AtlasEngine.gbl.Element(name_el, symbol_el, z_el, a_el, density_el)
            G4AtlasEngine.List_Elements.append(name_el)
            G4AtlasEngine.log.debug(' G4AtlasEngine: add_Element: The Element %s is added' % name_el)
        else:
            G4AtlasEngine.log.warning(' G4AtlasEngine: add_Element: The Element %s is alrady defined' % name_el)



    def add_SenDetector(self, SenDetector_obj):
        """ Adds a Sensitive detector directly to the AtlasG4Eng.

             This method is for very rare cases. Normally the SD is added
            to a specific DetFacility.
        """
        if isinstance(SenDetector_obj,SenDetector):
            if SenDetector_obj.Name not in G4AtlasEngine.Dict_SenDetector:
                G4AtlasEngine.Dict_SenDetector[SenDetector_obj.Name] = SenDetector_obj
                G4AtlasEngine.log.info(' G4AtlasEngine: add_SenDetector: %s added' % SenDetector_obj.Name)
            else:
                G4AtlasEngine.log.warning(' G4AtlasEngine: add_SenDetector simulation already has a %s' % SenDetector_obj.Name)
        else:
            G4AtlasEngine.log.error(' G4AtlasEngine: add_SenDetector: This is not a SenDetector object !!!')
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_SenDetector')



    def add_PhysicsList(self, PhysicsList_obj):
        """ Adds the Physics-List to the simulation engine.

           Several Physics-Lists can be defined and stored but
           only one can be added to the simulation and only
           one is built.
        """
        from ISF_Geant4Config.PhysicsList import PhysicsList
        if not isinstance(PhysicsList_obj, PhysicsList):
            G4AtlasEngine.log.error(' G4AtlasEngine: add_PhysicsList: This is not a PhysicsList object !!!')
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: add_PhysicsList')
        if 'physics' not in G4AtlasEngine.Dict:
            G4AtlasEngine.Dict['physics'] = PhysicsList_obj
            G4AtlasEngine.log.info(' G4AtlasEngine: add_PhysicsList: %s added' % PhysicsList_obj.Name)
        else:
            G4AtlasEngine.log.warning(' G4AtlasEngine: add_PhysicsList: simulation already has a Physics-List.')



    def _init_DetFacilityRecursive(self, name_det):
        """ Inits the geometry of a given DetFacility and associates it
            with the father DetFacility if this was not done yet.

           (for internal use: it is called by _init_DetFacility() )
        """
        dt = G4AtlasEngine.Dict_DetFacility.get(name_det)
        if not dt._Built:
            dt._build()
            dt.Built=True
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_DetFacilityRecursive: '+dt.Name+' was built')
        if not dt._BuiltAssoc:
            G4AtlasEngine.add_DetFacility(self, dt, G4AtlasEngine.Dict_DetFacility.get(dt.Father))
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_DetFacilityRecursive: %s was associated to parent %s' % (dt.Name, dt.Father))
        for dti in dt.Dict_DetFacility.values():
            # We ask the DetFacility to be in the G4AtlasEngine.Dict_DetFacility
            # in this way is easy to remove it if needed.
            if dti.Name not in G4AtlasEngine.Dict_DetFacility:
                G4AtlasEngine.log.debug(' G4AtlasEngine:'+
                                        ' _init_DetFacilityRecursive: '+dti.Name+' will not be built'+
                                        ' and associated because it is not in G4Eng.Dict_DetFacility ')
            else:
                if not dti._Built:
                    dti._build()
                    G4AtlasEngine.log.debug(' G4AtlasEngine: _init_DetFacilityRecursive: '+dti.Name+' was built')
                if not dti._BuiltAssoc:
                    G4AtlasEngine.add_DetFacility(self, dti, G4AtlasEngine.Dict_DetFacility.get(dt.Father))
                    G4AtlasEngine.log.debug(' G4AtlasEngine:' + ' _init_DetFacilityRecursive: ' +
                                                  dt.Name + ' was associated to ' + dt.Father)
                if not dti.Dict_DetFacility.values():
                    G4AtlasEngine._init_DetFacilityRecursive(self, dti.Name)


    def _init_DetFacility(self):
        """ Inits the geometry of the DetFacilities

           (for internal use)
        """
        if 'init_DetFacility' not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_DetFacility: init geometry of the DetFacilities ')
            wv = G4AtlasEngine.Dict.get('volume_world')
            G4AtlasEngine._init_DetFacilityRecursive(self, wv.Name)
            # Check if every DetFacility is Built
            for df in G4AtlasEngine.Dict_DetFacility.keys():
                df_obj = G4AtlasEngine.Dict_DetFacility.get(df)
                if df_obj._Built:
                   G4AtlasEngine.log.info(' G4AtlasEngine: _init_DetFacility: '+df_obj.Name+' is built')
                else:
                   G4AtlasEngine.log.warning(' G4AtlasEngine: _init_DetFacility: '+df_obj.Name+' is not built')
                # the post build
                if not df_obj._PostBuilt:
                    df_obj._postbuild()
                    df_obj.PostBuilt=True
            self._InitList.append('init_DetFacility')
        else:
            G4AtlasEngine.log.warning(' G4AtlasEngine: init_DetFacility is already done')


    def _init_G4(self):
        """ Inits G4

           (for internal use)
        """
        if "init_G4" not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_G4: init Geant4 ')
            if G4AtlasEngine.log.level <= 30:
                g4Command = G4AtlasEngine.gbl.G4Commands()
                g4Command.run.verbose(2)
            G4AtlasEngine._ctrl.initializeG4()
            self._InitList.append('init_G4')
            G4AtlasEngine._app_profiler('_init_G4: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_G4 is already done')


    def _init_MCTruth(self):
        """ Inits MCTruth strategies

           (for internal use)
        """
        if 'init_MCTruth' not in self._InitList:
            G4AtlasEngine.log.debug('G4AtlasEngine: _init_MCTruth: init MCTruth strategies ')
            for i in G4AtlasEngine.Dict_MCTruthStrg.keys():
                if i != 'SecondarySavingPolicy':
                    G4AtlasEngine.Dict_MCTruthStrg.get(i)._construct()
            G4AtlasEngine._ctrl.mctruthMenu.secondarySaving(G4AtlasEngine.Dict_MCTruthStrg.get('SecondarySavingPolicy'))
            G4AtlasEngine._ctrl.mctruthMenu.listStrategies()
            self._InitList.append('init_MCTruth')
            G4AtlasEngine._app_profiler('_init_MCTruth: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_MCTruth is already done')
            G4AtlasEngine._ctrl.mctruthMenu.listStrategies()


    def _init_RecEnvelope(self):
        """ Inits the recording within the envelopes

           (for internal use)
        """
        if 'init_RecEnvelope' not in self._InitList:
            G4AtlasEngine.log.debug('G4AtlasEngine: _init_RecEnvelope: init RecEnvelope  ')
            if G4AtlasEngine.Dict_RecEnvelope.keys():
                G4AtlasEngine._ctrl.mctruthMenu.enableEnvelopeRecording()
                for i in G4AtlasEngine.Dict_RecEnvelope.keys():
                   G4AtlasEngine.Dict_RecEnvelope.get(i)._construct()
            self._InitList.append('init_RecEnvelope')
            G4AtlasEngine._app_profiler('_init_RecEnvelope: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_RecEnvelope is already done')


    def _init_SenDetector(self):
        """ Inits all the sensitive detectors defined.

           (for internal use)
        """
        if 'init_SenDetector' not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_SenDetector: init SD ')
            ## First build the SD in the List_SenDetector_dependence
            for sd_n in xrange(len(G4AtlasEngine._List_SenDetector_dependence)):
                sd_first_name = G4AtlasEngine._List_SenDetector_dependence[sd_n]
                if sd_first_name in G4AtlasEngine.Dict_SenDetector:
                    sd_first = G4AtlasEngine.Dict_SenDetector.get(sd_first_name)
                    if not sd_first._Built:
                        sd_first._build()
                    G4AtlasEngine.log.debug('G4AtlasEngine:'+
                                                  ' _init_SenDetector: init SD '+sd_first_name+
                                                  'in the dependence list  _List_SenDetector_dependence')
            ## Look for SD associated to a DetFacility
            for df in G4AtlasEngine.Dict_DetFacility.keys():
                df_obj = G4AtlasEngine.Dict_DetFacility.get(df)
                for s in df_obj.Dict_SenDetector.keys():
                    sd = G4AtlasEngine.Dict_SenDetector.get(s)
                    if not sd._Built:
                        sd._build()
            ## Look for SD associated to a list of volumes
            for sd in G4AtlasEngine.Dict_SenDetector.keys():
                sd_obj = G4AtlasEngine.Dict_SenDetector.get(sd)
                if not sd_obj._Built and sd_obj.List_Volumes:
                    sd_obj._build()
            self._InitList.append('init_SenDetector')
            G4AtlasEngine._app_profiler('_init_SenDetector: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_SenDetector is already done')


    def _init_Fields(self):
        """ Inits all the fields defined.
           (for internal use)
        """
        if 'init_Fields' not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_Fields: init magnetic fields ')
            for mfd in G4AtlasEngine.Dict_Fields.keys():
                mfd_obj = G4AtlasEngine.Dict_Fields.get(mfd)
                mfd_obj._build()
            self._InitList.append('init_Fields')
            G4AtlasEngine._app_profiler('_init_Fields: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_Fields is already done')


    def _init_FieldIntegrationParameters(self):
        """ Inits all the field integration parameter objects defined.

           (for internal use)
        """
        if 'init_FieldIntegrationParameters' in self._InitList:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_FieldIntegrationParameters is already done')
            return

        G4AtlasEngine.log.debug('G4AtlasEngine: _init_FieldIntegrationParameters: init field integration parameters')
        for fip in G4AtlasEngine.Dict_FieldIntegrationParameters.values():
            fip._build()
        self._InitList.append('init_FieldIntegrationParameters')
        G4AtlasEngine._app_profiler('_init_FieldIntegrationParameters: ')


    def _init_FastSimModel(self):
        """ Inits all the parametrization models defined.
            (for internal use)

            Creates also the Physics Regions used by the model (>=G4.8)
        """
        if 'init_FastSimModel' in self._InitList:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_FastSimModel is already done')
            return
        if 'init_G4' in self._InitList:
            G4AtlasEngine.log.error('G4AtlasEngine: init_G4 is already done too late for _init_FastSimModel')
            return

        G4AtlasEngine.log.debug('G4AtlasEngine:_init_FastSimModel: init parametrization models')
        if G4AtlasEngine.Dict_FastSimModel.keys():
            G4AtlasEngine.load_Lib("G4FastSimulation")
            G4AtlasEngine.load_Dict("G4FastSimulationDict")
            fastmenu = G4AtlasEngine.gbl.FastSimMenu()
            for fsm in G4AtlasEngine.Dict_FastSimModel.keys():
                fsm_obj = G4AtlasEngine.Dict_FastSimModel.get(fsm)
                if fsm_obj.Library not in G4AtlasEngine.List_LoadedLib:
                    G4AtlasEngine.load_Lib(fsm_obj.Library)
                for r_n in fsm_obj.Dict_Regions.keys():
                    r_n_obj = fsm_obj.Dict_Regions.get(r_n)
                    r_n_obj._construct_Region()
                    fastmenu.AssignFsModel(r_n_obj.Name, fsm_obj.Name)
        self._InitList.append('init_FastSimModel')
        G4AtlasEngine._app_profiler('_init_FastSimModel: ')


    def _init_Graphics(self):
        """ Inits the G4 visualization stuff.

            (for internal use)
        """
        if 'init_Graphics' not in self._InitList:
            G4AtlasEngine.log.debug('G4AtlasEngine: _init_Graphics: init G4 Graphics ')
            G4Graphics = G4AtlasEngine.menu_Visualization()
            if G4Graphics.ActiveStatusOn:
                G4Graphics._init()
            else:
               G4AtlasEngine.log.debug('G4AtlasEngine: _init_Graphics: init G4 Graphics -->  no graphics request'                     )
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_Graphics is already done')


    def _init_Physics(self):
        """ Inits the Geant4 selected Physics list (for internal use)
        """
        if 'init_Physics' not in self._InitList:
            from ISF_Geant4Config.ISF_SimFlags import simFlags
            G4AtlasEngine.log.debug('ISF_G4AtlasEngine: _init_Physics: init physics list')
            if hasattr(simFlags, 'NeutronTimeCut') and simFlags.NeutronTimeCut.statusOn:
                import AtlasG4Eng
                AtlasG4Eng.G4Eng._ctrl.physicsMenu.SetNeutronTimeCut(simFlags.NeutronTimeCut.get_Value())
            if hasattr(simFlags, 'NeutronEnergyCut') and simFlags.NeutronEnergyCut.statusOn:
                import AtlasG4Eng
                AtlasG4Eng.G4Eng._ctrl.physicsMenu.SetNeutronEnergyCut(simFlags.NeutronEnergyCut.get_Value())
            physics = G4AtlasEngine.Dict.get('physics')
            physics._construct()
            physics.Built = True
            self._InitList.append('init_Physics')
            G4AtlasEngine._app_profiler('_init_Physics: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_Physics is already done')


    def _init_PhysicsRegions(self):
        """ Inits the physics regions

           (for internal use)
            This internal method inits the physics-regions used
            for physics purposes. Must be called before init_G4()
        """
        if 'init_PhysicsRegions' not in self._InitList and 'init_G4' not in self._InitList:
            G4AtlasEngine.log.debug('G4AtlasEngine: _init_PhysicsRegions: init physics regions')
            for i in self.Dict_DetFacility.keys():
                p = self.Dict_DetFacility.get(i)
                for j in p.Dict_Regions.keys():
                    k = p.Dict_Regions.get(j)
                    k._construct_Region()
            self._InitList.append('init_PhysicsRegions')
            G4AtlasEngine._app_profiler('_init_PhysicsRegions: ')
        elif 'init_G4' in self._InitList:
            G4AtlasEngine.log.error('G4AtlasEngine: init_G4 is already done, too late for regions')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_PhysicsRegions is already done')


    def _init_Simulation(self):
        """\
        Simulation engine initialization.

        Internal method which initializes the simulation engine. The simulation
        passes through several distinct levels at which user configuration may
        be done via the simFlags.InitFunctions dict, keyed on the appropriate
        hook point. The provided hook points are

          preInit - called before Athena initialize()
          pre/postInitDetFacility - called before/after the init_DetFacility method
          pre/postInitPhysics - called before/after the init_Physics method
          pre/postInitG4 - called before/after the init_G4 method
          pre/postInitFastSimModel - called before/after the init_FastSimModel method
          pre/postInitPhysicsRegions - called before/after the init_PhysicsRegions method
          pre/postInitSenDetector - called before/after the init_SenDetector method
          pre/postInitMCTruth - called before/after the init_MCTruth method
          pre/postInitFields - called before/after the init_Fields method
          pre/postInitFieldIntegrationParameters - called before/after the init_FieldIntegrationParameters method
          pre/postInitRecEnvelope - called before/after the init_RecEnvelope method
          pre/postInitGraphics - called before/after the init_Graphics method
          postInit - called after all sim engine initialisation methods

        The current init level is stored in G4AtlasEngine.init_status, and its
        name corresponds to the active hook at that point.
        """

        def _run_init_callbacks(init_level):
            from ISF_Geant4Config.ISF_SimFlags import simFlags
            if simFlags.InitFunctions.statusOn and init_level in simFlags.InitFunctions.get_Value():
                #print simFlags.InitFunctions.get_Value()
                for callback_fn in simFlags.InitFunctions.get_Value()[init_level]:
                    callback_fn.__call__()
                    G4AtlasEngine._callback_counter += 1

        def _run_init_stage(name):
            # TODO: Set self.init_status at each sub-step and write that out in _run_init_callbacks
            self.init_status = "preInit" + name
            G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
            _run_init_callbacks(self.init_status)
            self.init_status = "init" + name
            G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
            getattr(self, "_init_" + name).__call__()
            self.init_status = "postInit" + name
            G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
            _run_init_callbacks(self.init_status)

        _run_init_stage("DetFacility")
        _run_init_stage("FastSimModel")
        _run_init_stage("PhysicsRegions")
        _run_init_stage("SenDetector")
        _run_init_stage("Physics")
        _run_init_stage("G4")
        #_run_init_stage("MCTruth")
        _run_init_stage("Fields")
        _run_init_stage("FieldIntegrationParameters")
        #_run_init_stage("RecEnvelope")
        _run_init_stage("Graphics")

        self.init_status = "postInit"
        G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
        _run_init_callbacks(self.init_status)

        ## Check that all the callbacks were indeed called, and warn otherwise
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        num_reg_callbacks = sum(len(cblist) for cblist in simFlags.InitFunctions.get_Value().values())
        if G4AtlasEngine._callback_counter != num_reg_callbacks:
            G4AtlasEngine.log.warning("G4AtlasEngine: mismatch in num of callbacks regd/called = %d/%d" %
                                      (num_reg_callbacks, G4AtlasEngine._callback_counter))

        G4AtlasEngine._app_profiler('_init_Simulation')


    def list_DetFacility(self):
        """ Prints the detector facilities associated with a given G4AtlasEngine.
        """
        for i in self.Dict['Dict_DetFacility'].values():
            print i.Name


    def list_Element(self):
        """ Prints the elements associated with a given G4AtlasEngine.
        """
        self.gbl.Element.List()


    def list_Material(self):
        """ Prints the materials associated with a given G4AtlasEngine.
        """
        self.gbl.Material.List()


    def list_PhysicsList(self):
        """ Prints the Physics Lists availables.
        """
        if "G4PhysicsLists" not in G4AtlasEngine.List_LoadedLib:
            G4AtlasEngine._ctrl.load("G4PhysicsLists")
            G4AtlasEngine.List_LoadedLib.append("G4PhysicsLists")
        G4AtlasEngine._ctrl.physicsMenu.PrintLists()


    def list_Region(self):
        """ List the Physics regions already defined.
        """
        G4AtlasEngine._ctrl.physicsMenu.ListRegions()


    @classmethod
    def load_Lib(cls, lib_name):
        """ Loads libraries.

            lib_name ='name_library'
        """
        if lib_name:
            if lib_name not in G4AtlasEngine.List_LoadedLib:
                try:
                    G4AtlasEngine._ctrl.load(lib_name)
                    G4AtlasEngine.List_LoadedLib.append(lib_name)
                    G4AtlasEngine.log.debug('G4AtlasEngine:load_Lib: %s loaded' % lib_name)
                except:
                    raise RuntimeError('Lib %s cannot be found' % lib_name)


    @classmethod
    def load_Dict(cls, dict_name):
        """ Loads  python dictionaries

            dict_name = name of the dictionary
        """
        if dict_name and dict_name not in G4AtlasEngine.List_LoadedDict:
            try:
                PyCintex.loadDict(dict_name)
            except:
                print "Unexpected error:", sys.exc_info(),'\n'
                print 'ROOT5 migration problem: ', dict_name
            try:
                G4AtlasEngine.List_LoadedDict.append(dict_name)
                G4AtlasEngine.log.debug(' G4AtlasEngine:load_Dict: %s loaded' % dict_name)
            except:
                raise RuntimeError('Dict %s can not be found' % dict_name)


    def read_XML(self, xml_name):
        """ Reads XML files.

            xml_name ='name_XML_file'
        """
        if xml_name:
            if xml_name not in G4AtlasEngine.List_LoadedXML:
                try:
                    G4AtlasEngine._ctrl.ReadXML(xml_name)
                    G4AtlasEngine.List_LoadedXML.append(xml_name)
                    G4AtlasEngine.log.debug('G4AtlasEngine:read_XML: %s read' % xml_name)
                except:
                    RuntimeError('XML file %s can not be found' % xml_name)


    def print_Summary(self):
        """ Prints the summary
        """
        print self.Name
        print self.List_LoadedLib



    class menu_Field:
        """ Menu for the magnetic fields.
        """

        @classmethod
        def list_Fields(cls):
            """ Lists the existing magnetic fields.

                The libray must be loaded in advance. For example
               the G4Field can be loaded.
            """
            G4AtlasEngine._ctrl.fldMenu.list()

        @classmethod
        def set_EquationOfMotion(cls, eq_name):
            """ Set the equation of motion by name
            """
            G4AtlasEngine._ctrl.fldMenu.SetEquationOfMotion(eq_name)


        @classmethod
        def add_Field(cls, mag_obj):
            """ Adds a magnetic field to the simulation.


                mag_object must be a Python object of type MagneticField.
            """
            from ISF_Geant4Config.MagneticField import MagneticField
            if isinstance(mag_obj,MagneticField):
                if mag_obj.Name not in G4AtlasEngine.Dict_Fields:
                    G4AtlasEngine.Dict_Fields[mag_obj.Name] = mag_obj
                    G4AtlasEngine.log.debug(' G4AtlasEngine: menu_Field: add_Field: added ' + mag_obj.Name)
            else:
                G4AtlasEngine.log.error(' G4AtlasEngine: menu_Field: add_Field: This is not a MagneticField object!!!')



    class menu_FieldIntegrationParameters:
        """ Menu for the field integration parameters.

        """

        @classmethod
        def list_Parameters(cls):
            """ Lists the existing field integration parameters.
            """
            G4AtlasEngine._ctrl.fldIntMenu.list()

        @classmethod
        def show_PythonParameters(cls):
            """ Prints the parameters for all field integration parameters
                defined in the python layer
            """
            for par in G4AtlasEngine.Dict_FieldIntegrationParameters.keys() :
                G4AtlasEngine.Dict_FieldIntegrationParameters[par].show_Parameters()


        @classmethod
        def add_FieldIntegrationParameters(cls, fip_obj):
            """ Adds a field integration parameters instance to the simulation.

                fip_obj must be a Python object of type FieldIntegrationParameters.
            """
            if isinstance(fip_obj, FieldIntegrationParameters):
                if fip_obj.Name not in G4AtlasEngine.Dict_FieldIntegrationParameters:
                    G4AtlasEngine.Dict_FieldIntegrationParameters[fip_obj.Name] = fip_obj
                    G4AtlasEngine.log.debug('G4AtlasEngine:' +
                                                  ' menu_FieldIntegrationParameters: add_FieldIntegrationParameters: added '+fip_obj.Name)
            else:
                G4AtlasEngine.log.error(' G4AtlasEngine:'+
                                              ' menu_FieldIntegrationParameters: add_FieldIntegrationParameters: '+
                                              ' This is not a FieldIntegrationParameters object!!!')



    class menu_MCTruth:
        """ MC truth strategies can be added using this menu.

            As a menu, a instance of it is needed to access the
            following methods:

            .add_McTruthStrategy(MCTruthStrg_object)
            .add_SecondarySaving()
            .list_Strategies()
            .list_Parameters()
            .set_TruthStrategiesParameter('param_name',value)
        """

        @classmethod
        def add_McTruthStrategy(cls, strg_obj):
            """Adds a strategy to the dictionary of MCTruth strategies.

               The strategies will be built after the physics-list and
               physics-regions.
            """
            if isinstance(strg_obj,MCTruthStrg):
                G4AtlasEngine.Dict_MCTruthStrg[strg_obj.Name] = strg_obj
                G4AtlasEngine.log.debug(' G4AtlasEngine:'+
                                              'menu_MCTruth:add_McTruthStrategy: '+
                                              'added '+strg_obj.Name)
            else:
                G4AtlasEngine.log.error(' G4AtlasEngine:'+
                                              'menu_MCTruth:add_McTruthStrategy:'+
                                              ' This is not a MCTruthStrg object!!!')


        @classmethod
        def list_Strategies(cls):
            """ List the possible strategies.
            """
            G4AtlasEngine._ctrl.mctruthMenu.listStrategies()


        @classmethod
        def list_Parameters(cls):
            """ List parameters for the MCTruth.
            """
            G4AtlasEngine._ctrl.mctruthMenu.listParameters()


        @classmethod
        def set_SecondarySaving(cls, p):
            """ Sets the secondary saving.

                The possible values are:
                      Primaries, StoredSecondaries , All

                The value for production must be StoredSecondaries
            """
            G4AtlasEngine.Dict_MCTruthStrg['SecondarySavingPolicy'] = p


        @classmethod
        def set_TruthStrategiesParameter(cls, param_name, param_value):
            """ Sets the parameters for the MCTruth

               for the list of parameters look into list_Parameters
            """
            G4AtlasEngine._ctrl.mctruthMenu.setTruthStrategiesParameter(param_name,param_value)



    class menu_RecordEnvelope:
        """ Handles the volumes in which the track record will
            be applied

            .add_RecEnvelope(RecEnvelope_obj)
        """


        def add_RecEnvelope(self,obj):
            """ Adds a RecEnvelope object to the simulation
                engine.
            """
            from ISF_Geant4Config.atlas_mctruth import RecEnvelope
            if isinstance(obj,RecEnvelope):
                G4AtlasEngine.Dict_RecEnvelope[obj.Name]=obj
                G4AtlasEngine.log.debug(' G4AtlasEngine:'+\
                'menu_RecordEnvelope:add_RecEnvelope: '+\
                'added '+obj.Name)
            else:
               G4AtlasEngine.log.error(' G4AtlasEngine:'+\
               'menu_RecordEnvelope:add_RecEnvelope:'+\
               ' This is not a RecEnvelope object!!!')


        def _init(self):
            """ Done at the init time automatically. """
            G4AtlasEngine._ctrl.mctruthMenu.enableEnvelopeRecording()



    class menu_UserActions:
        """
        User actions can be added using this menu.
        """

        @classmethod
        def add_UserAction(cls, action_obj):
            """Adds a UserAction object to the simulation."""
            from ISF_Geant4Config.UserAction import UserAction
            if isinstance(action_obj,UserAction):
                G4AtlasEngine.log.debug('G4AtlasEngine: menu_UserActions:add_UserAction: ' +
                                              action_obj.Name)
                G4AtlasEngine.Dict_UserAction[action_obj.Name] = action_obj
                action_obj._construct()
            else:
               G4AtlasEngine.log.error('G4AtlasEngine: menu_UserActions:add_UserAction: '+
                                             'This is not a UserAction object!!!')

        @classmethod
        def list_Actions(cls):
            """
            Lists the possible actions.

            The list is extracted from the user-action libraries already loaded.
            """
            G4AtlasEngine._ctrl.actMenu.actionList()


        @classmethod
        def load_Lib(cls, Library):
            """Loads the library with the user-actions."""
            G4AtlasEngine.load_Lib(Library)
            G4AtlasEngine._ctrl.actMenu.actionList()



    class menu_EventFilter(object):
        """
        Access to the event filters
        """

        # TODO: Why the separate implementation object? Clean this up.
        class __impl:
            """
            All the filters are off by default.
            """
            def __init__(self):

                self.EtaPhiFilters = G4AtlasEngine.gbl.EtaPhiFilters('EtaPhiFilters')

                self.VertexRangeChecker = G4AtlasEngine.gbl.VertexRangeChecker('VertexRangeChecker')
                from ISF_Geant4Config.ISF_SimFlags import simFlags
                if hasattr(simFlags, 'WorldZRange') and simFlags.WorldZRange.statusOn:
                     self.VertexRangeChecker.SetZ(  simFlags.WorldZRange.get_Value() )
                if hasattr(simFlags, 'WorldRRange') and simFlags.WorldRRange.statusOn:
                     self.VertexRangeChecker.SetRmax( simFlags.WorldRRange.get_Value() )

                self.VertexPositioner = G4AtlasEngine.gbl.VertexPositioner('VertexPositioner')

                self.PrimaryEventRotations = G4AtlasEngine.gbl.PrimaryEventRotations('PrimaryEventRotations')

                self.BeamEffectTransformation = G4AtlasEngine.gbl.BeamEffectTransformation('BeamEffectTransformation')

                self.FilterStatusOn = {
                    'EtaPhiFilters' : False,
                    'PrimaryEventRotations' : False,
                    'BeamEffectTransformation' : False,
                    'VertexPositioner' : False,
                    'VertexRangeChecker' : False
                    }


        # TODO: Remove along with __impl
        __instance = None
        def __getattr__(self, attr):
            return getattr(self.__instance, attr)
        def __setattr__(self, attr, value):
            return setattr(self.__instance, attr, value)


        # TODO: Clean with __impl removal
        def __init__(self):
            if G4AtlasEngine.menu_EventFilter.__instance is None:
                G4AtlasEngine.menu_EventFilter.__instance = G4AtlasEngine.menu_EventFilter.__impl()
            # TODO: Clean this up when __setattr__ is not overridden
            self.__dict__['_menu_EventFilter.__instance'] = G4AtlasEngine.menu_EventFilter.__instance
            self._Built = False


        def getFilter(self, name_filter):
            """
            Provides access to a given filter by name.

            The filters can be customized from here.
            Available filters are:
            - EtaPhiFilters
            - PrimaryEventRotations
            - VertexPositioner
            - VertexRangeChecker
            - BeamEffectTransformation
            """
            f = getattr(self, name_filter)
            if f is None:
                G4AtlasEngine.log.warning('menu_EventFilter: event filter unknown')
            return f


        # TODO: Why? Use a property instead? This can't be written, can it?
        def getFilterStatus(self):
            """
            Returns the status of the different filters.

            The status can be changed from here, it is a Python dictionary that
            can be set to True or False values for each filter.  The filters
            will be activated following the status here selected.
            """
            return self.FilterStatusOn


        def _build(self):
            """
            Builds the filters and set them to the indicated status.

            See the flag 'simFlags.EventFilter'
            """
            if self._Built:
                G4AtlasEngine.log.debug('menu_EventFilter._build: init EventFilter manipulators already done\n%s' % self.getFilterStatus())
                return
            self._Built = True

            ## Switch filters on and off as required
            # TODO: Why this indirect nonsense?
            for fname, factive in self.FilterStatusOn.iteritems():
                fobj = getattr(self, fname)
                if not fobj:
                    raise Exception("Unknown event filter '%s'" % fname)
                # TODO: These objects would be better off with an API method that accepts an on/off boolean
                if factive:
                    fobj.switchOn()
                else:
                    fobj.switchOff()

            G4AtlasEngine.Dict['EventFilters'] = self
            G4AtlasEngine.log.debug('menu_EventFilter._build: init EventFilter manipulators\n%s' % self.getFilterStatus())



    class menu_G4RandomNrMenu(object):
        """ Access to the random number generator used by G4
        """
        class __impl:
            # TODO: Is this a joke?!? WHY?!?
            def spam(self):
                return id(self)
        __instance = None


        def __init__(self):
            if G4AtlasEngine.menu_G4RandomNrMenu.__instance is None:
                G4AtlasEngine.menu_G4RandomNrMenu.__instance = G4AtlasEngine.menu_G4RandomNrMenu.__impl()
                self.SaveStatus = False
                self.Menu = G4AtlasEngine._ctrl.rndMenu


        def __getattr__(self, attr):
            return getattr(self.__instance, attr)


        def __setattr__(self, attr, value):
            return setattr(self.__instance, attr, value)


        def set_SaveOn(self):
            """
            Activate the saving of the G4 seeds for the last event
            """
            self.SaveStatus = True


        def set_SaveOff(self):
            """
            Deactivate the saving of the G4 seeds for the last event
            """
            self.SaveStatus = False


        def get_SaveStatusOn(self):
            return self.SaveStatus


        def set_Seed(self, seed, from_file=False):
            """
            Set the G4 seeds.

            Normally the seed argument will be a numeric value to be directly
            passed as the current seed, but if the from_file arg is set to True,
            then the 'seed' argument will be treated as the name of a seed list
            file from which the seed for the current event will be read.

            TODO: Remove the loading from file if not required in MC11 production (2011-04-04)
            """
            try:
                if not from_file:
                    self.Menu.setSeed(seed)
                else:
                    self.Menu.retrieveStatus(seed)
            except Exception, e:
                G4AtlasEngine.log.error('menu_G4RandomNrMenu: incompatible argument for random number engine seed: %s' % e)



    # TODO: PLEASE can we remove this?
    class menu_Visualization(object):
        """
            Initial version of the menu for the visualization.

            NOT READY YET!!!!
        """
        class __impl:
            def spam(self):
                return id(self)
        __instance=None


        def __init__(self):
            if G4AtlasEngine.menu_Visualization.__instance is None:
                G4AtlasEngine.menu_Visualization.__instance = G4AtlasEngine.menu_Visualization.__impl()
                self._Built=False
                self.ActiveStatusOn=False
                self.VisTracks=False
                self.VisDriver='VRML2FILE'
                from AtlasG4Eng import GeV
                self.TrackPtCut=.3*GeV
                self.DrawNeutralTracks=False
                self.List_Volumen2Vis=list()
                self.List_Volumen2NotVis=list()


        def __getattr__(self, attr):
            return getattr(self.__instance, attr)


        def __setattr__(self, attr, value):
            return setattr(self.__instance, attr, value)


        def add_volume2vis(self,name_volume):
            """ Adds only one volume or wild-card to the list
                of volumes you want to visualize
            """
            self.List_Volumen2Vis.append(name_volume)


        def add_ListV2vis(self,list_volume):
            """ Adds a list of volumes or wild-cards to the list
                of volumes you want to visualize
            """
            self.List_Volumen2Vis=self.List_Volumen2Vis+list_volume


        def add_volume2Notvis(self,name_volume):
            """ Adds only one volume or wild-card to the list
                of volumes you do not want to visualize
            """
            self.List_Volumen2NotVis.append(name_volume)


        def add_ListV2Notvis(self,list_volume):
            """ Adds a list of volumes or wild-cards to the list
                of volumes you do not want to visualize
            """
            self.List_Volumen2NotVis=self.List_Volumen2NotVis+list_volume


        def get_ListVolume2vis(self):
            """ Gets the list of volumes that will be visible.
            """
            return self.List_Volumen2Vis


        def get_ListVolume2Notvis(self):
            """ Gets the list of volumes that will be invisible.
            """
            return self.List_Volumen2NotVis


        def set_active(self):
            """ Activates the visualization
            """
            self.ActiveStatusOn=True


        def set_Parameters(self,name_parameter,new_value):
            """ Changes the default visualization parameters.

                 VisDriver          (default 'VRML2FILE')
                 TrackPtCut         (default .3*GeV     )
                 DrawNeutralTracks  (default False      )
            """
            if (name_parameter=='VisDriver'):
                self.VisDriver=new_value
            if (name_parameter=='TrackPtCut'):
                self.TrackPtCut=new_value
            if (name_parameter=='DrawNeutralTracks'):
                self.DrawNeutralTracks=new_value


        def set_VisTrack(self):
            """ Activates the visualization of tracks.
                It will produce one wrl file for each event
            """
            self.VisTracks=True


        def print_status(self):
            """ Gets the actual status of the visualization menu
            """
            print 'AtlasG4Eng.menu_visualization status:  '
            print '---------------------------------------'
            print 'Built::  ',self._Built
            print 'Active::  ',self.ActiveStatusOn
            print 'Visualize Tracks:: ',self.VisTracks
            print 'Visualization driver:: ',self.VisDriver
            print 'TrackPtCut :: ',self.TrackPtCut
            print 'DrawNeutralTracks :: ',self.DrawNeutralTracks
            print 'List of Volumes to visualize '
            print self.List_Volumen2Vis
            print 'List of Volumes not to visualize '
            print self.List_Volumen2NotVis


        def _init(self):
            if self.ActiveStatusOn and not(self._Built):
                # init graphics
                G4AtlasEngine._ctrl.initializeGraphics()
                self._Built=True
                # support for event visualization
            if self.VisTracks:
                    G4AtlasEngine.load_Lib('G4UserActions')
                    G4AtlasEngine.load_Lib('G4EventGraphics')
                    VisAction=UserAction('G4EventGraphics',\
                        'DrawEventPyAction',['BeginOfEvent','EndOfEvent'])
                    G4AtlasEngine.menu_UserActions.add_UserAction(VisAction)
                    G4AtlasEngine.load_Dict("G4EventGraphicsDict")
                    self.EventGraphics=\
                      G4AtlasEngine.gbl.EventGraphicsPyMessenger.Instance()
                    self.EventGraphics.SetTrackDisplayLevel(3)
                    self.EventGraphics.SetTrackPtCut(self.TrackPtCut)
                    self.EventGraphics.SetTrackColorScheme(3)
                    self.EventGraphics.SetDrawNeutralTracks(self.DrawNeutralTracks)
                    self.__dict__['EventGraphics']=self.EventGraphics
            if self.ActiveStatusOn:
                # invisible volumes
                for v1 in self.List_Volumen2NotVis:
                    G4AtlasEngine._ctrl.geometryMenu.SetInvisible(v1)
                # visible volumes
                for v2 in self.List_Volumen2Vis:
                    G4AtlasEngine._ctrl.geometryMenu.SetVisible(v2)
                G4command=G4AtlasEngine.gbl.G4Commands()
                G4command.vis.open(self.VisDriver)
                G4command.vis.drawVolume()
                G4command.vis.viewer.flush()


        def visualize(self):
            if  self._Built:
                G4command = G4AtlasEngine.gbl.G4Commands()
                G4command.vis.open(self.VisDriver)
                G4command.vis.drawVolume()
                G4command.vis.viewer.flush()
