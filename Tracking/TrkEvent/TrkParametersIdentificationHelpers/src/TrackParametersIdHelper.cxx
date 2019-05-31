/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

namespace Trk {

  const unsigned int           TrackParametersIdHelper::m_version = 0;
  const BitField<unsigned int> TrackParametersIdHelper::m_validField(           31, 1 );

  const BitField<unsigned int> TrackParametersIdHelper::m_versionField(         28, 3 );
  const BitField<unsigned int> TrackParametersIdHelper::m_technologyField(      23, 5 );

  const BitField<unsigned int> TrackParametersIdHelper::m_isEntryToLayerField(  5,  2 );
  const BitField<unsigned int> TrackParametersIdHelper::m_caloSamplingField(    0,  5 );

}
 
