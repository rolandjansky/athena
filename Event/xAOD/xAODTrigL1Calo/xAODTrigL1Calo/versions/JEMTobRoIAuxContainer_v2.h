// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: JEMTobRoIAuxContainer_v2.h 639736 2015-01-15 15:27:50Z morrisj $
#ifndef XAODTRIGL1CALO_VERSIONS_JEMTOBROIAUXCONTAINER_V2_H
#define XAODTRIGL1CALO_VERSIONS_JEMTOBROIAUXCONTAINER_V2_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "xAODCore/AuxContainerBase.h"

namespace xAOD{

  /// Description of JEMTobRoIAuxContainer_v2
  ///
  /// @author John Morris <john.morris@cern.ch>
  ///
  /// $Revision: 639736 $
  /// $Date: 2015-01-15 16:27:50 +0100 (Thu, 15 Jan 2015) $

  class JEMTobRoIAuxContainer_v2 : public AuxContainerBase {
    public:
      /// Default constructor
      JEMTobRoIAuxContainer_v2();

    private:
      std::vector<uint32_t> roiWord;

  }; // class JEMTobRoIAuxContainer_v2
} // namespace xAOD

SG_BASE( xAOD::JEMTobRoIAuxContainer_v2 , xAOD::AuxContainerBase );

#endif

