from AthenaCommon import Logging
jetlog = Logging.logging.getLogger("GenerateTruthJets")

# Attach jet algorithms
def PrepareTruthJetInputs(algseq):
    if hasattr( algseq, "JetTruthCopyAlg" ): 
        "JetTruthCopyAlg already present, no need to add again"
        return
    from MCTruthClassifier import MCTruthClassifierConf
    from ParticleJetTools import ParticleJetToolsConf
    from JetRec import JetRecConf

    JetMCTruthClassifier = MCTruthClassifierConf.MCTruthClassifier("JetMCTruthClassifier",
                                                                    barcodeG4Shift=1000000) # or 200k? Doesn't matter anyway?
                                                       #barcodeG4Shift=firstSimCreatedBarcode())

    truthpartcopy = ParticleJetToolsConf.CopyTruthJetParticles("truthpartcopy",
                                                                OutputName="JetInputTruthParticles",
                                                                MCTruthClassifier=JetMCTruthClassifier,
                                                                BarCodeFromMetadata=0)

    truthpartcopywz = ParticleJetToolsConf.CopyTruthJetParticles("truthpartcopywz",
                                                                OutputName="JetInputTruthParticlesNoWZ",
                                                                MCTruthClassifier=JetMCTruthClassifier,
                                                                IncludePromptLeptons=False,
                                                               #IncludePromptPhotons=False,
                                                                BarCodeFromMetadata=0)

    truthpartonscopy = ParticleJetToolsConf.CopyTruthPartons("truthpartonscopy",
                                                              OutputName="TruthLabelPartons",
                                                              PtMin=5000)

    algseq += JetRecConf.JetAlgorithm("JetTruthCopyAlg", Tools=[truthpartcopy,truthpartcopywz,truthpartonscopy])

def ScheduleAntiKtTruthJets(jetradius,algseq,mods=""):
    jetcollname = 'AntiKt{0}{1}TruthJets'.format(int(jetradius*10),mods)
    if jetcollname in algseq: return
    from AthenaCommon.AppMgr import ToolSvc
    from JetRec import JetRecConf

    if hasattr(algseq, 'JetAlg'+jetcollname):
        jetlog.warning("Jet algorithm \"{0}\" already scheduled! Skipping...".format(jetcollname))
        return

    from AthenaCommon import CfgMgr
# Convert EM clusters to pseudojets
# In the midst of this migration, we can only cope with one pseudojet type
# otherwise we end up trying to update an object in SG, which is not allowed
# Needs JE's PseudojetContainer developments
# Set up the PseudoJetGetter
    pjget = None
    if mods=="":
            pjget =  CfgMgr.PseudoJetAlgorithm(
                     "truthget",
                     InputContainer = "JetInputTruthParticles",
                     Label = "Truth",
                     OutputContainer = "PseudoJetTruth",
                     SkipNegativeEnergy = True,
#                     GhostScale = 0.0,
#                     OutputLevel=VERBOSE,
  )

    elif mods=="WZ":
            pjget = CfgMgr.PseudoJetAlgorithm(
                    "truthwzget",
                    Label = "TruthWZ",
                    InputContainer = "JetInputTruthParticlesNoWZ",
                    OutputContainer = "PseudoJetTruthWZ",
#                    GhostScale = 0.0,
                    SkipNegativeEnergy = True,
#                    OutputLevel=VERBOSE,
                           )

    if not hasattr(algseq,pjget.name()):
        algseq += pjget

    # JetClusterer handles jet finding
    jclust = JetRecConf.JetClusterer(
        "jetcluster"+mods,
        JetAlgorithm = "AntiKt",
        JetRadius = jetradius,
        PtMin = 5000,
        InputPseudoJets = str(pjget.OutputContainer),
        GhostArea = 0.,
        # Numerical values ultimately from xAODJet/JetContainerInfo.h, here
        # written explicitly to avoid eiter having to load the enum from
        # the C++ dict or to have to compile JetRecConfig, which duplicates
        # the enum in python
        JetInputType = 5 if mods=="WZ" else 4,
    )

    jra = JetRecConf.JetRecAlg(
        "jetrecalg_"+jetcollname,
        Provider = jclust,
        OutputContainer = jetcollname)

    algseq += jra
 
    write_xAOD = False
    if write_xAOD:
       from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
       xaodStream = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.pool.root" )
       xaodStream.AddItem("xAOD::EventInfo#EventInfo")
       xaodStream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
       xaodStream.AddItem("xAOD::JetContainer#"+jetcollname)
       xaodStream.AddItem("xAOD::JetAuxContainer#"+jetcollname+"Aux.")
#     xaodStream.AddItem("xAOD::JetContainer#MTAntiKt4EMTopoJets")
#     xaodStream.AddItem("xAOD::JetAuxContainer#MTAntiKt4EMTopoJetsAux.")
#     xaodStream.AddItem("xAOD::CaloClusterContainer#CaloCalTopoClusters")
#     xaodStream.AddItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.")


def ScheduleAntiKtTruthWZJets(jetradius,algseq):
    ScheduleAntiKtTruthJets(jetradius,algseq,mods="WZ")
