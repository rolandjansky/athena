/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MSVERTEXTRACKLETTOOL_H
#define MSVERTEXTRACKLETTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MSVertexToolInterfaces/IMSVertexTrackletTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include "Identifier/Identifier.h"
#include "MSVertexUtils/TrackletSegment.h"
#include "MSVertexUtils/Tracklet.h"
#include <utility>
#include <vector>
#include "xAODTracking/TrackParticleContainer.h"
#include "MuonPrepRawData/MdtPrepDataContainer.h"



namespace Muon {

  class MSVertexTrackletTool : virtual public IMSVertexTrackletTool, public AthAlgTool
  {
	
  public :
    /** default constructor */
    MSVertexTrackletTool (const std::string& type, const std::string& name, const IInterface* parent);
    /** destructor */
    virtual ~MSVertexTrackletTool();

    static const InterfaceID& interfaceID();

    virtual StatusCode initialize(void) override;
    virtual StatusCode finalize(void) override;

  private:
    //tool handles & private data members

    const MdtIdHelper* m_mdtIdHelper;

    float m_SeedResidual;
    float m_minSegFinderChi2;
    float m_BarrelDeltaAlphaCut;
    float m_maxDeltabCut;
    float m_EndcapDeltaAlphaCut;
    //float m_DeltabCut; //this variable is not used right now
    //float m_TrackPhiAngle; //this variable is not used right now

    bool m_tightTrackletRequirement;



  public:
    StatusCode findTracklets(std::vector<Tracklet>& traklets, const EventContext &ctx) const override;
    
  private:
    //private functions
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
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtTESKey;//"MDT_DriftCircles"
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_TPContainer;
  };
  
  
}
#endif 
