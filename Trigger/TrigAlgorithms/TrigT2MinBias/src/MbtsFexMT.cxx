/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#include "MbtsFexMT.h"

MbtsFexMT::MbtsFexMT(const std::string& name, ISvcLocator* pSvcLocator) :
AthReentrantAlgorithm(name, pSvcLocator)
{
}

MbtsFexMT::~MbtsFexMT()
{
}

StatusCode MbtsFexMT::initialize()
{
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  return StatusCode::SUCCESS;
}
StatusCode MbtsFexMT::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode MbtsFexMT::execute(const EventContext& context) const
{
  SG::ReadHandle<TileTBID> MbtsHelper(m_tileTBID, context );



  // Pack values into vectors.
  for( m_itt=m_itBegin; m_itt != m_itEnd; ++m_itt ){
    Identifier id=(*m_itt)->ID();

    // MBTS Id type is  "side"  +/- 1
    int type_id = m_tileTBID->type(id);

    // MBTS Id channel is "eta"  0-1   zero is closer to beam pipe
    unsigned int channel_id = m_tileTBID->channel(id);

    // MBTS Id module is "phi"  0-7
    unsigned int module_id = m_tileTBID->module(id);

    ATH_MSG_DEBUG("type_id = " << type_id);
    ATH_MSG_DEBUG("channel_id = " << channel_id);
    ATH_MSG_DEBUG("module_id = " << module_id);

    // Catch errors
    if(abs(type_id) != 1) {
      ATH_MSG_WARNING("MBTS identifier type is out of range");
      continue;
    }
    if( channel_id > 1 ){
	    ATH_MSG_WARNING("MBTS identifier channel is out of range");
      continue;
    }
    if( module_id > 7 ){
	    ATH_MSG_WARNING("MBTS identifier module is out of range");
      continue;
    }

    bit_pos = 0; // The position of the bit

    if(type_id == -1) { // C-side
      bit_pos += 16;
    }

    bit_pos += channel_id*8;
    bit_pos += module_id;

    unsigned int nTriggerEnergies = m_triggerEnergies.size();
    if(bit_pos > (nTriggerEnergies-1)) {
      ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the energy vector " << nTriggerEnergies);
    }
    else {
      m_triggerEnergies[bit_pos] = (*m_itt)->energy();
      m_triggerID[bit_pos] = bit_pos;
      ATH_MSG_DEBUG("Counter id = " << bit_pos << ", energy = " << m_triggerEnergies[bit_pos] << " pC");
    }

    unsigned int nTriggerTimes = m_triggerTimes.size();
    if(bit_pos > (nTriggerTimes-1)) {
      ATH_MSG_ERROR("Bit pos " << bit_pos << " is greater than the size of the time vector " << nTriggerTimes);
    }
    else {
      m_triggerTimes[bit_pos] = (*m_itt)->time();
	    ATH_MSG_DEBUG("Counter id = " << bit_pos << ", time = " << m_triggerTimes[bit_pos] << " ns");
    }
  }


  /** A pointer for access to the Tile Test Beam Identifier helper. */

return StatusCode::SUCCESS;
}
