// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_HeavyIon
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_HeavyIon_H
#define SiSpacePointsSeedMaker_HeavyIon_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <map>
#include <vector>

class MsgStream;

namespace InDet {

  class SiSpacePointsSeedMaker_HeavyIon : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_HeavyIon
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_HeavyIon();
    virtual StatusCode               initialize();
    virtual StatusCode               finalize  ();

    ///////////////////////////////////////////////////////////////////
    // Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////

    void newEvent (int);
    void newRegion
    (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&);
    void newRegion
    (const std::vector<IdentifierHash>&,const std::vector<IdentifierHash>&,const IRoiDescriptor&);
     
    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with two space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    void find2Sp (const std::list<Trk::Vertex>&);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with three space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    void find3Sp (const std::list<Trk::Vertex>&);
    void find3Sp (const std::list<Trk::Vertex>&,const double*);

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with variable number space points with or without vertex constraint
    // Variable means (2,3,4,....) any number space points
    ///////////////////////////////////////////////////////////////////
 
    void findVSp (const std::list<Trk::Vertex>&);
      
    ///////////////////////////////////////////////////////////////////
    // Iterator through seeds pseudo collection produced accordingly
    // methods find    
    ///////////////////////////////////////////////////////////////////
      
    const SiSpacePointsSeed* next();
      
    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    MsgStream&    dump          (MsgStream   & out) const;
    std::ostream& dump          (std::ostream& out) const;

  protected:
      
      
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_HeavyIon() = delete;
    SiSpacePointsSeedMaker_HeavyIon(const SiSpacePointsSeedMaker_HeavyIon&) = delete;
    SiSpacePointsSeedMaker_HeavyIon &operator=(const SiSpacePointsSeedMaker_HeavyIon&) = delete;
    //@}     
      
    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
      
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    bool                        m_endlist {true}                      ;
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    bool                        m_trigger  {false}                     ;
    bool                        m_izvertex  {}                    ;
    int                         m_outputlevel  {}                 ;
    int                         m_nprint  {}                      ;
    int                         m_state {0}                        ;
    int                         m_nspoint {2}                      ;
    int                         m_mode   {0}                       ;
    int                         m_nlist  {0}                       ;
    IntegerProperty m_maxsize{this, "maxSize", 20000};
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 1};
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
    float m_drminv {20.}              ;
    FloatProperty m_drmax{this, "maxdRadius", 270.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    float                       m_dzdrmin{}                       ;
    float                       m_dzdrmax {}                      ;
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    float                       m_zminU  {}                       ;
    float                       m_zmaxU  {}                       ;
    float                       m_ftrig {}                        ;
    float                       m_ftrigW {}                       ;
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};
    FloatProperty m_diversss{this, "maxdImpactSSS", 1000.};
    float                       m_dazmax  {.02}                      ;
    FloatProperty m_ptmin{this, "pTmin", 500.};
    float                       m_ipt   {}                        ;
    float                       m_ipt2  {}                        ;
    float                       m_COF {}                          ;
    float                       m_K  {}                           ;
    int m_r_size      {}                                            ;
    int m_rf_size  {}                                               ;
    int m_rfz_size  {}                                              ;
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted  {nullptr}          ;
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [   583] ;
    std::list<InDet::SiSpacePointForSeed*>  m_rfzv_Sorted[   300] ;
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed         ;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin     ;

    int m_ns{},m_nsaz{},m_nsazv {}                                    ;
    int m_fNmax{},m_fvNmax {}                                       ;
    int m_fNmin{},m_fvNmin    {}                                    ;
    int m_zMin{}                                                  ;
    int  m_nr{}     ; int* m_r_index{nullptr}   ; int* m_r_map{nullptr}                 ;
    int  m_nrfz {}  , m_rfz_index  [583], m_rfz_map  [583]            ;
    int  m_nrfzv{}  , m_rfzv_index [300], m_rfzv_map [300]            ;
    int m_rfz_b[583],m_rfz_t[593],m_rfz_ib[583][9],m_rfz_it[583][9]     ;
    int m_rfzv_n[300],m_rfzv_i[300][6]                              ;
    float m_sF{}                                                  ;
    float m_sFv {}                                                ;

    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    ///////////////////////////////////////////////////////////////////
     
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 4000};
    InDet::SiSpacePointForSeed** m_SP  {nullptr}                         ;
    float               *  m_Zo {nullptr}                                ;
    float               *  m_Tz {nullptr}                                ;
    float               *  m_R  {nullptr}                                ;
    float               *  m_U {nullptr}                                 ;
    float               *  m_V  {nullptr}                                ;
    float               *  m_Er {nullptr}                                ;

    std::list<InDet::SiSpacePointsSeed*>           m_l_seeds    ;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seed     ;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seede    ;

    std::multimap<float,InDet::SiSpacePointsSeed*> m_mapOneSeeds;
    InDet::SiSpacePointsSeed*                      m_OneSeeds {}  ;
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 6};
    int                                            m_nOneSeeds {} ;
    std::list<float>                               m_l_vertex   ;
 
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
    // Protected methods
    ///////////////////////////////////////////////////////////////////

    MsgStream&    dumpConditions(MsgStream   & out) const;
    MsgStream&    dumpEvent     (MsgStream   & out) const;

    void buildFrameWork()                                       ;
    void buildBeamFrameWork()                                   ;

    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&)                            ;
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&)                                              ;
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&)                      ;

    void newOneSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&,const float&)         ;
    void fillSeeds()                                            ;

    void fillLists     ()                                       ;
    void erase         ()                                       ;
    void production2Sp ()                                       ;
    void production3Sp ()                                       ;
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
    void production3SpNoVertex
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,int&);

    bool newVertices(const std::list<Trk::Vertex>&)             ;
    void findNext()                                             ;
    bool isZCompatible     (float&,float&,float&)               ;
    float dZVertexMin(float&)                                   ;
    void convertToBeamFrameWork(const Trk::SpacePoint*const&,float*)  ;
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_HeavyIon&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_HeavyIon&);

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_HeavyIon::next()
  {
    if(m_i_seed==m_i_seede) {findNext(); if(m_i_seed==m_i_seede) return 0;} 
    return(*m_i_seed++);
  }

  inline bool SiSpacePointsSeedMaker_HeavyIon::isZCompatible  
  (float& Zv,float& R,float& T)
  {
    if(Zv < m_zmin || Zv > m_zmax) return false;
    if(!m_izvertex               ) return true ;

    std::list<float>::iterator v=m_l_vertex.begin(),ve=m_l_vertex.end();

    float dZmin = fabs((*v)-Zv); ++v;

    for(; v!=ve; ++v) {
      float dZ = fabs((*v)-Zv); if(dZ<dZmin) dZmin=dZ;
    }
    return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
  }

  inline float SiSpacePointsSeedMaker_HeavyIon::dZVertexMin(float& Z)
  {
    float dZm = 1.E10;
    std::list<float>::iterator v=m_l_vertex.begin(),ve=m_l_vertex.end();

    for(; v!=ve; ++v) {float dZ = fabs((*v)-Z); if(dZ<dZm) dZm = dZ;}
    return dZm;
  }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_HeavyIon::newSpacePoint
  (const Trk::SpacePoint*const& sp) 
  {
    SiSpacePointForSeed* sps;

    float r[3]; convertToBeamFrameWork(sp,r);

    if(m_i_spforseed!=m_l_spforseed.end()) {
      sps = (*m_i_spforseed++); sps->set(sp,r);
    }
    else                               {
      m_l_spforseed.push_back((sps=new SiSpacePointForSeed(sp,r)));
      m_i_spforseed = m_l_spforseed.end();
    }
      
    return sps;
  }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::newSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
   const float& z) 
  {
    if(m_i_seede!=m_l_seeds.end()) {
      SiSpacePointsSeed* s = (*m_i_seede++);
      s->erase     (  );
      s->add       (p1);
      s->add       (p2);
      s->setZVertex(double(z));
    }
    else                  {
      m_l_seeds.push_back(new SiSpacePointsSeed(p1,p2,z));
      m_i_seede = m_l_seeds.end();
    }
  }

  ///////////////////////////////////////////////////////////////////
  // New 3 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::newSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
   const Trk::SpacePoint*& p3,const float& z) 
  {
    if(m_i_seede!=m_l_seeds.end()) {
      SiSpacePointsSeed* s = (*m_i_seede++);
      s->erase     (  );
      s->add       (p1);
      s->add       (p2);
      s->add       (p3);
      s->setZVertex(double(z));
    }
    else                  {
      m_l_seeds.push_back(new SiSpacePointsSeed(p1,p2,p3,z));
      m_i_seede = m_l_seeds.end();
    }
  }
  
  ///////////////////////////////////////////////////////////////////
  // Fill seeds
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_HeavyIon::fillSeeds ()
  {
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator 
      l  = m_mapOneSeeds.begin(),
      le = m_mapOneSeeds.end  ();

    for(; l!=le; ++l) {

      if(m_i_seede!=m_l_seeds.end()) {
        SiSpacePointsSeed* s = (*m_i_seede++);
        *s = *(*l).second;
      }
      else                  {
        m_l_seeds.push_back(new SiSpacePointsSeed(*(*l).second));
        m_i_seede = m_l_seeds.end();
      }
    }
  }
  
} // end of name space

#endif // SiSpacePointsSeedMaker_HeavyIon_H
