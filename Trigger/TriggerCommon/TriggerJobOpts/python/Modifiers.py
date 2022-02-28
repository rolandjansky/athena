# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

##############################################################
# Modifiers.py
#
#  Small classes that modify the setup for non-standard running
#  conditions and fixes that are not yet in the slice/detector jO
#
#  for now there are no options foreseen for the modifiers
#
#  Permanent fixes that are only applied online should be
#  put into runHLT_standalone.py
###############################################################

from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from AthenaCommon.Logging import logging
log = logging.getLogger('Modifiers.py')

_run_number = None   # set by runHLT_standalone
_lb_number = None   # set by runHLT_standalone

# Base class
class _modifier:
    def name(self):
        return self.__class__.__name__

    def __init__(self):
        log.warning('using modifier: %s', self.name())
        log.warning(self.__doc__)

    def preSetup(self, flags):
        pass #default is no action

    def postSetup(self, flags):
        pass #default is no action


###############################################################
# Detector maps and conditions
###############################################################

class BunchSpacing25ns(_modifier):
    """
    ID (and other settings) related to 25ns bunch spacing
    """
    def preSetup(self, flags):
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.bunchSpacing = 25
        flags.Beam.BunchSpacing = 25
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        InDetTrigFlags.InDet25nsec.set_Value_and_Lock(True)

class BunchSpacing50ns(_modifier):
    """
    ID (and other settings) related to 50ns bunch spacing
    """
    def preSetup(self, flags):
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.bunchSpacing = 50
        flags.Beam.BunchSpacing = 50
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        InDetTrigFlags.InDet25nsec.set_Value_and_Lock(False)

class noLArCalibFolders(_modifier):
    """
    We should not use LAr electronics calibration data
    """
    def preSetup(self, flags):
        from LArConditionsCommon.LArCondFlags import larCondFlags
        larCondFlags.LoadElecCalib=False

class reducedLArCalibFolders(_modifier):
    """
    Load minimum amount of LAr electronics calibration data to run on transparent data
    """
    def preSetup(self, flags):
        from LArConditionsCommon.LArCondFlags import larCondFlags
        larCondFlags.SingleVersion=True
        larCondFlags.OFCShapeFolder=""

class ForceMuonDataType(_modifier):
    """
    Hardcode muon data to be of type of atlas
      this determines which cabling service to use
    """
    def preSetup(self, flags):
        from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
        muonByteStreamFlags.RpcDataType = 'atlas'
        muonByteStreamFlags.MdtDataType = 'atlas'
        muonByteStreamFlags.TgcDataType = 'atlas'


class useNewRPCCabling(_modifier):
    """
    Switch to new RPC cabling code
    """
    def preSetup(self, flags):
        from MuonCnvExample.MuonCnvFlags import muonCnvFlags
        if hasattr(muonCnvFlags,'RpcCablingMode'):
            muonCnvFlags.RpcCablingMode.set_Value_and_Lock('new')

class MdtCalibFromDB(_modifier):
    """
    setup MDT calibration from DB instead of ascii
    """
    def postSetup(self, flags):
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from IOVDbSvc.CondDB import conddb
        # Use COOL for MDT conditions (this will be moved to muon jobO fragment soon)
        conddb.addFolder("MDT","/MDT/T0")
        conddb.addFolder("MDT","/MDT/RT")
        from MdtCalibDbCoolStrTool.MdtCalibDbCoolStrToolConf import MuonCalib__MdtCalibDbCoolStrTool
        MdtDbTool = MuonCalib__MdtCalibDbCoolStrTool("MdtCalibDbTool",defaultT0=580,TubeFolder="/MDT/T0",RtFolder="/MDT/RT",RT_InputFiles=["Muon_RT_default.data"])
        ToolSvc += MdtDbTool
        from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbSvc, MdtCalibrationSvc
        svcMgr += MdtCalibrationDbSvc( DBTool=MdtDbTool )
        svcMgr += MdtCalibrationSvc()
        svcMgr.MdtCalibrationSvc.DoTofCorrection = False

class MdtCalibFixedTag(_modifier):
    """
    Force use of specific MDT calibration tag
    THIS IS ONLY MEANT FOR NIGHTLY TEST WITH MC
    """
    def postSetup(self, flags):
        from IOVDbSvc.CondDB import conddb
        conddb.blockFolder("/MDT/T0")
        conddb.blockFolder("/MDT/RT")
        conddb.addFolder("MDT","/MDT/T0 <tag>HEAD</tag>",force=True)
        conddb.addFolder("MDT","/MDT/RT <tag>HEAD</tag>",force=True)

class SolenoidOff(_modifier):
    """
    Turn solenoid field OFF
    """
    def postSetup(self, flags):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq.AtlasFieldMapCondAlg.MapSoleCurrent = 0

class ToroidsOff(_modifier):
    """
    Turn toroid fields OFF
    """
    def postSetup(self, flags):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq.AtlasFieldMapCondAlg.MapToroCurrent = 0

class BFieldFromDCS(_modifier):
    """
    Read B-field currents from DCS (also works for MC)
    """
    def postSetup(self, flags):
        from IOVDbSvc.CondDB import conddb
        conddb._SetAcc("DCS_OFL","COOLOFL_DCS")
        conddb.addFolder("DCS_OFL","/EXT/DCS/MAGNETS/SENSORDATA",className="CondAttrListCollection")
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        # see ATLASRECTS-5604 for these settings:
        condSeq.AtlasFieldMapCondAlg.LoadMapOnStart = False
        condSeq.AtlasFieldMapCondAlg.UseMapsFromCOOL = True
        condSeq.AtlasFieldCacheCondAlg.UseDCS = True
        if hasattr(svcMgr,'HltEventLoopMgr'):
            svcMgr.HltEventLoopMgr.setMagFieldFromPtree = False

class BFieldAutoConfig(_modifier):
    """
    Read field currents from configuration ptree (athenaHLT)
    """
    def postSetup(self, flags):
        if hasattr(svcMgr,'HltEventLoopMgr'):
            svcMgr.HltEventLoopMgr.setMagFieldFromPtree = True

class useOracle(_modifier):
    """
    Disable the use of SQLite for COOL and geometry
    """
    def postSetup(self, flags):
        if hasattr(svcMgr,'DBReplicaSvc'):
            svcMgr.DBReplicaSvc.UseCOOLSQLite = False
            svcMgr.DBReplicaSvc.UseCOOLFrontier = True
            svcMgr.DBReplicaSvc.UseGeomSQLite = False


class noPileupNoise(_modifier):
    """
    Disable pileup noise correction
    """
    def preSetup(self, flags):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
        flags.Trigger.calo.doOffsetCorrection = False

class usePileupNoiseMu8(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=8)
    """
    def preSetup(self, flags):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45)

class usePileupNoiseMu20(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=20)
    """
    def preSetup(self, flags):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*20/8)

class usePileupNoiseMu30(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=30)
    """
    def preSetup(self, flags):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)


class forcePileupNoise(_modifier):
    """
    Use noise correction from run with pileup noise filled in for the online database
    """
    def postSetup(self, flags):
        from IOVDbSvc.CondDB import conddb
        conddb.addMarkup("/CALO/Noise/CellNoise","<forceRunNumber>178540</forceRunNumber>")


class forceTileRODMap(_modifier):
    """
    Configure Tile ROD map based on run-number (ATR-16290)
    """
    def postSetup(self, flags):
        if not hasattr(svcMgr.ToolSvc,"TileROD_Decoder"):
           from TileByteStream.TileByteStreamConf import TileROD_Decoder
           svcMgr.ToolSvc+=TileROD_Decoder()
        # Get run number from input file if running in athena
        global _run_number
        if _run_number is None:
            import PyUtils.AthFile as athFile
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
            _run_number = af.run_number[0]
        if _run_number<318000:  # use old readout scheme (default is new one)
            log.info('Reverting to pre-2017 Tile ROD map')
            #ToolSvc.TrigDataAccess.fullTileMode=False
            #ToolSvc.TileRegionSelectorTable.FullRODs=False
            svcMgr.ToolSvc.TileROD_Decoder.fullTileMode=0
        if _run_number>=343000:  # use 2018 version of cabling after 31-Jan-2018
            log.info('Setting RUN2a (2018) cabling in TileCal')
            svcMgr.TileCablingSvc.CablingType=5



class useOnlineLumi(_modifier):
    """
    Use online LuminosityTool
    """
    def preSetup(self, flags):
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
        LuminosityCondAlgOnlineDefault()


class forceConditions(_modifier):
    """
    Force all conditions (except prescales) to match run from input file
    """
    def postSetup(self, flags):
        # Do not override these folders:
        ignore = ['/TRIGGER/HLT/PrescaleKey']   # see ATR-22143

        # All time-based folders (from IOVDbSvc DEBUG message, see athena!38274)
        timebased = ['/TDAQ/OLC/CALIBRATIONS',
                     '/TDAQ/Resources/ATLAS/SCT/Robins',
                     '/SCT/DAQ/Config/ChipSlim',
                     '/SCT/DAQ/Config/Geog',
                     '/SCT/DAQ/Config/MUR',
                     '/SCT/DAQ/Config/Module',
                     '/SCT/DAQ/Config/ROD',
                     '/SCT/DAQ/Config/RODMUR',
                     '/SCT/HLT/DCS/HV',
                     '/SCT/HLT/DCS/MODTEMP',
                     '/MUONALIGN/Onl/MDT/BARREL',
                     '/MUONALIGN/Onl/MDT/ENDCAP/SIDEA',
                     '/MUONALIGN/Onl/MDT/ENDCAP/SIDEC',
                     '/MUONALIGN/Onl/TGC/SIDEA',
                     '/MUONALIGN/Onl/TGC/SIDEC']

        assert _run_number and _lb_number, f'Run or LB number is undefined ({_run_number}, {_lb_number})'

        from TrigCommon.AthHLT import get_sor_params
        sor = get_sor_params(_run_number)
        timestamp = sor['SORTime'] // int(1e9)

        for i,f in enumerate(svcMgr.IOVDbSvc.Folders):
            if any(name in f for name in ignore):
                continue
            if any(name in f for name in timebased):
                svcMgr.IOVDbSvc.Folders[i] += f'<forceTimestamp>{timestamp:d}</forceTimestamp>'
            else:
                svcMgr.IOVDbSvc.Folders[i] += f'<forceRunNumber>{_run_number:d}</forceRunNumber> <forceLumiblockNumber>{_lb_number:d}</forceLumiblockNumber>'


class forceAFPLinkNum(_modifier):
    """
    force AFP link number translator to use Run2 setup
    """
    def postSetup(self, flags):
        from AthenaCommon.AlgSequence import AthSequencer
        from AthenaCommon.CFElements import findAlgorithm
        AFPRecoSeq = AthSequencer("AFPRecoSeq")
        AFP_RawDataProv = findAlgorithm(AFPRecoSeq, "AFP_RawDataProvider")
        if AFP_RawDataProv:
            AFP_RawDataProv.ProviderTool.AFP_ByteStream2RawCnv.AFP_WordReadOut.AFP_LinkNumTranslator.ForceRunConfig = 2
        else:
            log.info('The forceAFPLinkNum Modifier has no effect because AFP_RawDataProvider is not configured to run')


###############################################################
# Algorithm modifiers
###############################################################

class rewriteLVL1(_modifier):
    """
    Write LVL1 results to ByteStream output, usually used together with rerunLVL1
    """
    # Example:
    # athenaHLT -c "setMenu='PhysicsP1_pp_run3_v1';rerunLVL1=True;rewriteLVL1=True;" --filesInput=input.data TriggerJobOpts/runHLT_standalone.py

    def preSetup(self, flags):
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamEncodersRecExSetup
        L1ByteStreamEncodersRecExSetup()

    def postSetup(self, flags):
        if not flags.Output.doWriteBS:
            log.warning('rewriteLVL1 is True but flags.Output.doWriteBS is False')
        if not flags.Trigger.writeBS:
            log.warning('rewriteLVL1 is True but flags.Trigger.writeBS is False')

        if flags.Trigger.Online.isPartition:
            # online
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.HltEventLoopMgr.RewriteLVL1 = True
            if flags.Trigger.enableL1MuonPhase1 or flags.Trigger.enableL1CaloPhase1:
                svcMgr.HltEventLoopMgr.L1TriggerResultRHKey = 'L1TriggerResult'
            if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1:
                svcMgr.HltEventLoopMgr.RoIBResultRHKey = 'RoIBResult'
        else:
            # offline
            from AthenaCommon.AlgSequence import AthSequencer
            from AthenaCommon.CFElements import findAlgorithm
            seq = AthSequencer('AthOutSeq')
            streamBS = findAlgorithm(seq, 'BSOutputStreamAlg')
            if flags.Trigger.enableL1MuonPhase1 or flags.Trigger.enableL1CaloPhase1:
                streamBS.ExtraInputs += [ ('xAOD::TrigCompositeContainer', 'StoreGateSvc+L1TriggerResult') ]
                streamBS.ItemList += [ 'xAOD::TrigCompositeContainer#L1TriggerResult' ]
            if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1MuonPhase1:
                streamBS.ExtraInputs += [ ('ROIB::RoIBResult', 'StoreGateSvc+RoIBResult') ]
                streamBS.ItemList += [ 'ROIB::RoIBResult#RoIBResult' ]


class DisableMdtT0Fit(_modifier):
    """
    Disable MDT T0 re-fit and use constants from COOL instead
    """
    def preSetup(self, flags):
        if flags.Trigger.doMuon:
            from MuonRecExample.MuonRecFlags import muonRecFlags
            muonRecFlags.doSegmentT0Fit.set_Value_and_Lock(False)

###############################################################
# Monitoring and misc.
###############################################################


class doCosmics(_modifier):
    """
    set beamType flag to cosmics data taking
    """
    def preSetup(self, flags):
       from AthenaCommon.BeamFlags import jobproperties
       jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')
       from AthenaConfiguration.Enums import BeamType
       flags.Beam.Type = BeamType.Cosmics


class enableALFAMon(_modifier):
    """
    turn on ALFA monitoring in the HLT
    """
    def postSetup(self, flags):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigALFAROBMonitor.py")
        except IncludeError:
            log.error('No ALFA ROB monitoring available.')


class nameAuditors(_modifier):
    """
    Turn on name auditor
    """
    def postSetup(self, flags):
        from AthenaCommon import CfgMgr
        theApp.AuditAlgorithms = True
        theApp.AuditServices = True
        theApp.AuditTools = True
        svcMgr.AuditorSvc += CfgMgr.NameAuditor()

class chronoAuditor(_modifier):
    """
    Turn on ChronoAuditor
    """
    def postSetup(self, flags):
        from AthenaCommon import CfgMgr
        theApp.AuditAlgorithms = True
        theApp.AuditServices = True
        theApp.AuditTools = True
        svcMgr.AuditorSvc += CfgMgr.ChronoAuditor()
        svcMgr.AuditorSvc.ChronoAuditor.CustomEventTypes = ['Start','Stop']
        svcMgr.ChronoStatSvc.ChronoDestinationCout = True
        svcMgr.ChronoStatSvc.PrintEllapsedTime = True

class fpeAuditor(_modifier):
    """
    Turn on FPEAuditor
    """
    def postSetup(self, flags):
        from AthenaCommon import CfgMgr
        theApp.AuditAlgorithms = True
        theApp.AuditServices = True
        theApp.AuditTools = True
        svcMgr.AuditorSvc += CfgMgr.FPEAuditor()
        svcMgr.AuditorSvc.FPEAuditor.NStacktracesOnFPE=1

class athMemAuditor(_modifier):
    """
    Turn on AthMemoryAuditor
    """
    def postSetup(self, flags):
        from AthenaCommon import CfgMgr
        theApp.AuditAlgorithms = True
        theApp.AuditServices = True
        theApp.AuditTools = True
        svcMgr.AuditorSvc += CfgMgr.AthMemoryAuditor(MaxStacktracesPerAlg=20,
                                                     DefaultStacktraceDepth=50,
                                                     StacktraceDepthPerAlg=["Stream1 100"])

class perfmon(_modifier):
    """
    Enable PerfMon
    """
    def preSetup(self, flags):
        from PerfMonComps.PerfMonFlags import jobproperties
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doPersistencyMonitoring = False

class enableSchedulerMon(_modifier):
    """
    Enable SchedulerMonSvc
    """
    def preSetup(self, flags):
        if not flags.Trigger.Online.isPartition:
            log.debug('SchedulerMonSvc currently only works with athenaHLT / online partition. Skipping setup.')
            return

        from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
        from TrigSteerMonitor.TrigSteerMonitorConfig import SchedulerMonSvcCfg
        CAtoGlobalWrapper(SchedulerMonSvcCfg, flags)
    
    def postSetup(self, flags):
        if flags.Trigger.Online.isPartition:
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.HltEventLoopMgr.MonitorScheduler = True

class enableFPE(_modifier):
    """
    Turn on floating point exceptions
    """
    def postSetup(self, flags):
        theApp.CreateSvc += ["FPEControlSvc"]


class doValidation(_modifier):
    """
    Enable validation mode (e.g. extra histograms)
    """
    def preSetup(self, flags):
        flags.Trigger.doValidationMonitoring = True

class autoConditionsTag(_modifier):
    """
    UGLY hack to setup conditions based on stream tag
    require RecExConfig.RecFlags.triggerStream to be set elsewhere
    Please do not use this - for trigger reprocessing only!!!
    """
    def preSetup(self, flags):
        from RecExConfig.AutoConfiguration import ConfigureConditionsTag
        ConfigureConditionsTag()

class enableCostMonitoring(_modifier):
    """
    Enable Cost Monitoring for online
    """
    def preSetup(self, flags):
        flags.Trigger.CostMonitoring.doCostMonitoring = True

class forceCostMonitoring(_modifier):
    """
    Enable Cost Monitoring and produce the monitoring collections in each event
    without requiring the HLT cost monitoring chain to be present and active.
    """
    def preSetup(self, flags):
        flags.Trigger.CostMonitoring.doCostMonitoring = True
        flags.Trigger.CostMonitoring.monitorAllEvents = True


class BeamspotFromSqlite(_modifier):
    """
    Read beamspot from sqlite file (./beampos.db)
    """
    def postSetup(self, flags):
        folders = []
        for f in svcMgr.IOVDbSvc.Folders:
            if f.find('/Indet/Onl/Beampos')!=-1:
                folders += ['<db>sqlite://;schema=beampos.db;dbname=COMP200</db> /Indet/Onl/Beampos <key>/Indet/Beampos</key> <tag>IndetBeamposOnl-HLT-UPD1-001-00</tag>']
            else:
                folders += [f]
        svcMgr.IOVDbSvc.Folders = folders

class LumiFromSqlite(_modifier):
    """
    Read beamspot from sqlite file (./lumi.db)
    """
    def postSetup(self, flags):
        folders = []
        for f in svcMgr.IOVDbSvc.Folders:
            if f.find('/TRIGGER/LUMI/HLTPrefLumi')!=-1:
                folders += ['<db>sqlite://;schema=lumi.db;dbname=CONDBR2</db> /TRIGGER/LUMI/HLTPrefLumi <tag>HLTPrefLumi-HLT-UPD1-001-00</tag>']
            else:
                folders += [f]
        svcMgr.IOVDbSvc.Folders = folders


class useDynamicAlignFolders(_modifier):
    """
    enable the new (2016-) alignment scheme
    """
    def preSetup(self, flags):
        from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
        InDetGeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True)


class doRuntimeNaviVal(_modifier):
    """
    Checks the validity of each Decision Object produced by a HypoAlg, including all of its
    parents all the way back to the HLT Seeding. Potentially CPU expensive.
    """
    def preSetup(self, flags):
        log.info("Enabling Runtime Trigger Navigation Validation")
        flags.Trigger.doRuntimeNaviVal = True

