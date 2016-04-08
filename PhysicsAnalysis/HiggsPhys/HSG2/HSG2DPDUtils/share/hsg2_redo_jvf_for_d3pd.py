# code by Scott Snyder.
# this is needed to have correct jvf in d3pds
# it should be loaded as first preinlude in a reco_trf job

from RecExConfig.RecFlags import rec
rec.UserExecs = ["hsg2_redo_jvf_for_d3pd()"]
def hsg2_redo_jvf_for_d3pd():
    from JetRec.JetMomentGetter import make_JetMomentGetter

    from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
    jvatool = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
    # optional : rename the moment if you don't want to overwrite the existing one
    jvatool.JVFMomentName = "JVF2"
    make_JetMomentGetter( 'AntiKt4TopoJets' , [jvatool] )
    make_JetMomentGetter( 'AntiKt4TopoEMJets' , [jvatool] )


    import JetD3PDMaker
    from JetD3PDMaker.JetD3PDObject import JetD3PDObject
    JetD3PDObject.defineBlock(1, 'JVF2',
                              JetD3PDMaker.JetMomentFillerTool,
                              Moments=['JVF2'])


