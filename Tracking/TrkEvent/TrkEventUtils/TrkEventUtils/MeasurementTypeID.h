/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MeasurementTypeID.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
#ifndef TRKEVENTUTILS_MEASUREMENTTYPEID_H
#define TRKEVENTUTILS_MEASUREMENTTYPEID_H

#include "TrkEventPrimitives/TrackStateDefs.h"

class AtlasDetectorID;

namespace Trk {
  
  class MeasurementBase;
  
    /** @class MeasurementTypeID
     
        @brief classifies a MeasurementBase into one of the known inherited
               flavours or one of the detector types for ROT/CompROT.
               
        @author Wolfgang Liebig */

  class MeasurementTypeID {

  public:
    //! Constructor
    MeasurementTypeID(const AtlasDetectorID *id) : m_idHelper(id) {}

    /** createPerigee method - returned by object */
    Trk::TrackState::MeasurementType defineType(const MeasurementBase*) const;

  private:
    MeasurementTypeID() : m_idHelper(0) {}
    const AtlasDetectorID* m_idHelper;

 };
} // end of namespace


#endif  
