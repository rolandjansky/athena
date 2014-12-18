/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATA_P0_H
#define MUON_CSCRAWDATA_P0_H

/*
Persistent representation of the old CscRawData class
Used by ROOT streamer
Author: Marcin Nowak
        CERN, December 2005
*/

#include <inttypes.h>
#include <vector>

class CscRawData_p0
{

 public:
  CscRawData_p0 () : m_address(0), m_dpuID(0), m_id(0) {}
  virtual ~CscRawData_p0() {}

  void setDpuID (uint16_t dpuID) { m_dpuID = dpuID; }
  uint16_t dpuID() const { return m_dpuID; }
  uint16_t identify() const {return m_id;} 
  const std::vector<uint16_t>  getSamples() const { return m_amps; }
  uint32_t getAddress() const { return m_address; }

  // to be removed eventually
  const uint16_t * getAmplitudes() const { return m_amplitude; }

  
private:

  //!!!!!!!!!!!!!!!   Order of data fields sorted alphabethically for compatibility with ROOT 3 and 4 (Reflection dictionary)

  uint32_t m_address; 

  // this data member is
  // only here for backward compatibility with ROOT3
  uint16_t m_amplitude[4];

  std::vector<uint16_t> m_amps;
  uint16_t m_dpuID;
  uint16_t m_id;
  
};

#endif













