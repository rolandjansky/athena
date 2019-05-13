// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_Trigger
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_Trigger_H
#define SiSpacePointsSeedMaker_Trigger_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <list>
#include <map>
#include <mutex>
#include <vector>

class MsgStream;

namespace InDet {

  class SiSpacePointsSeedMaker_Trigger : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_Trigger(const std::string&, const std::string&, const IInterface*);
    virtual ~SiSpacePointsSeedMaker_Trigger() = default;
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    ///////////////////////////////////////////////////////////////////
    // Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////

    virtual void newEvent(int iteration);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT, const IRoiDescriptor& ORD);
      
    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with two space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find2Sp(const std::list<Trk::Vertex>& lv);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with three space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find3Sp(const std::list<Trk::Vertex>& lv);
    virtual void find3Sp(const std::list<Trk::Vertex>& lv, const double* ZVertex);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with variable number space points with or without vertex constraint
    // Variable means (2,3,4,....) any number space points
    ///////////////////////////////////////////////////////////////////
 
    virtual void findVSp(const std::list<Trk::Vertex>& lv);
      
    ///////////////////////////////////////////////////////////////////
    // Iterator through seeds pseudo collection produced accordingly
    // methods find    
    ///////////////////////////////////////////////////////////////////
      
    virtual const SiSpacePointsSeed* next();
      
    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    virtual MsgStream&    dump(MsgStream   & out) const;
    virtual std::ostream& dump(std::ostream& out) const;

  private:
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
      
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};

    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

    // Properties, which will not be changed after construction
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    IntegerProperty m_maxsize{this, "maxSize", 20000};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 3};
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 99};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1minv{this, "minVRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r1maxv{this, "maxVRadius1", 60.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2minv{this, "minVRadius2", 70.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r2maxv{this, "maxVRadius2", 200.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_drmin{this, "mindRadius", 10.};
    FloatProperty m_drmax{this, "maxdRadius", 270.};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};
    FloatProperty m_diversss{this, "maxdImpactSSS", 1000.};

    // Properties, which can be updated in initialize
    FloatProperty m_r3max{this, "maxRadius3", 600.}; // Always overwriten by m_r_rmax
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    FloatProperty m_ptmin{this, "pTmin", 500.};

    // Not updated at all
    float m_drminv{20.};

    // Updated in only initialize
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
    float m_dzdrmin{0.};
    float m_dzdrmax{0.};
    float m_ipt{0.};
    float m_ipt2{0.};
    float m_COF{0.};
    float m_sF{0.};
    float m_sFv{0.};

    mutable std::mutex m_mutex;
    mutable std::vector<EventContext::ContextEvt_t> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
    struct EventData { // To hold event dependent data
      bool endlist{true};
      bool trigger{false};
      int nprint{0};
      int state{0};
      int nspoint{2};
      int mode{0};
      int nlist{0};
      int ns{0};
      int nsaz{0};
      int nsazv{0};
      int fNmin{0};
      int fvNmin{0};
      int zMin{0};
      int nr{0};
      int nrfz{0};
      int nrfzv{0};
      int nOneSeeds{0};
      int rfz_index[SizeRFZ];
      int rfz_map[SizeRFZ];
      int rfzv_index[SizeRFZV];
      int rfzv_map[SizeRFZV];
      float zminU{0.};
      float zmaxU{0.};
      float zminB{0.};
      float zmaxB{0.};
      float ftrig{0.};
      float ftrigW{0.};
      float K{0.};

      std::vector<int> r_index;
      std::vector<int> r_map;
      std::vector<std::list<InDet::SiSpacePointForSeed*>> r_Sorted;
      std::vector<InDet::SiSpacePointsSeed> OneSeeds;
      std::multimap<float,InDet::SiSpacePointsSeed*> mapOneSeeds;
      std::multimap<float,InDet::SiSpacePointsSeed*> mapSeeds;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator seed;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator seede;
      std::list<InDet::SiSpacePointForSeed*> rfz_Sorted[SizeRFZ];
      std::list<InDet::SiSpacePointForSeed*> rfzv_Sorted[SizeRFZV];
      std::list<InDet::SiSpacePointForSeed> l_spforseed;
      std::list<InDet::SiSpacePointForSeed>::iterator i_spforseed;
      std::list<InDet::SiSpacePointForSeed*>::iterator rMin;
      std::list<InDet::SiSpacePointsSeed> l_seeds;
      std::list<InDet::SiSpacePointsSeed>::iterator i_seed;
      std::list<InDet::SiSpacePointsSeed>::iterator i_seede;
      std::list<float> l_vertex;
      ///////////////////////////////////////////////////////////////////
      // Tables for 3 space points seeds search
      ///////////////////////////////////////////////////////////////////
      std::vector<InDet::SiSpacePointForSeed*> SP;
      std::vector<float> R;
      std::vector<float> Tz;
      std::vector<float> Er;
      std::vector<float> U;
      std::vector<float> V;
      std::vector<float> Zo;
      ///////////////////////////////////////////////////////////////////
      // Beam geometry
      ///////////////////////////////////////////////////////////////////
      float xbeam[4]{0., 1., 0., 0.}; // x,ax,ay,az - center and x-axis direction
      float ybeam[4]{0., 0., 1., 0.}; // y,ax,ay,az - center and y-axis direction
      float zbeam[4]{0., 0., 0., 1.}; // z,ax,ay,az - center and z-axis direction
    };
    mutable std::vector<EventData> m_eventData ATLAS_THREAD_SAFE; // Guarded by m_mutex

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_Trigger() = delete;
    SiSpacePointsSeedMaker_Trigger(const SiSpacePointsSeedMaker_Trigger&) = delete;
    SiSpacePointsSeedMaker_Trigger &operator=(const SiSpacePointsSeedMaker_Trigger&) = delete;
    //@}

    MsgStream& dumpConditions(EventData& data, MsgStream& out) const;
    MsgStream& dumpEvent(EventData& data, MsgStream& out) const;

    void buildFrameWork();
    void buildBeamFrameWork(EventData& data) const;

    SiSpacePointForSeed* newSpacePoint
    (EventData& data, const Trk::SpacePoint*const&) const;
    void newSeed
    (EventData& data,
     const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&) const;

    void newOneSeed
    (EventData& data,
     const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&,const float&) const;
    void fillSeeds(EventData& data) const;

    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;
    void production3Sp(EventData& data) const;
    void production3Sp
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,int&) const;
    void production3SpTrigger
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,int&) const;
 
    bool newVertices(EventData& data, const std::list<Trk::Vertex>&) const;
    void findNext(EventData& data) const;
    bool isZCompatible(EventData& data, float&, float&, float&) const;
    void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const&, float*) const;
    float dZVertexMin(EventData& data, float&) const;

    EventData& getEventData() const;
  };

  MsgStream& operator << (MsgStream&, const SiSpacePointsSeedMaker_Trigger&);
  std::ostream& operator << (std::ostream&, const SiSpacePointsSeedMaker_Trigger&);

} // end of name space

#endif // SiSpacePointsSeedMaker_Trigger_H
