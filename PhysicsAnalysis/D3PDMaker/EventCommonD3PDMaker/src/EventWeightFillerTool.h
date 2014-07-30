// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventWeightFillerTool.h 603418 2014-06-24 15:37:50Z ssnyder $
#ifndef EVENTCOMMOND3PDMAKER_EVENTWEIGHTFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_EVENTWEIGHTFILLERTOOL_H

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Block filler tool for the event weight from @c EventInfo
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 603418 $
    * $Date: 2014-06-24 17:37:50 +0200 (Tue, 24 Jun 2014) $
    */
  class EventWeightFillerTool : public BlockFillerTool< xAOD::EventInfo > {

   public:
      /// Standard AlgTool constructor
      EventWeightFillerTool( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Book variables for this block
      virtual StatusCode book();
      /// Fill the variables for this block
      virtual StatusCode fill( const xAOD::EventInfo& ei );

   private:
      /// Variable: MC event weight
      float* m_mc_event_weight;
   };

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_EVENTWEIGHTFILLERTOOL_H
