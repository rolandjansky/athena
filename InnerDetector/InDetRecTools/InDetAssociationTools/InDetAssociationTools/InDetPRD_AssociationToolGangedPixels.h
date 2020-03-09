/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDET_PRD_ASSOCIATIONTOOL_H
#define INDET_PRD_ASSOCIATIONTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "InDetPrepRawData/PixelGangedClusterAmbiguities.h"

class AtlasDetectorID;
class Identifier;

namespace InDet {

  /** Concrete Implementation of the IPRD_AssociationTool interface.*/
  class InDetPRD_AssociationToolGangedPixels : public extends<AthAlgTool, Trk::IPRD_AssociationTool>
{

  public:
    typedef Trk::IPRD_AssociationTool::Maps Maps;

    InDetPRD_AssociationToolGangedPixels(const std::string&,const std::string&,const IInterface*);
    virtual ~InDetPRD_AssociationToolGangedPixels ();
    virtual StatusCode initialize() override;
    virtual StatusCode finalize  () override;

  /** add the PRDs from this track to the store
    @param track all PRDs from 'track' will be added to PRD_AssociationTool's internal store.*/
    virtual StatusCode addPRDs(const Trk::Track& track) override;

  /** add the PRDs from this track to maps.
    @param track all PRDs from 'track' will be added to PRD_AssociationTool's internal store.*/
    virtual StatusCode addPRDs(Maps& maps, const Trk::Track& track) const override;

  /** remove the PRDs from this track from maps
    @param track all PRDs from 'track' will be removed from maps */
    virtual StatusCode removePRDs(Maps& maps, const Trk::Track& track) const override;

  /** remove the PRDs from this track from the store
    @param track all PRDs from 'track' will be removed from the PRD_AssociationTool's 
    internal store.*/
    virtual StatusCode removePRDs(const Trk::Track& track) override;

  /** does this PRD belong to at least one track in maps?
    @param prd the PrepRawData in question
    @return true if 'prd' exists in at least one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added to maps with addPRDs()*/
    virtual bool isUsed(const Maps& maps, const Trk::PrepRawData& prd) const override final;

  /** does this PRD belong to at least one track?
    @param prd the PrepRawData in question
    @return true if 'prd' exists in at least one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added with addPRDs()*/
    virtual bool isUsed(const Trk::PrepRawData& prd) const override;

  /** does this PRD belong to more than one track in maps?
    @param prd the PrepRawData in question
    @return true if 'prd' exists on more than one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added to maps with addPRDs()*/
    virtual bool isShared(const Maps& maps, const Trk::PrepRawData& prd) const override final;

  /** does this PRD belong to more than one track?
    @param prd the PrepRawData in question
    @return true if 'prd' exists on more than one track (of course PRD_AssociationTool can only
    give information about tracks it knows about i.e. that were added with addPRDs()*/
    virtual bool isShared(const Trk::PrepRawData& prd) const override;

  /**returns a vector of PRDs belonging to the passed track.
    It's basically for the convenience of users and is created purely from the passed track.
    i.e. there is no caching if you do it multiple times on the same track, you're being 
    inefficient!!
    @param track this Track will be iterated through and all PrepRawData added to a vector
    @return vector of PrepRawData* belonging to 'track'. The PrepRawData should NOT be deleted 
    - they belong to the Track (and thus the event).*/
    virtual std::vector< const Trk::PrepRawData* > getPrdsOnTrack(const Trk::Track& track) const override;

    // getPrdsOnTrack with state passed explicitly.
    virtual std::vector< const Trk::PrepRawData* > getPrdsOnTrack(const Maps& maps,
                                                                  const Trk::Track& track) const override;

  /** returns set of tracks which share PRD with this one
  @param track this Track must be known to this tool. 
  @return a set of tracks which share PRD/hits with the passed 'track'*/
    virtual Trk::IPRD_AssociationTool::TrackSet findConnectedTracks( const Trk::Track& track) const override;

    // findConnectedTracks with state passed explicitly.
    virtual IPRD_AssociationTool::TrackSet findConnectedTracks( const Maps& maps, const Trk::Track& track) const override;

  /** get the Tracks associated with this Trk::PrepRawData. 
    IMPORTANT: Please use the typedefs IPRD_AssociationTool::PrepRawDataRange and 
    IPRD_AssociationTool::ConstPRD_MapIt (defined in the interface) to access the 
    tracks, as the way the data is stored internally may change.*/
    virtual Trk::IPRD_AssociationTool::PrepRawDataTrackMapRange onTracks(const Trk::PrepRawData& prd) const override;

  // onTracks with explicit state
    virtual IPRD_AssociationTool::PrepRawDataTrackMapRange onTracks(const Maps& maps,
                                                                    const Trk::PrepRawData& prd) const override;

      /** resets the tool - should be called before using tool (and maybe afterwards to free up 
    memory)*/
    virtual void reset() override;

  private:
    Maps m_maps;

    SG::ReadHandleKey<PixelGangedClusterAmbiguities> m_pixelClusterAmbiguitiesMapName;

    /** add TRT outliers in the addTrack method to avoid splits due to rejected extensions */
    bool m_addTRToutliers;
    bool m_setupCorrect;

};

inline bool
InDet::InDetPRD_AssociationToolGangedPixels::isUsed(const Maps& maps,
                                                    const Trk::PrepRawData& prd) const
{
  return (maps.m_prepRawDataTrackMap.count(&prd)>0);
}

inline bool
InDet::InDetPRD_AssociationToolGangedPixels::isUsed(const Trk::PrepRawData& prd) const
{
  return isUsed (m_maps, prd);
}

inline bool
InDet::InDetPRD_AssociationToolGangedPixels::isShared(const Maps& maps,
                                                      const Trk::PrepRawData& prd) const
{
  return (maps.m_prepRawDataTrackMap.count(&prd)>1);
}

inline bool
InDet::InDetPRD_AssociationToolGangedPixels::isShared(const Trk::PrepRawData& prd) const
{
  return isShared (m_maps, prd);
}


}
#endif
