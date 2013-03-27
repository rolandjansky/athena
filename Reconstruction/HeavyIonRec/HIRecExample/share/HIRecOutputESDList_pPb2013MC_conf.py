# modify ESD for 2013 p+Pb run
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
if MSMgr.StreamExists("StreamESD"):
    print 'HI modify ESD stream'
    from AthenaCommon.AppMgr import theApp
    theApp.Dlls+=[ "TruthExamples" ]
    theApp.TopAlg+=[ "PrintHijingPars" ]
    myStream=MSMgr.GetStream("StreamESD")
    myStream.AddItem("54790518#Hijing_event_params")
    myStream.AddItem("CaloClusterContainer#CaloTopoCluster")
