// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v2.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V2_H
#define XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V2_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

// System include(s):
#include <stdint.h>

namespace xAOD {

  /// Description of TriggerTower_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 631127 $
  /// $Date: 2014-11-26 10:54:48 +0100 (Wed, 26 Nov 2014) $  
  ///  
  /// Trigger towers are the inputs to all other parts of the calorimeter trigger.
  /// They are formed by analogue summation of cells (represented in simulation
  /// by LArTTL1 and TileTTL1 objects). The L1Calo PreProcessor digitizes these,
  /// performs Bunch-Crossing IDentification, final calibration and reduction
  /// to 8 bit words.
  ///   
  /// The TriggerTower class represents the output of the PreProcessor.
  /// Each TriggerTower object actually contains information from a pair of
  /// trigger towers, one EM and one Hadronic, sharing the same eta, phi
  /// coordinates. This is convenient for algorithm emulation, though it does
  /// not represent the readout arrangement.
  ///    
  /// For each tower, the TriggerTower contains the final ET (LUT output)
  /// as well as a vector of ADC data (at 25ns intervals) from which it was
  /// formed, plus error flags and raw BCID output. As the PreProcessor
  /// readout can read variable numbers of slices of all of these data, the
  /// internal storage is in std::vectors, and methods exist to return all
  /// slices as well as just the data corresponding to the triggered crossing.
  ///
  
  class TriggerTower_v2 : public SG::AuxElement{
    public:
      // Default constructor
      TriggerTower_v2();
      
      /// Fill tower with all information
      void initialize(const uint_least32_t& coolId,const uint_least8_t& layer,const float& eta,const float& phi,
                      const std::vector<uint_least8_t>& lut_cp,const std::vector<uint_least8_t>& lut_jep,
                      const std::vector<int_least16_t>& correction,const std::vector<uint_least8_t>& correctionEnabled,
                      const std::vector<uint_least8_t>& bcidVec,const std::vector<uint_least16_t>& adc,
                      const std::vector<uint_least8_t>& bcidExt,
                      const uint_least16_t& error,
                      const uint_least8_t& peak,
                      const uint_least8_t& adcPeak);

      // Tower identifiers
      
      /// get coolId
      uint_least32_t coolId() const;
      /// set coolID
      void setCoolId(uint_least32_t);
      
      /// get layer
      uint_least8_t layer() const;
      /// set layer
      void setLayer(uint_least8_t);
      
      
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float);
      
      /// get phi
      float phi() const;
      /// set phi
      void setPhi(float); 


      // Quantities with same number of slices as LUT data
      
      /// get lut_cp
      const std::vector<uint_least8_t>& lut_cp() const;
      /// set lut_cp
      void setLut_cp(const std::vector<uint_least8_t>&);
      
      /// get lut_jep
      const std::vector<uint_least8_t>& lut_jep() const;
      /// set lut_jep
      void setLut_jep(const std::vector<uint_least8_t>&);      
      
      /// get correction
      const std::vector<int_least16_t>& correction() const;
      /// set correction
      void setCorrection(const std::vector<int_least16_t>&);  
      
       /// get correctionEnabled
      const std::vector<uint_least8_t>& correctionEnabled() const;
      /// set correctionEnabled
      void setCorrectionEnabled(const std::vector<uint_least8_t>&);      
      
      /// get bcidVec
      const std::vector<uint_least8_t>& bcidVec() const;
      /// set bcidVec
      void setBcidVec(const std::vector<uint_least8_t>&);      
      

      // Quantities with same number of slices as ADC data
      
      /// get adc
      const std::vector<uint_least16_t>& adc() const;
      /// set adc
      void setAdc(const std::vector<uint_least16_t>&);    
      
      /// get bcidExt
      const std::vector<uint_least8_t>& bcidExt() const;
      /// set bcidExt
      void setBcidExt(const std::vector<uint_least8_t>&);       
      

      // One error word/tower/event
      
      /// get error
      uint_least16_t error() const;
      /// set error
      void setError(uint_least16_t);

      // Not part of readout, but to extract "peak" slice from vectors
      
      /// get peak
      uint_least8_t peak() const;
      /// set peak
      void setPeak(uint_least8_t);
      
      /// get adcPeak
      uint_least8_t adcPeak() const;
      /// set adcPeak
      void setAdcPeak(uint_least8_t);
      
      
      /// get cpET from peak of lut_cp
      uint_least8_t cpET() const;
      
      /// get jepET from peak of lut_jep
      uint_least8_t jepET() const;
           

      /// Is tower saturated?
      bool isCpSaturated() const;
      bool isJepSaturated() const;
    
    private:
      static const uint_least8_t m_saturation = 255;
  
  }; // class TriggerTower_v2
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V2_H
