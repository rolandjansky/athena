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
    (const std::string&, const std::string&, const IInterface*);
    virtual ~SiSpacePointsSeedMaker_Cosmic() = default;
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
    enum Size {SizeRF=53,
               SizeZ=11,
               SizeRFZ=SizeRF*SizeZ,
               SizeI=12};

    ///////////////////////////////////////////////////////////////////
    // Private data and methods
    ///////////////////////////////////////////////////////////////////
      
    ServiceHandle<MagField::IMagFieldSvc>  m_fieldServiceHandle{this, "MagFieldSvc", "AtlasFieldSvc"};
    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};

    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    bool m_endlist{true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", false};
    BooleanProperty m_checketa{this, "checkEta", false};
    int                         m_outputlevel {};
    int                         m_nprint {};
    bool m_state{0};
    int m_nspoint{2};
    int m_mode{0};
    int m_nlist{0};
    IntegerProperty m_maxsize{this, "maxSize", 100};
    int                         m_nseeds {};
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 1000.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.3};
    float                       m_dzdrmin {};
    float                       m_dzdrmax {};
    FloatProperty m_zmin{this, "minZ", -10000.};
    FloatProperty m_zmax{this, "maxZ", +10000.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 5.};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    float                       m_dazmax  {};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 0.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 0.};
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_etamax{this, "etaMax", 0.};
    int m_r_size {};
    int m_rf_size {};
    int m_rfz_size {};
    std::vector<std::list<InDet::SiSpacePointForSeed*>> m_r_Sorted;
    std::list<InDet::SiSpacePointForSeed*>  m_rf_Sorted[SizeRF];
    std::list<InDet::SiSpacePointForSeed*>  m_rfz_Sorted[SizeRFZ];
    std::list<InDet::SiSpacePointForSeed> m_l_spforseed;
    std::list<InDet::SiSpacePointForSeed>::iterator m_i_spforseed;
    std::list<InDet::SiSpacePointForSeed*>::iterator m_rMin;

    int m_ns{} ,m_nsaz {};
    int m_fNmax  {};
    int m_fNmin{};
    int m_zMin {};
    int  m_nr{};
    std::vector<int> m_r_index;
    std::vector<int> m_r_map;
    int  m_nrf{}     , m_rf_index[SizeRF], m_rf_map[SizeRF];
    int  m_nrfz{}    , m_rfz_index[SizeRFZ], m_rfz_map[SizeRFZ];
    int m_rfz_b[SizeRFZ],m_rfz_t[SizeRFZ],m_rfz_ib[SizeRFZ][SizeI],m_rfz_it[SizeRFZ][SizeI];
    float m_sF {};

    ///////////////////////////////////////////////////////////////////
    // Tables for 3 space points seeds search
    ///////////////////////////////////////////////////////////////////
     
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    std::vector<InDet::SiSpacePointForSeed*> m_SP;
    std::vector<float> m_R;
    std::vector<float> m_Tz;
    std::vector<float> m_Er;
    std::vector<float> m_U;
    std::vector<float> m_V;
    std::vector<float> m_Zo;

    std::multimap<float,InDet::SiSpacePointsSeed*>           m_l_seeds;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_i_seed;
    std::multimap<float,InDet::SiSpacePointsSeed*>::iterator m_i_seede;
    std::vector<InDet::SiSpacePointsSeed> m_seeds;

    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////
      
    SG::ReadHandleKey<SpacePointContainer>         m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer>         m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_Cosmic() = delete;
    SiSpacePointsSeedMaker_Cosmic(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    SiSpacePointsSeedMaker_Cosmic &operator=(const SiSpacePointsSeedMaker_Cosmic&) = delete;
    //@}

    MsgStream&    dumpConditions(MsgStream   & out) const;
    MsgStream&    dumpEvent     (MsgStream   & out) const;

    void buildFrameWork();

    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&);
    SiSpacePointForSeed* newSpacePoint
    (const Trk::SpacePoint*const&,const float*);
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const float&);
    void newSeed
    (const Trk::SpacePoint*&,const Trk::SpacePoint*&,
     const Trk::SpacePoint*&,const float&);
    void fillLists     ();
    void erase         ();
    void production2Sp ();
    void production3Sp ();
    void production3Sp
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float,float);

    void production3SpWithoutField();
    void production3SpWithoutField
    (std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float);

    bool isUsed(const Trk::SpacePoint*);
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_Cosmic&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_Cosmic&);

} // end of name space

#endif // SiSpacePointsSeedMaker_Cosmic_H
