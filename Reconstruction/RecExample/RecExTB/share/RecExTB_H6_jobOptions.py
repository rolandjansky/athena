#======================================================================
# TOP Job options file for H6 TB2004 reconstruction 
# of real or simulated data
# To write - CBNTAA
#          - or/and special TBtree 
#======================================================================

# -------------------------------
# Set global flags
# -------------------------------
# Input
if not 'FullFileName' in dir():
   FullFileName = [ "daq_beamDAQ_H6_0004300_file01.data" ]
if not 'RunNumber' in dir():
   RunNumber = 4300
if not 'EvtMax' in dir():
   EvtMax = -1
if not 'EvtSkip' in dir():
   EvtSkip = 0
if not 'doSim' in dir():
   doSim=False
if not 'OutputLevel' in dir():
   OutputLevel=ERROR
# Output
if not 'OutputDir' in dir():
   OutputDir="."
if not 'doCBNT' in dir():
   doCBNT=True
if doCBNT and not 'doCBNTLArDigits' in dir():
   doCBNTLArDigits=False
if not 'doWriteESD' in dir():
   doWriteESD=True
if not 'doTBTree' in dir():
   doTBTree=True
if not 'doTBNoiseWrite' in dir():
   doTBNoiseWrite=False
# Reco
if not 'doBeamDetectors' in dir():
   doBeamDetectors=True
if not 'doBeamQuality' in dir():
   doBeamQuality=False
if not 'doMakeElecCluster' in dir():
   doMakeElecCluster=True
if not 'doMakeTBCluster' in dir():
   doMakeTBCluster=True
if doMakeTBCluster and not 'ConeCuts' in dir():
   ConeCuts=0.25
if not 'doMakeTopoCluster' in dir():
   doMakeTopoCluster=True
if not 'doMakeEMTopoCluster' in dir():
   # broken for TB RawChannelBuilder
   doMakeEMTopoCluster=False
if not 'doTopoClusterLocalCalib' in dir():
   if doMakeTopoCluster or doMakeEMTopoCluster:
      doTopoClusterLocalCalib=True
   else:
      doTopoClusterLocalCalib=False   
if doMakeTopoCluster or doMakeEMTopoCluster:
   if not 'TopoSeed' in dir():
      TopoSeed = 4.0
   if not 'TopoNeighbour' in dir():
      TopoNeighbour = 2.0   
   if not 'TopoCell' in dir():
      TopoCell = 0.0   
if not 'doDMSplit' in dir():
   doDMSplit = True
if not 'doEMECXTalk' in dir():
   doEMECXTalk = True   
if doEMECXTalk and not 'EMECXTalkFactorLong' in dir(): 
   EMECXTalkFactorLong = 0.01
if doEMECXTalk and not 'EMECXTalkFactorEta' in dir():
   EMECXTalkFactorEta = 0.005
if doEMECXTalk and not 'EMECXTalkFactorEMEC2Eta' in dir():
   EMECXTalkFactorEMEC2Eta = 0.025

# Noise
if not 'doRndm' in dir():
   doRndm = False

if not 'doSimpleNoiseTool' in dir():
   doSimpleNoiseTool = False

if doSimpleNoiseTool:
   if not 'SimpleNoiseFile' in dir():
      SimpleNoiseFile="rdmtrig_noise"

if not 'doCellNoise' in dir():
   doCellNoise = True

if doCellNoise:
   if not 'NoiseFile' in dir():
      NoiseFile = ["dcap:///pnfs/rzg.mpg.de/mpp/atlas/user/strizene/H6TB2004/randoms/randoms_all.root",
                   "dcap:///pnfs/rzg.mpg.de/mpp/atlas/user/strizene/H6TB2004/randoms/randoms_all_1.root",
                   "dcap:///pnfs/rzg.mpg.de/mpp/atlas/user/strizene/H6TB2004/randoms/randoms_all_2.root"]
   if not 'NoiseSeed' in dir():
      NoiseSeed = "5678 1234"

# Masking
if not 'doMask' in dir():
   doMask=True

# -- For experts -------
doUseRampBuilder=False
doOscillationCorrection=True
doAddCellDetails=True
doMakeCellDescriptions=False
doMakeH6CellDescriptions=True 
doCBNTCaloClusterUseLink=True
if not doRndm:
   doTBNoiseWrite=False

if doBeamQuality:
   #----------------------------------------------------------
   # choices: "e+", "e-", "pi+", "pi-", "mu+", "mu-", "p"
   particleBeamQuality = ["e-","e+"]

# --------------------------------------
# Probably not needed to change the rest   
# --------------------------------------

doLAr=True
doMon=False
readESD=False
doOFC=True
doOFCPhysShape=False
doOFCCorr=False
if doSim:
    doByteStream=False
    doPool=True
    doBeamDetectors=False
    doCBNTLArDigits=False
else:
    doByteStream=True
    doPool=False
    doDMSplit=False

# ------------------------------------
# Compose output file names
if not 'HistOutputFileName' in dir():
  HistOutputFileName='%(dir)s/Histos_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
if not 'NtupleOutputFileName' in dir():
  NtupleOutputFileName='%(dir)s/nt%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
if not 'ESDOutputFileName' in dir():
   ESDOutputFileName='%(dir)s/ESD_%(No)d.root' % {"dir" : OutputDir, "No" : RunNumber}
# ------------------------------------

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock("ctbh6")
globalflags.Luminosity.set_Value_and_Lock("zero")

if doSim:
    globalflags.DataSource.set_Value_and_Lock("geant4")
else:
    globalflags.DataSource.set_Value_and_Lock("data")
    
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.pileup.all_setOff()
DetFlags.makeRIO.all_setOff()
DetFlags.readRDOPool.all_setOff()
DetFlags.readRDOBS.all_setOff()

DetFlags.detdescr.LAr_setOn()
DetFlags.makeRIO.LAr_setOn()

if doSim:
    DetFlags.readRDOPool.LAr_setOn()
else:
    DetFlags.readRDOBS.LAr_setOn()

DetFlags.Print()

#------------------------
# Read from POOL
#------------------------
if doPool:
    globalflags.InputFormat.set_Value_and_Lock("pool")
    include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )
    if doSim:
        include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
    include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
    include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )
    from AthenaCommon.AppMgr import ServiceMgr
    if not hasattr(ServiceMgr, 'EventSelector'):
      import AthenaPoolCnvSvc.ReadAthenaPool
    if not readESD:
        ServiceMgr.EventSelector.InputCollections = FullFileName 
        ServiceMgr.EventSelector.RunNumber = RunNumber
    else:
        ServiceMgr.EventSelector.InputCollections = [ "reconstruction.root" ]

#-----------------------
# ByteStream Input 
#-----------------------    
if doByteStream:
    globalflags.InputFormat.set_Value_and_Lock("bytestream")

    include( "TBCnv/TBReadH6BS_EventStorage_jobOptions.py" )

    theByteStreamInputSvc=svcMgr.ByteStreamInputSvc
    
    theByteStreamInputSvc.FullFileName =  FullFileName 
    
    ToolSvc = Service( "ToolSvc" )
    ToolSvc.TBByteStreamCnvTool.Dump        = False
    ToolSvc.TBByteStreamCnvTool.OutputLevel = WARNING
    
#-----------------------
#  GeoModel + DetDescr
#-----------------------    
DetDescrVersion='ATLAS-H6-2004-00'
globalflags.DetDescrVersion = 'ATLAS-H6-2004-00'
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-H6-2004-00"
#need also sim Flags unfortunatelly
from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout='tb_LArH6_2004'
SimFlags.load_tbLArH6_flags()
SimFlags.LArTB_H6Hec.set_Value(True)
SimFlags.LArTB_H6Hec.set_On()
SimFlags.LArTB_H6Emec.set_Value(True)
SimFlags.LArTB_H6Emec.set_On()
SimFlags.LArTB_H6Fcal.set_Value(True)
SimFlags.LArTB_H6Fcal.set_On()
SimFlags.LArTB_H6Coldnose.set_Value(True)
SimFlags.LArTB_H6Coldnose.set_On()
SimFlags.LArTB_H6Run1.set_Value(False)
SimFlags.LArTB_H6Run1.set_On()

#  and additional switches:
from RecExConfig.RecFlags import rec
rec.doTile=False
rec.doLArg=False
from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArNoiseMasking = True
jobproperties.CaloCellFlags.doLArHVCorr = False
jobproperties.CaloCellFlags.doLArSporadicMasking = False
jobproperties.CaloCellFlags.doLArCellEmMisCalib = False
jobproperties.CaloCellFlags.doPedestalCorr = False
jobproperties.CaloCellFlags.doLArRecalibration = False
jobproperties.CaloCellFlags.doLArBadFebMasking = False
jobproperties.CaloCellFlags.doLArDeadOTXCorr = False

if doSim:
   from RecExConfig.ObjKeyStore import objKeyStore
   objKeyStore.readInputFile('RecExPers/OKS_streamRDO.py')

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = DetDescrVersion
from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

#-----------------------
#  Conditions
#-----------------------    
from IOVDbSvc.CondDB import conddb
# Bloct the trigger tower mapping, doesn't exist for TB
include.block ( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py" )
include.block ( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py" )

if doLAr:
    useLArH6CondCool = True
    include ("LArDetDescr/LArDetDescr_H6_joboptions.py")
    if doSim:
       include("LArConditionsCommon/LArConditionsCommon_H6G4_jobOptions.py")
    else:
       include("LArConditionsCommon/LArConditionsCommon_H6_jobOptions.py")
    include( "LArConditionsCommon/LArIdMap_H6_jobOptions.py" )
#    include( "CaloConditions/CaloConditionsDict_joboptions.py")
    include( "CaloCondAthenaPool/CaloCondAthenaPool_joboptions.py")

    PoolSvc.ReadCatalog += [ "prfile:PoolCat_H6_tbcond.xml" ]
    print "FileCatalog: ", PoolSvc.ReadCatalog
    
DetectorStore.OutputLevel = ERROR

# output stream  
from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["FILE1 DATAFILE='"+NtupleOutputFileName+"' OPT='RECREATE'" ]

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#
# direct call of pool files (folders are blocked later, after all standard includes)
#
ProxyProviderSvc = Service( "ProxyProviderSvc" )
CondProxyProvider = Service( "CondProxyProvider" )
ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]

CondProxyProvider.InputCollections+=["/afs/ipp-garching.mpg.de/home/g/gdp/public/CaloLocalHadCoeff/vv05_29102009/cl_w_ooc_oocpi0_mc09_valid.107410-2.singlepart_singlepiplusminuszero_logE.recon.ESD.e342_s587_s582_r796.pool.root"]
CondProxyProvider.InputCollections+=["/afs/ipp-garching.mpg.de/home/g/gdp/public/CaloLocalHadCoeff/vv05_29102009/CaloLocalHadCoeff.dmcorr.MC09V.QGSPBERT.vv05.pool.root"]
os.system('pool_insertFileToCatalog /afs/ipp-garching.mpg.de/home/g/gdp/public/CaloLocalHadCoeff/vv05_29102009/cl_w_ooc_oocpi0_mc09_valid.107410-2.singlepart_singlepiplusminuszero_logE.recon.ESD.e342_s587_s582_r796.pool.root')
os.system('pool_insertFileToCatalog /afs/ipp-garching.mpg.de/home/g/gdp/public/CaloLocalHadCoeff/vv05_29102009/CaloLocalHadCoeff.dmcorr.MC09V.QGSPBERT.vv05.pool.root')



# -----------------------------
# Reconstruction
# -----------------------------
# non-ATLAS detectors
# -----------------------------
if doLAr:
    theApp.Dlls += [ "TBRec" ]
    from AthenaCommon.AppMgr import athMasterSeq
    if not doSim:
        #
        from TBRec.TBRecConf import TBEventStreamer
        EventInfoStreamer = TBEventStreamer("EventInfoStreamer")
        from TBRec.TBRecConf import TBPreScalerStreamerTool
        PreScaler = TBPreScalerStreamerTool("PreScaler")
        PreScaler.AcceptEventFraction = 1.0
        ToolSvc += PreScaler
        EventInfoStreamer.ToolNames +=  [ PreScaler ]
        #
        from TBRec.TBRecConf import TBEventInfoStreamerTool
        EventTypeSelector = TBEventInfoStreamerTool("EventTypeSelector")
        if not doRndm:
           EventTypeSelector.AcceptedTypes = ["Physics"] 
        else:   
           EventTypeSelector.AcceptedTypes = ["Random"] 
#        EventTypeSelector.AcceptedTypes += ["Special"]
#        EventTypeSelector.AcceptedTypes += ["f/e_Calibration"]
#        EventTypeSelector.AcceptedTypes += ["BPC_Calibration"]
#        ToolNames += [ "TBEventInfoStreamerTool/EventTypeSelector" ]
        ToolSvc += EventTypeSelector
        EventInfoStreamer.ToolNames += [ EventTypeSelector ]
        EventInfoStreamer.OutputLevel = ERROR
        athMasterSeq += EventInfoStreamer
        #
        from TBRec.TBRecConf import TBXCryYTableRead
        TBXCryYTableRead = TBXCryYTableRead()
        TBXCryYTableRead.FileName = "xcryo_ytable.txt" 
        athMasterSeq += TBXCryYTableRead
        #
        from TBRec.TBRecConf import TBPhaseRec
        TBPhaseRec = TBPhaseRec()
        include( "TBRec/H6PhaseRec_jobOptions.py" )
        athMasterSeq += TBPhaseRec
        #
        PhaseStreamer = TBEventStreamer("PhaseStreamer")
        from TBRec.TBRecConf import TBPhaseStreamerTool
        GuardCut = TBPhaseStreamerTool("GuardCut")
        GuardCut.TBPhaseKey = "TBPhase"
        GuardCut.Guard = 11.0
        ToolSvc += GuardCut
        PhaseStreamer.ToolNames += [ GuardCut ]
        athMasterSeq  += PhaseStreamer
        #

if doBeamDetectors:
    if not doSim:
        BeamStreamer = TBEventStreamer("BeamStreamer")
        from TBRec.TBRecConf import  TBPatternUnitStreamerTool
        BitSelector = TBPatternUnitStreamerTool("BitSelector")
        if not doRndm:
           BitSelector.AcceptPattern = [ "S1", "S2", "S3" ]
           BitSelector.RejectPattern = [
#                                           "VetoLeft",
#                                           "VetoRight",
#                                           "VetoUp",
#                                           "VetoDown",
#                                           "HoleVeto",
                                           "EarlyPU",
                                           "LatePU"
                                           ]
        else:                                   
           BitSelector.AcceptPattern = [ ]
           BitSelector.RejectPattern = [ ]

        ToolSvc += BitSelector 
        BeamStreamer.ToolNames +=  [ BitSelector ]
        athMasterSeq += BeamStreamer
        #
        from TBRec.TBRecConf import TBBPCRec
        TBBPCRec =  TBBPCRec()
        include( "TBRec/H6BPCRec_jobOptions.py" )
        athMasterSeq += TBBPCRec
        #   
        from TBRec.TBRecConf import TBScintillatorRec
        TBScintRec =  TBScintillatorRec("TBScintRec")
        include( "TBRec/H6ScintRec_jobOptions.py" )
        athMasterSeq += TBScintRec
        #
        from TBRec.TBRecConf import TBTailCatcherRec
        TBTailCatcherRec = TBTailCatcherRec()
        include( "TBRec/H6TailCatcherRec_jobOptions.py" )
        athMasterSeq += TBTailCatcherRec
        #
        from TBRec.TBRecConf import TBMWPCRec
        TBMWPCRec = TBMWPCRec()
        include( "TBRec/H6MWPCRec_jobOptions.py" )
        athMasterSeq += TBMWPCRec
        #
        from TBRec.TBRecConf import TBPlaneTrackingAlgo
        TBPlaneTrackingAlgo = TBPlaneTrackingAlgo()
        include( "TBRec/H6HitRec_jobOptions.py" )
        if RunNumber < 1249 :
            TBPlaneTrackingAlgo.CalibFileName = "BPCAlignment_2004TB_RunI.txt"
        else:
            TBPlaneTrackingAlgo.CalibFileName = "BPCAlignment_2004TB_RunII.txt"
        athMasterSeq += TBPlaneTrackingAlgo    

if doBeamQuality:
#    if doMon:
        # BQ monitoring before cuts
#        theApp.Dlls += [ "TBMonitoring" ]
        
#        theApp.TopAlg += ["AthenaMon/TBMonBQbefore"]
#        TBMonBQbefore = Algorithm("TBMonBQbefore")
#        TBMonBQbefore.AthenaMonTools += ["TBBeamQualityDoubleHitsMonTool/DoubleHitsMonbefore"]
#        ToolSvc.DoubleHitsMonbefore.ScintSelect=["B","S1","S2","S3"]
#        ToolSvc.DoubleHitsMonbefore.FillTime="before"

  if not doSim:
    from TBRec.TBRecConf import TBBeamQuality
    TBBeamQuality = TBBeamQuality() 
    TBBeamQuality.BQParticles = particleBeamQuality
    from TBRec.TBRecConf import TBBeamQualityDoubleHitsTool
    DoubleHits = TBBeamQualityDoubleHitsTool("DoubleHits") 
    DoubleHits.Scintnames = ["S1","S2","S3",
                                           "Veto","Halo","B",
                                           "muon1","muon2","muon3","muon4",
                                           "muon5","muon6","muon7","muon8"]
    ToolSvc += DoubleHits
    TBBeamQuality.BQToolNames += [ DoubleHits ]
    from TBRec.TBRecConf import TBBeamQualityMuonToolH6
    MuonCut = TBBeamQualityMuonToolH6("MuonCut")
    ToolSvc += MuonCut
    TBBeamQuality.BQToolNames +=  [ MuonCut ]

  else:  
    from TBRec.TBRecConf import TBBeamQualityMC
    TBBeamQuality = TBBeamQualityMC()
    TBBeamQuality.ReadFileforXcryo = False
    TBBeamQuality.CheckTrackParams = False
    TBBeamQuality.TrackCutX = 35.
    TBBeamQuality.TrackCutY = 35.
    TBBeamQuality.CheckPrimaryTrack = True
    TBBeamQuality.ScintForPrimaryTrack = [6,7]
    TBBeamQuality.CheckVetoScint = True
    TBBeamQuality.CheckClusters = False
    TBBeamQuality.CheckTrackReco = True
    
  #topSequence += TBBeamQuality
  from AthenaCommon.AppMgr import athMasterSeq
  athMasterSeq  += TBBeamQuality

# -----------------------------
# LAr detectors
# -----------------------------
if doLAr :

    # Default CaloNoiseTool, to be used by all:
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    from CaloTools.CaloNoiseFlags import jobproperties
    jobproperties.CaloNoiseFlags.UseCaloLuminosity.set_Value_and_Lock(False)
    theCaloNoiseTool = CaloNoiseToolDefault()
    theCaloNoiseTool.NMinBias                = 0
    theCaloNoiseTool.WithOF                  = True
    theCaloNoiseTool.UseTile                 = False
    theCaloNoiseTool.IsMC                    = False
    theCaloNoiseTool.UseSymmetry             = False
    theCaloNoiseTool.WorkMode                = 1
    if doOFC:
             theCaloNoiseTool.WithOF=True
    if doSim:
             theCaloNoiseTool.IsMC=True    

    ToolSvc += theCaloNoiseTool

    # get public tool LArADC2MeVTool
    from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
    theADC2MeVTool = LArADC2MeVToolDefault()
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theADC2MeVTool
    # get public tool LArOFCTool
    from LArRecUtils.LArOFCToolDefault import LArOFCToolDefault
    theOFCTool = LArOFCToolDefault()
    ToolSvc += theOFCTool

    ToolSvc.LArADC2MeVToolDefault.UseHVScaleCorr = False

    if not doSim :
        # read ByteStream and run RawChannelBuilder
        theApp.Dlls += ["LArRawUtils", "LArROD", "LArTools" ]
        #
        from LArROD.LArRODConf import LArDigitPreProcessor_TBLArDigitContainer_
        TBLArDigProcessor = LArDigitPreProcessor_TBLArDigitContainer_("TBLArDigProcessor")
        TBLArDigProcessor.InputContainers = ["MEDIUM","FREE"]
        TBLArDigProcessor.NumberOfSamples = 32
        TBLArDigProcessor.FirstSample     = 0
        if doOscillationCorrection:
            from LArCalibUtils.LArCalibUtilsConf import LArDigitOscillationCorrTool
            theOscCorr = LArDigitOscillationCorrTool()
            theOscCorr.SignalCutInSigma=3.0
            ToolSvc += theOscCorr
            TBLArDigProcessor.CorrectOscillations=True
        topSequence += TBLArDigProcessor
        #
        if not doOFC:
            # The modified RawChannelBuilder:
            from TBRec.TBRecConf import TBECLArRawChannelBuilder
            TBLArRawChannelBuilder = TBECLArRawChannelBuilder("TBLArRawChannelBuilder")
            if RunNumber < 1249 :
                TBLArRawChannelBuilder.maxSamp     = 8
            else:
                TBLArRawChannelBuilder.maxSamp     = 3
            TBLArRawChannelBuilder.RecoMode    = "CUBIC"
            TBLArRawChannelBuilder.CubicAdcCut = 50.
            TBLArRawChannelBuilder.UsePedestalDB=True
            TBLArRawChannelBuilder.UseRampDB=True
            topSequence += TBLArRawChannelBuilder
        else:
            # The modified RawChannelBuilder:
            from TBRec.TBRecConf import TBECLArRawChannelBuilder
            LArRawChannelBuilder = TBECLArRawChannelBuilder("LArRawChannelBuilder")
            LArRawChannelBuilder.UseTDC = True
            LArRawChannelBuilder.ADC2MeVTool = ToolSvc.LArADC2MeVToolDefault
            LArRawChannelBuilder.UseOFCTool             = False
            LArRawChannelBuilder.NOFCTimeBins = 25
            LArRawChannelBuilder.OFCTimeBin    = 1.0*ns
            LArRawChannelBuilder.BinHalfOffset = False
            if not doOFCPhysShape:
                LArRawChannelBuilder.AllowTimeSampleJump = True
                LArRawChannelBuilder.UseOFCPhase         = True
                LArRawChannelBuilder.PhaseInversion      = True
            LArRawChannelBuilder.PedestalFallbackMode = 1 # Use PedestalSample if pedestal not in DB
            if RunNumber < 1249 :
                LArRawChannelBuilder.InitialTimeSampleShift = 6
                LArRawChannelBuilder.PedestalSample = 4
            else: 
                LArRawChannelBuilder.InitialTimeSampleShift = 1
                LArRawChannelBuilder.PedestalSample = 0
            topSequence += LArRawChannelBuilder    
            
    # register LArRawChannel 
    from RecExConfig.ObjKeyStore import objKeyStore
    objKeyStore.addTransient("LArRawChannelContainer",['LArRawChannels'])    
    # make cells
    theApp.Dlls += [ "CaloRec","CaloTools","CaloDetDescr" ]
    from CaloRec.CaloRecConf import CaloCellMaker
    CaloCellMaker = CaloCellMaker()
#!!!!    include ("CaloRec/CaloCellMakerFromRaw_H6_jobOptions.py" )
    # This block is here, because LArCellFromLArRawTool_H6_jobOptions.py is broken
    #-------------------------------------------------------------------------------
    CaloCellMaker.CaloCellsOutputName = "AllCalo"
    # LAr part
    from CaloIdentifier import SUBCALO
    theApp.Dlls += [ "LArTools","LArCalibUtils" ]
    from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
    #cellbuilderEM = LArCellBuilderFromLArRawChannelTool("cellbuilderEM")
    #cellbuilderHEC = LArCellBuilderFromLArRawChannelTool("cellbuilderHEC")
    #cellbuilderFCal = LArCellBuilderFromLArRawChannelTool("cellbuilderFCal")
    cellbuilder = LArCellBuilderFromLArRawChannelTool("cellbuilder")
    # CellBuilder properties
    #cellbuilderEM.LArRegion      = "LAr_EM"
    #cellbuilderEM.EThreshold     =-100000. * MeV
    #cellbuilderHEC.LArRegion     = "LAr_HEC"
    #cellbuilderHEC.EThreshold    =-100000.  * MeV
    #cellbuilderFCal.LArRegion    = "LAr_FCal"
    #cellbuilderFCal.EThreshold   =-100000.  * MeV
    #ToolSvc += cellbuilderEM
    #ToolSvc += cellbuilderHEC
    #ToolSvc += cellbuilderFCal
    #CaloCellMaker.CaloCellMakerToolNames += [ cellbuilderEM ]
    #CaloCellMaker.CaloCellMakerToolNames += [ cellbuilderHEC ]
    #CaloCellMaker.CaloCellMakerToolNames += [ cellbuilderFCal ]
    ToolSvc += cellbuilder
    CaloCellMaker.CaloCellMakerToolNames += [ cellbuilder ]

    if doSim and ( doCellNoise or doEMECXTalk ):
       # Make also 0 energy cells
       from TBRec.TBRecConf import TBCellContainerFillerTool
       cellFiller = TBCellContainerFillerTool()
       ToolSvc += cellFiller
       CaloCellMaker.CaloCellMakerToolNames += [cellFiller]

    if doSim and doEMECXTalk :
       from TBRec.TBRecConf import TBEMECXTalkToyModel
       theEMECXTalkTool = TBEMECXTalkToyModel("EMECXTalkTool")
       theEMECXTalkTool.XTalkScaleLong      = EMECXTalkFactorLong
       theEMECXTalkTool.XTalkScaleEta       = EMECXTalkFactorEta
       theEMECXTalkTool.XTalkScaleEMEC2Eta  = EMECXTalkFactorEMEC2Eta
       #theEMECXTalkTool.OutputLevel = DEBUG
       ToolSvc += theEMECXTalkTool
       CaloCellMaker.CaloCellMakerToolNames += [ theEMECXTalkTool ]

    #finalize container
    from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool
    CaloCellContainerFinalizerTool = CaloCellContainerFinalizerTool()
    ToolSvc += CaloCellContainerFinalizerTool
    CaloCellMaker.CaloCellMakerToolNames += [CaloCellContainerFinalizerTool]
    #make lots of checks (should not be necessary eventually)
    from CaloRec.CaloRecConf import CaloCellContainerCheckerTool
    CaloCellContainerCheckerTool = CaloCellContainerCheckerTool()
    ToolSvc += CaloCellContainerCheckerTool
    CaloCellMaker.CaloCellMakerToolNames += [CaloCellContainerCheckerTool]
    #--------------------- end of block --------------------------------------------

    from TBRec.TBRecConf import TBEMECCellEtaCorrection
    CellEtaCorrection = TBEMECCellEtaCorrection("CellEtaCorrection")
    ToolSvc += CellEtaCorrection
    if not doSim:
        # Values for data
        ToolSvc.CellEtaCorrection.EMEC_alpha1  = 0.55
        ToolSvc.CellEtaCorrection.EMEC_beta1   = 1.04
        ToolSvc.CellEtaCorrection.EMEC_alpha2  = 0.55
        ToolSvc.CellEtaCorrection.ramp_corr    = 1.045
        ToolSvc.CellEtaCorrection.EMEC_rescale = 1.2515
        ToolSvc.CellEtaCorrection.FCAL0_scale  = 0.9777
    else:    
        # for MC only one region
        ToolSvc.CellEtaCorrection.EMEC_alpha1  = 0.15293
#        ToolSvc.CellEtaCorrection.EMEC_alpha1  = 0.
#        ToolSvc.CellEtaCorrection.EMEC_beta1   = 1.256
        ToolSvc.CellEtaCorrection.EMEC_beta1   = 0.8954
#        ToolSvc.CellEtaCorrection.EMEC_beta1   = 0.
#        ToolSvc.CellEtaCorrection.EMEC_alpha2  = -0.0582
        ToolSvc.CellEtaCorrection.EMEC_alpha2  = 0.
#        ToolSvc.CellEtaCorrection.EMEC_beta2   = 1.154
#        ToolSvc.CellEtaCorrection.EMEC_beta2   = 0.7604
        ToolSvc.CellEtaCorrection.EMEC_beta2   = 0.
        ToolSvc.CellEtaCorrection.ramp_corr    = 1.0
        ToolSvc.CellEtaCorrection.EMEC_rescale = 1.0211
        ToolSvc.CellEtaCorrection.FCAL_rescale = 1.0
        ToolSvc.CellEtaCorrection.FCAL0_scale  = 0.9153 
        ToolSvc.CellEtaCorrection.FCAL1_scale  = 1.06286
        ToolSvc.CellEtaCorrection.HEC0_scale   = 0.98135
        ToolSvc.CellEtaCorrection.HEC1_scale   = 0.98135
        ToolSvc.CellEtaCorrection.HEC2_scale   = 1.00090
        ToolSvc.CellEtaCorrection.LowEta       = 2.5
        ToolSvc.CellEtaCorrection.MiddleEta    = 3.2
        ToolSvc.CellEtaCorrection.HighEta      = 3.2

    from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
    theEtaCorrTool=CaloCellContainerCorrectorTool("EMECEtaCorr") 
    theEtaCorrTool.CellCorrectionToolNames=[ CellEtaCorrection ] 
    ToolSvc += theEtaCorrTool
    CaloCellMaker.CaloCellMakerToolNames += [ theEtaCorrTool ]
    
    if doSim and doCellNoise:
        from TBRec.TBRecConf import TBCellNoiseCorrection
        NoiseCorr = TBCellNoiseCorrection("NoiseCorr")
        NoiseCorr.NoiseFile = NoiseFile
        NoiseCorr.NoiseAdd = True
        if 'NoiseX' in dir():
           NoiseCorr.NoiseX = NoiseX
        if 'NoiseY' in dir():
           NoiseCorr.NoiseY = NoiseY
        if 'NoiseE' in dir():
           NoiseCorr.NoiseE = NoiseE   
        ToolSvc += NoiseCorr
        from AthenaServices.AthenaServicesConf import AtRanluxGenSvc
        ServiceMgr += AtRanluxGenSvc()
        ServiceMgr.AtRanluxGenSvc.Seeds = ["TB_NOISE "+NoiseSeed]

        theNoiseCorrTool=CaloCellContainerCorrectorTool("CellNoise")
        theNoiseCorrTool.CellCorrectionToolNames=[NoiseCorr]
        ToolSvc += theNoiseCorrTool
        CaloCellMaker.CaloCellMakerToolNames += [ theNoiseCorrTool ]
    

    if doMask:
       from LArCellRec.LArCellRecConf import LArCellNoiseMaskingTool
       theLArCellMaskingTool = LArCellNoiseMaskingTool()
       from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
       theLArBadChannelTool=LArBadChanTool()
       ToolSvc+=theLArBadChannelTool
       from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker   
       theLArMasker=LArBadChannelMasker("LArMasker")
       theLArMasker.TheLArBadChanTool = theLArBadChannelTool
       theLArMasker.DoMasking=True
       theLArMasker.ProblemsToMask= ["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"]
       ToolSvc+=theLArMasker
       theLArCellMaskingTool.MaskingTool = theLArMasker
       theLArCellMaskingTool.maskNoise = doMask
       theLArCellMaskingTool.maskSporadic = False
       ToolSvc += theLArCellMaskingTool
       CaloCellMaker.CaloCellMakerToolNames += [theLArCellMaskingTool]
       

    topSequence +=  CaloCellMaker   

    if not doUseRampBuilder:
       ToolSvc.LArADC2MeVToolDefault.UseMphysOverMcal = False

    # Make sure no symmetrization:    
    ToolSvc.LArADC2MeVToolDefault.MCSym = False
    ToolSvc.LArOFCToolDefault.FromDatabase=True

    # simple noise tool from text file
    if doSimpleNoiseTool:
       from CaloTools.CaloToolsConf import SimpleNoiseToolFromTextFile
       SimpleNoiseTool = SimpleNoiseToolFromTextFile("SimpleNoiseTool")
       SimpleNoiseTool.CellNoiseFileName = SimpleNoiseFile
       ToolSvc += SimpleNoiseTool

if doDMSplit:
   from TBRec.TBRecConf import TBDMContainerSplitter
   DMContSplit = TBDMContainerSplitter()
   DMContSplit.InputDMContainer = "LArCalibrationHitDeadMaterial"
   DMContSplit.OutputDM_Calo = "LArCalibrationHitDeadMaterial_Calo"
   DMContSplit.OutputDM_Leak = "LArCalibrationHitDeadMaterial_Leakage"
   athMasterSeq += DMContSplit

# -----------------------------
# TB clusters
# -----------------------------

from CaloRec.CaloRecConf import CaloClusterMaker

if doMakeTBCluster:
    ClusterMaker = CaloClusterMaker("ClusterMaker")
    H6Samplings = [ "EME2", "EME3", "HEC0", "HEC1", "HEC2", "FCal1", "FCal2", "FCal3"]
    ClusterMaker.ClustersOutputName           = "TBClusters"
    from TBRec.TBRecConf import TBClusterMaker
    TBCluster = TBClusterMaker("TBCluster")
    TBCluster.samplingNames      = H6Samplings
    TBCluster.coneCuts           = [ConeCuts, ConeCuts,
                                   ConeCuts, ConeCuts, ConeCuts,
                                   ConeCuts, ConeCuts, 0.00]
    TBCluster.seedCut            = 4.0
    TBCluster.cellCut            = -9999999.
    TBCluster.deltaR             = 0.02
    TBCluster.maxIterations      = 4
    TBCluster.CellEnergyInADC    = False
    TBCluster.fixClusterPosition = False
    TBCluster.etaCluster         = 2.79
    TBCluster.phiCluster         = 2.49
    if not doSimpleNoiseTool:
       TBCluster.noiseToolName      = theCaloNoiseTool.getFullName()
    else:
       TBCluster.noiseToolName      = SimpleNoiseTool.getFullName()
    from CaloRec.CaloRecConf import CaloClusterMomentsMaker
    Moments = CaloClusterMomentsMaker("Moments")
    Moments.MaxAxisAngle         = 30*deg
    Moments.MomentsNames         = [
       "FIRST_PHI"
      ,"FIRST_ETA"
      ,"SECOND_R"
      ,"SECOND_LAMBDA"
      ,"DELTA_PHI"
      ,"DELTA_THETA"
      ,"DELTA_ALPHA"
      ,"CENTER_X"
      ,"CENTER_Y"
      ,"CENTER_Z"
      ,"CENTER_LAMBDA"
      ,"LATERAL"
      ,"LONGITUDINAL"
    ]
    ClusterMaker.ClusterMakerTools            = [ TBCluster.getFullName() ]
    ClusterMaker.ClusterCorrectionTools       = [ Moments.getFullName() ]
    ClusterMaker += TBCluster
    ClusterMaker += Moments
    topSequence += ClusterMaker

if doMakeElecCluster:
    EMClusterMaker = CaloClusterMaker("EMClusterMaker")
    EMClusterMaker.ClustersOutputName           = "EMTBClusters"
    from TBRec.TBRecConf import TBClusterMaker
    EMTBCluster = TBClusterMaker("EMTBCluster")
    EMTBCluster.samplingNames      = H6Samplings
    EMTBCluster.coneCuts           = [0.10,0.10,
                                                     0.00,0.00,0.00,
                                                     0.10,0.00,0.00]
    EMTBCluster.seedCut            = 4.0
    EMTBCluster.cellCut            = -999.
    EMTBCluster.deltaR             = 0.02
    EMTBCluster.maxIterations      = 4
    EMTBCluster.CellEnergyInADC    = False
    EMTBCluster.fixClusterPosition = False
    EMTBCluster.etaCluster         = 99.
    EMTBCluster.phiCluster         = 99.
    EMTBCluster.noiseToolName      = theCaloNoiseTool.getFullName()
    EMClusterMaker += EMTBCluster
    EMClusterMaker.ClusterMakerTools = [ EMTBCluster.getFullName() ]
    topSequence += EMClusterMaker

# -----------------------------
# Topo clusters
# -----------------------------
if doMakeTopoCluster:
    from CaloRec.CaloTopoClusterFlags import jobproperties
    jobproperties.CaloTopoClusterFlags.doCellWeightCalib = False
    jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
    jobproperties.CaloTopoClusterFlags.doCalibHitMoments = True

    include ("CaloRec/CaloTopoCluster_jobOptions.py" )

#    topSequence.CaloTopoCluster.LocalCalib.EMFrac.UseNormalizedEnergyDensity = True
#    topSequence.CaloTopoCluster.LocalCalib.EMFrac.MaxEMFraction = 0.5
    topSequence.CaloTopoCluster.KeepCorrectionToolAndContainerNames += [topSequence.CaloTopoCluster.DMCalib.getFullName(),"CaloOOCTopoCluster"]
    topSequence.CaloTopoCluster.KeepCorrectionToolAndContainerNames += [topSequence.CaloTopoCluster.OOCCalib.getFullName(),"CaloWeightTopoCluster"]

    topSequence.CaloTopoCluster.TopoMaker.CellThresholdOnEorAbsEinSigma     =    TopoCell
    topSequence.CaloTopoCluster.TopoMaker.NeighborThresholdOnEorAbsEinSigma =    TopoNeighbour
    topSequence.CaloTopoCluster.TopoMaker.SeedThresholdOnEorAbsEinSigma  =    TopoSeed  
    # FCAL2 (tail catcher) is excluded
    topSequence.CaloTopoCluster.TopoMaker.SeedSamplingNames = ["PreSamplerB", "EMB1", "EMB2", "EMB3",
                                                               "PreSamplerE", "EME1", "EME2", "EME3",
                                                               "HEC0", "HEC1","HEC2", "HEC3",
                                                               "TileBar0", "TileBar1", "TileBar2",
                                                               "TileExt0", "TileExt1", "TileExt2",
                                                               "TileGap1", "TileGap2", "TileGap3",
                                                               "FCAL0", "FCAL1"]

    topSequence.CaloTopoCluster.TopoCalibMoments.MomentsNames += ["ENG_CALIB_OUT_L"]
    topSequence.CaloTopoCluster.TopoCalibMoments.MatchDmType = 1 # 1=loose, 2=medium (default), 3=tight
    topSequence.CaloTopoCluster.TopoCalibMoments.CalibrationHitContainerNames = ["LArCalibrationHitInactive","LArCalibrationHitActive"]
    if doDMSplit:
      topSequence.CaloTopoCluster.TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial_Calo"]
    else:
      topSequence.CaloTopoCluster.TopoCalibMoments.DMCalibrationHitContainerNames = ["LArCalibrationHitDeadMaterial"]


    if doSimpleNoiseTool:
       topSequence.CaloTopoCluster.TopoMaker.CaloNoiseTool           = SimpleNoiseTool
       topSequence.CaloTopoCluster.LocalCalib.LCWeight.CaloNoiseTool = SimpleNoiseTool
       topSequence.CaloTopoCluster.DMCalib.LCDeadMaterial.CaloNoiseTool      = SimpleNoiseTool

    # remove BadChannel tool from cluster corrections
    tlist=[]
    for x in topSequence.CaloTopoCluster.ClusterCorrectionTools:
      if x.find("BadChan") == -1:
         tlist += [x]
      else:   
         print "Removing: ",x," from CaloTopoCluster.ClusterCorrectionTools" 
    topSequence.CaloTopoCluster.ClusterCorrectionTools = tlist
    # remove CaloCellNeighborsAverageCorr from CaloCellMaker
    clist=[]
    for y in topSequence.CaloCellMaker.CaloCellMakerToolNames:
      if y.getName().find("NeighborsAverageCorr") == -1:
         clist += [y]
      else:   
         print "Removing: ",y," from CaloCellMaker.CaloCellMakerToolNames" 
    topSequence.CaloCellMaker.CaloCellMakerToolNames = clist
    
    # Configuring LArBadChanTool    
    ToolSvc.LArBadChanTool.ReadFromASCII = True 
    ToolSvc.LArBadChanTool.CoolFolder = ""
    ToolSvc.LArBadChanTool.ComplementaryCoolFolder = ""
    ToolSvc.LArBadChanTool.CoolMissingFEBsFolder = ""
    ToolSvc.LArBadChanTool.WriteEmptyFolders = True
    ToolSvc.LArBadChanTool.FCALAfile = "H6BadChannels.txt"
    ToolSvc.LArBadChanTool.HECAfile = "H6BadChannels.txt"
    ToolSvc.LArBadChanTool.EMECAfile = "H6BadChannels.txt"
    # and removing TileBadChanTool
    ToolSvc.remove("TileBadChanTool/TileBadChanTool")

    if doBeamQuality and doSim:
        from TBRec.TBRecConf import TBBeamQualityMC
        TBBeamQualityClus = TBBeamQualityMC("TBBeamQualityClus")
        TBBeamQualityClus.ReadFileforXcryo = False
        TBBeamQualityClus.CheckTrackParams = False
        TBBeamQualityClus.CheckPrimaryTrack = False
        TBBeamQualityClus.CheckVetoScint = False
        TBBeamQualityClus.CheckClusters = True
        TBBeamQualityClus.CheckTrackReco = False
#        from AthenaCommon.AppMgr import athMasterSeq
        topSequence += TBBeamQualityClus

if doMakeEMTopoCluster:
    include ("CaloRec/EMTopoCluster_jobOptions.py")

# -----------------------------
# CBNTAA filling
# -----------------------------
if doCBNT:
   #  common part
   include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
   CBNT_AthenaAware.StreamName="FILE1"
   CBNT_AthenaAware.TreeName="tree"
   include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

   from AnalysisTools.AnalysisToolsConf import AANTupleStream
   topSequence += AANTupleStream()
   topSequence.AANTupleStream.ExtraRefNames = [ "" ]
   topSequence.AANTupleStream.TreeName = "tree"
   topSequence.AANTupleStream.StreamName = "FILE1"
   topSequence.AANTupleStream.ExistDataHeader = False

   #  beam part
   if doBeamDetectors:
      include("TBRec/H6CBNTBeam_jobOptions.py")
   else:
      if doSim:
         from TBRec.TBRecConf import CBNTAA_TBTrack
         CBNT_TBTrack = CBNTAA_TBTrack()
         CBNT_TBTrack.ReadFileforXcryo = False
         CBNT_AthenaAware += CBNT_TBTrack
         from TBRec.TBRecConf import CBNTAA_TBScint
         CBNT_AthenaAware += CBNTAA_TBScint()
 

# Example how to remove some CBNT blocks already declared:
#for x in CBNT_AthenaAware.Members:
#    if not (x == "CBNTAA_CaloCluster/CBNT_CaloClusterTopo" or x == "CBNTAA_Truth/CBNT_Truth"):
#           CBNT_AthenaAware.Members.remove(x)

   #  Topo clusters part
   if doMakeTopoCluster:
       include ("CaloRec/CaloTopoCluster_CBNT_jobOptions.py")
       CBNT_AthenaAware.CBNT_CaloClusterTopo.AddCellDetails = doAddCellDetails
       if doSimpleNoiseTool:
          CBNT_AthenaAware.CBNT_CaloClusterTopo.CellDetailsNoiseTool = SimpleNoiseTool
   if doMakeEMTopoCluster:
       include ("CaloRec/EMTopoCluster_CBNT_jobOptions.py")
       CBNT_AthenaAware.CBNT_EMClusterTopo.AddCellDetails = doAddCellDetails
       if doSimpleNoiseTool:
          CBNT_AthenaAware.CBNT_EMCaloClusterTopo.CellDetailsNoiseTool = SimpleNoiseTool

   from CaloRec.CaloRecConf import CBNTAA_CaloCluster
   if doTopoClusterLocalCalib:
      CBNT_CaloClusterTopoEM = CBNTAA_CaloCluster("CBNT_CaloClusterTopoEM" )
      CBNT_CaloClusterTopoEM.ClusterColl = "CaloTopoCluster"
      CBNT_CaloClusterTopoEM.Suffix = "_topo_em"
      CBNT_CaloClusterTopoEM.EMOnly = False
      CBNT_CaloClusterTopoEM.UseLink = True
      CBNT_CaloClusterTopoEM.AddCellDetails = doAddCellDetails
      CBNT_CaloClusterTopoEM.MaxCaloCluster = 60
      CBNT_CaloClusterTopoEM.MaxCell = 1024
      if doSimpleNoiseTool:
          CBNT_CaloClusterTopoEM.CellDetailsNoiseTool = SimpleNoiseTool
      CBNT_AthenaAware += CBNT_CaloClusterTopoEM

      CBNT_CaloClusterTopoW = CBNTAA_CaloCluster("CBNT_CaloClusterTopoW" )
      CBNT_CaloClusterTopoW.ClusterColl = "CaloWeightTopoCluster"
      CBNT_CaloClusterTopoW.Suffix = "_topo_w"
      CBNT_CaloClusterTopoW.EMOnly = False
      CBNT_CaloClusterTopoW.UseLink = True
      CBNT_CaloClusterTopoW.AddCellDetails = doAddCellDetails
      CBNT_CaloClusterTopoW.MaxCaloCluster = 60
      CBNT_CaloClusterTopoW.MaxCell = 1024
      if doSimpleNoiseTool:
          CBNT_CaloClusterTopoW.CellDetailsNoiseTool = SimpleNoiseTool
      CBNT_AthenaAware += CBNT_CaloClusterTopoW

      CBNT_CaloClusterTopoOOC = CBNTAA_CaloCluster("CBNT_CaloClusterTopoOOC" )
      CBNT_CaloClusterTopoOOC.ClusterColl = "CaloOOCTopoCluster"
      CBNT_CaloClusterTopoOOC.Suffix = "_topo_ooc"
      CBNT_CaloClusterTopoOOC.EMOnly = False
      CBNT_CaloClusterTopoOOC.UseLink = True
      CBNT_CaloClusterTopoOOC.AddCellDetails = doAddCellDetails
      CBNT_CaloClusterTopoOOC.MaxCaloCluster = 60
      CBNT_CaloClusterTopoOOC.MaxCell = 1024
      if doSimpleNoiseTool:
          CBNT_CaloClusterTopoOOC.CellDetailsNoiseTool = SimpleNoiseTool
      CBNT_AthenaAware += CBNT_CaloClusterTopoOOC

   #  TB clusters part
   if doMakeTBCluster:
      CBNT_TBCaloCluster = CBNTAA_CaloCluster("CBNT_TBCaloCluster" )
      CBNT_TBCaloCluster.ClusterColl = "TBClusters"
      CBNT_TBCaloCluster.Suffix = "_tb"
      CBNT_TBCaloCluster.EMOnly = FALSE
      CBNT_TBCaloCluster.UseLink=TRUE
      CBNT_TBCaloCluster.AddCellDetails = doAddCellDetails
      CBNT_TBCaloCluster.MaxCell = 1024
      if doSimpleNoiseTool:
          CBNT_TBCaloCluster.CellDetailsNoiseTool = SimpleNoiseTool
      CBNT_AthenaAware += CBNT_TBCaloCluster

   if doMakeElecCluster:
      CBNT_EMTBCaloCluster = CBNTAA_CaloCluster("CBNT_EMTBCaloCluster" )
      CBNT_EMTBCaloCluster.ClusterColl = "EMTBClusters"
      CBNT_EMTBCaloCluster.Suffix = "_tb_em"
      CBNT_EMTBCaloCluster.EMOnly = FALSE
      CBNT_EMTBCaloCluster.UseLink=TRUE
      CBNT_EMTBCaloCluster.AddCellDetails = doAddCellDetails
      CBNT_EMTBCaloCluster.MaxCell = 1024
      if doSimpleNoiseTool:
          CBNT_EMTBCaloCluster.CellDetailsNoiseTool = SimpleNoiseTool
      CBNT_AthenaAware += CBNT_EMTBCaloCluster

   #  Cells part
   if doMakeCellDescriptions:
       include ("CaloRec/CaloCell_CBNT_jobOptions.py")
   if doMakeH6CellDescriptions:
       include ("TBRec/H6CBNTCalo_jobOptions.py")

   # CalibHits:
   if doSim and doMakeTBCluster:    
      include ("CaloCalibHitRec/CalibrationInfo_CBNT_jobOptions.py")
      CBNT_CalibrationInfo.CalibrationContainers = [ "LArCalibrationHitActive", "LArCalibrationHitInactive" ]
      CBNT_CalibrationInfo.ClusterContainer = "TBClusters"
      CBNT_CalibrationInfo.Suffix = "_tb"

   if doSim and (doMakeTopoCluster or doMakeEMTopoCluster):
      include ("CaloCalibHitRec/CalibrationInfoDM2_CBNT_jobOptions.py")
      CBNT_CalibrationInfoDM2.CalibrationContainerNamesAI = [ "LArCalibrationHitInactive", "LArCalibrationHitActive" ]
      CBNT_CalibrationInfoDM2.CalibrationContainerNamesDM = [ "LArCalibrationHitDeadMaterial" ]
      CBNT_CalibrationInfoDM2.RegularTileHitContainerNames = [ ]
      CBNT_CalibrationInfoDM2.AddHitsDetailsDM  = True
      CBNT_CalibrationInfoDM2.AddHitsDetailsAI  = True
      CBNT_CalibrationInfoDM2.AddHitsDetailsCLS = True
      CBNT_CalibrationInfoDM2.AddHitsDetailsReg = False
      CBNT_CalibrationInfoDM2.AssignDM2Cluster = True
      # truth information
      from CBNT_Truth.CBNTAA_TruthCfg import CBNTAA_Truth
      theCBNTAA_Truth=CBNTAA_Truth()
      from CBNT_Truth.CBNT_TruthConf import CBNT_TruthSelector
      theCBNTAA_Truth += CBNT_TruthSelector("All", PtMin = -1. * GeV, EtaMax = -1, OptionAll = True, Enable = True)
      CBNT_AthenaAware+=theCBNTAA_Truth
   
   # Digits    
   if doCBNTLArDigits:
      from LArROD.LArRODConf import CBNTAA_LArDigits
      CBNT_LArDigit = CBNTAA_LArDigits("CBNT_LArDigit")
      CBNT_LArDigit.NEvents = 50
      CBNT_LArDigit.ContainerKey = "FREE"
      CBNT_LArDigit.DumpGains = "MEDIUMHIGH" # which gains will be saved
      CBNT_LArDigit.DumpCut = 1200 # lower cut for max sample in channel
      CBNT_AthenaAware += CBNT_LArDigit

# Special TTree from Andrei M.:
if doTBTree:
    from TBRec.TBRecConf import TBTree_CaloClusterH6
    TBTree = TBTree_CaloClusterH6("TBTree")
    TBTree.OutputLevel      = OutputLevel
    TBTree.ClusterContainer = "TBClusters"
    TBTree.NoiseToolName    = theCaloNoiseTool.getFullName()
    TBTree.OFNoiseSuppToolName = ""
    if doMakeElecCluster:
        TBTree.useEMTBCluster   = True
    TBTree.Suffix           = "_tb"
    TBTree.addNoise         = False
    TBTree.addMoments       = False
    TBTree.addGain          = True
    TBTree.addTime          = True
    TBTree.addQuality       = True
    TBTree.addBeamTrack     = True
    if not doSim:
       TBTree.addWTC        = True
    else:   
       TBTree.addWTC        = False
    TBTree.zCalo            = 30000
    topSequence += TBTree

# Noise Write in separate file:
if doTBNoiseWrite:
   from TBRec.TBRecConf import TBNoiseWrite
   TBNoise = TBNoiseWrite("TBNoiseWrite")
   topSequence += TBNoise



#--------------------------------------------------------------
# Writing an ESD
#--------------------------------------------------------------
if doWriteESD:
    import AthenaPoolCnvSvc.WriteAthenaPool
    topSequence += CfgMgr.AthenaOutputStream("OutStream", WritingTool="AthenaOutputStreamTool")
    Stream1 = topSequence.OutStream
    Stream1.OutputFile    = ESDOutputFileName
    svcMgr.AthenaSealSvc.CheckDictionary = True
    try:
        fullItemList += ["EventInfo#*"]
    except:
        fullItemList = []
        fullItemList += ["EventInfo#*"]
    fullItemList += [ "CaloCellContainer#*" ]
    fullItemList += [ "CaloClusterContainer#*" ]
    fullItemList += [ "CaloCellLinkContainer#*" ]
    fullItemList += [ "CaloShowerContainer#*" ]
    fullItemList += [ "CaloCalibrationHitContainer#*" ]
    # regular hits for specific studies
    #fullItemList += ["LArHitContainer#LArHitEMB"]
    #fullItemList += ["LArHitContainer#LArHitHEC"]
    #fullItemList += ["LArHitContainer#LArHitEMEC"]
    #fullItemList += ["LArHitContainer#LArHitFCAL"]
    #TestBeam
    fullItemList += [ "TBEventInfo#*" ]
    fullItemList += [ "TBTrack#*" ]
    fullItemList += ["LArG4H6FrontHitCollection#*"]
    if not doSim:
       fullItemList += [ "TBBPCCont#*" ]
       fullItemList += [ "TBHitCont#*" ]
       fullItemList += [ "TBHitPlaneCont#*" ]
       fullItemList += [ "TBIdentifiedParticle#*" ]
#       fullItemList += [ "TBLArCalibDigitContainer#*" ]
       fullItemList += [ "TBLArDigitContainer#*" ]
       fullItemList += [ "TBMWPCCont#*" ]
       fullItemList += [ "TBPhase#*" ]
       fullItemList += [ "TBScintillatorCont#*" ]
       fullItemList += [ "TBTDC#*" ]
       fullItemList += [ "TBTailCatcher#*" ]
       fullItemList += [ "TBTriggerPatternUnit#*" ]

    Stream1.ItemList = fullItemList
    Stream1.TakeItemsFromInput=TRUE
    Stream1.ForceRead=TRUE


# This is a way to block a usage of those folders from "default" Db:
conddb.blockFolder("/CALO/HadCalibration2/CaloEMFrac")
conddb.blockFolder("/CALO/HadCalibration2/H1ClusterCellWeights")
conddb.blockFolder("/CALO/HadCalibration2/CaloOutOfCluster")
conddb.blockFolder("/CALO/HadCalibration2/CaloDMCorr2")
conddb.blockFolder("/CALO/HadCalibration2/CaloOutOfClusterPi0")

if doSim:
   conddb.iovdbsvc.forceRunNumber = RunNumber
conddb.blockFolder("/LAR/LArElecCalibEC04/LArShapeComplete/LArPhysWave")
conddb.blockFolder("/LAR/CellCorrOfl/deadOTX")
print conddb.iovdbsvc.Folders


#print "qqq1> topSequence",topSequence
#print "qqq2> dumpMasterSequence"
#from AthenaCommon.AlgSequence import dumpMasterSequence
#dumpMasterSequence()


#---------------------------
# Chrono svc
#---------------------------
#theApp.Dlls += [ "GaudiAud" ] 
#theAuditorSvc = svcMgr.AuditorSvc
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors =  [ "ChronoAuditor" ]

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
# Output level & events
MessageSvc.OutputLevel      = OutputLevel
theApp.EvtMax =  EvtMax 
ServiceMgr.EventSelector.SkipEvents = EvtSkip
#AthenaEventLoopMgr = Service("AthenaEventLoopMgr")
#AthenaEventLoopMgr.FailureMode=2 # 2-means don't pay attention to FAILURES, just skip event
from AthenaServices.AthenaServicesConf import AthenaEventLoopMgr
AthenaEventLoopMgr.OutputLevel = ERROR

if doSim and doCellNoise:
   ServiceMgr.AtRanluxGenSvc.OutputLevel = ERROR
# # Dump all objects available
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = False

MessageSvc.defaultLimit=1000000;
#MessageSvc.infoLimit          = 20000
#MessageSvc.debugLimit          = 60000
theApp.AuditAlgorithms=False
#theApp.ReflexPluginDebugLevel = 10000
#theApp.AuditServices=True
#theApp.AuditTools=True # FIXME crash on finalisation 
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"
