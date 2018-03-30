/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFPSILAYERSUMMARYPROFILEEVENTEND_H
#define AFPSILAYERSUMMARYPROFILEEVENTEND_H

#include "AFP_Monitoring/AFPSiLayerSummaryProfileBase.h"
#include "AFP_Monitoring/IAFPSiLayerSummaryEventEnd.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"
#include "AFP_Monitoring/IAFPSiStationMonitor.h"
#include "AFP_Monitoring/AFPHitsMonitorTool.h"

template <typename T>
class AFPSiLayerSummaryProfileEventEnd : public AFPSiLayerSummaryProfileBase, virtual public IAFPSiLayerSummaryEventEnd
{
public:
  AFPSiLayerSummaryProfileEventEnd (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (IAFPSiLayerMonitor::*value)() const) :
    AFPSiLayerSummaryProfileBase (parentTool, monGrp, name, title),
    m_value(value)
  {}

  void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) override;
  
protected:
  T (IAFPSiLayerMonitor::*m_value)() const;
};

template <typename T>
void AFPSiLayerSummaryProfileEventEnd<T>::fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer)  {
  m_profile->Fill(binName.data(), (layer->*m_value)());
}


// stop compiler from creating separete template for each instance of basic type
extern template class AFPSiLayerSummaryProfileEventEnd<int>;
extern template class AFPSiLayerSummaryProfileEventEnd<float>;
extern template class AFPSiLayerSummaryProfileEventEnd<double>;

#endif
