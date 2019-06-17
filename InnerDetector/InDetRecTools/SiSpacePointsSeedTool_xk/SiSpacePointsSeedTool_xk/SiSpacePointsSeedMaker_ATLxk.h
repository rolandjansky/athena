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
#include "SiSPSeededTrackFinderData/SiSpacePointForSeed.h"
#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkToolInterfaces/IPRD_AssociationTool.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include <iosfwd>
#include <list>
#include <vector>

class MsgStream;

namespace InDet {

  using EventData = SiSpacePointsSeedMakerEventData;

  class SiSpacePointsSeedMaker_ATLxk : 
    public extends<AthAlgTool, ISiSpacePointsSeedMaker>
  {
    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
      
  public:
      
    ///////////////////////////////////////////////////////////////////
    // Standard tool methods
    ///////////////////////////////////////////////////////////////////

    SiSpacePointsSeedMaker_ATLxk(const std::string&, const std::string&, const IInterface*);
    virtual ~SiSpacePointsSeedMaker_ATLxk() = default;
    virtual StatusCode initialize() override;
    virtual StatusCode finalize() override;

    ///////////////////////////////////////////////////////////////////
    // Methods to initialize tool for new event or region
    ///////////////////////////////////////////////////////////////////

    virtual void newEvent(EventData& data, int iteration) const override;
    virtual void newRegion(EventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT) const override;
    virtual void newRegion(EventData& data,
                           const std::vector<IdentifierHash>& vPixel, const std::vector<IdentifierHash>& vSCT,
                           const IRoiDescriptor& iRD) const override;
      
    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with two space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find2Sp(EventData& data, const std::list<Trk::Vertex>& lv) const override;

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with three space points with or without vertex constraint
    ///////////////////////////////////////////////////////////////////

    virtual void find3Sp(EventData& data, const std::list<Trk::Vertex>& lv) const override;
    virtual void find3Sp(EventData& data, const std::list<Trk::Vertex>& lv, const double* zVertex) const override;

    ///////////////////////////////////////////////////////////////////
    // Methods to initilize different strategies of seeds production
    // with variable number space points with or without vertex constraint
    // Variable means (2,3,4,....) any number space points
    ///////////////////////////////////////////////////////////////////
 
    virtual void findVSp(EventData& data, const std::list<Trk::Vertex>& lv) const override;
      
    ///////////////////////////////////////////////////////////////////
    // Iterator through seeds pseudo collection produced accordingly
    // methods find    
    ///////////////////////////////////////////////////////////////////
      
    virtual const SiSpacePointsSeed* next(EventData& data) const override;
      
    ///////////////////////////////////////////////////////////////////
    // Print internal tool parameters and status
    ///////////////////////////////////////////////////////////////////

    virtual MsgStream& dump(EventData& data, MsgStream& out) const override;

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
    PublicToolHandle<Trk::IPRD_AssociationTool> m_assoTool{this, "AssociationTool", "InDet::InDetPRD_AssociationToolGangedPixels"};

    ///////////////////////////////////////////////////////////////////
    // Space points containers
    ///////////////////////////////////////////////////////////////////
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsPixel{this, "SpacePointsPixelName", "PixelSpacePoints"};
    SG::ReadHandleKey<SpacePointContainer> m_spacepointsSCT{this, "SpacePointsSCTName", "SCT_SpacePoints"};
    SG::ReadHandleKey<SpacePointOverlapCollection> m_spacepointsOverlap{this, "SpacePointsOverlapName", "OverlapSpacePoints"};

    SG::ReadCondHandleKey<InDet::BeamSpotData> m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};

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

    // Properties, which can be updated in newEvent method. checketa is prepared in EventData.
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
    int m_outputlevel{0};
    bool m_initialized{false};

    // Updated only in buildFrameWork in initialize
    float m_dzdrmin0{0.};
    float m_dzdrmax0{0.};
    float m_ipt{0.};
    float m_ipt2{0.};
    float m_COF{0.};
    int m_r_size{0};
    int m_fNmax{0};
    int m_fvNmax{0};
    int m_rfz_b[SizeRFZ];
    int m_rfz_t[SizeRFZ];
    int m_rfz_ib[SizeRFZ][SizeI];
    int m_rfz_it[SizeRFZ][SizeI];
    int m_rfzv_n[SizeRFZV];
    int m_rfzv_i[SizeRFZV][SizeIV];
    float m_sF{0};
    float m_sFv{0};

    ///////////////////////////////////////////////////////////////////
    // Private methods
    ///////////////////////////////////////////////////////////////////

    /**    @name Disallow default instantiation, copy, assignment */
    //@{
    SiSpacePointsSeedMaker_ATLxk() = delete;
    SiSpacePointsSeedMaker_ATLxk(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    SiSpacePointsSeedMaker_ATLxk &operator=(const SiSpacePointsSeedMaker_ATLxk&) = delete;
    //@}

    MsgStream& dumpConditions(EventData& data, MsgStream& out) const;
    MsgStream& dumpEvent     (EventData& data, MsgStream& out) const;

    void buildFrameWork();
    void buildBeamFrameWork(EventData& data) const;

    SiSpacePointForSeed* newSpacePoint(EventData& data, const Trk::SpacePoint*const& sp) const;
    void newSeed(EventData& data, SiSpacePointForSeed*& p1, SiSpacePointForSeed*& p2, float z) const;

    void newOneSeed(EventData& data,
                    SiSpacePointForSeed*& p1, SiSpacePointForSeed*& p2,
                    SiSpacePointForSeed*& p3, float z, float q) const;

    void newOneSeedWithCurvaturesComparison
    (EventData& data, SiSpacePointForSeed*& SPb, SiSpacePointForSeed*& SP0, float Zob) const;

    void fillSeeds(EventData& data) const;
    void fillLists(EventData& data) const;
    void erase(EventData& data) const;
    void production2Sp(EventData& data) const;
    void production3Sp(EventData& data) const;
    void production3Sp
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rb,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rt,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
     int NB, int NT, int& nseed) const;
    void production3SpTrigger
    (EventData& data,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rb,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rbe,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rt,
     std::list<InDet::SiSpacePointForSeed*>::iterator* rte,
     int NB, int NT, int& nseed) const;
 
    bool newVertices(EventData& data, const std::list<Trk::Vertex>& lV) const;
    void findNext(EventData& data) const;
    bool isZCompatible(EventData& data, float& Zv, float& R, float& T) const;
    void convertToBeamFrameWork(EventData& data, const Trk::SpacePoint*const& sp, float* r) const;
    bool isUsed(const Trk::SpacePoint* sp) const;

    void initializeEventData(EventData& data) const;
  };
  
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
