/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_CLUSTERSHISTOGRAMS_H
#define EGAMMAVALIDATION_CLUSTERSHISTOGRAMS_H

#include <utility>

#include "GaudiKernel/ITHistSvc.h"
#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/xAODTruthHelpers.h"
#include "EgammaAnalysisInterfaces/IAsgElectronLikelihoodTool.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

namespace egammaMonitoring{

  class ClusterHistograms{
  public:

    // Histos
    ClusterHistograms(std::string name,
                   std::string title,
                   std::string folder,
                   ITHistSvc * &rootHistSvc
    ) :
      m_name(std::move(name)),
      m_title(std::move(title)),
      m_folder(std::move(folder)),
      m_rootHistSvc(rootHistSvc) {}

    std::map<std::string, TH1D* > histoMap;
    std::map<std::string, TH2D* > histo2DMap;
    std::map<std::string, TH3D* > histo3DMap;

    StatusCode initializePlots();
    void fill(const xAOD::Egamma& egamma);
    void fill(const xAOD::Egamma& egamma, float mu);

  protected:
    std::string m_name;
    std::string m_title;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc =  nullptr;

    
  };

}

#endif
