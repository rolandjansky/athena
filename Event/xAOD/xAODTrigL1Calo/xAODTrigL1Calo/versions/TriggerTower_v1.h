// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TriggerTower_v1.h 631127 2014-11-26 09:54:48Z gwatts $
#ifndef XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V1_H
#define XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V1_H

// EDM include(s):
#include "AthLinks/ElementLink.h"
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Description of TriggerTower_v1
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
  
  class TriggerTower_v1 : public SG::AuxElement{
    public:
      // Default constructor
      TriggerTower_v1();

      /// get EM energy for all time slices
      const std::vector<int>& emLUT() const;
      /// set EM energy for all time slices
      void setEmLUT(const std::vector<int>&);

      /// get HAD energy for all time slices
      const std::vector<int>& hadLUT() const;
      /// set HAD energy for all time slices
      void setHadLUT(const std::vector<int>&);     

      /// get EM ADC for all time slices
      const std::vector<int>& emADC() const;
      /// set EM ADC for all time slices
      void setEmADC(const std::vector<int>&);

      /// get HAD ADC for all time slices
      const std::vector<int>& hadADC() const;
      /// set HAD ADC for all time slices
      void setHadADC(const std::vector<int>&);   

      /// get EM BCID for all time slices
      const std::vector<int>& emBCIDvec() const;
      /// set EM BCID for all time slices
      void setEmBCIDvec(const std::vector<int>&);

      /// get HAD BCID for all time slices
      const std::vector<int>& hadBCIDvec() const;
      /// set HAD BCID for all time slices
      void setHadBCIDvec(const std::vector<int>&);    

      /// get EM extBCID for all time slices
      const std::vector<int>& emBCIDext() const;
      /// set EM extBCID for all time slices
      void setEmBCIDext(const std::vector<int>&);

      /// get HAD extBCID for all time slices
      const std::vector<int>& hadBCIDext() const;
      /// set HAD extBCID for all time slices
      void setHadBCIDext(const std::vector<int>&);       

      /// get phi (note that for L1Calo phi runs from 0 to 2pi)
      float phi() const;
      /// set phi
      void setPhi(float);
      
      /// get eta
      float eta() const;
      /// set eta
      void setEta(float);
      
      /// get key of TT
      unsigned int key() const;
      /// set key of TT
      void setKey(unsigned int);
      
      /// get EM error
      int emError() const;
      /// set EM error
      void setEmError(int);
      
      /// get HAD error
      int hadError() const;
      /// set HAD error
      void setHadError(int);  
      
      /// get EM peak
      int emPeak() const;
      /// set EM peak
      void setEmPeak(int);   

      /// get EM ADC peak
      int emADCPeak() const;
      /// set EM ADC peak
      void setEmADCPeak(int);  
      
      /// get HAD peak
      int hadPeak() const;
      /// set HAD peak
      void setHadPeak(int);   

      /// get HAD ADC peak
      int hadADCPeak() const;
      /// set HAD ADC peak
      void setHadADCPeak(int);     
      
      
      /// Fill EM towers with digits, calibrated ET vectors and status flags
      void addEM(const std::vector<int>& digits, const std::vector<int>& et, 
                  const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
                  int error, int peak, int adc_peak);
      /// Fill HAD towers with digits, calibrated ET vectors and status flags
      void addHad(const std::vector<int>& digits, const std::vector<int>& et, 
                  const std::vector<int>& digits_bcid, const std::vector<int>& bcid, 
                  int error, int peak, int adc_peak); 
      
      /// Specify the location of tower. Internally calls setEta();setPhi();
      void setCoord(double eta,double phi);
      
      /// Final EM energy value used by the algorithms
      int emEnergy() const;
      /// Final HAD energy value used by the algorithms
      int hadEnergy() const;    
      
      /// get EM BCID at peak
      int emBCID() const;
      /// get HAD BCID at peak
      int hadBCID() const;
      
      /// Is EM saturated?
      bool isEMSaturated() const;
      /// Is HAD saturated?
      bool isHadSaturated() const;
    
    private:
      static const int m_saturation = 255;
  
  }; // class TriggerTower_v1
} // namespace xAOD

#endif // XAODTRIGL1CALO_VERSIONS_TRIGGERTOWER_V1_H
