/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ActsSiSpacePointsSeedMaker_H
#define ActsSiSpacePointsSeedMaker_H

#include "InDetRecToolInterfaces/ISiSpacePointsSeedMaker.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"
#include "ActsTrkFinding/IActsSeedingTool.h"

#include "SiSPSeededTrackFinderData/SiSpacePointsSeedMakerEventData.h"
#include "SiSPSeededTrackFinderData/ITkSiSpacePointForSeed.h"

#include "BeamSpotConditionsData/BeamSpotData.h"
#include "MagFieldConditions/AtlasFieldCacheCondObj.h"
#include "MagFieldElements/AtlasFieldCache.h"

#include "TrkSpacePoint/SpacePointContainer.h" 
#include "TrkSpacePoint/SpacePointOverlapCollection.h"
#include "TrkEventUtils/PRDtoTrackMap.h"

//for validation
#include "GaudiKernel/ITHistSvc.h"
#include "TFile.h"
#include "TTree.h"


namespace ActsTrk {

  class ActsSiSpacePointsSeedMaker : 
    public extends<AthAlgTool, InDet::ISiSpacePointsSeedMaker> {
  private:
    enum SeedStrategy : short {PPP, SSS};

  public:
    ActsSiSpacePointsSeedMaker(const std::string &t, const std::string &n, const IInterface *p);
    virtual ~ActsSiSpacePointsSeedMaker() override = default;

    virtual StatusCode initialize() override;

    // Interface
    virtual void 
      newEvent(const EventContext& ctx, 
	       InDet::SiSpacePointsSeedMakerEventData& data, 
	       int iteration=-1) const override;

    virtual void 
      find3Sp(const EventContext& ctx, 
	      InDet::SiSpacePointsSeedMakerEventData& data,
	      const std::list<Trk::Vertex>& lv) const override;

    virtual const InDet::SiSpacePointsSeed* 
      next(const EventContext& ctx, 
	   InDet::SiSpacePointsSeedMakerEventData& data) const override;

    virtual void 
      writeNtuple(const InDet::SiSpacePointsSeed* seed, 
		  const Trk::Track* track, 
		  int seedType, 
		  long eventNumber) const override;
    
    virtual bool 
      getWriteNtupleBoolProperty() const override;

    virtual MsgStream& 
      dump(InDet::SiSpacePointsSeedMakerEventData& data, 
	   MsgStream& out) const override;

    // ===================================================================== //
    // WE DO NOT NEED THESE FOR NOW
    // ===================================================================== //

    virtual void 
      newRegion(const EventContext&,
		InDet::SiSpacePointsSeedMakerEventData&,
		const std::vector<IdentifierHash>&,
		const std::vector<IdentifierHash>&) const override 
    {
      ATH_MSG_ERROR("Method `newRegion` is not implemented.");
    }
    
    virtual void 
      newRegion(const EventContext&, 
		InDet::SiSpacePointsSeedMakerEventData&,
		const std::vector<IdentifierHash>&,
		const std::vector<IdentifierHash>&,
		const IRoiDescriptor&) const override 
    {
      ATH_MSG_ERROR("Method `newRegion` is not implemented.");
    }

    virtual void 
      find2Sp(InDet::SiSpacePointsSeedMakerEventData&,
	      const std::list<Trk::Vertex>&) const override
    {
      ATH_MSG_ERROR("Method `findfind2Sp` is not implemented.");
    }

    /// with three space points with or without vertex constraint
    /// with information about min and max Z of the  vertex
    virtual void 
      find3Sp(const EventContext&, 
	      InDet::SiSpacePointsSeedMakerEventData&,
	      const std::list<Trk::Vertex>&, 
	      const double*) const override 
    {
      ATH_MSG_ERROR("Method `find3Sp` is not implemented.");
    }

    /// with variable number space points with or without vertex constraint
    /// Variable means (2,3,4,....) any number space points
    virtual void 
      findVSp(const EventContext&, 
	      InDet::SiSpacePointsSeedMakerEventData&,
	      const std::list<Trk::Vertex>&) const override 
    { 
      ATH_MSG_ERROR("Method `findVSp` is not implemented.");
    }

    // ===================================================================== //
    // ===================================================================== //

  private:
    MsgStream& 
      dumpConditions(InDet::SiSpacePointsSeedMakerEventData& data, 
		     MsgStream& out) const;

    static MsgStream& 
      dumpEvent(InDet::SiSpacePointsSeedMakerEventData& data, 
		MsgStream& out);
    

  private:
    void buildBeamFrameWork(const EventContext& ctx,
			    InDet::SiSpacePointsSeedMakerEventData& data) const;
    void
      newSpacePoint(InDet::SiSpacePointsSeedMakerEventData& data,
		    const Trk::SpacePoint* const& sp) const;
    static void pixInform(const Trk::SpacePoint* const& sp,
			  float* r) ;
    static void stripInform(InDet::SiSpacePointsSeedMakerEventData& data,
			    const Trk::SpacePoint* const& sp,
			    float* r) ;
    
    // Retrieve
    StatusCode retrievePixel(const EventContext& ctx,
			     InDet::SiSpacePointsSeedMakerEventData& data,
			     const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const;
    StatusCode retrieveStrip(const EventContext& ctx,
                             InDet::SiSpacePointsSeedMakerEventData& data,
                             const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const;
    StatusCode retrieveOverlap(const EventContext& ctx,
			       InDet::SiSpacePointsSeedMakerEventData& data,
			       const Trk::PRDtoTrackMap* prd_to_track_map_cptr) const;

    // Validation
    StatusCode InitTree();

    void estimateParameters
      (const SeedStrategy seed_strategy,
       InDet::SiSpacePointsSeedMakerEventData& data,
       ITk::SiSpacePointForSeed*& bottom,
       ITk::SiSpacePointForSeed*& medium,
       ITk::SiSpacePointForSeed*& top,
       float) const;
    

  private:
    bool isUsed(const Trk::SpacePoint*, 
		const Trk::PRDtoTrackMap &prd_to_track_map) const;


  private:
    ToolHandle< ActsTrk::IActsSeedingTool > m_seedsToolPixel {this, "SeedToolPixel", "","Seed Tool for Pixel detector"};
    ToolHandle< ActsTrk::IActsSeedingTool > m_seedsToolStrip {this, "SeedToolStrip", "","Seed Tool for Strip detector"};

    SG::ReadHandleKey< ::SpacePointContainer > m_spacepointsPixel {this, "SpacePointsPixelName", "ITkPixelSpacePoints", "Pixel space points container"};
    SG::ReadHandleKey< ::SpacePointContainer > m_spacepointsStrip {this, "SpacePointsStripName", "ITkStripSpacePoints", "Strip space points container"};
    SG::ReadHandleKey< ::SpacePointOverlapCollection > m_spacepointsOverlap {this, "SpacePointsOverlapName", "OverlapSpacePoints"};
    SG::ReadHandleKey< Trk::PRDtoTrackMap > m_prdToTrackMap {this, "PRDtoTrackMap", "", "option PRD-to-track association"};

    SG::ReadCondHandleKey< InDet::BeamSpotData > m_beamSpotKey{this, "BeamSpotKey", "BeamSpotData", "SG key for beam spot"};
    SG::ReadCondHandleKey< AtlasFieldCacheCondObj > m_fieldCondObjInputKey {this, "AtlasFieldCacheCondObj", "fieldCondObj",
        "Name of the Magnetic Field conditions object key"};

    Gaudi::Property< bool > m_pixel {this, "usePixel", true};
    Gaudi::Property< bool > m_strip {this, "useStrip", true };
    Gaudi::Property< bool > m_useOverlap {this, "useOverlapSpCollection", true};
    Gaudi::Property< bool > m_fastTracking {this, "useFastTracking", false};

  private:
    // Validation
    Gaudi::Property< bool > m_writeNtuple {this, "WriteNtuple", false};

    mutable std::mutex m_mutex;

    ITHistSvc* m_thistSvc = nullptr;
    TTree* m_outputTree = nullptr;

    mutable std::string m_treeName ATLAS_THREAD_SAFE = "";
    mutable std::string m_treeFolder ATLAS_THREAD_SAFE = "/valNtuples/";

    mutable float m_d0 ATLAS_THREAD_SAFE = 0;
    mutable float m_z0 ATLAS_THREAD_SAFE = 0;
    mutable float m_pt ATLAS_THREAD_SAFE = 0;
    mutable float m_eta ATLAS_THREAD_SAFE = 0;
    mutable double m_x1 ATLAS_THREAD_SAFE = 0;
    mutable double m_x2 ATLAS_THREAD_SAFE = 0;
    mutable double m_x3 ATLAS_THREAD_SAFE = 0;
    mutable double m_y1 ATLAS_THREAD_SAFE = 0;
    mutable double m_y2 ATLAS_THREAD_SAFE = 0;
    mutable double m_y3 ATLAS_THREAD_SAFE = 0;
    mutable double m_z1 ATLAS_THREAD_SAFE = 0;
    mutable double m_z2 ATLAS_THREAD_SAFE = 0;
    mutable double m_z3 ATLAS_THREAD_SAFE = 0;
    mutable double m_r1 ATLAS_THREAD_SAFE = 0;
    mutable double m_r2 ATLAS_THREAD_SAFE = 0;
    mutable double m_r3 ATLAS_THREAD_SAFE = 0;
    mutable float m_quality ATLAS_THREAD_SAFE = 0;
    mutable int m_type ATLAS_THREAD_SAFE = 0;
    mutable double m_dzdr_t ATLAS_THREAD_SAFE = 0;
    mutable double m_dzdr_b ATLAS_THREAD_SAFE = 0;
    mutable bool m_givesTrack ATLAS_THREAD_SAFE = false;
    mutable float m_trackPt ATLAS_THREAD_SAFE = 0;
    mutable float m_trackEta ATLAS_THREAD_SAFE = 0;
    mutable long m_eventNumber ATLAS_THREAD_SAFE = 0;
  };

  ///////////////////////////////////////////////////////////////////
  // Inline Method
  //////////////////////////////////////////////////////////////////

  inline bool
    ActsSiSpacePointsSeedMaker::isUsed(const Trk::SpacePoint* sp, 
				       const Trk::PRDtoTrackMap &prd_to_track_map) const
  {
    const Trk::PrepRawData* d = sp->clusterList().first;
    if (!d || !prd_to_track_map.isUsed(*d)) return false;
    
    d = sp->clusterList().second;
    if (!d || prd_to_track_map.isUsed(*d)) return true;

    return false;
  }
  
}

#endif
