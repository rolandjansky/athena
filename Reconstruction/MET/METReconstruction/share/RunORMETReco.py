from AthenaCommon import Logging
log = Logging.logging.getLogger('run_fromAOD.py')

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=[""]

theApp.EvtMax = -1

import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

electronPT		= 10000
muonPT 			= 10000
photonPT 		= 10000
electronWP 		= "Medium"
muonWP 			= 1
photonWP 		= "Tight"
electronETA 		= 2.47
muonETA 		= 2.7
photonETA 		= 2.47
soft			= "Clus"
metWP			= "PFlow"

usePFOElectronLinks	= True
usePFOPhotonLinks	= True
retainMuon		= True

############################################################################
# Set up detector description 
from AthenaCommon.GlobalFlags import globalflags
from IOVDbSvc.CondDB import conddb
if len(globalflags.ConditionsTag())!=0:
    conddb.setGlobalTag(globalflags.ConditionsTag())
include("RecExCond/AllDet_detDescr.py")

############################################################################
# MET reco and extract OR PFOs 

include("eflowRec/jetAlgs.py")  # std jet reco (AntiKt4EMPFlowJets)

from METReconstruction.METRecoFlags import jobproperties, metFlags
metFlags.UsePFOElectronLinks=usePFOElectronLinks
metFlags.UsePFOPhotonLinks=usePFOPhotonLinks
metFlags.DoORMet = False  # used by ORMETAssoc.py

include("METUtilities/ORMETAssoc.py")

# doRetrieveORconstit 	= True to extract OR PFOs collection
# retainMuonConstit 	= True to include constituents associated to overlapping muons in OR PFOs collection
#			  (needed to recover standard muon-jet overlap removal)
# doORMet		= True for OR MET reco

from METUtilities.ORMETMakerConfig import getMETMakerAlg
makerAlg = getMETMakerAlg("AntiKt4EMPFlow", jetSelection=metWP, doRetrieveORconstit = True,retainMuonConstit = retainMuon, doORMet=False, muonID=muonWP, electronID=electronWP, photonID=photonWP)
makerAlg.ElectronPT = electronPT;
makerAlg.MuonPT = muonPT; 
makerAlg.PhotonPT = photonPT
makerAlg.ElectronETA = electronETA;
makerAlg.MuonETA = muonETA;
makerAlg.PhotonETA = photonETA;
makerAlg.Soft = soft
makerAlg.OutputLevel = DEBUG 
topSequence += makerAlg
metFlags.METOutputList().append("AntiKt4EMPFlow")

############################################################################
# OR MET reco 

include("METUtilities/ORjetReco.py")  # OR jets reco (AntiKt4OverlapRemovedEMPFlowJets)

from METReconstruction.METRecoFlags import jobproperties, metFlags
metFlags.UsePFOElectronLinks=usePFOElectronLinks
metFlags.UsePFOPhotonLinks=usePFOPhotonLinks
metFlags.DoORMet = True #used by ORMETAssoc.py

include("METUtilities/ORMETAssoc.py")

# doRetrieveORconstit 	= True to extract OR PFOs collection
# retainMuonConstit 	= True to include constituents associated to overlapping muons in OR PFOs collection
#			  (needed to recover standard muon-jet overlap removal)
# doORMet		= True for OR MET reco

from METUtilities.ORMETMakerConfig import getMETMakerAlg
makerAlg = getMETMakerAlg("AntiKt4OverlapRemovedEMPFlow", jetSelection=metWP, doRetrieveORconstit = False, retainMuonConstit = retainMuon, doORMet=True, muonID=muonWP, electronID=electronWP, photonID=photonWP)
makerAlg.ElectronPT = electronPT;
makerAlg.MuonPT = muonPT; 
makerAlg.PhotonPT = photonPT
makerAlg.ElectronETA = electronETA;
makerAlg.MuonETA = muonETA;
makerAlg.PhotonETA = photonETA;
makerAlg.Soft = soft
makerAlg.OutputLevel = DEBUG
topSequence += makerAlg
metFlags.METOutputList().append("AntiKt4OverlapRemovedEMPFlow")

log.warning('AssocOutputLists: %s', metFlags.METAssocOutputList())
log.warning('OutputLists: %s', metFlags.METOutputList())
include("METReconstruction/METReconstructionOutputAODList_jobOptions.py")

############################################################################

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewStream( "StreamAOD" )
xaodStream.AddItem("xAOD::JetContainer#AntiKt4OverlapRemovedEMPFlowJets")
xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4OverlapRemovedEMPFlowJetsAux.")
xaodStream.AddItem("xAOD::JetContainer#AntiKt4EMPFlowJets")
xaodStream.AddItem("xAOD::JetAuxContainer#AntiKt4EMPFlowJetsAux.")
for met in MissingETAODList: xaodStream.AddItem(met)



out_dir = "/storage_tmp/atlas/fpiazza/QT/run_analysis/test_nullEleTerm/v15/"
message = ""
sample = "Wenu"



#print sample 
out_file = 'met.root' #out_dir+'%s_%s_test.root' %(svcMgr.EventSelector.InputCollections[0].split("/")[9],soft)




print("outfile", out_file)

from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier

BkgElectronMCTruthClassifier = MCTruthClassifier(name = "BkgElectronMCTruthClassifier",
                                                 ParticleCaloExtensionTool = "")




alg = CfgMgr.Analysis(MCTruthClassifier = BkgElectronMCTruthClassifier, RetainMuon=True)



#Configuration
# alg.SelectElectrons = False;
alg.ElectronID = "LHMedium";
alg.MuonID = "LHMedium";
alg.PhotonID = "Tight";
alg.ElectronPT = 10000
alg.MuonPT = 10000; #!!! 2500
alg.PhotonPT = 10000
alg.ElectronETA = 2.47;
alg.MuonETA = 2.7;
alg.PhotonETA = 2.47;
alg.SoftTerm = soft
#alg.InputJets = "AntiKt4EMPflowJets" #unass
# alg.PVtxMatchingCPFOs = True;


alg.OutfileName = out_file;
# alg.OutfileName = "/data/jmacdonald/QT/eflowRecBugFix/histograms/Zee/" + ID + "/METHistograms_useLinks" + useMETLinks + ".root";

topSequence += alg



#tool=CfgMgr.met__METMaker("METMaker", OutputLevel=VERBOSE,JetSelection="Expert", CustomJetJvtCut=1.0,CustomJetJvtPtMax=1000000000)

from METUtilities.METUtilitiesConf import met__METMaker
METmakerTool=met__METMaker(OutputLevel=VERBOSE, DoPFlow=True, JetSelection="PFlow")

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += METmakerTool

muonSel = CfgMgr.CP__MuonSelectionTool("MuonSelectionTool",
                                           MuQuality=1, # Medium
                                           MaxEta=2.4)
ToolSvc += muonSel

elecSelLH = CfgMgr.AsgElectronLikelihoodTool("ElectronLHSelectionTool",
                                         WorkingPoint="MediumLHElectron")
ToolSvc += elecSelLH

photonSelIsEM = CfgMgr.AsgPhotonIsEMSelector("PhotonIsEMSelectionTool",
                                         WorkingPoint="TightPhoton")
ToolSvc += photonSelIsEM

tauSel = CfgMgr.TauAnalysisTools__TauSelectionTool("TauSelectionTool")
ToolSvc += tauSel


#METmakerTool.DoPFlow=False
#print METmakerTool.DoPFlow
alg.METMaker=METmakerTool
alg.MuonSelectionTool=muonSel
alg.ElectronLHSelectionTool=elecSelLH
alg.PhotonIsEMSelectionTool=photonSelIsEM
alg.TauSelectionTool=tauSel


ServiceMgr.MessageSvc.defaultLimit = 9999999

