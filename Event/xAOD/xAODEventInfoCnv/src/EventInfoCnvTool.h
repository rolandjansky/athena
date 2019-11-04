// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EventInfoCnvTool.h 793565 2017-01-23 22:00:14Z leggett $
#ifndef XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#ifndef XAOD_ANALYSIS
#ifndef SIMULATIONBASE
// Beam condition include(s):
#include "BeamSpotConditionsData/BeamSpotData.h"

// Luminosity include(s):
#include "LumiBlockData/LuminosityCondData.h"

#include "StoreGate/ReadCondHandleKey.h"
#endif
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
      virtual StatusCode initialize() override;

      /// Function that fills an existing xAOD::EventInfo object with data
      virtual StatusCode convert( const EventInfo* aod,
                                  xAOD::EventInfo* xaod,
                                  bool pileUpInfo = false,
                                  bool copyPileUpLinks = true,
                                  const EventContext& ctx = Gaudi::Hive::currentContext()) const override;

   private:
#ifndef XAOD_ANALYSIS
#ifndef SIMULATIONBASE
      /// Connection to the beam spot service
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

      SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey
      { this, "LumiDataKey", "", "SG key for luminosity data" };
#endif
#endif

      /// Internal flag for the availability of the beam conditions service
      bool m_beamCondSvcAvailable;

      
      /// Flag to disable beamspot service for AthenaMT migration purposes
      bool m_disableBeamSpot;

   }; // class EventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
