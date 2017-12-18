/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool.h"

#include "GaudiKernel/MsgStream.h"

#include "StoreGate/StoreGateSvc.h"

#include "Identifier/Identifier.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
//#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "TrkTrackSummary/TrackSummary.h"

#include "TrkExInterfaces/IExtrapolator.h"


#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
//#include "TrkGeometry/Layer.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"

#include "TrkToolInterfaces/ITrackHoleSearchTool.h"

#include "StoreGate/ReadHandle.h"

#include <vector>
#include <cassert>
#include <iostream>
#include <set>
#include <cmath>

Muon::MuonTrackSummaryHelperTool::MuonTrackSummaryHelperTool(
							     const std::string& t,
							     const std::string& n,
							     const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_muonTgTool("MuonHolesOnTrack"),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_edmHelperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
  m_slExtrapolator("Trk::Extrapolator/MuonStraightLineExtrapolator"),
  m_doHoles(false), 
  m_trackingGeometry(0),
  m_trackingGeometryName("MuonStandaloneTrackingGeometry")
{
  declareInterface<ITrackSummaryHelperTool>(this);
  declareProperty("DoHolesOnTrack",  m_doHoles);	
  declareProperty("CalculateCloseHits", m_calculateCloseHits = false );
  declareProperty("RoadWidth",           m_roadWidth = 135.,"width used to calculate hits within the road (mm)"); 
  declareProperty("Extrapolator",    m_extrapolator);
  declareProperty("HoleOnTrackTool", m_muonTgTool);	
  declareProperty("TrackingGeometryName", m_trackingGeometryName);
}

Muon::MuonTrackSummaryHelperTool::~MuonTrackSummaryHelperTool()
{
}

StatusCode Muon::MuonTrackSummaryHelperTool::initialize()
{
  bool muonOkay=true;
  StatusCode sc = detStore()->retrieve(m_rpcId);
  if (sc.isFailure())
    {
      msg (MSG::WARNING) << "Could not get RPC ID helper !" << endmsg;
      muonOkay=false;
    }

  if ( detStore()->retrieve( m_detMgr ).isFailure() ) {
    ATH_MSG_ERROR(" Cannot retrieve MuonDetDescrMgr ");
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve(m_tgcId);
  if (sc.isFailure())
    {
      msg (MSG::WARNING) << "Could not get TGC ID helper !" << endmsg;
      muonOkay=false;
    }

  sc = detStore()->retrieve(m_cscId);
  if (sc.isFailure())
    {
      msg (MSG::WARNING) << "Could not get CSC ID helper !" << endmsg;
      muonOkay=false;
    }

  sc = detStore()->retrieve(m_mdtId);
  if (sc.isFailure())
    {
      msg (MSG::WARNING) << "Could not get MDT ID helper !" << endmsg;
      muonOkay=false;
    }

  if (!muonOkay) 
    {
      msg (MSG::FATAL) << "Could not get ID Helper(s)!" << endmsg;
      return StatusCode::FAILURE;
    }

  if( m_calculateCloseHits && !m_extrapolator.empty() ){
    if (m_extrapolator.retrieve().isSuccess()){
      ATH_MSG_DEBUG("Retrieved " << m_extrapolator);
    }else{
      ATH_MSG_ERROR("Could not get " << m_extrapolator);
      return StatusCode::FAILURE;
    }
  }
  else{
    m_extrapolator.disable();
  }

  if( m_idHelperTool.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not get " << m_idHelperTool);      
    return StatusCode::FAILURE;
  }

  if( m_edmHelperTool.retrieve().isFailure() ){
    ATH_MSG_ERROR("Could not get " << m_edmHelperTool);      
    return StatusCode::FAILURE;
  }

  // if you try loading a tool when the DLL isn't there, an annoying warning message is printed.
  // this code attempts to check if the DLL exists before attempting to load the files
  //    IObjManager* objMgr;
  //    sc = service("ApplicationMgr", objMgr);
  //    if ( sc.isSuccess() ) 
  //    {
  //     if( objMgr->existsObjFactory( "MuonTGRecTools" ) ) 
  //     {    

  // eventually there will be no need for this, as the Geometry should always be there, but whilst we're
  // debugging it all I want a way to turn off the hole search if it does wrong.
  if (m_doHoles){
    sc = m_muonTgTool.retrieve();
    if (sc.isFailure()) {
      msg (MSG::FATAL) << "Could not get MuonHolesOnTrackTool :"<< endmsg;
      return StatusCode::FAILURE;
      //m_doHoles=false;
    } else {
      msg (MSG::VERBOSE) << "MuonHolesOnTrackTool "<< endmsg;
    }   
  } else {
    msg (MSG::VERBOSE) << "Hole search turned off, so MuonHolesOnTrackTool not loaded" << endmsg;        
    m_muonTgTool.disable();
  }

  ATH_CHECK(m_mdtKey.initialize());

  //      }else{ 
  //        msg << MSG::FATAL << "MuonTGRecTools library doesn't seem to be loaded." << endmsg;
  //      	return StatusCode::FAILURE;
  //      }
  //      objMgr->release();
  //    }
  msg (MSG::INFO) << "Initialise successful in  MuonTrackSummaryHelperTool (Hole search turned ";
  if (m_doHoles) 
    msg (MSG::INFO)<<"ON)"<<endmsg;
  else
    msg (MSG::INFO)<<"OFF)"<<endmsg;
  return StatusCode::SUCCESS;
}


void Muon::MuonTrackSummaryHelperTool::analyse(
					       const Trk::Track& /**trk*/, 
					       const Trk::RIO_OnTrack* rot, 
					       const Trk::TrackStateOnSurface* tsos,
					       std::vector<int>& information, 
					       std::bitset<Trk::numberOfDetectorTypes>& /**hitPattern*/ ) const
{
  using namespace Trk;
  if (tsos->type(Trk::TrackStateOnSurface::Outlier)) return; //ignore outliers

  Identifier id = rot->identify();
  ATH_MSG_DEBUG("Processing rot: "<<m_idHelperTool->toString(id));
  if(m_rpcId->is_rpc(id)){
    if( m_rpcId->measuresPhi(id) ) increment(information[numberOfRpcPhiHits]);
    else                           increment(information[numberOfRpcEtaHits]);
  }else if(m_cscId->is_csc(id)){
    if( m_cscId->measuresPhi(id) ) increment(information[numberOfCscPhiHits]);
    else  {                         
      increment(information[numberOfCscEtaHits]);
      const CscClusterOnTrack* clus = dynamic_cast<const CscClusterOnTrack*>(rot);
      if (clus && ((clus->status()==Muon::CscStatusUnspoiled) || (clus->status()==Muon::CscStatusSplitUnspoiled))) 
        increment(information[numberOfCscUnspoiltEtaHits]);
    }
  }else if(m_tgcId->is_tgc(id)){
    if( m_tgcId->isStrip(id) )     increment(information[numberOfTgcPhiHits]);
    else                           increment(information[numberOfTgcEtaHits]);
  }else if(m_mdtId->is_mdt(id)){  
    increment(information[numberOfMdtHits]);
  }else if(m_mdtId->is_mm(id) ){
    increment(information[numberOfCscUnspoiltEtaHits]);
  }else if(m_mdtId->is_stgc(id)){  
    int chType = m_idHelperTool->stgcIdHelper().channelType(id);
    if( chType == 2 )      increment(information[numberOfCscPhiHits]);
    else if( chType == 1 ) increment(information[numberOfCscEtaHits]);
    else                   increment(information[numberOfCscPhiHoles]);
  }else{
    msg (MSG::ERROR) << "Unknown muon detector type " << endmsg;
    msg (MSG::ERROR) << "Dumping TrackStateOnSurface "<<*tsos << endmsg;
  }
  return;
}

void Muon::MuonTrackSummaryHelperTool::analyse(
					       const Trk::Track& trk, 
					       const Trk::CompetingRIOsOnTrack* crot,
					       const Trk::TrackStateOnSurface* tsos,
					       std::vector<int>& information, 
					       std::bitset<Trk::numberOfDetectorTypes>& hitPattern ) const 
{
  // For competing ROTs we *only* count hits that are on different layers.
  std::set<Identifier> layIds;
  for (unsigned int i=0;i<crot->numberOfContainedROTs();i++){
    const Trk::RIO_OnTrack* rot = &crot->rioOnTrack(i);
    Identifier layId =  m_idHelperTool->layerId( rot->identify() );  
    ATH_MSG_DEBUG("ROT "<<i<<"\t LayerId="<<m_idHelperTool->toString(layId));
    std::pair< std::set<Identifier>::iterator, bool > pr = layIds.insert(layId); 
    if(pr.second == true) {
      // layer not seen before
      ATH_MSG_DEBUG("Have found hit on new layer. # of layers for this cROT currently="<<layIds.size());
      analyse(trk,rot, tsos, information, hitPattern);
    }
  }
}

void Muon::MuonTrackSummaryHelperTool::increment(int& type) const
{
  if (type<0)
    type=1; // they all start off at -1, so can't just increment
  else
    ++type;
  return;
}

void Muon::MuonTrackSummaryHelperTool::searchForHoles (
						       const Trk::Track& track,
						       std::vector<int>& information, 
						       Trk::ParticleHypothesis /**hyp*/) const 
{
  ATH_MSG_VERBOSE( "Entering searchForHoles");

  if (!m_doHoles) {
    msg (MSG::WARNING) << "SearchForHoles turned off!" << endmsg;
    return;
  }

  //    DataVector<const Trk::TrackStateOnSurface>* holes = m_muonTgTool->getHolesOnTrack(track, Trk::muon);
  const Trk::ITrackHoleSearchTool* constTool = &(*m_muonTgTool); 
  // the * triggers the de-ref of ToolHandle. Not as crazy as it looks! Ed
  Trk::ITrackHoleSearchTool* nonConstTool = const_cast<Trk::ITrackHoleSearchTool*>(constTool);
  const DataVector<const Trk::TrackStateOnSurface>* holes = nonConstTool->getHolesOnTrack(track);

  if (holes==0) {
    msg (MSG::WARNING) <<"Hole tool returns zero pointer! Aborting."<<endmsg;
    return;
  }
  // 
  if (!m_trackingGeometry) {   
    StoreGateSvc* m_detStore = 0;
    StatusCode sc = service( "DetectorStore", m_detStore );
    sc = m_detStore->retrieve(m_trackingGeometry, m_trackingGeometryName);
    if (sc.isFailure()) {
      msg (MSG::FATAL)<<"Could not find geometry "<< m_trackingGeometryName<<". Exiting."
		      << endmsg;
    } else {
      msg (MSG::INFO) << "  geometry Svc \""<<m_trackingGeometryName<<"\" booked "
		      << endmsg;
    }
  } 

  ATH_MSG_VERBOSE("Got "<<holes->size()<<" holes from tool");
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it    = holes->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = holes->end();
  for (;it!=itEnd;++it)
    {
      const Trk::TrackParameters* trackPara = (*it)->trackParameters();
      if (trackPara!=0)
	{
	  //       assert( trackPara!=0 );
	  // const Identifier& id = trackPara->associatedSurface()->associatedDetectorElementIdentifier();
	  const Trk::Layer* assocLayer = m_trackingGeometry->associatedLayer(trackPara->position());
	  int idl=0;
	  if (assocLayer) idl = assocLayer->layerType();
	  const Identifier id(idl);

	  if(m_rpcId->is_rpc(id)){
	    if( m_rpcId->measuresPhi(id) ) increment(information[Trk::numberOfRpcPhiHoles]);
	    else                           increment(information[Trk::numberOfRpcEtaHoles]);
	  }else if(m_cscId->is_csc(id)){
	    if( m_cscId->measuresPhi(id) ) increment(information[Trk::numberOfCscPhiHoles]);
	    else                           increment(information[Trk::numberOfCscEtaHoles]);
	  }else if(m_tgcId->is_tgc(id)){
	    if( m_tgcId->isStrip(id) )     increment(information[Trk::numberOfTgcPhiHoles]);
	    else                           increment(information[Trk::numberOfTgcEtaHoles]);
	  }else if(m_mdtId->is_mdt(id)){  
	    increment(information[Trk::numberOfMdtHoles]);
	  }else{
	    msg (MSG::ERROR) << "searchForHoles: Unknown muon detector type " << endmsg;
	    msg (MSG::ERROR) << "Dumping TrackStateOnSurface "<<*it << endmsg;
	  }
	}else{
	msg (MSG::WARNING) <<"No TrackParameter on TP returned from MuonMeasurementMgr::getHolesOnTrack"<<endmsg;
      }
    }

  delete holes;    
  return;
}

void Muon::MuonTrackSummaryHelperTool::addDetailedTrackSummary( const Trk::Track& track, Trk::TrackSummary& summary ) const {

  if( summary.m_muonTrackSummary ){
    ATH_MSG_DEBUG("TrackSummary already has detailed muon track summary, not adding a new one");
    return;
  }

  ATH_MSG_DEBUG("Adding detailed muon track summary");
  ATH_MSG_DEBUG(track.info());
  // loop over track and get chamber Identifiers
  const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
  if( !states || states->empty() ){
    return;
  }

  Trk::MuonTrackSummary* muonTrackSummary = new Trk::MuonTrackSummary();
  Trk::MuonTrackSummary& trackSummary = *muonTrackSummary;


  Trk::MuonTrackSummary::ChamberHitSummary* currentChamberSummary = 0;
  const Trk::TrackParameters* currentChamberPars = 0;

  // loop over TSOSs
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

  for( ; tsit!=tsit_end ; ++tsit ){

    const Trk::TrackParameters* pars = (*tsit)->trackParameters();

    if( (*tsit)->type(Trk::TrackStateOnSurface::Scatterer) ) {
      ++trackSummary.m_nscatterers;
      continue;
    }

    if( (*tsit)->type(Trk::TrackStateOnSurface::Hole) ) {
      if( !pars ){
        ATH_MSG_WARNING(" Hole state without track parameters, cannot identify hole ");
        continue;
      }	
      if( pars->associatedSurface().associatedDetectorElement() ){
        Identifier id = pars->associatedSurface().associatedDetectorElement()->identify();
        if( !id.is_valid() || !m_idHelperTool->isMuon(id) ) continue;
        Identifier chId = m_idHelperTool->chamberId(id);
        //Identifier layId = m_idHelperTool->layerId(id);
        bool isFirst =  isFirstProjection(id);
        bool isMdt = m_idHelperTool->isMdt(id);

// 	if( m_idHelperTool->isTrigger(id) ){
// 	  ATH_MSG_INFO("Found trigger hole " << m_idHelperTool->toString(id) );
// 	}

	// check whether first chamber or new chamber
        if( !currentChamberSummary || currentChamberSummary->m_chId != chId ){

	  /** go back through the chamber, nallhitsinroad - nhitsontrack = ncloseHits
	      for both projection1 and projection2
	      pass chamber summary to function calculateRoadHits(currentChamberSummary) */
	  if( m_calculateCloseHits && currentChamberSummary && currentChamberPars ) {
	    ATH_MSG_VERBOSE(" Calculating close hits (last hit a hole)");
	    calculateRoadHits(*currentChamberSummary, *currentChamberPars);
	    
	  }

	  // given that we cannot separate eta/phi holes, redo the assignment before moving to the next chamber
	  if( currentChamberSummary && !currentChamberSummary->isMdt() ){
	    updateHoleContent(*currentChamberSummary);
	  }

	  ATH_MSG_VERBOSE(" Adding new chamber (holes) " << m_idHelperTool->toString(id) << " " << *pars);
	  trackSummary.m_chamberHitSummary.push_back( Trk::MuonTrackSummary::ChamberHitSummary(chId,isMdt) );
	  currentChamberSummary = &trackSummary.m_chamberHitSummary.back();
	  currentChamberPars = pars;
	}
	Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = 
	  isFirst ? currentChamberSummary->m_first : currentChamberSummary->m_second;
	++proj.nholes;
      }
      continue;
    }


    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    if( !meas ){
      continue;
    }

    const Trk::PseudoMeasurementOnTrack* pseudo = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas);
    if( pseudo ) {
      ++trackSummary.m_npseudoMeasurements;	
      continue;
    }

    if(!pars){
      ATH_MSG_DEBUG("measurement without pars");
      continue;
    }

    Amg::Vector2D locPos;
    if( !meas->associatedSurface().globalToLocal(pars->position(),pars->position(),locPos) ){
      ATH_MSG_DEBUG(" localToGlobal failed !!!!! " );
      continue;
    }
    bool inBounds = true;

    Identifier id;
    std::set<Identifier> layIds;
    std::set<Identifier> goodLayIds; //holds mdt hits that have not been deweighted

    // check whether ROT
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
    if( rot ){
      id = rot->identify();
      if( !m_idHelperTool->isMuon(id) ) continue;

      // bound checks
      double tol1 = 100.;
      double tol2 = 2*tol1;
      if( !pseudo && m_idHelperTool->isMdt(id) ) tol1 = 5.;
      // we need a special bound check for MDTs so we cast to SL surface
      const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&meas->associatedSurface());
      if( slSurf ) {
	// perform bound check only for second coordinate
	inBounds = slSurf->bounds().insideLoc2(locPos,tol2);
      }else{
	inBounds = meas->associatedSurface().insideBounds(locPos,tol1,tol2);
      }

      Identifier layId =  m_idHelperTool->layerId( id );
      layIds.insert(layId);
      const MdtDriftCircleOnTrack* mdtdc = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
      if(mdtdc){
	MuonDriftCircleErrorStrategy errStrat=mdtdc->errorStrategy();
	if(!errStrat.creationParameter(MuonDriftCircleErrorStrategy::FixedError) && !errStrat.creationParameter(MuonDriftCircleErrorStrategy::StationError)){
	  goodLayIds.insert(layId);
	}
      }
    }else{
      const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
      if( crot ){
	if( crot->containedROTs().empty() ) continue;

	// take id of first ROT 
	id = crot->containedROTs().front()->identify();

	// count layers in competing rot
	std::vector<const Muon::MuonClusterOnTrack*>::const_iterator clit = crot->containedROTs().begin();
	std::vector<const Muon::MuonClusterOnTrack*>::const_iterator clit_end = crot->containedROTs().end();
	for( ;clit!=clit_end;++clit ){
	  // get layer Identifier and insert it into set
	  Identifier layId =  m_idHelperTool->layerId( (*clit)->identify() );
	  layIds.insert(layId);
	}
      }else{
	continue;
      }
    }

    Identifier chId = m_idHelperTool->chamberId(id);
    bool isFirst =  isFirstProjection(id);
    bool isMdt = m_idHelperTool->isMdt(id);
    ATH_MSG_VERBOSE(" Adding hit " << m_idHelperTool->toString(id));

    /** check whether first chamber or new chamber */
    if( !currentChamberSummary || currentChamberSummary->m_chId != chId ){

      if( m_calculateCloseHits && currentChamberSummary && currentChamberPars)  {
	ATH_MSG_VERBOSE(" Calculating close hits (last hit a measurement)");
	calculateRoadHits(*currentChamberSummary, *currentChamberPars);

      }

      // given that we cannot separate eta/phi holes, redo the assignment before moving to the next chamber
      if( currentChamberSummary  && !currentChamberSummary->isMdt() ){
	updateHoleContent(*currentChamberSummary);
      }


      ATH_MSG_VERBOSE(" Adding new chamber " << m_idHelperTool->toString(id) << " " << *pars);
      trackSummary.m_chamberHitSummary.push_back( Trk::MuonTrackSummary::ChamberHitSummary(chId,isMdt) );
      currentChamberSummary = &trackSummary.m_chamberHitSummary.back();
      currentChamberPars = pars;

    }

    Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = 
      isFirst ? currentChamberSummary->m_first : currentChamberSummary->m_second;

    if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {

      if( isMdt ){
	if( pars ){
	  double rDrift = fabs(meas->localParameters()[Trk::locR]);
	  double rTrack = fabs(pars->parameters()[Trk::locR]);
	  // flag delta electrons: check whether track prediction larger than drift radius, require that the track passes  the tube
	  if( rTrack > rDrift && rTrack < 14.6 ){
	    ++proj.ndeltas;
	    continue;
	  }
	}
      }
      ++proj.noutliers;

    }else{
      proj.nhits += layIds.size();
      proj.ngoodHits += goodLayIds.size();
    } 
    if(!inBounds && isMdt) proj.noutBounds++;

  } //end of for loop over Track State on Surfaces      

  /** calculate road hits for last chamber on track
      (otherwise it would have been skipped) */
  if( m_calculateCloseHits && currentChamberSummary && currentChamberPars ) {
    ATH_MSG_VERBOSE(" Calculating close hits (end of hit list)");
    calculateRoadHits(*currentChamberSummary, *currentChamberPars);
  }

  // given that we cannot separate eta/phi holes, redo the assignment before moving to the next chamber
  if( currentChamberSummary && !currentChamberSummary->isMdt() ){
    updateHoleContent(*currentChamberSummary);
  }

  summary.m_muonTrackSummary = muonTrackSummary;
}

void Muon::MuonTrackSummaryHelperTool::updateHoleContent( Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary ) const {
//   ATH_MSG_INFO("updateHoleContent " << m_idHelperTool->toString(chamberHitSummary.chamberId())
// 	       << " nphi " << chamberHitSummary.phiProjection().nhits << " holes " << chamberHitSummary.phiProjection().nholes 
// 	       << " neta " << chamberHitSummary.etaProjection().nhits << " holes " << chamberHitSummary.etaProjection().nholes );
  bool isCsc = m_idHelperTool->isCsc(chamberHitSummary.chamberId());
  int neta = isCsc ? 4 : 2;
  int nphi = isCsc ? 4 : 2;
  if( m_idHelperTool->isTgc(chamberHitSummary.chamberId()) ){

    const MuonGM::TgcReadoutElement* detEl = m_detMgr->getTgcReadoutElement(chamberHitSummary.chamberId());
    if( !detEl ){
      ATH_MSG_WARNING(" No detector element found for " << m_idHelperTool->toStringChamber(chamberHitSummary.chamberId()) );      
      return;
    }

    // get list of layers with a hole
    neta = detEl->Ngasgaps();
  }
  
  // code to recalculate the hole counts as they are not correct.
  // This is due to the fact that the identification of the layers goes via the readout element identifier 
  // so it is not possible to separate eta and phi holes
  int nMisEta = neta - chamberHitSummary.etaProjection().nhits - chamberHitSummary.etaProjection().noutliers;
  int nMisPhi = nphi - chamberHitSummary.phiProjection().nhits - chamberHitSummary.phiProjection().noutliers;
  int nholes  = chamberHitSummary.etaProjection().nholes + chamberHitSummary.phiProjection().nholes;
  if( nMisEta > 0 && nholes > 0 ){
    chamberHitSummary.m_first.nholes = nMisEta;
    nholes -= nMisEta;
  }
  if( nMisPhi > 0 && nholes > 0 ){
    chamberHitSummary.m_second.nholes = nholes;
    if( nholes != nMisPhi ) {
      ATH_MSG_WARNING("Inconsistent hole count: expected hits eta " << neta << " phi " << nphi 
                      << " hits eta " << chamberHitSummary.etaProjection().nhits + chamberHitSummary.etaProjection().noutliers 
                      << " phi " << chamberHitSummary.phiProjection().nhits + chamberHitSummary.phiProjection().noutliers 
                      << " missed eta " << nMisEta 
                      << " phi " << nMisPhi
                      << " holes eta " << chamberHitSummary.etaProjection().nholes 
                      << " phi " << chamberHitSummary.phiProjection().nholes 
                      << " recalculated phi holes " << nholes );
    }
  }
}


void Muon::MuonTrackSummaryHelperTool::calculateRoadHits(Trk::MuonTrackSummary::ChamberHitSummary& chamberHitSummary, 
							 const Trk::TrackParameters& pars ) const {
  bool isStraightLine = false;
  if ( pars.parameters().rows() < 5 ) { // no momentum parameter given
    isStraightLine = true;
  } else if ( std::abs( pars.parameters()[4] ) < 1e-8 ) { // |p| > 1e8 MeV = 100 TeV
    isStraightLine = true;
  } else {
    // Determine if TrackParameters correspond to a straight track (the ugly way)
    if ( pars.covariance() ) {
      const AmgSymMatrix(5)& covMat = *pars.covariance();
      if ( covMat.rows() < 5 ) { // no momentum available
        isStraightLine = true;
      } else {
        // if no error on momentum given, assume no momentum was measured (extrapolator fails on zero error)
        if ( std::abs( covMat(4,4) ) < 1e-20 ) isStraightLine = true;
      }
    }
  }
  const Trk::IExtrapolator* extrapolator = 0;
  if( !isStraightLine && !m_extrapolator.empty() ) {
    extrapolator = &*(m_extrapolator);
  } else if ( isStraightLine && !m_slExtrapolator.empty() ) {
    extrapolator = &*(m_slExtrapolator);
  }
  if ( !extrapolator ) return;

  ATH_MSG_DEBUG("road hits for chamber "<<m_idHelperTool->toString(chamberHitSummary.chamberId()));

  //currently treating MDTs only
  if(!chamberHitSummary.isMdt()) return;

  // loop over Mdt Prds (all hits in this chamber) and try to find prds of tubes with hits
  const Muon::MdtPrepDataCollection* mdtPrdCol = findMdtPrdCollection( chamberHitSummary.chamberId());
  if( !mdtPrdCol ) {
    ATH_MSG_DEBUG(" Retrieval of MdtPrepDataCollection failed!! ");
    return;
  }

  if ( isStraightLine ) {
    ATH_MSG_VERBOSE("Doing straight line extrapolation to get hits in road");
  } else {
    ATH_MSG_VERBOSE("Doing curved track extrapolation to get hits in road");
  }

  std::set<Identifier> addedIds;

  Muon::MdtPrepDataCollection::const_iterator pit = mdtPrdCol->begin();
  Muon::MdtPrepDataCollection::const_iterator pit_end = mdtPrdCol->end();
  for( ;pit!=pit_end; ++pit ){

    const Muon::MdtPrepData& mdtPrd = **pit; //hit
    const Identifier& id = mdtPrd.identify();

    bool isFirst =  isFirstProjection(id);
    Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = isFirst ? chamberHitSummary.m_first : chamberHitSummary.m_second;

    const Trk::Surface& surf = mdtPrd.detectorElement()->surface(id);

    const Trk::TrackParameters* exPars = 0;
    if( pars.associatedSurface() == surf ){
      exPars = &pars;
    }else{
      exPars = extrapolator->extrapolateDirectly(pars,surf,Trk::anyDirection,false,Trk::muon);
      if( !exPars ) {
        if ( isStraightLine ) {
          ATH_MSG_WARNING(" Straight line propagation to prd " << m_idHelperTool->toString(id) << " failed");
        } else {
          ATH_MSG_WARNING(" Curved track propagation to prd " << m_idHelperTool->toString(id) << " failed");
        }
        continue;
      }
    }

    //use exPars to get distance to wire
    double distance = exPars->parameters()[Trk::locR];

    //sometimes there is more than one hit in a tube,
    //which means there are two hits where the distance is the same but the tdc is different
    if(addedIds.count(id)){
      ATH_MSG_DEBUG(" same tube hit, not adding to close hits in road" );
    }else {
      //add all hits within the road width (defined in job options)
      if(fabs(distance) < m_roadWidth){
        ATH_MSG_VERBOSE( "Hit ID  within road: " << m_idHelperTool->toString(id) << " distance " << distance << " < " << m_roadWidth );
        ++proj.ncloseHits;
        addedIds.insert(id);
      } else {
        ATH_MSG_VERBOSE( "Hit ID outside road: " << m_idHelperTool->toString(id) << " distance " << distance << " >= " << m_roadWidth );
      }
    }


    /*    //add all hits within the road width (defined in job options)
	  if(fabs(distance) < m_roadWidth) ++proj.ncloseHits;
    */

    // to avoid double deleting when track is deleted, only delete
    //exPars when it's not the TrackParameters which was passed (pars)
    if(exPars != &pars) delete exPars;

  }

  //subtract the hits on the track in both projections:
  chamberHitSummary.m_first.ncloseHits -= chamberHitSummary.m_first.nhits;
  chamberHitSummary.m_second.ncloseHits -= chamberHitSummary.m_second.nhits;    

  if (chamberHitSummary.m_first.ncloseHits < 0) {
    ATH_MSG_WARNING("Number of hits in road < 0 in first projection: "
		    << chamberHitSummary.m_first.ncloseHits 
		    << ", setting = 0. (nhits in first projection = " 
		    << chamberHitSummary.m_first.ncloseHits << ")" );
    chamberHitSummary.m_first.ncloseHits = 0;
  }

  if (chamberHitSummary.m_second.ncloseHits < 0) {
    ATH_MSG_WARNING("Number of hits in road < 0 in second projection: "
		    << chamberHitSummary.m_second.ncloseHits
		    << ", setting = 0. (nhits in second projection = "
		    << chamberHitSummary.m_second.ncloseHits << ")" );
    chamberHitSummary.m_second.ncloseHits = 0;
  }
}

bool Muon::MuonTrackSummaryHelperTool::isFirstProjection( const Identifier& id ) const {
  if( !m_idHelperTool->isMdt(id) ) {
    return !m_idHelperTool->measuresPhi(id);
  }
  return m_idHelperTool->mdtIdHelper().multilayer(id) == 1;
}

const Muon::MdtPrepDataCollection* Muon::MuonTrackSummaryHelperTool::findMdtPrdCollection( const Identifier& chId ) const {

  SG::ReadHandle<Muon::MdtPrepDataContainer> mdtPrdContainer(m_mdtKey);

  if(!mdtPrdContainer.isValid()){
    ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_mdtKey);
    return 0;
  }

  if(!mdtPrdContainer.isPresent()){
    ATH_MSG_DEBUG("No MDT PRD container available");
    return 0;
  }

  IdentifierHash hash_id;
  m_idHelperTool->mdtIdHelper().get_module_hash(chId,hash_id );

  Muon::MdtPrepDataContainer::const_iterator colIt = mdtPrdContainer->indexFind(hash_id);
  if( colIt == mdtPrdContainer->end() ){
    ATH_MSG_DEBUG(" MdtPrepDataCollection for:   " << m_idHelperTool->toStringChamber(chId) 
		  << "  not found in container ");
    return 0;
  }
  return *colIt;
}
