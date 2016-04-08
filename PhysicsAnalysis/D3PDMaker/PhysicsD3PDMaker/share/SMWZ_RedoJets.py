from JetRec.JetRecFlags import jetFlags
jetFlags.jetPerformanceJob = True

from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.Jets=[ "AntiKt4TopoEM", "AntiKt6TopoEM", "AntiKt4LCTopo" ,"AntiKt6LCTopo"]
BTaggingFlags.Active=True
BTaggingFlags.JetFitterCharm=True
BTaggingFlags.MV3_bVSu=True
BTaggingFlags.MV3_bVSc=True
BTaggingFlags.MV3_cVSu=True

from JetTagD3PDMaker.JetTagD3PDMakerFlags import JetTagD3PDFlags
JetTagD3PDFlags.Taggers+=["JetFitterCharm","MV3_bVSu","MV3_bVSc","MV3_cVSu"]
JetTagD3PDFlags.JetFitterCharmTagInfo=True


