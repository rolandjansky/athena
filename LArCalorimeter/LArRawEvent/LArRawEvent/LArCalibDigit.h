/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARCALIBDIGIT_H
#define LARCALIBDIGIT_H
#include "LArDigit.h"
#include "stdint.h"

/**

@class LArCalibDigit
@brief Base class for LArDigits taken during calibration runs

Additional data members to LArDigits are the DAC value, the delay value
and a bool that tells whether the cell was pulsed or not

@author Walter Lampl
*/

/**
 * --------------------------------
 * Modifications:
 * 10/02//2004: Move to HWIdentifer (replaces LArSignalChannelID)
 *   Walter Lampl
 */


class LArCalibDigit : public LArDigit  {
  
private:
  
  /** @brief DAC-Value*/
  uint16_t m_DAC;

  /** @brief Delay-Value and isPulsed boolean encoded in this variable*/
  uint16_t m_delayPulsed;

public:

  /** @brief constructor
      @param[in] channel_value  online channel identifier
      @param[in]  gain_value    Gain
      @param[in] sample_values  reference to vector of ADC time samples
      @param[in] DAC_value   DAC 
      @param[in] delay_value delay
      @param[in] isPulsed_value  bool to tell if channel is pulse by calibration system
  */
  LArCalibDigit(HWIdentifier & channel_value, CaloGain::CaloGain gain_value, const std::vector < short > & sample_values,
		uint16_t DAC_value, uint16_t delay_value, bool isPulsed_value) : 
    LArDigit(channel_value, gain_value, sample_values), m_DAC(DAC_value), 
    m_delayPulsed((delay_value & 0x7FFF) | ((uint16_t)isPulsed_value<<15  ))//, m_isPulsed(isPulsed_value)
    {};

  /** @return DAC value */
  int DAC() const {return static_cast<int>(m_DAC);}

  /** @return delay value */
  int delay() const {return static_cast<int>(m_delayPulsed & 0x7FFF);}
  
 /** @return isPulsed flag */
  bool isPulsed() const {return static_cast<bool>(m_delayPulsed & 0x8000);} 

  /** @brief Destructor */
  virtual ~LArCalibDigit();

  /** @brief default constructor for persistency */
  LArCalibDigit();

};

#endif //LARDIGIT_H

