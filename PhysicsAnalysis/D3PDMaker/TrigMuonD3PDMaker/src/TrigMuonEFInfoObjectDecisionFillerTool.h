// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoObjectDecisionFillerTool.h 348854 2011-03-02 15:51:35Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTDECISIONFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTDECISIONFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"

// D3PD include(s):
#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool to record which triggers are passed by this object
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 348854 $
    * $Date: 2011-03-02 16:51:35 +0100 (Wed, 02 Mar 2011) $
    */
   class TrigMuonEFInfoObjectDecisionFillerTool
      : public TriggerObjectDecisionFillerTool< TrigMuonEFInfo > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoObjectDecisionFillerTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent );

      /// Function filling the D3PD variables
      virtual StatusCode fill( const TrigMuonEFInfo& muon );

   }; // class TrigMuonEFInfoObjectDecisionFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTDECISIONFILLERTOOL_H
