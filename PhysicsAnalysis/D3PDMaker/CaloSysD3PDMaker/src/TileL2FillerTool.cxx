/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileL2FillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 12, 2012, 4:50 PM
 */

#include "TileL2FillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"

using namespace std;

namespace D3PD{

TileL2FillerTool::TileL2FillerTool(const string& type, 
        const string& name, const IInterface* parent):
        BlockFillerTool<TileL2>(type,name,parent)
{
  m_saveL2 = true;
  m_saveMuROD = true;
  TileL2FillerTool::book().ignore(); // Avoid coverity warnings

  declareProperty("SaveL2Details",    m_saveL2 = true);
  declareProperty("SaveMuRODDetails",    m_saveMuROD = false);
}

TileL2FillerTool::~TileL2FillerTool() {
}

StatusCode TileL2FillerTool::book(){
    if(m_saveL2){
        CHECK(addVariable("Ex",m_Ex));
        CHECK(addVariable("Ey",m_Ey));
        CHECK(addVariable("Ez",m_Ez));
        CHECK(addVariable("ScalarEt",m_sumEt));
        CHECK(addVariable("ScalarEz",m_sumEz));
        CHECK(addVariable("ScalarE",m_sumE));
    }
    if(m_saveMuROD){
        CHECK(addVariable("ROD_Eta",m_eta));
        CHECK(addVariable("ROD_Phi",m_phi));
        CHECK(addVariable("ROD_EnergyDep0",m_energy0));
        CHECK(addVariable("ROD_EnergyDep1",m_energy1));
        CHECK(addVariable("ROD_EnergyDep2",m_energy2));
        CHECK(addVariable("ROD_EnergyDepNeighbors",m_energy3));
        CHECK(addVariable("ROD_Quality",m_quality));
    }
    
    return StatusCode::SUCCESS;
}

StatusCode TileL2FillerTool::fill(const TileL2& p){
    if(m_saveL2){
        *m_Ex=p.Ex();
        *m_Ey=p.Ey();
        *m_Ez=p.Ez();
        *m_sumEt=p.sumEt();
        *m_sumEz=p.sumEz();
        *m_sumE=p.sumE();
    }
    if(m_saveMuROD){
        for(size_t i=0;i<p.NMuons();++i){
            m_eta->push_back(p.eta(i));
            m_phi->push_back(p.phi(i));
            m_energy0->push_back(p.enemu0(i));
            m_energy1->push_back(p.enemu1(i));
            m_energy2->push_back(p.enemu2(i));
            m_quality->push_back(p.qual(i));
        }        
    }
    
    return StatusCode::SUCCESS;
}

}
