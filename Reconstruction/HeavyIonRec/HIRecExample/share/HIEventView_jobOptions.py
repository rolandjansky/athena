from HIRecExample.HIRecExampleFlags import jobproperties

if jobproperties.HIRecExampleFlags.doHIEventView:
    theApp.Dlls += ["EventViewBuilderUtils"]
    theApp.Dlls += ["EventViewBuilderAlgs"]
    theApp.Dlls += ["EventViewInserters"]
    theApp.Dlls += ["EventViewUserData"]
    theApp.TopAlg += ["EVToolLooper/heavyionEventView"]
    heavyionEVAlg = Algorithm( "heavyionEventView" )
    heavyionEVAlg.EventViewOutputName="heavyionEventView"
    heavyionEVAlg.EventViewTools=[
        "EVMissingEtUserData"  ]
    heavyionEVAlg.OutputLevel=INFO
    theApp.Dlls += ["EventViewDumpers"] 
