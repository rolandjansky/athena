/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EGAMMATOOLS_CONVVXSORTER_H
#define EGAMMATOOLS_CONVVXSORTER_H

#include "xAODTracking/Vertex.h"
#include "xAODEgamma/EgammaxAODHelpers.h"

/** Sort conversion vertices according to the following criteria:
  - Vertices with more Si tracks have priority
  - Vertices with more tracks have priority
  - Vertices with smaller radii have priority
  
  OLD SCHEME:
  - Vertices with 2 tracks have priority over the ones with 1 track
  - Vertices with Si + Si tracks have priority (if m_preferSi > 0)
  - Vertices with Si + TRT or TRT + TRT depending on m_preferSi
  - Vertices with smaller radii have priority
  **/ 
class ConvVxSorter
: public std::binary_function<xAOD::Vertex&, xAOD::Vertex&, bool> {
 public:
  bool operator()(const xAOD::Vertex& vx1, const xAOD::Vertex& vx2) const
  {
    xAOD::EgammaParameters::ConversionType convType1, convType2;
    convType1 = xAOD::EgammaHelpers::conversionType(&vx1);
    convType2 = xAOD::EgammaHelpers::conversionType(&vx2);
    
    if (convType1 != convType2)
    {
      // Different conversion type, preference to vertices with Si tracks
      int nSi1 = xAOD::EgammaHelpers::numberOfSiTracks(convType1);
      int nSi2 = xAOD::EgammaHelpers::numberOfSiTracks(convType2);
      if (nSi1 != nSi2) return nSi1 > nSi2;
      
      // Same number of Si tracks: either 0 or 1 (Si+TRT vs. Si single)
      // For 1 Si track, preference to Si+TRT
      if (nSi1 != 0) return convType1 == xAOD::EgammaParameters::doubleSiTRT;
      
      // No Si track, preference to doubleTRT over single TRT
      return convType1 == xAOD::EgammaParameters::doubleTRT;
    }
    
    // Same conversion type, preference to lower radius
    return (vx1.position().perp() < vx2.position().perp());
  }

};

#endif
