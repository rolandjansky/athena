/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAM_CSCRAWDATA_H
#define MUONBYTESTREAM_CSCRAWDATA_H

#include <inttypes.h>
#include <vector>
#include <iostream>

class MsgStream;

/**

Class to hold the electronic output for a single CSC readout channel:
n sampling ADC data + the address (Identifier of strip == use hash id)
@author Ketevi A. Assamagan
        Dec 24 2003, BNL
*/
class CscRawData {

 private:

  /// data members

  /** ADC samples */
  std::vector<uint16_t> m_amps;

  /** online identifier of the first strip in the ROD cluster */
  uint32_t m_address; 

  /** online identifier of the collection in which this strip is */
  uint16_t m_id;

  /** online peaking time of the first strip: this is 0 if failed to compute time in the ROD */
  uint16_t m_time;

  /** SPU -Sparsifier Processing Unit - identifier in which this strip is */
  uint16_t m_rpuID;

  /** number of consecutive strips forming the on-line cluster */
  uint16_t m_width;

  /** failed to compute the time in the ROD or not - if failure m_time = 0 */
  bool m_isTimeComputed;

  /** offline hash identifier of the first strip in the online cluster */
  uint32_t m_hashId; 
  
 public:

  /** default constructor */
  CscRawData () : m_address(0), m_id(0), m_time(0), m_rpuID(0), 
m_width(0), m_isTimeComputed(false), m_hashId(0) {}

  /** constructor for a single strip */
  CscRawData (const std::vector<uint16_t>& p_amp, uint32_t address, uint16_t id) : 
    m_amps (p_amp),
    m_address(address),
    m_id(id),
    m_time (0x0),
    m_rpuID (0x0),
    m_width (0x1),
    m_isTimeComputed (false),
    m_hashId (0x0)
  {
  }

  /** single strip constructor with SPU information */
  CscRawData (const std::vector<uint16_t>& p_amp, uint32_t address, 
	      uint16_t id, uint16_t rpuID) : 
    m_amps (p_amp),
    m_address(address),
    m_id(id),
    m_time (0x0),
    m_rpuID(rpuID),
    m_width (0x1),
    m_isTimeComputed (false),
    m_hashId (0x0)
  {
  }

  /** cluster of strip constructor with cluster width and SPU identifier */
  CscRawData (const std::vector<uint16_t>& p_amp, uint32_t address, uint16_t id, 
	      uint16_t rpuID, uint16_t width) : 
    m_amps (p_amp),
    m_address(address),
    m_id(id),
    m_time (0x0),
    m_rpuID(rpuID),
    m_width(width),
    m_isTimeComputed (false),
    m_hashId (0x0)
  {
  }

  /** full constructor for cluster with timing information */
  CscRawData (const std::vector<uint16_t>& p_amp, uint32_t address, uint16_t id, 
              uint16_t time, uint16_t rpuID, uint16_t width, uint32_t hashId) : 
    m_amps (p_amp),
    m_address(address),
    m_id(id),
    m_time(time),
    m_rpuID(rpuID),
    m_width(width),
    m_isTimeComputed (true),
    m_hashId(hashId)
  {
  }

  virtual ~CscRawData() {}

  /** construction methods */
  void setRpuID  (uint16_t rpuID) { m_rpuID = rpuID; }
  void setTime   (uint16_t time)  { m_time = time; m_isTimeComputed = true; }
  void setWidth  (uint16_t width) { m_width = width; }
  void setHashID (uint32_t hash)  { m_hashId = hash; }
  void set_isTimeComputed()       { m_isTimeComputed = true; }

  /** data access methods */
  uint16_t rpuID() const { return m_rpuID; }
  uint16_t identify() const { return m_id; } 
  uint16_t time() const { return m_time; }
  uint16_t width() const { return m_width; }
  bool isTimeComputed() const { return m_isTimeComputed; } 
  const std::vector<uint16_t>& samples() const { return m_amps; }
  uint32_t address() const { return m_address; }
  uint32_t hashId() const { return m_hashId; }

  /** retrieve the samples for the position i in the list */
  bool samples (const unsigned int i, const unsigned int numSample, std::vector<uint16_t>& adc) const ;
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const CscRawData& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const CscRawData& coll);


#endif







