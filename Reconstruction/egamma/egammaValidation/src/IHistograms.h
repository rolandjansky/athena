/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_BASICHISTOGRAMS_H
#define EGAMMAVALIDATION_BASICHISTOGRAMS_H


#include "GaudiKernel/ITHistSvc.h"
#include "TH1D.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/xAODTruthHelpers.h"

#include "xAODEgamma/Egamma.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

namespace egammaMonitoring {

  class IHistograms {

  public:

    IHistograms();
    IHistograms(std::string name,
                   std::string title,
                   std::string folder,
                   ITHistSvc * &rootHistSvc
    ) :
      m_name(name),
      m_title(title),
      m_folder(folder),
      m_rootHistSvc(rootHistSvc) {}

    virtual StatusCode initializePlots() = 0;

    virtual void fill(const xAOD::IParticle&) = 0;

    std::map<std::string, TH1D* > histoMap;

  protected:
    std::string m_name;
    std::string m_title;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc =  nullptr;


  };

}


#endif //EGAMMAVALIDATION_BASICHISTOGRAM_H
