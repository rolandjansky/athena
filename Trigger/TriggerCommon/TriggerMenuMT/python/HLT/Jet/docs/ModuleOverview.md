Overview of HLT jet reco configuration modules
=====

<details>
<summary>Note on RecoFragmentsPool</summary>

As an intermediate step before the new job configuration (see https://atlassoftwaredocs.web.cern.ch/guides/ca_configuration/) is fully adopted, the [`RecoFragmentsPool`](https://gitlab.cern.ch/atlas/athena/-/blob/master/Trigger/TriggerCommon/TriggerMenuMT/python/HLT/Menu/MenuComponents.py) construct is used to avoid duplication of algorithms within AlgSequences. This is used as follows. Wherever a function returning a sequence is called, the call should be made through the following expression:
```
mySequence = RecoFragmentsPool.retrieve( seqGenerator, configFlags, **kwargs )
```
where `seqGenerator` is a function that will return the desired sequence type, and must receive a one positional argument (`configFlags`) and an arbitrary number of keyword arguments (`**kwargs`). Internally, `RecoFragmentsPool` will cache the result, mapping it to the input arguments. Consequently, the kwargs must be hashable types (basically, no dicts). In the jet trigger configuration code, we frequently use the `**` operator to convert between dicts and kwargs.

</details>

<details>
<summary>Note on sequence types</summary>

The trigger algorithm sequencing is controlled by sequences and filter algorithms. For a more detailed description of how this functions, see e.g. [these slides](https://cds.cern.ch/record/2642559?ln=en).

Essentially, two types of sequence are used:
* parOR: executes all children in parallel and returns the result of an OR over these as its filter decision -- used for reco algorithms
* seqAND: executes all children in sequence and returns the result of an AND over these as its filter decision -- used to activate/deactivate subsequences.
An algorithm may be a child of multiple sequences, but will only execute once.

These are used as depicted in the diagram below. Each step is built with an OR that executes all filters for the step (e.g. these could be electron & muon legs of an e/mu chain). The filters return a pass/fail decision based on preceding hypos. Then the main OR attempts to execute all substeps in parallel. The same filter algorithms are placed in ANDs within each substep, so they block execution if the filter criterion failed (Filter A). Results from the hypos are passed to the next step.

```mermaid
graph LR;

  topAND[Step 1 AND]:::and --> prefiltOR[Prefilter OR]:::or

  subgraph step1[Step 1]

    prefiltOR --> filtA(Filter A):::filterfail
    prefiltOR --> filtB(Filter B):::filterpass

    topAND --> stepOR[Main OR]:::or
    stepOR --> seqA[AND A]:::and
    stepOR --> seqB[AND B]:::and

    subgraph Substep A
      seqA --> filtA2(Filter A):::filterfail
      filtA2 -.->|blocks| orA[OR A]:::or
      orA -.->recoA([Reco A]):::reco
      orA -.-> hypoA([Hypo A]):::hypo
    end
  
    subgraph Substep B
      seqB --> filtB2(Filter B):::filterpass
      filtB2 --> orB[OR B]:::or
      orB --> recoB([Reco B]):::reco
      orB --> hypoB([Hypo B]):::hypo
    end
  
  end

  classDef filterpass fill:#0d0;
  classDef filterfail fill:#d00;

  classDef reco fill:#ffa;
  classDef hypo fill:#faa;
  
  classDef or fill:#0ff;
  classDef and fill:#f0f;
```

This scheduling is basically handled by menu construction. As a rule of thumb, we use a `seqAND` as the basis for every `MenuSequence`, and `parOR` for all reconstruction sequences.

</details>

[GenerateJetChainDefs](../GenerateJetChainDefs.py)
-----

Called by the menu code in [`TriggerMenuMT/python/HLT/Menu/GenerateMenuMT.py`](../../Menu/GenerateMenuMT.py) to translate the HLT chain item into a concrete algorithm sequence.

The menu code creates a chain dictionary from the chain name, of which the jet parts are given to `GenerateJetChainDefs.generateChainConfigs` to be interpreted by `JetChainConfiguration`.

[JetChainConfiguration](../JetChainConfiguration.py)
-----

Defines the `JetChainConfiguration` object responsible for interpreting the chain dictionary and building a `Chain` object that is returned to the menu. `JetChainConfiguration` extends the [`ChainConfigurationBase`](../../Menu/ChainConfigurationBase.py) type.

Its `assembleChain` function extracts the reco configuration from the jet chain dictionary, using it (via functions from `JetMenuSequences.py`) to generate a `MenuSequence` that forms one or more jet `ChainStep` objects. Multiple `ChainSteps` may be combined into a single chain for filtering purpose, mainly to allow fast reco and filtering before slower reco is executed.

The following possible types of `ChainStep` are defined:
1. Calo Hypo ChainStep: a single-step chain, with only calo reco defining the hypo selection
2. Calo Reco ChainStep: a step that performs calo (cell+topocluster) reco with a passthrough hypo, which should be followed by a Tracking Hypo ChainStep. No jets are reconstructed.
3. Calo Presel ChainStep: a step that performs calo reco with a preselection hypo, which should be followed by a Tracking Hypo ChainStep. Calo jets will be reconstructed and used as input to the preselection hypo. The preselection criteria are determined from the "preselNjX" entry in the chain dictionary.
4. Tracking Hypo ChainStep: a step that performs (FullScan) tracking reco, possibly including Particle Flow, then reconstructs jets with tracks, and performs the final hypo selection on these. Must follow either a Calo Reco or a Calo Presel ChainStep.
5. TLA ChainStep: This step is appended for TLA chains, following the terminal hypo (calo or tracking). It performs no selection, and only flags a subset of the reconstructed jets to be written to file. Configuration for this is in `JetTLASequences.py`

[JetMenuSequences](../JetMenuSequences.py)
-----

Defines the `MenuSequence` objects that form the basis of `ChainSteps`. Each `MenuSequence` contains a reco sequence, an `InputMaker` (which defines the Region of Interest for the reco), a hypo algorithm and a hypo tool generator function. These are created as follows:
* Reco sequence -- defined via functions in `JetRecoSequences.py`, based on the `JetRecoDict` extracted from the chain dictionary.
* InputMaker -- varies depending on which sequence is needed. The basic fullscan InputMaker is provided by calo code.
* Hypo algorithm -- In most cases this is a `TrigJetHypoAlg` defined in the `TrigHLTJetHypo` package. For the calo reco sequence only, this is a streamer hypo that does no selection.
* Hypo tool generator -- For standard steps with hypo selection, this is the `TrigHLTJetHypo.TrigJetHypoToolConfig.trigJetHypoToolFromDict()` function, which will interpret the chain dict. For passthrough calo reco steps, this is instead just a function that returns a streamer hypo tool.

*Note: For every jet collection there is exactly one matching hypo algorithm, but every distinct selection on that collection is defined by a different hypo tool that is a child of this hypo algorithm.*

[JetRecoSequences](../JetRecoSequences.py)
-----

This module provides the functions that define the reconstruction sequences for any given reconstruction configuration.

The reconstruction sequence is determined from the contents of the reco information in the `chainDict`, and will contain some subset of:
* Calo reco sequence: cell unpacking and topoclustering -- one instance shared between all jet chains
* Constituent modifications [optional] -- pileup suppression on topoclusters or corrections to PFlow four-vectors
* PseudoJetGetters & algs -- conversion of the ATLAS EDM into `fastjet` EDM
* JetAlgorithm -- holds the jet finder tools and any modifiers e.g. calibration
The reco sequence may be nested further in the case of reclustering or trimming workflows, in which case the "basic" jet reco from clusters is embedded in a second sequence, which continues by running the second step reconstruction.
Configuration of the Athena components is handled by the [`Reconstruction/Jet/JetRecConfig`](../../../../../../../Reconstruction/Jet/JetRecConfig) package.

The main `jetRecoSequence()` function forwards to one of:
* `standardJetRecoSequence()` -- defines standard jet reconstruction from input pseudojets (clusters, PFOs). This in turn calls `standardJetBuildSequence()` to perform the reconstruction of uncalibrated jets, and if calibration is requested additionally schedules a `JetCopier` to shallow-copy and calibrate the jets.
* `groomedJetRecoSequence()` -- grooms an input jet collection (internally calls `standardJetBuildSequence()` to create the ungroomed jets).
* `reclusteredJetRecoSequence()` -- reclusters an input jet collection (internally calls `standardJetBuildSequence()` to create the basic jets)

[JetRecoConfiguration](../JetRecoConfiguration.py)
-----

Helper functions to facilitate the operations in `JetRecoSequences.py`:
* Extraction & compression of the `JetRecoDict`
* Translation of the `JetRecoDict` contents into the configuration objects from `JetRecConfig`, e.g. `JetConstituent` and `JetDefinition`.
* Definition of jet modifier lists

[JetTrackingConfig](../JetTrackingConfig.py)
-----

Helper functions to configure the jet tracking instance and to define track-related modifiers and collections within the jet domain.

[JetTLASequences](../JetTLASequences.py)
-----

Helper functions to configure the TLA jet sequence.

[TriggerJetMods](../TriggerJetMods.py)
-----

Definitions of specialised jet modifier configurations unique to the trigger context.

[generateJet](../generateJet.py)
-----

Prototype code for the new job configuration.

