from RecExConfig.RecFlags import rec
if rec.doTruth:
    if "TruthExamples" not in theApp.Dlls:
        theApp.Dlls += [ "TruthExamples" ]
    if "PrintHijingPars" not in theApp.TopAlg:
        theApp.TopAlg += ["PrintHijingPars"]

    HIESDItemList +=["54790518#Hijing_event_params"]
