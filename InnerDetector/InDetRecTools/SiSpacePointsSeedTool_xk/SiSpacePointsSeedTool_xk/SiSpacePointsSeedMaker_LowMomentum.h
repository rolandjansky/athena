// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_LowMomentum
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_LowMomentum_H
#define SiSpacePointsSeedMaker_LowMomentum_H

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

namespace Trk {
  class IPRD_AssociationTool;
}

namespace InDet {

  class SiSpacePointsSeedMaker_LowMomentum : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_LowMomentum
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_LowMomentum();
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
    SiSpacePointsSeedMaker_LowMomentum() = delete;
    SiSpacePointsSeedMaker_LowMomentum(const SiSpacePointsSeedMaker_LowMomentum&) = delete;
    SiSpacePointsSeedMaker_LowMomentum &operator=(const SiSpacePointsSeedMaker_LowMomentum&) = delete;
    //@}    
      
    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
      
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    bool m_endlist{true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", true};
    bool                        m_trigger  {}                     ;
    int                         m_outputlevel  {}                 ;
    int                         m_nprint {}                       ;
    int m_state{0};
    int m_nspoint{2};
    int m_mode{0};
    int m_nlist{0};
    IntegerProperty m_maxsize{this, "maxSize", 2000};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_r3max{this, "maxRadius3", 600.};
    FloatProperty m_drmin{this, "mindRadius", 10.};
    FloatProperty m_drmax{this, "maxdRadius", 200.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    float                       m_dzdrmin  {}                     ;
    float                       m_dzdrmax  {}                     ;
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    float                       m_zminU   {}                      ;
    float                       m_zmaxU   {}                      ;
    FloatProperty m_r_rmax{this, "radMax", 200.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 7.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.2};
    float                       m_dazmax{0.02};
    FloatProperty m_ptmin{this, "pTmin", 100.};
    FloatProperty m_ptmax{this, "pTmax", 500.};
    float                       m_iptmin   {}                     ;
    float                       m_iptmax{1./400.};

    int m_r_size   {}                                               ;
    int m_rf_size   {}                                              ;
    int m_rfz_size   {}                                             ;
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted{nullptr};
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [  220]  ;
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed         ;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin     ;
  
    int m_ns{},m_nsaz  {}                                           ;
    int m_fNmax  {}                                               ;
    int m_fNmin  {}                                               ;
    int m_zMin  {}                                                ;
    int  m_nr  {}   ; int* m_r_index{nullptr}; int* m_r_map{nullptr};
    int  m_nrfz {}  , m_rfz_index  [220], m_rfz_map  [220]            ;
    int m_rfz_b[220],m_rfz_t[220],m_rfz_ib[220][9],m_rfz_it[220][9]     ;
    float m_sF {}                                                ;

    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    ///////////////////////////////////////////////////////////////////
     
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    InDet::SiSpacePointForSeed** m_SP{nullptr};
    float                *  m_R{nullptr};
    float                *  m_Tz{nullptr};
    float                *  m_Er{nullptr};
    float                *  m_U{nullptr};
    float                *  m_V{nullptr};
    float                *  m_Zo{nullptr};

    std::list<InDet::SiSpacePointsSeed*>           m_l_seeds      ;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seed       ;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seede      ;

    std::multimap<float,InDet::SiSpacePointsSeed*> m_mapOneSeeds;
    InDet::SiSpacePointsSeed*                       m_OneSeeds{nullptr};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    int                                             m_nOneSeeds {};
    std::list<float>                                m_l_vertex  ;
 
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

    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};

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
     int,int,int&,float)                                      ;
     
    bool newVertices(const std::list<Trk::Vertex>&)             ;
    void findNext()                                             ;
    bool isZCompatible     (float&,float&,float&)               ;
    void convertToBeamFrameWork(const Trk::SpacePoint*const&,float*)  ;
    bool isUsed(const Trk::SpacePoint*);
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_LowMomentum&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_LowMomentum&);

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_LowMomentum::next()
  {
    if(m_i_seed==m_i_seede) {
      findNext();
      if(m_i_seed==m_i_seede) return 0;
    } 
    return(*m_i_seed++);
  }

  inline bool SiSpacePointsSeedMaker_LowMomentum::isZCompatible  
  (float& Zv,float& R,float& T)
  {
    if(Zv < m_zmin || Zv > m_zmax) return false;

    std::list<float>::iterator v=m_l_vertex.begin(),ve=m_l_vertex.end();
    if(v==ve) return true;

    float dZmin = fabs((*v)-Zv); ++v;

    for(; v!=ve; ++v) {
      float dZ = fabs((*v)-Zv); if(dZ<dZmin) dZmin=dZ;
    }
    return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
  }
  
  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_LowMomentum::newSpacePoint
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

  inline void SiSpacePointsSeedMaker_LowMomentum::newSeed
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

  inline void SiSpacePointsSeedMaker_LowMomentum::newSeed
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

  inline void SiSpacePointsSeedMaker_LowMomentum::fillSeeds ()
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

#endif // SiSpacePointsSeedMaker_LowMomentum_H
