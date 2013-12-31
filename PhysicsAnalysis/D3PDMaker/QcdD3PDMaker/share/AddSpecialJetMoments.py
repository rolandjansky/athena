from JetMomentTools.JetMomentsConfigHelpers import *
from QcdD3PDMaker.QcdD3PDMakerFlags import QcdD3PDMakerFlags
#specialMoment('AntiKt4TopoEMJets')
#specialMoment('AntiKt6TopoEMJets')
#specialMoment('AntiKt4LCTopoJets')
#specialMoment('AntiKt6LCTopoJets')
if QcdD3PDMakerFlags.doRecJet():
  if QcdD3PDMakerFlags.doTopoEMJet():
      recommendedAreaAndJVFMoments('AntiKt4TopoEMNewJets')
      recommendedAreaAndJVFMoments('AntiKt6TopoEMNewJets')
  if QcdD3PDMakerFlags.doLCTopoJet():
      recommendedAreaAndJVFMoments('AntiKt4LCTopoNewJets')
      recommendedAreaAndJVFMoments('AntiKt5LCTopoNewJets')
      recommendedAreaAndJVFMoments('AntiKt6LCTopoNewJets')
