# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Main configuration module for the full ATLAS simulations
"""

import math
from AthenaCommon import PhysicalConstants
from AthenaCommon.AppMgr import theApp
from AthenaCommon.Include import include
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.BeamFlags import jobproperties

import PyG4Atlas, AtlasG4Eng
from PyG4Atlas import SimSkeleton



class AtlasSimSkeleton(SimSkeleton):
    """
    Defines the simulation entity for the full ATLAS detector.
    """

    def __init__(self):
        if 'simu_skeleton' not in AtlasG4Eng.G4Eng.Dict:
            AtlasG4Eng.G4Eng.Dict['simu_skeleton'] = self
            AtlasG4Eng.G4Eng.Name = "ATLAS_G4Sim"
        else:
            AtlasG4Eng.G4Eng.log.warning('AtlasSimSkeleton.__init__ :: the simulation already has a skeleton.' +
                                               'You can find it in the G4AtlasEng.G4Eng.Dict()')


    @classmethod
    def _do_jobproperties(self):
        """
        Place to handle JobProperties.
        """

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if "atlas_flags" not in simFlags.extra_flags:
            simFlags.load_atlas_flags()
        if jobproperties.Beam.beamType() == "cosmics" and "cosmics_flags" not in simFlags.extra_flags:
            simFlags.load_cosmics_flags()

        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_jobproperties :: starting')

        ## Tidy up DBM DetFlags: temporary measure
        DetFlags.DBM_setOff()

        ## Tidy up NSW DetFlags: temporary measure
        DetFlags.sTGC_setOff()
        DetFlags.Micromegas_setOff()
        if hasattr(simFlags, 'SimulateNewSmallWheel'):
            if simFlags.SimulateNewSmallWheel():
                DetFlags.sTGC_setOn()
                DetFlags.Micromegas_setOn()

        ## Switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.overlay.all_setOff()
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
                AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Running Cavern BG simulation - turning off EtaPhi Filter!')
                simFlags.EventFilter.switchFilterOff('EtaPhiFilters')
            if simFlags.CavernBG.get_Value()=='Read' and \
               simFlags.EventFilter.statusOn and simFlags.EventFilter.get_Value()['VertexPositioner']:
                AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Reading Cavern BG events - turning off Vertex Positioner (and VertexFromCondDB and VertexTimeOffset)!')
                simFlags.EventFilter.switchFilterOff('VertexPositioner')
                simFlags.VertexFromCondDB.set_Off()
                simFlags.VertexTimeOffset.set_Off()

        # Switch off GeoModel Release in the case of parameterization
        if simFlags.LArParameterization.get_Value()>0 and simFlags.ReleaseGeoModel():
            AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: Running parameterization - switching off GeoModel release!')
            simFlags.ReleaseGeoModel = False

        ## Translate conditions tag into IOVDbSvc global tag: must be done before job properties are locked!!!
        from AthenaCommon.AppMgr import ServiceMgr
        from IOVDbSvc.IOVDbSvcConf import IOVDbSvc
        ServiceMgr += IOVDbSvc()
        if not hasattr(globalflags, "ConditionsTag") or not globalflags.ConditionsTag.get_Value():
            msg = "AtlasSimSkeleton._do_jobproperties :: conditions tag has not been set and is no longer guessed by SimAtlasKernel. "
            msg += "You must set the globaltags.ConditionsTag flag in your job options."
            AtlasG4Eng.G4Eng.log.error(msg)
            raise SystemExit("AtlasSimSkeleton._do_jobproperties :: Global ConditionsTag not set")
        if not hasattr(ServiceMgr.IOVDbSvc, 'GlobalTag') or not ServiceMgr.IOVDbSvc.GlobalTag:
            ServiceMgr.IOVDbSvc.GlobalTag = globalflags.ConditionsTag.get_Value()

        ## Enable floating point exception handling
        ## FIXME! This seems to cause the jobs to crash in the FpeControlSvc, so commenting this out for now...
        #from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        #athenaCommonFlags.RuntimeStrictness = 'abort'
        #AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_jobproperties :: Enabled floating point exceptions')

        ## Print flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel() < 40:
            AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_jobproperties :: printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')

        # Lock the job properties if not running ISF.
        if not simFlags.ISFRun:
            jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_jobproperties :: done')


    @classmethod
    def _do_external(self):
        """Place to handle the external services: GeoModel, IOVDb,
           frozen showers, etc
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_external :: starting')
        from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        geo2G4Svc = Geo2G4Svc()
        theApp.CreateSvc += ["Geo2G4Svc"]
        ServiceMgr += geo2G4Svc
        ## Enable top transforms for the ATLAS geometry
        geo2G4Svc.GetTopTransform = True

        ## Configure access to the BeamCondSvc if necessary.  Assuming
        ## it should be on always as BeamEffectTransformation is
        ## currently always switched on, else something like this will
        ## be required here.
        from G4AtlasApps.SimFlags import simFlags
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

        ## Forward Region Twiss files - needed before geometry setup!
        if simFlags.ForwardDetectors.statusOn:
            if DetFlags.geometry.FwdRegion_on():
                from AthenaCommon.CfgGetter import getPublicTool
                from AthenaCommon.AppMgr import ToolSvc
                ToolSvc += getPublicTool("ForwardRegionProperties")

        from AtlasGeoModel import GeoModelInit
        from AtlasGeoModel import SimEnvelopes
        from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
        gms = GeoModelSvc()
        ## Cosmics GeoModel tweaks
        #from G4AtlasApps.SimFlags import simFlags
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
            if hasattr(simFlags, 'SimulateNewSmallWheel'):
                if simFlags.SimulateNewSmallWheel():
                    AtlasG4Eng.G4Eng.log.info("Removing the original small wheel")
                    MuonDetectorTool.StationSelection  = 2
                    MuonDetectorTool.SelectedStations  = [ "EIL1" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL2" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL6" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL7" ]
                    MuonDetectorTool.SelectedStations  += [ "EIS*" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL10" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL11" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL12" ]
                    MuonDetectorTool.SelectedStations  += [ "EIL17" ]
                    MuonDetectorTool.SelectedStations  += [ "CSS*" ]
                    MuonDetectorTool.SelectedStations  += [ "CSL*" ]
                    MuonDetectorTool.SelectedStations  += [ "T4E*" ]
                    MuonDetectorTool.SelectedStations  += [ "T4F*" ]

            ## Additional material in the muon system
            from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
            AGDD2Geo = AGDDtoGeoSvc()
            from AthenaCommon import CfgGetter
            if not "MuonAGDDTool/MuonSpectrometer" in AGDD2Geo.Builders:
                ToolSvc += CfgGetter.getPublicTool("MuonSpectrometer", checkType=True)
                AGDD2Geo.Builders += ["MuonAGDDTool/MuonSpectrometer"]
            if hasattr(simFlags, 'SimulateNewSmallWheel'):
                if simFlags.SimulateNewSmallWheel():
                    if not "NSWAGDDTool/NewSmallWheel" in AGDD2Geo.Builders:
                        ToolSvc += CfgGetter.getPublicTool("NewSmallWheel", checkType=True)
                        AGDD2Geo.Builders += ["NSWAGDDTool/NewSmallWheel"]
            theApp.CreateSvc += ["AGDDtoGeoSvc"]
            ServiceMgr += AGDD2Geo

        ## Add configured GeoModelSvc to service manager
        ServiceMgr += gms

        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')

        ## Run the geometry envelope setup earlier than GeoSD
        self._do_GeoEnv() #TODO remove
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_external :: done')


    @classmethod
    def _do_GeoEnv(self):
        """ Construct the bare envelopes for simulation. OBSOLETE -  REMOVE
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoEnv :: starting')
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoEnv :: done')

    @classmethod
    def do_UserActions(self):
        """
        User actions: TrackProcessor
        """
        # this line to make sure that the default UAs are added to the list
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore

        # only setup those user actions if running within ISF
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.ISFRun:

            from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
            ##hack to allow for different named Configurations of the UserActions
            def UserActionPath(basename):
                actionString=''
                for t in ToolSvc.getAllChildren():
                    if basename in t.getName():
                        actionString = t.getName()
                        break
                from G4AtlasApps import AtlasG4Eng
                if ''==actionString:
                    AtlasG4Eng.G4Eng.log.error('ISF_AtlasSimSkeleton::do_UserActions: Could not find ' + basename + ' instance in ToolSvc!')
                    raise SystemExit('ISF_AtlasSimSkeleton::do_UserActions: Could not find ' + basename + ' instance in ToolSvc!')
                else:
                    AtlasG4Eng.G4Eng.log.info('ISF_AtlasSimSkeleton::do_UserActions: Found a ' + basename + ' instance called ' + actionString)
                return actionString
            from AthenaCommon.CfgGetter import getPublicTool
            UAStore.addSystemAction( getPublicTool(UserActionPath('MCTruthUserAction')),['BeginOfTracking','EndOfTracking'])
            from ISF_Config.ISF_jobProperties import ISF_Flags
            if ISF_Flags.ValidationMode.get_Value():
                UAStore.addAction( getPublicTool(UserActionPath('PhysicsValidationUserAction')),['BeginOfRun','EndOfEvent','Step','BeginOfTracking','BeginOfEvent'])
            UAStore.addAction( getPublicTool(UserActionPath('TrackProcessorUserAction')),['Step','BeginOfTracking','BeginOfEvent'])

    @classmethod
    def do_GeoSD(self):
        """ Configure the geometry and SD
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoSD :: starting')
        ## Different geometry-levels for rec envelopes depending into simFlags.SimLayout
        recenv_level = 2
        from G4AtlasApps.SimFlags import simFlags
        from AthenaCommon.BeamFlags import jobproperties
        if jobproperties.Beam.beamType() == 'cosmics' or \
                (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            recenv_level = 3

        # if this is an ISF run, allow the collections on store gate to be modified
        # by other algorithms (i.e. set them non-const)
        allowSGMods = True if simFlags.ISFRun else False

        ## Get a handle to the RecEnv menu
        if not simFlags.ISFRun:
            menuRecordEnvelopes = AtlasG4Eng.G4Eng.menu_RecordEnvelope()
            from atlas_mctruth import RecordingEnvelopes

        ## Inner detector
        if DetFlags.ID_on():
            if not simFlags.ISFRun:
                rc = RecordingEnvelopes.AtlasCaloEntryLayer(recenv_level, allowSGMods)
                menuRecordEnvelopes.add_RecEnvelope(rc.recenv)
            if DetFlags.geometry.TRT_on():
                AtlasG4Eng.G4Eng.read_XML("TRgeomodelgeometry.xml") # FIXME need to find a better way to do this

        ## Calorimeters
        if DetFlags.Calo_on():
            if not simFlags.ISFRun:
                rc = RecordingEnvelopes.AtlasMuonEntryLayer(recenv_level, allowSGMods)
                menuRecordEnvelopes.add_RecEnvelope(rc.recenv)

            ## LAr
            if DetFlags.geometry.LAr_on():
                from atlas_calo import PyLArG4RunControler
                lArG4RunControl = PyLArG4RunControler('PyLArG4RunControl', 'LArG4RunControlDict', allowMods=allowSGMods)

        ## Muon system
        if DetFlags.geometry.Muon_on():
            if not simFlags.ISFRun:
                rc = RecordingEnvelopes.AtlasMuonExitLayer(recenv_level, allowSGMods)
                menuRecordEnvelopes.add_RecEnvelope(rc.recenv)

        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoSD :: done')

    @classmethod
    def do_MCtruth(self):
        """ Configure the MCTruth strategies.
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_MCTruth :: starting')
        ## Different geometry levels for MCTruth strategies, depending on simFlags.SimLayout
        mctruth_level = 1 # default
        from G4AtlasApps.SimFlags import simFlags
        if jobproperties.Beam.beamType() == 'cosmics' or \
           (simFlags.CavernBG.statusOn and not 'Signal' in simFlags.CavernBG.get_Value() ):
            mctruth_level = 2

        if DetFlags.Truth_on():
            from atlas_mctruth import MCTruthStrategies
            mcTruthMenu = AtlasG4Eng.G4Eng.menu_MCTruth()

            if DetFlags.ID_on():
                strategy1 = MCTruthStrategies.StrategyIDET1(mctruth_level)
                strategy2 = MCTruthStrategies.StrategyIDET2(mctruth_level)
                strategy3 = MCTruthStrategies.StrategyIDET3(mctruth_level)
                strategy4 = MCTruthStrategies.StrategyIDET4(mctruth_level)
                strategy5 = MCTruthStrategies.StrategyIDET5(mctruth_level)
                strategy6 = MCTruthStrategies.StrategyIDET6(mctruth_level)
                mcTruthMenu.add_McTruthStrategy(strategy1.strg)
                mcTruthMenu.add_McTruthStrategy(strategy2.strg)
                mcTruthMenu.add_McTruthStrategy(strategy3.strg)
                mcTruthMenu.add_McTruthStrategy(strategy4.strg)
                mcTruthMenu.add_McTruthStrategy(strategy5.strg)
                mcTruthMenu.add_McTruthStrategy(strategy6.strg)
                if DetFlags.bpipe_on():
                   strategy1.strg.add_Volumes('BeamPipe::BeamPipe', mctruth_level)
                   strategy2.strg.add_Volumes('BeamPipe::BeamPipe', mctruth_level)
                   strategy3.strg.add_Volumes('BeamPipe::BeamPipe', mctruth_level)
                   strategy4.strg.add_Volumes('BeamPipe::BeamPipe', mctruth_level)
                   strategy5.strg.add_Volumes('BeamPipe::BeamPipeCentral', mctruth_level+1)
                   strategy6.strg.add_Volumes('BeamPipe::BeamPipe', mctruth_level)

            if DetFlags.Calo_on():
                strategyCalo = MCTruthStrategies.StrategyCALO(mctruth_level)
                mcTruthMenu.add_McTruthStrategy(strategyCalo.strg)

            if DetFlags.geometry.Muon_on():
                #strategyMuon = MCTruthStrategies.StrategyMUON(mctruth_level)
                #mcTruthMenu.add_McTruthStrategy(strategyMuon.strg)
                strategyMuonQ02 = MCTruthStrategies.StrategyMUONQ02(mctruth_level)
                mcTruthMenu.add_McTruthStrategy(strategyMuonQ02.strg)
            #mcTruthMenu.set_SecondarySaving('All')
            mcTruthMenu.set_SecondarySaving('StoredSecondaries')
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_MCTruth :: done')


    @classmethod
    def _do_MagField(self):
        """ Configure the magnetic field
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_MagField :: starting')
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.MagneticField.statusOn:
            if simFlags.MagneticField() == 'AtlasFieldSvc':
                from AthenaCommon.AppMgr import ServiceMgr as svcMgr
                import MagFieldServices.SetupField
                # TODO: would be useful if the AthenaSvc name can be handed over to the PyG4Atlas.MagneticField somehow
                # currently G4AtlasFieldSvc.cxx retrieves a field service with the default name 'AtlasFieldSvc'
                atlasfield = PyG4Atlas.MagneticField('G4Field', 'G4AtlasFieldSvc', typefield='MapField')
            else:
                raise Exception('IllegalFieldServiceSpecification')

            atlasfield.set_FieldMapFileName(simFlags.MagneticField.get_Value())

            if DetFlags.bpipe_on():
                # BeamPipe volume is not added to DetFacility until
                # later on in the simulation set-up (during the
                # do_GeoSD method), so have to rely on the DetFlag
                # check here.
                AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing BeamPipe Tracking Precision')
                atlasfield.add_Volume('BeamPipe::BeamPipe')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection',  'BeamPipe::BeamPipe', 0.00001)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep',       'BeamPipe::BeamPipe', 0.0001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'BeamPipe::BeamPipe', 0.001)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'BeamPipe::BeamPipe', 0.00001)
            if DetFlags.ID_on():
                AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing IDET Tracking Precision')
                atlasfield.add_Volume('IDET::IDET')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection', 'IDET::IDET', 0.00001)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep', 'IDET::IDET', 0.0001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'IDET::IDET', 0.001)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'IDET::IDET', 0.00001)
            if DetFlags.Muon_on():
                AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_MagField :: Increasing MUONQ02 Tracking Precision')
                atlasfield.add_Volume('MUONQ02::MUONQ02')
                atlasfield.set_G4FieldTrackParameters('DeltaIntersection', 'MUONQ02::MUONQ02', 0.00000002)
                atlasfield.set_G4FieldTrackParameters('DeltaOneStep', 'MUONQ02::MUONQ02', 0.000001)
                atlasfield.set_G4FieldTrackParameters('MaximumEpsilonStep', 'MUONQ02::MUONQ02', 0.0000009)
                atlasfield.set_G4FieldTrackParameters('MinimumEpsilonStep', 'MUONQ02::MUONQ02', 0.000001)
            AtlasG4Eng.G4Eng.menu_Field.add_Field(atlasfield)
            if simFlags.EquationOfMotion.statusOn:
                AtlasG4Eng.G4Eng.menu_Field.set_EquationOfMotion( simFlags.EquationOfMotion.get_Value() )
            ## Forward Region
            if simFlags.ForwardDetectors.statusOn:
                if DetFlags.geometry.FwdRegion_on():
                    # Set up the field
                    step_limitation = None
                    if simFlags.FwdStepLimitation.statusOn:
                        step_limitation = simFlags.FwdStepLimitation()
                    fieldVolumes = []
                    fieldVolumes += [ ['Q1', ['FwdRegion::LQXAA.1R1MagQ1'] ] ]
                    fieldVolumes += [ ['Q2', ['FwdRegion::LQXBA.2R1MagQ2a', 'FwdRegion::LQXBA.2R1MagQ2b'] ] ]
                    fieldVolumes += [ ['Q3', ['FwdRegion::LQXAG.3R1MagQ3'] ] ]
                    fieldVolumes += [ ['D1', [ 'FwdRegion::MBXW.A4R1MagD1a', 'FwdRegion::MBXW.B4R1MagD1b', 'FwdRegion::MBXW.C4R1MagD1c',\
                                               'FwdRegion::MBXW.D4R1MagD1d', 'FwdRegion::MBXW.E4R1MagD1e', 'FwdRegion::MBXW.F4R1MagD1f'] ] ]
                    fieldVolumes += [ ['D2', ['FwdRegion::LBRCD.4R1MagD2'] ] ]
                    fieldVolumes += [ ['Q4', ['FwdRegion::LQYCH.4R1MagQ4'] ] ]
                    fieldVolumes += [ ['Q5', ['FwdRegion::LQNDC.5R1MagQ5'] ] ]
                    fieldVolumes += [ ['Q6', ['FwdRegion::LQNDD.6R1MagQ6'] ] ]
                    fieldVolumes += [ ['Q7', ['FwdRegion::LQNFD.7R1MagQ7a', 'FwdRegion::LQNFD.7R1MagQ7b'] ] ]
                    fieldVolumes += [ ['Q1HKick', ['FwdRegion::LQXAA.1R1MagQ1HKick'] ] ]
                    fieldVolumes += [ ['Q1VKick', ['FwdRegion::LQXAA.1R1MagQ1VKick'] ] ]
                    fieldVolumes += [ ['Q2HKick', ['FwdRegion::LQXBA.2R1MagQ2HKick'] ] ]
                    fieldVolumes += [ ['Q2VKick', ['FwdRegion::LQXBA.2R1MagQ2VKick'] ] ]
                    fieldVolumes += [ ['Q3HKick', ['FwdRegion::LQXAG.3R1MagQ3HKick'] ] ]
                    fieldVolumes += [ ['Q3VKick', ['FwdRegion::LQXAG.3R1MagQ3VKick'] ] ]
                    fieldVolumes += [ ['Q4VKickA', ['FwdRegion::LQYCH.4R1MagQ4VKickA'] ] ]
                    fieldVolumes += [ ['Q4HKick', ['FwdRegion::LQYCH.4R1MagQ4HKick'] ] ]
                    fieldVolumes += [ ['Q4VKickB', ['FwdRegion::LQYCH.4R1MagQ4VKickB'] ] ]
                    fieldVolumes += [ ['Q5HKick', ['FwdRegion::LQNDC.5R1MagQ5HKick'] ] ]
                    fieldVolumes += [ ['Q6VKick', ['FwdRegion::LQNDD.6R1MagQ6VKick'] ] ]

                    epsMin = 1e-9
                    epsMax = 1e-8
                    deltaIntersection = 1e-9
                    deltaOneStep = 1e-8

                    for i in fieldVolumes:
                        a_field = PyG4Atlas.MagneticField('ForwardRegionMgField',i[0],typefield='MapField')
                        for v in i[1]:
                            a_field.add_Volume(v)
                            a_field.set_G4FieldTrackParameters('DeltaIntersection',  v, deltaIntersection)
                            a_field.set_G4FieldTrackParameters('DeltaOneStep',       v, deltaOneStep)
                            a_field.set_G4FieldTrackParameters('MaximumEpsilonStep', v, epsMax)
                            a_field.set_G4FieldTrackParameters('MinimumEpsilonStep', v, epsMin)

                            # limit step length
                            if step_limitation is not None:
                                AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetMaxStep(v, step_limitation)

                        a_field.set_FieldMapFileName(i[0])
                        AtlasG4Eng.G4Eng.menu_Field.add_Field(a_field)

                    if step_limitation is not None:
                        AtlasG4Eng.G4Eng._ctrl.geometryMenu.SetMaxStep('FwdRegion::ForwardRegionGeoModel', step_limitation)
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_MagField :: done')

    @classmethod
    def _do_metadata(self):
        """
        Setup and add metadata to the HIT file.
        """
        import AtlasG4Eng,os
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_metadata :: starting')
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        ## Read in special simulation job option fragments based on metadata passed by the evgen stage
        if 'G4ATLAS_SKIPFILEPEEK' in os.environ and os.environ['G4ATLAS_SKIPFILEPEEK']:
            AtlasG4Eng.G4Eng.log.info('Skipping reading of extra metadata - will not work with G4ATLAS_SKIPFILEPEEK set')
        elif athenaCommonFlags.PoolEvgenInput.statusOn:
            AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_metadata :: checking input file metadata.')
            import PyUtils.AthFile as af
            f = af.fopen(athenaCommonFlags.PoolEvgenInput()[0])
            #for k, v in f.infos.iteritems():
            #    print k, v
            if "specialConfiguration" in f.infos["tag_info"]:
                item = f.infos["tag_info"]["specialConfiguration"]
                ## Parse the specialConfiguration string
                ## Format is 'key1=value1;key2=value2;...'. or just '
                AtlasG4Eng.G4Eng.log.debug("AtlasSimSkeleton._do_metadata :: Evgen specialConfiguration directive: ", item)
                spcitems = item.split(";")
                params = {}
                for spcitem in spcitems:
                    #print spcitem
                    ## Ignore empty or "NONE" substrings, e.g. from consecutive or trailing semicolons
                    if not spcitem or spcitem.upper() == "NONE":
                        continue
                    ## If not in key=value format, treat as v, with k="preInclude"
                    if "=" not in spcitem:
                        spcitem = "preInclude=" + spcitem
                    ## Handle k=v directives
                    k, v = spcitem.split("=")
                    AtlasG4Eng.G4Eng.log.info("AtlasSimSkeleton._do_metadata :: specialConfiguration metadata item: %s => %s" % (k, v))
                    if k == "preInclude":
                        AtlasG4Eng.G4Eng.log.info("AtlasSimSkeleton._do_metadata :: Including %s as instructed by specialConfiguration metadata" % v)
                        include(v)
                    else:
                        params[k] = v
                    AtlasG4Eng.G4Eng.Dict_SpecialConfiguration.update( params )

        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.ISFRun:
            AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_metadata :: recording metadata')

            from IOVDbMetaDataTools import ParameterDbFiller
            dbFiller = ParameterDbFiller.ParameterDbFiller()

            ## Set run numbers
            minrunnum = 0
            maxrunnum = 2147483647
            import os
            from G4AtlasApps.SimFlags import simFlags

            if hasattr(simFlags, 'RunNumber') and simFlags.RunNumber.statusOn:
                minrunnum = simFlags.RunNumber()
                ## FIXME need to use maxrunnum = 2147483647 for now to keep overlay working but in the future this should be set properly.
                # maxrunnum = minrunnum + 1
            elif 'G4ATLAS_SKIPFILEPEEK' in os.environ and os.environ['G4ATLAS_SKIPFILEPEEK']:
                AtlasG4Eng.G4Eng.log.info('Skipping run number setting - would need to set simFlags.RunNumber for this')
            elif athenaCommonFlags.PoolEvgenInput.statusOn:
                import PyUtils.AthFile as af
                try:
                    f = af.fopen(athenaCommonFlags.PoolEvgenInput()[0])
                    if len(f.run_numbers) > 0:
                        minrunnum = f.run_numbers[0]
                        maxrunnum = minrunnum + 1
                    else:
                        raise Exception('IllegalRunNumber')
                except Exception :
                    myCommand = 'dumpRunNumber.py '+jobproperties.AthenaCommonFlags.PoolEvgenInput.get_Value()[0] +' | grep "run number:" | sed \'s/run number://\''
                    myOutput = os.popen3(myCommand)[1].read().strip() #read in child_stdout only
                    if (len(myOutput) > 0 and int(myOutput) > 0) :
                        AtlasG4Eng.G4Eng.log.info( "Setting run number from file to ",myOutput," with a range of one" )
                        minrunnum = myOutput
                        maxrunnum = myOutput+1
                    else:
                        AtlasG4Eng.G4Eng.log.info( "Setting run number from 0 to 2147483647 (could not open evgen file)" )

            AtlasG4Eng.G4Eng.log.info("AtlasSimSkeleton._do_metadata :: Setting IOV run number range to [%d, %d]" % (minrunnum, maxrunnum))
            dbFiller.setBeginRun(minrunnum)
            dbFiller.setEndRun(maxrunnum)

            ## Write sim parameters as metadata keys
            from AthenaCommon.JobProperties import JobProperty
            simParams = [sf for sf in dir(simFlags) if isinstance(getattr(simFlags, sf), JobProperty)]
            for sp in simParams:
                ## Don't write out random number seeds or RunDict as metadata
                if sp in ("RandomSeedList", "RandomSeedOffset", "RunDict"):
                    continue
                ## Only store InitFunction names
                if sp in ("InitFunctions"):
                    initfuncdict = dict()
                    for level in getattr(simFlags, sp).get_Value():
                        initfuncdict.setdefault(level, [])
                        for element in getattr(simFlags, sp).get_Value()[level]:
                            initfuncdict[level].append(element.__name__)
                    testValue = str( initfuncdict ) if getattr(simFlags, sp).statusOn else 'default'
                    dbFiller.addSimParam(sp, testValue)
                    continue
                testValue = str( getattr(simFlags, sp).get_Value() ) if getattr(simFlags, sp).statusOn else 'default'
                dbFiller.addSimParam(sp, testValue)
            import os
            dbFiller.addSimParam('G4Version', str(os.environ['G4VERS']))
            dbFiller.addSimParam('RunType', 'atlas')
            dbFiller.addSimParam('beamType', jobproperties.Beam.beamType.get_Value())

            ## Simulated detector flags: add each enabled detector to the simulatedDetectors list
            simDets = []
            for det in ['pixel','SCT','TRT','BCM','DBM','Lucid','FwdRegion','ZDC','ALFA','AFP','LAr','Tile','MDT','CSC','TGC','RPC','Micromegas','sTGC','Truth']:
                attrname = det+"_on"
                checkfn = getattr(DetFlags.geometry, attrname, None)
                if checkfn is None:
                    AtlasG4Eng.G4Eng.log.warning("AtlasSimSkeleton._do_metadata :: No attribute '%s' found on DetFlags.geometry" % attrname)
                    continue
                if checkfn():
                    simDets.append(det)
            AtlasG4Eng.G4Eng.log.info("AtlasSimSkeleton._do_metadata :: Setting 'SimulatedDetectors' = %s" % repr(simDets))
            dbFiller.addSimParam('SimulatedDetectors', repr(simDets))

            ## Hard-coded simulation hit file magic number (for major changes)
            dbFiller.addSimParam('hitFileMagicNumber', '0') ##FIXME Remove this?

            ## Write the db info
            dbFiller.genSimDb()
            folder = "/Simulation/Parameters"
            dbConnection = "sqlite://;schema=SimParams.db;dbname=SIMPARAM"
            import IOVDbSvc.IOVDb
            from AthenaCommon.AppMgr import ServiceMgr
            ServiceMgr.IOVDbSvc.Folders += [ folder + "<dbConnection>" + dbConnection + "</dbConnection>" ]
            ServiceMgr.IOVDbSvc.FoldersToMetaData += [folder]
            ServiceMgr.IOVSvc.partialPreLoadData = True

            if not athenaCommonFlags.PoolHitsOutput.statusOn:
                AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_metadata :: no output HITS file, so no metadata writing required.')
            else:
                from AthenaServices.AthenaServicesConf import AthenaOutputStream
                stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
                stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_metadata :: done')


    @classmethod
    def do_EventFilter(self):
        """ Configure the event manipulators
        """
        import AtlasG4Eng
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_EventFilter :: starting')
        ## Event filter
        from G4AtlasApps.SimFlags import simFlags
        if simFlags.EventFilter.statusOn:
            menu_EventFilter = AtlasG4Eng.G4Eng.menu_EventFilter()
            stat = menu_EventFilter.getFilterStatus()
            ## TODO: Always switch off the EtaPhiFilters if ALFA/ZDC/AFP are being simulated?
            stat['EtaPhiFilters'] = simFlags.EventFilter.get_Value()['EtaPhiFilters']
            stat['VertexPositioner'] = simFlags.EventFilter.get_Value()['VertexPositioner']
            if not stat['VertexPositioner']:
                if simFlags.VertexFromCondDB.statusOn and simFlags.VertexFromCondDB():
                    AtlasG4Eng.G4Eng.log.error('AtlasSimSkeleton._do_EventFilter :: simFlags.EventFilter[VertexPositioner] has been switched off, but simFlags.VertexFromCondDB is switched on.')
                    AtlasG4Eng.G4Eng.log.warning('Please check your configuration.')
                if simFlags.VertexTimeOffset.statusOn and simFlags.VertexTimeOffset.get_Value() != 0:
                    AtlasG4Eng.G4Eng.log.error('AtlasSimSkeleton._do_EventFilter :: simFlags.EventFilter[VertexPositioner] has been switched off, but simFlags.VertexTimeOffset is set to True.')
                    AtlasG4Eng.G4Eng.log.warning('Please check your configuration.')
            elif not simFlags.VertexFromCondDB.statusOn:
                 AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_EventFilter :: simFlags.EventFilter[VertexPositioner] is on, but simFlags.VertexFromCondDB is switched off. Assuming that you have set up the BeamCondSvc yourself.')
            stat['VertexRangeChecker'] = simFlags.EventFilter.get_Value()['VertexRangeChecker']
            stat['BeamEffectTransformation'] = simFlags.EventFilter.get_Value()['BeamEffectTransformation']
            stat['PrimaryEventRotations'] = simFlags.EventFilter.get_Value()['PrimaryEventRotations']
            menu_EventFilter._build()

            if stat['EtaPhiFilters']:
                ## Choose a wider allowed eta range if LUCID is enabled
                etaFilter = menu_EventFilter.getFilter('EtaPhiFilters')
                etarange = 7.0 if DetFlags.geometry.Lucid_on() else 6.0
                etaFilter.AddEtaInterval(-etarange, etarange)

            # Additional options for beam effect transport
            if stat['BeamEffectTransformation']:
                beFilter = menu_EventFilter.getFilter('BeamEffectTransformation')
                if simFlags.BeamEffectOptions.statusOn:
                    for opt in simFlags.BeamEffectOptions.get_Value().keys():
                        exec( 'beFilter.Set_'+opt+'(float('+str(simFlags.BeamEffectOptions.get_Value()[opt])+'))' )
                        exec( 'new_value = beFilter.Get_'+opt+'()' )
                        AtlasG4Eng.G4Eng.log.info('Set BeamEffectTransformation option '+opt+' to '+str( new_value ) )

        if simFlags.VertexTimeOffset.statusOn and simFlags.VertexTimeOffset.get_Value() != 0:
            raise RuntimeError( 'Vertex time offset should not be used!' )

        if hasattr(simFlags, 'RandomSvc') and simFlags.RandomSvc.statusOn:
                AtlasG4Eng.G4Eng._ctrl.physicsMenu.SetRandomNumberService( simFlags.RandomSvc() )
        if simFlags.VertexOverrideFile.statusOn:
            AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_EventFilter :: Setting vertex override file '+str(simFlags.VertexOverrideFile()))
            AtlasG4Eng.G4Eng._ctrl.physicsMenu.SetVertexOverrideFile( simFlags.VertexOverrideFile() )
        if simFlags.VertexOverrideEventFile.statusOn:
            AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_EventFilter :: Setting vertex override event file '+str(simFlags.VertexOverrideEventFile()))
            AtlasG4Eng.G4Eng._ctrl.physicsMenu.SetVertexOverrideEventFile( simFlags.VertexOverrideEventFile() )
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_EventFilter :: done')
