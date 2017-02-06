// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvTool.h 793565 2017-01-23 22:00:14Z leggett $
#ifndef XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#ifndef XAOD_ANALYSIS
// Beam condition include(s):
#include "InDetBeamSpotService/IBeamCondSvc.h"

// Luminosity include(s):
#include "LumiBlockComps/ILuminosityTool.h"
#endif

// xAOD include(s):
#include "xAODCnvInterfaces/IEventInfoCnvTool.h"

namespace xAODMaker {

   /**
    *  @short Implementation for the IEventInfoCnvTool interface
    *
    *         This is a simple tool that implements the converter function
    *         defined in IEventInfoCnvTool.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 793565 $
    * $Date: 2017-01-23 23:00:14 +0100 (Mon, 23 Jan 2017) $
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
                                  bool pileUpInfo = false,
                                  bool copyPileUpLinks = true ) const;

   private:
#ifndef XAOD_ANALYSIS
      /// Connection to the beam spot service
      ServiceHandle< IBeamCondSvc > m_beamCondSvc;
      /// Connection to the luminosity tool
      ToolHandle< ILuminosityTool > m_lumiTool;
#endif

      /// Internal flag for the availability of the beam conditions service
      bool m_beamCondSvcAvailable;

      
      /// Internal flag for the availability of the luminosity tool
      bool m_lumiToolAvailable;

   }; // class EventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
