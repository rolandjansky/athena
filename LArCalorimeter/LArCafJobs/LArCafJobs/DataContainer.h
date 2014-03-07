/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**
   @class LArSamplesDataContainer
   @brief Liquid Argon class for compact storage of samples data
*/

#ifndef LArSamples_DataContainer_H
#define LArSamples_DataContainer_H

#include "LArCafJobs/Definitions.h"
#include "CaloIdentifier/CaloGain.h"
#include <vector>
#include "LArElecCalib/LArVectorProxy.h"

class TH1D;


namespace LArSamples {

  class DataContainer {
    
    public:
      
      /** @brief Constructor  */
      DataContainer();
      
      DataContainer(CaloGain::CaloGain gain, const std::vector<short>& samples, 
           float energy, float time, float quality,
           int eventIndex,
		    LArVectorProxy autoCorr,
		    //const std::vector<float>& autoCorr, 
		    float noise = -1,
           float pedestal = 0, float pedestalRMS = -1,
	   int status = -1, float adcMax = -1);
      
      DataContainer(CaloGain::CaloGain gain, const std::vector<short>& samples, 
                    const std::vector<float>& corrs,
                    int eventIndex,
                    float energy, float time, float quality,
                    float pedestal, float pedestalRMS,
                    int status, float adcMax);
      
      DataContainer(const DataContainer& other, double adcMax = -1, double time = -1);
             
      /** @brief Destructor */
      virtual ~DataContainer();
  
      /** @return gain value */
      CaloGain::CaloGain gain() const { return m_gain; }
  
      /** @return number of samples */
      unsigned int nSamples() const { return m_samples.size(); }
  
      /** @return a sample value */
      const std::vector<short>& samples() const { return m_samples; }
      const short& sample(unsigned int i) const { return m_samples[i]; }
      
      /** @return event index */
      int eventIndex() const { return m_eventIndex; }
      
      /** @return energy */
      float energy() const { return m_energy; }
      
      /** @return time */
      float ofcTime() const { return m_time; }
  
      /** @return quality */
      float quality() const { return m_quality; }
      
      /** @return pedestal */
      float pedestal() const { return m_pedestal; }
      float pedestalRMS() const { return m_pedestalRMS; }
      
      /** @return status */
      int status() const { return m_status; }
      
      /** @return correlation matrix */
      const std::vector<float>& corrs() const { return m_corrs; }
      
      /** @return OFC peak parameters */
      float adcMax() const { return m_adcMax; }

      double footprint() const { return sizeof(*this) + m_samples.size()*sizeof(short) + m_corrs.size()*sizeof(float); }

      void setADCMax(float adcMax) { m_adcMax = adcMax; }
      void setTime  (float time)   { m_time = time; }
      void setEventIndex(int index) { m_eventIndex = index; }

      // debug only, use with care!
      void set(unsigned int i, short val) { m_samples[i] = val; }
      bool isValid() const;
      
    private:

      /** @brief gain */
      CaloGain::CaloGain m_gain;
      
      /** @brief vector of ADC samples */
      std::vector<short> m_samples;
            
      /** @brief noise + correlations in MeV */
      std::vector<float> m_corrs;
   
      int m_eventIndex;
      
      float m_energy, m_time, m_quality;
      float m_pedestal, m_pedestalRMS;
      unsigned int m_status;
      float m_adcMax;

      DataContainer& operator= (const DataContainer&);
  };
}
#endif

