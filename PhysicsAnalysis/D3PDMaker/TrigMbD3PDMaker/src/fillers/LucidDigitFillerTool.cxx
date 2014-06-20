/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/LucidDigitFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

namespace TrigMbD3PD {

  LucidDigitFillerTool::LucidDigitFillerTool(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
    : LucidDigitFillerBase (type, name, parent) {

    book().ignore(); // Avoid coverity warnings.
  }
  
  StatusCode LucidDigitFillerTool::initialize(){
    
    CHECK(LucidDigitFillerBase::initialize());
    
    return StatusCode::SUCCESS;
  }
  
  StatusCode LucidDigitFillerTool::book() {
    
    CHECK(addVariable("tubeID",  m_tubeID));
    CHECK(addVariable("npePmt",  m_npePmt));
    CHECK(addVariable("npe",     m_npe));
    CHECK(addVariable("isHit",   m_isHit));
    CHECK(addVariable("isSideA", m_isSideA));
    CHECK(addVariable("isSideC", m_isSideC));
  
    return StatusCode::SUCCESS;
  }

  StatusCode LucidDigitFillerTool::fill(const LucidDigitFillerObjectType& rdo) {

    this->clearData();
    
    if (rdo.size() != 32) {

      REPORT_MESSAGE(MSG::WARNING) << "LucidDigitContainer did not contain exactly 32 LucidDigit object. Number found = " << rdo.size();
      
      return StatusCode::SUCCESS;
    }

    LUCID_DigitContainer::const_iterator itr     = rdo.begin();
    LUCID_DigitContainer::const_iterator itr_end = rdo.end();
    
    for(; itr != itr_end; ++itr) {
      
      m_tubeID->push_back ((*itr)->getTubeID());
      m_npePmt->push_back ((*itr)->getNpePmt());
      m_npe->push_back    ((*itr)->getNpe());
      m_isHit->push_back  ((*itr)->isHit());  
      m_isSideA->push_back((*itr)->isSideA());  
      m_isSideC->push_back((*itr)->isSideC());  
    }

    return StatusCode::SUCCESS;
  }

  void LucidDigitFillerTool::clearData() {
    
    m_tubeID->clear();
    m_npePmt->clear();
    m_npe->clear();
    m_isHit->clear();
    m_isSideA->clear();
    m_isSideC->clear();
  }
}
