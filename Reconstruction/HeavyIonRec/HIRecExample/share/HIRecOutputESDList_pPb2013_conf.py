# modify ESD for 2013 p+Pb run
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
if MSMgr.StreamExists("StreamESD"):
    myStream=MSMgr.GetStream("StreamESD")
    myStream.AddItem("CaloClusterContainer#CaloTopoCluster")
    print 'CaloTopoClusterContainer added to ESD'
