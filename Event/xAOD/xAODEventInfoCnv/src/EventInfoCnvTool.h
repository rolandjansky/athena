// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvTool.h 603159 2014-06-23 13:53:47Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

// Beam condition include(s):
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Local include(s):
#include "xAODEventInfoCnv/IEventInfoCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IEventInfoCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IEventInfoCnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 603159 $
    * $Date: 2014-06-23 15:53:47 +0200 (Mon, 23 Jun 2014) $
    */
   class EventInfoCnvTool : public AthAlgTool,
                            public virtual IEventInfoCnvTool {

   public:
      /// Regular AlgTool constructor
      EventInfoCnvTool( const std::string& type, const std::string& name,
                        const IInterface* parent );

      /// Function initialising the tool
      virtual StatusCode initialize();

      /// Function that fills an existing xAOD::EventInfo object with data
      virtual StatusCode convert( const EventInfo* aod,
                                  xAOD::EventInfo* xaod,
                                  bool pileUpInfo = false );

   private:
      /// Connection to the beam spot service
      ServiceHandle< IBeamCondSvc > m_beamCondSvc;
      /// Internal flag for the availability of the beam conditions service
      bool m_beamCondSvcAvailable;

   }; // class EventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
