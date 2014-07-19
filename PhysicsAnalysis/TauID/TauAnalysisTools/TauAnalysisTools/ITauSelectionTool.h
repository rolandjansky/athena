// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef ITAUONSELECTIONTOOL_H
#define ITAUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODTau/TauJet.h"

namespace TauAnalysisTools {

   /// Interface for tau selector tool
   ///
   class ITauSelectionTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( TauAnalysisTools::ITauSelectionTool )

   public:
      /// Decide whether the tauon in question is a "good tauon" or not
      virtual const Root::TAccept& accept( const xAOD::TauJet& xTau ) const = 0;

   }; // class ITauSelectionTool

} // namespace TauAnalysisTools

#endif // ITAUONSELECTIONTOOL_H
