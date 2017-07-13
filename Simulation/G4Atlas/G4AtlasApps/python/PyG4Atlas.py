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


from AthenaCommon.ConcurrencyFlags import jobproperties as concurrencyProps
if concurrencyProps.ConcurrencyFlags.NumThreads() > 0:
    is_hive = True
else:
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
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        G4AtlasEngine.log.setLevel(10*(svcMgr.MessageSvc.OutputLevel - 1))

        # The profiler will work if log level = debug
        G4AtlasEngine._app_profiler = _app_profiler(G4AtlasEngine.log.level)

        # pylcgdict default dictionaries
        self.load_Dict('AtlasSealCLHEPDict')
        self.load_Dict('G4AtlasControlDict')
        G4AtlasEngine.gbl = cppyy.makeNamespace("")
        G4AtlasEngine._ctrl = G4AtlasEngine.gbl.SimControl()
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
            #G4AtlasEngine._ctrl.initializeG4(is_hive)
            G4AtlasEngine.log.info(' G4AtlasEngine: _init_G4: init Geant4 ')
            self._InitList.append('init_G4')
            G4AtlasEngine._app_profiler('_init_G4: ')
        else:
            G4AtlasEngine.log.warning('G4AtlasEngine: init_G4 is already done')


    def _init_Simulation(self):
        """\
        Simulation engine initialization.

        Internal method which initializes the simulation engine. The simulation
        passes through several distinct levels at which user configuration may
        be done via the simFlags.InitFunctions dict, keyed on the appropriate
        hook point. The provided hook points are

          preInit - called before Athena initialize()
          pre/postInitG4 - called before/after the init_G4 method
          pre/postInitFields - called before/after the init_Fields method
          postInit - called after all sim engine initialisation methods

        The current init level is stored in G4AtlasEngine.init_status, and its
        name corresponds to the active hook at that point.
        """
        from G4AtlasApps.SimFlags import simFlags

        def _run_init_callbacks(init_level):
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

        self.init_status = "postInit"
        G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + self.init_status)
        _run_init_callbacks(self.init_status)

        ## Check that all the callbacks were indeed called, and warn otherwise
        num_reg_callbacks = sum(len(cblist) for cblist in simFlags.InitFunctions.get_Value().values())
        if G4AtlasEngine._callback_counter != num_reg_callbacks:
            G4AtlasEngine.log.warning("G4AtlasEngine: mismatch in num of callbacks regd/called = %d/%d" %
                                      (num_reg_callbacks, G4AtlasEngine._callback_counter))

        G4AtlasEngine._app_profiler('_init_Simulation')


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


    def print_Summary(self):
        """ Prints the summary
        """
        print self.Name
        print self.List_LoadedLib



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
                self.FilterStatusOn = {
                    'EtaPhiFilters' : False,
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
            - VertexRangeChecker
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
        stream1 = AthenaPoolOutputStream("StreamHITS", athenaCommonFlags.PoolHitsOutput(), asAlg=as_alg, noTag=True)

        ## Write geometry tag info - move to main method
        #import EventInfoMgt.EventInfoMgtInit

        ## EventInfo & TruthEvent always written by default
        stream1.ForceRead=True
        stream1.ItemList = ["EventInfo#*",
                            "McEventCollection#TruthEvent",
                            "JetCollection#*"]

        ## If we are running quasi-stable particle simulation, include the original event record
        if hasattr(simFlags,'IncludeParentsInG4Event') and simFlags.IncludeParentsInG4Event.statusOn and simFlags.IncludeParentsInG4Event():
            stream1.ItemList += ["McEventCollection#GEN_EVENT"]

        from PyJobTransforms.trfUtils import releaseIsOlderThan
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
        if DetFlags.FwdRegion_on():
            stream1.ItemList += ["SimulationHitCollection#*"]

        ## ZDC
        if DetFlags.ZDC_on():
            stream1.ItemList += ["ZDC_SimPixelHit_Collection#*",
                                 "ZDC_SimStripHit_Collection#*"]
        ## ALFA
        if DetFlags.ALFA_on():
            stream1.ItemList += ["ALFA_HitCollection#*",
                                 "ALFA_ODHitCollection#*"]

        ## AFP
        if DetFlags.AFP_on():
            stream1.ItemList += ["AFP_TDSimHitCollection#*",
                                 "AFP_SIDSimHitCollection#*"]

        ### Ancillary scintillators
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

        G4AtlasEngine.log.verbose('SimSkeleton.do_run_number_modifications :: starting')
        from G4AtlasApps.G4Atlas_Metadata import configureRunNumberOverrides
        configureRunNumberOverrides()
        G4AtlasEngine.log.verbose('SimSkeleton.do_run_number_modifications :: done')


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
            # Recommendations from Peter vG 16.08.25
            svcMgr.AthenaPoolCnvSvc.PoolAttributes += [ "DatabaseName = '" + athenaCommonFlags.PoolHitsOutput() + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'" ]

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
            from G4AtlasApps.G4Atlas_Metadata import inputFileValidityCheck
            inputFileValidityCheck()
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
        if not hasattr(job, 'G4AtlasAlg'):
            from AthenaCommon import CfgGetter
            job += CfgGetter.getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
        G4AtlasEngine.log.verbose('SimSkeleton._doG4AtlasAlg :: done')


    @classmethod
    def do_GeoSD(cls):
        """ Place to define the geometry and sensitive detectors.
        """
        G4AtlasEngine.log.info('SimSkeleton.do_GeoSD :: nothing done')


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
                           'do_UserActions']
        else:
          known_methods = ['_do_jobproperties', '_do_external', '_do_metadata',
                           '_do_readevgen', '_do_persistency', '_do_G4AtlasAlg',
                           'do_UserActions']

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

        ## Run pre-init callbacks
        G4AtlasEngine.log.debug("G4AtlasEngine:init stage " + "preInit")
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
        known_methods = ['do_GeoSD']
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
