/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_BDTINPUTPLOTS_H
#define TAUHISTUTILS_BDTINPUTPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "TauHistUtils/ParamPlots.h"
#include "xAODTau/TauDefs.h" 
#include "xAODTau/TauJet.h"
#include "xAODPFlow/versions/PFO_v1.h"

namespace Tau{
   
   class BDTinputPlots: public PlotBase {
   public:
      BDTinputPlots(PlotBase *pParent, std::string sDir, std::string sTauJetContainerName);
      virtual ~BDTinputPlots();
      void fill(const xAOD::TauJet& tau);
      // BDT output
      
      TH1* m_id_BDTJetScore;
      TH1* m_id_BDTEleScore;
      TH1* m_pt_eleBDTloose;
      TH1* m_pt_eleBDTmed;
      TH1* m_pt_eleBDTtight;
      TH1* m_pt_jetBDTloose;
      TH1* m_pt_jetBDTmed;
      TH1* m_pt_jetBDTtight;
      
   private:
      void initializePlots();
      std::string m_sTauJetContainerName;
   };
   
}

#endif
