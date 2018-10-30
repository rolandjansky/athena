# JetRecTools: Support classes for jet reconstruction

## Package overview

This package exists to provide classes that contribute additional functionality to the basic jet reconstruction capabilities in `Reconstruction/Jet/JetRec`. These support classes are broadly of three types:
* Additional `PseudoJetGetter` classes for the following classes, or that require special selection or four-vector handling (e.g. because they are do not inherit from `IParticle`).
* Constituent modification classes that allow operations to be applied to the jet inputs (e.g. topoclusters, PFOs) prior to jet building, for example origin corrections and pileup suppression
* Others

### Custom PseudoJetGetters

* TrackPseudoJetGetter -- handles `TrackParticle`, optionally applying a vertex association restriction.
* PFlowPseudoJetGetter -- handles `ParticleFlowObject`, applying a vertex association restriction and some special selection for 0 pt objects.

*Note that `MuonSegment` anomalously has a custom PseudoJetGetter in `JetRec`*

### Constituent modifier classes

* JetConstitModSequence -- the top-level constituent modification scheduler for tools inheriting from `JetConstitModifierBase` (all subsequent tools in this list), which takes an input container (of topoclusters, PFOs, ...) and generates shallow copies to be modified by a sequence of individual tools. The updated four-vector is set as the default four-momentum, such that downstream tools can simply use the `IParticle::p4()` interface.
* CaloClusterConstituentsOrigin -- updates `CaloCluster` four-momenta to point to the nominal hard scatter primary vertex, rather than to the detector origin.
* ClusterAtEMScaleTool -- updates `CaloCluster` default four-vector to be the EM scale four-vector w/o changing signal state (ordinarily this is the LC-calibrated four-vector).
* CorrectPFOTool -- applies four-vector modifications to `PFO` constituents. For charged PFOs: weights to address EM scale response and disabling of cell subtraction in high energy density regions. For neutral PFOs: pointing of the four-vector to the nominal hard scatter vertex, rather than to the detector origin.
* ChargedHadronSubtractionTool -- marks `PFO` constituents as being from the hard scatter vertex or not, for subsequent selection.
* ConstitTimeCutTool -- Applies timing cuts to constituents.
* ConstituentSubtractorTool -- Applies constituent subtraction (see [arxiv:1403.3108](https://arxiv.org/abs/1403.3108)) to constituents.
* PuppiWeightTool -- Applies the PUPPI algorithm (see [arxiv:1407.6013](https://arxiv.org/abs/1407.6013)) to constituents.
* SoftKillerWeightTool -- Applies SoftKiller (see [arxiv:1407.0408](https://arxiv.org/abs/1407.0408)) to constituents.
* VoronoiWeightTool -- Applies Voronoi subtraction (rho subtraction assuming Voronoi areas), spreading or filtering to constituents.

### Other tools

*  JetInputElRemovalTool -- Removes clusters matched to electrons from the cluster collection
* JetTrackSelectionTool -- Uses the ID track selection tool to generate a preselected track collection, mainly for building track jets, and can also be used by jet modifiers to select tracks.
* SimpleJetTrackSelectionTool -- Track selection tool with manually-implemented cuts.
* TrackVertexAssociationTool -- Creates a JetTrackVertexAssociation object 

> Written with [StackEdit](https://stackedit.io/).