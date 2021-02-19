/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
// Author: M. T. Anthony
#ifndef PFO_FE_COMPARISONPLOTS_H
#define PFO_FE_COMPARISONPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODPFlow/FlowElementContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "StoreGate/ReadHandleKey.h"
#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
// Local include(s) for msg:
#include "AsgMessaging/AsgMessaging.h"

//tool include (to actually make the plots)
#include "PFOHistUtils/PFO_FE_Comparison_PlotMaker.h"
/*
This class is a factory class to generate comparison plots between PFO and Flow Elements, depending on if you want plots for neutral/charged objects
 */
class PFO_FE_ComparisonPlots : public PlotBase, public asg::AsgMessaging {
public:
  PFO_FE_ComparisonPlots(PlotBase* pParent, std::string sDir, std::string sPFOContainerName, std::string sFEContainerName, bool isNeutral); // constructor


 private:
  bool Match(const xAOD::PFO* pfo, const xAOD::FlowElement* fe); //  matching method to be called inside MatchAndFill (determined by if its neutral or not)
//PFO::PFO_FE_ComparisonPlots;
  void Fill(const xAOD::PFO* pfo, const xAOD::FlowElement* fe);
  bool m_isNeutral;
 PFO::PFO_FE_Comparison_PlotMaker m_PlotMaker;



 public: // since this method depends on the above, define afterward
 
 void MatchAndFill(SG::ReadHandle<xAOD::FlowElementContainer>& fecont, SG::ReadHandle<xAOD::PFOContainer>& pfocont);

};
#endif
