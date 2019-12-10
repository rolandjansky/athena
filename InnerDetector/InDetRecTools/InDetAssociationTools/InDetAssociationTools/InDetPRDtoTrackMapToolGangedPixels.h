/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef _InDet_InDetPRDtoTrackMapToolGangedPixels_h_
#define _InDet_InDetPRDtoTrackMapToolGangedPixels_h_

#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IPRDtoTrackMapTool.h"
#include "TrkEventUtils/PRDtoTrackMap.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"

class AtlasDetectorID;
class Identifier;

namespace InDet {

  /** Concrete Implementation of the IPRD_AssociationTool interface.*/
  class InDetPRDtoTrackMapToolGangedPixels : public extends<AthAlgTool, Trk::IPRDtoTrackMapTool>
{

  public:
    InDetPRDtoTrackMapToolGangedPixels(const std::string&,const std::string&,const IInterface*);

    virtual ~InDetPRDtoTrackMapToolGangedPixels ();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize  () override;

    virtual
    std::unique_ptr<Trk::PRDtoTrackMap> createPRDtoTrackMap() const override;
    virtual
    std::unique_ptr<Trk::PRDtoTrackMap> reduceToStorableMap(std::unique_ptr<Trk::PRDtoTrackMap> &&obj_in) const override;

  /** add the PRDs from this track to the store
    @param track all PRDs from 'track' will be added to PRD_AssociationTool's internal store.*/
    virtual StatusCode addPRDs(Trk::PRDtoTrackMap &virt_prd_to_track_map, const Trk::Track& track) const override;

  /**returns a vector of PRDs belonging to the passed track.
    It's basically for the convenience of users and is created purely from the passed track.
    i.e. there is no caching if you do it multiple times on the same track, you're being 
    inefficient!!
    @param track this Track will be iterated through and all PrepRawData added to a vector
    @return vector of PrepRawData* belonging to 'track'. The PrepRawData should NOT be deleted 
    - they belong to the Track (and thus the event).*/
    virtual std::vector< const Trk::PrepRawData* > getPrdsOnTrack(Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                                  const Trk::Track& track) const override;

  /** returns set of tracks which share PRD with this one
  @param track this Track must be known to this tool. 
  @return a set of tracks which share PRD/hits with the passed 'track'*/
  virtual Trk::IPRDtoTrackMapTool::TrackSet findConnectedTracks( Trk::PRDtoTrackMap &virt_prd_to_track_map,
                                                                const Trk::Track& track) const override;

  private:

    void ensureType(Trk::PRDtoTrackMap &virt_prd_to_track_map) const;

    SG::ReadHandleKey<PixelGangedClusterAmbiguities> m_pixelClusterAmbiguitiesMapName;

    /** add TRT outliers in the addTrack method to avoid splits due to rejected extensions */
    bool m_addTRToutliers;

    class PRDtoTrackMap : public Trk::PRDtoTrackMap
    {
      friend class InDetPRDtoTrackMapToolGangedPixels;
    public:
      const std::type_info& getType() const override {return typeid(PRDtoTrackMap); }

      PRDtoTrackMap(const SG::ReadHandleKey<PixelGangedClusterAmbiguities> &key);

    protected:
      SG::ReadHandle<PixelGangedClusterAmbiguities> m_gangedAmbis;
    };
  };



}
#endif
