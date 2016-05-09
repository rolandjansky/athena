/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT2MinBias/T2MbtsUtils.h"
#include "GaudiKernel/MsgStream.h"

#include <bitset>
#include <cmath>

T2MbtsUtils::T2MbtsUtils(): m_threshold(0.),
			    m_timeCut(0.),
			    m_globalTimeOffset(0.),
			    m_timeMean_A(0.),
			    m_timeMean_C(0.),
			    m_timeDiff_A_C(0.),
			    m_mult(std::make_pair(0,0)) {
  for(int i=0;i<32;i++) m_timeOffsets[i] = 0.;
}

int T2MbtsUtils::calculateMultiplicities(const xAOD::TrigT2MbtsBits *t2mbtsBits,
					 int mode,
					 MsgStream& mlog, 
					 unsigned int msgLvl) {
  m_mult = std::make_pair(0,0);

  if(!t2mbtsBits) {
    return 0;
  }

  if(msgLvl <= MSG::DEBUG) mlog << MSG::DEBUG << "Getting energy and time values." << endreq;

  std::vector<float> triggerEnergies = t2mbtsBits->triggerEnergies();
  std::vector<float> triggerTimes = t2mbtsBits->triggerTimes();

  if(triggerEnergies.size() != xAOD::TrigT2MbtsBits::NUM_MBTS || triggerTimes.size() != xAOD::TrigT2MbtsBits::NUM_MBTS ) {
    if(msgLvl <= MSG::WARNING) mlog << MSG::WARNING << "Vector sizes are not equal to number of MBTS counters." << endreq; 
    return 0;
  }

  if(msgLvl <= MSG::DEBUG) mlog << MSG::DEBUG << "Forming hit multiplicities." << endreq;
  
  std::bitset<16> ebaTriggerBits;
  std::bitset<16> ebcTriggerBits;
  unsigned ibit;
  int ebaCounters=0, ebcCounters=0;

  // Initialize monitoring variables.
  m_timeDiff_A_C = 0.;
  m_timeMean_A = 0.;
  m_timeMean_C = 0.;

  // Loop over each counter and form bit sets from time and energy (optional).
  for(ibit=0; ibit< xAOD::TrigT2MbtsBits::NUM_MBTS; ibit++) {
    if(mode==1 && !( ibit<8               || (ibit>=16 && ibit<24) )) continue; // count only inner
    if(mode==2 && !((ibit >=8 && ibit<16) || (ibit>=24 && ibit<32) )) continue;  // count only outer
    
    // Check the energy threshold.
    if(triggerEnergies[ibit] <= m_threshold) continue; 

    // Calculate the mean time for this side;
    if(ibit<16) {
      m_timeMean_A += triggerTimes[ibit]; ebaCounters++;
    }
    else {
      m_timeMean_C += triggerTimes[ibit]; ebcCounters++;
    }

    // Check the time cut if it is active.
    if(m_timeCut>=0 && fabs(triggerTimes[ibit] - m_timeOffsets[ibit] - m_globalTimeOffset) >= m_timeCut) continue;
    
    // Set the bits for this side;
    if(ibit<16) {
      ebaTriggerBits.set(ibit);
    }
    else {
      ebcTriggerBits.set(ibit-16); 
    }
  }

  if(ebaCounters>0) {
    m_timeMean_A /= ebaCounters;
  }
  else {
    m_timeMean_A = -999.0;
  }
  
  if(ebcCounters>0) {
    m_timeMean_C /= ebcCounters;
  }
  else {
    m_timeMean_C = -999.0;
  }
  
  if(ebaCounters>0 && ebcCounters>0) {
    m_timeDiff_A_C = m_timeMean_A - m_timeMean_C;
  }
  else {
    m_timeDiff_A_C = -999.0;
  }

  if(msgLvl <= MSG::DEBUG) { 
    mlog << MSG::DEBUG 
	  << "average MBTS trigger time side A: " << m_timeMean_A 
	  << ", side C: " << m_timeMean_C 
	  << ", difference A-C: "<< m_timeDiff_A_C << endreq;
    
    mlog << MSG::DEBUG << "MBTS EBA trigger bits: " << ebaTriggerBits << endreq;
    mlog << MSG::DEBUG << "MBTS EBC trigger bits: " << ebcTriggerBits << endreq;
  }
  
  m_mult = std::make_pair(ebaTriggerBits.count(),ebcTriggerBits.count());

  return 1;
}
