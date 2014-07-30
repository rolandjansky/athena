// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventTrigInfoFillerTool.h 596731 2014-05-13 08:14:49Z wlampl $
#ifndef EVENTCOMMOND3PDMAKER_EVENTTRIGINFOFILLERTOOL_H
#define EVENTCOMMOND3PDMAKER_EVENTTRIGINFOFILLERTOOL_H

// STL include(s):
#include <vector>
#include <string>

// EDM include(s):
#include "xAODEventInfo/EventInfo.h"

// D3PDMaker include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

namespace D3PD {

   /**
    *  @short Tool writing the stream decisions to D3PDs
    *
    *         This tool can be used to write flags into the D3PD files that
    *         tell the users which trigger streams one particular event ended
    *         up in. This should make handling the overlaps between streams
    *         quite a bit simpler.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 596731 $
    * $Date: 2014-05-13 10:14:49 +0200 (Tue, 13 May 2014) $
    */
  class EventTrigInfoFillerTool : public BlockFillerTool< xAOD::EventInfo > {

   public:
      /// Standard AlgTool constructor
      EventTrigInfoFillerTool( const std::string& type,
                               const std::string& name,
                               const IInterface* parent );

      /// Book variables for this block
      virtual StatusCode book();
      /// Fill the variables for this block
    virtual StatusCode fill( const xAOD::EventInfo& ei );

   private:
      /// Function deciding if a given stream was passed
    static bool streamPassed( const std::vector< xAOD::EventInfo::StreamTag >& stags,
			      const std::string& sname );

      /// Streams to which this event was written
      std::vector< bool* > m_streams;

      /// Streams for which the decision should be stored
      std::vector< std::string > m_configStreams;

      /// Variable: Extended level 1 ID.
      unsigned int* m_l1id;

   }; // class EventTrigInfoFillerTool

} // namespace D3PD

#endif // EVENTCOMMOND3PDMAKER_EVENTTRIGINFOFILLERTOOL_H
