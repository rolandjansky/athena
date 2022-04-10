// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
#define XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H

// Gaudi/Athena include(s):
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
#include "StoreGate/ReadCondHandleKey.h"
#endif // not XAOD_ANALYSIS or GENERATIONBASE

#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
// Beam condition include(s):
#include "BeamSpotConditionsData/BeamSpotData.h"
#endif // not XAOD_ANALYSIS or GENERATIONBASE

#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
// Luminosity include(s):
#include "LumiBlockData/LuminosityCondData.h"
#endif // not XAOD_ANALYSIS or SIMULATIONBASE or GENERATIONBASE

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
#if !defined(XAOD_ANALYSIS) && !defined(GENERATIONBASE)
      SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
      Gaudi::Property<bool> m_disableBeamSpot{ this, "DisableBeamSpot", false, "disable beamspot" };
#endif // not XAOD_ANALYSIS or GENERATIONBASE

#if !defined(XAOD_ANALYSIS) && !defined(SIMULATIONBASE) && !defined(GENERATIONBASE)
      SG::ReadCondHandleKey<LuminosityCondData> m_lumiDataKey
      { this, "LumiDataKey", "", "SG key for luminosity data" };
#endif // not XAOD_ANALYSIS or SIMULATIONBASE or GENERATIONBASE

      /// Internal flag for the availability of the beam spot information
      bool m_beamSpotInformationAvailable{};

   }; // class EventInfoCnvTool

} // namespace xAODMaker

#endif // XAODEVENTINFOCNV_EVENTINFOCNVTOOL_H
