Photon trigger chains
=====

This folder contains all the configuration of photon trigger chains. Any code common or shared with Electrons are available in the [Egamma](../Egamma) folder.
Photon triggers are implemented as a sequence of feature extraction and hypothesis testing steps. First selection is performed at The Level 1 (L1).
If L1 accepts the event, then it passes to the HLT the Regions of interest (RoIs) and their type (for us here EM type of RoI). 

The HLT will perform the identification of photons 

```mermaid
graph TD;
  subgraph Photon trigger Chain
  L1(L1Accept):::decision --> FastCalo[Fast Calo : Cut_Based]:::alg

  subgraph Fast Reconstruction

  FastCalo --> FastPhotons[/Fast Photon Hypo/]:::hypo

  end

  subgraph Precision Reconstruction
  FastPhotons --> PrecisionCalo[Precision Calo]:::alg
  PrecisionCalo --> PrecisionPhoton[/Precision Photon Hypo/]:::hypo

  end

  end

  classDef hypo fill:#dd0;
  classDef alg fill:#faf;
```

In more details, the algorithm and its instances running at each step are represented below:

```mermaid
graph TD;
 subgraph Fast Photon
     TrigEgammaFastPhoton[TrigEgammaFastPhotonReAlgo/TrigEgammaFastPhoton_ReFastAlgo]:::alg --> TrigEgammaFastPhotonHypo[/ TrigEgammaFastPhotonHypoAlg/TrigEgammaFastPhotonHypoAlg /]:::hypo
  end

subgraph Precision Reconstruction
 subgraph Precision Calo
  TrigEgammaFastPhotonHypo --> HLTCaloCellMakerRoI[HLTCaloCellMaker/HLTCaloCellMakerRoI]:::alg
  HLTCaloCellMakerRoI --> CaloTopoClusterMaker[ CaloTopoClusterMaker/TrigTopoMaker]:::alg
  CaloTopoClusterMaker --> CaloTopoClusterSplitter[CaloTopoClusterSplitter/TrigTopoSplitter]:::alg
  CaloTopoClusterSplitter --> CaloClusterMomentsMaker[CaloClusterMomentsMaker/TrigTopoMoments]:::alg
  CaloClusterMomentsMaker --> egammaTopoClusterCopier[egammaTopoClusterCopier/gTrigEgammaTopoClusterCopierPrecisionCaloRoIs]:::alg
  egammaTopoClusterCopier --> egammaRecBuilder[egammaRecBuilder/gTrigEgammaRecPrecisionCaloRoIs]:::alg
  egammaRecBuilder --> egammaSuperClusterBuilder[egammaSuperClusterBuilder/gTrigEgammaSuperClusterBuilderPrecisionCaloRoIs]:::alg
  egammaSuperClusterBuilder --> TrigEgammaPrecisionCaloHypoAlg[/TrigEgammaPrecisionCaloHypoAlg/PhotonprecisionCaloPhotonHypo/]:::hypo
 

 end
 subgraph Precision Photon

TrigEgammaPrecisionCaloHypoAlg --> xAODEgammaBuilder[xAODEgammaBuilder/TrigTopoEgammaPhotons]:::alg
xAODEgammaBuilder --> IsolationBuilder[IsolationBuilder/TrigPhotonIsolationBuilder]:::alg
IsolationBuilder --> egammaMonitorPhotonAlgorithm[egammaMonitorPhotonAlgorithm/PrecisionPhotonTopoEgammaBuilder]:::alg
egammaMonitorPhotonAlgorithm --> egammaMonitorSuperClusterAlgorithm[egammaMonitorSuperClusterAlgorithm/PrecisionPhotonSuperClusterBuilder]:::alg
egammaMonitorSuperClusterAlgorithm --> TrigEgammaPrecisionPhotonHypoAl[/ TrigEgammaPrecisionPhotonHypoAlg/PhotonprecisionPhotonHypo /]:::hypo
 end

TrigEgammaPrecisionCaloHypoAlg --> etcut(etcut chains end here)
end

  classDef hypo fill:#dd0;
  classDef alg fill:#faf;


```



Modules in this directory
-----

* [TrigPhotonFactories](TrigPhotonFactories.py)
  * ToolFactories to configure egammaAlgs to be used at the HLT
* [PhotonRecoSequences](PhotonRecoSequences.py)
  * Assembles the sequences for each stage of reconstruction
* [generatePhoton](generatePhoton.py)
  * Prototype for NewJO chain configuration
