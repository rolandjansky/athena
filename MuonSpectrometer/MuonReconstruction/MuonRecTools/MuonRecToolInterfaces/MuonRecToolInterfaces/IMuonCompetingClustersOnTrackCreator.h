/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_IMUONCOMPETINGCLUSTERSONTRACKCREATOR_H
#define MUON_IMUONCOMPETINGCLUSTERSONTRACKCREATOR_H

#include "TrkCompetingRIOsOnTrackTool/ICompetingRIOsOnTrackTool.h"
#include <list>

static const InterfaceID IID_IMuonCompetingClustersOnTrackCreator
    ("Muon::IMuonCompetingClustersOnTrackCreator",1,0);

namespace Trk {
  class PrepRawData;
}

namespace Muon {

  class CompetingMuonClustersOnTrack;

  /** @brief Interface for tools creating CompetingMuonClustersOnTrack objects, inherits from ICompetingRIOsOnTrackTool.
   */
  class IMuonCompetingClustersOnTrackCreator : virtual public Trk::ICompetingRIOsOnTrackTool
  {      
    public:
    
    static const InterfaceID& interfaceID();
    
    /** @brief method to create a CompetingMuonClustersOnTrack using the average position of the hits and 
	       the distance between the clusters as error
	@param prds list of Trk::PrepRawData objects
	@param errorScaleFactor error scale factor
	@return a pointer to a new CompetingMuonClustersOnTrack, zero if creation failed.
	        The ownership of the new object is passed to the client calling the tool  
    */
    virtual const CompetingMuonClustersOnTrack* createBroadCluster(const std::list< const Trk::PrepRawData * > & prds , 
								   const double errorScaleFactor ) const = 0;

  };
  
  inline const InterfaceID& IMuonCompetingClustersOnTrackCreator::interfaceID()
  {
    return IID_IMuonCompetingClustersOnTrackCreator;
  }
} // end of name space

#endif // MUON_IMUONCOMPETINGCLUSTERSONTRACKCREATOR_H

