// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_Cosmic
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_Cosmic_H
#define SiSpacePointsSeedMaker_Cosmic_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

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

namespace Trk {
  class IPRD_AssociationTool;
}

namespace InDet {

  class SiSpacePointsSeedMaker_Cosmic : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_Cosmic
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_Cosmic();
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
    SiSpacePointsSeedMaker_Cosmic() = delete;
    SiSpacePointsSeedMaker_Cosmic(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    SiSpacePointsSeedMaker_Cosmic &operator=(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    //@}
      
    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
      
    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};

    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    bool m_endlist{true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", false};
    BooleanProperty m_checketa{this, "checkEta", false};
    int                         m_outputlevel {}                   ;
    int                         m_nprint {}                        ;
    bool m_state{0};
    int m_nspoint{2};
    int m_mode{0};
    int m_nlist{0};
    IntegerProperty m_maxsize{this, "maxSize", 100};
    int                         m_nseeds {}                        ;
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 1000.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.3};
    float                       m_dzdrmin {}                       ;
    float                       m_dzdrmax {}                       ;
    FloatProperty m_zmin{this, "minZ", -10000.};
    FloatProperty m_zmax{this, "maxZ", +10000.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 5.};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    float                       m_dazmax  {}                       ;
    FloatProperty m_diverpps{this, "maxdImpactPPS", 0.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 0.};
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_etamax{this, "etaMax", 0.};
    int m_r_size {}                                                ;
    int m_rf_size {}                                               ;
    int m_rfz_size {}                                              ;
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted{nullptr};
    std::list<InDet::SiSpacePointForSeed*>  m_rf_Sorted  [    53] ;
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [   583] ;
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed       ;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin     ;

    int m_ns{} ,m_nsaz {}                                             ;
    int m_fNmax  {}                                                ;
    int m_fNmin{}                                                  ;
    int m_zMin {}                                                  ;
    int  m_nr{}      ; int* m_r_index{nullptr}; int* m_r_map{nullptr};
    int  m_nrf{}     , m_rf_index   [ 53], m_rf_map   [ 53]        ;
    int  m_nrfz{}    , m_rfz_index  [583], m_rfz_map  [583]        ;
    int m_rfz_b[583],m_rfz_t[593],m_rfz_ib[583][12],m_rfz_it[583][12] ;
    float m_sF {}                                                  ;

    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    ///////////////////////////////////////////////////////////////////
     
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    InDet::SiSpacePointForSeed** m_SP{nullptr};
    float               *  m_R{nullptr};
    float               *  m_Tz{nullptr};
    float               *  m_Er{nullptr};
    float               *  m_U{nullptr};
    float               *  m_V{nullptr};

    std::multimap<float,InDet::SiSpacePointsSeed*>           m_l_seeds;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_i_seed ;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_i_seede;
    InDet::SiSpacePointsSeed*                                m_seeds{nullptr};

    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////
      
    SG::ReadHandleKey<SpacePointContainer>         m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer>         m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    ///////////////////////////////////////////////////////////////////
    // Protected methods
    ///////////////////////////////////////////////////////////////////

    MsgStream&    dumpConditions(MsgStream   & out) const;
    MsgStream&    dumpEvent     (MsgStream   & out) const;

    void buildFrameWork()                                       ;

    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&)                            ;
    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&,const float*)               ;
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&)                                              ;
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&)                      ;
    void fillLists     ()                                       ;
    void erase         ()                                       ;
    void production2Sp ()                                       ;
    void production3Sp ()                                       ;
    void production3Sp
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float,float)                                     ;

    void production3SpWithoutField()                            ;
    void production3SpWithoutField
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float)                                           ;


     
    bool isUsed(const Trk::SpacePoint*)                         ;
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_Cosmic&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_Cosmic&);

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_Cosmic::next()
  {
    if(m_i_seed==m_i_seede) return 0;
    SiSpacePointsSeed* sp = (*m_i_seed).second; ++m_i_seed;
    return(sp);
  }
    
  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_Cosmic::newSpacePoint
  (const Trk::SpacePoint*const& sp) 
  {
    SiSpacePointForSeed* sps;

    float r[3] = {float(sp->globalPosition().x()),
                  float(sp->globalPosition().y()),
                  float(sp->globalPosition().z())};

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
  // New space point for seeds with error correction
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_Cosmic::newSpacePoint
  (const Trk::SpacePoint*const& sp,const float* sc) 
  {
    SiSpacePointForSeed* sps;

    float r[3]; r[0]=sp->globalPosition().x(); r[1]=sp->globalPosition().y(); r[2]=sp->globalPosition().z();

    if(m_i_spforseed!=m_l_spforseed.end()) {
      sps = (*m_i_spforseed++); sps->set(sp,r,sc);
    }
    else                               {
      m_l_spforseed.push_back((sps=new SiSpacePointForSeed(sp,r,sc)));
      m_i_spforseed = m_l_spforseed.end();
    }
      
    return sps;
  }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_Cosmic::newSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
   const float& z) 
  {

    if(m_nseeds < m_maxsize) {

      m_seeds[m_nseeds].erase     (  );
      m_seeds[m_nseeds].add       (p1);
      m_seeds[m_nseeds].add       (p2);
      m_seeds[m_nseeds].setZVertex(0.);
      m_l_seeds.insert(std::make_pair(z,m_seeds+m_nseeds));
      ++m_nseeds;
    }
    else                     {

      std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = m_l_seeds.rbegin();
      if((*l).first <= z) return;
      InDet::SiSpacePointsSeed* s = (*l).second;
      m_l_seeds.erase((*l).first);

      s->erase     (  );
      s->add       (p1);
      s->add       (p2);
      s->setZVertex(0.);
      m_l_seeds.insert(std::make_pair(z,s));
    }
  }

  ///////////////////////////////////////////////////////////////////
  // New 3 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_Cosmic::newSeed
  (const Trk::SpacePoint*& p1,const Trk::SpacePoint*& p2, 
   const Trk::SpacePoint*& p3,const float& z) 
  {
    if(m_nseeds < m_maxsize) {

      m_seeds[m_nseeds].erase     (  );
      m_seeds[m_nseeds].add       (p1);
      m_seeds[m_nseeds].add       (p2);
      m_seeds[m_nseeds].add       (p3);
      m_seeds[m_nseeds].setZVertex(0.);
      m_l_seeds.insert(std::make_pair(z,m_seeds+m_nseeds));
      ++m_nseeds;
    }
    else                     {

      std::multimap<float,InDet::SiSpacePointsSeed*>::reverse_iterator l = m_l_seeds.rbegin();
      if((*l).first <= z) return;
      InDet::SiSpacePointsSeed* s = (*l).second;
      m_l_seeds.erase((*l).first);

      s->erase     (  );
      s->add       (p1);
      s->add       (p2);
      s->add       (p3);
      s->setZVertex(0.);
      m_l_seeds.insert(std::make_pair(z,s));
    }
  }
  
} // end of name space

#endif // SiSpacePointsSeedMaker_Cosmic_H
