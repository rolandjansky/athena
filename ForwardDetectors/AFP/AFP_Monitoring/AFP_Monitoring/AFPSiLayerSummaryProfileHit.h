/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#ifndef AFPSILAYERSUMMARYPROFILEHIT_H
#define AFPSILAYERSUMMARYPROFILEHIT_H

#include "AFP_Monitoring/AFPSiLayerSummaryProfileBase.h"
#include "AFP_Monitoring/IAFPSiLayerSummaryHit.h"
#include "AFP_Monitoring/IAFPSiLayerMonitor.h"

#include <xAODForward/AFPSiHit.h>

/// Final class representing a summary distribution filled for every hit
template <typename T>
class AFPSiLayerSummaryProfileHit : public AFPSiLayerSummaryProfileBase, virtual public IAFPSiLayerSummaryHit
{
public:
   /// @brief Assigns values and calls base class constructor.
   /// 
   /// @param parentTool pointer to the main monitoring tool to which the summary distribution belongs
   /// @param monGrp configuration of the monitoring properties e.g. frequency of rebooking (per lumi-block, per run), directory where the distribution should be saved
   /// @param name name of the distribution, used as key when saving the distribution to a file
   /// @param title title of the distribution, displayed at the top of the distribution
   /// @param value pointer to the member method which calculates quantity used to fill the distribution
  AFPSiLayerSummaryProfileHit (AFPHitsMonitorTool* parentTool, const ManagedMonitorToolBase::MonGroup& monGrp, const std::string name, const std::string title, T (xAOD::AFPSiHit::*value)() const) :
    AFPSiLayerSummaryProfileBase (parentTool, monGrp, name, title),
    m_value(value)
  {}

  /// @copydoc IAFPSiLayerSummaryHit::fillHit()
  void fillHit (const std::string& binName, const xAOD::AFPSiHit& hit) override 
  {m_profile->Fill(binName.data(), (hit.*m_value)());}
  
protected:
  /// pointer to the member method which calculates quantity used to fill the distribution.
  T (xAOD::AFPSiHit::*m_value)() const;
};

// stop compiler from creating separete template for each instance of basic type
extern template class AFPSiLayerSummaryProfileHit<int>;
extern template class AFPSiLayerSummaryProfileHit<float>;
extern template class AFPSiLayerSummaryProfileHit<double>;

#endif
