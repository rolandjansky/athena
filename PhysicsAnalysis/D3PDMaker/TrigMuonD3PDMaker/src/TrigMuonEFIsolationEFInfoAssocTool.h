// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFIsolationEFInfoAssocTool.h 499444 2012-05-07 12:10:20Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOASSOCTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOASSOCTOOL_H

// Gaudi/Athena include(s):
#include "GaudiKernel/ToolHandle.h"

// Trigger include(s):
#include "TrigDecisionTool/TrigDecisionTool.h"

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFIsolation.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"

// D3PD include(s):
#include "D3PDMakerUtils/SingleAssociationTool.h"

namespace D3PD {

   /**
    * @short Tool "associating" TrigMuonEFIsolation objects to TrigMuonEFInfo ones
    * @author Takayuki
    *
    * $Revision: 499444 $
    * $Date: 2012-05-07 14:10:20 +0200 (Mon, 07 May 2012) $
    */
   class TrigMuonEFIsolationEFInfoAssocTool :
      public SingleAssociationTool< TrigMuonEFIsolation, TrigMuonEFInfo > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFIsolationEFInfoAssocTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

      /// Function initializing the tool
      virtual StatusCode initialize();

      /// Function performing the association
      virtual const TrigMuonEFInfo* get( const TrigMuonEFIsolation& muon );

   private:
      /// Use the trigger navigation to find the associated object?
      bool m_useTrigNav;

      /// Handle for the TrigDecisionTool
      ToolHandle< Trig::TrigDecisionTool > m_trigDec;

   }; // class TrigMuonEFIsolationEFInfoAssocTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFISOLATIONEFINFOASSOCTOOL_H
