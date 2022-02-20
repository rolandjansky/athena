/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

/* 
 * File:   TileHitFillerTool.cxx
 * Author: Stephen Cole <stephen.cole@cern.ch>
 * 
 * Created on January 13, 2012, 1:11 PM
 */

#include "TileHitFillerTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/errorcheck.h"
#include "CaloIdentifier/TileID.h"

using namespace std;

namespace D3PD{

TileHitFillerTool::TileHitFillerTool(const string& type, 
        const string& name, const IInterface* parent):
  BlockFillerTool<TileHit>(type,name,parent),
  m_tileID(0)
{
  TileHitFillerTool::book().ignore(); // Avoid coverity warnings
}

/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode TileHitFillerTool::initialize()
{
  CHECK( detStore()->retrieve(m_tileID) );
  return StatusCode::SUCCESS;
}

StatusCode TileHitFillerTool::book(){
    CHECK(addVariable("energy",m_energy));
    CHECK(addVariable("time",m_time));
    CHECK(addVariable("detector",m_detector));
    CHECK(addVariable("side",m_side));
    CHECK(addVariable("sample",m_sample));
    CHECK(addVariable("pmt",m_pmt));
    CHECK(addVariable("eta",m_eta));
    CHECK(addVariable("phi",m_phi));
    return StatusCode::SUCCESS;
}

StatusCode TileHitFillerTool::fill(const TileHit& p){
    Identifier id=p.identify();
    
    int size=p.size();    
    for(int i=0;i<size;++i){
        m_energy->push_back(p.energy(i));
        m_time->push_back(p.time(i));
        
        m_detector->push_back(m_tileID->section(id));
        m_side->push_back(m_tileID->side(id));
        m_sample->push_back(m_tileID->sample(id));
        m_pmt->push_back(m_tileID->pmt(id));
        m_eta->push_back(m_tileID->tower(id));
        m_phi->push_back(m_tileID->module(id));
    }
    
    return StatusCode::SUCCESS;
}

}
