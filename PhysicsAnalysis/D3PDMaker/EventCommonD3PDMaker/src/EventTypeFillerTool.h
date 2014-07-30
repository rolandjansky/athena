// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventTypeFillerTool.h 603418 2014-06-24 15:37:50Z ssnyder $
#ifndef EVENTCOMMOND3PDMAKER_EVENTTYPEFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_EVENTTYPEFILLERTOOL_H

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Block filler tool for the event type from @c EventInfo
    *
    *         The variables coming from the @c EventType object are handled
    *         by a separate filler tool, this one.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 603418 $
    * $Date: 2014-06-24 17:37:50 +0200 (Tue, 24 Jun 2014) $
    */
  class EventTypeFillerTool : public BlockFillerTool< xAOD::EventInfo > {

   public:
      /// Standard AlgTool constructor
      EventTypeFillerTool( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

      /// Book variables for this block
      virtual StatusCode book();
      /// Fill the variables for this block
    virtual StatusCode fill( const xAOD::EventInfo& ei );

   private:
      /// Variable: Is this a simulation event?
      bool* m_is_simulation;
      /// Variable: Is this a calibration event?
      bool* m_is_calibration;
      /// Variable: Is this a testbeam event?
      bool* m_is_testbeam;
   }; // class EventTypeFillerTool

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_EVENTTYPEFILLERTOOL_H
