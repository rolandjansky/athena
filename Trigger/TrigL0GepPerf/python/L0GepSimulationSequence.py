#put the GEP sequence here 

from AthenaCommon.Include import include
def setupL0GepSimulationSequence():
    from TrigL0GepPerf.JetBuilder import addStandardJets, modifyClusters, modifyJets
    modifyClusters()
    modifyJets()

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from TrigL0GepPerfConf import ClusterTiming
    topSequence += ClusterTiming("DefaultClusters",inputClusterName="CaloCalTopoClusters",outputClusterName="clusters420Timing")
    topSequence += ClusterTiming("VorSKClusters",inputClusterName="OrigVorSKTopoClusters",outputClusterName="clustersVorSK420Timing")

    from TrigL0GepPerfConf import MissingETGepCl
    topSequence += MissingETGepCl("cluster420",inputClusters="CaloCalTopoClusters",outputMET="cluster420") 
    topSequence += MissingETGepCl("cluster420Timing",inputClusters="clusters420Timing",outputMET="cluster420Timing")
    topSequence += MissingETGepCl("clusterSK420",inputClusters="OrigSKTopoClusters",outputMET="clusterSK420")
    topSequence += MissingETGepCl("clusterVor420",inputClusters="OrigVorTopoClusters",outputMET="clusterVor420")
    topSequence += MissingETGepCl("clusterVorSK420",inputClusters="OrigVorSKTopoClusters",outputMET="clusterVorSK420")
    topSequence += MissingETGepCl("clusterVorSK420Timing",inputClusters="clustersVorSK420Timing",outputMET="clusterVorSK420Timing")
    
    from TrigL0GepPerfConf import MissingETGepPufit
    topSequence += MissingETGepPufit("pufit420",inputClusters="CaloCalTopoClusters",outputMET="pufit420"); 
    topSequence += MissingETGepPufit("pufit420Timing",inputClusters="clusters420Timing",outputMET="pufit420Timing");
