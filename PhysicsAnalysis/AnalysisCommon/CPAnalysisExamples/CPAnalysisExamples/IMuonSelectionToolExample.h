// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IMuonSelectionTool.h 299883 2014-03-28 17:34:16Z krasznaa $
#ifndef CPANALYSISEXAMPLES_IMUONSELECTIONTOOL_H
#define CPANALYSISEXAMPLES_IMUONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODMuon/Muon.h"

namespace CP {

   /// Interface for (a) muon selector tool(s)
   ///
   /// This is an example of how to define object selection in
   /// a tool.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 299883 $
   /// $Date: 2014-03-28 18:34:16 +0100 (Fri, 28 Mar 2014) $
   ///
   class IMuonSelectionToolExample : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IMuonSelectionToolExample )

   public:
      /// Decide whether the muon in question is a "good muon" or not
      virtual const Root::TAccept& accept( const xAOD::Muon& mu ) const = 0;

   }; // class IMuonSelectionToolExample

} // namespace CP

#endif // CPANALYSISEXAMPLES_IMUONSELECTIONTOOL_H
