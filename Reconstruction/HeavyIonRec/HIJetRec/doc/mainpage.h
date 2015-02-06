/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////
///
/// \mainpage HIJetRec package
///
/// \author Aaron Angerami <angerami@cern.ch>
/// \date Jan, 2015
///
/// \section Overview
/// HIJetRec provides the necessary extensions of the standard jet software
/// to perform the UE background subtraction required in HI collisions.
///
/// 1. Due to the nature of the fluctuations HI events, the inputs to the 
/// calo jet recontruction are "HI clusters" - CaloCluster objects which are
/// essentially 0.1x0.1 CaloTowers with some slight modifications. The building
/// of these objects is handled by the HIClusterMaker algorithm.
///
/// 2. The UE background has significant variations in eta, phi and calorimeter 
/// sampling due to both detector effects and physics correlations.
/// This UE is initially calculated as part of the reco implemented in HIGlobal,
/// and the resulting object, HIEventShapeContainer, is used by HIJetRec.
/// Initially, the UE calculation contains a contribution from the jets
/// themselves. This contribution is removed in an iterative procedure in
/// which the jets are identified, and the UE is recalculated, excluding
/// the clusters associated with these jets.
/// The recalculation of the UE is performed by the HIEventShapeJetIteration
/// algorithm.
///
/// 3. As the UE is defined locally (eta/phi/layer variations) the background
/// subtraction is also local, and is applied separately to each constituent 
/// in the jet. This is accomplished by adding the HIJetConstituentSubtractionTool
/// to the list of modifiers for HI jets.
/// This tool is equiped on the python side with the name of the HIEventShapeContainer
/// that defines the UE to be subtracted, and a specific instance of a subtractor tool
/// which inherit from the IHISubtratorTool/HIJetSubtractorToolBase bases. This tool
/// contains the details of how to apply the background subtraction to a given constituent
/// and thus encapsulates things like back-navigation to the cells (HIJetCellSubtractorTool)
/// or simply subtracts at the constituent level (HIJetClusterSubtractorTool).
///
/// \section Design
/// The algorithm scheduling is performed in  HIJetRec_jobOptions.py. This job options file is
/// included from <tt>HIRecExample/HeavyIonRec_jobOptions.py</tt>, which is pulled into the combined
/// reconstruction if the <tt>rec.doHeavyIon</tt> flag is set to <tt>True</tt>. The algorithms proceed as follows:
///
/// - Checks on upstream algorithms whose output is required: CaloTowers and HIEventShapeContainer
/// - HI-style clusters are built from CaloTowers with HIClusterMaker algorithm.
/// - Initial jet finding performed with single instance of a JetAlgorithm
///     - PseudoJetGetter instance taking HIClusters as input is created
///     - Finding with various R values is run creating <tt>AntiKtRHI_Unsubtr</tt> jet collections
///     - Ghost associations are also performed as part of the finding
///     - JetModifier tools are added to calculate HI moments (HIJetMaxOverMeanTool) and to provide
///       extra associations needed later on.
///     - Non-calo jets, e.g. track and truth jets, are also be built here.
/// - Iteration steps
///     - Copy R=0.2 unsubtracted jets and filter them on the basis of the HI moments (separate JetAlgorithm instance).
///     - These jets form a set of seeds, which are passed along with the HIEventShapeContainer to the 
///       HIEventShapeJetIteration algorithm, which produces a new HIEventShapeContainer that has the contribution the 
///       seed jets removed from the calculation of the event shape variables. Specifically the jet-cluster association 
///       is used and all clusters within a radius DR controlled by a package flag, <tt>ExclusionRadius</tt>, are excluded.
///     - A new JetAlgorithm instance is scheduled where the unsubtracted R=0.2 jets are again copied, and an additional
///       JetModifier is added, HIJetConstituentSubtractionTool, which computes subtracted kinematics for each constituent
///       within the jet on the basis of the HIEventShapeContainer created in the previous step, and an instance of a
///       IHISubtractionTool, which handles the per-cluster subtraction. 
///     - Seeds are constructed from the set of jets defined in the previous step along with TrackJets and another 
///       HIEventShapeJetIteration algorithm is scheduled. The original HIEventShapeContainer is still used as input.
///     - At this stage, three separate HIEventShapeContainers have been created, each with different seeds. Three separate 
///       HIJetConstituentSubtractionTool instances are built, one for each background
/// - The HIClusterSubtraction algorithm is run, which uses the final HIEventShapeContainer to apply a background subtraction 
///   to all clusters in the event and store the subtracted kinematics as the <tt>alt</tt> signal state.
/// - Final jet kinematics: <tt>AntiKtRHIJets</tt>
///     - For each R value the initial unsubtracted jets are cloned, and the three HIJetConstituentSubtractorTool instances
///       are added as modifiers. In each case, the tool adds an effective signal state to the jet representing the kinematics
///       with the background subtraction applied. The third modifier is configured to set the actual jet kinematics to those
///       after subtraction.
///     - A JES calibration tool is added to the list of modifiers.
///     - Jet substructure and moment tools are added to the list of modifiers. When navigating back to the constituents, these
///       tools should now see clusters with the correct subtraction applied.
/// - b-tagging is applied to each of the final jet collections.
////////////////////////////////////////////////////////////////////////////////
