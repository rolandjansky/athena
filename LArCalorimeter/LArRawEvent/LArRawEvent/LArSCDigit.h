/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARSCDIGIT_H
#define LARSCDIGIT_H
#include "LArDigit.h"

/**

@class LArSCDigit
@brief Base class for LArDigits taken by LATOME

Additional data members to LArDigits are the LATOME channel number and the vector of BCIDs

@author Pavol Strizenec
*/

class LArSCDigit : public LArDigit  {
  
private:
  
  /** @brief LATOME channel*/
  uint16_t m_chan;

  /** @brief LATOME source Id*/
  uint32_t m_sourceId;

  /** @brief vector of BCID*/
  std::vector<uint16_t> m_BCId;

public:

  LArSCDigit(const HWIdentifier & channel_value, short channel, unsigned int sourceId, 
             const std::vector<short> & sample_values,
     	     const std::vector<unsigned short> & bcids) : 
    LArDigit(channel_value, CaloGain::LARHIGHGAIN, sample_values), 
    m_chan(channel), m_sourceId(sourceId), m_BCId(std::move(bcids))
    {};

  /** @return channel value */
  short Channel() const {return static_cast<short>(m_chan);}

  /** @return source Id value */
  unsigned int SourceId() const {return static_cast<unsigned int>(m_sourceId);}

  /** @return bcids */
  const std::vector<unsigned short> &BCId() const {return m_BCId;}

  /** @brief Destructor */
  virtual ~LArSCDigit();

  /** @brief default constructor for persistency */
  LArSCDigit();

};

#endif //LARDIGIT_H

