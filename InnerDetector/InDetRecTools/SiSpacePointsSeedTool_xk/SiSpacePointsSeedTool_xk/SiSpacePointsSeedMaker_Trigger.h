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
    virtual ~SiSpacePointsSeedMaker_Trigger();
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
    ///////////////////////////////////////////////////////////////////
    // Private data and methods
    ///////////////////////////////////////////////////////////////////
      
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    bool                        m_endlist{true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    bool                        m_trigger{false};
    int                         m_outputlevel {};
    int                         m_nprint   {};
    int                         m_state{0};
    int                         m_nspoint{2};
    int                         m_mode{0};
    int                         m_nlist{0};
    IntegerProperty m_maxsize{this, "maxSize", 20000};
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
    FloatProperty m_r3max{this, "maxRadius3", 600.};
    FloatProperty m_drmin{this, "mindRadius", 10.};
    float m_drminv{20.};
    FloatProperty m_drmax{this, "maxdRadius", 270.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    float                       m_dzdrmin {};
    float                       m_dzdrmax {};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    float                       m_zminU {};
    float                       m_zmaxU  {};
    float                       m_zminB  {};
    float                       m_zmaxB  {};
    float                       m_ftrig  {};
    float                       m_ftrigW {};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};
    FloatProperty m_diversss{this, "maxdImpactSSS", 1000.};
    float                       m_dazmax{0.02};
    FloatProperty m_ptmin{this, "pTmin", 500.};
    float                       m_ipt      {};
    float                       m_ipt2     {};
    float                       m_COF     {};
    float                       m_K      {};
    int m_r_size {};
    int m_rf_size   {};
    int m_rfz_size  {};
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted{nullptr};
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [   583];
    std::list<InDet::SiSpacePointForSeed*>  m_rfzv_Sorted[   300];
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin;

    int m_ns{},m_nsaz{},m_nsazv   {};
    int m_fNmax{},m_fvNmax  {};
    int m_fNmin{},m_fvNmin{};
    int m_zMin  {};
    int  m_nr {}; int* m_r_index{nullptr}; int* m_r_map{nullptr};
    int  m_nrfz {}  , m_rfz_index  [583], m_rfz_map  [583];
    int  m_nrfzv {} , m_rfzv_index [300], m_rfzv_map [300];
    int m_rfz_b[583],m_rfz_t[583],m_rfz_ib[583][9],m_rfz_it[583][9];
    int m_rfzv_n[300],m_rfzv_i[300][6];
    float m_sF {};
    float m_sFv  {};

    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    ///////////////////////////////////////////////////////////////////
     
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    InDet::SiSpacePointForSeed** m_SP{nullptr};
    float               *  m_Zo{nullptr};
    float               *  m_Tz{nullptr};
    float               *  m_R{nullptr};
    float               *  m_U{nullptr};
    float               *  m_V{nullptr};
    float               *  m_Er{nullptr};

    std::list<InDet::SiSpacePointsSeed*>           m_l_seeds;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seed;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seede;

    std::multimap<float,InDet::SiSpacePointsSeed*> m_mapOneSeeds;
    std::multimap<float,InDet::SiSpacePointsSeed*> m_mapSeeds;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_seed;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_seede;

    InDet::SiSpacePointsSeed*                      m_OneSeeds{nullptr};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 3};
    int                                            m_nOneSeeds {};
    std::list<float>                               m_l_vertex;
 
    ///////////////////////////////////////////////////////////////////
    // Beam geometry
    ///////////////////////////////////////////////////////////////////
    float m_xbeam[4]{0., 1., 0., 0.}; // x,ax,ay,az - center and x-axis direction
    float m_ybeam[4]{0., 0., 1., 0.}; // y,ax,ay,az - center and y-axis direction
    float m_zbeam[4]{0., 0., 0., 1.}; // z,ax,ay,az - center and z-axis direction

    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////

    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_Trigger() = delete;
    SiSpacePointsSeedMaker_Trigger(const SiSpacePointsSeedMaker_Trigger&) = delete;
    SiSpacePointsSeedMaker_Trigger &operator=(const SiSpacePointsSeedMaker_Trigger&) = delete;
    //@}

    MsgStream&    dumpConditions(MsgStream   & out) const;
    MsgStream&    dumpEvent     (MsgStream   & out) const;

    void buildFrameWork();
    void buildBeamFrameWork();

    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&);
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&);

    void newOneSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&,const float&);
    void fillSeeds();

    void fillLists     ();
    void erase         ();
    void production2Sp ();
    void production3Sp ();
    void production3Sp
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,int&);
    void production3SpTrigger
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,int&);
 
    bool newVertices(const std::list<Trk::Vertex>&);
    void findNext();
    bool isZCompatible     (float&,float&,float&);
    void convertToBeamFrameWork(const Trk::SpacePoint*const&,float*);
    float dZVertexMin(float&);
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_Trigger&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_Trigger&);

} // end of name space

#endif // SiSpacePointsSeedMaker_Trigger_H
