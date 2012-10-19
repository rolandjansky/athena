/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @mainpage The TauTrackTools package
 * Tracking related tools for tau ID and validation.
 *
 * @author Sebastian.Fleischmann@cern.ch
 *
 * @section introductionTauTrackTools Introduction
 *
 * This package contains tools and algs related to tracking for tau ID and tau ID validation.
 *
 * The directory root contains macros to plot the histograms produced by the algs.
 *
 * @section TauTrackToolsOverview Class Overview
 *   The PanTauEvent package contains the following classes:
 *
 *     - TauID::TruthTauCreatorAlg : Create TruthTau from TruthParticle
 *     - TauID::TauTrackEfficiencyAlg : Produces histograms with tracking efficiency for tracks from tau decays
 *     - TauID::ITauJetTruthMatchTool: Interface for tools matching Analysis::TauJet to TauID::TruthTau
 *     - TauID::TauJetTruthMatchTool: Implementation of TauID::ITauJetTruthMatchTool using a delta R matching
 *     - TauID::TauJetTrackBasedTruthMatchTool: Implementation of TauID::ITauJetTruthMatchTool using a matching based on reconstructed and truth tracks
 *     - TauID::TrackBasedTruthMatchUtility: Utility for track based matching used by TauID::TauJetTrackBasedTruthMatchTool
 *     @section ExtrasTauTrackTools Extra Pages
 *
 *      - @ref UsedTauTrackTools
 *      - @ref requirementsTauTrackTools
 *       */

/**
 * @page UsedTauTrackTools Used Packages
 * @htmlinclude used_packages.html
 * */

/**
 * @page requirementsTauTrackTools Requirements
 * @include requirements
 * */
