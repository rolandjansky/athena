// -*- C++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/////////////////////////////////////////////////////////////////////////////////
//  Header file for class SiSpacePointsSeedMaker_ATLxk
/////////////////////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
/////////////////////////////////////////////////////////////////////////////////
// Class for track candidates generation using space points information
// for standard Atlas geometry
/////////////////////////////////////////////////////////////////////////////////
// Version 1.0 3/10/2004 I.Gavrilenko
/////////////////////////////////////////////////////////////////////////////////

#ifndef SiSpacePointsSeedMaker_ATLxk_H
#define SiSpacePointsSeedMaker_ATLxk_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointForSeed.h"
#include "SiSpacePointsSeedTool_xk/SiSpacePointsProSeed.h" 
#include "TrkToolInterfaces/IPRD_AssociationTool.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <map>
#include <set>
#include <vector>

class MsgStream;

namespace InDet {


  class SiSpacePointsSeedMaker_ATLxk : 
    virtual public ISiSpacePointsSeedMaker, public AthAlgTool
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_ATLxk
    (const std::string&,const std::string&,const IInterface*);
    virtual ~SiSpacePointsSeedMaker_ATLxk();
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

    MsgStream&    dump(MsgStream   & out) const;
    std::ostream& dump(std::ostream& out) const;

  protected:
    ///////////////////////////////////////////////////////////////////
    // Protected data and methods
    ///////////////////////////////////////////////////////////////////
  
    ServiceHandle<MagField::IMagFieldSvc> m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};

    ///////////////////////////////////////////////////////////////////
    // Space points containers
    ///////////////////////////////////////////////////////////////////
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixelKey{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCTKey{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlapKey{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey { this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot" };

    // Properties, which will not be changed after construction
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_dbm{this, "useDBM", false};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", true};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", false};
    IntegerProperty m_maxsize{this, "maxSize", 50000};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 5000};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    FloatProperty m_etamax{this, "etaMax", 2.7};
    FloatProperty m_r1minv{this, "minVRadius1", 0.};
    FloatProperty m_r1maxv{this, "maxVRadius1", 60.};
    FloatProperty m_r2minv{this, "minVRadius2", 70.};
    FloatProperty m_r2maxv{this, "maxVRadius2", 200.};
    FloatProperty m_drmax{this, "maxdRadius", 270.};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", .02};

    // Properties, which can be updated in buildFrameWork method which is called in initialize method
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_r3max{this, "maxRadius3", 600.}; // This is always overwritten by m_r_rmax.
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 50.};
    FloatProperty m_divermax{this, "maxdImpactForDecays", 20.};
    FloatProperty m_ptmin{this, "pTmin", 500.};

    // Properties, which can be updated in newEvent method
    BooleanProperty m_checketa{this, "checkEta", false};

    // Properties, which are not used in this implementation of SiSpacePointsSeedMaker_ATLxk class
    UnsignedIntegerProperty m_maxNumberVertices{this, "maxNumberVertices", 99};
    FloatProperty m_r1min{this, "minRadius1", 0.};
    FloatProperty m_r1max{this, "maxRadius1", 600.};
    FloatProperty m_r2min{this, "minRadius2", 0.};
    FloatProperty m_r2max{this, "maxRadius2", 600.};
    FloatProperty m_r3min{this, "minRadius3", 0.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.7};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.7};

    // Not be updated at all.
    float m_drminv{20.};

    // Updated only in initialize
    int m_outputlevel{};

    // Updated only in buildFrameWork in initialize
    float                       m_dzdrmin0{}                      ;
    float                       m_dzdrmax0{}                      ;
    float                       m_ipt{}                           ;
    float                       m_ipt2{}                          ;
    float                       m_COF{}                           ;
    int m_r_size {}                                               ;
    int m_fNmax{},m_fvNmax{}                                      ;
    int m_rfz_b[583],m_rfz_t[583],m_rfz_ib[583][9],m_rfz_it[583][9];
    int m_rfzv_n[300],m_rfzv_i[300][6]                            ;
    float m_sF{}                                                  ;
    float m_sFv{}                                                 ;

    // Updated in buildFramework, newEvent, newRegion methods
    float m_K{};
    int m_ns{};
    int m_nr{};
    int* m_r_index{nullptr};
    int* m_r_map{nullptr};
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted{nullptr};

    // Updated in buildFramework and other many mthods
    int m_nsaz{}, m_nsazv{}                                       ;
    int m_nrfz{},  m_rfz_index  [583], m_rfz_map  [583]           ;
    int m_nrfzv{}, m_rfzv_index [300], m_rfzv_map [300]           ;
    std::list<InDet::SiSpacePointsProSeed*>           m_l_seeds   ;
    std::list<InDet::SiSpacePointsProSeed*>::iterator m_i_seed    ;
    std::list<InDet::SiSpacePointsProSeed*>::iterator m_i_seede   ;
    InDet::SiSpacePointsSeed*                m_seedOutput{nullptr};
    InDet::SiSpacePointsProSeed*               m_OneSeeds{nullptr};
    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    // Updated in buildFramework and other many mthods
    ///////////////////////////////////////////////////////////////////
    InDet::SiSpacePointForSeed** m_SP{nullptr}                    ;
    float               *  m_Zo{nullptr}                          ;
    float               *  m_Tz{nullptr}                          ;
    float               *  m_R{nullptr}                           ;
    float               *  m_U{nullptr}                           ;
    float               *  m_V{nullptr}                           ;
    float               *  m_Er{nullptr}                          ;

    // Updated only in newEvent and newRegion
    float              m_dzdrmin{}; // Always equals to m_dzdrmin0
    float              m_dzdrmax{}; // Always equals to m_dzdrmax0
    float              m_ipt2C{}; // Always equals to m_ipt2*m_COF
    bool               m_trigger{false}                  ;
    int                m_iteration{}                     ;
    float              m_ipt2K{}                         ;
    float              m_COFK{}                          ;
    int                m_r_first{}                       ;

    ///////////////////////////////////////////////////////////////////
    // Beam geometry
    // Updated only in buildBeamFrameWork,
    // which is called by newEvent and newRegion
    ///////////////////////////////////////////////////////////////////
    float m_xbeam[4]{0., 1., 0., 0.}; // x,ax,ay,az - center and x-axis direction
    float m_ybeam[4]{0., 0., 1., 0.}; // y,ax,ay,az - center and y-axis direction
    float m_zbeam[4]{0., 0., 0., 1.}; // z,ax,ay,az - center and z-axis direction

    // Updated in many methods
    bool                        m_endlist{true}                   ;
    bool                        m_isvertex{false}                 ;
    int                         m_nprint{}                        ;
    int                         m_state{0}                        ;
    int                         m_nspoint{2}                      ;
    int                         m_mode{0}                         ;
    int                         m_nlist{0}                        ;
    float                       m_zminU{}                         ;
    float                       m_zmaxU{}                         ;
    float                       m_zminB{}                         ;
    float                       m_zmaxB{}                         ;
    float                       m_ftrig{}                         ;
    float                       m_ftrigW{}                        ;
    float                       m_umax{}                          ;
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [   583] ;
    std::list<InDet::SiSpacePointForSeed*>  m_rfzv_Sorted[   300] ;
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed         ;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin       ;
    int m_fNmin{},m_fvNmin{}                                      ;
    int m_zMin{}                                                  ;

    std::multimap<float,InDet::SiSpacePointsProSeed*> m_seeds         ;
    std::multimap<float,InDet::SiSpacePointsProSeed*>::iterator m_seed;

    std::multimap<float,InDet::SiSpacePointsProSeed*> m_mapOneSeeds;
    int                                               m_nOneSeeds{};
    int                                               m_fillOneSeeds{};
    std::set<float>                                   m_l_vertex   ;
    std::vector<std::pair<float,InDet::SiSpacePointForSeed*>> m_CmSp;

    ///////////////////////////////////////////////////////////////////
    // Protected methods
    ///////////////////////////////////////////////////////////////////

    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_ATLxk() = delete;
    SiSpacePointsSeedMaker_ATLxk(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    SiSpacePointsSeedMaker_ATLxk &operator=(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    //@}

    MsgStream& dumpConditions(MsgStream& out) const;
    MsgStream& dumpEvent     (MsgStream& out) const;

    void buildFrameWork();
    void buildBeamFrameWork();

    SiSpacePointForSeed* newSpacePoint(const Trk::SpacePoint*const&);
    void newSeed(SiSpacePointForSeed*&, SiSpacePointForSeed*&, float);

    void newOneSeed(SiSpacePointForSeed*&, SiSpacePointForSeed*&,
     SiSpacePointForSeed*&, float, float);

    void newOneSeedWithCurvaturesComparison
    (SiSpacePointForSeed*&, SiSpacePointForSeed*&, float);

    void fillSeeds();
    void fillLists();
    void erase();
    void production2Sp();
    void production3Sp();
    void production3Sp
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int, int, int&);
    void production3SpTrigger
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int, int, int&);
 
    bool newVertices(const std::list<Trk::Vertex>&);
    void findNext();
    bool isZCompatible(float& Zv, float& R, float& T) const;
    void convertToBeamFrameWork(const Trk::SpacePoint*const& sp, float* r) const;
    bool isUsed(const Trk::SpacePoint* sp) const;
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_ATLxk&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_ATLxk&);

  ///////////////////////////////////////////////////////////////////
  // Inline methods
  ///////////////////////////////////////////////////////////////////

  inline const SiSpacePointsSeed* SiSpacePointsSeedMaker_ATLxk::next()
  {
    if (m_nspoint==3) {
      do {
        if (m_i_seed==m_i_seede) {
          findNext();
          if (m_i_seed==m_i_seede) return nullptr;
        }
        ++m_i_seed;
      } while (!(*m_seed++).second->set3(*m_seedOutput));
      return (m_seedOutput);
    } else {
      if (m_i_seed==m_i_seede) {
        findNext();
        if (m_i_seed==m_i_seede) return nullptr;
      } 
      (*m_i_seed++)->set2(*m_seedOutput);
      return (m_seedOutput);
    }
    return nullptr;
  }

  inline bool SiSpacePointsSeedMaker_ATLxk::isZCompatible  
  (float& Zv,float& R,float& T) const
  {
    if (Zv < m_zminU || Zv > m_zmaxU) return false;
    if (!m_isvertex) return true;

    std::set<float>::iterator v=m_l_vertex.begin(),ve=m_l_vertex.end();

    float dZmin = fabs((*v)-Zv);
    for (++v; v!=ve; ++v) {
      float dZ = fabs((*v)-Zv);
      if (dZ >= dZmin) break;
      dZmin=dZ;
    }
    return dZmin < (m_dzver+m_dzdrver*R)*sqrt(1.+T*T);
  }

  ///////////////////////////////////////////////////////////////////
  // New space point for seeds 
  ///////////////////////////////////////////////////////////////////

  inline SiSpacePointForSeed* SiSpacePointsSeedMaker_ATLxk::newSpacePoint
  (const Trk::SpacePoint*const& sp) 
  {
    SiSpacePointForSeed* sps;

    float r[3];
    convertToBeamFrameWork(sp,r);

    if (m_checketa) {

      float z = (fabs(r[2])+m_zmax);
      float x = r[0]*m_dzdrmin     ;
      float y = r[1]*m_dzdrmin     ;
      if ((z*z )<(x*x+y*y)) return nullptr;
    }

    if (m_i_spforseed!=m_l_spforseed.end()) {
      sps = (*m_i_spforseed++);
      sps->set(sp,r);
    } else {
      m_l_spforseed.push_back((sps=new SiSpacePointForSeed(sp,r)));
      m_i_spforseed = m_l_spforseed.end();
    }
      
    return sps;
  }

  ///////////////////////////////////////////////////////////////////
  // New 2 space points seeds 
  ///////////////////////////////////////////////////////////////////

  inline void SiSpacePointsSeedMaker_ATLxk::newSeed
  (SiSpacePointForSeed*& p1,SiSpacePointForSeed*& p2, float z) 
  {
    SiSpacePointForSeed* p3 = nullptr;

    if (m_i_seede!=m_l_seeds.end()) {
      SiSpacePointsProSeed* s = (*m_i_seede++);
      s->set(p1,p2,p3,z);
    } else {
      m_l_seeds.push_back(new SiSpacePointsProSeed(p1,p2,p3,z));
      m_i_seede = m_l_seeds.end();
    }
  }
  
} // end of name space

///////////////////////////////////////////////////////////////////
// Object-function for curvature seeds comparison
///////////////////////////////////////////////////////////////////

class comCurvature {
public:
  bool operator ()
  (const std::pair<float,InDet::SiSpacePointForSeed*>& i1, 
   const std::pair<float,InDet::SiSpacePointForSeed*>& i2)
  {
    return i1.first < i2.first;
  }
};

#endif // SiSpacePointsSeedMaker_ATLxk_H
