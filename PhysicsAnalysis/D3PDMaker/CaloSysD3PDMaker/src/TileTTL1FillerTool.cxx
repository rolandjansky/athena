/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileTTL1FillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 13, 2012, 10:27 AM
 */

#include "TileTTL1FillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "CaloIdentifier/TileTBID.h"

using namespace std;

namespace D3PD{

TileTTL1FillerTool::TileTTL1FillerTool(const string& type, 
        const string& name, const IInterface* parent):
  BlockFillerTool<TileTTL1>(type,name,parent),
  m_tileTBID(0),
  m_TT_ID(0)
{
  TileTTL1FillerTool::book().ignore(); // Avoid coverity warnings
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TileTTL1FillerTool::initialize()
{
  CHECK( detStore()->retrieve(m_TT_ID) );
  CHECK( detStore()->retrieve(m_tileTBID) );
  return StatusCode::SUCCESS;
}


StatusCode TileTTL1FillerTool::book()
{
  CHECK(addVariable("side",m_side));
  CHECK(addVariable("eta",m_eta));
  CHECK(addVariable("phi",m_phi));
  CHECK(addVariable("samples",m_samples));
  return StatusCode::SUCCESS;
}

StatusCode TileTTL1FillerTool::fill(const TileTTL1& p){
    Identifier id=p.TTL1_ID();
    
    if(m_tileTBID->is_tiletb(id)){
        *m_side=m_tileTBID->type(id);
        *m_eta=m_tileTBID->eta(id);
        *m_phi=m_tileTBID->module(id);
    }
    else{
        *m_side=m_TT_ID->pos_neg_z(id);
        *m_eta=m_TT_ID->eta(id);
        *m_phi=m_TT_ID->phi(id);
    }
    
    *m_samples=p.fsamples();
    return StatusCode::SUCCESS;
}

}
