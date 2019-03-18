/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/PileUpTimeEventIndex.h"
#include <cassert>

#include <iostream>

const std::string&
PileUpTimeEventIndex::typeName(PileUpTimeEventIndex::PileUpType typ) {
  static std::string typNam[PileUpTimeEventIndex::NTYPES+1] =
    {"Unknown" /*-1*/, "Signal" /*0*/, "MinimumBias" /*1*/, "Cavern" /*2*/,
     "HaloGas"/*3*/, "HighPtMinimumBias" /*4*/, "ZeroBias"/*5*/ };
  assert(typ < PileUpTimeEventIndex::NTYPES);
  return typNam[typ+1];
} 
PileUpTimeEventIndex::PileUpType
PileUpTimeEventIndex::ushortToType(unsigned short uType) {
  PileUpTimeEventIndex::PileUpType puType(PileUpTimeEventIndex::Unknown);
  switch (uType) {
  case 0: puType = PileUpTimeEventIndex::Signal; break;
  case 1: puType = PileUpTimeEventIndex::MinimumBias; break;
  case 2: puType = PileUpTimeEventIndex::Cavern; break;
  case 3: puType = PileUpTimeEventIndex::HaloGas; break;
  case 4: puType = PileUpTimeEventIndex::HighPtMinimumBias; break;
  case 5: puType = PileUpTimeEventIndex::ZeroBias; break;
  default: 
    std::ostringstream merr;
    merr << 
      "PileUpTimeEventIndex::ushortToType FATAL: invalid PileUpType specified " 
	 << uType;
    std::cerr << merr.str() << std::endl;
    throw GaudiException(merr.str(), "PileUpTimeEventIndex::ushortToType", StatusCode::FAILURE);
  }
  return puType;
}
PileUpTimeEventIndex::PileUpTimeEventIndex(): m_time(0), m_index(0), m_type(PileUpTimeEventIndex::Unknown) {}

PileUpTimeEventIndex::PileUpTimeEventIndex(time_type time) : 
  m_time((short)time), m_index(0), m_type(PileUpTimeEventIndex::Unknown) {}

PileUpTimeEventIndex::PileUpTimeEventIndex(time_type time, index_type index) : 
  m_time((short)time), m_index((short)index), m_type(PileUpTimeEventIndex::Unknown) {}

PileUpTimeEventIndex::PileUpTimeEventIndex(time_type time, index_type index, 
                                           PileUpTimeEventIndex::PileUpType typ) :
  m_time((short)time), m_index((short)index), m_type(typ) {} 


PileUpTimeEventIndex::time_type PileUpTimeEventIndex::time() const {  
  return m_time; 
}

PileUpTimeEventIndex::index_type PileUpTimeEventIndex::index() const {
    return (index_type)m_index; 
}

PileUpTimeEventIndex::PileUpType PileUpTimeEventIndex::type() const {
  return m_type; 
}

std::ostream& operator << (std::ostream& os, 
			   const PileUpTimeEventIndex& idx) {
  return os << "event xing time (ns): " << idx.time() 
	    << " - PileUpEventInfo subevt index: " << idx.index()
            << " - PileupEventInfo subevt type:  " << PileUpTimeEventIndex::typeName(idx.type());
}

MsgStream& operator << (MsgStream& os, 
			const PileUpTimeEventIndex& idx) {
  return os << "event xing time (ns): " << idx.time() 
	    << " - PileUpEventInfo subevt index: " << idx.index()
            << " - PileupEventInfo subevt type:  " << PileUpTimeEventIndex::typeName(idx.type());
}
