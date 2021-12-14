/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUDQA_PARAMPLOTS_H
#define TAUDQA_PARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODBase/IParticle.h"

namespace Tau{

class ParamPlots:public PlotBase {
   public:      
      ParamPlots(PlotBase *pParent, const std::string& sDir, std::string sParticleType);
      ~ParamPlots();
      void fill(const xAOD::IParticle& prt, float weight);
      
      TH1* eta;
      TH1* phi;
      TH1* pt;
      
      TH2* eta_phi;
      TH2* eta_pt;
      
   private:
      void initializePlots();
      std::string m_sParticleType;
};

}

#endif // TAUDQA_PARAMPLOTS_H


