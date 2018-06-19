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

/// Final class representing a summary distribution filled once per event
template <typename T>
class AFPSiLayerSummaryProfileEventEnd : public AFPSiLayerSummaryProfileBase, virtual public IAFPSiLayerSummaryEventEnd
{
public:
   /// @brief Assigns values and calls base class constructor.
   /// 
   /// @param parentTool pointer to the main monitoring tool to which the summary distribution belongs
   /// @param monGrp configuration of the monitoring properties e.g. frequency of rebooking (per lumi-block, per run), directory where the distribution should be saved
   /// @param name name of the distribution, used as key when saving the distribution to a file
   /// @param title title of the distribution, displayed at the top of the distribution
   /// @param value pointer to the member method which calculates quantity used to fill the distribution
  AFPSiLayerSummaryProfileEventEnd (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (IAFPSiLayerMonitor::*value)() const) :
    AFPSiLayerSummaryProfileBase (parentTool, monGrp, name, title),
    m_value(value)
  {}

   /// @copydoc IAFPSiLayerSummaryEventEnd::fillEventEnd()
  void fillEventEnd (const std::string& binName, const IAFPSiLayerMonitor* layer) override
  {m_profile->Fill(binName.data(), (layer->*m_value)());}
  
protected:
  /// pointer to the member method which calculates quantity used to fill the distribution.
  T (IAFPSiLayerMonitor::*m_value)() const;
};

// stop compiler from creating separete template for each instance of basic type
extern template class AFPSiLayerSummaryProfileEventEnd<int>;
extern template class AFPSiLayerSummaryProfileEventEnd<float>;
extern template class AFPSiLayerSummaryProfileEventEnd<double>;

#endif
