// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingET_v1.h 630930 2014-11-25 14:57:49Z gwatts $
#ifndef xAODTrigMissingET_TrigMissingET_V1_H
#define xAODTrigMissingET_TrigMissingET_V1_H

// STL include(s):
#include <string>
extern "C" {
#   include <stdint.h>
}

// EDM include(s):
#include "AthContainers/AuxElement.h"

namespace xAOD {

  /// Class holding the Missing ET trigger fex results.
  ///
  /// A class that holds the missing ET vector for the trigger. It further contains a
  /// list of components.
  ///
  /// @author Gordon Watts <gwatts@uw.edu>
  ///
  /// $Revision: 630930 $
  /// $Date: 2014-11-25 15:57:49 +0100 (Tue, 25 Nov 2014) $
  ///
  class TrigMissingET_v1 : public SG::AuxElement {
  public:
    /// Defaut constructor
    TrigMissingET_v1();

    /// @name Basic Kinematic Property accessors 
    /// @{

    /// Get the x component of the missing energy
    float ex() const;
    /// Set the x component of the missing energy
    void setEx (float s);

    /// Get the y component of the missing energy
    float ey() const;
    /// Set the y component of the missing energy
    void setEy (float s);

    /// Get the z component of the missing energy
    float ez() const;
    /// Set the z component of the missing energy
    void setEz (float s);

    /// Get the sum of the ET of the missing energy
    float sumEt() const;
    /// Set the sum of the ET of the missing energy
    void setSumEt (float s);

    /// Get the sum of the E of the missing energy
    float sumE() const;
    /// Set the sum of the ET of the missing energy
    void setSumE (float s);

    /// @}

    /// @name Status accessors
    /// @{
    
    /// Get the status flag
    int flag() const;
    /// Set the status flag
    void setFlag (int s);

    /// Get the ROI word
    uint32_t roiWord() const;
    /// Set the ROI word
    void setRoiWord (uint32_t s);
    /// @}

    /// @name MissingET Component accessors
    /// @{

    /// Define each of the components by name.
    ///
    /// This must be called first or an exception will be thrown if an attempt is
    /// made to set any component of the missing ET below. The components are stored
    /// in the order they are given.
    void defineComponents(const std::vector<std::string> &componentNames);

    /// Set the name of a component
    void setNameOfComponent(unsigned int index, const std::string &str);
    /// Get the name of a component
    const std::string& nameOfComponent(unsigned int index) const;

    /// Get the number of components
    unsigned int getNumberOfComponents() const;

    /// Set the x component fo the energy of a component
    void setExComponent(unsigned int index, float value);
    /// Get the x component fo the energy of a component
    float exComponent(unsigned int index) const;
    /// Set the y component fo the energy of a component
    void setEyComponent(unsigned int index, float value);
    /// Get the y component fo the energy of a component
    float eyComponent(unsigned int index) const;
    /// Set the z component fo the energy of a component
    void setEzComponent(unsigned int index, float value);
    /// Get the z component fo the energy of a component
    float ezComponent(unsigned int index) const;
    /// Set SumEt
    void setSumEtComponent(unsigned int index, float value);
    /// Get SumEt
    float sumEtComponent(unsigned int index) const;
    /// Set SumE
    void setSumEComponent(unsigned int index, float value);
    /// Get SumE
    float sumEComponent(unsigned int index) const;
    /// Set Calib0
    void setCalib0Component(unsigned int index, float value);
    /// Get Calib0
    float calib0Component(unsigned int index) const;
    /// Set Calib1
    void setCalib1Component(unsigned int index, float value);
    /// Get Calib1
    float calib1Component(unsigned int index) const;
    /// Set SumOfSigns
    void setSumOfSignsComponent(unsigned int index, float value);
    /// Get SumOfSigns
    float sumOfSignsComponent(unsigned int index) const;
    /// Set Status
    void setStatusComponent(unsigned int index, float value);
    /// Get Status
    short statusComponent(unsigned int index) const;
    /// Set used Channels
    void setUsedChannelsComponent(unsigned int index, float value);
    /// Get used Channels
    unsigned short usedChannelsComponent(unsigned int index) const;
    
    /// @}
  };

}

#endif
