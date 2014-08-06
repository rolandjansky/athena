/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_ROIGetterTool.cxx 272868 2010-01-12 17:10:09Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "Muon_ROIGetterTool.h"

namespace D3PD {

   Muon_ROIGetterTool::Muon_ROIGetterTool( const std::string& type, const std::string& name,
                                           const IInterface* parent )
      : ROIGetterTool< Muon_ROI >( type, name, parent, &LVL1_ROI::getMuonROIs ) {

   }

} // namespace D3PD
