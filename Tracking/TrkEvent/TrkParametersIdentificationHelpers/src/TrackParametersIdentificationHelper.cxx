/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParametersIdentificationHelpers/TrackParametersIdentificationHelper.h"

namespace Trk {

  TrackParametersIdHelper TrackParametersIdentificationHelper::m_helper;

  TrackParametersIdentifier TrackParametersIdentificationHelper::getIdentifier( const Trk::TrackParameters& pars ) {
    const CurvilinearParameters* cpars = dynamic_cast<const CurvilinearParameters*>(&pars);
    if( cpars) return getIdentifier(*cpars);
    return m_helper.encodeInValidIdentifier();
  }   

}
