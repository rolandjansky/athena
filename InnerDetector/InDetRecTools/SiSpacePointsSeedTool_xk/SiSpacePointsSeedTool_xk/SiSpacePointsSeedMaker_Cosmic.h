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
#include <mutex>
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

    ///////////////////////////////////////////////////////////////////
    // Space points container
    ///////////////////////////////////////////////////////////////////
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    // Properties, which will not be changed after construction
    BooleanProperty m_pixel{this, "usePixel", true};
    BooleanProperty m_sct{this, "useSCT", true};
    BooleanProperty m_useOverlap{this, "useOverlapSpCollection", false};
    BooleanProperty m_useassoTool{this, "UseAssociationTool", false};
    IntegerProperty m_maxsize{this, "maxSize", 100};
    IntegerProperty m_maxsizeSP{this, "maxSizeSP", 1500};
    FloatProperty m_drmin{this, "mindRadius", 5.};
    FloatProperty m_drmax{this, "maxdRadius", 1000.};
    FloatProperty m_r_rmax{this, "radMax", 600.};
    FloatProperty m_r_rstep{this, "radStep", 5.};

    // Properties, which can be updated in initialize
    FloatProperty m_ptmin{this, "pTmin", 500.};
    FloatProperty m_rapcut{this, "RapidityCut", 2.3};

    // Properties, which are not used in this implementation of SiSpacePointsSeedMaker_Cosmic class
    BooleanProperty m_checketa{this, "checkEta", false};
    FloatProperty m_zmin{this, "minZ", -10000.};
    FloatProperty m_zmax{this, "maxZ", +10000.};
    FloatProperty m_diver{this, "maxdImpact", 10.};
    FloatProperty m_diverpps{this, "maxdImpactPPS", 0.};
    FloatProperty m_diversss{this, "maxdImpactSSS", 0.};
    FloatProperty m_etamin{this, "etaMin", 0.};
    FloatProperty m_etamax{this, "etaMax", 0.};

    // Updated in only initialize
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

    mutable std::mutex m_mutex;
    mutable std::vector<EventContext::ContextEvt_t> m_cache ATLAS_THREAD_SAFE; // Guarded by m_mutex
    struct EventData { // To hold event dependent data
      bool endlist{true};
      int state{0};
      int nprint{0};
      int nspoint{2};
      int mode{0};
      int nlist{0};
      int nseeds{0};
      int ns{0};
      int nsaz{0};
      int nr{0};
      int nrf{0};
      int nrfz{0};
      int fNmin{0};
      int zMin{0};
      int rf_index[SizeRF];
      int rf_map[SizeRF];
      int rfz_index[SizeRFZ];
      int rfz_map[SizeRFZ];
      std::vector<std::list<InDet::SiSpacePointForSeed*>> r_Sorted;
      std::vector<int> r_index;
      std::vector<int> r_map;
      std::vector<InDet::SiSpacePointsSeed> seeds;
      std::multimap<float,InDet::SiSpacePointsSeed*> l_seeds;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seed;
      std::multimap<float,InDet::SiSpacePointsSeed*>::iterator i_seede;
      std::list<InDet::SiSpacePointForSeed*> rf_Sorted[SizeRF];
      std::list<InDet::SiSpacePointForSeed*> rfz_Sorted[SizeRFZ];
      std::list<InDet::SiSpacePointForSeed> l_spforseed;
      std::list<InDet::SiSpacePointForSeed>::iterator i_spforseed;
      std::list<InDet::SiSpacePointForSeed*>::iterator rMin;
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
    };
    mutable std::vector<EventData> m_eventData ATLAS_THREAD_SAFE; // Guarded by m_mutex

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////
    /**    @name Disallow default instantiation, copy, assignment */
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
    void production3Sp(EventData& data) const;
    void production3Sp
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float,float) const;

    void production3SpWithoutField(EventData& data) const;
    void production3SpWithoutField
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     std::list<InDet::SiSpacePointForSeed*>::iterator*,
     int,int,float) const;

    bool isUsed(const Trk::SpacePoint*) const;

    EventData& getEventData() const;
  };

  MsgStream&    operator << (MsgStream&   ,const SiSpacePointsSeedMaker_Cosmic&);
  std::ostream& operator << (std::ostream&,const SiSpacePointsSeedMaker_Cosmic&);

} // end of name space

#endif // SiSpacePointsSeedMaker_Cosmic_H
