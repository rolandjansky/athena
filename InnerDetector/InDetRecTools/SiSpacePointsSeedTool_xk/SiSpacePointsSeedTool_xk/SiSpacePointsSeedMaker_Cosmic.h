// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_Cosmic
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_Cosmic_H
#define SiSpacePointsSeedMaker_Cosmic_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

#include <iosfwd>
#include <list>
#include <vector>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MsgStream;

namespace InDet {

  using EventData = SiSpacePointsSeedMakerEventData;

  /**
   * @class SiSpacePointsSeedMaker_Cosmic
   * Class for track candidates generation using space points information
   * for standard Atlas geometry
   *
   * In AthenaMT, event dependent cache inside SiSpacePointsSeedMaker_Cosmic
   * is not preferred. SiSpacePointsSeedMakerEventData = EventData class
   * holds event dependent data for SiSpacePointsSeedMaker_Cosmic.
   * Its object is instantiated in SiSPSeededTrackFinder::execute.
   */

  class SiSpacePointsSeedMaker_Cosmic : 
    public extends<AthAlgTool, ISiSpacePointsSeedMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    /// @name Standard tool methods
    ///////////////////////////////////////////////////////////////////
    //@{
    SiSpacePointsSeedMaker_Cosmic
    (const std::string&, const std::string&, const IInterface*);
    virtual ~SiSpacePointsSeedMaker_Cosmic() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual void newEvent (const EventContext& ctx, EventData& data, int iteration) const override;
    virtual void newRegion(const EventContext& ctx, EventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const override;
    virtual void newRegion(const EventContext& ctx, EventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
                           const IRoiDescriptor& iRD) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Methods to initilize different strategies of seeds production
    ///////////////////////////////////////////////////////////////////
    //@{

    /// with two space points with or without vertex constraint
    virtual void find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const override;

    /// with three space points with or without vertex constraint
    virtual void find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const override;

    /// with three space points with or without vertex constraint
    /// with information about min and max Z of the vertex
    virtual void find3Sp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv, const double* zVertex) const override;

    /// with variable number space points with or without vertex constraint
    /// Variable means (2,3,4,....) any number space points
    virtual void findVSp(const EventContext& ctx, EventData& data, const std::list<Trk::Vertex>& lv) const override;
    //@}

    ///////////////////////////////////////////////////////////////////
    /// @name Iterator through seeds pseudo collection
    /// produced accordingly methods find    
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual const SiSpacePointsSeed* next(const EventContext& ctx, EventData& data) const override;
    //@}

    virtual void writeNtuple(const SiSpacePointsSeed* seed, const Trk::Track* track, int seedType, long eventNumber) const override;

    virtual bool getWriteNtupleBoolProperty() const override;

    ///////////////////////////////////////////////////////////////////
    /// @name Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////
    //@{
    virtual MsgStream& dump(EventData& data, MsgStream& out) const override;
    //@}

  private:
    /// enum for array sizes
    enum Size {SizeRF=53,
               SizeZ=11,
               SizeRFZ=SizeRF*SizeZ,
               SizeI=12};

    ///////////////////////////////////////////////////////////////////
    // Private data and methods
    ///////////////////////////////////////////////////////////////////

    /// @name Data handles
    //@{
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};
    SG::ReadHandleKey<Trk::PRDtoTrackMap>m_prdToTrackMap{this,"PRDtoTrackMap","","option PRD-to-track association"};
    // Read handle for conditions object to get the field cache
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                           "Name of the Magnetic Field conditions object key"};
    //@}

    /// @name Properties, which will not be changed after construction
    //@{
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    IntegerProperty m_maxsize{this, "maxSize", 100};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 1000.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 5.};
    //@}

    /// @name Properties, which can be updated in initialize
    //@{
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.3};
    //@}

    /// @name Properties, which are not used in this implementation of SiSpacePointsSeedMaker_Cosmic class
    //@{
    BooleanProperty m_checketa{this, "checkEta", false};
    FloatProperty m_zmin{this, "minZ", -10000.};
    FloatProperty m_zmax{this, "maxZ", +10000.};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 0.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 0.};
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_etamax{this, "etaMax", 0.};
    //@}

    /// @name Data members, which are updated in only initialize
    //@{
    bool m_initialized{false};
    int m_outputlevel{};
    int m_r_size{};
    int m_fNmax{};
    int m_rfz_b[SizeRFZ];
    int m_rfz_t[SizeRFZ];
    int m_rfz_ib[SizeRFZ][SizeI];
    int m_rfz_it[SizeRFZ][SizeI];
    float m_dzdrmin{};
    float m_dzdrmax{};
    float m_sF{};
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /// @name Disallow default instantiation, copy, assignment
    //@{
    SiSpacePointsSeedMaker_Cosmic() = delete;
    SiSpacePointsSeedMaker_Cosmic(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    SiSpacePointsSeedMaker_Cosmic &operator=(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    //@}

    MsgStream& dumpConditions(MsgStream& out) const;
    MsgStream& dumpEvent(EventData& data, MsgStream& out) const;

    void buildFrameWork();

    SiSpacePointForSeed* newSpacePoint
    (EventData& data, const Trk::SpacePoint*const&) const;
    SiSpacePointForSeed* newSpacePoint
    (EventData& data, const Trk::SpacePoint*const&, const float*) const;
    void newSeed
    (EventData& data,
     const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&) const;
    void newSeed
    (EventData& data,
     const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&) const;
    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;
    void production3Sp(const EventContext& ctx, EventData& data) const;
    void production3Sp
    (EventData& data,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float,float) const;

    void production3SpWithoutField(EventData& data) const;
    void production3SpWithoutField
    (EventData& data,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     std::vector<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float) const;

    bool isUsed(const Trk::SpacePoint*, const Trk::PRDtoTrackMap &prd_to_track_map) const;

    void initializeEventData(EventData& data) const;
  };

} // end of name space


namespace InDet {

  inline
  bool InDet::SiSpacePointsSeedMaker_Cosmic::isUsed(const Trk::SpacePoint* sp, const Trk::PRDtoTrackMap &prd_to_track_map) const {
    const Trk::PrepRawData* d = sp->clusterList().first;
    if (!d || !prd_to_track_map.isUsed(*d)) return false;

    d = sp->clusterList().second;
    if (!d || prd_to_track_map.isUsed(*d)) return true;

    return false;

  }
}

#endif // SiSpacePointsSeedMaker_Cosmic_H
