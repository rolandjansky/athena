// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRUTHRIVETTOOLS_HIGGSTRUTHCATEGORYTOOL_H
#define TRUTHRIVETTOOLS_HIGGSTRUTHCATEGORYTOOL_H 1

// ASG include(s):
#include "AsgTools/AsgTool.h"

// Rivet include(s):
#include "Rivet/AnalysisHandler.hh"

// Local include(s):
#include "TruthRivetTools/IHiggsTruthCategoryTool.h"
#include "TruthRivetTools/HiggsTemplateCrossSections.h"

/**
 * Dual-use tool for a Rivet routine for classifying MC events according to
 * the Higgs template cross section categories
 *
 * @author Jim Lacey (Carleton University) <james.lacey@cern.ch,jlacey@physics.carleton.ca>
 */
class HiggsTruthCategoryTool : public asg::AsgTool, 
                               public virtual IHiggsTruthCategoryTool {

public:
   /// Create an "Athena constructor" for the tool
   ASG_TOOL_CLASS( HiggsTruthCategoryTool, IHiggsTruthCategoryTool )

   /// AsgTool constructor
   HiggsTruthCategoryTool( const std::string& name );

   /// @name Functions inherited from AsgTool
   /// @{

   /// Function initialising the tool
   StatusCode initialize() override;
   /// Function finalising the tool
   StatusCode finalize() override;

   /// @}

   /// @name Function(s) inherited from IHiggsTruthCategoryTool
   /// @{

   HTXS::HiggsClassification
   getHiggsTruthCategoryObject( const HepMC::GenEvent& HepMCEvent ) override;

   /// @}

private:
   /// @name Tool properties
   /// @{

   /// Flag for writing out data during finalisation
   bool m_outHistos;
   /// Higgs production mode to use
   int m_prodMode;

   /// @}

   /// @name Status flag(s)
   /// @{

   /// Flag showing whether the internal analysis handler was initialised
   bool m_isInitialized;

   /// @}

   /// @name Helper tool(s)
   /// @{

   /// The analysis handler object
   Rivet::AnalysisHandler m_anaHandler;
   /// The analysis to run
   Rivet::HiggsTemplateCrossSections m_higgsTCS;

   /// @}

}; // class HiggsTruthCategoryTool

#endif // !HIGGSTRUTHCLASSIFIER_HIGGSTRUTHCATEGORYTOOL_H
