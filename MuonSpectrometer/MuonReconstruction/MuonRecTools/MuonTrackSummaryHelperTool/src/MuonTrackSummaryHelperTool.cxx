/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonTrackSummaryHelperTool.h"

#include "TrkDetElementBase/TrkDetElementBase.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkCompetingRIOsOnTrack/CompetingRIOsOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonDriftCircleErrorStrategy.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkSurfaces/Surface.h"
#include "StoreGate/ReadHandle.h"

#include <cassert>
#include <set>
#include <cmath>

Muon::MuonTrackSummaryHelperTool::MuonTrackSummaryHelperTool(const std::string& t, const std::string& n, const IInterface* p) :
    base_class(t,n,p)
{
  declareInterface<ITrackSummaryHelperTool>(this);
}

StatusCode Muon::MuonTrackSummaryHelperTool::initialize()
{
  ATH_CHECK(m_DetectorManagerKey.initialize());
  if(m_calculateCloseHits && !m_extrapolator.empty()) {
    ATH_CHECK(m_extrapolator.retrieve());
  }
  else{
    m_extrapolator.disable();
  }
  ATH_CHECK(m_idHelperSvc.retrieve());
  ATH_CHECK(m_mdtKey.initialize());
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
  ATH_MSG_DEBUG("Processing rot: "<<m_idHelperSvc->toString(id));
  if(m_idHelperSvc->isRpc(id)){
    if( m_idHelperSvc->rpcIdHelper().measuresPhi(id) ) increment(information[numberOfRpcPhiHits]);
    else                           increment(information[numberOfRpcEtaHits]);
  }else if(m_idHelperSvc->isCsc(id)) {
    if( m_idHelperSvc->cscIdHelper().measuresPhi(id) ) increment(information[numberOfCscPhiHits]);
    else  {                         
      increment(information[numberOfCscEtaHits]);
      const CscClusterOnTrack* clus = dynamic_cast<const CscClusterOnTrack*>(rot);
      if (clus && ((clus->status()==Muon::CscStatusUnspoiled) || (clus->status()==Muon::CscStatusSplitUnspoiled))) 
        increment(information[numberOfCscUnspoiltEtaHits]);
    }
  }else if(m_idHelperSvc->isTgc(id)){
    if( m_idHelperSvc->tgcIdHelper().isStrip(id) )     increment(information[numberOfTgcPhiHits]);
    else                           increment(information[numberOfTgcEtaHits]);
  }else if(m_idHelperSvc->isMdt(id)){  
    increment(information[numberOfMdtHits]);
  }else if(m_idHelperSvc->issTgc(id) ){
    if( m_idHelperSvc->stgcIdHelper().measuresPhi(id) )    increment(information[numberOfStgcPhiHits]);
    // we do not discriminate between pads or wires
    else                               increment(information[numberOfStgcEtaHits]);
  }else if(m_idHelperSvc->isMM(id)){  
    increment(information[numberOfMmHits]); 
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
    Identifier layId =  m_idHelperSvc->layerId( rot->identify() );  
    ATH_MSG_DEBUG("ROT "<<i<<"\t LayerId="<<m_idHelperSvc->toString(layId));
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
						       const Trk::Track& /**track*/,
						       std::vector<int>& /**information*/, 
						       Trk::ParticleHypothesis /**hyp*/) const 
{
  ATH_MSG_WARNING( "searchForHoles is not implemented in MuonTrackSummaryHelperTool");
}

void Muon::MuonTrackSummaryHelperTool::addDetailedTrackSummary( const Trk::Track& track, Trk::TrackSummary& summary ) const {

  if( summary.m_muonTrackSummary ){
    ATH_MSG_DEBUG("TrackSummary already has detailed muon track summary, not adding a new one");
    return;
  }

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle}; 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
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
        bool issTgc = m_idHelperSvc->issTgc(id);
        if(issTgc) {
	  // get the identifier for phi or eta holes 
          Identifier idh = pars->associatedSurface().associatedDetectorElementIdentifier();
          if(idh.is_valid()) {
            id = idh;
          }
        }
        if( !id.is_valid() || !m_idHelperSvc->isMuon(id) ) continue;
        Identifier chId = m_idHelperSvc->chamberId(id);
	// for is summary sTGC split STGC1 and STGC2 
        if(issTgc) chId = m_idHelperSvc->detElId(id);
        bool isFirst =  isFirstProjection(id);
        bool isMdt = m_idHelperSvc->isMdt(id);

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

	  ATH_MSG_VERBOSE(" Adding new chamber (holes) " << m_idHelperSvc->toString(id) << " " << *pars);
	  trackSummary.m_chamberHitSummary.push_back( Trk::MuonTrackSummary::ChamberHitSummary(chId,isMdt) );
	  currentChamberSummary = &trackSummary.m_chamberHitSummary.back();
	  currentChamberPars = pars;
	}
        if(!issTgc) { 
	  Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = 
	    isFirst ? currentChamberSummary->m_first : currentChamberSummary->m_second;
 	    ++proj.nholes;
        } else {
// sTgc holes keep track of phi and eta
          if( m_idHelperSvc->measuresPhi(id) ) {
            ATH_MSG_VERBOSE(" counting sTGC phi hole ");
            Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = currentChamberSummary->m_second; 
             ++proj.nholes;
          } else {
            ATH_MSG_VERBOSE(" counting sTGC eta hole ");
            Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = currentChamberSummary->m_first;
            ++proj.nholes;
          }
        }
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
      if( !m_idHelperSvc->isMuon(id) ) continue;

      // bound checks
      double tol1 = 100.;
      double tol2 = 2*tol1;
      if( !pseudo && m_idHelperSvc->isMdt(id) ) tol1 = 5.;
      // we need a special bound check for MDTs so we cast to SL surface
      const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>(&meas->associatedSurface());
      if( slSurf ) {
        // perform bound check only for second coordinate
        inBounds = slSurf->bounds().insideLoc2(locPos,tol2);
      }else{
        inBounds = meas->associatedSurface().insideBounds(locPos,tol1,tol2);
      }

      Identifier layId =  m_idHelperSvc->layerId( id );
      layIds.insert(layId);
      const MdtDriftCircleOnTrack* mdtdc = dynamic_cast<const MdtDriftCircleOnTrack*>(rot);
      if(mdtdc){
        MuonDriftCircleErrorStrategy errStrat=mdtdc->errorStrategy();
        if(!errStrat.creationParameter(MuonDriftCircleErrorStrategy::FixedError) && !errStrat.creationParameter(MuonDriftCircleErrorStrategy::StationError)){
          goodLayIds.insert(layId);
        }
      }
      else if(m_idHelperSvc->isCsc(id)){
        const Muon::CscClusterOnTrack* cscClus = dynamic_cast<const Muon::CscClusterOnTrack*>(rot);
        if(cscClus->status()==0 || cscClus->status()==10) goodLayIds.insert(layId);
      }
      else if(m_idHelperSvc->isMM(id)) {
        // MM quality requirements to be inserted here if needed
        goodLayIds.insert(layId);
      }
      else if(m_idHelperSvc->issTgc(id)) {
        // sTGC quality requirements to be inserted here if needed
        goodLayIds.insert(layId);
      }
    }else{
      const Muon::CompetingMuonClustersOnTrack* crot = dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>(meas);
      if( crot ){
        if( crot->containedROTs().empty() ) continue;

        // take id of first ROT 
        id = crot->containedROTs().front()->identify();

        // count layers in competing rot
        std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cl_it = crot->containedROTs().begin();
        std::vector<const Muon::MuonClusterOnTrack*>::const_iterator cl_it_end = crot->containedROTs().end();
        for( ;cl_it!=cl_it_end;++cl_it ){
          // get layer Identifier and insert it into set
          Identifier layId =  m_idHelperSvc->layerId( (*cl_it)->identify() );
          layIds.insert(layId);
          if(m_idHelperSvc->isCsc(id)){
            const Muon::CscClusterOnTrack* cscClus = dynamic_cast<const Muon::CscClusterOnTrack*>(rot);
            if(cscClus->status()==0 || cscClus->status()==10) goodLayIds.insert(layId);
          }
        }
      }else{
        continue;
      }
    }

    Identifier chId = m_idHelperSvc->chamberId(id);
    // for is summary sTGC split STGC1 and STGC2 
    bool issTgc = m_idHelperSvc->issTgc(id);
    if(issTgc) chId = m_idHelperSvc->detElId(id);
    bool isFirst =  isFirstProjection(id);
    bool isMdt = m_idHelperSvc->isMdt(id);
    ATH_MSG_VERBOSE(" Adding hit " << m_idHelperSvc->toString(id));

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


      ATH_MSG_VERBOSE(" Adding new chamber " << m_idHelperSvc->toString(id) << " " << *pars);
      trackSummary.m_chamberHitSummary.push_back( Trk::MuonTrackSummary::ChamberHitSummary(chId,isMdt) );
      currentChamberSummary = &trackSummary.m_chamberHitSummary.back();
      currentChamberPars = pars;

    }

    Trk::MuonTrackSummary::ChamberHitSummary::Projection& proj = 
      isFirst ? currentChamberSummary->m_first : currentChamberSummary->m_second;

    if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) {

      // MDTs: count outlier as delta electron if rDrift < rTrack < innerTubeRadius
      if( isMdt && pars ) {
        double rDrift = std::abs(meas->localParameters()[Trk::locR]);
        double rTrack = std::abs(pars->parameters()[Trk::locR]);
        double innerRadius = MuonDetMgr->getMdtReadoutElement(id)->innerTubeRadius();
        if( rTrack > rDrift && rTrack < innerRadius ) {
          ++proj.ndeltas;
          continue;
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
  if( m_idHelperSvc->issTgc(chamberHitSummary.chamberId()) ){
    ATH_MSG_DEBUG(" holes eta " << chamberHitSummary.etaProjection().nholes
      << " phi " << chamberHitSummary.phiProjection().nholes );
  }

  if( m_idHelperSvc->issTgc(chamberHitSummary.chamberId())
      || m_idHelperSvc->isMM(chamberHitSummary.chamberId()) ){
    return;
  }

  SG::ReadCondHandle<MuonGM::MuonDetectorManager> DetectorManagerHandle{m_DetectorManagerKey};
  const MuonGM::MuonDetectorManager* MuonDetMgr{*DetectorManagerHandle}; 
  if(MuonDetMgr==nullptr){
    ATH_MSG_ERROR("Null pointer to the read MuonDetectorManager conditions object");
    return; 
  } 

  bool isCsc = m_idHelperSvc->isCsc(chamberHitSummary.chamberId());
  int neta = isCsc ? 4 : 2;
  int nphi = isCsc ? 4 : 2;
  if( m_idHelperSvc->isTgc(chamberHitSummary.chamberId()) ){

    const MuonGM::TgcReadoutElement* detEl = MuonDetMgr->getTgcReadoutElement(chamberHitSummary.chamberId());
    if( !detEl ){
      ATH_MSG_WARNING(" No detector element found for " << m_idHelperSvc->toStringChamber(chamberHitSummary.chamberId()) );      
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

  ATH_MSG_DEBUG("road hits for chamber "<<m_idHelperSvc->toString(chamberHitSummary.chamberId()));

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
          ATH_MSG_WARNING(" Straight line propagation to prd " << m_idHelperSvc->toString(id) << " failed");
        } else {
          ATH_MSG_WARNING(" Curved track propagation to prd " << m_idHelperSvc->toString(id) << " failed");
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
      if(std::abs(distance) < m_roadWidth){
        ATH_MSG_VERBOSE( "Hit ID  within road: " << m_idHelperSvc->toString(id) << " distance " << distance << " < " << m_roadWidth );
        ++proj.ncloseHits;
        addedIds.insert(id);
      } else {
        ATH_MSG_VERBOSE( "Hit ID outside road: " << m_idHelperSvc->toString(id) << " distance " << distance << " >= " << m_roadWidth );
      }
    }
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
  if( !m_idHelperSvc->isMdt(id) ) {
    return !m_idHelperSvc->measuresPhi(id);
  }
  return m_idHelperSvc->mdtIdHelper().multilayer(id) == 1;
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
  m_idHelperSvc->mdtIdHelper().get_module_hash(chId,hash_id );

  auto coll = mdtPrdContainer->indexFindPtr(hash_id);
  if( coll == nullptr ){
    ATH_MSG_DEBUG(" MdtPrepDataCollection for:   " << m_idHelperSvc->toStringChamber(chId) 
		  << "  not found in container ");
    return 0;
  }
  return coll;
}
