#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrigEDMConfig.TriggerEDMRun3 import recordable
from AthenaCommon.CFElements import parOR

def precisionPhotonRecoSequence(RoIs):
    """ With this function we will setup the sequence of offline EgammaAlgorithms so to make a photon for TrigEgamma 

    Sequence of algorithms is the following:
      - egammaRecBuilder creates egammaObjects out of clusters and tracks. Here, at HLT photons we will only use clusters. 
      - photonSuperClusterBuilder algorithm will create superclusters out of the toposlusters and tracks in egammaRec under the photon hypothesis
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/python/egammaSuperClusterBuilder.py#L26 
      - TopoEgammBuilder will create photons and electrons out of trakcs and SuperClusters. Here at HLT photons the aim is to ignore electrons and not use tracks at all.
          https://gitlab.cern.ch/atlas/athena/blob/master/Reconstruction/egamma/egammaAlgs/src/topoEgammaBuilder.cxx
    """





    from egammaAlgs import egammaAlgsConf
    from egammaTools import egammaToolsConf
    from egammaRec.Factories import AlgFactory, ToolFactory
    #from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags

    from TriggerMenuMT.HLTMenuConfig.Egamma.PrecisionCaloSequenceSetup import precisionCaloMenuDefs
    from TriggerMenuMT.HLTMenuConfig.Egamma.PhotonDef import PhotonMenuDefs
    from TriggerMenuMT.HLTMenuConfig.Egamma.ElectronDef import ElectronMenuDefs

    from egammaTools.egammaToolsFactories import egammaSwTool, egammaMVASvc, EGammaAmbiguityTool, EMTrackMatchBuilder, EMConversionBuilder

    TrigEMClusterTool = ToolFactory(egammaToolsConf.EMClusterTool,
                             OutputClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters, #PhotonMenuDefs.outputClusterKey,
                             OutputTopoSeededClusterContainerName = PhotonMenuDefs.outputTopoSeededClusterKey,
                             ClusterCorrectionTool = egammaSwTool,
                             doSuperCluster = True,
                             MVACalibSvc = egammaMVASvc                             
                             )


    # Prepare first egammaRec:
    TrigEgammaRecBuilder = AlgFactory( egammaAlgsConf.egammaRecBuilder,
            name = 'TrigEgammaRecBuilder' ,
            InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters, # Use as input, the clusters made by precisionCalo
            egammaRecContainer=PhotonMenuDefs.EgammaRecKey,
            doTrackMatching = False,
            doConversions = False,
            ## Builder tools
            TrackMatchBuilderTool = EMTrackMatchBuilder, # Don't want to use these for trigger....
            ConversionBuilderTool = EMConversionBuilder,  # Don't want to use these for trigger....
            doAdd = False,
            )
    TrigPhotonSuperClusterBuilder = AlgFactory( egammaAlgsConf.photonSuperClusterBuilder,
                                            name = 'TrigPhotonSuperClusterBuilder',
                                            InputEgammaRecContainerName=PhotonMenuDefs.EgammaRecKey,
                                            SuperPhotonRecCollectionName=PhotonMenuDefs.SuperPhotonRecCollectionName,
                                            ClusterCorrectionTool=egammaSwTool,
                                            MVACalibSvc= egammaMVASvc,
                                            doConversions = False,
                                            AddClustrsMatchingVtxTracks = False,
                                            ConversionBuilderTool = EMConversionBuilder,
                                            doAdd = False
                                            )

    TrigTopoEgammaBuilder = AlgFactory( egammaAlgsConf.topoEgammaBuilder, name = 'TrigTopoEgammaBuilder',
        SuperElectronRecCollectionName = ElectronMenuDefs.SuperElectronRecCollectionName,
        SuperPhotonRecCollectionName = PhotonMenuDefs.SuperPhotonRecCollectionName,
        ElectronOutputName = ElectronMenuDefs.outputElectronKey,
        PhotonOutputName = PhotonMenuDefs.outputPhotonKey,  
        AmbiguityTool = EGammaAmbiguityTool,
        EMClusterTool = TrigEMClusterTool,
        doAdd = False,
        )


    # The sequence of these algorithms
    thesequence = parOR( "precisionPhoton_"+RoIs)

    # Create the sequence of three steps:
    #  - TrigEgammaBuilder, TrigPhotonSuperClusters, TrigTopoEgammaBuilder
    TrigEgammaAlgo = TrigEgammaRecBuilder()
    TrigEgammaAlgo.InputTopoClusterContainerName = precisionCaloMenuDefs.precisionCaloClusters
    thesequence += TrigEgammaAlgo

    trigPhotonAlgo = TrigPhotonSuperClusterBuilder()
    trigPhotonAlgo.InputEgammaRecContainerName = TrigEgammaAlgo.egammaRecContainer
    thesequence += trigPhotonAlgo

    trigTopoEgammaAlgo = TrigTopoEgammaBuilder()
    trigTopoEgammaAlgo.SuperPhotonRecCollectionName = trigPhotonAlgo.SuperPhotonRecCollectionName
    collectionOut = trigTopoEgammaAlgo.PhotonOutputName
    thesequence += trigTopoEgammaAlgo

    return (thesequence, collectionOut)








def l2PhotonAlgCfg( flags ):
    acc = ComponentAccumulator()
    from TrigEgammaHypo.TrigL2PhotonFexMTConfig import L2PhotonFex_1

    photonFex= L2PhotonFex_1()
    photonFex.TrigEMClusterName = recordable("HLT_L2CaloEMClusters")
    photonFex.PhotonsName = recordable("HLT_L2Photons")
    photonFex.RoIs = "L2PhotonRecoRoIs"

    return acc, photonFex

def l2PhotonRecoCfg( flags ):
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import InViewReco

    reco = InViewReco("L2PhotonReco")
    reco.inputMaker().RequireParentView = True
    reco.inputMaker().RoIsLink="roi"
    import AthenaCommon.CfgMgr as CfgMgr

    moveClusters = CfgMgr.AthViews__ViewDataVerifier("photonViewDataVerifier")
    moveClusters.DataObjects = [ ('xAOD::TrigEMClusterContainer','StoreGateSvc+HLT_L2CaloEMClusters') ]
    reco.addRecoAlg( moveClusters )

    algAcc, alg = l2PhotonAlgCfg( flags )
    reco.addRecoAlg( alg )
    reco.merge( algAcc )

    return reco

def l2PhotonHypoCfg( flags, Photons='Unspecified', RunInView=True):
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT

    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = Photons
    l2PhotonHypo.RunInView = RunInView

    return l2PhotonHypo


def generatePhotonsCfg( flags ):

    acc = ComponentAccumulator()
    from TriggerMenuMT.HLTMenuConfig.Menu.MenuComponents import MenuSequence, ChainStep, Chain, RecoFragmentsPool

    from TrigEgammaHypo.TrigL2CaloHypoTool import TrigL2CaloHypoToolFromDict
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2CaloHypoAlgMT
    l2CaloHypo              = TrigL2CaloHypoAlgMT("L2PhotonCaloHypo")
    l2CaloHypo.CaloClusters = 'L2CaloEMClusters'


    from TriggerMenuMT.HLTMenuConfig.Electron.ElectronRecoSequences import l2CaloRecoCfg
    
    
    l2CaloReco = RecoFragmentsPool.retrieve( l2CaloRecoCfg, flags )
    acc.merge( l2CaloReco )
    
    fastCaloSequence = MenuSequence( Sequence    = l2CaloReco.sequence(),
                                     Maker       = l2CaloReco.inputMaker(),
                                     Hypo        = l2CaloHypo,
                                     HypoToolGen = TrigL2CaloHypoToolFromDict )

    fastCaloStep = ChainStep( "Photon_step1", [fastCaloSequence] )



    l2PhotonReco = RecoFragmentsPool.retrieve( l2PhotonRecoCfg, flags )
    acc.merge( l2PhotonReco )
    
    from TrigEgammaHypo.TrigEgammaHypoConf import TrigL2PhotonHypoAlgMT
    l2PhotonHypo = TrigL2PhotonHypoAlgMT()
    l2PhotonHypo.Photons = "L2Photons"
    l2PhotonHypo.RunInView=True

    from TrigEgammaHypo.TrigL2PhotonHypoTool import TrigL2PhotonHypoToolFromDict

    l2PhotonSequence = MenuSequence( Sequence    = l2PhotonReco.sequence(),
                                     Maker       = l2PhotonReco.inputMaker(),
                                     Hypo        = l2PhotonHypo,
                                     HypoToolGen = TrigL2PhotonHypoToolFromDict )

    l2PhotonStep = ChainStep( "Photon_step2", [ l2PhotonSequence] )
    


    chains = [ Chain(c.split()[0], c.split()[1], [fastCaloStep, l2PhotonStep] )  for c in flags.Trigger.menu.photons ]    


    

    return acc, chains


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.Trigger.menu.photons = ["HLT_g5_etcut L1_EM3", "HLT_g10_etcut L1_EM3"]
    ConfigFlags.lock()

    acc, sequences = generatePhotonsCfg( ConfigFlags )
    acc.printConfig()
