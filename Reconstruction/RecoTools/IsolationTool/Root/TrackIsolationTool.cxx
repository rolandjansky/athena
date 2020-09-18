/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TrackIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "AsgDataHandles/ReadHandle.h"
#include "IsolationTool/TrackIsolationTool.h"
// #include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationCorrectionHelper.h"
#include "xAODMuon/Muon.h"

#include <iomanip>

namespace xAOD {

  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  TrackIsolationTool::TrackIsolationTool (const std::string& name):
    asg::AsgTool(name)
  {
#ifndef XAOD_ANALYSIS
    declareInterface<ITrackIsolationTool>(this);
#endif // XAOD_ANALYSIS
  }

  TrackIsolationTool::~TrackIsolationTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode TrackIsolationTool::initialize()
  {
#ifndef XAOD_ANALYSIS
    ATH_CHECK(m_tracksInConeTool.retrieve( DisableTool{m_simpleIsolation} ));
#endif // XAOD_ANALYSIS

      if(m_trkselTool.retrieve().isFailure()){
	ATH_MSG_FATAL("Could not retrieve InDetTrackSelectionTool");
	return StatusCode::FAILURE;
      }

    /** square cone */
    m_overlapCone2 = m_overlapCone*m_overlapCone;

    // the read handles
    ATH_CHECK(m_indetTrackParticleLocation.initialize());
    if (!m_vertexLocation.key().empty())
      ATH_CHECK(m_vertexLocation.initialize());

    return StatusCode::SUCCESS;
  }

  const Vertex* TrackIsolationTool::retrieveIDBestPrimaryVertex(const VertexContainer* vtxC) const
  {
    if (vtxC) {
      if (vtxC->empty()) {
	ATH_MSG_INFO("No vertex in container.");
	return nullptr;
      }
      // In fact, should rather do like that, in case front is not PriVtx
      for (const auto* const vtx : *vtxC ) {
	if (vtx->vertexType() == xAOD::VxType::PriVtx)
	  return vtx;
      }
    }
    return nullptr;
  }

  const IParticle* TrackIsolationTool::getReferenceParticle(const IParticle& particle) const {
    const TrackParticle* tp = dynamic_cast<const TrackParticle*>(&particle);
    if( tp ) return tp;
    const Muon* muon = dynamic_cast<const Muon*>(&particle);
    if( muon ) {
      const xAOD::TrackParticle* tp = nullptr;
      if(muon->inDetTrackParticleLink().isValid()) tp = *muon->inDetTrackParticleLink();
      if( !tp ) tp = muon->primaryTrackParticle();
      if( !tp ) {
        ATH_MSG_WARNING(" No TrackParticle found for muon " );
        return nullptr;
      }
      return tp;
    }
    return &particle;
  }


  bool TrackIsolationTool::trackIsolation( TrackIsolation& result, const IParticle& particle,
					   const std::vector<Iso::IsolationType>& isoTypes,
                                           TrackCorrection corrbitset,
					   const Vertex* vertex,
                                           const std::set<const TrackParticle*>* exclusionSet,
                                           const TrackParticleContainer* indetTrackParticles )  const
  {
    /// prepare input
    // If not vertex is given, use the ID best one. If one does not want to cut on z0sinT, use the TrackSelectionTool config
    SG::ReadHandle<VertexContainer> vtxH;
    if (vertex == nullptr && !m_vertexLocation.key().empty()) {
      vtxH = SG::makeHandle(m_vertexLocation);
      if (!vtxH.isValid()) {
	ATH_MSG_ERROR("Did not find a vertex container with key " << m_vertexLocation.key());
	return false;
      }
      vertex = retrieveIDBestPrimaryVertex(vtxH.ptr());
      if (vertex)
	ATH_MSG_DEBUG("No vertex provided, is required. Use the ID-chosen pvx, z = " << vertex->z());
    }

    /// get track particle
    const IParticle* tp = getReferenceParticle(particle);
    if( !tp ){
      ATH_MSG_WARNING("Failed to obtain reference particle");
      return false;
    }
    if (tp->pt() <= 0.)
      ATH_MSG_WARNING("A particle of type " << particle.type() << " with strange pT : " << tp->pt()*1e-3 << " GeV");
    TrackIsolationInput input( tp, corrbitset, vertex, exclusionSet );

    for( auto isoType : isoTypes ){
      Iso::IsolationFlavour flavour = Iso::isolationFlavour(isoType);
      if( flavour != Iso::ptcone ) {
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << Iso::toCString(isoType));
        return false;
      }
      float conesize = Iso::coneSize(isoType);
      input.coneSizesSquared.push_back(conesize*conesize);
    }

    for(size_t i=1; i<input.coneSizesSquared.size(); i++){
      if(input.coneSizesSquared[i]>input.coneSizesSquared[i-1]) ATH_MSG_ERROR("Isolation Cone should be in decreasing order. "<< i << "th variable is " << Iso::toCString(isoTypes[i]) << " is larger than the one before it: " << Iso::toCString(isoTypes[i-1]));
    }

    std::sort(input.coneSizesSquared.begin(),input.coneSizesSquared.end(),[](float i, float j) { return i>j; });

    initresult(result, corrbitset, input.coneSizesSquared.size());

    input.maxRadius = sqrt(input.coneSizesSquared[0]);

    bool success = false;
    // run isolation code
    if( indetTrackParticles )    success = simpleIsolation(input,result,indetTrackParticles);
    else if( m_simpleIsolation ) success = simpleIsolation(input,result);
    else                         success = binnedIsolation(input,result);

    if( msgLvl(MSG::DEBUG) ){
      if( !success ){
        ATH_MSG_DEBUG(" Track isolation calculation failed");
      }else{
        msg(MSG::DEBUG) << "Calculated track isolation: ";
        for( unsigned int i = 0; i< result.ptcones.size();++i ){
          msg(MSG::DEBUG) << " coneSizeSquared " << std::setw(3) << input.coneSizesSquared[i] << " value " << result.ptcones[i];
        }
        msg(MSG::DEBUG) << endmsg;
      }
    }
    return success;
  }


  bool TrackIsolationTool::binnedIsolation( TrackIsolationInput& input, TrackIsolation& result )  const
  {
    /// prepare look-up structure
    std::vector<const TrackParticle*> tps;
#ifndef XAOD_ANALYSIS
     if( !m_tracksInConeTool->particlesInCone(input.particle->eta(),input.particle->phi(),input.maxRadius,tps) ) return false;
#else
     if( !getparticlesInCone(input.particle->eta(),input.particle->phi(),input.maxRadius,tps) ) return false;
#endif

    for( const auto& tp : tps ) {
      if( ! m_trkselTool->accept( *tp , input.vertex ) ){
	ATH_MSG_DEBUG("reject track pt = " << tp->pt());
	continue;
      } else
	ATH_MSG_DEBUG("Accept track, pt = " << tp->pt());
      add( input,*tp, result );

    }
    return true;
  }

  bool TrackIsolationTool::simpleIsolation( TrackIsolationInput& input, TrackIsolation& result, const TrackParticleContainer* indetTrackParticles ) const
  {

    SG::ReadHandle<TrackParticleContainer> tpH;
    /// retrieve track particles if not passed into the interface
    if( !indetTrackParticles ) {
      tpH = SG::makeHandle(m_indetTrackParticleLocation);
      if (!tpH.isValid()) {
	ATH_MSG_ERROR("Could not open a TrackParticle container with key "
		      << m_indetTrackParticleLocation.key());
	return false;
      }
      indetTrackParticles = tpH.ptr();
    }

    // exit if not track particles found
    if( !indetTrackParticles ) return false;

    // loop over all track particles
    for( const auto& tp : *indetTrackParticles ) {
      if( ! m_trkselTool->accept( *tp , input.vertex ) ){
	ATH_MSG_DEBUG("[2] reject track pt = " << tp->pt());
	continue;
      }

      add( input, *tp, result );
    }

    return true;
  }

  void TrackIsolationTool::add( TrackIsolationInput& input, const TrackParticle& tp2, TrackIsolation& result ) const
  {
    // check if track pointer matches the one of input or one of the exclusion set
    if(input.corrections.trackbitset.test(static_cast<unsigned int>(Iso::coreTrackPtr))){
	 if(input.particle == &tp2 || (input.exclusionSet && input.exclusionSet->count(&tp2))){
	   ATH_MSG_DEBUG("track pointer " << &tp2 << ", track pt = " << tp2.pt() << ", input pt = " << input.particle->pt()) ;
	   result.coreCorrections[Iso::coreTrackPtr] += tp2.pt();
	   return;
	 }
       }

    // check eta
    float deta = input.particle->eta()-tp2.eta();
    if( fabs(deta) > input.maxRadius ) return;

    // check phi
    float dphi = phiInRange(input.particle->phi()-tp2.phi());
    if( fabs(dphi) > input.maxRadius ) return;

    // check dr2
    float dr2 = deta*deta + dphi*dphi;

    // check cone if using cone based overlap removal
    if(input.corrections.trackbitset.test(static_cast<unsigned int>(Iso::coreTrackCone))
       && dr2 < m_overlapCone2 ) {
      result.coreCorrections[Iso::coreTrackCone] += tp2.pt();
      return;
    }

    /// sum up particle to the different cones
    for( unsigned int k=0;k<input.coneSizesSquared.size();++k ){
      if( dr2 >= input.coneSizesSquared[k] ) return;
      result.ptcones[k] += tp2.pt();
      if( dr2 <= input.ptvarconeRadiusSquared ){
        result.ptvarcones_10GeVDivPt[k] += tp2.pt();
      }
    }
  }

  void TrackIsolationTool::initresult(TrackIsolation& result,
				      TrackCorrection corrlist,
				      unsigned int typesize) const {

    result.corrlist = corrlist;
    result.coreCorrections.clear();
    result.ptcones.resize(typesize,0.);
    result.ptvarcones_10GeVDivPt.resize(typesize,0.);

    std::vector<float> vec;
    vec.resize(typesize,0.);

    for(unsigned int i=0;i<static_cast<unsigned int>(Iso::numIsolationTrackCorrections);i++){
      result.coreCorrections[static_cast<Iso::IsolationTrackCorrection>(i)] = 0.;
    }
  }

#ifdef XAOD_ANALYSIS
  bool TrackIsolationTool::getparticlesInCone( float eta, float phi, float dr, std::vector< const TrackParticle*>& output ) const {

    /// retrieve container
    SG::ReadHandle<TrackParticleContainer> trks(m_indetTrackParticleLocation);
    if(!trks.isValid()) {
      ATH_MSG_ERROR("Could not find TrackParticle container with key "
		    << m_indetTrackParticleLocation.key());
      return false;
    }

    ATH_MSG_DEBUG("checing track eta=" << eta << ", phi=" << phi);
    /// make selection
    float dr2 = dr*dr;
    for(auto trk: *trks){
      float dEta = fabs(eta-trk->eta());
      if(dEta>dr) continue;
      float dPhi = fabs(phi-trk->phi());
      while(dPhi>M_PI){dPhi = 2.*M_PI -dPhi;}
      if(dPhi>dr) continue;
      ATH_MSG_DEBUG("ID trk pt=" << trk->pt()*0.001 << " eta=" << trk->eta() << ", phi=" << trk->phi() << " dEta=" << dEta << ", dPhi=" << dPhi << ", dR2 = " << dEta*dEta+dPhi*dPhi);
      if(dr2>(dEta*dEta+dPhi*dPhi)) output.push_back(trk);
    }

    return true;
  }
#endif // XAOD_ANALYSIS

}	// end of namespace
