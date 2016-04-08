# 
# @author Haifeng Li <Haifeng.Li@cern.ch>
# 

from RecExConfig.RecFlags import rec

def myjets():
    from JetRec.JetMomentGetter import make_JetMomentGetter
    from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
    jvatool = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
    make_JetMomentGetter( 'AntiKt4TopoEMJets' , [jvatool] )
    make_JetMomentGetter( 'AntiKt6TopoEMJets' , [jvatool] )
    
    
rec.UserExecs = ["myjets()"]
