#======================================================================
__doc__ = """This is an example steering jobOption file for RecExCommission run on MuonSystem in Release 14.X.Y.
It sets default values for muonRecFlags, recFlags, globalflags, DetFlags."""
#======================================================================
# Example JobOPt to monitor Muon Technical(Px) and Commissioning(Mx) Runs
# Last modified Nectarios.Chr. Benekos (UIUC) <Nectarios.Benekos@cern.ch>
#======================================================================
# this version is for the setup we will use for M6 Feb run reprocessing!
#======================================================================
#####
### athena.py -c "CollData=True" -b ../share/MuonDQAMonitoring_CommRunsTier0.py > log_collisions | tee
#####
include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
#======================================================================
import os
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import jobproperties, rec
from AthenaCommon.BFieldFlags import jobproperties
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
#======================================================================
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.detdescr.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.detdescr.Calo_setOn()
DetFlags.Muon_setOn()
# --- printout
DetFlags.Print()
#======================================================================
from RecExConfig.RecFlags import rec as recFlags 
recFlags.Commissioning=True
recFlags.oldRecExCommissionConfig = False
recFlags.oldFlagTopSteering = False
recFlags.ScopingLevel = 4 
#======================================================================
# choose what detector to turn on:
recFlags.doInDet = False
recFlags.doLArg  = False
recFlags.doTile  = False
#======================================================================
# Only Muons
recFlags.doMuon                 = True 
recFlags.doMuonCombined         = False
recFlags.doHist                 = True
#======================================================================
recFlags.doDetStatus            = False # because of bug in DetectorStatus/DetStatusSvc_CondDB.py"
recFlags.doInDet                = False
recFlags.doCalo                 = False
recFlags.doLArg                 = False
recFlags.doTile                 = False
recFlags.readESD                = False
recFlags.doESD                  = True
recFlags.doWriteESD             = True
recFlags.doCBNT                 = False
recFlags.doWriteAOD             = False
recFlags.doWriteTAG             = False
recFlags.doJiveXML              = False
recFlags.doAOD                  = False
recFlags.doAODall               = False
recFlags.doTrigger              = False
recFlags.doPerfMon              = False
recFlags.doHeavyIon             = False
recFlags.doEgamma               = False
recFlags.doTau                  = False
recFlags.doHeavyIon             = False
recFlags.doJetMissingETTag      = False
recFlags.doPersint              = False
recFlags.doMonitoring           = True
#======================================================================
#If True monitor software performance, see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
recFlags.doPerfMon=True 
recFlags.doDetailedPerfMon=True
rec.lock_JobProperties()
#======================================================================
# Set default recAlgsFlags
#======================================================================
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
recAlgsFlags.doMuonIDStandAlone = False # extrapolate Moore tracks to the IP
recAlgsFlags.doMuonIDCombined   = False # combined reconstruction with MuIDCombined
recAlgsFlags.doStaco            = False # combined reconstruction with Staco
recAlgsFlags.doMuTag            = False # muon identification with MuTag (requires Muonboy and Staco)
recAlgsFlags.doMuGirl           = False # muon identification with MuGirl
recAlgsFlags.doTrigger          = False
recAlgsFlags.doTrackRecordFilter = True # filter truth muons getting into the spectrometer
recAlgsFlags.doTileMuID         = False
recAlgsFlags.doCaloTrkMuId      = False
recAlgsFlags.doMissingET        = False
recAlgsFlags.doObjMissingET     = False
recAlgsFlags.doMissingETSig     = False
recAlgsFlags.doEFlow            = False
recAlgsFlags.doEFlowJet         = False
recAlgsFlags.doAtlfast          = False
#======================================================================
# Set default globalflags
#======================================================================
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo      = 'commis'
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.InputFormat = 'bytestream' 
globalflags.Luminosity  = 'zero'
#==========  Switch to the desired algorithms =========================
ATLASCosmicFlags.doLVL1Calo=False
ATLASCosmicFlags.doCTPMon = False
ATLASCosmicFlags.doHLT  = False
ATLASCosmicFlags.doCombinedFit = False
ATLASCosmicFlags.doDQMonitoring = False
## hack to turn off hlt met monitoring
HLTMonDoMET = False
ATLASCosmicFlags.UseDCS = False
ATLASCosmicFlags.doFilteredESD = False  
ATLASCosmicFlags.OnlineJiveXML = False
ATLASCosmicFlags.AtlantisGeometry = False
ATLASCosmicFlags.CaloDetailedJiveXML = False 
ATLASCosmicFlags.doPersint = False
#======================================================================
# Setup Internal flags for the Muon DQA Monitoring (defaults values)
from MuonDQAMonFlags.MuonDQAFlags import MuonDQAFlags as MuonDQAFlags
from AthenaCommon import CfgMgr
from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doGlobalMon = False
DQMonFlags.doLVL1CaloMon = False
DQMonFlags.doCTPMon = False
DQMonFlags.doHLTMon = False
DQMonFlags.doPixelMon = False
DQMonFlags.doSCTMon = False
DQMonFlags.doTRTMon = False
DQMonFlags.doInDetGlobalMon = False
DQMonFlags.doInDetAlignMon = False
DQMonFlags.doInDetPerfMon = False
DQMonFlags.doTileMon = False
DQMonFlags.doLArMon = False
DQMonFlags.doEgammaMon = False
DQMonFlags.doMissingEtMon = False
DQMonFlags.doTauMon = False
DQMonFlags.doJetMon = False
DQMonFlags.doJetTagMon = False
DQMonFlags.doMuonCombinedMon = False
#-----------------------------------------------------------------
DQMonFlags.doMuonRawMon = True
DQMonFlags.doMuonTrackMon = True
DQMonFlags.doMuonSegmentMon = True
DQMonFlags.doMuonAlignMon = True
DQMonFlags.doMuonPhysicsMon = False

# --- Callgrind Tool
MuonDQAFlags.doCallgrind = False #False 

# --- CPU and txt files
MuonDQAFlags.doCpuMuonMon = True
MuonDQAFlags.doMuonDQAPerfMonitoring = True
#----------------------------------------------------------------- 
##if you want to run Hephaestus Tool
### https://twiki.cern.ch/twiki/bin/view/Atlas/UsingHephaestus
MuonDQAFlags.doMuonDQAHephTool = False
#----------------------------------------------------------------- 
MuonDQAFlags.doAuditor = True
MuonDQAFlags.doFinalCPU = False  
MuonDQAFlags.doDumpFile = False
MuonDQAFlags.doSimpleFile = True

# --- Muon Monitoring options
MuonDQAFlags.doMuonMonitoring = True
ATLASCosmicFlags.doMuonMon = True

# --- Muon EventInfo Monitoring option
MuonDQAFlags.doMuonEvtMon = True
ATLASCosmicFlags.doMuonEvtMon = True

# --- Muon Hit Monitoring options
ATLASCosmicFlags.doMuonHitMon = True
# --- for each subdetector
ATLASCosmicFlags.doMDTMon = True
ATLASCosmicFlags.doRPCMon = True
ATLASCosmicFlags.doRPCL1Mon = True
ATLASCosmicFlags.doMDTRPCMon = True
ATLASCosmicFlags.doTGCMon = True
ATLASCosmicFlags.doTGCL1Mon = True
ATLASCosmicFlags.doMDTTGCL1Mon = True   
# --- Temporary for M8---------------
ATLASCosmicFlags.doCSCMon = False
MuonDQAFlags.doCSCMon = False
# --- Temporary for M8---------------

## --- Muon Segment Monitoring options
ATLASCosmicFlags.doMuonSegmMon = True
#
## --- MuonTrk Monitoring options
ATLASCosmicFlags.doMuonTrackMon = True
## --- MuonTrk Trigger Aware Monitoring options
MuonDQAFlags.MuonTrkMonDoTrigger = False
#
## --- Muon Trk Alignement Monitoring
MuonDQAFlags.doMuonTrkAlignMon = True
ATLASCosmicFlags.doMuonTrkAlignMon = True
#
## --- MuonTrk Monitoring options
ATLASCosmicFlags.doMuonPhysMon = False
#======================================================================

if not 'Field' in dir():
    Field = False
if not 'noField' in dir():
    noField = False
if not 'BeamGas' in dir():
    BeamGas = False
if not 'BeamHalo' in dir():
    BeamHalo = False
if not 'SimBeamGas' in dir():
    SimBeamGas = False
if not 'SimBeamHalo' in dir():
    SimBeamHalo = False
if not 'CollData' in dir():
    CollData = False   
#======================================================================
# Muon Monitoring Hist output
if not 'MuonSystem' in dir():
    MuonSystem = "ALL"        
#======================================================================    
#if BeamHalo or BeamGas or SimBeamHalo or SimBeamGas:
#    jobproperties.Beam.beamType.set_Value_and_Lock("singlebeam")
#else:
#    jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")
#
#if SimBeamHalo or SimBeamGas:
#    globalflags.DataSource = 'geant4'
#    globalflags.InputFormat = 'pool'
#======================================================================
#-----------------------------------------------------------------
# Setup Internal flags for the Muon Spectrometer (defaults values)
#-----------------------------------------------------------------
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doMoore = True
muonRecFlags.doMuonboy = True
muonRecFlags.doCSCs=False
#======================================================================
# Set properly beam type, DataSource, InputFormat, Detector Description, Conditions Tag
if globalflags.DataSource() == 'data':
    recFlags.doTruth = False
    jobproperties.Beam.beamType.set_Value_and_Lock("singlebeam")
    globalflags.InputFormat.set_Value_and_Lock('bytestream')
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
    globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
else :
    globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-02-01-00')
    globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-SIM-00-00-00')

if globalflags.DataSource() == 'data' and globalflags.InputFormat == 'bytestream':
    if muonRecFlags.doCSCs():
        athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2008/91897/physics_TGCwBeam/daq.ATLAS.0091897.physics.TGCwBeam.LB0001.SFO-1._0001.data'])
    else:
        athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])

if SimBeamHalo:
    globalflags.DetDescrVersion ='ATLAS-CSC-02-02-00'
    athenaCommonFlags.PoolRDOInput = ["rfio:/castor/cern.ch/user/d/dellacqu/beamHalo_digits.pool.root"]
    ATLASCosmicFlags.IOVDbSvcGlobalTagSim = 'OFLCOND-CSC-00-01-07'
    ATLASCosmicFlags.IOVDbSvcGlobalTagSimOverride = True
if SimBeamGas:
    globalflags.DetDescrVersion ='ATLAS-CSC-01-02-00'
    athenaCommonFlags.PoolRDOInput = ["/afs/cern.ch/user/g/gencomm/w0/RTT_INPUT_DATA/BeamGas/hijing.O.digit._00430.pool.root"]
    jobproperties.BField.solenoidOn=True
    jobproperties.BField.barrelToroidOn=True
    jobproperties.BField.endcapToroidOn=True        
if BeamGas:
    ATLASCosmicFlags.CosmicSetup = "M8"
    globalflags.DetDescrVersion = 'ATLAS-Comm-09-00-00'
    ATLASCosmicFlags.IOVDbSvcGlobalTagData = 'COMCOND-006-01'
    athenaCommonFlags.BSRDOInput = [
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0001.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0005.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0007.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0017.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0022.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-1.0023.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-2.0017.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-2.0018.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-2.0019.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-3.0004.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-3.0007.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-3.0008.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-4.0008.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-4.0009.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-5.0005.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-5.0006.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-5.0007.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-5.0010.data",
        "/castor/cern.ch/user/m/muoncomm/SingleBeam/run87764/run87764.MBTS_BCM_LUCID.LB0000.SFO-5.0021.data"
        ]    
    jobproperties.BField.solenoidOn=False
    jobproperties.BField.barrelToroidOn=True
    jobproperties.BField.endcapToroidOn=True

MuonDQAFlags.doM8=True
if MuonDQAFlags.doM8:
    ATLASCosmicFlags.CosmicSetup = "ATLAS"
    ATLASCosmicFlags.doMasking=False
    include ("RecExCommission/GetInputFiles.py")
    if CollData:
        if not 'RunNumberList' in dir():
            RunNumberList = [ 87863 ] 
        if not 'FilePrefix' in dir():
            FilePrefix = "daq.NoTag" 
        if not 'Partition' in dir():
            Partition      = "BPTX"
        if not 'Trigger' in dir():
            Trigger = "*.physics."+Partition
        if not 'InputDir' in dir():
            InputDir = "/castor/cern.ch/grid/atlas/DAQ/2008/"+str(RunNumberList[0])+"/physics_"+Partition	
        BSRDOInput = []
        for RunNumber in RunNumberList:
            InputFiles = GetInputFilesFromTokens(InputDir,RunNumber,FilePrefix,Trigger)
        print "Input files:"
        athenaCommonFlags.BSRDOInput.set_Value_and_Lock(InputFiles)
        print athenaCommonFlags.BSRDOInput
        ATLASCosmicFlags.NumFile = []
        # Detector description #jamie, use version with field (even though solenoid is off)
        globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEO-04-00-00')
        globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
        # for running over data with field on need this to be setto true
        from AthenaCommon.BFieldFlags import jobproperties
        jobproperties.BField.solenoidOn.set_Value_and_Lock(False)
        jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
        jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
    if Field:
        globalflags.DetDescrVersion ='ATLAS-Comm-09-00-00'
        ATLASCosmicFlags.IOVDbSvcGlobalTagData = 'COMCOND-006-00'  #Database
        ATLASCosmicFlags.Magnet = False
        ATLASCosmicFlags.MagnetToroid = True
        jobproperties.BField.solenoidOn = False
        jobproperties.BField.barrelToroidOn = True
        jobproperties.BField.endcapToroidOn = True
        if not 'RunNumberList' in dir():
            RunNumberList = [ 82075 ] # for M8 ###====LIST of GOOD M7 runs====> /afs/cern.ch/atlas/project/tzero/prod1/goodruns and https://twiki.cern.ch/twiki/bin/view/Atlas/M7MuonRunList
        if not 'FilePrefix' in dir():
            FilePrefix = "daq.ATLAS" # for M8
        if not 'Partition' in dir():
            Partition      = "HLT_Cosmics_MU3"   # for M7,M8 | physics.HLT_Cosmics_MU3 | physics.HLT_Cosmics
        if not 'Trigger' in dir():
            Trigger = "*.physics."+Partition
        if not 'InputDir' in dir():
            InputDir = "/castor/cern.ch/grid/atlas/DAQ/2008/"+str(RunNumberList[0])+"/physics_"+Partition # for M8	
        BSRDOInput = []
        for RunNumber in RunNumberList:
            InputFiles = GetInputFilesFromTokens(InputDir,RunNumber,FilePrefix,Trigger)
        print "Input files:"
        athenaCommonFlags.BSRDOInput=InputFiles
        print athenaCommonFlags.BSRDOInput
        ATLASCosmicFlags.NumFile = []
    if noField:
        globalflags.DetDescrVersion = 'ATLAS-CommNF-09-00-00'      # Detector description
        ATLASCosmicFlags.IOVDbSvcGlobalTagData = 'COMCOND-006-00'  #Database
        jobproperties.BField.solenoidOn = False
        jobproperties.BField.barrelToroidOn = False
        jobproperties.BField.endcapToroidOn = False
        if not 'RunNumberList' in dir():
            RunNumberList = [ 77168 ] # for M8 ###====LIST of GOOD M7 runs====> /afs/cern.ch/atlas/project/tzero/prod1/goodruns and https://twiki.cern.ch/twiki/bin/view/Atlas/M7MuonRunList
        if not 'FilePrefix' in dir():
            FilePrefix = "daq.ATLAS" # for M8
        if not 'Partition' in dir():
            Partition      = "HLT_Cosmics_MU3"   # for M7,M8 | physics.HLT_Cosmics_MU3 | physics.HLT_Cosmics
        if not 'Trigger' in dir():
            Trigger = "*.physics."+Partition
        if not 'InputDir' in dir():
            InputDir = "/castor/cern.ch/grid/atlas/DAQ/2008/"+str(RunNumberList[0])+"/physics_"+Partition # for M8	
        BSRDOInput = []
        for RunNumber in RunNumberList:
            InputFiles = GetInputFilesFromTokens(InputDir,RunNumber,FilePrefix,Trigger)
        print "Input files:"
        athenaCommonFlags.BSRDOInput=InputFiles
        print athenaCommonFlags.BSRDOInput
        ATLASCosmicFlags.NumFile = []

###############################################################
#
#  Configuration jobOptions file
#
###############################################################  
#=======================Output Files===================================
if Field or noField or CollData:
    DQMonFlags.histogramFile = MuonSystem+"MuonMonitoring_"+str(ATLASCosmicFlags.CosmicSetup)+"Setup_"+str(RunNumber)+".root"
    athenaCommonFlags.PoolESDOutput = MuonSystem+"MuonMonitoringESD_"+str(ATLASCosmicFlags.CosmicSetup)+"_"+str(RunNumber)+".pool.root"
    ATLASCosmicFlags.FilteredESDOutputFile = MuonSystem+"MuonMonitoringESD_Filtered"+str(ATLASCosmicFlags.CosmicSetup)+"_"+str(RunNumber)+".pool.root"
    
if BeamHalo or BeamGas:
    recFlags.RootHistoOutput = MuonSystem+"MuonMonitoring_BeamHalo.root"
    athenaCommonFlags.PoolESDOutput = MuonSystem+"MuonMonitoringESD_BeamHalo.pool.root"
    ATLASCosmicFlags.FilteredESDOutputFile = MuonSystem+"MuonMonitoringESD_Filtered_BeamHalo.pool.root"

if SimBeamHalo or SimBeamGas:
    recFlags.RootHistoOutput = MuonSystem+"MuonMonitoring_SimBeamHalo.root"
    athenaCommonFlags.PoolESDOutput = MuonSystem+"MuonMonitoringESD_SimBeamHalo.pool.root"
    ATLASCosmicFlags.FilteredESDOutputFile = MuonSystem+"MuonMonitoringESD_Filtered_SimBeamHalo.pool.root"
 
#======================================================================
#Steering flags
#======================================================================
athenaCommonFlags.EvtMax.set_Value_and_Lock(10)
athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)
#======================================================================
if 'doAuditors' in dir():
    from RecExConfig.RecFlags  import jobproperties
    jobproperties.Rec.doNameAuditor.set_Value(True)
#====================================================================== 
#=BEGINNN=====================================================================
# the main jobOpt
if BeamGas:
    include( "RecExCommission/RecExCommission_SingleBeamtopOptions.py" )
else:
    include( "RecExCommission/RecExCommission.py" )
    include ("RecExCommon/RecExCommon_topOptions.py")
#==ENDDDDD====================================================================
 
####Change Collections####################
#if MuonDQAFlags.doMuonTrackMon:
#    ToolSvc += CfgMgr.MuonMonTrkSummary(MuonTrackCollections=["MooreTrackParticles" ,"MuonboyTrackParticles"])
#    ToolSvc += CfgMgr.MuonTrackValidation(MuonTrackCollections=["MooreTrackParticles" ,"MuonboyTrackParticles"])
#    ToolSvc += CfgMgr.MuonAlignMonGenericTracks(MuonTrackCollections=["MooreTrackParticles" ,"MuonboyTrackParticles"])     

###############################################################
#
#  Configuration jobOptions file
#
###############################################################
#--------------------------------------------------------------
# Performace monitoring (CPU and Memory)
#--------------------------------------------------------------
if MuonDQAFlags.doCpuMuonMon:
    include("MuonDQAMonitoring/MuonDQAMonitoring_CpuPerfMon.py")
    jobproperties.PerfMonFlags.OutputFile = MuonSystem+"muondqa.perfmon.pmon.gz"
    
#if recFlags.doPerfMon:
#    from PerfMonComps.PerfMonFlags import jobproperties
#    jobproperties.PerfMonFlags.doPerEvtLeakCheck = True

if MuonDQAFlags.doMuonDQAPerfMonitoring and MuonDQAFlags.doMuonDQAHephTool:
    from PerfMonComps.PerfMonFlags import jobproperties
    jobproperties.PerfMonFlags.doPerEvtLeakCheck = True
#--------------------------------------------------------------
# Valgrind- Callgrind
#--------------------------------------------------------------
if MuonDQAFlags.doCallgrind:
     from Valkyrie.ValkyrieConf import ValgrindSvc
     valgrindSvc = ValgrindSvc( OutputLevel = VERBOSE )
     if ATLASCosmicFlags.doMuonHitMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindHitAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
     if ATLASCosmicFlags.doMuonTrackMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindTrkAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only
     if ATLASCosmicFlags.doMuonSegmMon:
         valgrindSvc.ProfiledAlgs += MuonDQAFlags.CallgrindSegmAlgs
         valgrindSvc.IgnoreFirstNEvents = MuonDQAFlags.CallgrindSkip      # to start instrumentation after the second event only          
     # Add service and turn on auditing
     svcMgr += valgrindSvc
     theApp.CreateSvc += [svcMgr.ValgrindSvc.getFullJobOptName()]
     theApp.AuditAlgorithms = True
     theApp.AuditTools      = True
     theApp.AuditServices   = True
# -------------------------------------------------------------
#Output level
# -------------------------------------------------------------
# --------------For RawData Monitoring----------------------------
if MuonDQAFlags.doMuonHitMon:
    ToolSvc += CfgMgr.MDTRawDataBSValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.MDTRawDataESDValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.RPCRawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.MDTvsRPCRawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.RPCLV1RawDataValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.TGCRawDataESDValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.TGCLV1RawDataESDValAlg(OutputLevel = INFO)
    ToolSvc += CfgMgr.MDTvsTGCRawDataESDValAlg(OutputLevel = INFO)
    if MuonDQAFlags.doCSCMon:
        ToolSvc += CfgMgr.CSCPrdValAlg(OutputLevel = INFO)
        ToolSvc += CfgMgr.CSCRdoValAlg(OutputLevel = INFO)
        ToolSvc += CfgMgr.CSCClusterTree(OutputLevel = INFO)
        ToolSvc += CfgMgr.CSCClusterValAlg(OutputLevel = INFO)
# --------------For MuonSegment Monitoring------------------------    
if MuonDQAFlags.doMuonSegmMon:
    ToolSvc += CfgMgr.MuonSegmValidation(OutputLevel = INFO)
# --------------For MuonTrk Alignment Monitoring------------------------    
if MuonDQAFlags.doMuonTrkAlignMon:
    ToolSvc += CfgMgr.MuonAlignValidation(OutputLevel = INFO)
# --------------For MuonTrk Monitoring----------------------------
# --------------For No Trigger MuonTrk Monitoring-----------------
if MuonDQAFlags.doMuonTrackMon and not MuonDQAFlags.MuonTrkMonDoTrigger:
    ToolSvc += CfgMgr.MuonSelectedTracksMon(OutputLevel = INFO) 
    ToolSvc += CfgMgr.MuonGenericTracksMon(OutputLevel = INFO) 
# --------------For TriggerAware MuonTrk Monitoring---------------
if MuonDQAFlags.doMuonTrackMon and MuonDQAFlags.MuonTrkMonDoTrigger:
    ### UseTriggerVector =True
    ToolSvc += CfgMgr.MuonSelectedTracksMon_Trig(OutputLevel = INFO) 
    ToolSvc += CfgMgr.MuonGenericTracksMon_Trig(OutputLevel = INFO)     
    ### UseTriggerVector = False
    ToolSvc += CfgMgr.MuonSelectedTracksMon_trigmu10(OutputLevel = INFO) 
    ToolSvc += CfgMgr.MuonSelectedTracksMon_trigmu20(OutputLevel = INFO)         
    ToolSvc += CfgMgr.MuonSelectedTracksMon_trigmu40(OutputLevel = INFO) 
    ToolSvc += CfgMgr.MuonSelectedTracksMon_trig2mu10(OutputLevel = INFO)
    ToolSvc += CfgMgr.MuonGenericTracksMon_trigmu10(OutputLevel = INFO)
    ToolSvc += CfgMgr.MuonGenericTracksMon_trigmu20(OutputLevel = INFO)
    ToolSvc += CfgMgr.MuonGenericTracksMon_trigmu40(OutputLevel = INFO)
    ToolSvc += CfgMgr.MuonGenericTracksMon_trig2mu10(OutputLevel = INFO)     
# --------------For MuonEventInfo Monitoring----------------------------
if MuonDQAFlags.doMuonEvtMon:
    ToolSvc += CfgMgr.MuonDQA__MuonEventInfoMonTool(OutputLevel = INFO)  
#--------------------------------------------------------------------------- 
Service( "THistSvc" ).OutputLevel = WARNING
#--------------------------------------------------------------------------- 
# THIS IS TEMPORY HACK TO GET RID OF MUON ERRORS BEING PRINTED FOR EVERY EVENT
# NEED A REAL FIX 
#--------------------------------------------------------------------------- 
Algorithm("TgcRdoToTgcPrepData").OutputLevel = ERROR
#--------------------------------------------------------------------------- 
print "List all DLL"
print theApp.Dlls
print "List all ExtSvc"
print theApp.ExtSvc
print "List of all top algorithms"
print theApp.TopAlg
#--------------------------------------------------------------------------- 
print topSequence
print ServiceMgr
print ToolSvc
#======================================================================
