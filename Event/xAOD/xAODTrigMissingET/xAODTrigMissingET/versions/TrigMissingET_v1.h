// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingET_v1.h 615398 2014-09-06 06:37:02Z florianb $
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
  /// $Revision: 615398 $
  /// $Date: 2014-09-06 08:37:02 +0200 (Sat, 06 Sep 2014) $
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


    /// @}
  };

}

#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigMissingET_v1, 76717345, 1 )
#endif // not XAOD_STANDALONE

#endif
