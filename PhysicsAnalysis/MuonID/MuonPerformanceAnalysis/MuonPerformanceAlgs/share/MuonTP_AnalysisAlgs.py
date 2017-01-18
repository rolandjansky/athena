from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence

from MuonPerformanceAlgs import ZmumuTPRecoAnalysis
from MuonPerformanceAlgs import DYmumuTPRecoAnalysis
from MuonPerformanceAlgs import DYmumuTPMuonAnalysis
from MuonPerformanceAlgs import ZmumuTPMuonAnalysis
from MuonPerformanceAlgs import JPsiTPRecoAnalysis
from MuonPerformanceAlgs import JPsiTPMuonAnalysis
from MuonPerformanceAlgs import CommonMuonTPConfig
from MuonPerformanceAlgs import HighPtSingleMuonWriter


from PyUtils import AthFile
af = AthFile.fopen(ServiceMgr.EventSelector.InputCollections[0]) #opens the first file from the InputCollections list
# af.fileinfos #this is a dict of dicts, take a look at what's available (e.g. do af.fileinfos.keys() to see the main keys)! Below are some examples:
streamName = af.fileinfos['stream_names'] #will be something like 'StreamDAOD_XXX' if a derivation
# print streamName
is_MUON1 = "DAOD_MUON1" in streamName[0]
is_MUON23 = "DAOD_MUON2" in streamName[0] or "DAOD_MUON3" in streamName[0]
# sys.exit(1)
# find out what we are running on
# print " Checking input type!"
# is_MUON1 = False
# is_MUON23 = False
from AthenaCommon.JobProperties import jobproperties

ClosureTest = jobproperties.MuonTPFlags.doClosure()

# acf =  jobproperties.AthenaCommonFlags
# try:
#     for f in acf.PoolAODInput.StoredValue:
#         if "DAOD_MUON1" in f:
#             is_MUON1 = True
#             print " Detected we are running on a MUON1 DAOD. Turning off JPSi analysis components!"
#         if "DAOD_MUON2" in f or "DAOD_MUON3"  in f:
#             is_MUON23 = True
#             print " Detected we are running on a MUON2/3 DAOD. Turning off Z analysis components!"
#     # also look in here!
# except:
#     pass
# try:
#     for f in acf.FilesInput.StoredValue:
#         if "DAOD_MUON1" in f:
#             is_MUON1 = True
#             print " Detected we are running on a MUON1 DAOD. Turning off JPSi analysis components!"
#         if "DAOD_MUON2" in f or "DAOD_MUON3"  in f:
#             is_MUON23 = True
#             print " Detected we are running on a MUON2/3 DAOD. Turning off Z analysis components!"

# except:
#     pass
#     
if is_MUON1:   
    print "==== Detected we are running on a MUON1 derivation, configuring for this scenario ======"    
elif is_MUON23:   
    print "==== Detected we are running on a MUON2/3 derivation, configuring for this scenario ======"    
else: 
    print "==== Detected we are running on a primary AOD, configuring for this scenario ======"

IsRunOnDAOD = (is_MUON1 or is_MUON23)
############## Zmumu T&P Configuration ########################
do_Zmumu_RecoEff_TagProbe      = True # this is for Z->mumu, reco eff. You can add your own analysis in a similar way.
do_Zmumu_RecoEff_HighEta      = True # this is for Z->mumu, reco eff at high eta. You can add your own analysis in a similar way.

do_DYmumu_RecoEff_TagProbe      = False # this is for high-mass DY->mumu, reco eff. You can add your own analysis in a similar way.
do_Zmumu_IsolationEff_TagProbe = True # this is for Z->mumu, isolation eff. You can add your own analysis in a similar way.
do_Zmumu_TriggerEff_TagProbe   = True # this is for Z->mumu, Trigger eff.
do_DYmumu_IsolationEff_TagProbe = False # this is for high-mass DY->mumu, isolation eff. You can add your own analysis in a similar way.
do_DYmumu_TriggerEff_TagProbe   = False # this is for high-mass DY->mumu, Trigger eff.

############## JPsi T&P Configuration ########################
do_JPsi_RecoEff_TagProbe       = True
do_JPsi_IsolationEff_TagProbe  = True
do_JPsi_TriggerEff_TagProbe    = True

############## High-Pt single muons ##########################
do_HighPtSingleMuons       = True

import os
import os.path
GRL_to_use  = []
if jobproperties.MuonTPFlags.GRL() != '':
    GRL_to_use  = [os.environ["TestArea"]+'/PhysicsAnalysis/MuonID/MuonPerformanceAnalysis/MuonPerformanceAlgs/scripts/'+jobproperties.MuonTPFlags.GRL()]
    print 'GRL : ',GRL_to_use
    if not os.path.isfile(GRL_to_use[0]) :
        print "Careful, you configured a GRL that does not exit! Running without GRL!"
        GRL_to_use = []
#GRL_to_use  = ["data15_13TeV.periodA_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"]
#GRL_to_use  = ["/afs/cern.ch/user/a/atlasdqm/grlgen/All_Good/data15_13TeV.periodA_DetStatus-v62-pro18_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"]
CommonMuonTPConfig.AddMuonTPMetaAlg()
if not (is_MUON1 or is_MUON23):
    CommonMuonTPConfig.AddPromptLeptonDecorators()
CommonMuonTPConfig.AddCalibratedMuonsProvider()
if not (is_MUON1 or is_MUON23):
    from MuonPerformanceAlgs import  JPsiVertexFitSetup
    JPsiVertexFitSetup.AddJPsiVertexingAlgs(prefix='MUON1')
#CommonMuonTPConfig.AddTrackIsolationDecorAlg()
CommonMuonTPConfig.AddGoodRunsListSelectionTool(GRL_to_use)

##### Reco eff analysis options
if do_Zmumu_RecoEff_TagProbe and not is_MUON23:
    ZmumuTPRecoAnalysis.AddZmumuTPRecoAnalysis(doEtaSlices=False,
                                               writeNtuple=True,
                                               doClosure=ClosureTest,
                                               doDRSys=True,
                                               doValid=False,
                                               doProbeWithoutCuts=False,
                                               DoProbeMatchPlots = jobproperties.MuonTPFlags.doPlots(),   # not used for Z any longer
                                               ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                                               IsRunOnDAOD=IsRunOnDAOD)
if do_DYmumu_RecoEff_TagProbe and not is_MUON23:
    DYmumuTPRecoAnalysis.AddDYmumuTPRecoAnalysis(doEtaSlices=False,
                                               writeNtuple=True,
                                               doClosure=ClosureTest,
                                               doDRSys=True,
                                               doValid=False,
                                               DoProbeMatchPlots = jobproperties.MuonTPFlags.doPlots(),
                                               ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                                               IsRunOnDAOD=IsRunOnDAOD)
if do_JPsi_RecoEff_TagProbe: # and not is_MUON1:
    JPsiTPRecoAnalysis.AddJPsiTPRecoAnalysis(writeNtuple=True,
                                             doValid=False,
                                             doDRSys=True,
                                             doClosure=ClosureTest,
                                             DoProbeMatchPlots = jobproperties.MuonTPFlags.doPlots(),
                                             ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                                               IsRunOnDAOD=IsRunOnDAOD)

if (do_Zmumu_TriggerEff_TagProbe or do_Zmumu_IsolationEff_TagProbe or do_Zmumu_RecoEff_HighEta) and not is_MUON23:
    ZmumuTPMuonAnalysis.AddZmumuTPMuonAnalysis( doIso = do_Zmumu_IsolationEff_TagProbe,
                            doTrig = do_Zmumu_TriggerEff_TagProbe,
                            doTrigEtaSlices    = False,
                            doIsolEtaSlices    = False,
                            doIsolPlots      = False,
                            doTriggerPlots      = True,
                            doVeryLooseProbes = True,
                            writeNtuple=True, doClosure=ClosureTest,
                            doDRSys=False,
                            doLooseProbes=True, doMediumProbes=False,
                            doTightProbes=False, doTruthProbes=True,
                            doHighPtProbes=False,
                            doLooseTrackOnly=True,
                            doMinimalCutsTree=jobproperties.MuonTPFlags.IncludeMinimalCutTree(),
                            doLoose=True,
                            doTight=True,
                            doGradient=True,
                            doGradientLoose=True,
                            doL1=True,
                            doHLT=True, DoProbeMatchPlots=jobproperties.MuonTPFlags.doPlots(),
                            ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                            IsRunOnDAOD=IsRunOnDAOD)

if (do_DYmumu_TriggerEff_TagProbe or do_DYmumu_IsolationEff_TagProbe) and not is_MUON23:
    DYmumuTPMuonAnalysis.AddDYmumuTPMuonAnalysis( doIso = do_DYmumu_IsolationEff_TagProbe,
                            doTrig = do_DYmumu_TriggerEff_TagProbe,
                            doTrigEtaSlices    = False,
                            doIsolEtaSlices    = False,
                            doIsolPlots      = False,
                            doTriggerPlots      = True,
                            writeNtuple=True, doClosure=ClosureTest,
                            doDRSys=False,
                            doLooseProbes=True, doMediumProbes=False,
                            doTightProbes=False, doTruthProbes=True,
                            doHighPtProbes=False,
                            doLooseTrackOnly=True,
                            doMinimalCutsTree=jobproperties.MuonTPFlags.IncludeMinimalCutTree(),
                            doLoose=True,
                            doTight=True,
                            doGradient=True,
                            doGradientLoose=True,
                            doL1=True,
                            doHLT=True, DoProbeMatchPlots=jobproperties.MuonTPFlags.doPlots(),
                            ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                            IsRunOnDAOD=IsRunOnDAOD)

if (do_JPsi_TriggerEff_TagProbe or do_JPsi_IsolationEff_TagProbe): # and not is_MUON1:
    JPsiTPMuonAnalysis.AddJPsiTPMuonAnalysis( doIso = do_JPsi_IsolationEff_TagProbe,
                            doTrig = do_JPsi_TriggerEff_TagProbe,
                            doTrigEtaSlices    = False,
                            doIsolEtaSlices    = False,
                            doIsolPlots      = False,
                            doTriggerPlots      = True,
                            writeNtuple=True, doClosure=ClosureTest,
                            doDRSys=False,
                            doLooseProbes=True, doMediumProbes=False,
                            doTightProbes=False, doTruthProbes=True,
                            doHighPtProbes=False,
                            doLooseTrackOnly=True,
                            doLoose=True,
                            doTight=True,
                            doGradient=True,
                            doGradientLoose=True,
                            doL1=True,
                            doHLT=True, DoProbeMatchPlots=jobproperties.MuonTPFlags.doPlots(),
                            ProduceEfficiencies=jobproperties.MuonTPFlags.doPlots() and jobproperties.MuonTPFlags.doEff(),
                            IsRunOnDAOD=IsRunOnDAOD)

if do_HighPtSingleMuons:
    HighPtSingleMuonWriter.AddHighPtSingleMuonAnalysis(writeNtuple=True,
                       writeProbeIso=True,doIDTracks=False,doMSTracks=False,doMuons=True,IsRunOnDAOD=IsRunOnDAOD)

# Add HistSvc
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["MUONTP DATAFILE='{}' OPT='RECREATE'".format(jobproperties.MuonTPFlags.outputFilename())]

# Finally, extend the space for the tool names in the log messages a bit
ServiceMgr.MessageSvc.Format = "% F%67W%S%7W%R%T %0W%M"

# I don't like spam!
from AthenaCommon.AppMgr import ToolSvc
if hasattr(ToolSvc, "JetVertexCharge_AODFix"):
    from AthenaCommon.Constants import WARNING
    ToolSvc.JetVertexCharge_AODFix.OutputLevel=WARNING
# Looking for FAs failed, I know... 
if hasattr(ToolSvc, "TrigDecisionTool"):
    if hasattr(ToolSvc.TrigDecisionTool, "Navigation"):
        from AthenaCommon.Constants import WARNING
        ToolSvc.TrigDecisionTool.Navigation.OutputLevel = WARNING

## Uncomment this for running valgrind
#     from AthenaCommon.Constants import DEBUG
#     from Valkyrie.JobOptCfg import ValgrindSvc
#     ServiceMgr += ValgrindSvc( OutputLevel = DEBUG,
#                          ProfiledAlgs = [".*TPAlg.*"],
#                          ProfiledIntervals = ["BeginRun.incident:EndRun.incident"] )
