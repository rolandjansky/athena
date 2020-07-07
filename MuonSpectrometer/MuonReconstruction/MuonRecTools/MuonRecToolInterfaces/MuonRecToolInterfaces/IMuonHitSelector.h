/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONHITSELECTOR_H
#define MUON_IMUONHITSELECTOR_H

#include "GaudiKernel/IAlgTool.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include <vector>

static const InterfaceID IID_IMuonHitSelector
    ("Muon::IMuonHitSelector",1,0);

namespace Trk {
  class PrepRawData;
  class	RIOsOnTrack;
  class	RIO_OnTrack;
  class	CompetingRIOsOnTrack;
}

namespace Muon {

  class IMuonHitSelector : virtual public IAlgTool 
  {      
    public:
    static const InterfaceID& interfaceID();
    
    /** Clean up phi hits
	The hits are divided into two categories
	- more or less clean hits associated with a segment or track candidate
	- hits in the vincinity of the initial road that are not associated with any segment or track candidate
    */
    
    virtual std::vector<const Trk::MeasurementBase*>* select_rio( const double pmom, const std::vector<const Trk::RIO_OnTrack*>& associatedHits,
								  const std::vector<const Trk::PrepRawData*>& unassociatedHits ) const = 0;

  };
  
  inline const InterfaceID& IMuonHitSelector::interfaceID()
    {
      return IID_IMuonHitSelector;
    }

} // end of name space

#endif // IMuonHitSelector_H

