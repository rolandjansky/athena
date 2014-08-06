/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Jet_ROIGetterTool.cxx 272868 2010-01-12 17:10:09Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "Jet_ROIGetterTool.h"

namespace D3PD {

   Jet_ROIGetterTool::Jet_ROIGetterTool( const std::string& type, const std::string& name,
                                         const IInterface* parent )
      : ROIGetterTool< Jet_ROI >( type, name, parent, &LVL1_ROI::getJetROIs ) {

   }

} // namespace D3PD
