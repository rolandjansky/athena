/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/MbtsLvl2FillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"

namespace TrigMbD3PD {

MbtsLvl2FillerTool::MbtsLvl2FillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
  : MbtsLvl2FillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode MbtsLvl2FillerTool::initialize(){

  CHECK( MbtsLvl2FillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode MbtsLvl2FillerTool::book()
{

  CHECK( addVariable ("nElements", m_nElements) );
  CHECK( addVariable ("energies", m_triggerEnergies) );
  CHECK( addVariable ("ntimes", m_trigger_ntimes) );
  CHECK( addVariable ("times", m_triggerTimes) );

  return StatusCode::SUCCESS;
}

StatusCode MbtsLvl2FillerTool::fill (const MbtsLvl2FillerObjectType& t2MbtsBits)
{

  this->clearData();

  *m_nElements = t2MbtsBits.size();

  TrigT2MbtsBitsContainer::const_iterator itr = t2MbtsBits.begin();
  TrigT2MbtsBitsContainer::const_iterator itr_end = t2MbtsBits.end();
	
  if(itr != itr_end) {
    //*m_mbtsWord = (*itr)->mbtsWord();   ////-------- FIX ME: just a temp solution for now!! 
    *m_triggerTimes = (*itr)->triggerTimes();
    *m_trigger_ntimes = m_triggerTimes->size();
  }

  return StatusCode::SUCCESS;
}

void MbtsLvl2FillerTool::clearData(){
  
  *m_nElements = 0;
  m_triggerEnergies->clear();
  *m_trigger_ntimes = 0;
  m_triggerTimes->clear();
}

} // namespace TrigMbD3PD
