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
from SimSkeleton import SimSkeleton



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
        if not simFlags.IsEventOverlayInputSim():
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

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

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
        User actions: with the migration ot MT UA, this method is no longer needed.
        """
        return


    @classmethod
    def do_GeoSD(self):
        """ Configure the geometry and SD
        """
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoSD :: starting')
        # TODO drop this method.
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_GeoSD :: done')


    @classmethod
    def _do_metadata(self):
        """
        Setup and add metadata to the HIT file.
        """
        import AtlasG4Eng
        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_metadata :: starting')
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.ISFRun:
            AtlasG4Eng.G4Eng.log.debug('AtlasSimSkeleton._do_metadata :: recording metadata')
            from G4AtlasApps.G4Atlas_Metadata import createSimulationParametersMetadata
            createSimulationParametersMetadata()
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            if not athenaCommonFlags.PoolHitsOutput.statusOn:
                AtlasG4Eng.G4Eng.log.info('AtlasSimSkeleton._do_metadata :: no output HITS file, so no metadata writing required.')
            else:
                from AthenaServices.AthenaServicesConf import AthenaOutputStream
                stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
                stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]

        AtlasG4Eng.G4Eng.log.verbose('AtlasSimSkeleton._do_metadata :: done')


