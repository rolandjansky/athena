/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TAUHISTUTILS_PARAMPLOTS_H
#define TAUHISTUTILS_PARAMPLOTS_H

#include "TrkValHistUtils/PlotBase.h"
#include "xAODBase/IParticle.h"

namespace Tau{

class ParamPlots:public PlotBase {
   public:      
      ParamPlots(PlotBase *pParent, std::string sDir, std::string sParticleType);
      ~ParamPlots();
      void fill(const xAOD::IParticle& prt);
      
      TH1* eta;
      TH1* phi;
      TH1* pt;
      
      TH1* eta_phi;
      TH1* eta_pt;
      
   private:
      void initializePlots();
      std::string m_sParticleType;
};

}

#endif // TRKVALHISTUTILS_PARAMPLOTS_H


