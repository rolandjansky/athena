/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/BcmRdoFillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "InDetBCM_RawData/BCM_RDO_Collection.h"

namespace TrigMbD3PD {

BcmRdoFillerTool::BcmRdoFillerTool (const std::string& type,
				  const std::string& name,
				  const IInterface* parent)
  : BcmRdoFillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode BcmRdoFillerTool::initialize(){

  CHECK( BcmRdoFillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode BcmRdoFillerTool::book()
{
  CHECK( addVariable ("channel", m_bcm_channel) );
  CHECK( addVariable ("nhits", m_bcm_nhits) );
  CHECK( addVariable ("pulse1pos", m_bcm_pulse1pos) );
  CHECK( addVariable ("pulse1width", m_bcm_pulse1width) );
  CHECK( addVariable ("pulse2pos", m_bcm_pulse2pos) );
  CHECK( addVariable ("pulse2width", m_bcm_pulse2width) );
  CHECK( addVariable ("lvl1a", m_bcm_lvl1a) );
  CHECK( addVariable ("bcid", m_bcm_bcid) );
  CHECK( addVariable ("lvl1id", m_bcm_lvl1id) );

  return StatusCode::SUCCESS;
}

StatusCode BcmRdoFillerTool::fill (const BcmRdoFillerObjectType& bcm)
{
  int hit_count = 0;

  BCM_RDO_Collection::const_iterator hit_itr = bcm.begin();
  BCM_RDO_Collection::const_iterator hit_itr_end = bcm.end();
  for(;hit_itr != hit_itr_end; ++hit_itr) {
    if ((*hit_itr)->getPulse1Width()>0){
      //ATH_MSG_INFO ( "BCM hit: " ); 
      hit_count++;
      m_bcm_pulse1pos->push_back((*hit_itr)->getPulse1Position());
      m_bcm_pulse1width->push_back((*hit_itr)->getPulse1Width());
        if ((*hit_itr)->getPulse2Width()>0){
          hit_count++;
          m_bcm_pulse2pos->push_back((*hit_itr)->getPulse2Position());
          m_bcm_pulse2width->push_back((*hit_itr)->getPulse2Width());
	}
      m_bcm_lvl1a->push_back((*hit_itr)->getLVL1A());
      m_bcm_bcid->push_back((*hit_itr)->getBCID());
      m_bcm_lvl1id->push_back((*hit_itr)->getLVL1ID());
      m_bcm_channel->push_back(bcm.getChannel());
    }
  }
  *m_bcm_nhits = hit_count;

  return StatusCode::SUCCESS;
}


} // namespace TrigMbD3PD
