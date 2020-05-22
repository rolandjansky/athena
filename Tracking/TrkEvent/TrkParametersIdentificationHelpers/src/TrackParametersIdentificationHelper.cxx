/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkParametersIdentificationHelpers/TrackParametersIdentificationHelper.h"

namespace Trk {

  const TrackParametersIdHelper TrackParametersIdentificationHelper::m_helper;

  TrackParametersIdentifier TrackParametersIdentificationHelper::getIdentifier( const Trk::TrackParameters& pars ) {
    const CurvilinearParameters* cpars = dynamic_cast<const CurvilinearParameters*>(&pars);
    if( cpars) { return getIdentifier(*cpars);}
    return m_helper.encodeInValidIdentifier();
  }   

}
