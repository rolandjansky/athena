/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACK_PARAMETERS_p2_TRK_H
#define TRACK_PARAMETERS_p2_TRK_H

//-----------------------------------------------------------------------------
//
// file:   TrackParameters_p2.h
//
//-----------------------------------------------------------------------------

#include <vector>
#include "Identifier/Identifier.h"

namespace Trk
{
   class TrackParameters_p2
   {
  public:
     TrackParameters_p2() : 
       m_surfaceType(4), m_associatedDetElementId(0)// Default is 'plane'
     {}
     
     // float                    m_parameters[5]; //!< FIXME - or store offset(s) to long list of floats in the _tl object ? 
     std::vector<float>         m_parameters;
     //----- Surface
     uint8_t                    m_surfaceType; //!< Used to recreate the correct TrackParameters
     Identifier::value_type     m_associatedDetElementId; // Corresponds to unsigned long long for 64bit.
     std::vector<float>         m_transform; // FIXME - better to have TPObjRef here? Or better to have fixed array?
     
     TPObjRef                   m_errorMatrix; // FIXME - is it better to still have measured/unmeasured track parameters? Or store numbers directly here?
   };
}

#endif // TRACK_PARAMETERS_p2_TRK_H
