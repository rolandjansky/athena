/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_TRACKPARAMETERSIDENTIFICATIONHELPER_H
#define TRK_TRACKPARAMETERSIDENTIFICATIONHELPER_H

#include "TrkParametersIdentificationHelpers/TrackParametersIdHelper.h"

#include "TrkParameters/TrackParameters.h"


namespace Trk {

  /** helper class to get the TrackParametersIdentifier from a TrackParameters */
  class TrackParametersIdentificationHelper {
  public:
    
    /** get Identifier from base-class */
    static TrackParametersIdentifier getIdentifier( const Trk::TrackParameters& pars );    

    /** get Identifier from CurvilinearParameters */
    static TrackParametersIdentifier getIdentifier( const CurvilinearParameters& pars );    
    
  
  private:
    static TrackParametersIdHelper m_helper;
  };

  inline TrackParametersIdentifier TrackParametersIdentificationHelper::getIdentifier( const CurvilinearParameters& pars ) {
    return pars.cIdentifier();
  }

  
}

#endif
