/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFPSILAYERSUMMARYPROFILEHIT_H
#define AFPSILAYERSUMMARYPROFILEHIT_H

#include "AFP_Monitoring/AFPSiLayerSummaryProfileBase.h"
#include "AFP_Monitoring/IAFPSiLayerSummaryHit.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

template <typename T>
class AFPSiLayerSummaryProfileHit : public AFPSiLayerSummaryProfileBase, virtual public IAFPSiLayerSummaryHit
{
public:
  AFPSiLayerSummaryProfileHit (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (xAOD::AFPSiHit::*value)() const) :
    AFPSiLayerSummaryProfileBase (parentTool, monGrp, name, title),
    m_value(value)
  {}

  void fillHit (const std::string& binName, const xAOD::AFPSiHit& hit) override {
    m_profile->Fill(binName.data(), (hit.*m_value)());
  }
  
protected:
  T (xAOD::AFPSiHit::*m_value)() const;
};

// stop compiler from creating separete template for each instance of basic type
extern template class AFPSiLayerSummaryProfileHit<int>;
extern template class AFPSiLayerSummaryProfileHit<float>;
extern template class AFPSiLayerSummaryProfileHit<double>;

#endif
