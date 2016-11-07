// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// This is a copy from
// $Id: IMuonSmearingTool.h 299883 2014-03-28 17:34:16Z krasznaa $

#ifndef CPANALYSISEXAMPLES_IJETCALIBRATIONTOOL_H
#define CPANALYSISEXAMPLES_IJETCALIBRATIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODJet/Jet.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace CP {

   /// Simple interface for calibrating jets
   ///
   /// Following the design principles outlined in the TF3 recommendations.
   ///
   ///
   class IJetCalibrationTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IJetCalibrationTool )

   public:
      /// Apply the correction on a modifyable object
      virtual CorrectionCode applyCorrection( xAOD::Jet& mu ) = 0;
      /// Create a corrected copy from a constant jet
      virtual CorrectionCode correctedCopy( const xAOD::Jet& input,
                                            xAOD::Jet*& output ) = 0;

   }; // class IJetCalibrationTool

} // namespace CP

#endif // CPANALYSISEXAMPLES_IJETCALIBRATIONTOOL_H
