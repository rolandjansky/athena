// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvTool.h 670837 2015-05-29 10:17:08Z krasznaa $
#ifndef XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

// Beam condition include(s):
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Luminosity include(s):
#include "LumiBlockComps/ILuminosityTool.h"

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
    * $Revision: 670837 $
    * $Date: 2015-05-29 12:17:08 +0200 (Fri, 29 May 2015) $
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

      /// Connection to the luminosity tool
      ToolHandle< ILuminosityTool > m_lumiTool;
      /// Internal flag for the availability of the luminosity tool
      bool m_lumiToolAvailable;

   }; // class EventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
