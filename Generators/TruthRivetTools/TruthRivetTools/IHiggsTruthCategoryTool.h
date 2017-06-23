// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_IHIGGSTRUTHCATEGORYTOOL_H
#define TRUTHRIVETTOOLS_IHIGGSTRUTHCATEGORYTOOL_H 1

// ASG include(s):
#include "AsgTools/IAsgTool.h"

// Local include(s):
#include "TruthRivetTools/HiggsClassification.h"

// Forward declaration(s):
namespace HepMC {
   class GenEvent;
}

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
