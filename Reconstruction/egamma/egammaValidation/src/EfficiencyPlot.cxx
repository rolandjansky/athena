/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "EfficiencyPlot.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/AnaToolHandle.h"

namespace egammaMonitoring {

  EfficiencyPlot::EfficiencyPlot(std::string name, std::string folder, ITHistSvc * &rootHistSvc ) :
    m_name(std::move(name)),
    m_folder(std::move(folder)),
    m_rootHistSvc(rootHistSvc) {

  }



  StatusCode EfficiencyPlot::divide( IHistograms *pass,  IHistograms* total ) {

    gROOT->ForceStyle();
    gStyle->SetOptStat(0);

    for ( const auto &p : pass->histoMap ){
      TH1D *efficiency = (TH1D*) p.second->Clone(Form("%s_%s",m_name.c_str(),p.first.c_str()));


      efficiency->Divide(p.second, total->histoMap[p.first], 1, 1, "B");
      efficiency->GetYaxis()->SetTitle("Efficiency");
      efficiency->GetYaxis()->SetRangeUser(0,1.1);
      ATH_CHECK(m_rootHistSvc->regHist(Form("%sEff_%s", m_folder.c_str(),  p.first.c_str()), efficiency));

    }


    return StatusCode::SUCCESS;



  } //


} // namespace
