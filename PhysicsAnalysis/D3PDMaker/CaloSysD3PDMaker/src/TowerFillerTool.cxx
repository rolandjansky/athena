/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TowerFillerTool.cxx
 * Author: stephen
 * 
 * Created on August 26, 2011, 1:36 PM
 */

#include "TowerFillerTool.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD{

TowerFillerTool::TowerFillerTool(const std::string& type,
        const std::string& name,
        const IInterface* parent): BlockFillerTool<CaloTower>(type,name,parent)
{
  m_SaveConstituents = false;
  declareProperty("SaveNCellConstituents",m_SaveConstituents=true);
  TowerFillerTool::book().ignore();
}


StatusCode TowerFillerTool::book(){
  if (m_SaveConstituents) {
    CHECK(addVariable("tower_nCaloCells",m_nCaloCells));
    CHECK(addVariable("tower_CellWeights",m_cellWeights));
    CHECK(addVariable("tower_statusWord",m_statWord));
  }
  else {
    CHECK(addVariable("tower_pT",m_pT));
    CHECK(addVariable("tower_eta",m_eta));
    CHECK(addVariable("tower_phi",m_phi));
    CHECK(addVariable("tower_E",m_E));
  }
  return StatusCode::SUCCESS;
}



StatusCode TowerFillerTool::fill(const CaloTower& p){
    if(m_SaveConstituents){ 
        *m_nCaloCells=p.getNumberOfCells();
        *m_statWord=p.getRecoStatus().getStatusWord();
        Navigable<CaloCellContainer,double>::object_iter it=p.cell_begin();
        for(;it!=p.cell_end();++it){
            double weight=p.getCellWeight(it);
            m_cellWeights->push_back(weight);            
        }
    }
    else{
        *m_pT=p.pt();
        *m_eta=p.eta();
        *m_phi=p.phi();
        *m_E=p.energy();
    }
    
    return StatusCode::SUCCESS;
}

}
