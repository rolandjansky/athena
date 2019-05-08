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
    virtual StatusCode initialize();
    virtual StatusCode finalize();

    ///////////////////////////////////////////////////////////////////
    // Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////

    virtual void newEvent(int iteration);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT);
    virtual void newRegion(const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT, const IRoiDescriptor& IRD);
       
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
    bool m_endlist{true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", true};
    bool                        m_trigger  {};
    int                         m_outputlevel  {};
    int                         m_nprint {};
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
    float                       m_dzdrmin  {};
    float                       m_dzdrmax  {};
    FloatProperty m_zmin{this, "minZ", -250.};
    FloatProperty m_zmax{this, "maxZ", +250.};
    float                       m_zminU   {};
    float                       m_zmaxU   {};
    FloatProperty m_r_rmax{this, "radMax", 200.};
    FloatProperty m_r_rstep{this, "radStep", 2.};
    FloatProperty m_dzver{this, "maxdZver", 5.};
    FloatProperty m_dzdrver{this, "maxdZdRver", 0.02};
    FloatProperty m_diver{this, "maxdImpact", 7.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 1.2};
    float                       m_dazmax{0.02};
    FloatProperty m_ptmin{this, "pTmin", 100.};
    FloatProperty m_ptmax{this, "pTmax", 500.};
    float                       m_iptmin   {};
    float                       m_iptmax{1./400.};

    int m_r_size   {};
    int m_rf_size   {};
    int m_rfz_size   {};
    std::list<InDet::SiSpacePointForSeed*>* m_r_Sorted{nullptr};
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted [  220];
    std::list<InDet::SiSpacePointForSeed*>  m_l_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin;
  
    int m_ns{},m_nsaz  {};
    int m_fNmax  {};
    int m_fNmin  {};
    int m_zMin  {};
    int  m_nr  {}; int* m_r_index{nullptr}; int* m_r_map{nullptr};
    int  m_nrfz {}  , m_rfz_index  [220], m_rfz_map  [220];
    int m_rfz_b[220],m_rfz_t[220],m_rfz_ib[220][9],m_rfz_it[220][9];
    float m_sF {};

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

    std::list<InDet::SiSpacePointsSeed*>           m_l_seeds;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seed;
    std::list<InDet::SiSpacePointsSeed*>::iterator m_i_seede;

    std::multimap<float,InDet::SiSpacePointsSeed*> m_mapOneSeeds;
    InDet::SiSpacePointsSeed*                       m_OneSeeds{nullptr};
    IntegerProperty m_maxOneSize{this, "maxSeedsForSpacePoint", 5};
    int                                             m_nOneSeeds {};
    std::list<float>                                m_l_vertex;
 
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
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_LowMomentum() = delete;
    SiSpacePointsSeedMaker_LowMomentum(const SiSpacePointsSeedMaker_LowMomentum&) = delete;
    SiSpacePointsSeedMaker_LowMomentum &operator=(const SiSpacePointsSeedMaker_LowMomentum&) = delete;
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
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&);

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
     int,int,int&,float);
     
    bool newVertices(const std::list<Trk::Vertex>&);
    void findNext();
    bool isZCompatible     (float&,float&,float&);
    void convertToBeamFrameWork(const Trk::SpacePoint*const&,float*);
    bool isUsed(const Trk::SpacePoint*);
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_LowMomentum&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_LowMomentum&);

} // end of name space

#endif // SiSpacePointsSeedMaker_LowMomentum_H
