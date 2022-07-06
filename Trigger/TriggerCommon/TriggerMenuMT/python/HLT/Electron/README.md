Electron trigger chains
=====

This folder contains all the configuration of electron trigger chains. Any code common or shared with Electrons are available in the [Egamma](../Egamma) folder.
Electron triggers are implemented as a sequence of feature extraction and hypothesis testing steps. First selection is performed at The Level 1 (L1).
If L1 accepts the event, then it passes to the HLT the Regions of interest (RoIs) and their type (for us here EM type of RoI). 

The HLT will perform the reconstruction and identification of online electrons first doing a fast reconstruction and then a precision reconstruction.

```mermaid
graph TD;
  subgraph Electron trigger Chain
  L1(L1Accept):::decision --> FastCalo[Fast Calo : Ringer]:::alg

  subgraph Fast Reconstruction

  FastCalo --> FastTrack[ Fast Tracking ]:::alg
  FastTrack --> FastElectrons[/Fast Electron Hypo/]:::hypo

  end

  subgraph Precision Reconstruction
  FastElectrons --> PrecisionCalo[Precision Calo]:::alg
  PrecisionCalo --> PrecisionTrack[Precision Tracking]:::alg
  PrecisionTrack --> PrecisionElectron[/Precision Electron Hypo/]:::hypo

  end

  end

  classDef hypo fill:#dd0;
  classDef alg fill:#faf;
```

In more details, the algorithm and its instances running at each step are represented below. As Electron chains have many steps and many algorithms, below you will find one diagram for the Fast reconstruction and another for the Precision reconstruction:



## Fast Electron Reconstruction

```mermaid
graph TD;
   subgraph Fast Calo
     TrigEgammaFastCalo[T2CaloEgammaReFastAlgo/FastCaloL2EgammaAlg]:::alg --> TrigEgammaElectronHypo[/ TrigEgammaFastCaloHypoAlg/ElectronEgammaFastCaloHypo /]:::hypo
   end

   subgraph Fast Track
     TrigEgammaElectronHypo --> PixelRawDataProvider[PixelRawDataProvider/InDetPixelRawDataProvider__electron]:::alg
     PixelRawDataProvider --> SCTRawDataProvider[SCTRawDataProvider/InDetSCTRawDataProvider__electron]:::alg
     SCTRawDataProvider --> SCTEventFlagWriter[SCTEventFlagWriter/InDetSCTEventFlagWriter__electron]:::alg
     SCTEventFlagWriter --> Pixel[InDet::PixelClusterization/InDetPixelClusterization__electron]:::alg
     Pixel --> SCT[InDet::SCT_Clusterization/InDetSCT_Clusterization__electron]:::alg
     SCT --> SiTracker[InDet::SiTrackerSpacePointFinder/InDetSiTrackerSpacePointFinder__electron]:::alg
     SiTracker --> TrigFastTrackFinder[TrigFastTrackFinder/TrigFastTrackFinder__electron]:::alg
     TrigFastTrackFinder --> TrigTracking[InDet::TrigTrackingxAODCnvMT/InDetTrigTrackParticleCreatorAlg_electron]:::alg
     TrigTracking --> TrigEgammaFastElectron[TrigEgammaFastElectronReAlgo/EgammaFastElectronFex_Clean_gen]:::alg
   end
   TrigEgammaFastElectron --> TrigEgammaFastElectronHypoAlg[/ TrigEgammaFastElectronHypoAlg/TrigEgammaFastElectronHypoAlg /]:::hypo

  classDef hypo fill:#dd0;
  classDef alg fill:#faf;

```



## Precision Electron Reconstruction

We currently have two flavours/variants of Electron reconstruction at HLT. One default variant without any refit of the tracks at precision step, and another *with* track refiting using Gaussian Sum Filter refit.

### Precision Calo for Electrons
The algorithm sequence of Precision Calo for electrons is the same as for Photons. Precision Calo electrons seeds the precision Tracking part of the chain and also provides the electron ```etcut``` chains.

```mermaid
graph TD;
 subgraph Precision Calo

     TrigEgammaFastElectronHypoAlg[/ TrigEgammaFastElectronHypoAlg/TrigEgammaFastElectronHypoAlg /] --> HLTCaloCellMaker[HLTCaloCellMaker/HLTCaloCellMakerRoI]:::alg
     HLTCaloCellMaker --> TrigCaloClusterMaker[TrigCaloClusterMaker/TrigCaloClusterMaker_topoRoI]:::alg
     TrigCaloClusterMaker --> CaloTopoClusterMaker[CaloTopoClusterMaker/TrigTopoMaker]:::alg
     CaloTopoClusterMaker --> CaloTopoClusterSplitter[CaloTopoClusterSplitter/TrigTopoSplitter]:::alg
     CaloTopoClusterSplitter --> CaloClusterMomentsMaker[CaloClusterMomentsMaker/TrigTopoMoments]:::alg
     CaloClusterMomentsMaker --> egammaTopoClusterCopier[egammaTopoClusterCopier/eTrigEgammaTopoClusterCopierPrecisionCaloRoIs]:::alg
     egammaTopoClusterCopier --> egammaRecBuilder[egammaRecBuilder/eTrigEgammaRecPrecisionCaloRoIs]:::alg
     egammaRecBuilder --> egammaSuperClusterBuilder[egammaSuperClusterBuilder/eTrigEgammaSuperClusterBuilderPrecisionCaloRoIs]:::alg
     egammaSuperClusterBuilder --> TrigEgammaPrecisionCaloHypoAlg[/ TrigEgammaPrecisionCaloHypoAlg/ElectronprecisionCaloElectronHypo /]:::hypo

 end

TrigEgammaPrecisionCaloHypoAlg --> etcut(etcut chains end here)
TrigEgammaPrecisionCaloHypoAlg --> PrecisionTracking[Precision Tracking]:::alg
classDef hypo fill:#dd0;
classDef alg fill:#faf;

```

### Precision Tracking

Precision tracking is instnaciated after Precision Calo electron part of the chain. ITs run within the Electron RoI and tracks are matched to the cluster in the previous step. Precision Tracking seeds Precision Electron reconstruction at the HLT and also provides idperf chains. 

GSF refitting of the track is new and will be used online for the first time in Run3. For commissioning and comparing w.r.t. Run2 we still provide and support non-GSF refitted tracks/electrons too


```mermaid
graph TD;
 subgraph Precision Tracking

   TrkAmbiguityScore[Trk::TrkAmbiguityScore/InDetTrigMTTrkAmbiguityScore_electron]:::alg
   TrkAmbiguityScore --> TrkAmbiguitySolver[Trk::TrkAmbiguitySolver/InDetTrigMTTrkAmbiguitySolver_electron]:::alg
   TrkAmbiguitySolver --> TRTRawDataProvider[TRTRawDataProvider/InDetTrigMTTRTRawDataProvider_electron]:::alg
   TRTRawDataProvider --> TRT_RIO_Maker[InDet::TRT_RIO_Maker/InDetTrigMTTRTDriftCircleMaker_electron]:::alg
   TRT_RIO_Maker --> TRT_TrackExtensionAlg[InDet::TRT_TrackExtensionAlg/InDetTrigMTTrackExtensionAlg_electron]:::alg
   TRT_TrackExtensionAlg --> InDetExtensionProcessor[InDet::InDetExtensionProcessor/InDetTrigMTExtensionProcessor_electron]:::alg
   InDetExtensionProcessor --> TrackParticleCnvAlg[xAODMaker::TrackParticleCnvAlg/InDetTrigMTxAODParticleCreatorAlgelectron_IDTrig]:::alg
   TrackParticleCnvAlg --> TrigEgammaPrecisionTrackingHypoAlg[/TrigEgammaPrecisionTrackingHypoAlg/ElectronprecisionTrackingHypo/]:::hypo


   TrigEgammaPrecisionTrackingHypoAlg --> EMBremCollectionBuilder[EMBremCollectionBuilder/TrigEMBremCollectionBuilder]:::alg
   EMBremCollectionBuilder --> TrigEgammaPrecisionTrackingHypoAlgGSF[TrigStreamerHypoAlg/precisionTracks_GSFRefittedHypo]:::hypo
 end

 
TrigEgammaPrecisionTrackingHypoAlg --> idperf(idperf chains end here)
TrigEgammaPrecisionTrackingHypoAlg --> PrecisionElectron[Precision Electron]:::alg
TrigEgammaPrecisionTrackingHypoAlgGSF --> idperfgsf(idperf_gsf chains end here)
TrigEgammaPrecisionTrackingHypoAlgGSF --> PrecisionElectron[Precision ElectronGSF]:::alg



  classDef hypo fill:#dd0;
  classDef alg fill:#faf;
```

### Precision Electron
Precision electrons at HLT are ```xAOD::Electron``` objects made by ```xAODEgammaBuilder``` algorithm combining clusters from the Precision Calo electron stage and tracks from Precision Tracking. In Run2 tracks were not reffited for Brem recovery at HLT. Although by default we do Gaussian Sum Filter (GSF) to reffit tracks for Brem recovery in Run3, we still support electron Trigger Chains built from  nonGSF reffitted traks.

In precision Electron step both flavours are identical, although the name of the alg instances have a different suffix (```noGSF``` or ```GSFRefit```) depending on the case.


```mermaid
graph TD;



 subgraph Precision Electron nonGSF


egammaRecBuildere[egammaRecBuilder/TrigEgammaRecElectron_noGSF]:::alg
egammaRecBuildere --> electronSuperClusterBuilder[electronSuperClusterBuilder/TrigElectronSuperClusterBuilder_noGSF]:::alg
electronSuperClusterBuilder --> xAODEgammaBuilder[xAODEgammaBuilder/TrigTopoEgammaElectronCfg_noGSF]:::alg
xAODEgammaBuilder --> IsolationBuilder[IsolationBuilder/TrigElectronIsoBuilderCfg_noGSF]:::alg
IsolationBuilder --> TrigEgammaPrecisionElectronHypoAlg[TrigEgammaPrecisionElectronHypoAlg/TrigEgammaprecisionElectronHypoAlg_noGSF]:::hypo
 end

  classDef hypo fill:#dd0;
  classDef alg fill:#faf;
```

Precision Electrons at HLT in Run3 by default are reconstructed combining GSF-reffited tracks and superclusters. 

```mermaid
graph TD;

 subgraph Precision GSF Electron

egammaRecBuilderGSF[egammaRecBuilder/TrigEgammaRecElectron_GSFRefit]:::alg
egammaRecBuilderGSF --> electronSuperClusterBuilderGSF[electronSuperClusterBuilder/TrigElectronSuperClusterBuilder_GSFRefit]:::alg
electronSuperClusterBuilderGSF --> xAODEgammaBuilderGSF[xAODEgammaBuilder/TrigTopoEgammaElectronCfg_GSFRefit]:::alg
xAODEgammaBuilderGSF --> IsolationBuilderGSF[IsolationBuilder/TrigElectronIsoBuilderCfg_GSFRefit]:::alg
IsolationBuilderGSF --> TrigEgammaPrecisionElectronHypoAlgGSF[TrigEgammaPrecisionElectronHypoAlg/TrigEgammaprecisionElectronHypoAlg_GSFRefit]:::hypo

 end


  classDef hypo fill:#dd0;
  classDef alg fill:#faf;


```

Modules in this directory
-----

* [TrigElectronFactories](TrigElectronFactories.py)
  * ToolFactories to configure egammaAlgs to be used at the HLT
* [ElectronRecoSequences](ElectronRecoSequences.py)
  * Assembles the sequences for fast reconstruction
* [PrecisionElectronRecoSequences](PrecisionElectronRecoSequences.py)
  * Assembles the sequences for precision reconstruction
* [PrecisionElectronRecoSequences_GSF](PrecisionElectronRecoSequences_GSF.py)
  * Assembles the sequences for precision reconstruction with Gaussian Sum Filter tracks
* [TrigEMBremCollectionBuilder](TrigEMBremCollectionBuilder.py)
  * ToolFactory to instantiate TrigEMBremCollectionBuilder with default configuration
* [generateElectron](generateElectron.py)
  * Prototype for NewJO chain configuration
