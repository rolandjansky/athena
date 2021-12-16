/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTriggerFillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 13, 2012, 11:20 AM
 */

#include "TileTriggerFillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileTBID.h"

using namespace std;

namespace D3PD{

TileTriggerFillerTool::TileTriggerFillerTool(const string& type, 
        const string& name, const IInterface* parent):
  BlockFillerTool<TileTrigger>(type,name,parent),
  m_TT_ID(0)
{
  TileTriggerFillerTool::book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TileTriggerFillerTool::initialize()
{
  CHECK( detStore()->retrieve(m_TT_ID) );
  return StatusCode::SUCCESS;
}

StatusCode TileTriggerFillerTool::book()
{
  CHECK(addVariable("side",m_maxTowerSide));
  CHECK(addVariable("eta",m_maxTowerEta));
  CHECK(addVariable("phi",m_maxTowerPhi));
  CHECK(addVariable("pulse",m_maxTowerPulse));
  return StatusCode::SUCCESS;
}

StatusCode TileTriggerFillerTool::fill(const TileTrigger& p){
    vector<Identifier> towerid=p.GetBoardTowerID();
    vector<double> towerpulse=p.GetBoardTowerSum();
    
    for(size_t i=0;i<towerid.size();++i){
        m_maxTowerSide->push_back(m_TT_ID->pos_neg_z(towerid[i]));
        m_maxTowerEta->push_back(m_TT_ID->eta(towerid[i]));
        m_maxTowerPhi->push_back(m_TT_ID->phi(towerid[i]));
        m_maxTowerPulse->push_back(towerpulse[i]);
    }
    
    return StatusCode::SUCCESS;
}

}
