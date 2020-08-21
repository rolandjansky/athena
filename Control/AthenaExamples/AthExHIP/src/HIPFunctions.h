// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef ATHEXHIP_HIPFUNCTIONS_H
#define ATHEXHIP_HIPFUNCTIONS_H

// System include(s).
#include <iosfwd>
#include <vector>

namespace AthHIPExamples {

   /// Return information about the selected device
   void printHipDeviceInfo( int deviceId, std::ostream& out );

   /// Perform a linear transformation on an array
   void linearTransform( int deviceId, std::vector< float >& data,
                         float a, float b );

} // namespace AthHIPExamples

#endif // ATHEXHIP_HIPFUNCTIONS_H
