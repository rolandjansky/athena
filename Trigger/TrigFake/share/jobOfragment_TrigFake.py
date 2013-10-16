theApp.Dlls += [ "TrigFake" ]
if doTruth:
    theApp.TopAlg += [ "ZVertexFromKine"  ]

    if DetDescrVersion[0:2]=="DC2":
        Algorithm("ZVertexFromKine").McEventCollectionName = "G4Truth"
    else:
        Algorithm("ZVertexFromKine").McEventCollectionName = "TruthEvent"

