/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonCnvAlg.cxx 298140 2013-11-19 11:32:49Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "muonEvent/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"

// Local include(s):
#include "MuonCnvAlg.h"

namespace xAODMaker {

  MuonCnvAlg::MuonCnvAlg( const std::string& name,
    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ),
      m_muonDressingTool("MuonCombined::MuonDressingTool/MuonDressingTool")
  {

    declareProperty( "AODContainerName", m_aodContainerName = "Muons" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "Muons" );
    declareProperty( "xAODInDetTrackParticleContainerName", m_inDetTrackParticles = "InDetTrackParticles" );
    declareProperty( "xAODSATrackParticleContainerName", m_saTrackParticles = "ExtrapolatedMuonTrackParticles" );
    declareProperty( "xAODCombinedTrackParticleContainerName", m_cbTrackParticles = "" );
  }

  StatusCode MuonCnvAlg::initialize() {

    ATH_MSG_DEBUG( "AODContainerName  = " << m_aodContainerName << "  xAODContainerName = " << m_xaodContainerName );
    ATH_CHECK(m_muonDressingTool.retrieve());

      // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode MuonCnvAlg::execute() {

    // Retrieve the AOD particles:
    const Analysis::MuonContainer* aod = evtStore()->tryConstRetrieve<Analysis::MuonContainer>(m_aodContainerName);
    if (!aod) {
      ATH_MSG_DEBUG("No MuonContainer with key " << m_aodContainerName << " found. Do nothing.");
      return StatusCode::SUCCESS;
    }


    ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );

    // Create the xAOD container and its auxiliary store:
    xAOD::MuonContainer* xaod = new xAOD::MuonContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::MuonAuxContainer* aux = new xAOD::MuonAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded Muons with key: "
      << m_xaodContainerName );

    // Create the xAOD objects:
    Analysis::MuonContainer::const_iterator itr = aod->begin();
    Analysis::MuonContainer::const_iterator end = aod->end();
    for( ; itr != end; ++itr ) {
      // Create the xAOD object:
      xAOD::Muon* muon = new xAOD::Muon();
      xaod->push_back( muon );
      
      // p4
      muon->setP4((*itr)->pt(),(*itr)->eta(),(*itr)->phi());
      
      // author(s)
      muon->setAuthor( static_cast<xAOD::Muon_v1::Author>((*itr)->author()));//FIXME - error prone. Only works if they have the same indices
      muon->setAllAuthors( (*itr)->allAuthors());
      
      //type  
      if ((*itr)->isCombinedMuon())
        muon->setMuonType(xAOD::Muon::Combined);
      else if ((*itr)->isStandAloneMuon())
        muon->setMuonType(xAOD::Muon::MuonStandAlone);
      else if ((*itr)->isSegmentTaggedMuon())
        muon->setMuonType(xAOD::Muon::SegmentTagged);
      else if ((*itr)->isCaloMuonId())
        muon->setMuonType(xAOD::Muon::CaloTagged);
      else if ((*itr)->isSiliconAssociatedForwardMuon())  
        muon->setMuonType(xAOD::Muon::SiliconAssociatedForwardMuon);
      else 
        ATH_MSG_WARNING("Unknown type of AOD Muon");

      //setQuality
      xAOD::Muon::Quality tightness;
      if ((*itr)->isTight()) {
        tightness=xAOD::Muon::Tight;
      } else if ((*itr)->isMedium()) {
        tightness=xAOD::Muon::Medium;
      } else {
        tightness=xAOD::Muon::Loose;
      }
      muon->setQuality( tightness );
      
      //setParameters
      setParameters(**itr,*muon);
      
      //setIsolations
      setIsolations(**itr,*muon);
      
      // set Links
      setLinks(**itr,*muon); 
      
      // ATH_MSG_VERBOSE(" AOD Muon pt " << (*itr)->pt() << " eta " << (*itr)->eta() << " phi " << (*itr)->phi());
      ATH_MSG_VERBOSE(" xAOD Muon pt " << muon->pt() << " eta " << muon->eta() << " phi " << muon->phi());
      ATH_MSG_VERBOSE(" author " << muon->author() << " type " << muon->muonType() 
          << " links CB " << muon->combinedTrackParticleLink().isValid() 
          << " ID " << muon->inDetTrackParticleLink().isValid() 
          << " MS " << muon->muonSpectrometerTrackParticleLink().isValid() 
          << " Primary " << muon->primaryTrackParticleLink().isValid() 
          );

      setMuonHitContent(**itr,*muon); 
    }

      // Return gracefully:
    return StatusCode::SUCCESS;
  }
  
  void MuonCnvAlg::setParameters(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon){
    // We're not doing all AOD parameters here because some are dropped, and some are moved elsewhere.
    checkAndSetParameter(MuonParameters::spectrometerFieldIntegral        ,   xAOD::Muon::spectrometerFieldIntegral        ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::scatteringCurvatureSignificance  ,   xAOD::Muon::scatteringCurvatureSignificance  ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::scatteringNeighbourSignificance  ,   xAOD::Muon::scatteringNeighbourSignificance  ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::momentumBalanceSignificance      ,   xAOD::Muon::momentumBalanceSignificance      ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::segmentDeltaEta                  ,   xAOD::Muon::segmentDeltaEta                  ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::segmentDeltaPhi                  ,   xAOD::Muon::segmentDeltaPhi                  ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::t0                               ,   xAOD::Muon::t0                               ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::beta                             ,   xAOD::Muon::beta                             ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::annBarrel                        ,   xAOD::Muon::annBarrel                        ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::annEndCap                        ,   xAOD::Muon::annEndCap                        ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::innAngle                         ,   xAOD::Muon::innAngle                         ,  aodmuon,   xaodmuon);
    checkAndSetParameter(MuonParameters::midAngle                         ,   xAOD::Muon::midAngle                         ,  aodmuon,   xaodmuon);   
  }

  void MuonCnvAlg::setMuonHitContent(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon){

    const Rec::TrackParticle* aTrackParticle = aodmuon.track();
    if( !aTrackParticle->trackSummary() ) return;
    m_muonDressingTool->addMuonHitSummary(xaodmuon,aTrackParticle->trackSummary());
  }
  
  
  void MuonCnvAlg::checkAndSetParameter(MuonParameters::ParamDef aodParameter,xAOD::Muon::ParamDef xaodParameter, const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon){
    double result = aodmuon.parameter(aodParameter);
    if (result>-999.) {
      float parameter = static_cast<float>(result);
      xaodmuon.setParameter(parameter, xaodParameter);
    }
  }
  
  void MuonCnvAlg::setIsolations(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon){
    //checkAndSetIsolation(MuonParameters::etcone10     ,   xAOD::Iso::etcone10     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::etcone20     ,   xAOD::Iso::etcone20     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::etcone30     ,   xAOD::Iso::etcone30     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::etcone40     ,   xAOD::Iso::etcone40     ,  aodmuon,   xaodmuon);
    //checkAndSetIsolation(MuonParameters::nucone10     ,   xAOD::Iso::nucone10     ,  aodmuon,   xaodmuon);
    //checkAndSetIsolation(MuonParameters::nucone20     ,   xAOD::Iso::nucone20     ,  aodmuon,   xaodmuon);
    //checkAndSetIsolation(MuonParameters::nucone30     ,   xAOD::Iso::nucone30     ,  aodmuon,   xaodmuon);
    //checkAndSetIsolation(MuonParameters::nucone40     ,   xAOD::Iso::nucone40     ,  aodmuon,   xaodmuon);
    //checkAndSetIsolation(MuonParameters::ptcone10     ,   xAOD::Iso::ptcone10     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::ptcone20     ,   xAOD::Iso::ptcone20     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::ptcone30     ,   xAOD::Iso::ptcone30     ,  aodmuon,   xaodmuon);
    checkAndSetIsolation(MuonParameters::ptcone40     ,   xAOD::Iso::ptcone40     ,  aodmuon,   xaodmuon);   
    //checkAndSetIsolation(MuonParameters::etconeNoEm10 ,   xAOD::Iso::etconeNoEm10 ,  aodmuon,   xaodmuon);   
    //checkAndSetIsolation(MuonParameters::etconeNoEm20 ,   xAOD::Iso::etconeNoEm20 ,  aodmuon,   xaodmuon);   
    //checkAndSetIsolation(MuonParameters::etconeNoEm30 ,   xAOD::Iso::etconeNoEm30 ,  aodmuon,   xaodmuon);   
    //checkAndSetIsolation(MuonParameters::etconeNoEm40 ,   xAOD::Iso::etconeNoEm40 ,  aodmuon,   xaodmuon);   
  }
  
  void MuonCnvAlg::checkAndSetIsolation(MuonParameters::ParamDef aodParameter,xAOD::Iso::IsolationType xaodParameter, const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon){
    double result = aodmuon.parameter(aodParameter);
    if (result>-999.) {
      float isolation = static_cast<float>(aodmuon.parameter(aodParameter));
      xaodmuon.setIsolation(isolation, xaodParameter);
    }
  }
  
  void MuonCnvAlg::setLinks(const Analysis::Muon& aodmuon, xAOD::Muon& xaodmuon) const {
    // Need to reset links from old TrackParticle to xAOD::TrackParticles
    // ElementLink<xAOD::TrackParticleContainer > newLink = getNewLink(aodmuon.inDetTrackLink(), m_inDetTrackParticles);
    xaodmuon.setTrackParticleLink(xAOD::Muon_v1::InnerDetectorTrackParticle,   getNewLink(aodmuon.inDetTrackLink(), m_inDetTrackParticles) );
    // ElementLink<xAOD::TrackParticleContainer > newLink2 = getNewLink(aodmuon.muonExtrapTrackLink(), m_saTrackParticles);
    xaodmuon.setTrackParticleLink(xAOD::Muon_v1::MuonSpectrometerTrackParticle,getNewLink(aodmuon.muonExtrapTrackLink(), m_saTrackParticles) );
    // ElementLink<xAOD::TrackParticleContainer > newLink3 = getNewLink(aodmuon.muonExtrapTrackLink(), m_cbTrackParticles);
    xaodmuon.setTrackParticleLink(xAOD::Muon_v1::CombinedTrackParticle,        getNewLink(aodmuon.combinedTrackLink(), m_cbTrackParticles) );
    
    // Sanity checks - can remove at some stage
    if (!xaodmuon.primaryTrackParticleLink().isValid()){
      ATH_MSG_WARNING(" xAOD::Muon with no primary TrackParticle! Type: " << xaodmuon.muonType());
      ATH_MSG_WARNING(" author " << xaodmuon.author() << " type " << xaodmuon.muonType() 
          << " links CB " << xaodmuon.combinedTrackParticleLink().isValid() 
          << " ID " << xaodmuon.inDetTrackParticleLink().isValid() 
          << " MS " << xaodmuon.muonSpectrometerTrackParticleLink().isValid() 
          << " Primary " << xaodmuon.primaryTrackParticleLink().isValid() 
          );
      ATH_MSG_WARNING(" AOD::Muon links CB " << aodmuon.combinedTrackLink().isValid() 
          << " ID " << aodmuon.combinedTrackLink().isValid() 
          << " MS " << aodmuon.muonExtrapTrackLink().isValid() 
          );
    }
  }
  
  ElementLink<xAOD::TrackParticleContainer> MuonCnvAlg::getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, const std::string& name) const{
    ElementLink<xAOD::TrackParticleContainer> newLink;
    std::string destname = name;
    if (destname.empty())
      destname = oldLink.dataID();
    newLink.resetWithKeyAndIndex( destname, oldLink.index() );
    ATH_MSG_VERBOSE("MuonCnvAlg::getNewLink - Old link is "<<(oldLink.isValid()?"VALID":"INVALID")<< " with SGKey = "<<oldLink.dataID()<< " & index = "<<oldLink.index()
             <<" and new link is "<<(newLink.isValid()?"VALID":"INVALID")<< " with SGKey = "<<newLink.dataID()<< " & index = "<<newLink.index());
    return newLink;
  }
  
} // namespace xAODMaker
