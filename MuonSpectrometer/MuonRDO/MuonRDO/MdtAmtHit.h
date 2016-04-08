/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONRDO_MDTAMTHIT_H
#define MUONRDO_MDTAMTHIT_H

#include<stdint.h>
#include<vector>
#include <iostream>


class MsgStream;

/** MDT RDO's : data from a single channel of an AMT 
  Atlas Muon TDC
  @author Stefano Rosati, Feb 2003
*/
class MdtAmtHit
{

 private:
  
  // TDC Id in the CSM (TDC online Id)
  uint16_t m_tdcId;
  // Channel number 
  uint16_t m_channelId;

  // Leading edge boolean flag
  bool m_leading;
  
  // Decoded time of the first leading edge
  uint16_t m_coarse;
  uint16_t m_fine;
  // Decoded width in case of a combined measurement
  uint16_t m_width;

  // Masked channel flag
  bool m_isMasked;
  
  // All the datawords (no headers and footers) coming from this channel 
  // to be decoded on demand using MdtReadOut methods
  std::vector<uint32_t> m_dataWords;

 public:
  
  // Default constructor
  MdtAmtHit();
  
  // Full constructor 
  MdtAmtHit(uint16_t tdcId, uint16_t channelId);

  // Full constructor with masked flag
  MdtAmtHit(uint16_t tdcId, uint16_t channelId, bool isMasked);

  // set data members
  void setValues(uint16_t coarse, uint16_t fine, uint16_t width);

  void setTdcCounts(const uint16_t coarse, const uint16_t fine);
  
  void setWidth(const uint16_t width)  {m_width = width;};

  void setLeading(const bool leading)  {m_leading = leading;};

  // Add a data word to the vector
  void addData(uint32_t dataWord);
  
  // Retrieve the decoded data from the first leading edge
  // combined or single
  uint16_t tdcId()     const {return m_tdcId;}
  uint16_t channelId() const {return m_channelId;}
  uint16_t coarse()    const {return m_coarse;}
  uint16_t fine()      const {return m_fine;}
  uint16_t width()     const {return m_width;}
  bool     leading()   const {return m_leading;}
  bool     isMasked()  const {return m_isMasked;}
  
  // Vector containing all 32 bits data words (undecoded) 
  // FIXME: Should return by reference!
  const std::vector<uint32_t>* dataWords() const {return &m_dataWords;}
    
};

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const MdtAmtHit& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const MdtAmtHit& coll);


#endif // MUONRDO_MDTAMTHIT_H


