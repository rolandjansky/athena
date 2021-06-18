#put the GEP sequence here 
import logging as log
from AthenaCommon.Include import include

def setupL0GepSimulationSequence():

    from AthenaCommon import CfgMgr
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    from TrigL0GepPerf.JetBuilder import addStandardJets, modifyClusters, modifyJets
    from TrigL0GepPerfConf import GepClusteringAlg
    from TrigL0GepPerfConf import GepJetAlg
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
    # jet reconstruction
    jetAlgs = ['AntiKt4']

    for topoMaker in topoclAlgs:

        # topoclusters
        if( topoMaker == 'CaloCal' or topoMaker == 'Calo420' or topoMaker == 'Calo422'):
            # use existing topoclusters
            log.info('\n Using '+topoMaker+'TopoClusters')
        else:
            # run custom topoclustering 
            log.info('\n Running '+topoMaker+' topoclustering \n')
            topSequence += GepClusteringAlg( topoMaker, TopoclAlg=topoMaker, CaloNoiseTool=theCaloNoiseTool )

        # run pileup suppression algorithms on new cluster container
        modifyClusters( topoMaker, puSupprAlgs ) 

        for puSupAlg in puSupprAlgs:            

            topoclLabel = topoMaker+puSupAlg

            # jets
            for jetAlg in jetAlgs:
                if jetAlg=='AntiKt4':
                    # run Athena AntiKt4 jet reconstruction
                    modifyJets( topoclLabel )      
                else:
                    # run custom jet algorithm
                    topSequence += GepJetAlg( jetAlg+topoclLabel, TopoclLabel=topoclLabel, JetAlg=jetAlg)

            # get MET
            inputTopoclName = topoclLabel+'TopoClusters' 
            outputMETName = 'cluster'+topoclLabel
            topSequence += MissingETGepCl(outputMETName,inputClusters=inputTopoclName,outputMET=outputMETName) 
            # get MET with pufit
            outputMETPufitName = 'pufit'+topoclLabel
            topSequence += MissingETGepPufit(outputMETPufitName,inputClusters=inputTopoclName,outputMET=outputMETPufitName); 



    # ClusterTiming currently cannot run on custom clusters
    topSequence += ClusterTiming("DefaultClusters",inputClusterName="CaloCalTopoClusters",outputClusterName="clusters420Timing")

