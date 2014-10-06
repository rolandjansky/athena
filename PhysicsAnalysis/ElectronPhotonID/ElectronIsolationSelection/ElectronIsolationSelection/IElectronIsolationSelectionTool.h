// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IElectronIsolationSelectionTool.h 605167 2014-07-07 14:11:27Z morrisj $
#ifndef ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H
#define ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H

// Framework include(s):
#include "AsgTools/IAsgTool.h"
#include "PATCore/TAccept.h"

// EDM include(s):
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

namespace CP {

   ///
   /// @author John Morris <john.morris@cern.ch>
   ///
   /// $Revision: 605167 $
   /// $Date: 2014-07-07 16:11:27 +0200 (Mon, 07 Jul 2014) $
   ///
   class IElectronIsolationSelectionTool : public virtual asg::IAsgTool {

      /// Declare the interface that the class provides
      ASG_TOOL_INTERFACE( CP::IElectronIsolationSelectionTool )

   public:
      /// Decide whether the muon in question is a "good muon" or not
//       virtual const Root::TAccept& accept( const xAOD::Electron&  ) const = 0;

      virtual const Root::TAccept& acceptEtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const = 0;
      virtual const Root::TAccept& acceptPtcone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const = 0;
      virtual const Root::TAccept& acceptTopocone( const xAOD::Electron& ele , const float& coneSize, const unsigned int& Efficiency_required ) const = 0;
      
      // Switch between egamma PID working points
      virtual void SetPID_MediumPP() = 0;
      virtual void SetPID_TightPP() = 0; // default - no need to do anything post constructor
      virtual void SetPID_LH_Medium() = 0;
      virtual void SetPID_LH_Tight() = 0;
      virtual void SetPID_LH_VeryTight() = 0;      
      
      
   }; // class IElectronIsolationSelectionTool

} // namespace CP

#endif // ELECTRONISOLATIONSELECTION_IELECTRONISOLATIONSELECTIONTOOL_H
