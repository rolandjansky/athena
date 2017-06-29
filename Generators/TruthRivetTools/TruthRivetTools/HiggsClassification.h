// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_HIGGSCLASSIFICATION_H
#define TRUTHRIVETTOOLS_HIGGSCLASSIFICATION_H

// Rivet include(s):
#include <Rivet/Particle.hh>
#include <Rivet/Jet.hh>

// Local include(s):
#include "TruthRivetTools/Enums.h"

/// Rivet namespace
namespace Rivet {

   /// Structure holding information about the current event
   ///
   /// Four-momenta and event classification according to the
   /// Higgs Template Cross Section
   ///
   struct HiggsClassification {

      /// Higgs production mode
      HTXS::HiggsProdMode prodMode;

      /// The Higgs boson
      Rivet::Particle higgs;
      /// Vector boson produced in asscoiation with the Higgs
      Rivet::Particle V;
      /// The four momentum sum of all stable decay products orignating from
      /// the Higgs boson
      Rivet::FourMomentum p4decay_higgs;
      /// The four momentum sum of all stable decay products orignating from
      /// the vector boson in associated production
      Rivet::FourMomentum p4decay_V;
      /// Jets built ignoring Higgs decay products and leptons from V decays,
      /// pT thresholds at 25 GeV and 30 GeV
      Rivet::Jets jets25, jets30;

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

   }; // class HiggsClassification

} // namespace Rivet

#endif // TRUTHRIVETTOOLS_HIGGSCLASSIFICATION_H
