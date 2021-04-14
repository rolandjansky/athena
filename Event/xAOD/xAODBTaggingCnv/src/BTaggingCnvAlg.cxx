/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODBTagging/BTaggingAuxContainer.h"

// Local include(s):
#include "BTaggingCnvAlg.h"

namespace xAODMaker {

  BTaggingCnvAlg::BTaggingCnvAlg( const std::string& name,
				  ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc ) {

    declareProperty( "AODContainerName", m_aodContainerName = "???" );
    declareProperty( "xAODContainerName", m_xaodContainerName = "BTaggingCollection" );

    //declareProperty( "TrackMapName", m_trackMapName="GSFTrackAssociation",  "Old new Track Map Name" );
    //declareProperty("TrackParticleContainerName"  , m_trackParticleContainerName   = "TrackParticleCandidate");

  }

  StatusCode BTaggingCnvAlg::initialize() {

    ATH_MSG_INFO( "Initializing" );
    ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName );
    ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  StatusCode BTaggingCnvAlg::execute() {

    //// Retrieve the AOD particles:
    //const BTaggingContainer* aod = evtStore()->tryConstRetrieve<BTaggingContainer>(m_aodContainerName); 
    //if (!aod) { 
    //  ATH_MSG_WARNING("No BTaggingCollection with key " << m_aodContainerName << " found. Do nothing."); 
    //  return StatusCode::SUCCESS; 
    //} 
    //ATH_MSG_DEBUG( "Retrieved particles with key: " << m_aodContainerName );

    //const Rec::TrackParticleContainer*  m_trackTES = evtStore()->tryConstRetrieve<Rec::TrackParticleContainer>(m_trackParticleContainerName );
    //if( !m_trackTES ) {
    //  ATH_MSG_WARNING("No AOD TrackParticle container found in TES: StoreGate Key = " <<m_trackParticleContainerName);
    //  return StatusCode::SUCCESS;
    //}
    //ATH_MSG_DEBUG( "Retrieved particles with key: " << m_trackParticleContainerName );

    /////////////////////////////////////////////////////////////////////////////////////////

    // Create the xAOD container and its auxiliary store:
    xAOD::BTaggingContainer* xaod = new xAOD::BTaggingContainer();
    CHECK( evtStore()->record( xaod, m_xaodContainerName ) );
    xAOD::BTaggingAuxContainer* aux = new xAOD::BTaggingAuxContainer();
    CHECK( evtStore()->record( aux, m_xaodContainerName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded BTaggings with key: " << m_xaodContainerName );

//    // Create the xAOD objects:
//    BTaggingContainer::const_iterator itr = aod->begin();
//    BTaggingContainer::const_iterator end = aod->end();
//
//    for( ; itr != end; ++itr ) {
//
//      xAOD::BTagging* btag = new xAOD::BTagging();
//      // Create the xAOD object:
//      xaod->push_back( btag );
//      // author(s)
//      btag->setAuthor( (*itr)->author() );
//      //OQ
//      btag->setOQ( (*itr)->isgoodoq() );
//      // charge
//      btag->setCharge( (*itr)->charge() );
//
////      //setParameters
////      setParameters(**itr,*btag);
////	//setTrackMatch
////	setTrackMatch(**itr,*btag);	
////	// set Links
////	setLinks(**itr,*btag); 
//
//    }

    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  //void BTaggingCnvAlg::setParameters(const Analysis::BTagging& aodel, xAOD::BTagging& xaodel){
  //  // We're not doing all AOD parameters here because some are dropped, and some are moved elsewhere.
  //  checkAndSetParameter(egammaParameters::e011        ,   xAOD::EgammaParameters::e011        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e033        ,   xAOD::EgammaParameters::e033        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e132        ,   xAOD::EgammaParameters::e132        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e1152       ,   xAOD::EgammaParameters::e1152       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::ethad1      ,   xAOD::EgammaParameters::ethad1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::ehad1       ,   xAOD::EgammaParameters::ehad1       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::f1          ,   xAOD::EgammaParameters::f1          ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::f3          ,   xAOD::EgammaParameters::f3          ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::f1core      ,   xAOD::EgammaParameters::f1core      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::f3core      ,   xAOD::EgammaParameters::f3core      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e233        ,   xAOD::EgammaParameters::e233        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e235        ,   xAOD::EgammaParameters::e235        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e255        ,   xAOD::EgammaParameters::e255        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e237        ,   xAOD::EgammaParameters::e237        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e277        ,   xAOD::EgammaParameters::e277        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e333        ,   xAOD::EgammaParameters::e333        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e335        ,   xAOD::EgammaParameters::e335        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e337        ,   xAOD::EgammaParameters::e337        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e377        ,   xAOD::EgammaParameters::e377        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::weta1       ,   xAOD::EgammaParameters::weta1       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::weta2       ,   xAOD::EgammaParameters::weta2       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e2ts1       ,   xAOD::EgammaParameters::e2ts1       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::e2tsts1     ,   xAOD::EgammaParameters::e2tsts1     ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::fracs1      ,   xAOD::EgammaParameters::fracs1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::widths1     ,   xAOD::EgammaParameters::widths1     ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::widths2     ,   xAOD::EgammaParameters::widths2     ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::poscs1      ,   xAOD::EgammaParameters::poscs1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::poscs2      ,   xAOD::EgammaParameters::poscs2      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::asy1        ,   xAOD::EgammaParameters::asy1        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::pos         ,   xAOD::EgammaParameters::pos         ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::pos7        ,   xAOD::EgammaParameters::pos7        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::barys1      ,   xAOD::EgammaParameters::barys1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::wtots1      ,   xAOD::EgammaParameters::wtots1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::emins1      ,   xAOD::EgammaParameters::emins1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::emaxs1      ,   xAOD::EgammaParameters::emaxs1      ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::r33over37allcalo ,   xAOD::EgammaParameters::r33over37allcalo,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::ecore       ,   xAOD::EgammaParameters::ecore       ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::zvertex     ,   xAOD::EgammaParameters::zvertex     ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::errz        ,   xAOD::EgammaParameters::errz        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::etap        ,   xAOD::EgammaParameters::etap        ,  aodel,   xaodel);
  //  checkAndSetParameter(egammaParameters::depth       ,   xAOD::EgammaParameters::depth       ,  aodel,   xaodel);
  //}
  
  //void BTaggingCnvAlg::checkAndSetParameter(egammaParameters::ParamDef aodParameter,
  //      				    xAOD::EgammaParameters::ShowerShapeType xaodParameter, 
  //      				    const Analysis::BTagging& aodel, xAOD::BTagging& xaodel){
  //  double result = aodel.detailValue(aodParameter);
  //  float parameter = static_cast<float>(result);
  //  xaodel.setShowerShapeValue(parameter, xaodParameter);
  //}
  
  //void BTaggingCnvAlg::setLinks(const Analysis::BTagging& aodel, xAOD::BTagging& xaodel) const {
  //  // Need to reset links from old CaloCluster to xAOD::CaloCluster
  //  ElementLink<xAOD::CaloClusterContainer> newclusterElementLink;
  //  if( aodel.author() & egammaParameters::AuthorBTagging ){ 
  //    newclusterElementLink.resetWithKeyAndIndex( m_caloClusters, aodel.clusterElementLink().index()  );
  //
  //    std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
  //    for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
  //      linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticlesGSF) );
  //    }
  //    xaodel.setTrackParticles( linksToTracks );
  //
  //  }
  //  else if ( aodel.author() &  egammaParameters::AuthorSofte ) {
  //    newclusterElementLink.resetWithKeyAndIndex( m_caloClustersSofte, aodel.clusterElementLink().index()  );
  //  
  //    //softe do not use GSF Tracks
  //    std::vector< ElementLink< xAOD::TrackParticleContainer > > linksToTracks;
  //    for(unsigned int i(0); i<aodel.nTrackParticles(); ++i){
  //      linksToTracks.push_back( getNewLink(aodel.trackParticleElementLink(i), m_inDetTrackParticles) );
  //    }
  //    xaodel.setTrackParticles( linksToTracks );
  //  
  //  }
  //  else{ 
  //    newclusterElementLink.resetWithKeyAndIndex( m_caloClustersFrwd, aodel.clusterElementLink().index()  );
  //  }
  //
  //  std::vector< ElementLink< xAOD::CaloClusterContainer > > linksToClusters;
  //  linksToClusters.push_back(newclusterElementLink);
  //  xaodel.setCaloClusters(linksToClusters);
  //}
  
  //ElementLink<xAOD::TrackParticleContainer> BTaggingCnvAlg::getNewLink(const ElementLink<Rec::TrackParticleContainer>& oldLink, 
  //      							       const std::string& name) const{
  //  ElementLink<xAOD::TrackParticleContainer> newLink;
  //  newLink.resetWithKeyAndIndex( name, oldLink.index() );
  //  return newLink;
  //}

} // namespace xAODMaker

