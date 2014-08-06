/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EmTau_ROIGetterTool.cxx 272868 2010-01-12 17:10:09Z krasznaa $

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "EmTau_ROIGetterTool.h"

namespace D3PD {

   EmTau_ROIGetterTool::EmTau_ROIGetterTool( const std::string& type, const std::string& name,
                                           const IInterface* parent )
      : ROIGetterTool< EmTau_ROI >( type, name, parent, &LVL1_ROI::getEmTauROIs ) {

   }

} // namespace D3PD
