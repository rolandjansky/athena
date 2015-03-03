// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonFeatureObjectDecisionFillerTool.h 307534 2010-06-28 16:16:41Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_MUONFEATUREOBJECTDECISIONFILLERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATUREOBJECTDECISIONFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/MuonFeature.h"

// D3PD include(s):
#include "TriggerD3PDMaker/TriggerObjectDecisionFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool to record which triggers are passed by this object
    *
    * @author Takayuki Kanno
    *
    * $Revision: 307534 $
    * $Date: 2010-06-28 18:16:41 +0200 (Mon, 28 Jun 2010) $
    */
   class MuonFeatureObjectDecisionFillerTool
      : public TriggerObjectDecisionFillerTool< MuonFeature > {

   public:
      /// Regular AlgTool constructor
      MuonFeatureObjectDecisionFillerTool( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent );



   }; // class MuonFeatureObjectDecisionFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATUREOBJECTDECISIONFILLERTOOL_H
