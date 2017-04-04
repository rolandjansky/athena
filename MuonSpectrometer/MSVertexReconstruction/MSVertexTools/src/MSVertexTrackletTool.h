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

    virtual StatusCode initialize(void);  
    virtual StatusCode finalize(void);  

  private:
    //tool handles & private data members

    const MdtIdHelper* m_mdtIdHelper;
    static const MdtIdHelper* s_mdtCompareIdHelper;

    float m_SeedResidual;
    float m_minSegFinderChi2;
    float m_BarrelDeltaAlphaCut;
    float m_maxDeltabCut;
    float m_EndcapDeltaAlphaCut;
    float m_DeltaAlphaCut;
    //float m_DeltabCut; //this variable is not used right now
    //float m_TrackPhiAngle; //this variable is not used right now

    bool m_tightTrackletRequirement;

    float m_PI;
    float m_BIL;
    float m_BML;
    float m_BMS;
    float m_BOL;


  public:
    StatusCode findTracklets(std::vector<Tracklet>& traklets);
    
  private:
    //private functions
    int SortMDThits(std::vector<std::vector<Muon::MdtPrepData*> >& SortedMdt);
    bool SortMDT(Identifier& i1, Identifier& i2);    
    std::vector<TrackletSegment> TrackletSegmentFitter(std::vector<Muon::MdtPrepData*>& mdts);
    std::vector<TrackletSegment> TrackletSegmentFitterCore(std::vector<Muon::MdtPrepData*>& mdts,std::vector<std::pair<float,float> >& SeedParams);
    std::vector<std::pair<float,float> > SegSeeds(std::vector<Muon::MdtPrepData*>& mdts);
    float SeedResiduals(std::vector<Muon::MdtPrepData*>& mdts, float slope, float inter);
    std::vector<TrackletSegment> CleanSegments(std::vector<TrackletSegment>& segs);
    bool DeltabCalc(TrackletSegment& ML1seg, TrackletSegment& ML2seg);
    float TrackMomentum(int chamber,float deltaAlpha);
    float TrackMomentumError(TrackletSegment& ml1, TrackletSegment& ml2);
    float TrackMomentumError(TrackletSegment& ml1);
    std::vector<Tracklet> ResolveAmbiguousTracklets(std::vector<Tracklet>& tracks);
    void convertToTrackParticles(std::vector<Tracklet>& tracklets, SG::WriteHandle<xAOD::TrackParticleContainer> &container);
    float sq(float x) { return (x)*(x); }
    static bool mdtComp(const Muon::MdtPrepData* mprd1, const Muon::MdtPrepData* mprd2);
    void addMDTHits( std::vector<Muon::MdtPrepData*>& hits, std::vector<std::vector<Muon::MdtPrepData*> >& SortedMdt ) const;
    SG::ReadHandleKey<Muon::MdtPrepDataContainer> m_mdtTESKey;//"MDT_DriftCircles"
    SG::WriteHandleKey<xAOD::TrackParticleContainer> m_TPContainer;
  };
  
  
}
#endif 
