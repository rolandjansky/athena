/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

namespace Trk {

  unsigned int           TrackParametersIdHelper::m_version = 0;
  BitField<unsigned int> TrackParametersIdHelper::m_validField(           31, 1 );

  BitField<unsigned int> TrackParametersIdHelper::m_versionField(         28, 3 );
  BitField<unsigned int> TrackParametersIdHelper::m_technologyField(      23, 5 );

  BitField<unsigned int> TrackParametersIdHelper::m_isEntryToLayerField(  5,  2 );
  BitField<unsigned int> TrackParametersIdHelper::m_caloSamplingField(    0,  5 );

}
 
