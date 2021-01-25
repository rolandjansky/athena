/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONTRACKREFINER_H
#define MUON_IMUONTRACKREFINER_H

#include <vector>
#include <memory>
#include "GaudiKernel/IAlgTool.h"


static const InterfaceID IID_IMuonTrackRefiner
    ("Muon::IMuonTrackRefiner",1,0);

namespace Trk {
  class Track;
  class MeasurementBase;
}

namespace Muon {

  class MuPatTrack;
  class MuPatHit;
  
  /** @brief The IMuonTrackRefiner is a pure virtual interface for tools which refine the hit content of a given track

      The following interface is available.
      @code
         MuPatTrack* refine( MuPatTrack& track );
      @endcode					      

  */  
  class IMuonTrackRefiner : virtual public IAlgTool 
  {      
    public:
    /** access to tool interface */
    static const InterfaceID& interfaceID();

    /** @brief interface for tools which refine the hit content of a given track
	@param track input track
        @return new refined track. Pointer could be zero, ownership passed to caller
    */
    virtual void refine( MuPatTrack& track,
                         std::vector<std::unique_ptr<MuPatHit> >& hitsToBeDeleted,
                         std::vector<std::unique_ptr<const Trk::MeasurementBase> >& measurementsToBeDeleted ) const = 0;

  };
  
  inline const InterfaceID& IMuonTrackRefiner::interfaceID()
  {
    return IID_IMuonTrackRefiner;
  }
} // end of name space

#endif // IMuonTrackRefiner_H
