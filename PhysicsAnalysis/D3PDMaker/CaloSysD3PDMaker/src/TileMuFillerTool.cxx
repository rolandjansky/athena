/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileMuFillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 12, 2012, 3:59 PM
 */

#include "TileMuFillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"

using namespace std;

namespace D3PD{

TileMuFillerTool::TileMuFillerTool(const string& type, 
        const string& name, const IInterface* parent):
        BlockFillerTool<TileMu>(type,name,parent)
{
  TileMuFillerTool::book().ignore(); // Avoid coverity warnings
}

TileMuFillerTool::~TileMuFillerTool() {
}

StatusCode TileMuFillerTool::book(){
    CHECK(addVariable("Eta",m_eta));
    CHECK(addVariable("Phi",m_phi));
    CHECK(addVariable("EnergyDep0",m_energy0));
    CHECK(addVariable("EnergyDep1",m_energy1));
    CHECK(addVariable("EnergyDep2",m_energy2));
    CHECK(addVariable("EnergyDepNeighbors",m_energy3));
    CHECK(addVariable("Quality",m_quality));
    
    return StatusCode::SUCCESS;
}

StatusCode TileMuFillerTool::fill(const TileMu& p){
    *m_eta=p.eta();
    *m_phi=p.phi();
    *m_energy0=p.enedep()[0];
    *m_energy1=p.enedep()[1];
    *m_energy2=p.enedep()[2];
    *m_energy3=p.enedep()[3];
    
    return StatusCode::SUCCESS;
}

}
