# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Main configuration module for the ATLAS test-beams

CTB(2004): ATLAS Combined Test Beam
Tile standalone test beam 2000-2003
LAr  standalone test beams at H6
"""

__author__ = 'M. Gallas'


import math
from AthenaCommon               import PhysicalConstants
from AthenaCommon.AppMgr        import theApp
from AthenaCommon.Include       import include
from AthenaCommon.GlobalFlags   import globalflags
from AthenaCommon.DetFlags      import DetFlags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.BeamFlags     import jobproperties
import PyG4Atlas, AtlasG4Eng
from SimSkeleton import SimSkeleton




class TBSimSkeleton(SimSkeleton):
    """
    Defines a common SimSkeleton for the test beams.
    """

    @classmethod
    def _do_metadata(self):
        """
        Setup and add metadata to the HIT file
        """
        import AtlasG4Eng
        AtlasG4Eng.G4Eng.log.verbose('TBSimSkeleton :: _do_metadata :: starting')
        from G4AtlasApps.G4Atlas_Metadata import createTBSimulationParametersMetadata
        createTBSimulationParametersMetadata()

        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        if (athenaCommonFlags.PoolHitsOutput.statusOn) :
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : Recording Metadata')
            from AthenaServices.AthenaServicesConf import AthenaOutputStream
            stream1_SimMetaData = AthenaOutputStream("StreamHITS_SimMetaData")
            stream1_SimMetaData.ItemList += [ "IOVMetaDataContainer#*" ]
            AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_metadata done')
        else :
            AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_metadata nothing done')



#--- CTB 2004 ----------------------------------------------------------
class CtbSim(TBSimSkeleton):
    """ Defines the simulation entity for the
        ATLAS Combined Test Beam (2004)
    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="CTB_G4Sim"         # assigns a name
        else:
            AtlasG4Eng.G4Eng.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_ctb_flags()

        # - switch off tasks
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

        # - see if there are beam-conditions
        if (simFlags.BeamConditions.statusOn and
            simFlags.BeamConditions.get_Value()):
                try:
                    AtlasG4Eng.G4Eng.log.info(' SimCtbKernel: '+\
                           ' loading CTB beam-conditions from the'+\
                           ' CTB run-condition file !!')
                    BeamCond=__import__(modulepath,globals(),locals(),prepath).BeamCond
                    beam_cond_obj=BeamCond.retrieve_Run(simFlags.RunNumber.get_Value())
                    beam_cond_obj._map()
                except:
                    AtlasG4Eng.G4Eng.log.warning(' SimCtbKernel: '+
                    'No particular beam conditions found for the run '+\
                    str(simFlags.RunNumber.get_Value()))
        # - switch off non-existing detectors
        DetFlags.FCal_setOff()
        DetFlags.HEC_setOff()
        # - Muons off for 13.0.0 release
        DetFlags.Muon_setOff()
        AtlasG4Eng.G4Eng.log.info('SimCtbKernel : Muons are not supported in releases >= 13.0.0')
        # -  photon runs under run-conditions. For all the photon runs with
        # run number > 2860 SCT and Pixel were not at the beam
        # -- LAr material studies
        if (simFlags.SimLayout.get_Value()=='ctbh8_lar-material'):
            DetFlags.SCT_setOff()
            DetFlags.Muon_setOff()
        if (simFlags.SimLayout.get_Value()=='ctbh8_calodata'):
            DetFlags.ID_setOff()
            DetFlags.Muon_setOff()
        #
        # Note: for combined    runs tag 04 in IDET
        #           photon      runs tag 08
        #           larmaterial runs tag 01
        if simFlags.SimLayout.get_Value() == 'ctbh8_combined' and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-04-01"
        elif simFlags.SimLayout.get_Value() == 'ctbh8_phothon'and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-08-01"
        elif simFlags.SimLayout.get_Value() == 'ctbh8_lar-material' and not simFlags.IdetOracleTag.statusOn:
            simFlags.IdetOracleTag = "InnerDetector-CTB-01-01"
        else:
            simFlags.IdetOracleTag.set_On()
        #
        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh8"
        ## mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.DetDescrVersion = 'CTB'
        __main__.globalflags = globalflags
        globalflags.DetDescrVersion = 'CTB'

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

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print out flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')

        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')


    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')
        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr +=Geo2G4Svc
        # We do not use top transforms for the CTB geometry
        Geo2G4Svc.GetTopTransform = False

        #--- GeoModel stuff ----------------------------------------------------
        from AtlasGeoModel import SetGeometryVersion

        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks =False

        from G4AtlasApps.SimFlags import simFlags
        GeoModelSvc.InDetVersionOverride =simFlags.IdetOracleTag.get_Value()

        if(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B-00'):
            GeoModelSvc.TileVersionOverride='TileTB-3B-00'
        elif(simFlags.GeoModelTileVersion.get_Value()=='TileTB-3B3EB-00'):
            GeoModelSvc.TileVersionOverride='TileTB-3B3EB-00'

        from AtlasGeoModel import GeoModelInit
        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
            if(DetFlags.em_on()):
                include("LArDetDescr/LArDetDescr_H8_joboptions.py")
        ##--- Persistency of test-beam layout ----------------------------------
        if jobproperties.AthenaCommonFlags.PoolHitsOutput.statusOn :
            include( "TBDetDescrAthenaPool/TBDetDescrAthenaPool_joboptions.py" )
            from TBDetDescrAlg.TBDetDescrAlgConf import TBDetDescrLoader
            from AthenaCommon.AlgSequence import AlgSequence
            job=AlgSequence()
            TBDetDescrLoader=TBDetDescrLoader()
            job+=TBDetDescrLoader
            TBDetDescrLoader.TBElementContainer = "TBElementCnt"
            TBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"
            include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )
            # do not read anything from StoreGate
            TBDetDescrLoader.ReadAction = 0
            # and write TBDetDescrContainer to StoreGate every event
            TBDetDescrLoader.WriteAction = 2
            TBDetDescrLoader.OutputLevel = 5
        #
        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

#--- Tile TB 2000-2003  ------------------------------------------------
class Tile2000_2003(TBSimSkeleton):
    """
       Describes the old standalone Tile test beams (2000-2003)
       in the same context as CTB. The mother volume (with Geant4
       name 'CTB') is now a box 20x20x20 m

    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="Tile2000_2003"  # assigns a name
        else:
            G4AtlasEngine.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_tbtile_flags()

        # - switch off tasks
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

        # not needed detectors
        DetFlags.ID_setOff()
        DetFlags.Muon_setOff()
        DetFlags.FCal_setOff()
        DetFlags.HEC_setOff()
        DetFlags.em_setOff()
        #
        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh8"
        # mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.DetDescrVersion = 'CTB'
        __main__.globalflags = globalflags
        globalflags.DetDescrVersion = 'CTB'

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

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')
        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')

    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')
        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr +=Geo2G4Svc
        Geo2G4Svc.GetTopTransform = False

        from AtlasGeoModel import SetGeometryVersion
        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks =False

        from G4AtlasApps.SimFlags import simFlags
        if (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B2EB'):
            # 2 Barrels + 2 Extended Barrels
            GeoModelSvc.TileVersionOverride='TileTB-2B2EB-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_2B1EB'):
            # 2 Barrels + 1 Extended Barrel
            GeoModelSvc.TileVersionOverride='TileTB-2B1EB-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_3B'):
            # 3 Barrels
            GeoModelSvc.TileVersionOverride='TileTB-3B-00'
        elif (simFlags.SimLayout.get_Value()=='tb_Tile2000_2003_5B'):
            # 5 Barrels
            GeoModelSvc.TileVersionOverride='TileTB-5B-00'

        from AtlasGeoModel import GeoModelInit
        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
        ##--- Persistency of test-beam layout ----------------------------------
        if jobproperties.AthenaCommonFlags.PoolHitsOutput.statusOn :
            include( "TBDetDescrAthenaPool/TBDetDescrAthenaPool_joboptions.py" )
            from TBDetDescrAlg.TBDetDescrAlgConf import TBDetDescrLoader
            from AthenaCommon.AlgSequence import AlgSequence
            job=AlgSequence()
            TBDetDescrLoader=TBDetDescrLoader()
            job+=TBDetDescrLoader
            TBDetDescrLoader.TBElementContainer = "TBElementCnt"
            TBDetDescrLoader.TBDetDescrManager  = "TBDetDescrMgr"
            include( "TBDetDescrCnv/TBDetDescrCnv_jobOptions.py" )
            # do not read anything from StoreGate
            TBDetDescrLoader.ReadAction = 0
            # and write TBDetDescrContainer to StoreGate every event
            TBDetDescrLoader.WriteAction = 2
            TBDetDescrLoader.OutputLevel = 5

        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

#--- LAr TB 2002-2004  ------------------------------------------------
class LArH6_TB(TBSimSkeleton):
    """
       Describes the old standalone LAr test beams at H6

    """
    def __init__(self):
        if not(AtlasG4Eng.G4Eng.Dict.has_key('simu_skeleton')):
            AtlasG4Eng.G4Eng.Dict['simu_skeleton']=self
            AtlasG4Eng.G4Eng.Name="LArH6"         # assigns a name
        else:
            G4AtlasEngine.log.warning(' SimSkeleton: the simulation '+\
            'has already a skeleton, you can find it in the '+\
            'G4AtlasEng.G4Eng.Dict()')

    @classmethod
    def _do_jobproperties(self):
        """ Place to handle JobProperties .
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties starting')

        ## Import extra flags if it hasn't already been done
        from G4AtlasApps.SimFlags import simFlags
        if not simFlags.extra_flags:
            simFlags.load_tbLArH6_flags()

        # - switch off tasks
        DetFlags.pileup.all_setOff()
        DetFlags.simulateLVL1.all_setOff()
        DetFlags.overlay.all_setOff()
        DetFlags.digitize.all_setOff()
        DetFlags.readRDOPool.all_setOff()
        DetFlags.makeRIO.all_setOff()
        DetFlags.writeBS.all_setOff()
        DetFlags.readRDOBS.all_setOff()
        DetFlags.readRIOBS.all_setOff()
        DetFlags.readRIOPool.all_setOff()
        DetFlags.writeRIOPool.all_setOff()
        DetFlags.writeRDOPool.all_setOff()

        DetFlags.ID_setOff()
        DetFlags.Muon_setOff()
        DetFlags.Tile_setOff()

        globalflags.DataSource = "geant4"
        globalflags.DetGeo = "ctbh6"
        # mgallas this is a hack! needed for GeoModel and IOVDbSvc
        # TODO: Wow! Is this really still needed?
        import __main__
        __main__.globalflags = globalflags
        if simFlags.SimLayout.get_Value() == "tb_LArH6_2003":
            globalflags.DetDescrVersion = 'ATLAS-H6-2003-02'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6_2002":
            globalflags.DetDescrVersion = 'ATLAS-H6-2002-01'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6EC_2002":
            globalflags.DetDescrVersion = 'ATLAS-TBEC-01'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()
        elif simFlags.SimLayout.get_Value() == "tb_LArH6_2004":
            globalflags.DetDescrVersion = 'ATLAS-H6-2004-00'
            __main__.DetDescrVersion = globalflags.DetDescrVersion.get_Value()

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

        if not simFlags.ISFRun:
            from G4AtlasApps.G4Atlas_Metadata import checkForSpecialConfigurationMetadata
            checkForSpecialConfigurationMetadata()

        ## Print flags
        if AtlasG4Eng.G4Eng.log.getEffectiveLevel()<40:
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing detector flags DetFlags')
            DetFlags.Print()
            AtlasG4Eng.G4Eng.log.info('SimCtbKernel : printing simulation flags simFlags')
            jobproperties.print_JobProperties('tree&value')
        jobproperties.lock_JobProperties()
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_JobProperties done')

    @classmethod
    def _do_external(self):
        """ Place to handle the external services: GeoModel, CondDB, etc.
        """
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'starting')

       #from LArH6Detectors import Det_Layouts
        from tbLArH6_calo import Det_Layouts

        from AthenaCommon.AppMgr import ServiceMgr
        from Geo2G4.Geo2G4Conf import Geo2G4Svc
        Geo2G4Svc=Geo2G4Svc()
        theApp.CreateSvc += [ "Geo2G4Svc" ]
        ServiceMgr += Geo2G4Svc
        # We do not use top transforms for the CTB geometry
        Geo2G4Svc.GetTopTransform = False

        #--- GeoModel stuff ----------------------------------------------------
        from AtlasGeoModel import SetGeometryVersion
        from AtlasGeoModel import GeoModelInit

        from AthenaCommon.Configurable import Configurable
        if Configurable.allConfigurables.get('GeoModelSvc'):
            GeoModelSvc=Configurable.allConfigurables.get('GeoModelSvc')
        else:
            GeoModelSvc=theApp.service('GeoModelSvc')
        GeoModelSvc.AlignCallbacks = False

        if(DetFlags.Calo_on()):
            # Common for the Calo
            include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py" )
            include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

        ## Explicitly create DetectorGeometrySvc - temporary fix
        from AthenaCommon.CfgGetter import getService, getPublicTool
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += getService('DetectorGeometrySvc')
        ServiceMgr.ToolSvc += getPublicTool('PhysicsListToolBase')
        AtlasG4Eng.G4Eng.log.info('SimSkeleton :: _do_external '+\
                                       'done')

        # mgallas do more here: this is not yet supported

