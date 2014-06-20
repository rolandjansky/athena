/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/MbtsLvl2WordFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigCaloEvent/TrigT2MbtsBitsContainer.h"

namespace TrigMbD3PD {

MbtsLvl2WordFillerTool::MbtsLvl2WordFillerTool (const std::string& type,
					const std::string& name,
					const IInterface* parent)
  : MbtsLvl2WordFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode MbtsLvl2WordFillerTool::initialize(){

  CHECK( MbtsLvl2WordFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode MbtsLvl2WordFillerTool::book()
{

  CHECK( addVariable ("nElements", m_nElements) );
  CHECK( addVariable ("word", m_mbtsWord) );
  CHECK( addVariable ("ntimes", m_trigger_ntimes) );
  CHECK( addVariable ("times", m_triggerTimes) );

  return StatusCode::SUCCESS;
}

StatusCode MbtsLvl2WordFillerTool::fill (const MbtsLvl2WordFillerObjectType& t2MbtsBits)
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

void MbtsLvl2WordFillerTool::clearData(){
  
  *m_nElements = 0;
  *m_mbtsWord = 0;
  *m_trigger_ntimes = 0;

  m_triggerTimes->clear();
}

} // namespace TrigMbD3PD
