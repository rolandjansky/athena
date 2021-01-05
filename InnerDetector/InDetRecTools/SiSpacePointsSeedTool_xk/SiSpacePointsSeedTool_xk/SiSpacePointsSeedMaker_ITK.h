// -*- C++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ITK
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ITK_H
#define SiSpacePointsSeedMaker_ITK_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "SiSPSeededTrackFinderData/SiSpacePointForSeedITK.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MagField cache
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iosfwd>
#include <list>
#include <vector>

class MsgStream;


namespace InDet {

  using EventData = SiSpacePointsSeedMakerEventData;

  /**
   * @class SiSpacePointsSeedMaker_ATLxk
   * Class for track candidates generation using space points information
   * for standard Atlas geometry
   *
   * In AthenaMT, event dependent cache inside SiSpacePointsSeedMaker_ITK
   * is not preferred. SiSpacePointsSeedMakerEventData = EventData class
   * holds event dependent data for SiSpacePointsSeedMaker_ITK.
   * Its object is instantiated in SiSPSeededTrackFinder::execute.
   */

  class SiSpacePointsSeedMaker_ITK : 
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
    SiSpacePointsSeedMaker_ITK
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_ITK() = default;
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
               SizeI=9,
               SizeRFV=100,
               SizeZV=3,
               SizeRFZV=SizeRFV*SizeZV,
               SizeIV=6};

    ///////////////////////////////////////////////////////////////////
    // Private data and methods
    ///////////////////////////////////////////////////////////////////

    /// @name Data handles
    //@{
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints", "SCT space points container"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints", "Pixel space points container"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};
    SG::ReadHandleKey<Trk::PRDtoTrackMap> m_prdToTrackMap{this,"PRDtoTrackMap","","option PRD-to-track association"};
    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey<AtlasFieldCacheCondObj> m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
                                                                           "Name of the Magnetic Field conditions object key"};
    //@}

    /// @name Properties, which will not be changed after construction
    //@{
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    IntegerProperty m_maxsize{this, "maxSize", 50000};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 5000};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    FloatProperty m_etamax{this, "etaMax", 2.7};
    FloatProperty m_r1minv{this, "minVRadius1", 0.};
    FloatProperty m_r1maxv{this, "maxVRadius1", 60.};
    FloatProperty m_r2minv{this, "minVRadius2", 70.};
    FloatProperty m_r2maxv{this, "maxVRadius2", 200.};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 300.};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this , "maxZ", +250.};
    FloatProperty m_r_rmin{this, "radMin", 0.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 20.};
    FloatProperty m_divermax{this, "maxdImpactForDecays", 20.};
    FloatProperty m_dzmaxPPP{this, "dZmaxForPPPSeeds", 600.};
    //@}

    /// @name Properties, which will be updated in initialize
    //@{
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_r_rmax{this, "radMax", 1100.};
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_umax{this, "minSeedsQuality", 0.};
    //@}

    /// @name Properties, which will be updated in event methods, checketa is prepared in EventData.
    //@{
    BooleanProperty m_checketa{this, "checkEta", false};
    //@}

    /// @name Properties, which are not used in this implementation of SiSpacePointsSeedMaker_ITK class
    //@{
    BooleanProperty m_dbm{this, "useDBM", false};
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 99};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_r3max{this, "maxRadius3", 600.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};
    //@}

    /// @name Data member, which is not updated at all
    //@{
    float m_drminv{20.};
    //@}

    /// @name Data members, which are updated only in initialize
    //@{
    bool m_initialized{false};
    int m_outputlevel{0};
    int m_r_size{0};
    int m_fNmax{0};
    int m_fvNmax{0};
    int m_rfz_b[SizeRFZ];
    int m_rfz_t[SizeRFZ];
    int m_rfz_ib[SizeRFZ][SizeI];
    int m_rfz_it[SizeRFZ][SizeI];
    int m_rfzv_n[SizeRFZV];
    int m_rfzv_i[SizeRFZV][SizeIV];
    float m_dzdrmin0{0.};
    float m_dzdrmax0{0.};
    float m_ipt{0.};
    float m_ipt2{0.};
    float m_COF{0.};
    float m_sF{0.};
    float m_sFv{0.};
    //@}

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /// @name Disallow default instantiation, copy, assignment
    //@{
    SiSpacePointsSeedMaker_ITK() = delete;
    SiSpacePointsSeedMaker_ITK(const SiSpacePointsSeedMaker_ITK&) = delete;
    SiSpacePointsSeedMaker_ITK &operator=(const SiSpacePointsSeedMaker_ITK&) = delete;
    //@}

    MsgStream& dumpConditions(EventData& data, MsgStream& out) const;
    MsgStream& dumpEvent(EventData& data, MsgStream& out) const;

    void buildFrameWork();
    void buildBeamFrameWork(EventData& data) const;

    SiSpacePointForSeedITK* newSpacePoint
    (EventData& data, const Trk::SpacePoint*const&) const;
    void newSeed
    (EventData& data,
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float) const;

    void newOneSeed
    (EventData& data, 
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,
     SiSpacePointForSeedITK*&,float,float) const;

    void newOneSeedWithCurvaturesComparison
    (EventData& data,
     SiSpacePointForSeedITK*&,SiSpacePointForSeedITK*&,float) const;

    void fillSeeds(EventData& data) const;
    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;
    void production3Sp(EventData& data) const;
    void production3SpSSS
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
    void production3SpPPP
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
    void production3SpTrigger
    (EventData& data,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     std::list<InDet::SiSpacePointForSeedITK*>::iterator*,
     int,int,int&) const;
 
    bool newVertices(EventData& data, const std::list<Trk::Vertex>&) const;
    void findNext(EventData& data) const;
    bool isZCompatible(EventData& data, float&,float&,float&) const;
    void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const&,float*) const;
    bool isUsed(const Trk::SpacePoint*, const Trk::PRDtoTrackMap &prd_to_track_map) const;

    void initializeEventData(EventData& data) const;
  };

} // end of name space

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvatureITK {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeedITK*>& i1, 
   const std::pair<float,InDet::SiSpacePointForSeedITK*>& i2)
  {
    return i1.first < i2.first;
  }
};


///////////////////////////////////////////////////////////////////
// Test is space point used
///////////////////////////////////////////////////////////////////

namespace InDet {
  inline
  bool SiSpacePointsSeedMaker_ITK::isUsed(const Trk::SpacePoint* sp, const Trk::PRDtoTrackMap &prd_to_track_map) const
  {
    const Trk::PrepRawData* d = sp->clusterList().first;
    if (!d || !prd_to_track_map.isUsed(*d)) return false;

    d = sp->clusterList().second;
    if (!d || prd_to_track_map.isUsed(*d)) return true;

    return false;
  }
}


#endif // SiSpacePointsSeedMaker_ITK_H
