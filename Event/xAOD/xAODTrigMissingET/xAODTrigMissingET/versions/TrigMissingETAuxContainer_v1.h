// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETAuxContainer_v1.h 631541 2014-11-27 14:52:35Z gwatts $
#ifndef xAODTrigMissingET_TrigMissingETAUXCONTAINTER_V1_H
#define xAODTrigMissingET_TrigMissingETAUXCONTAINTER_V1_H

// System include(s):
#include <vector>
extern "C" {
#   include <stdint.h>
}

// EDM include(s): 
#include "xAODCore/AuxContainerBase.h"

namespace xAOD {

  /// AuxContainer for TrigMissingET classes.
  ///
  /// Storage of all items in the TrigMissingET classes. Includes vectors to hold onto
  /// an arbitrary list of TrigMissingET Components.
  ///
  /// @author Gordon Watts <gwatts@uw.edu>
  ///
  /// $Revision: 631541 $
  /// $Date: 2014-11-27 15:52:35 +0100 (Thu, 27 Nov 2014) $
  ///
  class TrigMissingETAuxContainer_v1 : public AuxContainerBase {

  public:
    /// Default constructor
    TrigMissingETAuxContainer_v1();

  private:
    /// @name Storage for basic kinematic properties
    /// @{
    std::vector<float> ex;
    std::vector<float> ey;
    std::vector<float> ez;
    std::vector<float> sumEt;
    std::vector<float> sumE;
    /// @}

    /// @name Storage for status words
    /// @{
    std::vector<int> flag;
    std::vector<uint32_t> roiWord;
    /// @}

    /// @name Storage for Component data  
    /// @{
    std::vector<std::vector<std::string> > nameOfComponent;
    std::vector<std::vector<float> > exComponent;
    std::vector<std::vector<float> > eyComponent;
    std::vector<std::vector<float> > ezComponent;
    std::vector<std::vector<float> > sumEtComponent;
    std::vector<std::vector<float> > sumEComponent;
    std::vector<std::vector<float> > calib0Component;
    std::vector<std::vector<float> > calib1Component;
    std::vector<std::vector<float> > sumOfSignsComponent; 
    std::vector<std::vector<short> > statusComponent; 
    std::vector<std::vector< unsigned short> > usedChannelsComponent;     
    /// @} 
  };

}

#include "xAODCore/BaseInfo.h"
SG_BASE(   xAOD::TrigMissingETAuxContainer_v1, xAOD::AuxContainerBase );

#endif
