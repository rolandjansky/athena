/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


/** 
    
@class LArAccumulatedDigit
@brief Data class for  ADC samples and autocorr preprocessed by the DSP

Contains sum of ADC counts and the sum of the s_i*s_{i+j}
adc counts plus gain, number of triggers and samples

@author Remi Lafaye

Modifications:
Walter Lampl, 27 Aug 2009:
Remove storage of individual substeps. The class represents now
exactly one substep or the sum of many substeps. 

*/ 

#ifndef LARACCUMULATEDDIGIT_H
#define LARACCUMULATEDDIGIT_H
#include <vector>

#include "Identifier/HWIdentifier.h"
#include "CaloIdentifier/CaloGain.h"     

#include "stdint.h"

class LArAccumulatedDigit {

private:

  /** @brief Online Identifier */
  HWIdentifier m_hardwareID;
  
  /** @brief gain */
  CaloGain::CaloGain m_gain;

  /** @brief sampleSum over ntrigger*nsamples */
  std::vector<uint64_t> m_sampleSum;   
  
  /** @brief vector(index: j from s_i*s_{i+j}) */
  std::vector<uint64_t> m_sampleSquare;   

  /** @brief number of total triggers */
  uint32_t m_nTrigger;

public:

  /** @brief Constructor (first type)*/      
  LArAccumulatedDigit(HWIdentifier & channel_value, 
		      CaloGain::CaloGain gain_value, 
		      const std::vector<uint32_t>& sampleSum_value, 
		      const std::vector<uint32_t>& sampleSquare_value,
		      uint32_t nTrigger_value);
  
  /** @brief Constructor (second type)*/      
  LArAccumulatedDigit(HWIdentifier & channel_value);

  /** @brief Destructor */
  ~LArAccumulatedDigit();

  /** @brief default constructor for persistency */
  LArAccumulatedDigit();

  /** @brief Return HWIdentifier */
  const HWIdentifier & hardwareID() const {return m_hardwareID; }

  /** @brief Return channel ID */
  const HWIdentifier & channelID() const { return m_hardwareID; } 

  /** @brief return gain value */
  CaloGain::CaloGain gain() const { return m_gain; }

  /** @brief return number of samples */
  int nsample() const { return m_sampleSquare.size(); }

  /** @brief return the sample-sums */
  const std::vector<uint64_t>& sampleSum() const { return m_sampleSum; };

  /** @brief return a reference to a stl vector containing the sum of the squares of the sample  */
  const std::vector < uint64_t > & sampleSquare() const { return m_sampleSquare; }

  /** @brief return the number of triggers */
  unsigned nTrigger() const {return m_nTrigger;}

  /** @brief Calculates and returns the Mean value of ADC samples */
  float mean(int n_min=-1, int n_max=-1) const;

  /** @brief Calculates and returns the RMS value of ADC samples  */
  float RMS(int n_min=-1, int n_max=-1) const;

  /** @brief Compute the autocorrelation elements */
  //MGV implement switch m_normalize to normalize
  void getCov(std::vector<float> &cov, int normalize) const;

  /** @brief Accumulate single digit */
  bool setAddDigit(const std::vector<short>& digit);


  /** @brief Accumulate new values  */
  bool setAddSubStep(const CaloGain::CaloGain gain_value,
                     const HWIdentifier chid, 
		     const std::vector<uint64_t>& sampleSum,
                     const std::vector <uint64_t>& sampleSquare, 
		     const unsigned nTrigger);

  void setAddSubStep(CaloGain::CaloGain gain_value, std::vector<uint32_t> sampleSum, 
 		     std::vector < uint32_t > sampleSquare, unsigned nTrigger); 
  void setAddSubStep(CaloGain::CaloGain gain_value, std::vector<int32_t> sampleSum, 
 		     std::vector < int32_t > sampleSquare, unsigned nTrigger, int32_t base); 


  bool setAddSubStep(const LArAccumulatedDigit& ad) {
    return setAddSubStep(ad.gain(), ad.channelID(), ad.sampleSum(),ad.sampleSquare(),ad.nTrigger());
  };
};

#endif //LARACCUMULATEDDIGIT_H

