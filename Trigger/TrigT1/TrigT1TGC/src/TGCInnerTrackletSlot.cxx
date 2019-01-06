/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1TGC/TGCInnerTrackletSlot.hh"

namespace LVL1TGCTrigger {

  extern bool        g_USE_CONDDB;

  /** Constructor */
  TGCInnerTrackletSlot::TGCInnerTrackletSlot() {
    m_sideId = -1;
    m_slotId = -1;
    clearTriggerBits();
  }

  void TGCInnerTrackletSlot::setSideId(const int sideId) { 
    m_sideId = sideId; 
  }

  int TGCInnerTrackletSlot::getSideId() const {
    return m_sideId;
  }

  void TGCInnerTrackletSlot::setSlotId(const int slotId) {
    m_slotId = slotId;
  }

  int TGCInnerTrackletSlot::getSlotId() const {
    return m_slotId;
  }

  bool TGCInnerTrackletSlot::setTriggerBits(const bool val) {
    for (int region=0; region<NUMBER_OF_REGIONS; region++){
      for (int readout=0; readout<NUMBER_OF_READOUTS; readout++){
	for (int iBit=0; iBit<NUMBER_OF_TRIGGER_BITS; iBit++){
	  m_triggerBit[region][readout][iBit] = val;
	}
      }
    }
    return true;
  }

  bool TGCInnerTrackletSlot::setTriggerBit(const unsigned int region, const unsigned int readout, const unsigned int iBit, const bool triggerBit) {
    if(region>=NUMBER_OF_REGIONS) return false;
    if(readout>=NUMBER_OF_READOUTS) return false;
    if(iBit>=NUMBER_OF_TRIGGER_BITS) return false;
    m_triggerBit[region][readout][iBit] = triggerBit;
    return true;
  }

  bool TGCInnerTrackletSlot::getTriggerBit(const unsigned int region, const unsigned int readout, const unsigned int iBit) const {
    if(region>=NUMBER_OF_REGIONS) return false;
    if(readout>=NUMBER_OF_READOUTS) return false;
    if(iBit>=NUMBER_OF_TRIGGER_BITS) return false;

    return m_triggerBit[region][readout][iBit];
  }

  void TGCInnerTrackletSlot::clearTriggerBits() {
    for(unsigned int region=0; region<NUMBER_OF_REGIONS; region++) {
      for(unsigned int readout=0; readout<NUMBER_OF_READOUTS; readout++) {
	for(unsigned int iBit=0; iBit<NUMBER_OF_TRIGGER_BITS; iBit++) {
	  m_triggerBit[region][readout][iBit] = false;
	}
      }
    }
  }

  void TGCInnerTrackletSlot::setTriggerWord(unsigned int word){
    unsigned int tstBit =1;
    for(unsigned int region=0; region<NUMBER_OF_REGIONS; region++) {
      for(unsigned int readout=0; readout<NUMBER_OF_READOUTS; readout++) {
	for(unsigned int iBit=0; iBit<NUMBER_OF_TRIGGER_BITS; iBit++) {
	  m_triggerBit[region][readout][iBit] = ((tstBit & word) !=0 );
	  tstBit = tstBit*2;
	}
      }
    }
  }

  unsigned short TGCInnerTrackletSlot::getTriggerWord() const{
    unsigned int word=0;
    unsigned int pos =0;
    for(unsigned int region=0; region<NUMBER_OF_REGIONS; region++) {
      for(unsigned int readout=0; readout<NUMBER_OF_READOUTS; readout++) {
	for(unsigned int iBit=0; iBit<NUMBER_OF_TRIGGER_BITS; iBit++) {
	  if (m_triggerBit[region][readout][iBit]) {
	    word += 1 << pos;
	  }
	  pos += 1;
	}
      }
    }
    return word;
  }

} //end of namespace bracket
