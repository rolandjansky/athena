// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMuonEfficiencyToolExample.h 299754 2014-03-28 15:37:02Z krasznaa $
#ifndef CPANALYSISEXAMPLES_IMUONEFFICIENCYTOOL_H
#define CPANALYSISEXAMPLES_IMUONEFFICIENCYTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace CP {

   /// Simple interface for calculating "a muon efficiency"
   ///
   /// Meant to try to implement the TF3 recommendations in a realistic
   /// test case.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299754 $
   /// $Date: 2014-03-28 16:37:02 +0100 (Fri, 28 Mar 2014) $
   ///
   class IMuonEfficiencyToolExample : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IMuonEfficiencyToolExample )

   public:
      /// Get the "muon efficiency" as a return value
      virtual CorrectionCode getEfficiency( const xAOD::Muon& mu,
                                            float& eff ) = 0;
      /// Decorate the muon with its efficiency
      virtual CorrectionCode applyEfficiency( xAOD::Muon& mu ) = 0;

      /// Get the "muon scale factor" as a return value
      virtual CorrectionCode getScaleFactor( const xAOD::Muon& mu,
                                             float& sf ) = 0;
      /// Decorate the muon with its scale factor
      virtual CorrectionCode applyScaleFactor( xAOD::Muon& mu ) = 0;

   }; // class IMuonEfficiencyToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_IMUONEFFICIENCYTOOL_H
