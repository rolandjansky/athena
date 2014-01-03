// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMuonEFInfoObjectSourceFillerTool.h 514304 2012-08-20 08:37:38Z krasznaa $
#ifndef TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTSOURCEFILLERTOOL_H
#define TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTSOURCEFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/TrigMuonEFInfo.h"

// D3PDMaker include(s):
#include "TriggerD3PDMaker/TriggerObjectSourceFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool for flagging EF muons based on their "source"
    *
    *         This tool is mostly used to differentiate between
    *         the EF muon objects created by the RoI based chains
    *         and the full-scan one(s).
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 514304 $
    * $Date: 2012-08-20 10:37:38 +0200 (Mon, 20 Aug 2012) $
    */
   class TrigMuonEFInfoObjectSourceFillerTool :
      public TriggerObjectSourceFillerTool< TrigMuonEFInfo > {

   public:
      /// Regular AlgTool constructor
      TrigMuonEFInfoObjectSourceFillerTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent );

   }; // class TrigMuonEFInfoObjectSourceFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_TRIGMUONEFINFOOBJECTSOURCEFILLERTOOL_H
