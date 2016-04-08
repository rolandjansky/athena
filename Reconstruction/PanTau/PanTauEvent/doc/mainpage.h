/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @mainpage The PanTauEvent package
 * The EDM class package for PanTau.
 *
 * @author Sebastian.Fleischmann@cern.ch
 *
 * @section introductionPanTauEvent Introduction
 *
 * This package contains the data classes for PanTau.
 *
 * @section PanTauOverview Class Overview
 *   The PanTauEvent package contains the following classes:
 *
 *     - PanTau::TauSeed : Seed for PanTau
 *     - PanTau::TauSeedCollection : Collection of Seeds for PanTau
 *     - PanTau::SeedTruthMap : Map from TauSeed to TruthTau
 *     - PanTau::TauFeature : Store extracted tau features used in the ID
 *     - PanTau::CorrectedEflowMomentum : Helper class containing massive and fitted 4-momenta of eflow objects
 *     - PanTau::PanTauDetails : Adds PanTau specific data to a TauJet (inherits from TauDetails)
 *     - PanTau::CandTruthMap : Map from TauJet to TruthTau
 *
 *     @section ExtrasPanTauEvent Extra Pages
 *
 *      - @ref UsedPanTauEvent
 *      - @ref requirementsPanTauEvent
 *       */

/**
 * @page UsedPanTauEvent Used Packages
 * @htmlinclude used_packages.html
 * */

/**
 * @page requirementsPanTauEvent Requirements
 * @include requirements
 * */
