// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TrigMissingETAuxContainer_v1.h 606381 2014-07-14 18:01:32Z florianb $
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
  /// $Revision: 606381 $
  /// $Date: 2014-07-14 20:01:32 +0200 (Mon, 14 Jul 2014) $
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
    /// @}
  };

}
// SET UP A CLID AND STOREGATE INHERITANCE FOR THE CLASS:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TrigMissingETAuxContainer_v1, 1292357238, 1 )
#include "SGTools/BaseInfo.h"
SG_BASE(   xAOD::TrigMissingETAuxContainer_v1, xAOD::AuxContainerBase );
#endif // not XAOD_STANDALONE

#endif
