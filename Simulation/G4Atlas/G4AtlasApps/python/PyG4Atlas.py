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
import cppyy


# Identify the environment as hive (multi-threading).
try:
    # TODO: we need a better way to do this.
    import GaudiHive
    is_hive = True
except ImportError:
    is_hive = False



# TODO: Rename to AppProfiler, to avoid class/variable confusion
class _app_profiler(object):
    """\
    Monitor the memory and CPU time used.

    This is used at different points in the initialization phase of the
    application. It runs if G4AtlasEngine.log.level <= 10.
    """

    def __init__(self, verboselevel):
        self._vlevel = verboselevel
        self._firstTime = time()
        self._outputfileName = 'G4InitStats.out'
        self._isfirstCall = False

    def __call__(self, action):
        if self._vlevel <= 10 or G4AtlasEngine.log.level <= 10:
            pid = os.getpid()
            statm = string.split(open('/proc/%d/statm' % pid, 'r').readlines()[0])
            stat = string.split(open('/proc/%d/stat' % pid, 'r').readlines()[0])
            now = time()
            G4AtlasEngine.log.debug('G4AtlasEngine::app_profiler')
            print "                                                  "
            print "      G4AtlasEngine::app_profiler at ", action
            print "      Memory status: Virtual %d kB, RSS %d kB" % (int(stat[22])/1048.576,int(statm[1])*4)
            print "      Time since beginning: %f s" % (now - self._firstTime)
            print "                                                  "
            filemode = "a"
            if not self._isfirstCall:
                filemode = "w"
                self._isfirstCall = True
            outputfile = open(self._outputfileName, filemode)
            outputfile.write("%s %d %d %f\n" % (action, int(stat[22])/1048.576, int(statm[1])*4, now-self._firstTime))



class G4AtlasEngine:
    """
    Generic simulation engine class.

    Geometry, sensitive detectors, physics lists, physical cuts, etc. must be
    attached to this class.
    """

    _NumInstances = 0

    def __init__(self, name_simu="", useISF=False):
        """
        Default simulation engine object initialization.

        Only one instance per simulation job.
        """
        if G4AtlasEngine._NumInstances > 0:
            msg = 'There is already an instance of the G4AtlasEngine class -- try to use it!'
            G4AtlasEngine.log.error('G4AtlasEngine: ' + msg)
            raise RuntimeError('PyG4Atlas: G4AtlasEngine: __init__(): Attempted multiple instantiation')

        ## Init dictionaries and lists
        # TODO: Clean up this whole ugly "Dict*" mess
        G4AtlasEngine.Dict = dict()
        G4AtlasEngine.Dict_DetConfig = dict()
        G4AtlasEngine.Dict_Materials = dict()
        #G4AtlasEngine.Dict_Colors = dict()
        G4AtlasEngine.Dict_MCTruthStrg = dict()
        G4AtlasEngine.Dict_MCTruthStrg['SecondarySavingPolicy'] = 'All'
        G4AtlasEngine.Dict_Fields = dict()
        G4AtlasEngine.Dict_UserAction = dict()
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

        # The profiler will work if log level = debug
        G4AtlasEngine._app_profiler = _app_profiler(G4AtlasEngine.log.level)

        # pylcgdict default dictionaries
        self.load_Dict('AtlasSealCLHEPDict')
        self.load_Dict('G4AtlasControlDict')
        G4AtlasEngine.gbl = cppyy.makeNamespace("")
        G4AtlasEngine._ctrl = G4AtlasEngine.gbl.SimControl()
        #self.load_Lib("G4DetectorEnvelopes") #FIXME remove?
        #self.load_Lib("GeoDetectorPlugIns")  #FIXME remove?
        #self.load_Lib("G4StepLimitation")    #FIXME still needed?
        self.init_status = 0

        self.useISF = useISF

        G4AtlasEngine._InitList = list()
        G4AtlasEngine.log.info('G4AtlasEngine: starting engine')


    def setUseISF(self, useISF) :
        G4AtlasEngine.log.info('setting useISF to %s' % useISF)
        self.useISF = useISF


    def _init_G4(self):
        """ Inits G4

           (for internal use)
        """
        if "init_G4" not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_G4: init Geant4 ')
            if G4AtlasEngine.log.level <= 30:
                g4Command = G4AtlasEngine.gbl.G4Commands()
                g4Command.run.verbose(2) # FIXME make configurable based on Athena message level?
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


    def _init_Fields(self):
        """ Inits all the fields defined.
           (for internal use)
        """
        if 'init_Fields' not in self._InitList:
            G4AtlasEngine.log.debug(' G4AtlasEngine: _init_Fields: init magnetic fields ')
            for mfd in sorted(G4AtlasEngine.Dict_Fields.keys()):
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


    def _init_Simulation(self):
        """\
        Simulation engine initialization.

        Internal method which initializes the simulation engine. The simulation
        passes through several distinct levels at which user configuration may
        be done via the simFlags.InitFunctions dict, keyed on the appropriate
        hook point. The provided hook points are

          preInit - called before Athena initialize()
          pre/postInitG4 - called before/after the init_G4 method
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
            from G4AtlasApps.SimFlags import simFlags
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

        _run_init_stage("G4")

        if not self.useISF:
            _run_init_stage("MCTruth")
        else:
            G4AtlasEngine.log.debug('not initializing MCTruth in G4AtlasEngine because useISF=True')

        _run_init_stage("Fields")
        _run_init_stage("FieldIntegrationParameters")

        # Doesn't work with hive
        if not is_hive:
            if not self.useISF:
                _run_init_stage("RecEnvelope")
            else:
                G4AtlasEngine.log.debug('not initializing RecEnvelope in G4AtlasEngine because useISF=True')
        else:
            G4AtlasEngine.log.debug('not initializing RecEnvelope in G4AtlasEngine because this is a G4Hive job.')

        _run_init_stage("Graphics")

        self.init_status = "postInit"
        G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
        _run_init_callbacks(self.init_status)

        ## Check that all the callbacks were indeed called, and warn otherwise
        from G4AtlasApps.SimFlags import simFlags
        num_reg_callbacks = sum(len(cblist) for cblist in simFlags.InitFunctions.get_Value().values())
        if G4AtlasEngine._callback_counter != num_reg_callbacks:
            G4AtlasEngine.log.warning("G4AtlasEngine: mismatch in num of callbacks regd/called = %d/%d" %
                                      (num_reg_callbacks, G4AtlasEngine._callback_counter))

        G4AtlasEngine._app_profiler('_init_Simulation')


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
                cppyy.loadDict(dict_name)
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



#    class menu_UserActions:
#        """
#        User actions can be added using this menu.
#        """
#
#        @classmethod
#        def add_UserAction(cls, action_obj):
#            """Adds a UserAction object to the simulation."""
#            if isinstance(action_obj,UserAction):
#                G4AtlasEngine.log.debug('G4AtlasEngine: menu_UserActions:add_UserAction: ' +
#                                              action_obj.Name)
#                G4AtlasEngine.Dict_UserAction[action_obj.Name] = action_obj
#                action_obj._construct()
#            else:
#               G4AtlasEngine.log.error('G4AtlasEngine: menu_UserActions:add_UserAction: '+
#                                             'This is not a UserAction object!!!')
#
#        @classmethod
#        def list_Actions(cls):
#            """
#            Lists the possible actions.
#
#            The list is extracted from the user-action libraries already loaded.
#            """
#            G4AtlasEngine._ctrl.actMenu.actionList()
#
#
#        @classmethod
#        def load_Lib(cls, Library):
#            """Loads the library with the user-actions."""
#            G4AtlasEngine.load_Lib(Library)
#            G4AtlasEngine._ctrl.actMenu.actionList()
#


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
                from G4AtlasApps.SimFlags import simFlags
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
        different initialization phases of the G4AtlasEngine.
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
        G4AtlasEngine.load_Dict(self.LCGDictionaryName)
        if self.Name not in G4AtlasEngine.Dict_DetConfig:
            G4AtlasEngine.Dict_DetConfig[self.Name] = self
            self.build(self.Mode)
            self._Built = True
        else:
            G4AtlasEngine.log.info(' DetConfigurator: with name '+self.Name+ ' is already in the Dict_DetConfig: please use it!')


    def build(self,mode):
        """ You should re_write this method to decide which C++ configuration
            classes you want to make available to the users
        """
        G4AtlasEngine.log.warning(' DetConfigurator:_build: nothing is done!!!, you have to overwrite it!')



class MagneticField:
    """ Magnetic field.

    The Python object "MagneticField". It can be 'Constant' or
    magnetic field map 'MapField' type (by default 'Constant'),
    unless the last argument will be set to 'MapField'.
    """

    # TODO: change signature so that lib can have a default value (i.e. swap the order of lib & name)
    # TODO: reinstate stepper="StepperDispatcher"):
    def __init__(self, lib, name, volume="", typefield="Constant", stepper=None):
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
        else:
            raise ValueError(' PyG4Atlas.MagneticField not allowed field type: '+str(typefield))

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
            # Check if this is the main detector field or not
            from G4AtlasApps.SimFlags import simFlags
            main_field = hasattr(simFlags, 'MagneticField') and (not simFlags.MagneticField.statusOn or simFlags.MagneticField() in self.Name)

            if main_field: G4AtlasEngine._ctrl.fldMenu.select(self.Name)
            if main_field: G4AtlasEngine._ctrl.fldMenu.fieldmap(self.MapFieldFileName)
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
            if main_field: G4AtlasEngine._ctrl.fldMenu.initialize()
            G4AtlasEngine.log.debug(' MagneticField:_build: MapField ' + self.Name)

        ## Enable default stepper
        stepper = self.FieldStepper
        if stepper is None: stepper=G4AtlasEngine._ctrl.fldMenu.GetDefaultStepper()
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
               G4AtlasEngine.log.debug(' RecEnvelope:'+
                                             '_construct: '+self.Name+' and store at the exit of volume  '+i)
           self._Built = True
           G4AtlasEngine._app_profiler('  _build RecEnvelope: '+self.Name)



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
    def hits_persistency(cls):
        """ HITS POOL file persistency
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.DetFlags import DetFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

        ## Not yet understood, but need to treat StreamHITS as alg in Hive.
        ## Seems to also work fine outside of Hive, but to be extra safe I'm
        ## only changing it in Hive.
        as_alg = is_hive
        ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
        stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput(), asAlg=as_alg)

        ## Write geometry tag info - move to main method
        #import EventInfoMgt.EventInfoMgtInit

        ## EventInfo & TruthEvent always written by default
        stream1.ForceRead=True
        stream1.ItemList = ["EventInfo#*",
                            "McEventCollection#TruthEvent",
                            "JetCollection#*"]

        from PyJobTransforms.trfUtils import releaseIsOlderThan
        if releaseIsOlderThan(20,0):
            #Hack to maintain compatibility of G4AtlasApps trunk with
            #19.2.X.Y after EDM changes in release 20.0.0.
            stream1.ItemList += ["xAOD::JetContainer_v1#*",
                                 "xAOD::JetAuxContainer_v1#*"]
        else:
            stream1.ItemList += ["xAOD::JetContainer#*",
                                 "xAOD::JetAuxContainer#*"]

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
                                 #"SimpleScintillatorHitCollection#*",
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

        ## FwdRegion
        checkFwdRegion = getattr(DetFlags, 'FwdRegion_on', None) #back-compatibility
        if checkFwdRegion is not None: #back-compatibility
            if checkFwdRegion():
                stream1.ItemList += ["SimulationHitCollection#*"]

        ## ZDC
        if DetFlags.ZDC_on():
            stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                 "ZDC_SimStripHit_Collection#*"]
        ## ALFA
        if DetFlags.ALFA_on():
            stream1.ItemList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*"]

        ## AFP
        checkAFP = getattr(DetFlags, 'AFP_on', None) #back-compatibility
        if checkAFP is not None: #back-compatibility
            if checkAFP():
                stream1.ItemList += ["AFP_TDSimHitCollection#*","AFP_SIDSimHitCollection#*"]

        ## Ancillary scintillators
        #stream1.ItemList += ["ScintillatorHitCollection#*"]

        ## TimingAlg
        stream1.ItemList +=["RecoTimingObj#EVNTtoHITS_timings"]

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


    @classmethod
    def evgen_persistency(cls):
        """ EVGEN POOL file persistency
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
        ## NB. Two-arg constructor is needed, since otherwise metadata writing fails!
        if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
            stream2 = AthenaPoolOutputStream("StreamEVGEN", simFlags.WriteTR.get_Value())
            stream2.ItemList += ["IOVMetaDataContainer#*"]
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


    @classmethod
    def will_write_output_files(cls):
        """ Check if any POOL files will be written by this job
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.PoolHitsOutput.statusOn:
            return True
        elif ("tb" not in simFlags.SimLayout.get_Value()):
            if hasattr(simFlags, "WriteTR") and simFlags.WriteTR.statusOn:
                return True
            elif hasattr(simFlags,'StoppedParticleFile') and simFlags.StoppedParticleFile.statusOn:
                return True
        return False


    @classmethod
    def do_run_number_modifications(cls):
        """ Set the run number in the simulation.  In order of priority, use:
             - The RunNumber flag
             - The input file run number """

        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        import os
        G4AtlasEngine.log.verbose('SimSkeleton.do_run_number_modifications :: starting')

        myRunNumber = 1
        myFirstLB = 1
        myInitialTimeStamp = 0
        if hasattr(simFlags, "RunNumber") and simFlags.RunNumber.statusOn:
            myRunNumber = simFlags.RunNumber.get_Value()
            G4AtlasEngine.log.info('SimSkeleton.do_run_number_modifications :: Found run number %d in sim flags.', myRunNumber)
            ## Set event selector details based on evgen metadata

            ######update the run/event info for each event
            if not hasattr(svcMgr,'EvtIdModifierSvc'):
                import AthenaServices.Configurables as asc
                svcMgr +=asc.EvtIdModifierSvc(EvtStoreName="StoreGateSvc")
                from AthenaCommon.AppMgr import theApp
                theApp.CreateSvc += ["EvtIdModifierSvc"]
            else:
                G4AtlasEngine.log.warning('SimSkeleton.do_run_number_modifications :: Will override the settings of the EvtIdModifierSvc that was previously set up!')
            #fix iov metadata
            if not hasattr(svcMgr.ToolSvc, 'IOVDbMetaDataTool'):
                from AthenaCommon import CfgMgr
                svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()
            svcMgr.ToolSvc.IOVDbMetaDataTool.MinMaxRunNumbers = [myRunNumber, 2147483647]
            ## FIXME need to use maxRunNumber = 2147483647 for now to keep overlay working but in the future this should be set properly.

            # Using event numbers to avoid "some very large number" setting
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            totalNumber = 1000000
            if athenaCommonFlags.EvtMax() is not None and athenaCommonFlags.EvtMax()>0: totalNumber = athenaCommonFlags.EvtMax()+1
            if athenaCommonFlags.SkipEvents() is not None and athenaCommonFlags.SkipEvents()>0: totalNumber += athenaCommonFlags.SkipEvents()
            try:
                from RunDependentSimComps.RunDMCFlags import runDMCFlags
                myInitialTimeStamp = runDMCFlags.RunToTimestampDict.getTimestampForRun(myRunNumber)
                #print "FOUND TIMESTAMP ", str(myInitialTimeStamp)
            except:
                myInitialTimeStamp = 1
            svcMgr.EvtIdModifierSvc.add_modifier(run_nbr=myRunNumber, lbk_nbr=myFirstLB, time_stamp=myInitialTimeStamp, nevts=totalNumber)
            if hasattr(svcMgr.EventSelector,'OverrideRunNumberFromInput'): svcMgr.EventSelector.OverrideRunNumberFromInput = True
        elif 'G4ATLAS_SKIPFILEPEEK' in os.environ and os.environ['G4ATLAS_SKIPFILEPEEK']:
            G4AtlasEngine.log.warning('SimSkeleton.do_run_number_modifications\
            :: Requires simFlags.RunNUmber to be specified in this running mode.\
            Using default value of 1 for RunNumber.')
        elif athenaCommonFlags.PoolEvgenInput.statusOn:
            ## Read input file metadata
            import PyUtils.AthFile as af
            hitfile = athenaCommonFlags.PoolEvgenInput.get_Value()[0]
            f = af.fopen(hitfile)

            ## Get evgen run number and lumi block
            if len(f.run_numbers) > 0:
                myRunNumber = f.run_numbers[0]
                G4AtlasEngine.log.info('SimSkeleton.do_run_number_modifications :: Found run number %d in hits file metadata.', myRunNumber)
            else:
                G4AtlasEngine.log.warning("SimSkeleton.do_run_number_modifications :: Failed to find run number in hits file metadata.")
            if f.lumi_block:
                myFirstLB = f.lumi_block[0]
        else:
            G4AtlasEngine.log.warning('SimSkeleton.do_run_number_modifications\
            :: Require at least one of the the following to be\
            specified: input file or simFlags.RunNumber to be\
            specified. Using default value of 1 for RunNumber.')

        svcMgr.EventSelector.OverrideRunNumber = True
        svcMgr.EventSelector.RunNumber = myRunNumber
        svcMgr.EventSelector.FirstLB = myFirstLB
        # Necessary to avoid a crash
        svcMgr.EventSelector.InitialTimeStamp = myInitialTimeStamp

    @classmethod
    def _do_persistency(cls):
        """ Place to handle the persistency.
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if cls.will_write_output_files():
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

            ## Write geometry tag info
            import EventInfoMgt.EventInfoMgtInit

            ## Instantiate StreamHITS
            if athenaCommonFlags.PoolHitsOutput.statusOn:
                cls.hits_persistency()

            ## AthenaPool converters
            # TODO: Remove? Indentation?
            theApp.Dlls += [ "GeneratorObjectsAthenaPoolPoolCnv"]

            ## StreamEVGEN: needed for cosmic simulations and cavern BG
            ## Separate stream of track record (TR) info -- it does not apply to the CTB simulations.
            # TODO: Can this be merged into the cosmics sec above, or do the AthenaPool includes *need* to be in-between?
            if "tb" not in simFlags.SimLayout.get_Value():
                cls.evgen_persistency()

            G4AtlasEngine.log.verbose('SimSkeleton._do_persistency :: done')
        else:
            G4AtlasEngine.log.info('SimSkeleton._do_persistency :: nothing done')

        # Check on run numbers and update them if necessary
        cls.do_run_number_modifications()


    @classmethod
    def _do_readevgen(cls):
        """ Place to read evgen events.
        """
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        G4AtlasEngine.log.verbose('SimSkeleton._do_readevgen :: starting')

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        ## ReadTR is only present in simFlags for ATLAS geometries with cosmics switched on
        if (not simFlags.ISFRun) and hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn:
            include("CosmicGenerator/SetCosmicGenerator.py")

        if athenaCommonFlags.PoolEvgenInput.statusOn:
            ## Tell the event selector about the evgen input files and event skipping
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaPoolCnvSvc.ReadAthenaPool
            svcMgr.EventSelector.InputCollections = athenaCommonFlags.PoolEvgenInput()
            if athenaCommonFlags.SkipEvents.statusOn:
                svcMgr.EventSelector.SkipEvents = athenaCommonFlags.SkipEvents()

            import os
            if 'G4ATLAS_SKIPFILEPEEK' in os.environ and os.environ['G4ATLAS_SKIPFILEPEEK']:
                G4AtlasEngine.log.info('SimSkeleton._do_readevgen: Skipping IS_SIMULATION check')
            else:
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

        else:
            ## No input file so assume that we are running a Generator in the same job
            if not hasattr(svcMgr, 'EventSelector'):
                import AthenaCommon.AtlasUnixGeneratorJob
            # TODO: Check that there is at least one algorithm already in the AlgSequence?
            ## Warn if attempting to skip events in a generator job
            if athenaCommonFlags.SkipEvents.statusOn and athenaCommonFlags.SkipEvents()!=0:
                msg = "SimSkeleton._do_readevgen :: athenaCommonFlags.SkipEvents set in a job without an active "
                msg += "athenaCommonFlags.PoolEvgenInput flag: ignoring event skip request"
                G4AtlasEngine.log.warning(msg)

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
            from AthenaCommon import CfgGetter
            #FIXME move to CfgGetter method
            CfgGetter.getPublicTool('AthenaStackingAction',tryDefaultConfigurable=True).KillAllNeutrinos = True

        if hasattr(simFlags, 'ReleaseGeoModel') and simFlags.ReleaseGeoModel.statusOn:
            ## Save the fast simulation
            job.G4AtlasAlg.ReleaseGeoModel = simFlags.ReleaseGeoModel.get_Value()

        if hasattr(simFlags, 'RecordFlux') and simFlags.RecordFlux.statusOn:
            ## Record the particle flux during the simulation
            job.G4AtlasAlg.RecordFlux = simFlags.RecordFlux.get_Value()

        if hasattr(simFlags, 'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn:
            ## Propagate quasi-stable particles
            job.G4AtlasAlg.IncludeParentsInG4Event = simFlags.IncludeParentsInG4Event.get_Value()

        if hasattr(simFlags, 'KillAbortedEvents') and simFlags.KillAbortedEvents.statusOn:
            ## default true
            job.G4AtlasAlg.KillAbortedEvents = simFlags.KillAbortedEvents.get_Value()

        if hasattr(simFlags, 'FlagAbortedEvents') and simFlags.FlagAbortedEvents.statusOn:
            ## default false
            job.G4AtlasAlg.FlagAbortedEvents = simFlags.FlagAbortedEvents.get_Value()
            if simFlags.FlagAbortedEvents.get_Value() and simFlags.KillAbortedEvents.get_Value():
                G4AtlasEngine.log.warning('When G4AtlasAlg.FlagAbortedEvents is True G4AtlasAlg.KillAbortedEvents should be False!!! Setting G4AtlasAlg.KillAbortedEvents = False now!')
                job.G4AtlasAlg.KillAbortedEvents = False

        if hasattr(simFlags, 'RandomSvc') and simFlags.RandomSvc.statusOn:
            ## default true
            job.G4AtlasAlg.AtRndmGenSvc = simFlags.RandomSvc.get_Value()

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
    def do_UserActions(cls):
        """ Place to define default user actions.

           In most of the cases this will be empty unless a well
         defined user action will be applied in all the jobs for a
         given simulation entity.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_UserActions :: nothing done')


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
        G4AtlasEngine.log.verbose('SimSkeleton.__checks :: done')


    @classmethod
    def _do_PreInit(cls):
        """
        Execute all the do_something methods in the phase before Athena initialization.

        Do not overload this method.
        """
        G4AtlasEngine.log.verbose('SimSkeleton._do_PreInit :: starting')

        # use some different methods for ISF and G4 standalone run
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.ISFRun:
          known_methods = ['_do_jobproperties', '_do_external', '_do_metadata',
                           '_do_MagField', '_do_FieldIntegrationParameters','do_UserActions']
        else:
          known_methods = ['_do_jobproperties', '_do_external', '_do_metadata',
                           '_do_readevgen', '_do_persistency', '_do_G4AtlasAlg','do_UserActions',
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

        # Add core services
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr(ServiceMgr.ToolSvc, "SensitiveDetectorMasterTool"):
            from AthenaCommon.CfgGetter import getPublicTool
            sensitiveDetectorTool = getPublicTool("SensitiveDetectorMasterTool")
        if not hasattr(ServiceMgr.ToolSvc, "FastSimulationMasterTool"):
            from AthenaCommon.CfgGetter import getPublicTool
            fastSimulationTool = getPublicTool("FastSimulationMasterTool")

        # Add User actions
        if not hasattr(ServiceMgr, "UserActionSvc"):
            from AthenaCommon.CfgGetter import getService
            userActionSvc = getService("UserActionSvc")

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
        known_methods = ['do_GeoSD', 'do_MCtruth']
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
            # user actions need to be called at preinit
            if i.find('do_') == 0 and i not in known_methods and i!='do_UserActions':
               try:
                   G4AtlasEngine.log.debug('SimSkeleton :: evaluating method %s' % i)
                   getattr(cls, i).__call__()
               except Exception, err:
                   print "Error: %s" % str(err)
                   import traceback, sys
                   traceback.print_exc(file=sys.stdout)
                   raise RuntimeError('SimSkeleton: found problems with the method %s' % i)
        cls.__checks()
        G4AtlasEngine.log.verbose('SimSkeleton._do_All :: done')



## In Hive, cannot yet use a python alg due to python GIL.
## So, we use a service for now instead.
from AthenaPython import PyAthena
if is_hive:
    PyG4Atlas_base = PyAthena.Svc
    PyG4Atlas_name = 'PyG4AtlasSvc'
else:
    PyG4Atlas_base = PyAthena.Alg
    PyG4Atlas_name = 'PyG4AtlasAlg'
## Python algorithm to set up the Python side of the G4 sim
## For now, the type of this component depends on the environment.
class _PyG4AtlasComp(PyG4Atlas_base):
    """
    PyAthena class which runs the ATLAS simulation.
    """

    def __init__(self, name=PyG4Atlas_name, sim_module=None):
        """
        name                  --> must be 'PyG4AtlasAlg' (non-hive) or 'PyG4AtlasSvc' (hive)
        sim_module            --> name of a Python module which describes the simulation

        NB. If sim_module is an empty string or None, the sim skeleton contained
        in G4AtlasEng.G4Eng.Dict() will be run.
        """
        super(_PyG4AtlasComp, self).__init__(name=name)
        self.doFirstEventG4SeedsCheck = True #FIXME - remove for G4Hive?
        self.sim_module = sim_module

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
            AtlasG4Eng.G4Eng.log.error('No sim skeleton registered by time of %s construction: STOP!!' % self.name())
            raise ValueError('Unknown sim setup: STOP')

        ## Import sim module if requested
        # TODO: is this ever used?
        if self.sim_module:
            G4AtlasEngine.log.info("The kernel simulation Python module which describes the simulation is: %s" % self.sim_module)
            try:
                __import__(self.sim_module, globals(), locals())
            except:
                G4AtlasEngine.log.fatal("The kernel simulation Python module '%s' was not found!" % self.sim_module)
                raise RuntimeError('PyG4Atlas: %s: initialize()' % self.name())

        ## Call the sim skeleton pre-init method
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_PreInit()

        ## Start profiler
        AtlasG4Eng.G4Eng._app_profiler('PyG4AtlasAlg before initialize')


    def initialize(self):
        import AtlasG4Eng
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('%s starting at (UTC): %s' % (self.name(), timestr))
        AtlasG4Eng.G4Eng._app_profiler('%s begin of initialize' % self.name())
        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_All()
        AtlasG4Eng.G4Eng._init_Simulation()

        from G4AtlasApps.SimFlags import simFlags
        if simFlags.ISFRun:
          # TODO: does this 'HACK' need to be fixed at some point?
          # *AS* HACK, as "G4AtlasControl/SimControl.cxx" fails dynamic cast
          # see also G4AtlasRunManager
          AtlasG4Eng.G4Eng.gbl.G4Commands().run.verbose(2)
          AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
          #AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)

        AtlasG4Eng.G4Eng._app_profiler('%s end of initialize' % self.name())
        if "atlas_flags" in simFlags.extra_flags:
            beamcondsvc = PyAthena.py_svc('BeamCondSvc/BeamCondSvc', createIf=True, iface=cppyy.gbl.IBeamCondSvc)
            assert(type(beamcondsvc) ==  cppyy.gbl.IBeamCondSvc)

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
            theApp.EvtMax = athenaCommonFlags.EvtMax()
        return True


    def finalize(self):
        import AtlasG4Eng
        AtlasG4Eng.G4Eng._app_profiler('%s at finalize  ' % self.name())
        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4AtlasEngine.log.info('%s ending at (UTC): %s'  % (self.name(), timestr))
        return True

if is_hive:
    PyG4AtlasSvc = _PyG4AtlasComp
else:
    PyG4AtlasAlg = _PyG4AtlasComp
