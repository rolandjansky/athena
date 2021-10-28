/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MM_RAWDATA_H
#define MUONRDO_MM_RAWDATA_H

#include <inttypes.h>
#include <vector>
#include <iostream>
#include "Identifier/Identifier.h"

class MsgStream;

namespace Muon {

/**
Temporary class to hold the MM RDO.
*/
class MM_RawData {

  friend class MM_RawDataCnv_p1;  
  friend class MM_RawDataCnv_p2;  

 private:
  
  // not sure whether the offline identifier is needed
  const Identifier m_id;
  
  /// channel Id
  int m_channel;
  /// tdc counts
  int m_time;
  /// adc counts
  int m_charge;
  /// rel bcid
  uint16_t m_relBcid;
  
 public:

  /** default constructor */
  MM_RawData ( const Identifier& id ) : m_id(id), m_channel(0), m_time(0), m_charge(0), m_relBcid(0) {}


  MM_RawData ( const Identifier& id, const int channel, 
	       const int time, const int charge, const uint16_t relBcid) 
    :  m_id(id), m_channel(channel), m_time(time), m_charge(charge), m_relBcid(relBcid) {}


  MM_RawData () {} ; //!< TODO remove this. Currently have problems with convertor if I do though.
  
  virtual ~MM_RawData() {}

  const Identifier& identify() const { return m_id; }

  // access functions to the online data
  int channel() const { return m_channel; }
  int time() const { return m_time; }
  uint16_t relBcid() const { return m_relBcid; }
  int charge() const { return m_charge; }

};
}

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const Muon::MM_RawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const Muon::MM_RawData& coll);


#endif







