#====================================================================
# JETM8.py 
# reductionConf flag JETM8 in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *

#
if DerivationFrameworkIsMonteCarlo:
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

#====================================================================
# SKIMMING TOOL 
#====================================================================
# this recopying the latest JETM6

from DerivationFrameworkJetEtMiss.TriggerLists import *
electronTriggers = singleElTriggers
muonTriggers = singleMuTriggers

# For first data
jetSelection = '(count( AntiKt10LCTopoJets.pt > 100.*GeV ) >=1)'
#jetSelection = '(count( CamKt12LCTopoJets.pt > 150.*GeV ) >=1)'

orstr  = ' || '
andstr = ' && '
eltrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(electronTriggers)
elofflinesel = andstr.join(['count((Electrons.pt > 20*GeV) && (Electrons.DFCommonElectronsLHMedium)) == 1',
                            'count(AntiKt10LCTopoJets.pt > 75*GeV) >=1'])
electronSelection = '( (' + eltrigsel + ') && (' + elofflinesel + ') )'

mutrigsel = '(EventInfo.eventTypeBitmask==1) || '+orstr.join(muonTriggers)
muofflinesel = andstr.join(['count((Muons.pt > 20*GeV) && (Muons.DFCommonMuonsPreselection)) == 1',
                            'count(AntiKt10LCTopoJets.pt > 75*GeV) >=1'])
muonSelection = ' ( (' + mutrigsel + ') && (' + muofflinesel + ') ) '
# MET filter wanted? : MET_Reference_AntiKt4LCTopo > 20*GeV # should use a different container

lepSelection = '( ' + electronSelection + ' || ' + muonSelection + ' )'


expression = jetSelection + ' || '+ lepSelection


from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
JETM8TrigSkimmingTool = DerivationFramework__TriggerSkimmingTool(   name           = "JETM8TrigSkimmingTool",
                                                                    TriggerListOR  = jetTriggers+electronTriggers+muonTriggers )
                                                                    #TriggerListOR  = triggers )
ToolSvc += JETM8TrigSkimmingTool

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
JETM8OfflineSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name = "JETM8OfflineSkimmingTool",
                                                                        expression = expression)
ToolSvc += JETM8OfflineSkimmingTool

#====================================================================
# THINNING TOOLS 
#====================================================================
thinningTools = []

## from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__ViewContainerThinning
## JETM8TrackJetInputThin = DerivationFramework__ViewContainerThinning( name = "JETM8ViewContTrackThinning",
##                                                                      ThinningService        = "JETM8ThinningSvc",
##                                                                      SourceContainer = "InDetTrackParticles",
##                                                                      ViewContainer = "JetSelectedTracks_LooseTrackJets",
##                                                                      ParticleType = 4, # trackParticles
##                                                                      ApplyAnd = False)

## ToolSvc += JETM8TrackJetInputThin
## thinningTools.append(JETM8TrackJetInputThin)    


# Truth particle thinning
doTruthThinning = True
preserveAllDescendants = False
from AthenaCommon.GlobalFlags import globalflags
if doTruthThinning and DerivationFrameworkIsMonteCarlo:
    truth_cond_WZH    = "((abs(TruthParticles.pdgId) >= 23) && (abs(TruthParticles.pdgId) <= 25))"            # W, Z and Higgs
    truth_cond_Lepton = "((abs(TruthParticles.pdgId) >= 11) && (abs(TruthParticles.pdgId) <= 16) && (TruthParticles.barcode < 200000))"            # Leptons
    truth_cond_Quark  = "((abs(TruthParticles.pdgId) <=  5  && (TruthParticles.pt > 10000.)) || (abs(TruthParticles.pdgId) == 6))"                 # Quarks
    truth_cond_Gluon  = "((abs(TruthParticles.pdgId) == 21) && (TruthParticles.pt > 10000.))"                                                # Gluons
    truth_cond_Photon = "((abs(TruthParticles.pdgId) == 22) && (TruthParticles.pt > 10000.) && (TruthParticles.barcode < 200000))"                 # Photon
    
    truth_expression = '('+truth_cond_WZH+' || '+truth_cond_Lepton +' || '+truth_cond_Quark+' || '+truth_cond_Gluon+' || '+truth_cond_Photon+')'
    
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    JETM8TruthThinningTool = DerivationFramework__GenericTruthThinning( name = "JETM8TruthThinningTool",
                                                                        ThinningService        = "JETM8ThinningSvc",
                                                                        ParticleSelectionString = truth_expression,
                                                                        PreserveDescendants     = preserveAllDescendants,
                                                                        PreserveGeneratorDescendants = not preserveAllDescendants,
                                                                        PreserveAncestors = True)
    
    ToolSvc += JETM8TruthThinningTool
    thinningTools.append(JETM8TruthThinningTool)    


    from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__ViewContainerThinning
    JETM8TruthJetInputThin = DerivationFramework__ViewContainerThinning( name = "JETM8ViewContThinning",
                                                                         ThinningService        = "JETM8ThinningSvc",
                                                                         SourceContainer = "TruthParticles",
                                                                         ViewContainer = "JetInputTruthParticles",
                                                                         ParticleType = 201, # truthParticles
                                                                         ApplyAnd = False)

    ToolSvc += JETM8TruthJetInputThin
    thinningTools.append(JETM8TruthJetInputThin)    

#=======================================
# CREATE PRIVATE SEQUENCE
#=======================================

jetm8Seq = CfgMgr.AthSequencer("JETM8Sequence")
DerivationFrameworkJob += jetm8Seq
#jetm8Seq = DerivationFrameworkJob

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel(	name = "JETM8TrigKernel", 
									SkimmingTools = [JETM8TrigSkimmingTool],
									ThinningTools = [])

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
reducedJetList = ["AntiKt2PV0TrackJets",
                  "AntiKt4PV0TrackJets",
                  "AntiKt4TruthJets",
                  "AntiKt10TruthJets",
                  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,jetm8Seq,"JETM8")

jetm8Seq += CfgMgr.DerivationFramework__DerivationKernel( name = "JETM8MainKernel", 
                                                          SkimmingTools = [JETM8OfflineSkimmingTool],
                                                          ThinningTools = thinningTools)

#====================================================================
# Special jets
#====================================================================

OutputJets["JETM8"] = []

from JetRecTools.JetRecToolsConfig import ctm
from JetRecTools.JetRecToolsConf import CaloClusterConstituentsOrigin, ConstituentSubtractorTool, VoronoiWeightTool

ctm.add( VoronoiWeightTool("JetConstit_Voronoi",
         doSpread=True, nSigma=0),
         alias = 'voronoi' )

ctm.add( VoronoiWeightTool("JetConstit_VoronoiSupp",
         doSpread=False, nSigma=0),
         alias = 'voronoiSupp' )

ctm.add( ConstituentSubtractorTool("JetConstit_ConstSub"),
         alias = 'constsub' )

clustOrigSeq = ctm.buildConstitModifSequence( 'ConstitOrigSeq',
                                              OutputContainer = 'OrigTopoClusters',
                                              InputContainer= 'CaloCalTopoClusters',
                                              modList = [  'lc_origin'] )

clustSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigSKSeq',
                                            OutputContainer = 'OrigSKTopoClusters',
                                            InputContainer= 'OrigTopoClusters',
                                            modList = [  'softkiller'] , InputType="CaloCluster")

clustVorSeq = ctm.buildConstitModifSequence( 'ConstitOrigVorSeq',
                                            OutputContainer = 'OrigVorTopoClusters',
                                            InputContainer= 'OrigTopoClusters',
                                            modList = [  'voronoi'] , InputType="CaloCluster")

clustVorSuppSeq = ctm.buildConstitModifSequence( 'ConstitOrigVorSuppSeq',
                                                 OutputContainer = 'OrigVorSuppTopoClusters',
                                                 InputContainer= 'OrigTopoClusters',
                                                 modList = [  'voronoiSupp'] , InputType="CaloCluster")

clustVorSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigVorSKSeq',
                                            OutputContainer = 'OrigVorSuppSKTopoClusters',
                                            InputContainer= 'OrigVorSuppTopoClusters',
                                            modList = [  'softkiller'] , InputType="CaloCluster")

clustCSSeq = ctm.buildConstitModifSequence( 'ConstitOrigCSSeq',
                                            OutputContainer = 'OrigCSTopoClusters',
                                            InputContainer= 'OrigTopoClusters',
                                            modList = [  'constsub'] , InputType="CaloCluster")

clustCSSKSeq = ctm.buildConstitModifSequence( 'ConstitOrigCSSKSeq',
                                            OutputContainer = 'OrigCSSKTopoClusters',
                                            InputContainer= 'OrigCSTopoClusters',
                                            modList = [  'softkiller'] , InputType="CaloCluster")

correctedClusters = [ "OrigTopoClusters", "OrigSKTopoClusters", "OrigCSTopoClusters", "OrigCSSKTopoClusters", "OrigVorSuppSKTopoClusters", "OrigVorTopoClusters", "OrigVorSuppTopoClusters" ]

from JetRec.JetRecConf import JetAlgorithm
clustSeqAlg = JetAlgorithm("ClusterModifiers", Tools = [clustOrigSeq, clustSKSeq, clustCSSeq, clustCSSKSeq, clustVorSeq, clustVorSuppSeq, clustVorSKSeq])
jetm8Seq += clustSeqAlg

if jetFlags.useTruth:
    addStandardJets("CamKt", 1.5, "Truth", ptmin=40000, algseq=jetm8Seq, outputGroup="JETM8")

# CamKt15LCTopo
addStandardJets("CamKt", 1.5, "LCTopo", mods="lctopo_ungroomed", calibOpt="none", ghostArea=0.01, ptmin=2000, ptminFilter=50000,
                algseq=jetm8Seq, outputGroup="JETM8")

# AntiKt10Track
addTrimmedJets("AntiKt", 1.0, "PV0Track", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")

# PFlow fat jets
#addTrimmedJets("AntiKt", 1.0, "EMCPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")
addTrimmedJets("AntiKt", 1.0, "EMPFlow", rclus=0.2, ptfrac=0.05, algseq=jetm8Seq, outputGroup="JETM8")

# AntiKt10*PtFrac5Rclus20
addDefaultTrimmedJets(jetm8Seq,"JETM8")

#AntiKt4PV0TrackJets
addAntiKt2PV0TrackJets(jetm8Seq, "JETM8")
addAntiKt4PV0TrackJets(jetm8Seq, "JETM8")

if DerivationFrameworkIsMonteCarlo:
     addAntiKt4TruthJets(jetm8Seq, "JETM8")
     addAntiKt10TruthJets(jetm8Seq, "JETM8")

#=======================================
# SCHEDULE REPLACEMENT B-TAG COLLECTIONS
#=======================================

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt2PV0TrackJets'], Sequencer = jetm8Seq)

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_JETM8Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_JETM8Stream )
JETM8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
JETM8Stream.AcceptAlgs(["JETM8MainKernel"])
# for thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="JETM8ThinningSvc", outStreams=[evtStream] )

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
JETM8SlimmingHelper = SlimmingHelper("JETM8SlimmingHelper")
JETM8SlimmingHelper.SmartCollections = ["Electrons", "Photons", "Muons", "TauJets",
                                        "InDetTrackParticles", "PrimaryVertices",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "MET_Reference_AntiKt4LCTopo",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        "AntiKt4EMTopoJets","AntiKt4LCTopoJets","AntiKt4EMPFlowJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "BTagging_AntiKt4EMTopo", "BTagging_AntiKt2Track",
                                        ] #+ correctedClusters
JETM8SlimmingHelper.AllVariables = ["CaloCalTopoClusters", 
                                    "MuonTruthParticles", "egammaTruthParticles",
                                    "TruthParticles", "TruthEvents", "TruthVertices",
                                    "JetETMissChargedParticleFlowObjects", "JetETMissNeutralParticleFlowObjects",
                                    "Kt4EMTopoOriginEventShape","Kt4LCTopoOriginEventShape","Kt4EMPFlowEventShape"]

JETM8SlimmingHelper.AppendToDictionary.update({"LCOriginTopoClusters":"xAOD::CaloClusterContainer",
                                               "LCOriginTopoClustersAux":"xAOD::ShallowAuxContainer"})
JETM8SlimmingHelper.ExtraVariables =['LCOriginTopoClusters.calE.calEta.calM.calPhi']

#JETM8SlimmingHelper.ExtraVariables = []

for truthc in [
    "TruthMuons",
    "TruthElectrons",
    "TruthPhotons",
    "TruthTaus",
#    "TruthNeutrinos"
    ]:
    JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleContainer#"+truthc)
    JETM8SlimmingHelper.StaticContent.append("xAOD::TruthParticleAuxContainer#"+truthc+"Aux.")

for caloc in correctedClusters:
    JETM8SlimmingHelper.AppendToDictionary.update({caloc:"xAOD::CaloClusterContainer",
                                                   caloc+"Aux":"xAOD::ShallowAuxContainer"})
    JETM8SlimmingHelper.ExtraVariables +=[
        caloc+'.calE.calEta.calM.calPhi']

print JETM8SlimmingHelper.AppendToDictionary

# Trigger content
from DerivationFrameworkCore.JetTriggerContent import JetTriggerContent
for trig in JetTriggerContent:
    if 'HLT' in trig and not 'Aux' in trig:
        JETM8SlimmingHelper.AllVariables.append(trig)
JETM8SlimmingHelper.IncludeJetTriggerContent = True

# Add the jet containers to the stream
addJetOutputs(JETM8SlimmingHelper,["SmallR",
                                   "LargeR",
                                   "JETM8","AntiKt10EMPFlowJets"])
# Add the MET containers to the stream
addMETOutputs(JETM8SlimmingHelper,["Diagnostic","AntiKt4LCTopo","AntiKt4EMPFlow","Track"])

JETM8SlimmingHelper.AppendContentToStream(JETM8Stream)
JETM8Stream.RemoveItem("xAOD::TrigNavigation#*")
JETM8Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")

def removeVars(coll, vars):
    """Terrible hack to force removing a limited set of variables.
    Intended to remove definingParametersCovMatrix if possible.. """
    items = JETM8Stream.GetItems()
    origS = None
    for i in items:
        if coll in i and '.' in i:
            origS = i
            break
    if origS is None:
        return
    existV = origS.split('.')
    cleanedV = []
    for v in existV:
        if v in vars:
            continue
        cleanedV.append(v)
    newS = '.'.join(cleanedV)
    JETM8Stream.RemoveItem( origS ) 
    JETM8Stream.AddItem( newS ) 
removeVars('InDetTrackParticles', ['definingParametersCovMatrix',])
