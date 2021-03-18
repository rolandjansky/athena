/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// EventInfoPixelDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H
#define DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODTracking/TrackMeasurementValidationContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class EventInfoPixelDecorator : public ExpressionParserUser<AthAlgTool>, public IAugmentationTool {
    public: 
      EventInfoPixelDecorator(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
      Gaudi::Property<std::string> m_selectionString
         { this, "SelectionString", "" , "" };

      mutable std::atomic<unsigned int> m_ntot {};
      mutable std::atomic<unsigned int> m_npass {};

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
         { this, "EventInfoKey", "EventInfo", ""};
      Gaudi::Property<std::string> m_decorationPrefix
         { this, "DecorationPrefix", "", ""};
      SG::ReadHandleKey<xAOD::TrackMeasurementValidationContainer> m_pixelKey
         { this, "TrackMeasurementValidationKey", "PixelClusters", ""};

      enum EIntDecor {kperModuleMultiplicity,
                      klayer,
                      keta_module,
                      kphi_module,
                      kNIntDecor};
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_intDecorKeys;
  };
}

#endif // DERIVATIONFRAMEWORK_EVENTINFOPIXELDECORATOR_H
