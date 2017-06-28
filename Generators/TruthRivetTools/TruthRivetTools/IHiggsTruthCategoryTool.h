// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_IHIGGSTRUTHCATEGORYTOOL_H
#define TRUTHRIVETTOOLS_IHIGGSTRUTHCATEGORYTOOL_H 1

// ROOT include(s):
#include <TLorentzVector.h>

// ASG include(s):
#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "TruthRivetTools/Enums.h"

// Forward declaration(s):
namespace HepMC {
   class GenEvent;
}

/// Higgs Template Cross Section namespace
namespace HTXS {

   /// Structure holding information about the current event
   ///
   /// Four-momenta and event classification according to the
   /// Higgs Template Cross Section.
   ///
   struct HiggsClassification {

      /// Higgs production mode
      HTXS::HiggsProdMode prodMode;

      /// The Higgs boson
      TLorentzVector higgs;
      /// The Higgs boson decay products
      TLorentzVector p4decay_higgs;
      /// Associated vector bosons
      TLorentzVector V;
      /// The V-boson decay products
      TLorentzVector p4decay_V;
      /// Jets are built ignoring Higgs decay products and leptons from V decays
      /// jets with pT > 25 GeV and 30 GeV
      std::vector< TLorentzVector > jets25, jets30;

      /// Stage-0 HTXS event classifcation
      /// See: https://cds.cern.ch/record/2138079
      HTXS::Stage0::Category stage0_cat;
      /// Stage-1 HTXS event classifcation
      /// See: https://cds.cern.ch/record/2138079
      HTXS::Stage1::Category stage1_cat_pTjet25GeV;
      /// Stage-1 HTXS event classifcation
      /// See: https://cds.cern.ch/record/2138079
      HTXS::Stage1::Category stage1_cat_pTjet30GeV;

      /// Error code: classification was succesful or some error occured
      HTXS::ErrorCode errorCode;

   }; // struct HiggsClassification

} // namespace HTXS

/**
 * Dual-use tool interface for Rivet routine for classifying MC events according
 * to the Higgs template cross section categories.
 *
 * @authors Jim Lacey (Carleton University) <james.lacey@cern.ch,jlacey@physics.carleton.ca>
 */
class IHiggsTruthCategoryTool : public virtual asg::IAsgTool {

public:
   /// Declare the interface to Athena
   ASG_TOOL_INTERFACE( IHiggsTruthCategoryTool )

   /// Get the classification for the specifed generated event
   virtual HTXS::HiggsClassification
   getHiggsTruthCategoryObject( const HepMC::GenEvent& HepMCEvent ) = 0;

}; // class IHiggsTruthCategoryTool

#endif // !HIGGSTRUTHCLASSIFIER_IHIGGSTRUTHCATEGORYTOOL_H
