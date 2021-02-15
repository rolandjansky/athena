from AthenaCommon import Logging
log = Logging.logging.getLogger('RunORMETReco.py')

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
usePFOLinks		= True
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
#metFlags.UsePFOLinks=usePFOLinks
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
makerAlg.OutputLevel = VERBOSE
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

ServiceMgr.MessageSvc.defaultLimit = 9999999

