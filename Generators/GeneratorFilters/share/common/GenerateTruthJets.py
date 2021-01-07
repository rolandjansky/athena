from AthenaCommon import Logging
jetlog = Logging.logging.getLogger("GenerateTruthJets")

# Attach jet algorithms
def PrepareTruthJetInputs(algseq):
    if hasattr( algseq, "JetTruthCopyAlg" ): 
        "JetTruthCopyAlg already present, no need to add again"
        return
    from AthenaCommon.AppMgr import ToolSvc
    from MCTruthClassifier import MCTruthClassifierConf
    from ParticleJetTools import ParticleJetToolsConf
    from JetRec import JetRecConf

    ToolSvc += MCTruthClassifierConf.MCTruthClassifier("JetMCTruthClassifier",
                                                       barcodeG4Shift=1000000) # or 200k? Doesn't matter anyway?
                                                       #barcodeG4Shift=firstSimCreatedBarcode())

    ToolSvc += ParticleJetToolsConf.CopyTruthJetParticles("truthpartcopy",
                                                          OutputName="JetInputTruthParticles",
                                                          MCTruthClassifier=ToolSvc.JetMCTruthClassifier,
                                                          BarCodeFromMetadata=0)

    ToolSvc += ParticleJetToolsConf.CopyTruthJetParticles("truthpartcopywz",
                                                          OutputName="JetInputTruthParticlesNoWZ",
                                                          MCTruthClassifier=ToolSvc.JetMCTruthClassifier,
                                                          IncludePromptLeptons=False,
                                                          #IncludePromptPhotons=False,
                                                          BarCodeFromMetadata=0)

    ToolSvc += ParticleJetToolsConf.CopyTruthPartons("truthpartonscopy",
                                                     OutputName="TruthLabelPartons",
                                                     PtMin=5000)

    algseq += JetRecConf.JetAlgorithm("JetTruthCopyAlg", Tools=[ToolSvc.truthpartcopy,ToolSvc.truthpartcopywz,ToolSvc.truthpartonscopy])

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
                     InputContainer = ToolSvc.truthpartcopy.OutputName,
                     Label = "Truth",
                     OutputContainer = "PseudoJetTruth",
                     SkipNegativeEnergy = True,
#                     GhostScale = 0.0,
                     OutputLevel=VERBOSE,
  )

    elif mods=="WZ":
            pjget = CfgMgr.PseudoJetAlgorithm(
                    "truthwzget",
                    Label = "TruthWZ",
                    InputContainer = ToolSvc.truthpartcopywz.OutputName,
                    OutputContainer = "PseudoJetTruthWZ",
#                    GhostScale = 0.0,
                    SkipNegativeEnergy = True,
                    OutputLevel=VERBOSE,
                           )

 


    algseq += pjget

# Set up the jet finder
    JetFinder_AntiKt4 = CfgMgr.JetFinder(jetcollname+"Finder",
                                          JetAlgorithm = "AntiKt",
                                          JetRadius = jetradius,
                                          GhostArea = 0.01,
                                          PtMin = 5000,
                                          OutputLevel=VERBOSE,
                                          )


#Then we setup a jet builder to calculate the areas needed for the rho subtraction
# Actually, we don't really need the areas but we may as well use this one
    JetBuilder_AntiKt4 = CfgMgr.JetFromPseudojet("jblda", Attributes = ["ActiveArea", "ActiveArea4vec"],
                                              OutputLevel=VERBOSE)
    JetFinder_AntiKt4.JetBuilder = JetBuilder_AntiKt4
 
#Now we setup a JetRecTool which will use the above JetFinder
    JetRecTool = CfgMgr.JetRecTool(jetcollname+"Rec",
                                OutputLevel=VERBOSE)
    JetRecTool.JetFinder = JetFinder_AntiKt4
    JetRecTool.InputPseudoJets = [pjget.OutputContainer]
    JetRecTool.OutputContainer = jetcollname
 
    algseq += CfgMgr.JetAlgorithm(jetcollname+"Alg",Tools = [JetRecTool])
    

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
