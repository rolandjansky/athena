from LArEventTest.LArEventTestConf import DumpLArRawChannels
topSequence += DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels_FromDigits")

from CaloRec.CaloRecConf import CaloCellDumper
topSequence += CaloCellDumper()

from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterDumper
topSequence += ClusterDumper("TopoDumper", ContainerName="CaloCalTopoClusters", FileName="TopoCluster.txt")
topSequence += ClusterDumper("FwdTopoDumper", ContainerName="CaloCalFwdTopoTowers", FileName="FwdTopoCluster.txt")
