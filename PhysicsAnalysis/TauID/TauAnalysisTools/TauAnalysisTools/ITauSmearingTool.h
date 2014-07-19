// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ITAUSMEARINGTOOL_H
#define ITAUSMEARINGTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

// Local include(s):
#include "TauAnalysisTools/CorrectionCode.h"

namespace TauAnalysisTools {

   class ITauSmearingTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( TauAnalysisTools::ITauSmearingTool )

   public:
      /// Apply the correction on a modifyable object
      virtual TauAnalysisTools::CorrectionCode applyCorrection( xAOD::TauJet& xTau ) = 0;
      /// Create a corrected copy from a constant tau
      virtual TauAnalysisTools::CorrectionCode correctedCopy( const xAOD::TauJet& input,
                                            xAOD::TauJet*& output ) = 0;

   }; // class ITauSmearingTool

} // namespace TauAnalysisTools

#endif // ITAUSMEARINGTOOL_H
