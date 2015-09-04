/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/TrtLvl2FillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigInDetEvent/TrigTrtHitCountsCollection.h"

namespace TrigMbD3PD {

TrtLvl2FillerTool::TrtLvl2FillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : TrtLvl2FillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode TrtLvl2FillerTool::initialize(){

  CHECK( TrtLvl2FillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode TrtLvl2FillerTool::book()
{

  CHECK( addVariable ("nElements", m_nElements) );
  CHECK( addVariable ("hTotBins", m_hTotBins) );
  CHECK( addVariable ("hTotMin", m_hTotMin) );
  CHECK( addVariable ("hTotMax", m_hTotMax) );

  CHECK( addVariable ("endcapC_contents", m_endcapC_contents) );
  CHECK( addVariable ("barrel_contents", m_barrel_contents) );
  CHECK( addVariable ("endcapA_contents", m_endcapA_contents) );

  return StatusCode::SUCCESS;
}

StatusCode TrtLvl2FillerTool::fill (const TrtLvl2FillerObjectType& trigTrtHitCountsCollection)
{

  this->clearData();

  TrigHisto1D endcapC;
  TrigHisto1D barrel;
  TrigHisto1D endcapA;

  *m_nElements = trigTrtHitCountsCollection.size();
  
  TrigTrtHitCountsCollection::const_iterator itr = trigTrtHitCountsCollection.begin();
  TrigTrtHitCountsCollection::const_iterator itr_end = trigTrtHitCountsCollection.end();

  if(itr != itr_end) {
  
    endcapC = (*itr)->endcapC();
    barrel = (*itr)->barrel();
    endcapA = (*itr)->endcapA();

    *m_hTotBins = endcapC.nbins_x();
    *m_hTotMin = endcapC.min_x();
    *m_hTotMax = endcapC.max_x();
    *m_endcapC_contents = endcapC.contents();
    *m_barrel_contents = barrel.contents();
    *m_endcapA_contents = endcapA.contents();
  }

  return StatusCode::SUCCESS;
}

void TrtLvl2FillerTool::clearData(){

  // Clear ntuple variables
  *m_nElements = 0;
  *m_hTotBins = 0;
  *m_hTotMin = 0;
  *m_hTotMax = 0;
  m_endcapC_contents->clear();
  m_barrel_contents->clear();
  m_endcapA_contents->clear();
}

} // namespace TrigMbD3PD
