// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoIAuxContainer_v1.h 652818 2015-03-09 22:54:24Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMTOBROIAUXCONTAINER_V1_H
#define XAODTRIGL1CALO_VERSIONS_JEMTOBROIAUXCONTAINER_V1_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of JEMTobRoIAuxContainer_v1
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 652818 $
  /// $Date: 2015-03-09 23:54:24 +0100 (Mon, 09 Mar 2015) $

  class JEMTobRoIAuxContainer_v1 : public AuxContainerBase {
    public:
      /// Default constructor
      JEMTobRoIAuxContainer_v1();

    private:
      std::vector<uint32_t> roiWord;

  }; // class JEMTobRoIAuxContainer_v1
} // namespace xAOD

SG_BASE( xAOD::JEMTobRoIAuxContainer_v1 , xAOD::AuxContainerBase );

#endif

