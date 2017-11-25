/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSeededSegmentFinder.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"

// interfaces
#include "TrkExInterfaces/IPropagator.h"
#include "MuonRecToolInterfaces/IMuonSegmentMaker.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"


#include <iostream>

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonIdHelpers/MdtIdHelper.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"

#include "TrkParameters/TrackParameters.h"

#include "TrkEventPrimitives/LocalParameters.h"
#include "TrkMeasurementBase/MeasurementBase.h"

#include "TrkSurfaces/StraightLineSurface.h"

#include "TrkRoad/TrackRoad.h"
#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

namespace Muon {

  MuonSeededSegmentFinder::MuonSeededSegmentFinder(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_segMaker("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
      m_segMakerNoHoles("Muon::DCMathSegmentMaker/DCMathSegmentMaker"),
      m_propagator("Trk::RungeKuttaPropagator/AtlasRungeKuttaPropagator"),
      m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
      m_magFieldProperties(Trk::NoField),
      m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
      m_mdtPrdContainer(0),
      m_rpcPrdContainer(0),
      m_tgcPrdContainer(0),
      m_cscPrdContainer(0)
  {
    declareInterface<IMuonSeededSegmentFinder>(this);

    declareProperty("IdHelper",m_idHelper);
    declareProperty("Printer",m_printer);
    declareProperty("MdtRotCreator",  m_mdtRotCreator );
    declareProperty("Propagator",  m_propagator );
    declareProperty("SegmentMaker",   m_segMaker);
    declareProperty("SegmentMakerNoHoles",   m_segMakerNoHoles);
    declareProperty("MdtPrepDataContainer", m_key_mdt="MDT_DriftCircles");
    declareProperty("RpcPrepDataContainer", m_key_rpc="RPC_Measurements");
    declareProperty("TgcPrepDataContainer", m_key_tgc="TGC_Measurements");
    declareProperty("CscPrepDataContainer", m_key_csc="CSC_Clusters");
    declareProperty("AdcCut", m_adcCut = 50 );
    declareProperty("MdtSigmaFromTrack",m_maxSigma = 3);
  }


  MuonSeededSegmentFinder::~MuonSeededSegmentFinder(){}


  StatusCode MuonSeededSegmentFinder::initialize()
  {

    ATH_CHECK( detStore()->retrieve( m_detMgr ) );
    ATH_CHECK( m_segMaker.retrieve() );
    ATH_CHECK( m_segMakerNoHoles.retrieve() );
    ATH_CHECK( m_propagator.retrieve() );
    ATH_CHECK( m_mdtRotCreator.retrieve() );
    ATH_CHECK( m_idHelper.retrieve() );
    
    return StatusCode::SUCCESS;
  }
  StatusCode MuonSeededSegmentFinder::finalize()
  {
    return StatusCode::SUCCESS;
  }



  std::vector<const MuonSegment*>* MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars, const std::set<Identifier>& chIds ) const {

    // get MdtPrepData collections correspondign to the chamber Identifiers
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIds );
    
    if( mdtPrds.empty() ){
      ATH_MSG_DEBUG(" no MdtPrepData found ");
      return 0;
    }
    
    // find segments
    return find(pars,mdtPrds);    
  }

  std::vector<const MuonSegment*>* MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars, const std::set<IdentifierHash>& chIdHs ) const {

    // get MdtPrepData collections correspondign to the chamber Identifiers
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIdHs );
    
    if( mdtPrds.empty() ){
      ATH_MSG_DEBUG(" no MdtPrepData found ");
      return 0;
    }
    
    // find segments
    return find(pars,mdtPrds);    
  }

    
  std::vector<const MuonSegment*>* MuonSeededSegmentFinder::find( const Trk::TrackParameters& pars, 
								  const std::vector<const MdtPrepData*>& mdtPrds ) const {

    // are we close to the chamber edge?
    bool doHoleSearch = true;

    // select and calibrate the MdtPrepData
    std::vector<const MdtDriftCircleOnTrack*> mdtROTs;
    mdtROTs.reserve( mdtPrds.size() );
    selectAndCalibrate( pars, mdtPrds, mdtROTs, doHoleSearch );

    if( mdtROTs.empty() ){
      ATH_MSG_DEBUG(" no MdtDriftCircles selected ");
      return 0;
    }

    // create track road
    double roadWidthEta = 1.;
    if( pars.covariance() ){
      double trackError = Amg::error(*pars.covariance(),Trk::theta);
      ATH_MSG_DEBUG(" local track Error on theta " << trackError);
      if( trackError < 0.2 ) trackError = 0.2;
      roadWidthEta = 5.*trackError;
    }
    Trk::TrackRoad road( pars.position(), pars.momentum(), roadWidthEta, 1. );

    // create dummy vector<vector>
    std::vector< std::vector<const MdtDriftCircleOnTrack*> > mdtROTsVec;
    mdtROTsVec.push_back(mdtROTs);
    std::vector< std::vector<const MuonClusterOnTrack*> >    clusterROTsVec;

    // call segment finder
    std::vector<const MuonSegment*>* segments = doHoleSearch ? 
      m_segMaker->find( road, mdtROTsVec, clusterROTsVec, true ) :
      m_segMakerNoHoles->find( road, mdtROTsVec, clusterROTsVec, true );
    
    // delete ROTs
    std::for_each( mdtROTs.begin(), mdtROTs.end(), MuonDeleteObject<const MdtDriftCircleOnTrack>() );

    if( !segments ){
      ATH_MSG_DEBUG(" No segments found ");
      return 0;
    }else{
      ATH_MSG_DEBUG(" Number of segments found: " << segments->size() << std::endl << m_printer->print(*segments));
    }

    return segments;
  }

  std::vector<const MdtPrepData*> MuonSeededSegmentFinder::extractPrds( const std::set<Identifier>& chIds ) const {
    // set of IdHashes corresponding to these identifiers
    std::set<IdentifierHash> chIdHs;
    
    // loop over chambers and get collections
    std::set<Identifier>::const_iterator chit = chIds.begin();
    std::set<Identifier>::const_iterator chit_end = chIds.end();
    for( ;chit!=chit_end;++chit ){
    
      if( !m_idHelper->isMdt(*chit) ) {
	ATH_MSG_WARNING(" Requested chamber is not an MDT:   " << m_idHelper->toStringChamber(*chit));
	continue;
      }
  
      const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(*chit);
      if( !detEl ) {
	ATH_MSG_WARNING(" Requested chamber does not exist in geometry:   " << m_idHelper->toStringChamber(*chit));
	continue;
      }
      IdentifierHash hash_id = detEl->collectionHash();
      chIdHs.insert(hash_id);
    }
    
    // vector to store pointers to collections 
    std::vector<const MdtPrepData*> mdtPrds = extractPrds( chIdHs );
    
    return mdtPrds;
  } 
  
  std::vector<const MdtPrepData*> MuonSeededSegmentFinder::extractPrds( const std::set<IdentifierHash>& chIdHs ) const {

    if (evtStore()->retrieve(m_mdtPrdContainer,m_key_mdt).isFailure()) {
      ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt);
      return std::vector<const MdtPrepData*>();
    }
      
    // vector to store pointers to collections 
    std::vector<const MdtPrepData*> mdtPrds;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      MdtPrepDataContainer::const_iterator colIt = m_mdtPrdContainer->indexFind(*chit);
      if( colIt == m_mdtPrdContainer->end() ){
	//ATH_MSG_DEBUG(" MdtPrepDataCollection for:   " 
	//	     << m_idHelper->toStringChamber(*chit) << "  not found in container ");
	continue;
      }
      
      // reserve space for the new PRDs
      mdtPrds.insert( mdtPrds.end(), (*colIt)->begin(), (*colIt)->end() );
    }

    return mdtPrds;
  }

  void MuonSeededSegmentFinder::extractMdtPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const MdtPrepDataCollection*>& target ) const {

    if( evtStore()->contains<Muon::MdtPrepDataContainer>(m_key_mdt) ) {
      if (evtStore()->retrieve(m_mdtPrdContainer,m_key_mdt).isFailure()) {
        ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt);
        return;
      }
    }else return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      MdtPrepDataContainer::const_iterator colIt = m_mdtPrdContainer->indexFind(*chit);
      if( colIt == m_mdtPrdContainer->end() || (*colIt)->empty() ){
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelper->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());
      
      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }

  void MuonSeededSegmentFinder::extractRpcPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const RpcPrepDataCollection*>& target ) const {

    if( evtStore()->contains<Muon::RpcPrepDataContainer>(m_key_rpc) ) {
      if (evtStore()->retrieve(m_rpcPrdContainer,m_key_rpc).isFailure()) {
        ATH_MSG_WARNING("Cannot retrieve rpcPrepDataContainer " << m_key_rpc);
        return;
      }
    }else return;

      
    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      RpcPrepDataContainer::const_iterator colIt = m_rpcPrdContainer->indexFind(*chit);
      if( colIt == m_rpcPrdContainer->end() || (*colIt)->empty() ){
	//ATH_MSG_DEBUG(" RpcPrepDataCollection for:   " 
	//	     << m_idHelper->toStringChamber(*chit) << "  not found in container ");
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelper->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());
      
      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }

  void MuonSeededSegmentFinder::extractTgcPrdCols( const std::set<IdentifierHash>& chIdHs, 
						   std::vector<const TgcPrepDataCollection*>& target ) const {

    if( evtStore()->contains<Muon::TgcPrepDataContainer>(m_key_tgc) ) {
      if (evtStore()->retrieve(m_tgcPrdContainer,m_key_tgc).isFailure()) {
        ATH_MSG_WARNING("Cannot retrieve tgcPrepDataContainer " << m_key_tgc);
        return;
      }
    }else return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      TgcPrepDataContainer::const_iterator colIt = m_tgcPrdContainer->indexFind(*chit);
      if( colIt == m_tgcPrdContainer->end() || (*colIt)->empty() ){
	//ATH_MSG_DEBUG(" TgcPrepDataCollection for:   " 
	//	     << m_idHelper->toStringChamber(*chit) << "  not found in container ");
	continue;
      }
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelper->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());

      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }

  void MuonSeededSegmentFinder::extractCscPrdCols( const std::set<IdentifierHash>& chIdHs,
						   std::vector<const CscPrepDataCollection*>& target ) const {

    if( evtStore()->contains<Muon::CscPrepDataContainer>(m_key_csc) ) {
      if ( evtStore()->retrieve(m_cscPrdContainer,m_key_csc).isFailure()) {
	ATH_MSG_WARNING("Cannot retrieve cscPrepDataContainer " << m_key_csc);
	return;
      }
    }else return;

    // loop over chambers and get collections
    std::set<IdentifierHash>::const_iterator chit = chIdHs.begin();
    std::set<IdentifierHash>::const_iterator chit_end = chIdHs.end();
    for( ;chit!=chit_end;++chit ){
      CscPrepDataContainer::const_iterator colIt = m_cscPrdContainer->indexFind(*chit);
      if( colIt == m_cscPrdContainer->end() || (*colIt)->empty() ){
	//ATH_MSG_DEBUG(" CscPrepDataCollection for:   " 
	//	     << m_idHelper->toStringChamber(*chit) << "  not found in container ");
	continue;
      }
      
      ATH_MSG_DEBUG(" Adding for:   " 
		    << m_idHelper->toStringChamber( (*colIt)->front()->identify() ) << "  size " 
		    << (*colIt)->size());

      // reserve space for the new PRDs
      target.push_back(*colIt);
    }
  }

  void MuonSeededSegmentFinder::selectAndCalibrate( const Trk::TrackParameters& pars, 
						    const std::vector<const MdtPrepData*>& mdtPrdCols,
						    std::vector<const MdtDriftCircleOnTrack*>& mdtROTs,
						    bool& doHoleSearch ) const {

    ATH_MSG_VERBOSE(" in selectAndCalibrate, get PRDs  " << mdtPrdCols.size());
    
    // loop over MdtPrepDataCollections
    std::vector<const MdtPrepData*>::const_iterator mit = mdtPrdCols.begin();
    std::vector<const MdtPrepData*>::const_iterator mit_end = mdtPrdCols.end();
    for( ;mit!=mit_end;++mit ){
      if((*mit)->status() != 1) continue;
      // calibrate MDT
      const MdtDriftCircleOnTrack* mdt = handleMdtPrd( pars, **mit, doHoleSearch );
	
      // not selected
      if( !mdt ) continue;
	
      mdtROTs.push_back(mdt);
    }
    ATH_MSG_VERBOSE(" calibrated " << mdtROTs.size() << " prds out of " << mdtPrdCols.size() );

  }

  const MdtDriftCircleOnTrack* MuonSeededSegmentFinder::handleMdtPrd( const Trk::TrackParameters& pars, 
								      const MdtPrepData& mdtPrd,
								      bool& doHoleSearch ) const {

    // skip noise hits
    if( mdtPrd.adc() < m_adcCut ) return 0;

    // get surface of PRD
    const Identifier& id = mdtPrd.identify();
    const MuonGM::MdtReadoutElement& detEl = *mdtPrd.detectorElement();
    const Trk::StraightLineSurface& surf = detEl.surface(id);
     
    // propagate segment parameters to first measurement
    const Trk::TrackParameters* exPars = m_propagator->propagate(pars,surf,Trk::anyDirection,false,m_magFieldProperties);
    if( !exPars ) {
      ATH_MSG_DEBUG(" Propagation failed!! ");
      return 0;
    }

    // calculate position on wire + error
    double distanceToWire = exPars->parameters()[Trk::locR];
    double posAlongWire = exPars->parameters()[Trk::locZ];
    
    double errorR = exPars->covariance() ? fabs(Amg::error(*exPars->covariance(),Trk::locR)) : 500.;
    double errorZ = exPars->covariance() ? fabs(Amg::error(*exPars->covariance(),Trk::locZ)) : 300.;

    // range check
    bool isOnSurface = surf.isOnSurface(exPars->position(),true,5*errorR,5*errorZ);
    
    // get tube length
    int layer = m_idHelper->mdtIdHelper().tubeLayer(id);
    int tube  = m_idHelper->mdtIdHelper().tube(id);
    double halfTubeLength = 0.5*detEl.getActiveTubeLength(layer,tube);
    double tubeRadius     = detEl.innerTubeRadius();

    // take into account the tube width
    double roadWidthR = 5*errorR + 4*tubeRadius;
    double roadWidthZ = 5*errorZ + 100.;
    
    double driftdr = Amg::error(mdtPrd.localCovariance(),Trk::locR);
    double nSigmaFromTrack = fabs(fabs(distanceToWire) - mdtPrd.localPosition()[Trk::locR])/sqrt(errorR*errorR+driftdr*driftdr);

    if( msgLvl(MSG::VERBOSE) ) {
      std::string boundCheckStr = isOnSurface ? "  onSurface" : " outOfSurface";
      msg() << MSG::VERBOSE << "  " << m_idHelper->toString(mdtPrd.identify()) 
	    << " r " << distanceToWire << " range " << roadWidthR
	    << " z " << posAlongWire << " range " << halfTubeLength + roadWidthZ
	    << boundCheckStr;
    }

    //if( fabs(distanceToWire) > roadWidthR || fabs(posAlongWire) > halfTubeLength + roadWidthZ ){
    if(nSigmaFromTrack > m_maxSigma || fabs(posAlongWire) > halfTubeLength + roadWidthZ ){
      if( msgLvl(MSG::VERBOSE) ) msg() << " --- dropped" << endmsg;
      delete exPars;
      return 0;      
    }



    // update hole search flag, set to false if we are close to the tube edge
    if( doHoleSearch && fabs(posAlongWire) < halfTubeLength + roadWidthZ ) doHoleSearch = false;
    
    // pointer to ROT
    const MdtDriftCircleOnTrack* mdtROT = m_mdtRotCreator->createRIO_OnTrack(mdtPrd,exPars->position(), &(exPars->momentum()));
    
    // clean up pointers
    delete exPars;

    // check whether ROT is create 
    if( !mdtROT ){
      ATH_MSG_DEBUG(" failed to calibrate MdtPrepData " << m_idHelper->toString(mdtPrd.identify()));
      return 0;
    }
    

    if( msgLvl(MSG::VERBOSE) ) {
      double radius = mdtROT->localParameters()[Trk::locR];
      double error  = driftdr;
      double residual = radius - fabs(distanceToWire);
      double fullError = sqrt( errorR*errorR + error*error );
      double radialPull = residual/fullError;
      std::string hitType;
      if( fabs(radialPull) < 5 ) hitType = "onTrack"; 
      else if( fabs(radialPull) > 5 && residual > 0 )  hitType = "delta";
      else                       hitType = "outOfTime";
      msg() << " r_drift  " << radius << " res " << residual  << " pull " << radialPull << " "  << hitType << endmsg;
    }
    return mdtROT;
  }


}
