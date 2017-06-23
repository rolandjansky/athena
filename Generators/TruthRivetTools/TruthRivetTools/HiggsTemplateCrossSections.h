// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_HIGGSTEMPLATECROSSSECTIONS_H
#define TRUTHRIVETTOOLS_HIGGSTEMPLATECROSSSECTIONS_H 1

// Rivet include(s):
#include "Rivet/Analysis.hh"
#include "Rivet/Event.hh"

// Local include(s):
#include "TruthRivetTools/HiggsClassification.h"
#include "TruthRivetTools/Enums.h"

namespace Rivet {

   /// Rivet routine for classifying MC events according to the Higgs template
   /// cross section categories
   ///
   /// @author Jim Lacey (DESY) <james.lacey@cern.ch,jlacey@desy.de>
   /// @author Dag Gillberg (Carleton University) <dag.gillberg@cern.ch>
   ///
   class HiggsTemplateCrossSections : public Analysis {

   public:
      /// Constructor
      HiggsTemplateCrossSections();

      /// @name Utility methods
      /// Methods to identify the Higgs boson and
      /// associated vector boson and to build jets
      /// @{

      /// Returns the classification object with the error code set.
      /// Prints an warning message, and keeps track of number of errors
      HiggsClassification
      error( HiggsClassification& cat, HTXS::ErrorCode err, 
             std::string msg = "", int NmaxWarnings = 20 );

      /// @}

      /// Main classificaion method.
      HiggsClassification classifyEvent( const Event& event,
                                         const HTXS::HiggsProdMode prodMode );

      /// @name Default Rivet analysis methods and steering methods
      /// @{

      /// Sets the Higgs production mode
      void setHiggsProdMode( HTXS::HiggsProdMode prodMode );

      /// Default Rivet Analysis::init method
      ///
      /// Booking of histograms, initializing Rivet projection
      /// Extracts Higgs production mode from shell variable if not set
      /// manually using setHiggsProdMode
      void init() override;

      /// Perform the per-event analysis
      void analyze( const Event& event ) override;

      /// Finalize the analysis
      void finalize() override;

      /// @}

   private:
      /// @name Functions used during initialisation and finalisation
      /// @{

      void printClassificationSummary();
      void initializeHistos();

      /// @}

      double m_sumw;
      HTXS::HiggsProdMode m_HiggsProdMode;
      std::map<HTXS::ErrorCode,size_t> m_errorCount;
      Histo1DPtr hist_stage0;
      Histo1DPtr hist_stage1_pTjet25, hist_stage1_pTjet30;
      Histo1DPtr hist_pT_Higgs, hist_y_Higgs;
      Histo1DPtr hist_pT_V, hist_pT_jet1;
      Histo1DPtr hist_deltay_jj, hist_dijet_mass, hist_pT_Hjj;
      Histo1DPtr hist_Njets25, hist_Njets30;

   }; // class HiggsTemplateCrossSections

   // the PLUGIN only needs to be decleared when running standalone Rivet
   // and causes compilation / linking issues if included in Athena / RootCore
   //check for Rivet environment variable RIVET_ANALYSIS_PATH
#ifdef RIVET_ANALYSIS_PATH
   // The hook for the plugin system
   DECLARE_RIVET_PLUGIN( HiggsTemplateCrossSections );
#endif

} // namespace Rivet

#endif // TRUTHRIVETTOOLS_HIGGSTEMPLATECROSSSECTIONS_H
