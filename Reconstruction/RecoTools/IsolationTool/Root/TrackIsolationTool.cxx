/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// TrackIsolationTool
//
//  (c) ATLAS software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "IsolationTool/TrackIsolationTool.h"
// #include "InDetTrackSelectionTool/IInDetTrackSelectionTool.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include "xAODPrimitives/IsolationCorrectionHelper.h"
#include "xAODPrimitives/tools/getIsolationDecorator.h"
#include "xAODPrimitives/tools/getIsolationCorrectionDecorator.h"
#include "xAODMuon/Muon.h"

#include <iomanip>

namespace xAOD {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  TrackIsolationTool::TrackIsolationTool (const std::string& name):
    asg::AsgTool(name)
#ifndef XAOD_ANALYSIS
    , m_tracksInConeTool("xAOD::TrackParticlesInConeTool/TrackParticlesInConeTool", this),
	m_trkselTool( "InDet::InDetTrackSelectionTool/TrackSelectionTool", this )
#endif // XAOD_ANALYSIS
  {
#ifndef XAOD_ANALYSIS
    declareInterface<ITrackIsolationTool>(this);
    //declareInterface<IChargedEFlowIsolationTool>(this);
    declareProperty("TracksInConeTool", m_tracksInConeTool);
#endif // XAOD_ANALYSIS
    declareProperty("TrackParticleLocation",m_indetTrackParticleLocation = "InDetTrackParticles");
    declareProperty("SimpleIsolation",  m_simpleIsolation = false);
    declareProperty("OverlapCone",      m_overlapCone2 = 0.1); // will be squared later
    declareProperty("TrackSelectionTool", m_trkselTool );
  }

  TrackIsolationTool::~TrackIsolationTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode TrackIsolationTool::initialize() 
  {
#ifndef XAOD_ANALYSIS
    if( !m_simpleIsolation ) ATH_CHECK(m_tracksInConeTool.retrieve());
#endif // XAOD_ANALYSIS

      if(m_trkselTool.retrieve().isFailure()){
	ATH_MSG_FATAL("Could not retrieve InDetTrackSelectionTool");    
	return StatusCode::FAILURE;
      }

    /** square cone */
    m_overlapCone2 *= m_overlapCone2;
    
    return StatusCode::SUCCESS;
  }

  const TrackParticleContainer* TrackIsolationTool::retrieveTrackParticleContainer() const 
  {
    const TrackParticleContainer* indetTrackParticles = 0;
    if(evtStore()->contains<TrackParticleContainer>(m_indetTrackParticleLocation)) {
      if(evtStore()->retrieve(indetTrackParticles,m_indetTrackParticleLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << m_indetTrackParticleLocation );
        return 0;
      }
    }
    return indetTrackParticles;
  }

  const Vertex* TrackIsolationTool::retrieveIDBestPrimaryVertex() const 
  {
    std::string PVXLocation = "PrimaryVertices";
    const VertexContainer *vtxC = 0;
    if (evtStore()->contains<VertexContainer>(PVXLocation)) {
      if (evtStore()->retrieve(vtxC,PVXLocation).isFailure()) {
        ATH_MSG_FATAL( "Unable to retrieve " << PVXLocation);
        return 0;
      } else {
	if (vtxC->size() == 0) {
	  ATH_MSG_INFO("No vertex in container " << PVXLocation);
	  return 0;
	}
	//return vtxC->front(); // the first one, probably the beam spot if a single one in the container
	// In fact, should rather do like that, in case front is not PriVtx
	for (const auto* const vtx : *vtxC ) {
	  if (vtx->vertexType() == xAOD::VxType::PriVtx) 
	    return vtx;
	}
      }
    }
    return 0;
  }

  const IParticle* TrackIsolationTool::getReferenceParticle(const IParticle& particle) const {
    const TrackParticle* tp = dynamic_cast<const TrackParticle*>(&particle);
    if( tp ) return tp;
    const Muon* muon = dynamic_cast<const Muon*>(&particle);
    if( muon ) {
      const xAOD::TrackParticle* tp = 0;
      if(muon->inDetTrackParticleLink().isValid()) tp = *muon->inDetTrackParticleLink();
      if( !tp ) tp = muon->primaryTrackParticle();
      if( !tp ) {
        ATH_MSG_WARNING(" No TrackParticle found for muon " );
        return 0;
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
    if (vertex == 0) { 
      vertex = retrieveIDBestPrimaryVertex();
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
        ATH_MSG_WARNING("Unsupported isolation type passed, cannot calculate isolation " << Iso::toString(isoType));
        return false;
      }
      float conesize = Iso::coneSize(isoType);
      input.coneSizesSquared.push_back(conesize*conesize);
    }

    for(size_t i=1; i<input.coneSizesSquared.size(); i++){
      if(input.coneSizesSquared[i]>input.coneSizesSquared[i-1]) ATH_MSG_ERROR("Isolation Cone should be in decreasing order. "<< i << "th variable is " << Iso::toString(isoTypes[i]) << " is larger than the one before it: " << Iso::toString(isoTypes[i-1]));
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

  bool TrackIsolationTool::decorateParticle( const IParticle& tp, 
                                             const std::vector<Iso::IsolationType>& cones, 
                                             TrackCorrection corrections, 
                                             const Vertex* vertex, 
                                             const std::set<const TrackParticle*>* exclusionSet, 
                                             const TrackParticleContainer* indetTrackParticles ) {
    
    // calculate the isolation
    TrackIsolation result;
    if( !trackIsolation(result,tp,cones,corrections,vertex,exclusionSet,indetTrackParticles) ) {
      ATH_MSG_DEBUG("Calculation of TrackIsolation failed");
      return false;
    }

    // get the applied corrections
    std::vector<Iso::IsolationTrackCorrection> correctionTypes;
    Iso::IsolationTrackCorrectionBitsetHelper::decode(corrections.trackbitset,correctionTypes);
    ATH_MSG_DEBUG("Decoded correction types: " << correctionTypes.size() );


    // decorate the track particle

    // This is independant of the size. At least for the time being
    // fill bitset
    SG::AuxElement::Decorator< uint32_t >* bitsetAcc = getIsolationCorrectionBitsetDecorator(Iso::isolationFlavour(cones[0]));

    if( bitsetAcc ){
      (*bitsetAcc)(tp) = corrections.trackbitset.to_ulong();
    }
    
    // fill corrections
    for( auto ctype : correctionTypes ){
      auto el = result.coreCorrections.find(ctype);
      if( el == result.coreCorrections.end() ){
	ATH_MSG_WARNING("Correction value not found " << Iso::toString(ctype) );         
	continue;
      }
      SG::AuxElement::Decorator< float >* isoCorAcc = getIsolationCorrectionDecorator( Iso::isolationFlavour(cones[0]), ctype );
      if( isoCorAcc ){
	(*isoCorAcc)(tp) = el->second;
      }        
    }

    // loop over cones
    for( unsigned int i=0;i<cones.size();++i ){

      Iso::IsolationType type = cones[i];
      //Iso::IsolationFlavour flavour = Iso::isolationFlavour(type);
      Iso::IsolationConeSize coneSize = enumconeSize(type);

      // fill main isolation
      if( result.ptcones.size() == cones.size() ){
        SG::AuxElement::Decorator< float >* isoTypeAcc = getIsolationDecorator(type);
        if( isoTypeAcc ){
	  ATH_MSG_DEBUG("Filling std cone " << result.ptcones[i]);
          (*isoTypeAcc)(tp) = result.ptcones[i];
        }
      }else if( !result.ptcones.empty() ){
        ATH_MSG_WARNING("Inconsistent ptcones vector size: " << result.ptvarcones_10GeVDivPt.size() << " number of cones " << cones.size() );
      }

      // also fill var cone
      if( result.ptvarcones_10GeVDivPt.size() == cones.size() ){
        Iso::IsolationType varIsoType = Iso::isolationType( Iso::ptvarcone, coneSize );
        SG::AuxElement::Decorator< float >* isoTypeAcc = getIsolationDecorator(varIsoType);
        if( isoTypeAcc ){
	  ATH_MSG_DEBUG("Filling var cone " << result.ptvarcones_10GeVDivPt[i]);
          (*isoTypeAcc)(tp) = result.ptvarcones_10GeVDivPt[i];
        }
      }else if( !result.ptvarcones_10GeVDivPt.empty() ){
        ATH_MSG_WARNING("Inconsistent ptvarcones_10GeVDivPt vector size: " << result.ptvarcones_10GeVDivPt.size() << " number of cones " << cones.size() );
      }

    }
    return true;
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
    /// retrieve track particles if not passed into the interface
    if( !indetTrackParticles ) indetTrackParticles = retrieveTrackParticleContainer();
    
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
    const TrackParticleContainer* trks = retrieveTrackParticleContainer();
    if(!trks) return false;

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
