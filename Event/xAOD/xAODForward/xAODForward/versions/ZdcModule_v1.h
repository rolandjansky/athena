// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ALFAData_v1.h 693858 2015-09-09 10:30:15Z krasznaa $
#ifndef XAODFORWARD_VERSIONS_ZDCMODULE_V1_H
#define XAODFORWARD_VERSIONS_ZDCMODULE_V1_H

// EDM include(s):
#include "AthContainers/AuxElement.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include <stdint.h>

namespace xAOD {

   /// Class containing ZDC Module information
   ///
   /// For information how to retrive, and to some level use this object,
   /// see the detailed package description.
   ///
   /// @author Peter Steinberg <peter.steinberg@bnl.gov>
   ///
   /// $Revision: 693858 $
   /// $Date: 2015-09-09 12:30:15 +0200 (Wed, 09 Sep 2015) $
   ///
  class ZdcModule_v1 : public SG::AuxElement {

   public:

    
     /// Default constructor
     ZdcModule_v1();
     
     /// @name Links to TriggerTowers
     /// @{
     /// Gain 0, Delay 0
     const ElementLink< TriggerTowerContainer >& TTg0d0Link() const;
     void setTTg0d0Link(const ElementLink< TriggerTowerContainer >& );
    //TriggerTower* TTg1d1() const;
     /// Gain 0, Delay 1
     const ElementLink< TriggerTowerContainer >& TTg0d1Link() const;
     void setTTg0d1Link(const ElementLink< TriggerTowerContainer >& );
    //TriggerTower* TTg1d1() const;
     
     /// Gain 1, Delay 0
     const ElementLink< TriggerTowerContainer >& TTg1d0Link() const;
     void setTTg1d0Link(const ElementLink< TriggerTowerContainer >& );
    //TriggerTower* TTg1d1() const;
     /// Gain 1, Delay 1
     const ElementLink< TriggerTowerContainer >& TTg1d1Link() const;
     void setTTg1d1Link(const ElementLink< TriggerTowerContainer >& );
    //TriggerTower* TTg1d1() const;

    //uint64_t id() const;
    //void setId(uint64_t iid);

    //uint32_t id() const; // changed october 29, 2021 to avoid conflict with TruthVertex_v1
    //void setId(uint32_t );
    int id() const;
    void setId(int );
    int side() const;
    void setSide(int );
    int zdcModule() const;
    void setZdcModule(int );
    int type() const;
    void setType(int );
    int channel() const;
    void setChannel(int );

    const std::vector<float>& waveformTime() const;
    void setWaveformTime(const std::vector<float>& );
    const std::vector<float>& waveformADC() const;
    void setWaveformADC(const std::vector<float>& );

    const std::vector<float>& waveformTimeG0() const;
    void setWaveformTimeG0(const std::vector<float>& );
    const std::vector<float>& waveformADCG0() const;
    void setWaveformADCG0(const std::vector<float>& );

    const std::vector<float>& waveformTimeG1() const;
    void setWaveformTimeG1(const std::vector<float>& );
    const std::vector<float>& waveformADCG1() const;
    void setWaveformADCG1(const std::vector<float>& );

    float amplitude() const;
    void setAmplitude(float );
    float energy() const;
    void setEnergy(float );
    float time() const;
    void setTime(float );
    float qual() const;
    void setQual(float );

    float amplitudeG0() const;
    void setAmplitudeG0(float );
    float energyG0() const;
    void setEnergyG0(float );
    float timeG0() const;
    void setTimeG0(float );
    float qualG0() const;
    void setQualG0(float );

    float amplitudeG1() const;
    void setAmplitudeG1(float );
    float energyG1() const;
    void setEnergyG1(float );
    float timeG1() const;
    void setTimeG1(float );
    float qualG1() const;
    void setQualG1(float );
    
      /// @}

   }; // class ZdcModule_v1

} // namespace xAOD

// Declare the inheritance of the class:
#include "xAODCore/BaseInfo.h"
SG_BASE(xAOD::ZdcModule_v1, SG::AuxElement);

#endif // XAODFORWARD_VERSIONS_ZDCMODULE_V1_H
