/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXTRACKLETTOOL_H
#define MSVERTEXTRACKLETTOOL_H

#include "MSVertexToolInterfaces/IMSVertexTrackletTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "MSVertexUtils/TrackletSegment.h"
#include "MSVertexUtils/Tracklet.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"
#include "StoreGate/WriteHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

#include <utility>
#include <vector>


namespace Muon {

  class MSVertexTrackletTool : virtual public IMSVertexTrackletTool, public AthAlgTool {
  public :
    MSVertexTrackletTool (const std::string& type, const std::string& name, const IInterface* parent);
    virtual ~MSVertexTrackletTool()=default;

    virtual StatusCode initialize() override;

    StatusCode findTracklets(std::vector<Tracklet>& traklets, const EventContext &ctx) const override;

  private:
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    float m_SeedResidual;
    float m_minSegFinderChi2;
    float m_BarrelDeltaAlphaCut;
    float m_maxDeltabCut;
    float m_EndcapDeltaAlphaCut;

    bool m_tightTrackletRequirement;

    int SortMDThits(std::vector<std::vector<const Muon::MdtPrepData*> >& SortedMdt, const EventContext &ctx) const;
    bool SortMDT(Identifier& i1, Identifier& i2) const;    
    std::vector<TrackletSegment> TrackletSegmentFitter(std::vector<const Muon::MdtPrepData*>& mdts) const ;
    std::vector<TrackletSegment> TrackletSegmentFitterCore(std::vector<const Muon::MdtPrepData*>& mdts,std::vector<std::pair<float,float> >& SeedParams) const;
    std::vector<std::pair<float,float> > SegSeeds(std::vector<const Muon::MdtPrepData*>& mdts) const;
    float SeedResiduals(std::vector<const Muon::MdtPrepData*>& mdts, float slope, float inter) const;
    std::vector<TrackletSegment> CleanSegments(std::vector<TrackletSegment>& segs) const;
    bool DeltabCalc(TrackletSegment& ML1seg, TrackletSegment& ML2seg) const;
    float TrackMomentum(int chamber,float deltaAlpha) const;
    float TrackMomentumError(TrackletSegment& ml1, TrackletSegment& ml2) const;
    float TrackMomentumError(TrackletSegment& ml1) const;
    std::vector<Tracklet> ResolveAmbiguousTracklets(std::vector<Tracklet>& tracks) const;
    void convertToTrackParticles(std::vector<Tracklet>& tracklets, SG::WriteHandle<xAOD::TrackParticleContainer> &container) const;

    void addMDTHits( std::vector<const Muon::MdtPrepData*>& hits, std::vector<std::vector<const Muon::MdtPrepData*> >& SortedMdt ) const;
    
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtTESKey { this, "mdtTES", "MDT_DriftCircles"};
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_TPContainer { this, "xAODTrackParticleContainer", "MSonlyTracklets"};
  };
  
  
}
#endif 
