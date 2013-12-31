from RecExConfig.RecFlags import rec
from JetRec.JetMomentGetter import make_JetMomentGetter
from JetMomentTools.SetupJetMomentTools import getJetVertexAssociationTool
jvatool = getJetVertexAssociationTool('AntiKt', 0.4, 'Topo')
jvatool.StoreQualityVariables = True
# jvatool.JVFMomentName = "JVF2"
make_JetMomentGetter( 'AntiKt4TopoEMJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt6TopoEMJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt4TopoJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt6TopoJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt4LCTopoJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt6LCTopoJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt4TowerJets' , [jvatool] ) 
make_JetMomentGetter( 'AntiKt6TowerJets' , [jvatool] ) 

from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
if QcdD3PDMakerFlags.doRecJet():
    make_JetMomentGetter( 'AntiKt4TopoEMNewJets' , [jvatool] ) 
    make_JetMomentGetter( 'AntiKt6TopoEMNewJets' , [jvatool] ) 
    make_JetMomentGetter( 'AntiKt4TopoNewJets' , [jvatool] ) 
    make_JetMomentGetter( 'AntiKt6TopoNewJets' , [jvatool] ) 
    make_JetMomentGetter( 'AntiKt4LCTopoNewJets' , [jvatool] ) 
    make_JetMomentGetter( 'AntiKt6LCTopoNewJets' , [jvatool] ) 
    if not rec.readAOD():
        make_JetMomentGetter( 'AntiKt4TowerEMNewJets' , [jvatool] ) 
        make_JetMomentGetter( 'AntiKt6TowerEMNewJets' , [jvatool] ) 
        make_JetMomentGetter( 'AntiKt4TowerNewJets' , [jvatool] ) 
        make_JetMomentGetter( 'AntiKt6TowerNewJets' , [jvatool] ) 
