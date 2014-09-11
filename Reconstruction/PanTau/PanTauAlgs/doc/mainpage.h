/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@mainpage PanTauAlgs Package

This package contains algorithms for PanTau.

@author Peter Wienemann <peter.wienemann@cern.ch>
@author Sebastian Fleischmann <Sebastian.Fleischmann@cern.ch>

@section PanTauAlgsIntro Introduction

 The PanTauAlgs package contains the following algorithms:

     - PanTau::TauSeedBuilderAlg : Seed builder for PanTau
     - PanTau::TauJetBuilderAlg : Tau jet builder for PanTau (takes PanTau::TauSeed and creates Analysis::TauJet)

 and the following tools:

     - PanTau::TauSeedTruthMatchTool : Tool to match TauSeeds to TruthTaus
     - PanTau::TauSeedTrackBasedTruthMatchTool : Tool to match TauSeeds to TruthTaus using track to truth information
     - PanTau::TauJetTruthMatchTool : Tool to match TauJets to TruthTaus
     - PanTau::TauDiscriminantTool : Gets discriminant variables from TMVA reader
     - PanTau::TauImpactParameterExtractionTool : extract TauFeature (track impact parameter)
     - PanTau::TauJetFeatureExtractionTool :  extract TauFeature (jet shapes)
     - PanTau::TauKinematicFitTool  : kinematic fit of eflow momenta to decay modes

     @section ExtrasPanTauA Extra Pages

      - @ref UsedPanTauAlgs
      - @ref requirementsPanTauAlgs
*/

/**
 * @page UsedPanTauAlgs Used Packages
 * @htmlinclude used_packages.html
 * */

/**
 * @page requirementsPanTauAlgs Requirements
 * @include requirements
 * */
