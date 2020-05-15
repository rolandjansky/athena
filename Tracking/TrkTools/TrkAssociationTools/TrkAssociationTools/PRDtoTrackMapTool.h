/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRK_PRDtoTrackMapTool_H
#define TRK_PRDtoTrackMapTool_H

#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include <set>
#include <map>

namespace Trk {

  class Track;
  class PrepRawData;

  /** Concrete Implementation of the IPRD_AssociationTool interface.*/
  class PRDtoTrackMapTool : public extends<AthAlgTool, IPRDtoTrackMapTool>
{

  public:
    PRDtoTrackMapTool(const std::string&,const std::string&,const IInterface*);
    virtual ~PRDtoTrackMapTool()=default;
    virtual StatusCode initialize() override;

    virtual
    std::unique_ptr<Trk::PRDtoTrackMap> createPRDtoTrackMap() const override;
    virtual
    std::unique_ptr<Trk::PRDtoTrackMap> reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const override;

  /** add the PRDs from this track to the store
    @param track all PRDs from 'track' will be added to PRDtoTrackMapTool's internal store.*/
    StatusCode addPRDs(Trk::PRDtoTrackMap &virt_prd_to_track_map,  const Trk::Track& track ) const override;

  /**returns a vector of PRDs belonging to the passed track.
    It's basically for the convenience of users and is created purely from the passed track.
    i.e. there is no caching if you do it multiple times on the same track, you're being 
    inefficient!!
    @param track this Track will be iterated through and all PrepRawData added to a vector
    @return vector of PrepRawData* belonging to 'track'. The PrepRawData should NOT be deleted 
    - they belong to the Track (and thus the event).*/
    virtual std::vector< const PrepRawData* > getPrdsOnTrack(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                             const Track& track) const override;


    virtual IPRDtoTrackMapTool::TrackSet findConnectedTracks(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                            const Track& track) const override ;

  class PRDtoTrackMap : public Trk::PRDtoTrackMap
    {
      friend class PRDtoTrackMapTool;
    public:
      const std::type_info& getType() const override {return typeid(PRDtoTrackMap); }
    };

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  protected:
    void ensureType(Trk::PRDtoTrackMap &virt_prd_to_track_map) const ;

  };

}
#endif
