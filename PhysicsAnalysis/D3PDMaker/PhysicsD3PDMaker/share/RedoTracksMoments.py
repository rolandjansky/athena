def tmp_getJetVertexAssociationTool(finder, mainParam, input, toolName="JetVertexAssociation", **options):
    from JetRec.JetAlgConfiguration import checkAndUpdateOptions
    options=checkAndUpdateOptions(finder=finder, mainParam=mainParam, input=input, **options)
    
    from JetMomentTools.JetMomentToolsConf import JetVertexAssociationTool, JetOriginCorrectionTool

    jvaTool = JetVertexAssociationTool(toolName)

    jvaTool.AssociateToHighestJVF = True

    from JetRec.TrackSelectionForJets import getDefaultJetVtxTrackHelper
    jvaTool.JetVtxTrackHelper = getDefaultJetVtxTrackHelper()

    # origin correction tool
    origCorr = JetOriginCorrectionTool("JetOriginCorrection")
    origCorr.CellCalibrator = None
    origCorr.UseGCW = False
    origCorr.UseClusters = True
    
    # Official Choice of Jet/MET Group : Primary Vertex
    origCorr.UseJVA           = False
    origCorr.UsePrimaryVertex = True
    origCorr.UseBeamSpot      = True
    jvaTool.OriginCorrectionTool = origCorr
    
    return jvaTool


# simply use this as :     momAlg = specialMoment('AntiKt4LCTopoJets')
def specialMoment(jetcollname):
    
    gAssoc = ["TrackAssoc"]
    if rec.doTruth():
        gAssoc+=[ "TruthAssoc"]

    from JetMomentTools.GhostAssociation import addGhostAssociation

    momAlg = addGhostAssociation(jetcollname, gAssoc)

    # make JVF calculation use the Ghost-associated tracks
    #    from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool    
    # jvtxTool = getJetVertexAssociationTool('AntiKt', 0.4, 'LCTopo')    # parameters don't matter
    # use the tmp_ version as long as the default is not fixed
    #    jvtxTool = tmp_getJetVertexAssociationTool('AntiKt', 0.4, 'LCTopo', toolName=jetcollname+'JVAtool')    # parameters don't matter

    #    jvtxTool.TrackAssocName = "TrackAssoc"
    #    jvtxTool.JetVtxTrackHelper.ptMin = 0.5*GeV
    #    momAlg.CalculatorTools += [ jvtxTool ]   

    # enable trackWIDDTH
    from JetMomentTools.SetupJetMomentTools import getJetTracksMomentTool
    tm = getJetTracksMomentTool( DoAllPV = True)
    tm.DoAllPV = True
    tm.MinPt = 1 *GeV
    momAlg.CalculatorTools += [ tm ]

    
