/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARDIGIT_H
#define LARDIGIT_H
#include <vector>

#include "Identifier/Identifier.h"
#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"

/**
   @class LArDigit
   @brief Liquid Argon digit base class

   represents the output of the FrontEnd electronics: ADC time samples in a given gain for a readout channel
*/

/* modifications : */
/* 11/05/2002 : channelID transformed into a const method */
/* 09/19/2002 : move to DataVector, remove ContainedObject, clid, etc*/ 
/* 10/02/2004 : Migrate for new HWIdentifier. Old interface is removed. (Walter Lampl) */

class LArDigit  {
  
 private:
  
  /** @brief Online Identifier */
  HWIdentifier m_hardwareID;
  
  /** @brief gain */
  CaloGain::CaloGain m_gain;
  
  /** @brief vector of ADC samples */
  std::vector < short > m_samples;
  
 public:
  /** @brief constructor 
      @param[in] channel_value  Online identifier
      @param[in] gain_value  Gain
      @param[in] sample_value Reference of vector with ADC samples
  */
  LArDigit(const HWIdentifier & channel_value, CaloGain::CaloGain gain_value, const std::vector < short > & sample_values):
    m_hardwareID(channel_value), m_gain(gain_value),m_samples(sample_values) {}

  LArDigit(const HWIdentifier & channel_value,
           CaloGain::CaloGain gain_value,
           std::vector < short > && sample_values)
    : m_hardwareID(channel_value),
      m_gain(gain_value),
      m_samples(std::move(sample_values))
  {}
    
    /** @return HWIdentifier */
    const HWIdentifier & hardwareID() const {return m_hardwareID; }

    /** @return channel ID */
    const HWIdentifier & channelID() const { return m_hardwareID; } 

    /** @return gain value */
    CaloGain::CaloGain gain() const { return m_gain; }

    /** @return number of samples */
    int nsamples() const { return m_samples.size(); }

    /** @return a reference to a stl vector containing the sample values */
    const std::vector < short > & samples() const { return m_samples; }

    /** @brief Conversion operator to a std::string <br> Can be used in a cast operation : (std::string) digit <br> */
    virtual operator std::string() const;
    
    /** @brief Set new digits in object.
        @param[in] samples  vector of ADC time samples
    */
    void setSamples( const std::vector < short >& samples);

    /** @brief Destructor */
    virtual ~LArDigit() { }

    /** @brief default constructor for persistency */
    LArDigit();
};

#endif //LARDIGIT_H

