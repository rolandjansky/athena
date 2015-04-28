// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CPMTobRoIAuxContainer_v1.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_CPMTOBROIAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_CPMTOBROIAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of CPMTobRoIAuxContainer_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652818 $
  /// $Date: 2015-03-09 23:54:24 +0100 (Mon, 09 Mar 2015) $

  class CPMTobRoIAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      CPMTobRoIAuxContainer_v1();

    private:
      std::vector<uint32_t> roiWord;

  }; // class CPMTobRoIAuxContainer_v1
} // namespace xAOD

SG_BASE( xAOD::CPMTobRoIAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

