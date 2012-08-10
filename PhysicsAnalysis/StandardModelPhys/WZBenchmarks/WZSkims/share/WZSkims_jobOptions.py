# WZSkims Athena Job Options file for various skims
# Homero Martinez, Bernardo Resende, Sanya Solodkov
# 17/02/12

###################################################

# new light Skim (keep only high-pt objects and much less details)
if not 'LightSkim' in dir():
    LightSkim = False;

# use old full Skim, but less details for MET and Jets and keep only high-pt objects
if not 'MediumSkim' in dir():
    MediumSkim = False;

# enable or disable GSF electrons
from RecExConfig.RecAlgsFlags import recAlgs
if not 'doGSF' in dir():
    doGSF = recAlgs.doEgammaBremReco();

# enable or disable trigger fillers (to speed-up tests)
if not 'doTrig' in dir():
    doTrig = True; # can not use global variable doTrigger
    
# global output level (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL)
if not 'OL' in dir():
    OL = WARNING; # can not use global variable OutputLevel

# special output level for WZSkim algorithm
if not 'SkimOL' in dir():
    SkimOL = min(INFO,OL)

if not 'EvtMAX' in dir():
    EvtMAX = -1; # can not use global variable EvtMax

if not 'EvtMIN' in dir():
    EvtMIN = 0;

# Set up a logger:
from AthenaCommon.Logging import logging
WZSkims_msg = logging.getLogger( 'WZSkims' )
WZSkims_msg.setLevel(OL)
WZSkims_msg.info("LightSkim  = %s " % LightSkim)
WZSkims_msg.info("MediumSkim = %s " % MediumSkim)
WZSkims_msg.info("doGSF      = %s " % doGSF)
WZSkims_msg.info("doTrig     = %s " % doTrig)
WZSkims_msg.info("OL         = %s " % OL)
WZSkims_msg.info("SkimOL     = %s " % SkimOL)
WZSkims_msg.info("EvtMIN     = %s " % EvtMIN)
WZSkims_msg.info("EvtMAX     = %s " % EvtMAX)

###################################################

# Debug output
from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.OutputLevel = OL
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%80W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False

# Input file
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]

# Output file
if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'WZSkimsD3PD.root'

# Configure RecExCommon
from RecExConfig.RecFlags import rec
rec.doCBNT.set_Value_and_Lock(False)
rec.doWriteESD.set_Value_and_Lock(False)
rec.doWriteAOD.set_Value_and_Lock(False)
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doDPD.set_Value_and_Lock(False)
rec.doWriteTAG.set_Value_and_Lock(False)
rec.doForwardDet.set_Value_and_Lock(False)
rec.OutputLevel = OL

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
D3PDMakerFlags.DoTrigger = doTrig

if LightSkim:
    rec.DPDMakerScripts.append("PhysicsD3PDMaker/SMWZLightD3PD.py")
else:
    rec.DPDMakerScripts.append("PhysicsD3PDMaker/SMWZD3PD.py")

if doGSF:
    include ("RecExCond/RecExCommon_flags.py")
    DetFlags.ID_setOn()
    DetFlags.geometry.Calo_setOn()
    recAlgs.doEgammaBremReco.set_Value_and_Lock(True)

# Main jobOption - must always be included!!!
include ("RecExCommon/RecExCommon_topOptions.py")

# Number of events to be processed
theApp.EvtMax = EvtMAX
svcMgr.EventSelector.SkipEvents = EvtMIN


# Stops writing of monitoring ntuples
from PerfMonComps.PerfMonFlags import jobproperties as jp
jp.PerfMonFlags.doMonitoring = False
jp.PerfMonFlags.doFastMon = False




# Configure WZSkims
from WZSkims.WZSkimsConf import WZSkimsMain

# Select skim to do (please select only one!)
if ('doJpsiee'               in dir() and doJpsiee):
    WZSkimsMain.doJpsieeSkim               = True
elif ('doJpsimumu'             in dir() and doJpsimumu):
    WZSkimsMain.doJpsimumuSkim             = True
elif ('doZee'                  in dir() and doZee):
    WZSkimsMain.doZeeSkim                  = True
elif ('doZmumu'                in dir() and doZmumu):
    WZSkimsMain.doZmumuSkim                = True
elif ('doWenu'                 in dir() and doWenu):
    WZSkimsMain.doWenuSkim                 = True
elif ('doWmunu'                in dir() and doWmunu):
    WZSkimsMain.doWmunuSkim                = True
elif ('doTagAndProbeElec'      in dir() and doTagAndProbeElec):
    WZSkimsMain.doTagAndProbeElecSkim      = True
elif ('doTagAndProbeMuon'      in dir() and doTagAndProbeMuon):
    WZSkimsMain.doTagAndProbeMuonSkim      = True
elif ('doBackgroundElec'       in dir() and doBackgroundElec):
    WZSkimsMain.doBackgroundElecSkim       = True
elif ('doBackgroundMuon'       in dir() and doBackgroundMuon):
    WZSkimsMain.doBackgroundMuonSkim       = True
elif ('doZtautau'              in dir() and doZtautau):
    WZSkimsMain.doZtautauSkim              = True
elif ('doZeeWithTagAndProbe'   in dir() and doZeeWithTagAndProbe):
    WZSkimsMain.doZeeWithTagAndProbeSkim   = True
elif ('doZmumuWithTagAndProbe' in dir() and doZmumuWithTagAndProbe):
    WZSkimsMain.doZmumuWithTagAndProbeSkim = True
elif ('doTwoLeptons'           in dir() and doTwoLeptons):
    WZSkimsMain.doTwoLeptonsSkim           = True
elif ('doThreeLeptons'         in dir() and doThreeLeptons):
    WZSkimsMain.doThreeLeptonsSkim         = True
elif ('doWenujj'               in dir() and doWenujj):
    WZSkimsMain.doWenujjSkim               = True
elif ('doWmunujj'              in dir() and doWmunujj):
    WZSkimsMain.doWmunujjSkim              = True
else:
    WZSkimsMain.doCustomSkim               = True

## By default, no other WZSkims properties need to be changed. Uncomment following lines only if needed.

# Containers to read (all skims)

#WZSkimsMain.electronContainerName = "ElectronAODCollection"
#WZSkimsMain.muonContainerName = "BOTH"          # options: StacoMuonCollection, MuidMuonCollection, BOTH
#WZSkimsMain.trackContainerName = "TrackParticleCandidate"
#WZSkimsMain.missingEtObjectName = "MET_RefFinal"

# Custom skim configuration (only valid if doCustomSkim = True)

#WZSkimsMain.electronNumber = 2
#WZSkimsMain.electronNumber2 = 0
#WZSkimsMain.electronPtCut = 5000.0
#WZSkimsMain.electronPtCut2 = 1000.0
#WZSkimsMain.electronEtaCut = 2.5
#WZSkimsMain.electronEtaCut2 = 2.5
#WZSkimsMain.electronType = "NOCHECK"   # options: LOOSE, MEDIUM, TIGHT, NOCHECK
#WZSkimsMain.electronType2 = "NOCHECK"   

#WZSkimsMain.muonNumber = 0
#WZSkimsMain.muonNumber2 = 0
#WZSkimsMain.muonPtCut = 8000.0
#WZSkimsMain.muonPtCut2 = 15000.0
#WZSkimsMain.muonEtaCut = 2.5
#WZSkimsMain.muonEtaCut2 = 2.5
#WZSkimsMain.muonType = "NOCHECK"
#WZSkimsMain.muonType2 = "NOCHECK"

#WZSkimsMain.doTrackCut = False
#WZSkimsMain.TrackPtCut = 10000.0
#WZSkimsMain.TrackEtaCut = 2.5


#WZSkimsMain.doEtmissCut = False
#WZSkimsMain.EtmissCut = 30000.0

#WZSkimsMain.doInvMassCut = True
#WZSkimsMain.Mmin = 50000.0
#WZSkimsMain.Mmax = 200000.0

#WZSkimsMain.doTriggerCut = False
#WZSkimsMain.TriggerName = ""
#WZSkimsMain.TriggerName2 = ""

#WZSkimsMain.doPrescale = False
#WZSkimsMain.prescaleRate = 1

# Output level
WZSkimsMain.OutputLevel = SkimOL




# D3PD stream, handled by MultipleStreamManager
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
from EventCommonD3PDMaker.CutFlowMetadataConfig import CutFlowMetadataConfig

SkimD3PDStream = MSMgr.NewRootStream( "OutStream", tupleFileOutput, "physics" )

# add specific truth jets
include ("PhysicsD3PDMaker/MyJetMake.py")

#configure MuonScatteringAngleSignificanceTool
include("JetTagD3PDMaker/MuonScatteringSigToolConfig.py")

if LightSkim or MediumSkim:
    #### add specific containers
    include ("PhysicsD3PDMaker/ElectronSelector.py")
    include ("PhysicsD3PDMaker/PhotonSelector.py")
    include ("PhysicsD3PDMaker/MuonSelector.py")
    include ("PhysicsD3PDMaker/JetSelector.py")
    include ("PhysicsD3PDMaker/TauSelector.py")

    if doGSF:
        gsfCont = 'HighPtGSFElectrons'
    else:
        gsfCont = 'None'
        
    if LightSkim:
        from PhysicsD3PDMaker.SMWZLightD3PD import SMWZLightD3PD
        SMWZLightD3PD( SkimD3PDStream,
                       stdElectronContainer = 'HighPtElectrons',
                       gsfElectronContainer = gsfCont,
                       stacoMuonContainer   = 'HighPtStacoMuons',
                       muidMuonContainer    = 'HighPtMuidMuons',
                       photonContainer      = 'HighPtPhotons',
                       tauContainer         = 'HighPtTaus',
                       jetContainer         = 'HighPtJets' )

    else:    
        from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
        SMWZD3PD( SkimD3PDStream,
                  stdElectronContainer = 'HighPtElectrons',
                  gsfElectronContainer = gsfCont,
                  stacoMuonContainer   = 'HighPtStacoMuons',
                  muidMuonContainer    = 'HighPtMuidMuons',
                  caloMuonContainer    = 'None',
                  photonContainer      = 'HighPtPhotons',
                  tauContainer         = 'HighPtTaus',
                  jetEM4Container      = 'HighPtJets',
                  jetEM6Container      = 'HighPtJetsEM6',
                  jetLC4Container      = 'HighPtJetsLC4',
                  jetLC6Container      = 'HighPtJetsLC6',
                  jetDetails           = 2,
                  METDetails           = 2)

else:
    from PhysicsD3PDMaker.SMWZD3PD import SMWZD3PD
    SMWZD3PD( SkimD3PDStream )

SkimD3PDStream.filterSeq += WZSkimsMain("WZSkimsMain")
SkimD3PDStream.MetadataTools += [ CutFlowMetadataConfig( SkimD3PDStream ) ]


# additional branch selection for W -> l nu j j 
if ('doWenujj' in dir() and doWenujj) or ('doWmunujj' in dir() and doWmunujj):
    include("WZSkims/Wlnujj_branchSelection.py")


# Debug output once again
from AthenaCommon.AppMgr import theApp
svcMgr = theApp.serviceMgr()
svcMgr.MessageSvc.OutputLevel = OL
svcMgr.MessageSvc.defaultLimit = 1000000
svcMgr.MessageSvc.Format = "% F%80W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.useColors = False
