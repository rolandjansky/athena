#Rerun the decorations of muon clusters
from AthenaCommon import CfgMgr
muonTCLinkAlg = CfgMgr.ClusterMatching__CaloClusterMatchLinkAlg("MuonTCLinks",
                                                ClustersToDecorate="MuonClusterCollection",
                                                UseLeadCellEtaPhi=True)

topSequence += muonTCLinkAlg
