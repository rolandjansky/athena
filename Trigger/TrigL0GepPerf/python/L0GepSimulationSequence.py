#put the GEP sequence here 
import logging as log
from AthenaCommon.Include import include

def setupL0GepSimulationSequence():

    from AthenaCommon import CfgMgr
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from TrigL0GepPerf.JetBuilder import addStandardJets, modifyClusters, modifyJets
    from TrigL0GepPerfConf import GepClusteringAlg
    from TrigL0GepPerfConf import ClusterTiming
    from TrigL0GepPerfConf import MissingETGepCl
    from TrigL0GepPerfConf import MissingETGepPufit


    # noise tool
    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool = CaloNoiseToolDefault('db', 'CaloNoiseTool')
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += theCaloNoiseTool


    # redo topoclusters at EM scale
    from CaloRec.CaloTopoClusterFlags import jobproperties
    jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib.set_Value(False)
    from TrigL0GepPerf.CaloClusterTopoGetterGep import CaloClusterTopo422Getter, CaloClusterTopo420Getter
    CaloClusterTopo422Getter()
    CaloClusterTopo420Getter()
    from xAODCaloEventCnv.xAODCaloEventCnvConf import ClusterCreator
    topSequence+=ClusterCreator("CaloCluster2xAOD")


    # choose algorithms to run
    # topoclustering and default clusters
    topoclAlgs = ['CaloCal','Calo420','Calo422']
    # pileup suppression
    puSupprAlgs = ['', 'Vor', 'SK', 'VorSK']


    for topoMaker in topoclAlgs:

        # run topoclustering algorithm
        if( topoMaker == 'CaloCal' or topoMaker == 'Calo420' or topoMaker == 'Calo422'):
            log.info('\n No topoclustering algorithm given. Using '+topoMaker+'TopoClusters')
        else:
            log.info('\n Running '+topoMaker+' topoclustering \n')
            topSequence += GepClusteringAlg( topoMaker, TopoclAlg=topoMaker, CaloNoiseTool=theCaloNoiseTool )

        # run pileup suppression algorithms on new cluster container
        modifyClusters( topoMaker, puSupprAlgs ) 

        for puSupAlg in puSupprAlgs:            

            clusterAlgs = topoMaker+puSupAlg
            inputClusterName = clusterAlgs+'TopoClusters' 
            # run AntiKt4 jet reconstruction
            modifyJets( clusterAlgs )      

            # get MET
            outputMETName = 'cluster'+clusterAlgs
            topSequence += MissingETGepCl(outputMETName,inputClusters=inputClusterName,outputMET=outputMETName) 
            # get MET with pufit
            outputMETPufitName = 'pufit'+clusterAlgs
            topSequence += MissingETGepPufit(outputMETPufitName,inputClusters=inputClusterName,outputMET=outputMETPufitName); 


    # ClusterTiming currently cannot run on custom clusters
    topSequence += ClusterTiming("DefaultClusters",inputClusterName="CaloCalTopoClusters",outputClusterName="clusters420Timing")

