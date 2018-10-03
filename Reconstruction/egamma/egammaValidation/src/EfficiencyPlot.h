/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMAVALIDATION_EFFICIENCYPLOTS_H
#define EGAMMAVALIDATION_EFFICIENCYPLOTS_H

#include "TH1.h"
#include "TMath.h"
#include "GaudiKernel/ITHistSvc.h"
#include "IHistograms.h"

#include <TROOT.h>
#include <TStyle.h>

namespace egammaMonitoring{
  
  class EfficiencyPlot{
  public:

    EfficiencyPlot(std::string name, std::string folder,  ITHistSvc * &rootHistSvc ) ;
    ~EfficiencyPlot(){ };
    StatusCode divide(std::unique_ptr<IHistograms>  &pass,  std::unique_ptr<IHistograms> &total);
      
  private:
    std::string m_name;
    std::string m_folder;
    ITHistSvc*  m_rootHistSvc = nullptr;

  };
  
}

#endif


