# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger('Modifiers.py')

_run_number = None   # set by runHLT_standalone

# Base class
class _modifier:
    def name(self):
        return self.__class__.__name__

    def __init__(self):
        log.warning('using modifier: %s', self.name())
        log.warning(self.__doc__)

    def preSetup(self):
        pass #default is no action

    def postSetup(self):
        pass #default is no action


###############################################################
# Detector maps and conditions
###############################################################

class streamingOnly(_modifier):
    """
    Turn off things not needed for streaming only setup
    """
    def preSetup(self):
        from MuonRecExample.MuonRecFlags import muonRecFlags
        muonRecFlags.doMDTs=False
        muonRecFlags.doRPCs=False
        muonRecFlags.doTGCs=False
        TriggerFlags.doID=False
        TriggerFlags.doCalo=False

    def postSetup(self):
        #remove MDT folders as one of them takes 10s to load
        svcMgr.IOVDbSvc.Folders=[]
        Folders=[]
        for text in svcMgr.IOVDbSvc.Folders:
            if text.find("/MDT")<0:
                Folders+=[text]
        svcMgr.IOVDbSvc.Folders=Folders

        # There is no magnetic field service in this setup
        if hasattr(svcMgr,'HltEventLoopMgr'):
            svcMgr.HltEventLoopMgr.setMagFieldFromPtree = False

class noID(_modifier):
    """
    Turning of ID - make sure no algorithm needs it!
    """
    def preSetup(self):
        TriggerFlags.doID=False

class noCalo(_modifier):
    """
    Turning of Calorimeter - make sure no algorithm needs it!
    """
    def preSetup(self):
        TriggerFlags.doCalo=False


class BunchSpacing25ns(_modifier):
    """
    ID (and other settings) related to 25ns bunch spacing
    """
    def preSetup(self):
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        InDetTrigFlags.InDet25nsec.set_Value_and_Lock(True)

class BunchSpacing50ns(_modifier):
    """
    ID (and other settings) related to 50ns bunch spacing
    """
    def preSetup(self):
        from AthenaCommon.BeamFlags import jobproperties
        jobproperties.Beam.bunchSpacing.set_Value_and_Lock(50)
        from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
        InDetTrigFlags.InDet25nsec.set_Value_and_Lock(False)

class noLArCalibFolders(_modifier):
    """
    We should not use LAr electronics calibration data
    """
    def preSetup(self):
        from LArConditionsCommon.LArCondFlags import larCondFlags
        larCondFlags.LoadElecCalib=False

class reducedLArCalibFolders(_modifier):
    """
    Load minimum amount of LAr electronics calibration data to run on transparent data
    """
    def preSetup(self):
        from LArConditionsCommon.LArCondFlags import larCondFlags
        larCondFlags.SingleVersion=True
        larCondFlags.OFCShapeFolder=""


class useHLTMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doHLT() and TriggerFlags.doMuon():
            from MuonRecExample import MuonAlignConfig  # noqa: F401
            #temporary hack to workaround DB problem - should not be needed any more
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MDT/BARREL')!=-1:
                    f+='<key>/MUONALIGN/MDT/BARREL</key>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders
            svcMgr.AmdcsimrecAthenaSvc.AlignmentSource=2


class useRecentHLTMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doHLT() and TriggerFlags.doMuon():
            from MuonRecExample import MuonAlignConfig  # noqa: F401
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MUONALIGN')!=-1 and f.find('TGC')==-1:
                    f+='<forceTimestamp> 1384749388 </forceTimestamp>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders


class ForceMuonDataType(_modifier):
    """
    Hardcode muon data to be of type of atlas
      this determines which cabling service to use
    """
    def preSetup(self):
        from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
        muonByteStreamFlags.RpcDataType = 'atlas'
        muonByteStreamFlags.MdtDataType = 'atlas'
        muonByteStreamFlags.TgcDataType = 'atlas'


class RPCcablingHack(_modifier):
    """
    Hack for low pt thresholds - doesn't seem to do anything for HLT?
    """
    def postSetup(self):
        if hasattr(svcMgr,'RPCcablingSimSvc'):
            svcMgr.RPCcablingSimSvc.HackFor1031 = True

class useNewRPCCabling(_modifier):
    """
    Switch to new RPC cabling code
    """
    def preSetup(self):
        from MuonCnvExample.MuonCnvFlags import muonCnvFlags
        if hasattr(muonCnvFlags,'RpcCablingMode'):
            muonCnvFlags.RpcCablingMode.set_Value_and_Lock('new')

class MdtCalibFromDB(_modifier):
    """
    setup MDT calibration from DB instead of ascii
    """
    def postSetup(self):
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
    def postSetup(self):
        from IOVDbSvc.CondDB import conddb
        conddb.blockFolder("/MDT/T0")
        conddb.blockFolder("/MDT/RT")
        conddb.addFolder("MDT","/MDT/T0 <tag>HEAD</tag>",force=True)
        conddb.addFolder("MDT","/MDT/RT <tag>HEAD</tag>",force=True)

class SolenoidOff(_modifier):
    """
    Turn solenoid field OFF
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq.AtlasFieldMapCondAlg.MapSoleCurrent = 0

class ToroidsOff(_modifier):
    """
    Turn toroid fields OFF
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq.AtlasFieldMapCondAlg.MapToroCurrent = 0

class BFieldFromDCS(_modifier):
    """
    Read B-field currents from DCS (also works for MC)
    """
    def postSetup(self):
        from IOVDbSvc.CondDB import conddb
        conddb._SetAcc("DCS_OFL","COOLOFL_DCS")
        conddb.addFolder("DCS_OFL","/EXT/DCS/MAGNETS/SENSORDATA")
        from AthenaCommon.AlgSequence import AthSequencer
        condSeq = AthSequencer("AthCondSeq")
        condSeq.AtlasFieldCacheCondAlg.UseDCS = True

class BFieldAutoConfig(_modifier):
    """
    Read field currents from configuration ptree (athenaHLT)
    """
    def postSetup(self):
        if hasattr(svcMgr,'HltEventLoopMgr'):
            svcMgr.HltEventLoopMgr.setMagFieldFromPtree = True

class allowCOOLUpdates(_modifier):
    """
    Enable COOL folder updates during the run
    """
    def postSetup(self):
        if hasattr(svcMgr,'HltEventLoopMgr'):
            svcMgr.HltEventLoopMgr.CoolUpdateTool.enable()

class useOracle(_modifier):
    """
    Disable the use of SQLite for COOL and geometry
    """
    def postSetup(self):
        if hasattr(svcMgr,'DBReplicaSvc'):
            svcMgr.DBReplicaSvc.UseCOOLSQLite = False
            svcMgr.DBReplicaSvc.UseCOOLFrontier = True
            svcMgr.DBReplicaSvc.UseGeomSQLite = False


class noPileupNoise(_modifier):
    """
    Disable pileup noise correction
    """
    def preSetup(self):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(0)
        TriggerFlags.doCaloOffsetCorrection.set_Value_and_Lock(False)

class usePileupNoiseMu8(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=8)
    """
    def preSetup(self):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45)

class usePileupNoiseMu20(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=20)
    """
    def preSetup(self):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*20/8)

class usePileupNoiseMu30(_modifier):
    """
    Enable pileup noise correction for fixed luminosity point (mu=30)
    """
    def preSetup(self):
        from CaloTools.CaloNoiseFlags import jobproperties
        jobproperties.CaloNoiseFlags.FixedLuminosity.set_Value_and_Lock(1.45*30/8)


class forcePileupNoise(_modifier):
    """
    Use noise correction from run with pileup noise filled in for the online database
    """
    def postSetup(self):
        from IOVDbSvc.CondDB import conddb
        conddb.addMarkup("/CALO/Noise/CellNoise","<forceRunNumber>178540</forceRunNumber>")


class forceTileRODMap(_modifier):
    """
    Configure Tile ROD map based on run-number (ATR-16290)
    """
    def postSetup(self):
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
    def preSetup(self):
        from LumiBlockComps.LuminosityCondAlgDefault import LuminosityCondAlgOnlineDefault
        LuminosityCondAlgOnlineDefault()


class forceConditions(_modifier):
    """
    Force all conditions (except prescales) to match run from input file
    """
    def postSetup(self):
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

        from RecExConfig.RecFlags import rec
        from TrigCommon.AthHLT import get_sor_params
        sor = get_sor_params(rec.RunNumber())

        for i,f in enumerate(svcMgr.IOVDbSvc.Folders):
            if any(name in f for name in ignore):
                continue
            if any(name in f for name in timebased):
                svcMgr.IOVDbSvc.Folders[i] += '<forceTimestamp>%d</forceTimestamp>' % (sor['SORTime'] // int(1e9))
            else:
                svcMgr.IOVDbSvc.Folders[i] += '<forceRunNumber>%d</forceRunNumber>' % sor['RunNumber']



###############################################################
# Algorithm modifiers
###############################################################

class physicsZeroStreaming(_modifier):
    """
    set all physics chains to stream prescale 0 except streamer chains
    """
    def preSetup(self):
        TriggerFlags.zero_stream_prescales=True

class physicsPTmode(_modifier):
    """
    set all physics chains to PT=1 except streamer chains
    """
    def preSetup(self):
        TriggerFlags.physics_pass_through=True

class disableIBLInTracking(_modifier):
    """
    Turn off IBL in tracking algorithms (data still available for PEB etc)
    """

    def postSetup(self):
        svcMgr.SpecialPixelMapSvc.MaskLayers = True
        svcMgr.SpecialPixelMapSvc.LayersToMask = [0]


class doMuonRoIDataAccess(_modifier):
    """
    Use RoI based decoding of muon system
    """
    def preSetup(self):
        TriggerFlags.MuonSlice.doEFRoIDrivenAccess=True

class rerunLVL1(_modifier):
    """
    Reruns the L1 simulation on real data
    """
    def preSetup(self):

        from AthenaCommon.Include import include
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        #configure LVL1 config svc with xml file
        from TrigConfigSvc.TrigConfigSvcConfig import L1TopoConfigSvc
        L1TopoConfigSvc = L1TopoConfigSvc()
        L1TopoConfigSvc.XMLMenuFile = TriggerFlags.inputL1TopoConfigFile()

        #configure LVL1 config svc with xml file
        from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
        LVL1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
        LVL1ConfigSvc.XMLMenuFile = TriggerFlags.inputLVL1configFile()

        # rerun L1calo simulation
        include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
        include ("TrigT1CaloSim/TrigT1CaloSimJobOptions_ReadTT.py" )

        #rederive MuCTPI inputs to CTP from muon RDO
        #writes this to the usual MuCTPICTP storegate location
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        if ConfigFlags.Trigger.enableL1Phase1:
            from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1_on_RDO as L1Muctpi_on_RDO
        else:
            from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
        topSequence += L1Muctpi_on_RDO()
        topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "L1MuCTPItoCTPLocation"
        topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "L1MuCTPItoRoIBLocation"

        # Add L1TopoSimulation if it was not already added, e.g. by L1TopoROBMonitor
        topSequenceAlgNames=[alg.getName() for alg in topSequence.getChildren()]
        if 'L1TopoSimulation' not in topSequenceAlgNames:
            from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
            topSequence += L1TopoSimulation()
            log.info( "adding L1TopoSimulation() to topSequence" )

            if ConfigFlags.Trigger.enableL1Phase1:
                from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1Tool as L1MuctpiTool
            else:
                from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
            from AthenaCommon.AppMgr import ToolSvc
            ToolSvc += L1MuctpiTool()
            topSequence.L1TopoSimulation.MuonInputProvider.MuctpiSimTool = L1MuctpiTool()

            # enable the reduced (coarse) granularity topo simulation
            # currently only for MC
            from AthenaCommon.GlobalFlags  import globalflags
            if globalflags.DataSource()!='data':
                log.info("Muon eta/phi encoding with reduced granularity for MC (L1 Simulation)")
                topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
            else:
                log.info("Muon eta/phi encoding with full granularity for data (L1 Simulation) - should be faced out")
                topSequence.L1TopoSimulation.MuonInputProvider.MuonEncoding = 1
        else:
            log.info( "not adding L1TopoSimulation() to topSequence as it is already there" )
        log.debug( "topSequence: %s", topSequenceAlgNames )

        from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
        ctpSimulation = CTPSimulationOnData("CTPSimulation")
        ctpSimulation.DoBCM   = False # TriggerFlags.doBcm()
        ctpSimulation.DoLUCID = False # TriggerFlags.doLucid()
        ctpSimulation.DoZDC   = False # TriggerFlags.doZdc()
        ctpSimulation.DoBPTX  = False
        ctpSimulation.DoMBTS  = False

        topSequence += ctpSimulation

        from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
        topSequence += RoIBuilder("RoIBuilder")
        # For backwards compatibility with 16.1.X (see Savannah #85927)
        if "RoIOutputLocation_Rerun" in topSequence.CTPSimulation.properties():
            topSequence.RoIBuilder.CTPSLinkLocation = 'CTPSLinkLocation_Rerun'

        # Get run number from input file if running in athena
        global _run_number
        if _run_number is None:
            import PyUtils.AthFile as athFile
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
            _run_number = af.run_number[0]

        # On run-1 data, need to re-create the L1 menu with correct L1Calo energy scale (ATR-10174)
        if _run_number is not None and _run_number<222222:
            TriggerFlags.useRun1CaloEnergyScale = True
            TriggerFlags.outputLVL1configFile = 'LVL1config_'+TriggerFlags.triggerMenuSetup()+'_Run1CaloEnergyScale.xml'
            TriggerFlags.readLVL1configFromXML = False
            log.warning('rerunLVL1: Re-creating L1 menu with run-1 L1Calo energy scale!')


    def postSetup(self):
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "MuCTPI_RIO/MUCTPI_RIO",
            "CTP_RIO/CTP_RIO"
            ]

class rerunDMLVL1(_modifier):
    """
    Reruns the L1 simulation on real data with dead material corrections
    """
    def preSetup(self):

         from AthenaCommon.Include import include
         from AthenaCommon.AlgSequence import AlgSequence
         topSequence = AlgSequence()

         #write cool objects to detector store
         from IOVDbSvc.CondDB import conddb
         conddb.addFolderWithTag('TRIGGER', "/TRIGGER/LVL1/BunchGroupContent", "HEAD")
         conddb.addFolder('TRIGGER', '/TRIGGER/LVL1/CTPCoreInputMapping')

         #configure LVL1 config svc with xml file
         from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc
         LVL1ConfigSvc = LVL1ConfigSvc("LVL1ConfigSvc")
         LVL1ConfigSvc.XMLFile = TriggerFlags.inputLVL1configFile()

         # rerun L1calo simulation
         include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
         include ("TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py")
         include ("TrigT1CaloSim/TrigT1CaloSimJobOptions_ReprocessDM.py" )

         #Run MuCTPI simulation (before or after importing DeriveSim??)
         #rederive MuCTPI inputs to CTP from muon RDO
         #writes this to the usual MuCTPICTP storegate location
         from AthenaConfiguration.AllConfigFlags import ConfigFlags
         if ConfigFlags.Trigger.enableL1Phase1:
             from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1_on_RDO as L1Muctpi_on_RDO
         else:
             from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiPhase1_on_RDO as L1Muctpi_on_RDO
         topSequence += L1Muctpi_on_RDO()
         topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "L1MuCTPItoCTPLocation"
         topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "L1MuCTPItoRoIBLocation"

         from TrigT1CTMonitoring.TrigT1CTMonitoringConf import TrigT1CTMonitoring__DeriveSimulationInputs as DeriveSimulationInputs
         topSequence += DeriveSimulationInputs(do_MuCTPI_input=True,
                                               do_L1Calo_input=False,
                                               do_L1Calo_sim=True)

         from TrigT1CTP.TrigT1CTPConfig import CTPSimulationOnData
         topSequence += CTPSimulationOnData("CTPSimulation")

         from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
         topSequence += RoIBuilder("RoIBuilder")
         # For backwards compatibility with 16.1.X (see Savannah #85927)
         if "RoIOutputLocation_Rerun" in topSequence.CTPSimulation.properties():
             topSequence.RoIBuilder.CTPSLinkLocation = '/Event/CTPSLinkLocation_Rerun'

    def postSetup(self):
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [
            "MuCTPI_RIO/MUCTPI_RIO",
            "CTP_RIO/CTP_RIO"
            ]


class rewriteLVL1(_modifier):
    """
    Write LVL1 results to ByteStream output, usually used together with rerunLVL1
    """
    # Example:
    # athenaHLT -c "setMenu='PhysicsP1_pp_run3_v1';rerunLVL1=True;rewriteLVL1=True;" --filesInput=input.data TriggerJobOpts/runHLT_standalone.py

    def preSetup(self):
        from TrigT1ResultByteStream.TrigT1ResultByteStreamConfig import L1ByteStreamEncodersRecExSetup
        L1ByteStreamEncodersRecExSetup()

    def postSetup(self):
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        if not TriggerFlags.writeBS:
            log.warning('rewriteLVL1 is True but TriggerFlags.writeBS is False')
        if not ConfigFlags.Output.doWriteBS:
            log.warning('rewriteLVL1 is True but ConfigFlags.Output.doWriteBS is False')
        if not ConfigFlags.Trigger.writeBS:
            log.warning('rewriteLVL1 is True but ConfigFlags.Trigger.writeBS is False')

        if ConfigFlags.Trigger.Online.isPartition:
            # online
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.HltEventLoopMgr.RewriteLVL1 = True
        else:
            # offline
            from AthenaCommon.AlgSequence import AthSequencer
            from AthenaCommon.CFElements import findAlgorithm
            seq = AthSequencer('AthOutSeq')
            streamBS = findAlgorithm(seq, 'BSOutputStreamAlg')
            if ConfigFlags.Trigger.enableL1Phase1:
                out_type = 'xAOD::TrigCompositeContainer'
                out_name = 'L1TriggerResult'
            else:
                out_type = 'ROIB::RoIBResult'
                out_name = 'RoIBResult'
            streamBS.ExtraInputs += [ (out_type, 'StoreGateSvc+'+out_name) ]
            streamBS.ItemList += [ out_type+'#'+out_name ]


class writeBS(_modifier):
    """
    Write bytestream output in athena
    """
    def postSetup(self):
        from AthenaCommon.Include import include
        include("TriggerJobOpts/BStoBS_post.py")

class DisableMdtT0Fit(_modifier):
    """
    Disable MDT T0 re-fit and use constants from COOL instead
    """
    def preSetup(self):
        if TriggerFlags.doMuon():
            from MuonRecExample.MuonRecFlags import muonRecFlags
            muonRecFlags.doSegmentT0Fit.set_Value_and_Lock(False)

###############################################################
# Monitoring and misc.
###############################################################


class doCosmics(_modifier):
    """
    set beamType flag to cosmics data taking
    """
    def preSetup(self):
       from AthenaCommon.BeamFlags import jobproperties
       jobproperties.Beam.beamType.set_Value_and_Lock('cosmics')


class CRCcheck(_modifier):
    """
    turn on CRC checks in the HLT
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from AthenaCommon.Include import include, IncludeError
            try:
                include("TrigOnlineMonitor/TrigROBMonitor.py")
            except IncludeError:
                log.error('No ROB monitoring available.')

class CRCstream(_modifier):
    """
    turn on CRC checks in the HLT and sends bad events to debug stream
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from AthenaCommon.Include import include,IncludeError
            try:
                include("TrigOnlineMonitor/TrigROBMonitor.py")
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence = AlgSequence()
                topSequence.ROBMonitor.SetDebugStream = True
            except IncludeError:
                log.error('No ROB monitoring available.')

class L1TopoCheck(_modifier):
    """
    turn on L1Topo checks in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigL1TopoWriteValData.py")
        except IncludeError:
            log.error('No L1Topo WriteValData available.')

class muCTPicheck(_modifier):
    """
    turn on muCTPi checks in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigMuCTPiROBMonitor.py")
        except IncludeError:
            log.error('No muCTPi ROB monitoring available.')

class muCTPistream(_modifier):
    """
    turn on muCTPi checks in the HLT and sends bad events to debug stream
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from AthenaCommon.Include import include,IncludeError
            try:
                include("TrigOnlineMonitor/TrigMuCTPiROBMonitor.py")
                from AthenaCommon.AlgSequence import AlgSequence
                topSequence = AlgSequence()
                topSequence.MuCTPiROBMonitor.SetDebugStream = True
            except IncludeError:
                log.error('No muCTPi ROB monitoring available.')

class enableALFAMon(_modifier):
    """
    turn on ALFA monitoring in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigALFAROBMonitor.py")
        except IncludeError:
            log.error('No ALFA ROB monitoring available.')


class nameAuditors(_modifier):
    """
    Turn on name auditor
    """
    def postSetup(self):
        from AthenaCommon import CfgMgr
        theApp.AuditAlgorithms = True
        theApp.AuditServices = True
        theApp.AuditTools = True
        svcMgr.AuditorSvc += CfgMgr.NameAuditor()

class chronoAuditor(_modifier):
    """
    Turn on ChronoAuditor
    """
    def postSetup(self):
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
    def postSetup(self):
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
    def postSetup(self):
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
    def preSetup(self):
        from PerfMonComps.PerfMonFlags import jobproperties
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doPersistencyMonitoring = False

class enableSchedulerMon(_modifier):
    """
    Enable SchedulerMonSvc
    """
    def preSetup(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        if not flags.Trigger.Online.isPartition:
            log.debug('SchedulerMonSvc currently only works with athenaHLT / online partition. Skipping setup.')
            return

        from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
        from TrigSteerMonitor.TrigSteerMonitorConfig import SchedulerMonSvcCfg
        CAtoGlobalWrapper(SchedulerMonSvcCfg, flags)
    
    def postSetup(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        if flags.Trigger.Online.isPartition:
            from AthenaCommon.AppMgr import ServiceMgr as svcMgr
            svcMgr.HltEventLoopMgr.MonitorScheduler = True

class enableFPE(_modifier):
    """
    Turn on floating point exceptions
    """
    def postSetup(self):
        theApp.CreateSvc += ["FPEControlSvc"]


class doValidation(_modifier):
    """
    Force validation mode (i.e. no message timestamps)
    """

    def preSetup(self):
        TriggerFlags.Online.doValidation = True
        # Replace Online with Validation monitoring
        TriggerFlags.enableMonitoring = filter(lambda x:x!='Online', TriggerFlags.enableMonitoring())+['Validation']
        for m in self.modifiers:
            m.preSetup()

class autoConditionsTag(_modifier):
    """
    UGLY hack to setup conditions based on stream tag
    require RecExConfig.RecFlags.triggerStream to be set elsewhere
    Please do not use this - for trigger reprocessing only!!!
    """
    def preSetup(self):
        from RecExConfig.AutoConfiguration import ConfigureConditionsTag
        ConfigureConditionsTag()

class enableCostMonitoring(_modifier):
    """
    Enable Cost Monitoring for online
    """
    def preSetup(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.CostMonitoring.doCostMonitoring = True

class forceCostMonitoring(_modifier):
    """
    Enable Cost Monitoring and produce the monitoring collections in each event
    without requiring the HLT cost monitoring chain to be present and active.
    """
    def preSetup(self):
        from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
        flags.Trigger.CostMonitoring.doCostMonitoring = True
        flags.Trigger.CostMonitoring.monitorAllEvents = True


class BeamspotFromSqlite(_modifier):
    """
    Read beamspot from sqlite file (./beampos.db)
    """
    def postSetup(self):
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
    def postSetup(self):
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
    def preSetup(self):
        from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
        InDetGeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True)


class doRuntimeNaviVal(_modifier):
    """
    Checks the validity of each Decision Object produced by a HypoAlg, including all of its
    parents all the way back to the L1 decoder. Potentially CPU expensive.
    """
    def preSetup(self):
        log.info("Enabling Runtime Trigger Navigation Validation")
        from AthenaConfiguration.AllConfigFlags import ConfigFlags
        ConfigFlags.Trigger.doRuntimeNaviVal = True

