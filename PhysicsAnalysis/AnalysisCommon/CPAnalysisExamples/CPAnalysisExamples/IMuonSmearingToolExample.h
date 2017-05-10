// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMuonSmearingToolExample.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef CPANALYSISEXAMPLES_IMUONSMEARINGTOOL_H
#define CPANALYSISEXAMPLES_IMUONSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

// Local include(s):
#include "PATInterfaces/CorrectionCode.h"

namespace CP {

   /// Simple interface for calibrating/correcting muons
   ///
   /// Following the design principles outlined in the TF3 recommendations.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class IMuonSmearingToolExample : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IMuonSmearingToolExample )

   public:
      /// Apply the correction on a modifyable object
      virtual CorrectionCode applyCorrection( xAOD::Muon& mu ) = 0;
      /// Create a corrected copy from a constant muon
      virtual CorrectionCode correctedCopy( const xAOD::Muon& input,
                                            xAOD::Muon*& output ) = 0;

   }; // class IMuonSmearingToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_IMUONSMEARINGTOOL_H
