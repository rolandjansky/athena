# MuonTP_topOptions.py
#
# Massimiliano Bellomo
# July 2014
#
# Example job option for running tag-and-probe tools in Athena from xAOD
#

# Specify input file.
if not "pandaJob" in globals():
    include ('MuonPerformanceAlgs/Zmumu_19.0.2.1.py')
else:
    InputAODList=[FNAME]
#--------------------------------------------------------------
# Input stream
#--------------------------------------------------------------

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = InputAODList

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------

# Number of events to be processed (default is until the end of
# input, or -1, however, since we have no input, a limit needs
# to be set explicitly, here, choose 10)
theApp.EvtMax =  -1
if not "pandaJob" in globals():
    theApp.EvtMax = -1
    
# optional override for local testing
if "EVTMAX" in  globals():
    theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Configure algorithm.
#--------------------------------------------------------------

# Allow messge service more than default 500 lines.
ServiceMgr.MessageSvc.infoLimit = 200000

# Full job is a list of algorithms
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# add a track iso tool

from IsolationTool.IsolationToolConf import xAOD__TrackIsolationTool

ToolSvc += xAOD__TrackIsolationTool("MyTrackIsoTool")
from MuonPerformanceAlgs.MuonPerformanceAlgsConf import TrackIsolationDecorAlg

job += TrackIsolationDecorAlg("MyDecorator")
job.MyDecorator.IsolationTool = ToolSvc.MyTrackIsoTool


include ('MuonPerformanceAlgs/ZmumuTPAnalysis.py')
# add muon quality updater

AddMuonSelectionTool()


# commented for now. Works with the lates MuonSelectorTools
#from MuonSelectorTools.MuonSelectorToolsConf import CP__MuonQualityUpdaterAlg
#alg = CfgMgr.CP__MuonQualityUpdaterAlg("MuonQualityUpdater",Input="Muons",Output="UpdatedMuons")
#alg.Tool = ToolSvc.TagProbeMuonSelectionTool
#job += alg

# now we can add the TP analysis itself

MuonContainerToUse = "Muons"
if hasattr(job, "MuonQualityUpdater"):
    MuonContainerToUse = "UpdatedMuons"

    
do_Zmumu_RecoEff_TagProbe = True        # this is for Z->mumu, reco eff. You can add your own analysis in a similar way.

if do_Zmumu_RecoEff_TagProbe:
    ##########################################################################################
    # Add the Zmm TP algorithm using ID tracks as probes and matching to muons
    AddConfiguredZmumuTPAlg(name_suffix = "IDProbes",
                            ProbeKind = "ID",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doMedium = True,
                            doStandalone = False, doCaloTag = True,
                            doEtaSlices=True)


    ##########################################################################################
    # Add the Zmm TP algorithm using CaloTag muons as probes and matching to muons
    AddConfiguredZmumuTPAlg(name_suffix = "CaloProbes",
                            ProbeKind = "CaloTag",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doMedium = True,
                            doStandalone = False, doCaloTag = False,
                            doEtaSlices=True)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to ID tracks
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_ID",
                            ProbeKind = "MS",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doCB = False, doMedium = False,
                            doStandalone = False, doCaloTag = False,
                            doEtaSlices=True)


    ##########################################################################################
    # Add the Zmm TP algorithm using MS muons as probes and matching to CaloTag muons
    AddConfiguredZmumuTPAlg(name_suffix = "MSProbes_Muon",
                            ProbeKind = "MS",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doMedium = True,
                            doStandalone = False, doCaloTag = True,
                            doEtaSlices=True)

    ##########################################################################################
    # Add the Zmm TP algorithm using Truth probes for Muons
    AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_Muon",
                            ProbeKind = "Truth",
                            MatchContainer = MuonContainerToUse,
                            doID = False, doCB = True, doMedium = True,
                            doStandalone = True, doCaloTag = True,
                            doEtaSlices=True)

    ##########################################################################################
    # Add the Zmm TP algorithm using Truth probes for Tracks
    AddConfiguredZmumuTPAlg(name_suffix = "TruthProbes_ID",
                            ProbeKind = "Truth",
                            MatchContainer = "InDetTrackParticles",
                            doID = True, doCB = False, doMedium = False,
                            doStandalone = False, doCaloTag = False,
                            doEtaSlices=True)

print ToolSvc
print job
# Add HistSvc
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MUONTP DATAFILE='muontp.root' OPT='RECREATE'"]
