/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page PanTauEvent_page PanTauEvent package
 * The EDM class package for PanTau.
 *
 * @author Sebastian.Fleischmann@cern.ch
 *
@section PanTauEvent_@section introductionPanTauEvent Introduction
 *
 * This package contains the data classes for PanTau.
 *
@section PanTauEvent_@section PanTauOverview Class Overview
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
@section PanTauEvent_@section ExtrasPanTauEvent Extra Pages
 *
 **/
