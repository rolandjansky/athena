/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

//----------------------------------------------------------------//
#include "TrkToolInterfaces/ITrackParticleCreatorTool.h"
#include "Particle/TrackParticle.h"
#include "Particle/TrackParticleContainer.h"

/////////////////////////////////////////////////////////
#include "MboyEDMTool/MuonboyToParticleTool.h"

MuonboyToParticleTool::MuonboyToParticleTool(const std::string& t, 
                                            const std::string& n,
                                            const IInterface*  p ):AthAlgTool(t,n,p),
p_ITrackParticleCreatorTool("Trk::TrackParticleCreatorTool/MuonParticleCreatorTool")
{
  declareInterface<IMuonboyToParticleTool>(this);

  declareProperty("ITrackParticleCreatorTool", p_ITrackParticleCreatorTool);
  
}

MuonboyToParticleTool::~MuonboyToParticleTool(){}

// Initialize
StatusCode MuonboyToParticleTool::initialize() {

  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_INFO( "Initialisation started     " << name() << " - package version " << PACKAGE_VERSION  ) ;

  sc = AthAlgTool::initialize(); 
  if ( sc.isFailure() ) {
    ATH_MSG_FATAL(  " AthAlgTool::initialize() failed" ) ;
    return( StatusCode::FAILURE );
  }

//Get MBToTrackTool
  if ( p_ITrackParticleCreatorTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL(  "Failed to retrieve tool " << p_ITrackParticleCreatorTool ) ;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO(  "Retrieved tool " << p_ITrackParticleCreatorTool ) ;


  ATH_MSG_INFO(  "Initialisation ended     " ) ;

  return StatusCode::SUCCESS;

}

// Finalize
StatusCode MuonboyToParticleTool::finalize(){return StatusCode::SUCCESS;}

// Convert
Rec::TrackParticle* MuonboyToParticleTool::convert(
                                const Trk::Track* /*pTrack*/
){

  //const Trk::VxCandidate* pVxCandidate = 0 ;
  return 0;// p_ITrackParticleCreatorTool->createParticle(pTrack, pVxCandidate, Trk::NoVtx);

}
// Convert
StatusCode MuonboyToParticleTool::convertCollection(
                                const TrackCollection*       pTrackCollection,
                                Rec::TrackParticleContainer* pTrackParticleContainer
){

  for (TrackCollection::const_iterator it = pTrackCollection->begin(); it!=pTrackCollection->end(); ++it){

    const Trk::Track* pTrack = *it ;
    Rec::TrackParticle* pTrackParticle = convert(pTrack);

    if (!pTrackParticle) {
      ATH_MSG_DEBUG( " convert failed " ) ;
    }

    if ( pTrackParticle ) {
//    Set the muon mass 
      const  Trk::Track* pTrackOfpTrackParticle = pTrackParticle->originalTrack();
      double ThePX = ((pTrackOfpTrackParticle->perigeeParameters())->momentum()).x() ;
      double ThePY = ((pTrackOfpTrackParticle->perigeeParameters())->momentum()).y() ;
      double ThePZ = ((pTrackOfpTrackParticle->perigeeParameters())->momentum()).z() ;
      double TheMuonMass  = 105.659 ;
      double TheE  = sqrt( (TheMuonMass*TheMuonMass)+(ThePX*ThePX)+(ThePY*ThePY)+(ThePZ*ThePZ) );
      pTrackParticle->setE(TheE);
      pTrackParticle->setStorableObject(pTrackCollection);
      pTrackParticleContainer->push_back(pTrackParticle);
    }

  }


  return StatusCode::SUCCESS;

}
