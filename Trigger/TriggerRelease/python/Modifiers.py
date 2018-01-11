# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################
# Modifiers.py
#
#  Small classes that modify the setup for non-standard running
#  conditions and fixes that are not yet in the slice/detector jO
#
#  for now there are no options foreseen for the modifiers
#
#  Permanent fixes that are only applied online should be
#  put into Trigger_topOptions_standalone.py
###############################################################

from AthenaCommon.Constants import *
from AthenaCommon.AppMgr import ToolSvc
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
        print 'WARNING: using modifier:',self.name()
        print self.__doc__
    
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
        if hasattr(svcMgr,'HltEventLoopMgr'): svcMgr.HltEventLoopMgr.setMagFieldFromPtree = False
         
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


class ForceNewTRTDigiversion(_modifier):
    """
    setup TRT to use latest digi version (for rel >=14 MC)
    determines which t0 shift to use
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"TRT_DriftFunctionTool"):
            ToolSvc.TRT_DriftFunctionTool.AllowDigiVersionOverride=True
            ToolSvc.TRT_DriftFunctionTool.ForcedDigiVersion=11
        if hasattr(ToolSvc,"InDetTrigTRT_DriftFunctionTool"):
            ToolSvc.InDetTrigTRT_DriftFunctionTool.AllowDigiVersionOverride=True
            ToolSvc.InDetTrigTRT_DriftFunctionTool.ForcedDigiVersion=11

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

class TRTCalibFromDB(_modifier):
    """
    setup TRT calibration from DB 
    """
    def preSetup(self):
        if TriggerFlags.doID():
            from AthenaCommon.AppMgr import ToolSvc
            from TRT_DriftFunctionTool.TRT_DriftFunctionToolConf import TRT_DriftFunctionTool
            TRT_DriftFunctionTool = TRT_DriftFunctionTool(name = "TRT_DriftFunctionTool")
            ToolSvc += TRT_DriftFunctionTool

            from IOVDbSvc.CondDB import conddb
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DetGeo=='commis':
                conddb.addFolder("TRT","/TRT/Calib/RT<tag>TrtCalibRt-bootstrap-01</tag>" )
                conddb.addFolder("TRT","/TRT/Calib/T0<tag>TrtCalibT0-bootstrap-01</tag>" )
                conddb.addOverride("/TRT/Calib/RT","TrtCalibRt-bootstrap-01")
                conddb.addOverride("/TRT/Calib/T0","TrtCalibT0-bootstrap-01")
            else:
                if not conddb.folderRequested("/TRT/Calib/RT"):
                    conddb.addFolder("TRT","/TRT/Calib/RT" )
                if not conddb.folderRequested("/TRT/Calib/T0"):
                    conddb.addFolder("TRT","/TRT/Calib/T0" )   

class disableTRTActiveFraction(_modifier):
    """
    remove TRT ActiveFractionSvc from the configuration 
    temporarily disable a non-essential component to work around M5 crashes 
    https://its.cern.ch/jira/browse/ATR-9017
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if hasattr(svcMgr,'InDetTRTActiveFractionSvc'):
           delattr(svcMgr,'InDetTRTActiveFractionSvc')
           ToolSvc.InDetTrigTRTDriftCircleCut.UseActiveFractionSvc=False
           ToolSvc.InDetTrigTRTDriftCircleCut.TrtConditionsSvc=""
           #print 'jmasik', svcMgr.getAllChildren()
           #print ToolSvc.InDetTrigTRTDriftCircleCut


class TrackingInStandBy(_modifier):
    """
    adjust settings of the beamspot tracking to work with SCT in standby
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"StrategyB_BeamSpot"):
            ToolSvc.StrategyB_BeamSpot.Map_ForcedTurnedOffLayers = [0,1,2,7,8,9]
            ToolSvc.StrategyB_BeamSpot.Extension_CutSpacePointsMiss2 = 3
            ToolSvc.StrategyB_BeamSpot.Extension_SpacePointsMiss2 = 4
            ToolSvc.StrategyB_BeamSpot.Vertexing_Enable = False


class setRecCommissioning(_modifier):
    """
    Use commissioning setup
    """
    def preSetup(self):
         from RecExConfig.RecFlags import rec
         rec.Commissioning=True 

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


class shiftMBTSTiming(_modifier):
    """
    Shift MBTS timing by 45 ns in T2MBTS hypos
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            for child in topSequence.TrigSteer_HLT.getChildren():
                if child.getType()=='T2MbtsHypo':
                    child.GlobalTimeOffset=45

class useL2MuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doLVL2():
            from MuonRecExample import MuonAlignConfig
            #temporary hack to workaround DB problem - should not be needed any more
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MDT/BARREL')!=-1:
                    f+='<key>/MUONALIGN/MDT/BARREL</key>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders
            svcMgr.AmdcsimrecAthenaSvc.AlignmentSource=2

class useEFMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doEF():
            from MuonRecExample import MuonAlignConfig
            #temporary hack to workaround DB problem
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MDT/BARREL')!=-1:
                    f+='<key>/MUONALIGN/MDT/BARREL</key>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders

class useHLTMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from MuonRecExample import MuonAlignConfig
            #temporary hack to workaround DB problem - should not be needed any more
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MDT/BARREL')!=-1:
                    f+='<key>/MUONALIGN/MDT/BARREL</key>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders
            svcMgr.AmdcsimrecAthenaSvc.AlignmentSource=2


class useRecentL2MuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doLVL2():
            from MuonRecExample import MuonAlignConfig
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MUONALIGN')!=-1 and f.find('TGC')==-1:
                    f+='<forceTimestamp> 1384749388 </forceTimestamp>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders

class useRecentEFMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doEF():
            from MuonRecExample import MuonAlignConfig
            folders=svcMgr.IOVDbSvc.Folders
            newFolders=[]
            for f in folders:
                if f.find('MUONALIGN')!=-1 and f.find('TGC')==-1:
                    f+='<forceTimestamp> 1384749388 </forceTimestamp>'
                newFolders.append(f)
            svcMgr.IOVDbSvc.Folders=newFolders

class useRecentHLTMuonAlign(_modifier):
    """
    Apply muon alignment
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            print "useHLTMuonAlign"
            from MuonRecExample import MuonAlignConfig
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

class TGCCablingPatch(_modifier):
    """
    TGC cabling IDs are different
    """
    def preSetup(self):
        from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiSvc
        TGCRecRoiSvc.PatchForP5 = True

class TGCCablingFixOn(_modifier):
    """
    Fix TGC cabling problem in real data taking in fall 2008
    """
    def preSetup(self):
        from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiSvc
        TGCRecRoiSvc.PatchForRoIWord=True

class TGCCablingFixOff(_modifier):
    """
    Turn off the TGC cabling problem in real data taking in fall 2008 - should be off for MC
    """
    def preSetup(self):
        from TrigT1TGCRecRoiSvc.TrigT1TGCRecRoiConfig import TGCRecRoiSvc
        TGCRecRoiSvc.PatchForRoIWord=False

class openThresholdRPCCabling(_modifier):
    """
    RPC cabling need to tell CMA to use open thresholds
    see http://alxr.usatlas.bnl.gov/lxr/source/atlas/MuonSpectrometer/MuonCablings/RPCcabling/src/EtaCMA.cxx#448
    and other entries at http://alxr.usatlas.bnl.gov/lxr/ident?i=cosmic
    """
    def postSetup(self):
        from MuonCnvExample.MuonCnvFlags import muonCnvFlags
        if muonCnvFlags.RpcCablingMode=='new':
            from MuonRPC_Cabling.MuonRPC_CablingConfig import MuonRPC_CablingSvc
            MuonRPC_CablingSvc.CosmicConfiguration = True
        else:
            from RPCcabling.RPCcablingConfig import RPCcablingSvc
            RPCcablingSvc.CosmicConfiguration = True

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
        svcMgr.AtlasFieldSvc.UseSoleCurrent = 0

class ToroidsOff(_modifier):
    """
    Turn toroid fields OFF
    """
    def postSetup(self):
        svcMgr.AtlasFieldSvc.UseToroCurrent = 0

class BFieldFromDCS(_modifier):
    """
    Read B-field currents from DCS (also works for MC)
    """
    def postSetup(self):
        from IOVDbSvc.CondDB import conddb
        conddb._SetAcc("DCS_OFL","COOLOFL_DCS")
        conddb.addFolder("DCS_OFL","/EXT/DCS/MAGNETS/SENSORDATA")
        svcMgr.AtlasFieldSvc.UseDCS = True

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
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(svcMgr.ToolSvc,"TileROD_Decoder"):
           from TileByteStream.TileByteStreamConf import TileROD_Decoder
           svcMgr.ToolSvc+=TileROD_Decoder()
        if _run_number<318000:  # use old readout scheme (default is new one)
            log.info('Reverting to pre-2017 Tile ROD map')
            #ToolSvc.TrigDataAccess.fullTileMode=False
            #ToolSvc.TileRegionSelectorTable.FullRODs=False
            svcMgr.ToolSvc.TileROD_Decoder.fullTileMode=0



class useOnlineLumi(_modifier):
    """
    Use online LuminosityTool
    """
    def preSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
        ToolSvc += LuminosityToolOnline()

        # This is needed for the correct functioning of LumiBlockMuTool
        from LumiBlockComps.LumiBlockCompsConf import LumiBlockMuWriter        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += LumiBlockMuWriter("LumiBlockMuWriter",
                                         LuminosityTool = ToolSvc.LuminosityTool)
        
        
###############################################################
# Algorithm modifiers 
###############################################################

class enableCoherentPS(_modifier):
    """
    Enables coherent prescales
    """
    def postSetup(self):
        if TriggerFlags.doHLT():
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()
            topSequence.TrigSteer_HLT.enableCoherentPrescaling=True 


class disableCachingMode(_modifier):
    """
    Disables caching in Steering/Naivigation
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence.TrigSteer_HLT.cachingMode=0

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



class enable7BitL1TTStreaming(_modifier):
    """
    select setup with 7 bit L1 TT streaming
    """
    def preSetup(self):
        TriggerFlags.CosmicSlice.UseSingleBeam7BitL1Menu=True


class enableHotIDMasking(_modifier):
    """
    Masking of hot pixel and SCT modules enabled in L2 spacepoint provider
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"OnlineSpacePointProviderTool"):
            ToolSvc.OnlineSpacePointProviderTool.UsePixelClusterThreshold=True
            ToolSvc.OnlineSpacePointProviderTool.UseSctClusterThreshold=True

class softTRTsettings(_modifier):
    """
    Lower cuts for standalone TRT tracking in cosmic chains
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics"):
            ToolSvc.InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics.MinimalNumberOfTRTHits = 15
        if hasattr(ToolSvc,"InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics_NoField"):
            ToolSvc.InDetL2_TRT_TrackSegmentsMaker_BarrelCosmics_NoField.MinimalNumberOfTRTHits = 15

class disableCaloAllSamples(_modifier):
    """
    Request calorimeter data in 4 requests. Inefficient more needed for large sample (>5) running
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ToolSvc
        if hasattr(ToolSvc,"TrigDataAccess"):
            ToolSvc.TrigDataAccess.loadAllSamplings=False

class emulateMETROBs(_modifier):
    """
    Emulate the MET information as it would have been extracted from the MET ROBs online
    """
    def postSetup(self):
        # See also Savannah bug 87398
        from AthenaCommon.AppMgr import ToolSvc
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()        
        if hasattr(ToolSvc,"TrigDataAccess") and TriggerFlags.doHLT():
            ToolSvc.TrigDataAccess.loadFullCollections = True
            for alg in topSequence.TrigSteer_HLT.getChildren():
                if alg.getType()=='T2CaloMissingET': alg.OneByOne = True                    
        
class ignoreL1Vetos(_modifier):
    """
    Also run algorithms for L1 items that trigger, but were vetoed (disabled or prescaled)
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_L2"):
            topSequence.TrigSteer_L2.LvlConverterTool.ignorePrescales=True
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.LvlConverterTool.ignorePrescales=True
            
class optimizeChainOrder(_modifier):
    """
    Execute chains in order of decreasing RoI size
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.ExecutionOrderStrategy.order=[ 'name:.+beamspot.+',
                                                                    'name:.+j.+',
                                                                    'name:.+2tau.+',
                                                                    'name:.+tau.+'  ]

class disablePixels(_modifier):
    """
    Turns off pixels in region selector
    """
    def postSetup(self):
        svcMgr.RegSelSvc.enablePixel=False

class disableSCTBarrel(_modifier):
    """
    Turns off SCT Barrel in region selector and badly mapped ROB
    """
    def postSetup(self):
        svcMgr.RegSelSvc.DeleteSiRobList=range(0x210000,0x21000a+1)+range(0x210100,0x21010a+1)+range(0x220000,0x22000a+1)+range(0x220100,0x22010a+1)+[0x240005]

class disableIBL(_modifier):
    """
    Turn off IBL from readout
    """

    def postSetup(self):
        import TrigInDetValidation.InDetModules as IDM
        pixel_barrel_layer1_hashes = IDM.getHashes(IDM.getLayer(IDM.getBarrel(IDM.Pixel),0))
        svcMgr.RegSelSvc.DeletePixelHashList=pixel_barrel_layer1_hashes
        svcMgr.ROBDataProviderSvc.ignoreROB=[1310848, 1310849, 1310850, 1310851, 1310899, 1310944, 1310913, 1310946, 1310929, 1310912, 1310914, 1310736, 1310737, 1310738, 1310739, 1310752, 1310753, 1310754, 1310755, 1310883, 1310897, 1310930, 1310896, 1310898, 1310768, 1310769, 1310770, 1310771, 1310784, 1310785, 1310786, 1310787, 1310867, 1310931, 1310881, 1310880, 1310882, 1310800, 1310801, 1310802, 1310803, 1310816, 1310817, 1310818, 1310819, 1310915, 1310865, 1310864, 1310945, 1310928, 1310866, 1310832, 1310833, 1310834, 1310835, 1310947]

class disableIBLInTracking(_modifier):
    """
    Turn off IBL in tracking algorithms (data still available for PEB etc)
    """

    def postSetup(self):
        svcMgr.SpecialPixelMapSvc.MaskLayers = True; svcMgr.SpecialPixelMapSvc.LayersToMask = [0]


class detailedErrorStreams(_modifier):
    """
    Split hlterror streams into four separate streams
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.ResultBuilder.DefaultStreamTagForErrors='HltError'
            topSequence.TrigSteer_HLT.ResultBuilder.ErrorStreamTags = ["ABORT_EVENT ALGO_ERROR TIMEOUT: HltTimeout debug",
                                                                      "ABORT_EVENT NO_HLT_RESULT UNKNOWN: MissingData debug",
                                                                      "ABORT_EVENT NO_HLT_RESULT USERDEF_1: MissingData debug",
                                                                      "ABORT_EVENT WRONG_HLT_RESULT USERDEF_2: MissingData debug",
                                                                      "ABORT_EVENT WRONG_HLT_RESULT USERDEF_4: MissingData debug",
                                                                      ]
    
class ignoreErrorStream(_modifier):
    """
    No events are sent to error stream
    """
    def postSetup(self):
        errReasons = [ "MISSING_FEATURE", "GAUDI_EXCEPTION", "EFORMAT_EXCEPTION", "STD_EXCEPTION",     
                       "UNKNOWN_EXCEPTION", "NAV_ERROR", "MISSING_ROD", "CORRUPTED_ROD", "TIMEOUT", "BAD_JOB_SETUP",      
                       "USERDEF_1", "USERDEF_2", "USERDEF_3", "USERDEF_4" ]
        errList = [ "ABORT_CHAIN " + x +" ALGO_ERROR: "  for  x in errReasons ]
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.ResultBuilder.ErrorStreamTags = errList

class inclusiveErrorStream(_modifier):
    """
    Events with errors are sent to both error and debug stream
    """
    def postSetup(self):
        errReasons = [ "MISSING_FEATURE", "GAUDI_EXCEPTION", "EFORMAT_EXCEPTION", "STD_EXCEPTION",     
                       "UNKNOWN_EXCEPTION", "NAV_ERROR", "MISSING_ROD", "CORRUPTED_ROD", "TIMEOUT", "BAD_JOB_SETUP",      
                       "USERDEF_1", "USERDEF_2", "USERDEF_3", "USERDEF_4" ]
        errList = [ "ABORT_CHAIN " + x +" ALGO_ERROR: hlterror physics"  for  x in errReasons ]
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.ResultBuilder.ErrorStreamTags = errList

class mufastDetMask(_modifier):
    """
    set mufast to use detector mask
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,'TrigSteer_L2'):
            for instance in ['muFast_Muon','muFast_900GeV','muFast_HALO']:
                muFast = topSequence.TrigSteer_L2.allConfigurables.get(instance)
                if muFast:
                    muFast.DetMaskCheck=True
        if hasattr(topSequence,'TrigSteer_HLT'):
            for instance in ['muFast_Muon','muFast_900GeV','muFast_HALO']:
                muFast = topSequence.TrigSteer_HLT.allConfigurables.get(instance)
                if muFast:
                    muFast.DetMaskCheck=True


class muonCommissioningStep1(_modifier):
    """
    Set muon exclusive/inclusive thresholds according to commissioning step 1
        (4 inclusive, 2 exclusive)
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_L2"):
            topSequence.TrigSteer_L2.LvlConverterTool.Lvl1ResultAccessTool.muonCommissioningStep=1
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.LvlConverterTool.Lvl1ResultAccessTool.muonCommissioningStep=1

class muonEFTimeouts(_modifier):
    """
    set MuonEF to throw speculative timeout based on number of MDT hits
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,'TrigSteer_EF'):
            for inst in topSequence.TrigSteer_EF.getAllChildren():
                if inst.getType()=='TrigMuonEFSegmentFinder':
                    inst.doTimeOutGuard=True
                    inst.maxMdtHits=1000
        if hasattr(topSequence,'TrigSteer_HLT'):
            for inst in topSequence.TrigSteer_HLT.getAllChildren():
                if inst.getType()=='TrigMuonEFSegmentFinder':
                    inst.doTimeOutGuard=True
                    inst.maxMdtHits=1000

class doMuonRoIDataAccess(_modifier):
    """
    Use RoI based decoding of muon system
    """
    def preSetup(self):
        TriggerFlags.MuonSlice.doEFRoIDrivenAccess=True
                    
class forceMuonDataPrep(_modifier):
    """
    Execute muon data preparation on every event
    """
    def preSetup(self):
        pass  # the actual modifier is implemented in share/Trigger_topOptions_standalone.py
        
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.LvlConverterTool.useL1Calo=True

class FakeLVL1(_modifier):
    """
    setup fake LVL1 RoIs
    """
    def postSetup(self):
        from TrigFake.TrigFakeConf import FakeLvl1RoIatFixedEtaPhi
        fake = FakeLvl1RoIatFixedEtaPhi()
        fake.FakeMuonRoiPhi=TriggerFlags.CosmicSlice.FakeLVL1ROIPhi()
        fake.FakeMuonRoiEta=TriggerFlags.CosmicSlice.FakeLVL1ROIEta()
        #fake.FakeMuonRoiEta=0.5
        fake.FakeMuonRoiLabel=TriggerFlags.CosmicSlice.FakeLVL1Threshold()
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.LvlConverterTool =  fake
            if (TriggerFlags.CosmicSlice.forceLVL2Accept()):
                HltEventLoopMgr.ForceLvl2Accept =  True
            if (TriggerFlags.CosmicSlice.filterEmptyROB()):
                svcMgr.ROBDataProviderSvc.filterEmptyROB=True


class rerunLVL1(_modifier):
    """
    Reruns the L1 simulation on real data
    """
    def preSetup(self):

        # Do nothing for EF only running
        if TriggerFlags.doLVL2()==False and TriggerFlags.doEF()==True: return
         
        from AthenaCommon.Include import include
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        #write cool objects to detector store
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderWithTag('TRIGGER', "/TRIGGER/LVL1/BunchGroupContent", "HEAD")
        conddb.addFolder('TRIGGER', '/TRIGGER/LVL1/CTPCoreInputMapping')
   
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
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
        topSequence += L1Muctpi_on_RDO()
        topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
        topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "/Run/L1MuCTPItoRoIBLocation"

        # Add L1TopoSimulation if it was not already added, e.g. by L1TopoROBMonitor
        topSequenceAlgNames=[alg.getName() for alg in topSequence.getChildren()]
        if 'L1TopoSimulation' not in topSequenceAlgNames:
            from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
            topSequence += L1TopoSimulation()
            log.info( "adding L1TopoSimulation() to topSequence" )

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
        log.debug( "topSequence: %s" % topSequenceAlgNames )

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
            topSequence.RoIBuilder.CTPSLinkLocation = '/Event/CTPSLinkLocation_Rerun'

        # Get run number from input file if running in athena
        global _run_number
        if _run_number==None:
            import PyUtils.AthFile as athFile
            from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
            af = athFile.fopen(athenaCommonFlags.BSRDOInput()[0])
            _run_number = af.run_number[0]
             
        # On run-1 data, need to re-create the L1 menu with correct L1Calo energy scale (ATR-10174)
        if _run_number!=None and _run_number<222222:
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

         # Do nothing for EF only running
         if TriggerFlags.doLVL2()==False and TriggerFlags.doEF()==True: return
         
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
         from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi_on_RDO
         topSequence += L1Muctpi_on_RDO()
         topSequence.L1Muctpi_on_RDO.CTPOutputLocID = "/Run/L1MuCTPItoCTPLocation"
         topSequence.L1Muctpi_on_RDO.RoIOutputLocID = "/Run/L1MuCTPItoRoIBLocation"

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
    Rewrite LVL1 (use together with rerunLVL1)
    """
    # Example:
    # athena -c "testPhysicsV3=1;rerunLVL1=1;rewriteLVL1=1;doLVL2=False;doEF=False;BSRDOInput='input.data'" TriggerRelease/runHLT_standalone.py

    def postSetup(self):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        from TriggerJobOpts.TriggerFlags import TriggerFlags        
        
        # Process all events
        theApp.EvtMax = -1
        TriggerFlags.writeBS = True

        athenaCommonFlags.BSRDOOutput = 'AppName=Athena, OutputDirectory=./, FileTag=testWrite'
        # Persistent BS construction and intialization
        from ByteStreamCnvSvc import WriteByteStream
        StreamBSFileOutput = WriteByteStream.getStream("EventStorage","StreamBSFileOutput")

        # Bytestream conversion
        StreamBSFileOutput.ItemList += [ "ROIB::RoIBResult#*" ]

        # Merge with original bytestream
        from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMergeOutputSvc
        svcMgr += ByteStreamMergeOutputSvc(ByteStreamOutputSvc='ByteStreamEventStorageOutputSvc',
                                           ByteStreamInputSvc='ByteStreamInputSvc',
                                           overWriteHeader=True)

        StreamBSFileOutput.OutputFile = "ByteStreamMergeOutputSvc"
        svcMgr.ByteStreamCnvSvc.ByteStreamOutputSvcList=['ByteStreamMergeOutputSvc']


class writeBS(_modifier):
    """
    Write bytestream output in athena
    """
    def postSetup(self):
        from AthenaCommon.Include import include
        include("TriggerRelease/BStoBS_post.py")
        
        
class UseParamFromDataForBjet(_modifier):
    """
    Enables the usage of the JetProb calibration derived from data
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_HLT"):
            for inst in topSequence.TrigSteer_HLT.getAllChildren():
                if inst.getType()=='TrigBjetFex':
                    inst.UseParamFromData=True


class UseBeamSpotFlagForBjet(_modifier):
    """
    Enables the beam spot status flag check for b-jet
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_HLT"):
            for inst in topSequence.TrigSteer_HLT.getAllChildren():
                if inst.getType() in ['TrigBjetFex','TrigBtagFex','TrigSecVtxFinder','InDet::TrigVxSecondary','InDet::TrigVxSecondaryCombo','TrigBjetHypo']:
                    log.debug('Setting %s.UseBeamSpotFlag = True' % inst.getName())
                    inst.UseBeamSpotFlag=True
                    
class UseRPCTimeDelayFromDataForMufast(_modifier):
    """
    Enables the RPC timing shift correction in data
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_L2"):
            for inst in topSequence.TrigSteer_L2.getAllChildren():
                if inst.getType()=='muFast':
                    delay=-40
                    if inst.name() == 'muFast_MuonEcut4Empty':
                        delay=-15
                    print 'UseRPCTimeDelayFromDataForMufast: modifying RpcTimeDelay to ',delay,' for ',inst.name()
                    inst.RpcTimeDelay=delay
        if hasattr(topSequence,"TrigSteer_HLT"):
            for inst in topSequence.TrigSteer_HLT.getAllChildren():
                if inst.getType()=='muFast':
                    delay=-40
                    if inst.name() == 'muFast_MuonEcut4Empty':
                        delay=-15
                    print 'UseRPCTimeDelayFromDataForMufast: modifying RpcTimeDelay to ',delay,' for ',inst.name()
                    inst.RpcTimeDelay=delay

class UseLUTFromDataForMufast(_modifier):
    """
    Enables the usage of the LUTs derived from data
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        from AthenaCommon.AppMgr import ToolSvc
        from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import MuonBackExtrapolatorForData
        ToolSvc += MuonBackExtrapolatorForData()
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_L2"):
            for inst in topSequence.TrigSteer_L2.getAllChildren():
                if inst.getType()=='muFast':
                    inst.UseLUTForMC=False
                elif inst.getType()=='MuFastSteering':
                    inst.UseLUTForMC=False
                    print 'UseLUTFromDataForMufast: set backExtrapolator to MuonBackExtrapolatorForData()'
                    inst.BackExtrapolator = MuonBackExtrapolatorForData()

        if hasattr(topSequence,"TrigSteer_HLT"):
            for inst in topSequence.TrigSteer_HLT.getAllChildren():
                if inst.getType()=='muFast':
                    inst.UseLUTForMC=False
                elif inst.getType()=='MuFastSteering':
                    inst.UseLUTForMC=False
                    print 'UseLUTFromDataForMufast: set backExtrapolator to MuonBackExtrapolatorForData()'
                    inst.BackExtrapolator = MuonBackExtrapolatorForData()

class DisableMdtT0Fit(_modifier):
    """
    Disable MDT T0 re-fit and use constants from COOL instead
    """
    def preSetup(self):
        from MuonRecExample.MuonRecFlags import muonRecFlags
        muonRecFlags.doSegmentT0Fit.set_Value_and_Lock(False)

class UseBackExtrapolatorDataForMuIso(_modifier):
   """
   Enables the usage of the BackExtrapolator derived from data
   """
   def postSetup(self):
       from AthenaCommon.AlgSequence import AlgSequence
       from AthenaCommon.AppMgr import ToolSvc
       from TrigMuonBackExtrapolator.TrigMuonBackExtrapolatorConfig import MuonBackExtrapolatorForData
       ToolSvc += MuonBackExtrapolatorForData()
       topSequence = AlgSequence()
       if hasattr(topSequence,"TrigSteer_HLT"):
           for inst in topSequence.TrigSteer_HLT.getAllChildren():
               if inst.getType()=='muIso':
                   print 'UseBackExtrapolatorDataForMuComb: set backExtrapolatorLUT to MuonBackExtrapolatorForData()'
                   inst.BackExtrapolatorLUT = MuonBackExtrapolatorForData()
       if hasattr(topSequence,"TrigSteer_L2"):
           for inst in topSequence.TrigSteer_L2.getAllChildren():
               if inst.getType()=='muIso':
                   print 'UseBackExtrapolatorDataForMuComb: set backExtrapolatorLUT to MuonBackExtrapolatorForData()'
                   inst.BackExtrapolatorLUT = MuonBackExtrapolatorForData()


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
       print "jobproperties.Beam.beamType",jobproperties.Beam.beamType
            
   
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
                print '  No ROB monitoring available.'
            
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
                print '  No ROB monitoring available.'


class L1TopoCheck(_modifier):
    """
    turn on L1Topo checks in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigL1TopoROBMonitor.py")
        except IncludeError:
            print '  No L1Topo ROB monitoring available.'        

class muCTPicheck(_modifier):
    """
    turn on muCTPi checks in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigMuCTPiROBMonitor.py")
        except IncludeError:
            print '  No muCTPi ROB monitoring available.'

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
                print '  No muCTPi ROB monitoring available.'

class enableALFAMon(_modifier):
    """
    turn on ALFA monitoring in the HLT
    """
    def postSetup(self):
        from AthenaCommon.Include import include, IncludeError
        try:
            include("TrigOnlineMonitor/TrigALFAROBMonitor.py")
        except IncludeError:
            print '  No ALFA ROB monitoring available.'






class mufastDebug(_modifier):
    """
    enable additional log output from muFast
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_L2"):
            TrigSteer=topSequence.TrigSteer_L2
        elif hasattr(topSequence,"TrigSteer_HLT"):
            TrigSteer=topSequence.TrigSteer_HLT

        muFast = TrigSteer.allConfigurables.get('muFast_Muon')
        muFast.MUlvl1INFO  = True
        muFast.MUtrackINFO = True
        muFast.MUroadsINFO = True
        muFast.MUdecoINFO  = True
        muFast.MUcontINFO  = True
        muFast.MUfitINFO   = True
        muFast.MUsagINFO   = True
        muFast.MUptINFO    = True        
        muFast.TestString = "muFast_Muon  REGTEST "
        muFast = TrigSteer.allConfigurables.get('muFast_900GeV')
        muFast.MUlvl1INFO  = True
        muFast.MUtrackINFO = True
        muFast.MUroadsINFO = True
        muFast.MUdecoINFO  = True
        muFast.MUcontINFO  = True
        muFast.MUfitINFO   = True
        muFast.MUsagINFO   = True
        muFast.MUptINFO    = True        
        muFast.TestString = "muFast_900GeV  REGTEST "
        
class disableLBHistos(_modifier):
    """
    Disable per lumiblock histograms in steering
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.MonTools["TrigChainMoniOnline"].useLBHistos=False
            topSequence.TrigSteer_HLT.MonTools["TrigSignatureMoniOnline"].useLBHistos=False


class caf(_modifier):
    """
    Detailed monitoring setup for CAF
    """
    def __init__(self):
        _modifier.__init__(self)
        self.modifiers = [detailedTiming()]
        self.modifiers += [memMon()]   # temporary, see https://savannah.cern.ch/task/?21514
        
    def preSetup(self):
        TriggerFlags.enableMonitoring = ['Validation','Time']
        for m in self.modifiers: m.preSetup()

    def postSetup(self):
        for m in self.modifiers: m.postSetup()

        
class libAuditor(_modifier):
    """
    Turn on print out of shared library loading
    """
    def postSetup(self):
        from AthenaServices.AthDsoLogger import DsoLogger
        dso = DsoLogger()


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
class detailedTiming(_modifier):
    """
    Add detailed timing information
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        svcMgr.TrigTimerSvc.IncludeName=".+"
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if hasattr(topSequence,'TrigSteer_L2'):
            for instance in ['muFast_Muon','muFast_900GeV']:
                muFast = topSequence.TrigSteer_L2.allConfigurables.get(instance)
                if muFast:
                    muFast.Timing=True
        if hasattr(topSequence,'TrigSteer_HLT'):
            for instance in ['muFast_Muon','muFast_900GeV']:
                muFast = topSequence.TrigSteer_HLT.allConfigurables.get(instance)
                if muFast:
                    muFast.Timing=True
                    
class perfmon(_modifier):
    """
    Enable PerfMon
    """
    def preSetup(self):
        from PerfMonComps.PerfMonFlags import jobproperties
        jobproperties.PerfMonFlags.doMonitoring = True
        jobproperties.PerfMonFlags.doPersistencyMonitoring = False

class memMon(_modifier):
    """
    Enable TrigMemMonitor printout
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            try:
                topSequence.TrigSteer_HLT.MonTools['TrigMemMonitor'].OutputLevel = VERBOSE
            except:
                log.warning("memMon=True but TrigMemMonitor not present in the HLTMonTools")
                    
class chainOrderedUp(_modifier):
    """
    run chains sorted by ascending chain counter
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.sortChains=1

class chainOrderedDown(_modifier):
    """
    run chains sorted by descending chain counter
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.sortChains=-1

class noCaching(_modifier):
    """
    Disable caching in steering
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        if TriggerFlags.doHLT():
            topSequence.TrigSteer_HLT.cachingMode = 0


class enableFPE(_modifier):
    """
    Turn on floating point exceptions
    """
    def postSetup(self):
        theApp.CreateSvc += ["FPEControlSvc"]


class PeriodicScaler(_modifier):
    """
    Use periodic scaler to get fully reproducible results
    """
    def postSetup(self):
        svcMgr.ScalerSvc.DefaultType = "HLT::PeriodicScaler"

class PeriodicScalerTake1st(_modifier):
    """
    Use periodic scaler to get fully reproducible results but run all chains on 1st event
    """
    def postSetup(self):
        svcMgr.ScalerSvc.DefaultType = "HLT::PeriodicScalerTake1st"


class doValidation(_modifier):
    """
    Force validation mode (i.e. no message timestamps)
    """
    def __init__(self):
        _modifier.__init__(self)
        self.modifiers = [PeriodicScalerTake1st(),chainOrderedUp()]
        
    def preSetup(self):
        TriggerFlags.Online.doValidation = True
        # Replace Online with Validation monitoring
        TriggerFlags.enableMonitoring = filter(lambda x:x!='Online', TriggerFlags.enableMonitoring())+['Validation']
        for m in self.modifiers: m.preSetup()
        
    def postSetup(self):
        for m in self.modifiers: m.postSetup()
        
class TriggerRateTool(_modifier):
    """
    Make trigger rate tuple - only for running in athena
    """
    def postSetup(self):
        from AthenaCommon.Include import include
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from TrigDecisionMaker.TrigDecisionMakerConfig import WriteTrigDecision
        trigDecWriter = WriteTrigDecision()
        from TriggerRateTools.TriggerRateToolsConf import TriggerRateTools
        triggerRateTools = TriggerRateTools()
        triggerRateTools.doTextOutput         = False
        triggerRateTools.doVeto               = False
        triggerRateTools.doRawTD              = True
        triggerRateTools.OutputLevel          = 3
        triggerRateTools.IgnoreList           = ["L2_always","EF_always"]
        triggerRateTools.CplxAndList          = []
        triggerRateTools.CplxOrList           = []
        triggerRateTools.PrescaleOverrideList = []
        triggerRateTools.MenusList            = []
        triggerRateTools.StreamsList          = []
        topSequence += triggerRateTools
        svcMgr.THistSvc.Output += ["TriggerRateTools DATAFILE='TriggerRates.root' OPT='RECREATE'"]
        triggerRateTools.xSection = 0.070
        triggerRateTools.Luminosity = 10000000.0
        if hasattr(topSequence,"TrigSteer_L2"):
            topSequence.TrigSteer_L2.ignorePrescales=True
            topSequence.TrigSteer_L2.LvlConverterTool.Lvl1ResultAccessTool.ignorePrescales=True
        if hasattr(topSequence,"TrigSteer_EF"):
            topSequence.TrigSteer_EF.ignorePrescales=True
        if hasattr(topSequence,"TrigSteer_HLT"):
            topSequence.TrigSteer_HLT.ignorePrescales=True
            topSequence.TrigSteer_HLT.LvlConverterTool.Lvl1ResultAccessTool.ignorePrescales=True


class doMufastNtuple(_modifier):
    """
    fill mufast ntuple - does not work in online mode
    """
    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        topSequence = AlgSequence()
        for algo in ['muFast_900GeV','muFast_HALO','muFast_Muon']:
            if hasattr(topSequence.TrigSteer_L2,algo):
                getattr(topSequence.TrigSteer_L2,algo).MONntuple=True
            if hasattr(topSequence.TrigSteer_HLT,algo):
                getattr(topSequence.TrigSteer_HLT,algo).MONntuple=True
        theApp.HistogramPersistency = "ROOT"
        # output the ntuple file
        svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='mufast-ntuple.root' OPT='NEW'" ]

class autoConditionsTag(_modifier):
    """
    UGLY hack to setup conditions based on stream tag
    require RecExConfig.RecFlags.triggerStream to be set elsewhere
    Please do not use this - for trigger reprocessing only!!!
    """
    def preSetup(self):
        from RecExConfig.AutoConfiguration import ConfigureConditionsTag
        ConfigureConditionsTag()


class disableOnlineMode(_modifier):
    """
    Athena-only modifier for running with isOnline=False
    Needed only for accessing offline only conditions
    """
    def preSetup(self):
        from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
        athenaCommonFlags.isOnline = False


class enableCostD3PD(_modifier):
    """
    Enables creation of Cost D3PD during trigger execution
    Should be used in conjunction with enableCostMonitoring
    D3PDMaker packages must be in the release or compiled manually
    """
    def postSetup(self):
        import imp
        try:
            imp.find_module('TrigCostD3PDMaker')
            from AthenaCommon.Include import include
            include("TrigCostD3PDMaker/TrigCostD3PDMaker_prodJobOFragment.py")
        except:
            print 'TrigCostD3PDMaker packages not available, will not produce CostMonitoring D3PD.'
            #import sys, traceback # <-- For debug
            #traceback.print_exc(file=sys.stdout) # <-- For debug

class enableRateD3PD(_modifier):
    """
    Enables creation of Rate D3PD during trigger execution
    Should be used in conjunction with enableCostMonitoring
    D3PDMaker packages must be in the release or compiled manually
    """
    def postSetup(self):
        import imp
        try:
            imp.find_module('TrigCostD3PDMaker')
            from AthenaCommon.Include import include
            include("TrigCostD3PDMaker/TrigRateD3PDMaker_prodJobOFragment.py")
        except:
            print 'TrigCostD3PDMaker packages not available, will not produce RateMonitoring D3PD.' 

class enableCostDebug(_modifier):
    """
    Enables cost debugging options
    """
    def postSetup(self):
        from TrigCostMonitor.TrigCostMonitorConfig import setupCostDebug
        setupCostDebug()

class enableCostMonitoring(_modifier):
    """
    Enable Cost Monitoring for online
    """
    def preSetup(self):
        TriggerFlags.enableMonitoring = TriggerFlags.enableMonitoring.get_Value()+['CostExecHLT']

    def postSetup(self):
        from TrigCostMonitor.TrigCostMonitorConfig import postSetupOnlineCost
        postSetupOnlineCost()

class enableCostForCAF(_modifier):
    """
    Enable Cost Monitoring for CAF processing - use together with enableCostMonitoring
    """
    def preSetup(self):
        try:
            import TrigCostMonitor.TrigCostMonitorConfig as costConfig
            costConfig.preSetupCostForCAF()
        except AttributeError, msg:
            print '    TrigCostMonitor has not CAF preSetup option... OK to continue'
            
    def postSetup(self):
        try:
            import TrigCostMonitor.TrigCostMonitorConfig as costConfig
            costConfig.postSetupCostForCAF()
        except AttributeError, msg:
            print '    TrigCostMonitor has not CAF postSetup option... OK to continue'

class doEnhancedBiasWeights(_modifier):
    """
    Enable calculaton of EnhancedBias weights, either on or offline - use together with enableCostMonitoring and enableCostForCAF (if offline).
    """
    def postSetup(self):
        try:
            import TrigCostMonitor.TrigCostMonitorConfig as costConfig
            costConfig.postSetupEBWeighting()
        except AttributeError, msg:
            print 'TrigCostMonitor has no EnhancedBias postSetup option...'
        # Try to put this in D3PD (will only work offline), still goes in the BS anyway
        import imp
        try:
            imp.find_module('TrigCostD3PDMaker')
            from AthenaCommon.Include import include
            include("TrigCostD3PDMaker/TrigEBWeightD3PDMaker_prodJobOFragment.py")
        except:
            print 'TrigCostD3PDMaker packages not available, will not produce Enhanced Bias weighting D3PD.' 
        
class ForceMCSCTMapping(_modifier):
    """
    Hardcoded SCT cable map to use the full MC ones
    MC cabling map is not the default setting anymore
    """
    def postSetup(self):
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr,"SCT_CablingSvc"):
            from SCT_Cabling.SCT_CablingConf import SCT_CablingSvc
            svcMgr+=SCT_CablingSvc()
        svcMgr.SCT_CablingSvc.DataSource = "SCT_MC_FullCabling_svc.dat"


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

class LumiRegionZmax168(_modifier):
    """
    decrease the size (equivalent of 3*sigma_z) of luminous region for ID tracking to 168 mm
    """
    def preSetup(self):
        from InDetTrigRecExample.ConfiguredNewTrackingTrigCuts import L2IDTrackingCuts
        from AthenaCommon.SystemOfUnits import mm 
        L2IDTrackingCuts.setRegSelZmax(168* mm)

    def postSetup(self):
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        RegSelSvc=topSequence.allConfigurables.get("RegSelSvcDefault")
        from AthenaCommon.SystemOfUnits import mm 
        RegSelSvc.DeltaZ = 168* mm

class useDynamicAlignFolders(_modifier):
    """
    enable the new (2016-) alignment scheme
    """
    def preSetup(self):
        from AtlasGeoModel.InDetGMJobProperties import GeometryFlags;
        GeometryFlags.useDynamicAlignFolders.set_Value_and_Lock(True)

    
###############################################################
# Modifiers believed to be obsolete.
###############################################################

