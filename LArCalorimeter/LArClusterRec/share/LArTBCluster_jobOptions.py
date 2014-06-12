
#theApp.Dlls += [  "LArClusterRec"  ]
#theApp.TopAlg += [
#"CaloClusterMaker/LArTBClusterMaker"
#]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
# cluster maker a la emtb
# 
#LArTBClusterMaker = Algorithm( "LArTBClusterMaker" )
from CaloRec.CaloRecConf import CaloClusterMaker
LArTBClusterMaker = CaloClusterMaker("LArTBClusterMaker")
LArTBClusterMaker.ClustersOutputName="LArClusterTBEM"

from LArClusterRec.LArClusterRecConf import LArTBClusterBuilder
TB33 = LArTBClusterBuilder("TB33")
TB33.EtaSize=3.
TB33.PhiSize=3.
TB33.CellContainers="AllCalo"
TB33.EnergyThreshold=-1000000.*MeV
TB33.ClusterNbr=1
TB33.LArCaloRegion="BARREL"

ToolSvc += TB33
LArTBClusterMaker.ClusterMakerTools=[ ToolSvc.TB33.getFullName() ]
    
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += LArTBClusterMaker
