/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_SHOWERSHAPESHISTOGRAMS_H
#define EGAMMAVALIDATION_SHOWERSHAPESHISTOGRAMS_H

#include "GaudiKernel/ITHistSvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "TH1.h"

namespace egammaMonitoring{

  class ShowerShapesHistograms{
  public:

    // Histos
    ShowerShapesHistograms(std::string name,
                   std::string title,
                   std::string folder,
                   ITHistSvc * &rootHistSvc
    ) :
      m_name(name),
      m_title(title),
      m_folder(folder),
      m_rootHistSvc(rootHistSvc) {}

    std::map<std::string, TH1D* > histoMap;
    StatusCode initializePlots();
    void fill(const xAOD::Egamma& egamma);

  protected:
    std::string m_name;
    std::string m_title;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc =  nullptr;

    float m_eta2, m_rhad, m_rhad1, m_hadrleak, m_Reta, m_Rphi, m_shweta2, m_Eratio, m_DeltaE, m_frac_f1, m_shfside, m_shwtots1, m_shws3;

       
    
  };

}

#endif
