/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/LucidRawDataFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "StoreGate/StoreGateSvc.h"

namespace TrigMbD3PD {

  LucidRawDataFillerTool::LucidRawDataFillerTool(const std::string& type,
						 const std::string& name,
						 const IInterface* parent)
    : LucidRawDataFillerBase (type, name, parent) {

    book().ignore(); // Avoid coverity warnings.
  }
  
  StatusCode LucidRawDataFillerTool::initialize(){

    CHECK(LucidRawDataFillerBase::initialize());
    
    return StatusCode::SUCCESS;
  }

  StatusCode LucidRawDataFillerTool::book() {
    
    CHECK(addVariable("word0",  m_word0));
    CHECK(addVariable("word1",  m_word1));
    CHECK(addVariable("word2",  m_word2));
    CHECK(addVariable("word3",  m_word3));
    CHECK(addVariable("word0p", m_word0p));
    CHECK(addVariable("word1p", m_word1p));
    CHECK(addVariable("word2p", m_word2p));
    CHECK(addVariable("word3p", m_word3p));
    CHECK(addVariable("word0n", m_word0n));
    CHECK(addVariable("word1n", m_word1n));
    CHECK(addVariable("word2n", m_word2n));
    CHECK(addVariable("word3n", m_word3n));
    CHECK(addVariable("status", m_status));

    CHECK(addVariable("totalHits", m_totalHits));
    CHECK(addVariable("ASideHits", m_aSideHits));
    CHECK(addVariable("CSideHits", m_cSideHits));
  
    return StatusCode::SUCCESS;
  }

  StatusCode LucidRawDataFillerTool::fill(const LucidRawDataFillerObjectType& rdo) {

    this->clearData();
    
    // This can actually happen sometimes in the pPb runs:
    if(rdo.size() != 1){
      REPORT_MESSAGE (MSG::WARNING) << "LucidRawDataContainer did not contain exactly 1 LucidRawData object.  Number found = "<<rdo.size();
      return StatusCode::SUCCESS;
    }

    *m_word0  = rdo[0]->getWord0();
    *m_word1  = rdo[0]->getWord1();
    *m_word2  = rdo[0]->getWord2();
    *m_word3  = rdo[0]->getWord3();
    *m_word0p = rdo[0]->getWord0p();
    *m_word1p = rdo[0]->getWord1p();
    *m_word2p = rdo[0]->getWord2p();
    *m_word3p = rdo[0]->getWord3p();
    *m_word0n = rdo[0]->getWord0n();
    *m_word1n = rdo[0]->getWord1n();
    *m_word2n = rdo[0]->getWord2n();
    *m_word3n = rdo[0]->getWord3n();
    *m_status = rdo[0]->getStatus();

    *m_aSideHits = rdo[0]->getNhitsPMTsideA();
    *m_cSideHits = rdo[0]->getNhitsPMTsideC();
    *m_totalHits = *m_aSideHits + *m_cSideHits;
  
    return StatusCode::SUCCESS;
  }

  void LucidRawDataFillerTool::clearData() {

    *m_word0  = -999;
    *m_word1  = -999;
    *m_word2  = -999;
    *m_word3  = -999;	
    *m_word0p = -999;
    *m_word1p = -999;
    *m_word2p = -999;
    *m_word3p = -999;	
    *m_word0n = -999;
    *m_word1n = -999;
    *m_word2n = -999;
    *m_word3n = -999;
    *m_status = -999;
  
    *m_totalHits = -999;
    *m_aSideHits = -999;
    *m_cSideHits = -999;
  }
}
