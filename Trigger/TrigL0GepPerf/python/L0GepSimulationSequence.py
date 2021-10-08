#put the GEP sequence here 
import logging as log
from AthenaCommon.Include import include

#
# The default available options are the following.
#
# Topoclusters:
# 'Calo422' : 42 topoclusters at EM scale 
# 'Calo420' : 420 topoclusters at EM scale 
# 'CaloCal' : 420 topoclusters LC calibrated 
#
# Pileup suppression:
# 'Vor'   : Voronoi, 
# 'SK'    : SoftKiller
# 'VorSK' : Voronoi+SoftKiller
#
# Jet reconstruction:
# 'AntiKt4' : offline AntiKt4
#
# Custom topoclustering and jet reconstruction algorithms can be called 
# following the naming scheme in GepClusteringAlg.cxx and GepJetAlg.cxx.
# 



def setupL0GepSimulationSequence(
        topoclAlgs = ['Calo422'], 
        puSupprAlgs = [''],
        jetAlgs = ['AntiKt4'] ):
    
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


    # run JFEX algorithms
    from TrigT1CaloFexPerf.L1PerfControlFlags import L1Phase1PerfFlags as pflags
    pflags.Calo.UseAllCalo=True
    pflags.CTP.RunCTPEmulation=False 
    from RecExConfig.RecFlags import rec
    rec.readAOD=True
    rec.readESD=True
    rec.readRDO=False
    rec.doESD=True
    rec.doWriteAOD=False
    include("TrigT1CaloFexPerf/createL1PerfSequence.py")


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

