# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.DetFlags import DetFlags
from ISF_Geant4Config.ISF_SimFlags import simFlags
from ISF_Geant4Config import AtlasG4Eng
from ISF_Geant4Config.iGeant4 import G4Eng
from AthenaCommon.Include import include

class ISF_G4ConfigTool(PyAthena.AlgTool):
    """
    Python algorithm which configures the ATLAS simulation.
    """

    def __init__(self, name="ISF_G4ConfigTool", **kw):
        """
        This Athena Python AlgTool configures the ISF-Geant4 simulation
        """
        kw['name'] = name
        super(ISF_G4ConfigTool,self).__init__(**kw)

        # the G4 Engine
        #G4Eng  = kw.get('G4Eng',  None)

        # initialization
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        from AthenaCommon.GlobalFlags import globalflags
        simFlags.SimLayout = globalflags.DetDescrVersion.get_Value()

        #
        #
        # all following lines are pasted from ISF_PyG4Atlas.py __init__ method:
        #
        #
        self.doFirstEventG4SeedsCheck = True
        self.RndG4Menu = None

        G4Eng.Name = "ATLAS_G4Sim"
        G4Eng.log.info('Configuring ATLAS simulations')
        self._do_jobproperties( G4Eng )
        self._do_external( G4Eng )
        #self._do_metadata( G4Eng )
        #self._do_readevgen( G4Eng )
        #self._do_persistency( G4Eng )
        self._do_MagField( G4Eng )
        #self._do_FieldIntegrationParameters( G4Eng )

        G4Eng.log.debug("G4AtlasEngine:init stage " + "preInit")
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        if simFlags.InitFunctions.statusOn and "preInit" in simFlags.InitFunctions.get_Value():
            for callback_fn in simFlags.InitFunctions.get_Value()["preInit"]:
                    callback_fn.__call__()
                    G4Eng._callback_counter += 1

     
        ## Start profiler
        G4Eng._app_profiler('ISF_PyG4AtlasAlg before initialize')

    def initialize(self):
#        from G4AtlasApps import AtlasG4Eng
#
#        from time import gmtime, strftime
#        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
#        G4AtlasEngine.log.info('ISF_PyG4AtlasAlg starting at (UTC): %s' % timestr)
#        AtlasG4Eng.G4Eng._app_profiler('ISF_PyG4AtlasAlg begin of initialize')
#        AtlasG4Eng.G4Eng.Dict['simu_skeleton']._do_All()
#        AtlasG4Eng.G4Eng._init_Simulation()
#
#        # *AS* HACK, as "G4AtlasControl/SimControl.cxx" fails dynamic cast
#        # see also G4AtlasRunManager
#        AtlasG4Eng.G4Eng.gbl.G4Commands().run.verbose(2)
#        AtlasG4Eng.G4Eng._ctrl.G4Command("/run/initialize")
#        #AtlasG4Eng.G4Eng.gbl.G4Commands().tracking.verbose(1)
#       
#        AtlasG4Eng.G4Eng._app_profiler('ISF_PyG4AtlasAlg end of initialize')
#        self.RndG4Menu = AtlasG4Eng.G4Eng.menu_G4RandomNrMenu()
#        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
#            theApp.EvtMax = athenaCommonFlags.EvtMax()
#        return True
        self.msg.info('==> initialize...')
        if G4Eng is None:
          self.msg.error ('no Geant4 Python Engine given')
          return StatusCode.Failure

        from time import gmtime, strftime
        timestr = strftime("%a, %d %b %Y %H:%M:%S +0000", gmtime())
        G4Eng.log.info('ISF_G4ConfigTool starting at (UTC): %s' % timestr)
        G4Eng._app_profiler('ISF_G4ConfigTool begin of initialize')
        self.do_GeoSD( G4Eng )
        #self.do_MCtruth()
        self.do_UserActions( G4Eng )
        self.do_Physics( G4Eng )
        self._do_DefaultUserActions( G4Eng )
        self.__checks( G4Eng )

        G4Eng._init_Simulation()
      
        # *AS* HACK, as "G4AtlasControl/SimControl.cxx" fails dynamic cast
        # see also G4AtlasRunManager
        G4Eng.gbl.G4Commands().run.verbose(2)
        G4Eng._ctrl.G4Command("/run/initialize")
        #G4Eng.gbl.G4Commands().tracking.verbose(1)
        
        G4Eng._app_profiler('ISF_PyG4AtlasAlg end of initialize')
        self.RndG4Menu = G4Eng.menu_G4RandomNrMenu()
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from AthenaCommon.AppMgr import theApp   
        if athenaCommonFlags.EvtMax.statusOn and theApp.EvtMax == -1:
            theApp.EvtMax = athenaCommonFlags.EvtMax()

        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...')
        return StatusCode.Success


    @classmethod
    def do_GeoSD(self, G4Eng):
        
        """
         Configure the geometry and SD for ISF
        """
        
        print "in ISF_G4ConfigTool do_GeoSD"
        
        ## Extra materials
        from ISF_Geant4Config import atlas_materials
        ## Different geometry-levels for rec envelopes depending into simFlags.SimLayout
        recenv_level = 2

        ## Add cavern for cosmics layouts and cavern bg sim
        from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import AtlasEnvelope

        atlasenv = AtlasEnvelope( G4Eng )
        atlas = atlasenv.atlas
        if jobproperties.Beam.beamType() == 'cosmics' or \
           (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            ## Put in place cavern description for the two setups
            from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import CosmicShortCut, AtlasCavernGeoModel
            cavern = AtlasCavernGeoModel()
            cavern.DeclarePhysReg()
            G4Eng.add_DetFacility(cavern.world, cavern.world)
            G4Eng.add_DetFacility(cavern.point1, cavern.world)
            G4Eng.add_DetFacility(atlas, cavern.world)
            if jobproperties.Beam.beamType() == 'cosmics' and hasattr(simFlags, "ReadTR"):
                cosmicSC = CosmicShortCut(not simFlags.ReadTR.statusOn)
                G4Eng.add_DetFacility(cosmicSC.ttr_barrel, atlas)
            recenv_level = 3
            #atlas.df.RotateX(0.013196326794)
        else:
            G4Eng.add_DetFacility(atlas, atlas)

        ## Get a handle to the RecEnv menu
        menuRecordEnvelopes = G4Eng.menu_RecordEnvelope()
        from ISF_Geant4Config.atlas_mctruth import RecordingEnvelopes

        allowMods=True 

        ## Inner detector
        if DetFlags.ID_on():
            idet = G4Eng.Dict_DetFacility["IDET"]
            rc = RecordingEnvelopes.AtlasCaloEntryLayer(recenv_level,allowMods)
            menuRecordEnvelopes.add_RecEnvelope(rc.recenv)
            if DetFlags.geometry.pixel_on():
                #from ISF_atlas_idet import Pixel
                from ISF_Geant4Config.atlas_idet import Pixel
                pixel = Pixel(allowMods)
                pixel._initSD()
                pixel._initPR()
                G4Eng.add_DetFacility(pixel.atlas_pixel, idet)
            if DetFlags.geometry.SCT_on():
                from ISF_Geant4Config.atlas_idet import SCT
                sct = SCT(allowMods)
                sct._initSD()
                sct._initPR()
                G4Eng.add_DetFacility(sct.atlas_sct, idet)
            if DetFlags.geometry.TRT_on():
                from ISF_Geant4Config.atlas_idet import TRT
                trt = TRT(allowMods)
                trt._initSD()
                trt._initPR()
                G4Eng.add_DetFacility(trt.atlas_trt, idet)
                trt._initTRTProcess()

            ## Beam pipe
            if DetFlags.bpipe_on():
                from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import BeamPipe
                atlasbeampipe = BeamPipe(G4Eng)
                atlasbeampipe._initPR()
                G4Eng.add_DetFacility(atlasbeampipe.beampipe, atlas)

            from ISF_Geant4Config.atlas_idet import IDetServicesMat
            idetmat = IDetServicesMat()
            G4Eng.add_DetFacility(idetmat.atlas_idetServMat, idet)

        ## Calorimeters
        if DetFlags.Calo_on():
            calo = G4Eng.Dict_DetFacility["CALO"]
            rc = RecordingEnvelopes.AtlasMuonEntryLayer(recenv_level,allowMods)
            menuRecordEnvelopes.add_RecEnvelope(rc.recenv)

            ## LAr
            if DetFlags.geometry.LAr_on():
                from ISF_Geant4Config.atlas_calo import PyLArG4RunControler
                lArG4RunControl = PyLArG4RunControler('PyLArG4RunControl', 'LArG4RunControlDict',allowMods=allowMods)
                from ISF_Geant4Config.atlas_calo import LAr
                LArDetector = LAr(allowMods)
                ## Shower parameterization overrides the calibration hit flag
                if simFlags.LArParameterization.statusOn and simFlags.LArParameterization() > 0 \
                   and simFlags.CalibrationRun.statusOn and simFlags.CalibrationRun.get_Value() in ['LAr','LAr+Tile','DeadLAr']:
                    G4Eng.log.fatal('SimSkeleton: You requested both calibration hits and frozen showers / parameterization in the LAr.')
                    G4Eng.log.fatal('  Such a configuration is not allowed, and would give junk calibration hits where the showers are modified.')
                    G4Eng.log.fatal('  Please try again with a different value of simFlags.LArParameterization or simFlags.CalibrationRun ')
                    raise RuntimeError('Configuration not allowed')
                if simFlags.LArParameterization() > 0:
                    LArDetector._initSD(0)
                    LArDetector._initParameterization()
                    G4Eng.log.info('SimSkeleton: you chose to activate shower parameterization to level %d', simFlags.LArParameterization())
                    G4Eng.log.warning('SimSkeleton: you chose to activate shower parameterization: switching off ALL calibration hits')
                elif simFlags.LArParameterization() is None or simFlags.LArParameterization() == 0:
                ## As the default we select always no-parametrization
                    if simFlags.CalibrationRun.statusOn:
                        if simFlags.CalibrationRun.get_Value() in ['LAr', 'LAr+Tile']:
                            LArDetector._initSD(1)
                        elif simFlags.CalibrationRun.get_Value() == 'DeadLAr':
                            LArDetector._initSD(2)
                    else:
                        LArDetector._initSD(0)
                    LArDetector._initPR()
                G4Eng.add_DetFacility(LArDetector.atlas_lar, calo)

            ## Tile
            if DetFlags.geometry.Tile_on():
                from ISF_Geant4Config.atlas_calo import PyTileSimUtils
                pyTileSimUtilsOptions = PyTileSimUtils('PyTileSimUtils', 'TileSimUtilsDict', 'standard')
                ## Cosmic setup
                if jobproperties.Beam.beamType() == 'cosmics' or (hasattr(simFlags, "ReadTR") and simFlags.ReadTR.statusOn):
                    pyTileSimUtilsOptions.TileG4SimOptions.SetDeltaTHit(1)
                    pyTileSimUtilsOptions.TileG4SimOptions.SetDoTOFCorrection(False)
                    if G4Eng.log.level <= G4Eng.log.info:
                        pyTileSimUtilsOptions.TileG4SimOptions.printMe()
                from ISF_Geant4Config.atlas_calo import Tile
                tile = Tile(allowMods)
                if simFlags.CalibrationRun.statusOn and (simFlags.CalibrationRun.get_Value() in ['Tile', 'LAr+Tile']):
                    tile._initSD(1)
                else:
                    tile._initSD(0)
                G4Eng.add_DetFacility(tile.atlas_tile, calo)

        ## LUCID
        if DetFlags.geometry.Lucid_on():
            from ISF_Geant4Config.atlas_forward import Lucid
            atlaslucid = Lucid()
            atlaslucid._initSD()
            G4Eng.add_DetFacility(atlaslucid.lucid, atlas)
            atlaslucid._initOpProcess()

        if simFlags.ForwardDetectors.statusOn:
            from ISF_Geant4Config.atlas_forward import ForwardRegion
            atlasFwdRegion = ForwardRegion()
            fwdRegion = atlasFwdRegion.atlas_ForwardRegion
            G4Eng.add_DetFacility(fwdRegion, atlas)

            ## ZDC
            if DetFlags.geometry.ZDC_on():
                from ISF_Geant4Config.atlas_forward import ZDC
                atlasZDC = ZDC()
                atlasZDC._initSD()
                G4Eng.add_DetFacility(atlasZDC.atlas_ZDC, fwdRegion)

            ## ALFA
            if DetFlags.geometry.ALFA_on():
                from ISF_Geant4Config.atlas_forward import ALFA
                atlasalfa = ALFA()
                atlasalfa._initSD()
                G4Eng.add_DetFacility(atlasalfa.alfa, fwdRegion)

        ## Muon system
        if DetFlags.geometry.Muon_on():
            muonq02 = G4Eng.Dict_DetFacility["MUONQ02"]
            #G4Eng.add_DetFacility(muonq02, atlas)
            from ISF_Geant4Config.atlas_muon import Muon
            muon = Muon(allowMods)
            muon._initSD()
            muon._initPR()
            G4Eng.add_DetFacility(muon.atlas_muon, muonq02)
            rc = RecordingEnvelopes.AtlasMuonExitLayer(recenv_level,allowMods)
            menuRecordEnvelopes.add_RecEnvelope(rc.recenv)


    @classmethod
    def do_UserActions(cls, G4Eng):
        """
        User actions: TrackProcessor
        """
 
        print "in ISF_AtlasSimSkeleton::do_UserActions"
        from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
 
        # TODO: generate ToolSvc.ISFG4TransportTool.ISFG4TrackProcessorUserAction string automatically (eg via parameters)
        from ISF_Geant4Config.UserAction import UserAction
        ISF_G4PhysicsValidation = UserAction('ISF_Geant4Tools','ToolSvc.ISFG4PhysicsValidationUserAction',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
 
        ISF_G4TrackProcessor = UserAction('ISF_Geant4Tools','ToolSvc.ISFG4TrackProcessorUserAction',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
 
        ISF_SDActivateUserAction = UserAction('ISF_Geant4Tools','ToolSvc.SDActivateUserAction',['PreUserTrackingAction','PostUserTrackingAction'])

        ISF_MCTruthUserAction = UserAction('ISF_Geant4Tools','ToolSvc.ISFMCTruthUserAction',['Step','PreUserTrackingAction'])
       
        actions = G4Eng.menu_UserActions()
        print 'ISF_AtlasSimSkeleton::do_UserActions add ISF_G4PhysicsValidation'
        actions.add_UserAction(ISF_G4PhysicsValidation)
        print 'ISF_AtlasSimSkeleton::do_UserActions add ISF_G4TrackProcessor'
        actions.add_UserAction(ISF_G4TrackProcessor)
        print 'ISF_AtlasSimSkeleton::do_UserActions add SDActivateUserAction'
        actions.add_UserAction(ISF_SDActivateUserAction)
        print 'ISF_AtlasSimSkeleton::do_UserActions add ISF_MCTruthUserAction'
        actions.add_UserAction(ISF_MCTruthUserAction)

        exit(3)

    @classmethod
    def do_Physics(self, G4Eng):
        """
           Physics List from the simFlags.PhysicsList flag
           with default cut of 1 mm.
        """
        G4Eng.log.verbose('AtlasSimSkeleton._do_Physics :: starting')
        ## ATLAS physics list
        from ISF_Geant4Config.PhysicsList import PhysicsList
        atlasPhysics = PhysicsList(simFlags.PhysicsList.get_Value(), 1)
        G4Eng.add_PhysicsList(atlasPhysics)
        G4Eng.log.verbose('AtlasSimSkeleton._do_Physics :: done')


    @classmethod
    def __checks(cls, G4Eng):
        """
        Internals.
 
        Do not re-write this method. Here it is checked if the
        user has defined a world volume and a physics list. If
        this is not the case a default world_volume is created
        and the QGSP_BERT is selected.
        """
        G4Eng.log.verbose('SimSkeleton.__checks :: starting')
        if 'volume_world' not in G4Eng.Dict:
             G4Eng.log.debug('SimSkeleton.__checks :: defined a default world volume 50x50x50 meters')
             worldvolume = DetFacility("ResizeableBox", "WorldVolume")
             worldvolume.df.SetDx(50000.)
             worldvolume.df.SetDy(50000.)
             worldvolume.df.SetDz(50000.)
             G4Eng.add_DetFacility(worldvolume, worldvolume)
        if 'physics' not in G4Eng.Dict:
             G4Eng.log.debug('SimSkeleton.__checks :: defined the default Physics List: QGSP_BERT ')
             physics = PhysicsList('QGSP_BERT', 1)
             G4Eng.add_PhysicsList(physics)
        G4Eng.log.verbose('SimSkeleton.__checks :: done')


    @classmethod
    def _do_DefaultUserActions(self, G4Eng):
        """
        Default user actions: G4SimMem, G4SimTimer, G4TrackCounter
   
        Do not re-write this method. The memory and time
        checkers must be on always.
        """
        G4Eng.log.verbose('AtlasSimSkeleton._do_DefaultUserActions :: starting')
        #from atlas_utilities import G4SimMem, G4SimTimer, G4TrackCounter
        from ISF_Geant4Config.UserAction import UserAction
        G4SimTimer = UserAction('G4UserActions', 'G4SimTimer', ['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun'])
        G4SimMem = UserAction('G4UserActions', 'G4SimMem', ['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun'])
        G4TrackCounter = UserAction('G4UserActions', 'G4TrackCounter', ['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun'])
        actions = G4Eng.menu_UserActions()
        actions.add_UserAction(G4SimTimer)
        actions.add_UserAction(G4SimMem)
        actions.add_UserAction(G4TrackCounter)
        from G4AtlasApps.SimFlags import simFlags
        if jobproperties.Beam.beamType() == 'cosmics' and not simFlags.CavernBG.statusOn:
            # Add a user action to save the perigees
            cpAction = UserAction('G4UserActions', 'CosmicPerigeeAction', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
            cpAction.set_Properties({"allowMods":"1"})
            actions.add_UserAction(cpAction)
        if simFlags.PhysicsList == 'QGSP_BERT_HP':
            # Add a user action to get rid of low energy photons and do the looper killing for us
            cpAction = UserAction('G4UserActions', 'PhotonKiller', ['Step'])
            actions.add_UserAction(cpAction)
        if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() == 'Read':
            # Add a user action to time wrap hits in the muon system
            cpAction = UserAction('G4UserActions', 'HitWrapper', ['EndOfEvent'])
            actions.add_UserAction(cpAction)
        if simFlags.StoppedParticleFile.statusOn:
            # Add a user action to filter events with no stopping positions
            positionAction = UserAction('G4UserActions','StoppedParticleAction',['Step'])
            actions.add_UserAction(positionAction)
            filterAction = UserAction('G4CosmicFilter','G4CosmicFilter',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
            filterAction.set_Properties({'VolumeName':'StoppingPositions'})
            actions.add_UserAction(filterAction)
        G4Eng.log.verbose('AtlasSimSkeleton._do_DefaultUserActions :: done')

  
    @classmethod
    def _do_jobproperties(self, G4Eng):
        """
        Place to handle JobProperties.
        """

        ## Import extra flags if it hasn't already been done
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        if "atlas_flags" not in simFlags.extra_flags:
            simFlags.load_atlas_flags()
        if jobproperties.Beam.beamType() == "cosmics" and "cosmics_flags" not in simFlags.extra_flags:
            simFlags.load_cosmics_flags()

        G4Eng.log.verbose('AtlasSimSkeleton._do_jobproperties :: starting')
        ## Switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()
        DetFlags.writeRDOPool.all_setOff()

        ## Global flags needed by externals
        from AthenaCommon.GlobalFlags import globalflags
        globalflags.DataSource = 'geant4'
        if jobproperties.Beam.beamType() == 'cosmics':
            globalflags.DetGeo = 'commis'
        else:
            globalflags.DetGeo = 'atlas'

        ## At this point we can set the global job properties flag
        globalflags.DetDescrVersion = simFlags.SimLayout.get_Value()

        ## Switch off filters in the case of cavern BG
        if simFlags.CavernBG.statusOn and simFlags.CavernBG.get_Value() != 'Signal':
            if simFlags.EventFilter.statusOn and simFlags.EventFilter.get_Value()['EtaPhiFilters']:
                G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Running Cavern BG simulation - turning off EtaPhi Filter!')
                simFlags.EventFilter.get_Value()['EtaPhiFilters'] = False
            if simFlags.CavernBG.get_Value()=='Read' and \
               simFlags.EventFilter.statusOn and simFlags.EventFilter.get_Value()['VertexPositioner']:
                G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Reading Cavern BG events - turning off Vertex Positioner (and VertexFromCondDB and VertexTimeOffset)!')
                simFlags.EventFilter.get_Value()['VertexPositioner'] = False
                simFlags.VertexFromCondDB.set_Off()
                simFlags.VertexTimeOffset.set_Off()

        # Switch off GeoModel Release in the case of parameterization
        if simFlags.LArParameterization.get_Value()>0 and simFlags.ReleaseGeoModel():
            G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Running parameterization - switching off GeoModel release!')
            simFlags.ReleaseGeoModel = False

        ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
        from AthenaCommon.AppMgr import ServiceMgr
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        ServiceMgr += IOVDbSvc()
        if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
            msg = "AtlasSimSkeleton._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
            msg += "You must set the globaltags.ConditionsTag flag in your job options."
            G4Eng.log.error(msg)
            raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
        # TODO: does this always work without the 'if' statement?
        #if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag.get_Value():
        #    ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()
        ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

        ## Print flags
        if G4Eng.log.getEffectiveLevel() < 40:
            G4Eng.log.info('SimAtlasSkeleton._do_jobproperties :: printing detector flags DetFlags')
            DetFlags.Print()
            G4Eng.log.info('SimAtlasSkeleton._do_jobproperties :: printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')

        ## Lock the job properties
        jobproperties.lock_JobProperties()
        G4Eng.log.verbose('AtlasSimSkeleton._do_jobproperties :: done')


    @classmethod
    def _do_external(self, G4Eng):
        """Place to handle the external services: GeoModel, IOVDb,
           frozen showers, etc
        """
        G4Eng.log.verbose('AtlasSimSkeleton._do_external :: starting')
        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        geo2G4Svc = Geo2G4Svc()
        #theApp.CreateSvc += ["Geo2G4Svc"]
        ServiceMgr += geo2G4Svc
        ## Enable top transforms for the ATLAS geometry
        geo2G4Svc.GetTopTransform = True
    
        ## Configure access to the BeamCondSvc if necessary.  Assuming
        ## it should be on always as BeamEffectTransformation is
        ## currently always switched on, else something like this will
        ## be required here.
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        include("InDetBeamSpotService/BeamCondSvc.py")
        if not hasattr(ServiceMgr, 'BeamCondSvc'):
            from InDetBeamSpotService.InDetBeamSpotServiceConf import BeamCondSvc
            if simFlags.VertexFromCondDB.statusOn and simFlags.VertexFromCondDB():
                beamcondsvc = BeamCondSvc('BeamCondSvc')
            else:
                ## If hard-coded values other than zero smearing are
                ## required then the BeamCondSvc should be configured
                ## in the preInclude job options and added to the
                ## ServiceMgr at that point.
                beamcondsvc = BeamCondSvc('BeamCondSvc', posX = 0.0,
                                          posY = 0.0, posZ = 0.0,
                                          sigmaX = 0.0, sigmaY = 0.0,
                                          sigmaZ = 0.0, sigmaXY = 0.0,
                                          tiltX = 0.0, tiltY = 0.0)
    
            ServiceMgr += beamcondsvc
            #theApp.CreateSvc += ["BeamCondSvc"]
        ServiceMgr.BeamCondSvc.useDB = simFlags.VertexFromCondDB()
    
        ## GeoModel stuff
        ## TODO: Tidy imports etc.
        from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
        from AthenaCommon.GlobalFlags import jobproperties
        from AtlasGeoModel import SetGeometryVersion
        from AtlasGeoModel import GeoModelInit
        from AtlasGeoModel import SimEnvelopes
        from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
        gms = GeoModelSvc()
        ## Cosmics GeoModel tweaks
        #from ISF_Geant4Config.ISF_SimFlags import simFlags
        if jobproperties.Beam.beamType() == 'cosmics' or \
           (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
            gms.DetectorTools += [ CavernInfraDetectorTool() ]
        ## Protects GeoModelSvc in the simulation from the AlignCallbacks
        gms.AlignCallbacks = False
        ## Muon GeoModel tweaks
        if DetFlags.Muon_on():
            ## Turn off caching in the muon system
            from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
            MuonDetectorTool = MuonDetectorTool()
            MuonDetectorTool.FillCacheInitTime = 0 # default is 1
            if 'ATLAS-GEO' in simFlags.SimLayout.get_Value():
                ## Additional material in the muon system
                from AGDD2Geo.AGDD2GeoConf import AGDD2GeoSvc
                agdd2GeoSvc = AGDD2GeoSvc()
                import logging
                if G4Eng.log.getEffectiveLevel()<logging.INFO:
                    agdd2GeoSvc.PrintSections = True
                else:
                    agdd2GeoSvc.PrintSections = False
                agdd2GeoSvc.Locked = True
                #theApp.CreateSvc += ["AGDD2GeoSvc"]
                ServiceMgr += agdd2GeoSvc
            else:
                G4Eng.log.info("AtlasSimSkeleton._do_external :: You are using a layout that does not support additional Muon material.")
    
        ## Add configured GeoModelSvc to service manager
        ServiceMgr += gms
    
        ## Service for the fast simulation
        if simFlags.LArParameterization() != 0:
            from LArG4FastSimSvc.LArG4FastSimSvcInit import LArG4FastSimSvcInit
            LArG4FastSimSvcInit()
    
        ## Enable floating point exception handling
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.RuntimeStrictness = 'abort'
        G4Eng.log.debug('AtlasSimSkeleton._do_external :: Enabled floating point exceptions')
    
        ## Run the geometry envelope setup earlier than GeoSD
        self._do_GeoEnv( G4Eng )
        G4Eng.log.verbose('AtlasSimSkeleton._do_external :: done')


    @classmethod
    def _do_MagField(self, G4Eng):
        """ Configure the magnetic field
        """
        G4Eng.log.verbose('AtlasSimSkeleton._do_MagField :: starting')
        from G4AtlasApps.SimFlags import simFlags
        from ISF_Geant4Config.MagneticField import MagneticField
        if simFlags.MagneticField.statusOn:
            include("BFieldAth/BFieldAth_jobOptions.py")
            if simFlags.MagneticField() == 'COOL':
                atlasfield = MagneticField('G4Field', 'AtlasCoolField', typefield='MapField')
            else:
                atlasfield = MagneticField('G4Field', 'AtlasField', typefield='MapField')
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            MagFieldAthenaSvc = svcMgr.MagFieldAthenaSvc
            if simFlags.MagneticField() == 'OracleDB':
                MagFieldAthenaSvc.NameOfTheSource = "ORACLE"
            elif simFlags.MagneticField() == 'COOL':
                MagFieldAthenaSvc.NameOfTheSource = "COOL"
            else:
                MagFieldAthenaSvc.NameOfTheSource = "BYPYTH"
                MagFieldAthenaSvc.NameOfTheBfieldFile = simFlags.MagneticField.get_Value()

            atlasfield.set_FieldMapFileName(simFlags.MagneticField.get_Value())
            if DetFlags.bpipe_on():
                # BeamPipe volume is not added to DetFacility until
                # later on in the simulation set-up (during the
                # do_GeoSD method), so have to rely on the DetFlag
                # check here.
                G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing BeamPipe Tracking Precision')
                atlasfield.add_Volume('BeamPipe::BeamPipe')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection',  'BeamPipe::BeamPipe', 0.00001)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep',       'BeamPipe::BeamPipe', 0.0001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'BeamPipe::BeamPipe', 0.001)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'BeamPipe::BeamPipe', 0.00001)  
            if 'IDET' in G4Eng.Dict_DetFacility:
                G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing IDET Tracking Precision')
                atlasfield.add_Volume('IDET::IDET')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection', 'IDET::IDET', 0.00001)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep', 'IDET::IDET', 0.0001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'IDET::IDET', 0.001)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'IDET::IDET', 0.00001)
            if 'MUONQ02' in G4Eng.Dict_DetFacility:
                G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing MUONQ02 Tracking Precision')
                atlasfield.add_Volume('MUONQ02::MUONQ02')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection', 'MUONQ02::MUONQ02', 0.00000002)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep', 'MUONQ02::MUONQ02', 0.000001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'MUONQ02::MUONQ02', 0.0000009)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'MUONQ02::MUONQ02', 0.000001)   
            G4Eng.menu_Field.add_Field(atlasfield)
            if simFlags.EquationOfMotion.statusOn:
                G4Eng.menu_Field.set_EquationOfMotion( simFlags.EquationOfMotion.get_Value() )
        G4Eng.log.verbose('AtlasSimSkeleton._do_MagField :: done')

    @classmethod
    def _do_GeoEnv(cls, G4Eng):
        """ Construct the bare envelopes for simulation
        """
        G4Eng.log.verbose('AtlasSimSkeleton._do_GeoEnv :: starting')
        ## ATLAS envelope volumes
        from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import AtlasEnvelope
        from ISF_Geant4Config.ISF_SimFlags import simFlags
        atlasenv = AtlasEnvelope( G4Eng )
        atlas = atlasenv.atlas
    
        from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import MuonEnvelope
        muonenv = MuonEnvelope( G4Eng )
        muonq02 = muonenv.muonq02
        if DetFlags.geometry.Muon_on():
            G4Eng.add_DetFacility(muonq02, atlas)
    
        from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import InDetEnvelope
        idenv = InDetEnvelope( G4Eng )
        idet = idenv.idet
        if DetFlags.ID_on():
            G4Eng.add_DetFacility(idet, atlas)
    
        from ISF_Geant4Config.ISF_AtlasVolumeEnvelopes import CaloEnvelope
        caloenv = CaloEnvelope( G4Eng )
        calo = caloenv.calo
        if DetFlags.Calo_on():
            G4Eng.add_DetFacility(calo, atlas)
        G4Eng.log.verbose('AtlasSimSkeleton._do_GeoEnv :: done')

