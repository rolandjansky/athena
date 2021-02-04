/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
/** 
    
@class LArAccumulatedCalibDigit
@brief Data class for calibration ADC samples preprocessed by the DSP

@author Sandrine Laplace
@author Isabelle Wingerter-Seez
*/

/* modifications : */
/*
03-12-06, G.Rosenbaum
Added constructor to take an int for isPulsed to store info re: which caliblines
have been pulsed (HEC can be pulsed by up to 4.

Changed getDelay to get the forst 12 bits of m_delayPulsed
Added functions to get the isPusled info


25-08-09 W. Lampl
Remove accumulation in converter, simplify LArAccumulatedDigit class
Contains now only the data of the currently processed substep.

*/



#ifndef LARACCUMULATEDCALIBDIGIT_H
#define LARACCUMULATEDCALIBDIGIT_H
#include <vector>

#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"     

#include "stdint.h"

class LArAccumulatedCalibDigit {

private:

  /** @brief Online Identifier */
  HWIdentifier m_hardwareID;
  
  /** @brief gain */
  CaloGain::CaloGain m_gain;

  /** @brief vector of sample sums */
  std::vector < uint32_t > m_sampleSum;   
  
  /** @brief vector of sample square sums */
  std::vector < uint32_t > m_sample2Sum;

  /** @brief number of samples */
  //uint32_t m_nsamples;

  /** @brief number of trigger step*/
  uint32_t m_nTriggers;

  /** @brief DAC-Value*/
  uint32_t m_DAC;

  /** @brief Delay-Value and isPulsed boolean encoded in this variable*/
  uint16_t m_delayPulsed;

  uint16_t m_nStep;

  uint16_t m_iStep;

public:

  /** @brief Constructor (initialize everything)*/      
  LArAccumulatedCalibDigit(HWIdentifier & channel_value, CaloGain::CaloGain gain_value, 
			   const std::vector < uint32_t >& sampleSum_value, 
			   const std::vector < uint32_t >& sample2Sum_value,
			   uint32_t nTrigger_value,  uint32_t DAC_value=0, uint16_t delay_value=0, 
			   uint16_t isPulsed_value=false, uint16_t nStep_value=0, uint16_t iStep_value=0) : 
    m_hardwareID(channel_value), m_gain(gain_value), m_sampleSum(sampleSum_value), m_sample2Sum(sample2Sum_value),
    m_nTriggers(nTrigger_value), m_DAC(DAC_value), 
    m_delayPulsed((delay_value & 0xFFF) | (isPulsed_value<<12)),  
    m_nStep(nStep_value), m_iStep(iStep_value) 
    {};
  
  /** @brief Constructor (initialize for later accumulation) */
  LArAccumulatedCalibDigit(HWIdentifier & channel_value, CaloGain::CaloGain gain_value, 
			   uint32_t nsamples=7, uint32_t DAC_value=0, uint16_t delay_value=0, 
			   uint16_t isPulsed_value=false, uint16_t nStep_value=0, uint16_t iStep_value=0) : 
    m_hardwareID(channel_value), m_gain(gain_value), 
    m_nTriggers(0), m_DAC(DAC_value),
    m_delayPulsed((delay_value & 0xFFF) | (isPulsed_value<<12)),
    m_nStep(nStep_value), m_iStep(iStep_value) 
  {
    m_sampleSum.resize(nsamples);
    m_sample2Sum.resize(nsamples);
  };

  /** @brief Destructor */
  virtual ~LArAccumulatedCalibDigit();

  /** @brief default constructor for persistency */
  LArAccumulatedCalibDigit() :
    m_hardwareID(0), m_gain(CaloGain::LARNGAIN), m_nTriggers(0), 
    m_DAC(0), m_delayPulsed(0), m_nStep(0), m_iStep(0) 
    {};

  /** @brief Return HWIdentifier */
  const HWIdentifier & hardwareID() const {return m_hardwareID; }

  /** @brief Return channel ID */
  const HWIdentifier & channelID() const { return m_hardwareID; } 

  /** @brief return gain value */
  CaloGain::CaloGain gain() const { return m_gain; }

  /** @brief return number of samples */
  size_t nsamples() const { return m_sampleSum.size(); }

  /** @brief return a reference to a stl vector containing the sample-sums */
  const std::vector < uint32_t > & sampleSum() const { return m_sampleSum; }

  /** @brief return a reference to a stl vector containing the sum of the squares of the sample  */
  const std::vector < uint32_t > & sample2Sum() const { return m_sample2Sum; }

  /** @brief return the number of triggers */
  unsigned nTriggers() const {return m_nTriggers;}

  /** @brief return the number of samples */
  //unsigned nSamples() const {return m_nsamples;}

  /** @brief return the setting of the DAC */
  int DAC() const {return static_cast<int>(m_DAC);}

  /** @brief return the setting of the delay */
  int delay() const {return static_cast<int>(m_delayPulsed & 0xFFF);}
  
  /** @brief Tell if this channel was pulsed */
  bool isPulsed() const {return static_cast<bool>(m_delayPulsed & 0xF000);}
 
  /** @brief Tell if calib line n was pulsed for this channel (n=1,2,3,4) */
  bool isPulsed(int n) const {
    if(n==1) return static_cast<bool>(m_delayPulsed & 0x1000);
    if(n==2) return static_cast<bool>(m_delayPulsed & 0x2000);
    if(n==3) return static_cast<bool>(m_delayPulsed & 0x4000);
    if(n==4) return static_cast<bool>(m_delayPulsed & 0x8000);
    return false;//WRITE ERROR MESSAGE
  }

  /** @brief get the four bit int that tells which lines pulsed */
  uint16_t getIsPulsedInt() const { return (m_delayPulsed >> 12);}

  /** @brief Calculates and returns the Mean value of each ADC sample */
  std::vector<float> mean() const;

  /** @brief Calculates and returns the RMS value of each ADC sample */
  std::vector<float> RMS() const;


  /** @brief Calculates and returns the Mean value for one ADC sample */
  float mean(const size_t i) const;

  /** @brief Calculates and returns the RMS value for one ADC sample */
  float RMS(const size_t i) const;

  uint16_t nSteps() const { return m_nStep;}

  uint16_t stepIndex() const {return m_iStep;}

  /** @brief Set the sampleSum */
  void setSampleSum(const std::vector< uint32_t >& sampleSum);

  /** @brief Set the sample2Sum */
  void setSample2Sum(const std::vector< uint32_t >& sample2Sum);

  /** @brief Sum up with another (individual) LArCalibDigit */
  bool setAddDigit(const std::vector<short>& samples);

  /** @brief Sum up with another substep */
  bool setAddSubStep(const std::vector < uint32_t >& sampleSum, const std::vector < uint32_t >& sample2Sum, const uint32_t nTriggerPerStep);

};

#endif //LARDIGIT_H

