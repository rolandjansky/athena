// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonEfficiencyToolExample.h 299750 2014-03-28 12:39:15Z krasznaa $
#ifndef CPANALYSISEXAMPLES_MUONEFFICIENCYTOOL_H
#define CPANALYSISEXAMPLES_MUONEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/AsgTool.h"

// Local include(s):
#include "CPAnalysisExamples/IMuonEfficiencyToolExample.h"

namespace CP {

   /// Implementation for the "muon efficiency" provider tool
   ///
   /// Meant to try to implement the TF3 recommendations in a realistic
   /// test case.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299750 $
   /// $Date: 2014-03-28 13:39:15 +0100 (Fri, 28 Mar 2014) $
   ///
   class MuonEfficiencyToolExample : public virtual IMuonEfficiencyToolExample,
                                     public asg::AsgTool {

      /// Create a proper constructor for Athena
      ASG_TOOL_CLASS( MuonEfficiencyToolExample, CP::IMuonEfficiencyToolExample )

   public:
      /// Create a constructor for standalone usage
      MuonEfficiencyToolExample( const std::string& name );

      /// @name Function(s) implementing the asg::IAsgTool interface
      /// @{

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// @}

      /// @name Function(s) implementing the IMuonEfficiencyToolExample interface
      /// @{

      /// Get the "muon efficiency" as a return value
      virtual CorrectionCode getEfficiency( const xAOD::Muon& mu,
                                            float& eff );
      /// Decorate the muon with its efficiency
      virtual CorrectionCode applyEfficiency( xAOD::Muon& mu );

      /// Get the "muon scale factor" as a return value
      virtual CorrectionCode getScaleFactor( const xAOD::Muon& mu,
                                             float& sf );
      /// Decorate the muon with its scale factor
      virtual CorrectionCode applyScaleFactor( xAOD::Muon& mu );

      /// @}

   private:
      /// Some dummy property
      double m_dummyProperty;

   }; // class MuonEfficiencyToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_MUONEFFICIENCYTOOL_H
