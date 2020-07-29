from AthenaCommon import Logging
jetlog = Logging.logging.getLogger("GenerateTruthJets")

# Attach jet algorithms
def PrepareTruthJetInputs(algseq):
    print "will prepare thruth jets"
#    if "JetTruthCopyAlg" in algseq:
    if hasattr( algseq, "JetTruthCopyAlg" ): 
        "JetTruthCopyAlg already present, no need to add again"
        return
    from AthenaCommon.AppMgr import ToolSvc
    from MCTruthClassifier import MCTruthClassifierConf
    from ParticleJetTools import ParticleJetToolsConf
    from JetRec import JetRecConf
    #from MCTruthClassifier.MCTruthClassifierConfig import firstSimCreatedBarcode

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

    # Set up the PseudoJetGetter
    pjget = None
    if mods=="":
        if "truthget" in ToolSvc:
            pjget = ToolSvc.truthget
        else:
            pjget = JetRecConf.PseudoJetGetter("truthget",
                                               Label = "Truth",
                                               InputContainer = ToolSvc.truthpartcopy.OutputName,
                                               OutputContainer = "PseudoJetTruth",
                                               GhostScale = 0.0,
                                               SkipNegativeEnergy = True
                                               )
    elif mods=="WZ":
        if "truthwzget" in ToolSvc:
            pjget = ToolSvc.truthwzget
        else:
            pjget = JetRecConf.PseudoJetGetter("truthwzget",
                                               Label = "TruthWZ",
                                               InputContainer = ToolSvc.truthpartcopywz.OutputName,
                                               OutputContainer = "PseudoJetTruthWZ",
                                               GhostScale = 0.0,
                                               SkipNegativeEnergy = True,
                                               )        
    if pjget:
        ToolSvc += pjget
    else:
        jetlog.error("Unsupported jet collection \"{0}\" requested!".format(jetcollname))

    # Set up the jet builder (no area moments)
    if not "jbld" in ToolSvc:
        ToolSvc += JetRecConf.JetFromPseudojet("jbld")

    # Set up the jet finder
    finder = JetRecConf.JetFinder(jetcollname+"Finder",
                                  JetAlgorithm = "AntiKt",
                                  JetRadius = jetradius,
                                  JetBuilder = ToolSvc.jbld,
                                  GhostArea = 0.01,
                                  PtMin = 5000
                                  )
    ToolSvc += finder

    if "truthpartonget" in ToolSvc:
        truthpartonget = ToolSvc.truthpartonget
    else:
        truthpartonget = JetRecConf.PseudoJetGetter("truthpartonget",
                                                    Label = "GhostPartons",
                                                    InputContainer = ToolSvc.truthpartonscopy.OutputName,
                                                    OutputContainer = "PseudoJetGhostPartons",
                                                    GhostScale = 1e-40,
                                                    SkipNegativeEnergy = True,
                                                    )
        ToolSvc += truthpartonget


    from ParticleJetTools import ParticleJetToolsConf
    partontruthlabel = ParticleJetToolsConf.Analysis__JetPartonTruthLabel("partontruthlabel")
    ToolSvc += partontruthlabel

    #Now we setup a JetRecTool which will use the above JetFinder
    jetrectool = JetRecConf.JetRecTool(jetcollname+"Rec",
                                       JetFinder = finder,
                                       PseudoJetGetters = [pjget,truthpartonget],
                                       OutputContainer = jetcollname,
                                       JetModifiers = [partontruthlabel]
                                       )
    ToolSvc += jetrectool

    algseq += JetRecConf.JetAlgorithm(jetcollname+"Alg", Tools=[jetrectool])

def ScheduleAntiKtTruthWZJets(jetradius,algseq):
    ScheduleAntiKtTruthJets(jetradius,algseq,mods="WZ")
