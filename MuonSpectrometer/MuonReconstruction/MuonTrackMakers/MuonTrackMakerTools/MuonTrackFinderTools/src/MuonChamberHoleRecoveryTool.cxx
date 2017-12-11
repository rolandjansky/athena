/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonChamberHoleRecoveryTool.h"

#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

#include "GaudiKernel/MsgStream.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"
#include "MuonStationIntersectSvc/MuonStationIntersect.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonPrepRawData/MuonCluster.h"

#include "MuonSegment/MuonSegment.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkParameters/TrackParameters.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "GeoPrimitives/GeoPrimitivesToStringConverter.h"

#include "StoreGate/StoreGateSvc.h"

#include <map>

namespace Muon {

  MuonChamberHoleRecoveryTool::MuonChamberHoleRecoveryTool(const std::string& ty,const std::string& na,const IInterface* pa)
    : AthAlgTool(ty,na,pa),
      m_intersectSvc("MuonStationIntersectSvc",name()),
      m_extrapolator("Trk::Extrapolator/MuonExtrapolator"),
      m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
      m_tubeRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtTubeHitOnTrackCreator"),
      m_cscRotCreator("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"),
      m_clusRotCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
      m_pullCalculator("Trk::ResidualPullCalculator/ResidualPullCalculator"),
      m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"), 
      m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
      m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
  {
    declareInterface<IMuonHoleRecoveryTool>(this);
    declareInterface<MuonChamberHoleRecoveryTool>(this);



    declareProperty("MdtPrepDataContainer", m_key_mdt="MDT_DriftCircles");
    declareProperty("CscPrepDataContainer", m_key_csc="CSC_Clusters");
    declareProperty("TgcPrepDataContainer", m_key_tgc="TGC_Measurements");
    declareProperty("RpcPrepDataContainer", m_key_rpc="RPC_Measurements");
    declareProperty("MuonStationIntersectSvc", m_intersectSvc);
    declareProperty("Extrapolator",            m_extrapolator);
    declareProperty("MdtRotCreator",           m_mdtRotCreator);
    declareProperty("TubeRotCreator",          m_tubeRotCreator);
    declareProperty("CscRotCreator",           m_cscRotCreator);
    declareProperty("ClusterRotCreator",          m_clusRotCreator);
    declareProperty("PullCalculator",          m_pullCalculator);
    declareProperty("IdHelper",                m_idHelperTool);
    declareProperty("EDMHelper",               m_helperTool);
    declareProperty("EDMPrinter",              m_printer);

    declareProperty("AddMeasurements",       m_addMeasurements = true);
    declareProperty("AssociationPullCutEta", m_associationPullCutEta = 3.  );
    declareProperty("AssociationPullCutPhi", m_associationPullCutPhi = 10. );
    declareProperty("DetectBadSorting", m_detectBadSort = false );
    declareProperty("AdcCut", m_adcCut = 50 );
  }


  MuonChamberHoleRecoveryTool::~MuonChamberHoleRecoveryTool(){}


  StatusCode MuonChamberHoleRecoveryTool::initialize()
  {

    ATH_CHECK( detStore()->retrieve( m_detMgr ) );
    ATH_CHECK( m_helperTool.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_mdtRotCreator.retrieve() );

    if( !m_cscRotCreator.empty() ) {
      ATH_CHECK( m_cscRotCreator.retrieve() );
    }else{
      ATH_MSG_INFO("CSC rot creation disabled" );
    }

    ATH_CHECK( m_clusRotCreator.retrieve() );
    ATH_CHECK( m_idHelperTool.retrieve() );
    ATH_CHECK( m_intersectSvc.retrieve() );

    ATH_CHECK(m_key_mdt.initialize());
    ATH_CHECK(m_key_csc.initialize());
    ATH_CHECK(m_key_tgc.initialize());
    ATH_CHECK(m_key_rpc.initialize());
    
    return StatusCode::SUCCESS;
  }
  StatusCode MuonChamberHoleRecoveryTool::finalize()
  {
    return StatusCode::SUCCESS;
  }
  
  Trk::Track* MuonChamberHoleRecoveryTool::recover( const Trk::Track& track ) const {
    
    m_chamberLayersOnTrack.clear();
    m_checkForBadSort = true;
    
    // call actual recovery routine
    Trk::Track* newTrack = recoverTrack(track);
    
    m_chamberLayersOnTrack.clear();
    m_checkForBadSort = false;
    
    // return result
    return newTrack;
  }

  Trk::Track* MuonChamberHoleRecoveryTool::recoverTrack( const Trk::Track& track ) const {
    
    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* trkstates = track.trackStateOnSurfaces();
    if( !trkstates ){
      ATH_MSG_DEBUG(" track without states, discarding track " );
      return 0;
    }
    if( msgLvl(MSG::DEBUG) ) {
      msg(MSG::DEBUG) << " performing hole search track " << std::endl 
		      << m_printer->print(track );
      if( msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << std::endl << m_printer->printMeasurements( track );
      }
      msg(MSG::DEBUG) << endmsg;
    }
    // vector to store states, the boolean indicated whether the state was create in this routine (true) or belongs to the track (false)
    // If any new state is created, all states will be cloned and a new track will beformed from them.
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    newStates.reserve(trkstates->size()+5);

    std::vector<const Trk::TrackStateOnSurface*> states;
    states.reserve(trkstates->size());
    states.insert(states.end(),trkstates->begin(),trkstates->end());

    std::set<MuonStationIndex::StIndex> stations;
    Identifier currentMdtChId;
    unsigned int nholes = 0;
    // loop over TSOSs
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit = states.begin();
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end = states.end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // drop existing holes on track
      if( (*tsit)->type(Trk::TrackStateOnSurface::Hole) ) {
	ATH_MSG_VERBOSE(" new TSOS is holes state, dropping " );
	++nholes;
	continue;
      }

      // check whether state is a measurement
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) {
	ATH_MSG_VERBOSE(" new TSOS without parameters, copying " );
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	ATH_MSG_VERBOSE(" new TSOS without measurement, copying " );
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      Identifier id = m_helperTool->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelperTool->mdtIdHelper().is_muon(id) ){
	if( msgLvl(MSG::VERBOSE)){
	  if(!id.is_valid() ) ATH_MSG_VERBOSE(" new TSOS without valid identifier, copying " );
	  else if( !m_idHelperTool->mdtIdHelper().is_muon(id) ) ATH_MSG_VERBOSE(" new TSOS with none muon identifier, copying " );
	}
	newStates.push_back( std::make_pair(false,*tsit) );	
	continue;
      }

      if( m_idHelperTool->isMdt(id) ) {
	// get station index
	Identifier chId = m_idHelperTool->chamberId(id);
	MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(chId);
	stations.insert(stIndex);

	// MDT case: Run hole search in chamber.
	tsit = insertMdtsWithHoleSearch( tsit, tsit_end, newStates );
	continue;

      }else if( m_idHelperTool->isTrigger(id) || m_idHelperTool->isCsc(id) || m_idHelperTool->isMM(id) || m_idHelperTool->issTgc(id) ) {
 
	// TGC case: Look for missing layers in chamber, add them to track
	tsit = insertClustersWithHoleSearch( tsit, tsit_end, newStates );
	continue;

      }else{
	ATH_MSG_WARNING(" unknown Identifier " );
      }

    }

    if( newStates.size() + nholes < states.size() ) 
      ATH_MSG_DEBUG("Invalid number of states: collected " << newStates.size()
		      << " holes " << nholes << " original " << states.size() );

    ATH_MSG_DEBUG(" track has stations: " << stations.size() << "   original states " <<  states.size()
		  << " new states " << newStates.size() );
    // states were added, create a new track
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( newStates.size() );
      
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit = newStates.begin();
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >::iterator nit_end = newStates.end();
    for( ;nit!=nit_end;++nit ){
      // add states. If nit->first is true we have a new state. If it is false the state is from the old track and has to be cloned
      trackStateOnSurfaces->push_back( nit->first ? nit->second : nit->second->clone() );
      
    }
    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );
    
    return newTrack;      
  }

std::vector<const Trk::TrackStateOnSurface*>::const_iterator 
MuonChamberHoleRecoveryTool::insertClustersWithHoleSearch( std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
                                                           std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
							     std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states
							     ) const {
    // iterator should point to a valid element
    if( tsit == tsit_end ) {
      ATH_MSG_WARNING(" iterator pointing to end of vector, this should no happen " );
      return --tsit;
    }

    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if( !meas  || !pars ){
      if( tsit+1 == tsit_end ) --tsit;
      return tsit;
    }

    // loop over states until we reached the last tgc hit in this detector element
    // keep trackof the identifiers and the states
    std::set<Identifier> layIds;
    Identifier* detectorElId = 0; // pointer to detector element ID, created when finding the first TGC
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    for( ;tsit!=tsit_end;++tsit ){
      
      if( !*tsit ) continue;

      // skip existing holes as they will be recalculated
      if( (*tsit)->type(Trk::TrackStateOnSurface::Hole) ) continue;
      
      // check whether state is a measurement, keep if not
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // get identifier, keep state if it has no identifier.
      Identifier id = m_helperTool->getIdentifier(*meas);
      if( !id.is_valid() ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }
      
      // get chamber Id
      Identifier detElId =  m_idHelperTool->detElId(id);
      if( !detectorElId ) detectorElId = new Identifier( detElId );

      // check whether we are still in the same chamber, stop loop if not
      if( detElId != *detectorElId ) break;

      // sanity check, this SHOULD be a RPC, TGC or CSC measurement
      if( !( m_idHelperTool->isTrigger(id) || m_idHelperTool->isCsc(id) || m_idHelperTool->isMM(id) || m_idHelperTool->issTgc(id) ) ){
	newStates.push_back( std::make_pair(false,*tsit) );
	ATH_MSG_WARNING(" Identifier should be TGC/RPC/CSC/MM/STGC: " << m_idHelperTool->toString(id) );
	continue;
      }

       
      // split competing ROTs into constituents
      const CompetingMuonClustersOnTrack* comp = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
      if( comp ) {
	
	std::vector<const MuonClusterOnTrack*>::const_iterator clit = comp->containedROTs().begin();
	std::vector<const MuonClusterOnTrack*>::const_iterator clit_end = comp->containedROTs().end();
	for( ;clit!=clit_end;++clit ){
	  // get layer Identifier and insert it into set
	  Identifier layId =  m_idHelperTool->layerId( (*clit)->identify() );
	  layIds.insert(layId);
	}
      }else{
	// get layer Identifier and insert it into set
	Identifier layId =  m_idHelperTool->layerId(id);
	layIds.insert(layId);
      }
      if( msgLvl(MSG::VERBOSE) ) {
	msg(MSG::VERBOSE) << " new hit " << m_idHelperTool->toString(id);
	if( comp ) msg(MSG::VERBOSE) << " competing, layers: " << layIds.size();
	msg(MSG::VERBOSE) << endmsg;
      }
      newStates.push_back( std::make_pair(false,*tsit) );
    }

    // create holes
    if( detectorElId ){
      createHoleTSOSsForClusterChamber( *detectorElId,*pars,layIds,newStates );
      delete detectorElId;
    }else{
      ATH_MSG_WARNING(" no chamber identifier set, this should not happen " );
    }

    // sort all states in this chamber
    std::stable_sort(newStates.begin(),newStates.end(),SortTSOSByDistanceToPars(pars) );

    // insert the states into 
    states.insert( states.end(),newStates.begin(),newStates.end() );

    // iterator should point to the last TGC in this chamber
    return --tsit;
  }


std::vector<const Trk::TrackStateOnSurface*>::const_iterator 
MuonChamberHoleRecoveryTool::insertMdtsWithHoleSearch( std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit,
                                                       std::vector<const Trk::TrackStateOnSurface*>::const_iterator tsit_end,
							 std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states
							 ) const {
    // iterator should point to a valid element
    if( tsit == tsit_end ) {
      ATH_MSG_WARNING(" iterator pointing to end of vector, this should no happen " );
      return --tsit;
    }

    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if( !meas  || !pars ){
      if( tsit+1 == tsit_end ) --tsit;
      return tsit;
    }

    // this should be a MDT
    const MdtDriftCircleOnTrack* mdtFirst = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
    if( !mdtFirst ){
      ATH_MSG_WARNING("Bad hit: not a MDT " << m_idHelperTool->toString(m_helperTool->getIdentifier(*meas)) );
      if( tsit+1 == tsit_end ) --tsit;
      return tsit;
    }

    // get chamber identifier
    MuonStationIndex::ChIndex currentChIndex = m_idHelperTool->chamberIndex(mdtFirst->identify());
    Identifier chId = m_idHelperTool->chamberId(mdtFirst->identify());

    // to correctly handle low pt tracks average the direction of the track in the chamber
    const Trk::TrackParameters* parsLast = 0;

    // loop over states until we reached the last mdt hit in this chamber
    // keep trackof the identifiers and the states
    std::set<Identifier> ids;
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    for( ;tsit!=tsit_end;++tsit ){
      
      if( !*tsit ) continue;
      
      // skip existing holes as they will be recalculated
      if( (*tsit)->type(Trk::TrackStateOnSurface::Hole) ) continue;

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // don't break on pseudo measurements
      const Trk::PseudoMeasurementOnTrack* psmeas = dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas);
      if( psmeas ){
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      // only use MDTs
      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
      if( !mdt ) break;

      const Identifier& id = mdt->identify();

      // check whether we are still in the same chamber layer
      if( currentChIndex != m_idHelperTool->chamberIndex(id) ) break;
      
      ids.insert(id); // insert identifier
      newStates.push_back( std::make_pair(false,*tsit) );
      
      // get pointer to last parameters
      parsLast = (*tsit)->trackParameters();
    }

    // check if chamber index is already processed
    bool doHoleSearch = true;
    if( m_checkForBadSort ){
      if( m_chamberLayersOnTrack.count(currentChIndex) ){
	if( m_detectBadSort ) ATH_MSG_WARNING("Detected badly sorted track, not adding holes in current chamber");
	else                  ATH_MSG_DEBUG("Detected badly sorted track, not adding holes in current chamber");
	doHoleSearch = false;
      }else{
	m_chamberLayersOnTrack.insert(currentChIndex);
      }
    }

    if( doHoleSearch ){

      // ensure that we are not passing in the same parameters
      if( parsLast == pars ) parsLast = 0;

      // create holes
      createHoleTSOSsForMdtChamber( chId,*pars, parsLast,ids,newStates );
      
      ATH_MSG_DEBUG(" Chamber " << m_idHelperTool->toStringChamber(chId) 
		    << " has hits " << ids.size() << " new states " << newStates.size() - ids.size() );

      // sort all states in this chamber
      std::stable_sort(newStates.begin(),newStates.end(),SortTSOSByDistanceToPars(pars) );
    }

    // insert the states into 
    states.insert( states.end(),newStates.begin(),newStates.end() );

    // iterator should point to the last MDT in this chamber
    return --tsit;
  }

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForClusterChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
								      std::set<Identifier>& layIds, 
								      std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {

    ATH_MSG_VERBOSE(" performing holes search in chamber " << m_idHelperTool->toString(detElId) ); 

    if( m_idHelperTool->isCsc(detElId) )      createHoleTSOSsForCscChamber(detElId,pars,layIds,states);
    else if( m_idHelperTool->isTgc(detElId) ) createHoleTSOSsForTgcChamber(detElId,pars,layIds,states);
    else if( m_idHelperTool->isRpc(detElId) ) createHoleTSOSsForRpcChamber(detElId,pars,layIds,states);
    else if( m_idHelperTool->isMM(detElId) || m_idHelperTool->issTgc(detElId) ) ATH_MSG_DEBUG(" NSW hole recovery not implemented yet");
    else ATH_MSG_WARNING(" unexpected Identifier, cannot perform holes search" ); 
  }

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForCscChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
								  std::set<Identifier>& layIds, 
								  std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {
    // get list of layers with a hole
    unsigned int nGasGaps = 4;
    LayerHoleVec holeVec = holesInClusterChamber( pars,detElId,layIds, nGasGaps );
    if( holeVec.empty() ) return;

    // create identifiers from LayerHoleVec
    std::set<Identifier> chHoles;
    LayerHoleVec::iterator it = holeVec.begin();
    LayerHoleVec::iterator it_end = holeVec.end();
    for( ;it!=it_end;++it ){
      Identifier parentId = m_idHelperTool->cscIdHelper().parentID(detElId);
      Identifier layId = m_idHelperTool->cscIdHelper().channelID(parentId,2,it->first,it->second,1);
      bool measuresPhi = m_idHelperTool->measuresPhi(layId);
      // sanity check this should always be true!!!
      if( measuresPhi != (bool)it->second ){
	ATH_MSG_WARNING(" Problem spotted creating CSC identifier, measures Phi not correctly set " ); 
	continue;
      }
      ATH_MSG_VERBOSE(" adding hole layer " << m_idHelperTool->toString(layId)
			     << "     layer " << it->first << " measPhi " << it->second ); 
      chHoles.insert( layId );
    }

    if( chHoles.empty() ) return;
      
    // loop over CSC Prds and try to find prds of tubes with holes
    const CscPrepDataCollection* prdCol = findCscPrdCollection( detElId );
    if( !prdCol ) {
      ATH_MSG_DEBUG(" Retrieval of CscPrepDataCollection failed!! " );
      //return;
    }

    // copy CSC prds into generic MuonCluster vector
    std::vector<const MuonCluster*> prds;
    if( prdCol ){
      prds.reserve(prdCol->size());
      prds.insert(prds.begin(),prdCol->begin(),prdCol->end());
    }
    createHoleTSOSsForClusterChamber( detElId, pars, layIds, chHoles, prds, states ); 
  }

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForTgcChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
								  std::set<Identifier>& layIds, 
								  std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {

    // get detector element
    const MuonGM::TgcReadoutElement* detEl = m_detMgr->getTgcReadoutElement(detElId);
    if( !detEl ){
      ATH_MSG_WARNING(" No detector element found for " << m_idHelperTool->toStringChamber(detElId) );      
      return;
    }

    // get list of layers with a hole
    unsigned int nGasGaps = detEl->Ngasgaps();
    LayerHoleVec holeVec = holesInClusterChamber( pars,detElId,layIds, nGasGaps );
    if( holeVec.empty() ) return;

    // create identifiers from LayerHoleVec
    std::set<Identifier> chHoles;
    LayerHoleVec::iterator it = holeVec.begin();
    LayerHoleVec::iterator it_end = holeVec.end();
    for( ;it!=it_end;++it ){
      // the second gas gap of the three layer stations does not have a phi measurement
      if( nGasGaps == 3 && it->first == 2 && it->second ) continue;

      chHoles.insert( m_idHelperTool->tgcIdHelper().channelID(detElId,it->first,it->second,1) );
    }

    if( chHoles.empty() ) return;
      
    // loop over Mdt Prds and try to find prds of tubes with holes
    const TgcPrepDataCollection* prdCol = findTgcPrdCollection( detElId );
    if( !prdCol ) {
      ATH_MSG_DEBUG(" Retrieval of TgcPrepDataCollection failed!! " );
      return;
    }

    // copy TGC prds into generic MuonCluster vector
    std::vector<const MuonCluster*> prds;
    prds.reserve(prdCol->size());
    TgcPrepDataCollection::const_iterator hit = prdCol->begin();
    TgcPrepDataCollection::const_iterator hit_end = prdCol->end();
    for( ;hit!=hit_end;++hit ){
      if( detElId == m_idHelperTool->detElId( (*hit)->identify() ) ) prds.push_back(*hit);
    }
    //prds.insert(prds.begin(),prdCol->begin(),prdCol->end());
	       
    createHoleTSOSsForClusterChamber( detElId, pars, layIds, chHoles, prds, states ); 

  }

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForRpcChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
								  std::set<Identifier>& layIds, 
								  std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {
    // get list of layers with a hole
    unsigned int nGasGaps = 2;
    LayerHoleVec holeVec = holesInClusterChamber( pars,detElId,layIds, nGasGaps );
    if( holeVec.empty() ) return;
    const MuonGM::RpcReadoutElement* detEl = m_detMgr->getRpcReadoutElement(detElId);
    if( !detEl ){
      ATH_MSG_WARNING("Could not find ReadoutElement for " << m_idHelperTool->toString(detElId) );
      return;
    }
    int dbZ   = detEl->getDoubletZ();
    int dbPhi = detEl->getDoubletPhi();

    ATH_MSG_DEBUG("Looking up RPC structure: dbZ " << dbZ << " dbPhi " << dbPhi
		  << " nphi gp " << detEl->Nphigasgaps());

    // create identifiers from LayerHoleVec
    std::set<Identifier> chHoles;
    LayerHoleVec::iterator it = holeVec.begin();
    LayerHoleVec::iterator it_end = holeVec.end();
    for( ;it!=it_end;++it ){
      Identifier clusId = m_idHelperTool->rpcIdHelper().channelID(detElId,dbZ,dbPhi,it->first,it->second,1); 
      chHoles.insert( clusId );
      
      ATH_MSG_VERBOSE("Adding hole " << m_idHelperTool->toString(clusId) );
      
    }
      
    // loop over Mdt Prds and try to find prds of tubes with holes
    const RpcPrepDataCollection* prdCol = findRpcPrdCollection( detElId );
    if( !prdCol ) {
      ATH_MSG_DEBUG(" Retrieval of RpcPrepDataCollection failed!! " );
      return;
    }

    // copy RPC prds into generic MuonCluster vector
    std::vector<const MuonCluster*> prds;
    prds.reserve(prdCol->size());
    RpcPrepDataCollection::const_iterator hit = prdCol->begin();
    RpcPrepDataCollection::const_iterator hit_end = prdCol->end();
    for( ;hit!=hit_end;++hit ){
      if( detElId == m_idHelperTool->detElId( (*hit)->identify() ) ) prds.push_back(*hit);
    }
    //prds.insert(prds.begin(),prdCol->begin(),prdCol->end());
	       
    createHoleTSOSsForClusterChamber( detElId, pars, layIds, chHoles, prds, states ); 

  }

  struct PullCluster {
    double pull;
    const Trk::TrackParameters* pars;
    const MuonClusterOnTrack* clus;
  };

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForClusterChamber( const Identifier& detElId, const Trk::TrackParameters& pars,
								      std::set<Identifier>& layIds, std::set<Identifier>& chHoles, 
								      const std::vector<const MuonCluster*>& prds,
								      std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {
    

    if( msgLvl(MSG::DEBUG) ) {
      msg() << " chamber " << m_idHelperTool->toString(detElId) 
	    << " PRDs in chamber  " << prds.size() << "  holes " << chHoles.size();
      if( msgLvl(MSG::VERBOSE) ) {
	if( pars.covariance() ) msg() << MSG::VERBOSE << " extrapolation error " << Amg::error(*pars.covariance(),Trk::locX);
      }
      msg() << endmsg;
    }
    std::vector<const Trk::TrackParameters*> parsToBeDeleted;
    parsToBeDeleted.reserve(prds.size()+10);

    typedef std::map<Identifier,PullCluster> ClusterLayerMap;
    ClusterLayerMap clusterLayerMap;

    const Trk::TrkDetElementBase* detEl = 0;
    if( m_idHelperTool->isTgc(detElId) )      detEl = m_detMgr->getTgcReadoutElement(detElId);
    else if( m_idHelperTool->isRpc(detElId) ) detEl = m_detMgr->getRpcReadoutElement(detElId);
    else if( m_idHelperTool->isCsc(detElId) ) detEl = m_detMgr->getCscReadoutElement(detElId);
    
    unsigned int nNewHits = 0;
    // loop over prds
    std::vector<const MuonCluster*>::const_iterator it = prds.begin();
    std::vector<const MuonCluster*>::const_iterator it_end = prds.end();
    for( ; it!=it_end;++it ){
      
      // get Identifier, check whether we have a hole in that layer. If not go to next cluster
      const MuonCluster& clus = **it;
      if( !detEl ) detEl = clus.detectorElement();
      const Identifier& id = clus.identify();
      Identifier layId = m_idHelperTool->layerId(id);

      // do not add hits in layers that are already on track
      if( layIds.count(layId) ){
       continue;
      }
      const Trk::Surface& surf = clus.detectorElement()->surface(id);

      const Trk::TrackParameters* exPars = 0;
      if( pars.associatedSurface() == surf ) {
	ATH_MSG_VERBOSE(" Same surface, cloning parameters " );
	exPars = &pars;
      }else{
	exPars = m_extrapolator->extrapolateDirectly(pars,surf,Trk::anyDirection,false,Trk::muon);
	if( !exPars ) {
	  ATH_MSG_WARNING(" Propagation cluster PRD failed!! ");
	  continue;
	}
	//parsToBeDeleted.push_back(exPars);
      }
      if( msgLvl(MSG::VERBOSE) ) {
	msg() << MSG::VERBOSE << "  --- " << m_idHelperTool->toString(id) << " error " << Amg::error(clus.localCovariance(),Trk::locX);
	if( exPars->covariance() ) msg() << MSG::VERBOSE << " measured parameters, error " << Amg::error(*exPars->covariance(),Trk::locX);
	msg() << endmsg;
      }

      const MuonClusterOnTrack* clusterOnTrack = 0;
      if( m_idHelperTool->isTrigger(clus.identify()) ){
	clusterOnTrack = m_clusRotCreator->createRIO_OnTrack( clus, exPars->position(), exPars->momentum().unit()  );
      }else{
	if ( m_cscRotCreator.empty() ) {
	  clusterOnTrack = 0;
	} else {
	  clusterOnTrack = m_cscRotCreator->createRIO_OnTrack(  clus, exPars->position(), exPars->momentum().unit() );
	}
      }
      if( !clusterOnTrack ) {
	if( &pars != exPars ) delete exPars;
	continue;
      }

      const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( clusterOnTrack, exPars, Trk::ResidualPull::Unbiased );
      if( !resPull ){
	if( &pars != exPars ) delete exPars;
	delete clusterOnTrack;
	continue;
      }
      if( resPull->pull().empty() ){
	if( &pars != exPars ) delete exPars;
	delete clusterOnTrack;
	delete resPull;
	continue;
      }

      double pullCut = m_idHelperTool->measuresPhi(id) ? m_associationPullCutPhi : m_associationPullCutEta;
      double pull = fabs(resPull->pull().front());
      Amg::Vector2D locExPos(exPars->parameters()[Trk::locX],exPars->parameters()[Trk::locY]);
      bool inbounds = surf.insideBounds(locExPos,10.,10.);
      
      if( msgLvl(MSG::VERBOSE) ) {
	std::string boundStr = inbounds ? " inside bounds " : " outside bounds ";
	double residual = resPull->residual().front();
	ATH_MSG_VERBOSE(" found prd: " << m_idHelperTool->toString(id)
			<< " res " << residual << " pull " << pull << boundStr);
      }
      delete resPull;

      // check whether hit within cut and in bounds
      if( pull < pullCut && inbounds ){
	
	// check whether there are other hits in the same layer with smaller residual, if not add
	ClusterLayerMap::iterator clit = clusterLayerMap.find(layId);
	if( clit != clusterLayerMap.end() ){
	  if( pull < clit->second.pull ) {
	    clit->second.pull = pull;
	    if( &pars != clit->second.pars ) delete clit->second.pars;
	    delete clit->second.clus;
	    clit->second.pars = exPars;
	    clit->second.clus = clusterOnTrack;
	  }else{
	    if( &pars != exPars ) delete exPars;
	    delete clusterOnTrack;
	  }
	}else{
	  PullCluster pullClus;
	  pullClus.pull = pull;
	  pullClus.pars = exPars;
	  pullClus.clus = clusterOnTrack;
	  clusterLayerMap[layId] = pullClus;
	}
      }else{
	if( &pars != exPars ) delete exPars;
	delete clusterOnTrack;
      }
    }

    // loop over cluster layer map and add the clusters to the state vector
    ClusterLayerMap::iterator clit = clusterLayerMap.begin();
    ClusterLayerMap::iterator clit_end = clusterLayerMap.end();
    for( ;clit!=clit_end;++clit ){
 
//      bool addMeasurements = true;
      Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS( clit->second.clus, clit->second.pars != &pars ? clit->second.pars : clit->second.pars->clone() , 
								       Trk::TrackStateOnSurface::Measurement );
//								       addMeasurements ? Trk::TrackStateOnSurface::Measurement : Trk::TrackStateOnSurface::Outlier );
      states.push_back( std::make_pair(true,tsos) );
      ATH_MSG_VERBOSE(" added hit " << m_idHelperTool->toString(clit->second.clus->identify()));      
      ++nNewHits;
    }

    unsigned int nholes = 0;
    std::set<Identifier>::iterator hit = chHoles.begin();
    std::set<Identifier>::iterator hit_end = chHoles.end();
    for( ;hit!=hit_end;++hit ){
      const Identifier& id = *hit;

      if( clusterLayerMap.count(id) ) continue;
      
      // if no detEl yet try retrieving if from the detMgr
      if( !detEl ) {
	if( m_idHelperTool->isTgc(id) )      detEl = m_detMgr->getTgcReadoutElement(id);
	else if( m_idHelperTool->isRpc(id) ) detEl = m_detMgr->getRpcReadoutElement(id);
	else if( m_idHelperTool->isCsc(id) ) detEl = m_detMgr->getCscReadoutElement(id);
	if( !detEl ){
	  ATH_MSG_DEBUG("No detector element found for " << m_idHelperTool->toString(id) );
	  continue;
	}
      }
      const Trk::Surface& surf = detEl->surface(id);

      const Trk::TrackParameters* exPars = 0;
      if( pars.associatedSurface() == surf ) {
	ATH_MSG_DEBUG(" Same surface, cloning parameters " );
	exPars = &pars;
      }else{
	exPars = m_extrapolator->extrapolateDirectly(pars,surf,Trk::anyDirection,false,Trk::muon);
	if( !exPars ) {
	  ATH_MSG_DEBUG(" Propagation cluster hole failed!! ");
	  continue;
	}
	parsToBeDeleted.push_back(exPars);
      }

      bool inBounds = false;
      Amg::Vector2D locPos;;
      if( surf.globalToLocal(exPars->position(),exPars->momentum(),locPos) ){
	inBounds = surf.insideBounds(locPos,-100.,-100.);
      }
      if( msgLvl(MSG::VERBOSE) ) {
	std::string boundStr = inBounds ? " inside bounds " : " outside bounds ";
	ATH_MSG_VERBOSE(" new hole " << m_idHelperTool->toString(id) << " position " 
			<< exPars->parameters()[Trk::locR] << boundStr);
      }

      if( !inBounds ){
	continue;
      }
      
      Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createHoleTSOS(exPars->clone());
      states.push_back( std::make_pair(true,tsos) );	
      ++nholes;
      
      //break; // only add one hole per det el
    }
    
    ATH_MSG_DEBUG(" chamber " << m_idHelperTool->toStringChamber(detElId) << " added hits " << nNewHits << " found holes " << nholes  );

    // clean up parameters
    std::for_each( parsToBeDeleted.begin(), parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );

  }

  void MuonChamberHoleRecoveryTool::createHoleTSOSsForMdtChamber( const Identifier& chId, const Trk::TrackParameters& pars,
								  const Trk::TrackParameters* parsLast,
								  std::set<Identifier>& ids, 
								  std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > >& states ) const {

    Amg::Vector3D direction = pars.momentum().unit();

    // check whether we need to average the parameters
    if( parsLast ) direction = (parsLast->position() - pars.position()).unit();

    std::set<Identifier> chHoles = holesInMdtChamber( pars.position(),direction,chId,ids );
    ATH_MSG_VERBOSE(" chamber " << m_idHelperTool->toStringChamber(chId) << " has holes " << chHoles.size() );
    if( chHoles.empty() ) return;

      
    unsigned int nholes  = 0;
    unsigned int nstates = 0;

    std::vector<const Trk::TrackParameters*> parsToBeDeleted;
    parsToBeDeleted.reserve(10);

    // set to store chambers that were already handled
    std::set<Identifier> chIds;

    // loop over holes, find prd data collection belonging to hole, recover hole
    std::set<Identifier>::iterator hit = chHoles.begin();
    std::set<Identifier>::iterator hit_end = chHoles.end();
    for( ;hit!=hit_end; ++hit ){

      // only handle chamers once
      Identifier chIdentifier = m_idHelperTool->chamberId(*hit);
      if( chIds.count(chIdentifier) ) continue;
      chIds.insert(chIdentifier);
      
      // loop over Mdt Prds and try to find prds of tubes with holes
      const MdtPrepDataCollection* mdtPrdCol = findMdtPrdCollection( chIdentifier );
      if( !mdtPrdCol ) {
	ATH_MSG_DEBUG(" Retrieval of MdtPrepDataCollection failed!! " );
	break;
      }

      std::set<Identifier> associatedHoles; // holes in tubes that also have a prd

      MdtPrepDataCollection::const_iterator pit = mdtPrdCol->begin();
      MdtPrepDataCollection::const_iterator pit_end = mdtPrdCol->end();
      for( ;pit!=pit_end; ++pit ){
	const MdtPrepData& mdtPrd = **pit;
	const Identifier& id = mdtPrd.identify();
	
	// only consider holes
	if( !chHoles.count(id) ) continue;
	
	// ensure that Identifiers are not added twice
	if( associatedHoles.count(id) ) continue;

	const Trk::Surface& surf = mdtPrd.detectorElement()->surface(id);

	const Trk::TrackParameters* exPars = 0;
	if(  pars.associatedSurface() == surf ){
	  exPars = &pars;
	}else{
	  exPars = m_extrapolator->extrapolateDirectly(pars,surf,Trk::anyDirection,false,Trk::muon);
	  if( !exPars ) {
	    ATH_MSG_WARNING(" Propagation to MDT prd failed!! ");
	    continue;
	  }
	  parsToBeDeleted.push_back(exPars);
	}

	// calibrate Mdt PRD
	const MdtDriftCircleOnTrack* mdtROT = m_mdtRotCreator->createRIO_OnTrack(mdtPrd,exPars->position(),&(exPars->momentum()));
	// sanity check
	if( !mdtROT ){
	  ATH_MSG_DEBUG(" failed to calibrate MdtPrepData " << m_idHelperTool->toString(id) );
	  continue;
	}
      
      
	const Trk::StraightLineSurface* slSurf = dynamic_cast<const Trk::StraightLineSurface*>( &mdtROT->associatedSurface() );
	if( !slSurf ){
	  ATH_MSG_WARNING(" dynamic cast to StraightLineSurface failed for mdt!!! " );
	}else{
	  
	  // calculate Amg::Vector2D using surf to obtain sign
	  Amg::Vector2D locPos;
	  if( !slSurf->globalToLocal(exPars->position(),exPars->momentum(),locPos) ){
	    ATH_MSG_DEBUG(" failed to calculate drift sign " );
	  }else{
	    // calculate side
	    Trk::DriftCircleSide side = locPos[Trk::driftRadius] < 0 ? Trk::LEFT : Trk::RIGHT;
	  
	    // cast away constness 
	    MdtDriftCircleOnTrack* changeMdtROT = const_cast<MdtDriftCircleOnTrack*>(mdtROT);
	    // sanity check
//	    if( !changeMdtROT ){
//	      ATH_MSG_WARNING(" failed to cast away constness of mdtROt " << m_idHelperTool->toString(id) );
//	      continue;
//	    }
	  
	    // update sign
	    m_mdtRotCreator->updateSign( *changeMdtROT, side );
	  }
	}
      
	// check whether MDT ROT has sagged wire surface, if so redo propagation
	if( slSurf != &mdtPrd.detectorElement()->surface(id) ){
	  exPars = m_extrapolator->extrapolateDirectly(pars,*slSurf,Trk::anyDirection,false,Trk::muon);
	  if( !exPars ) {
	    ATH_MSG_WARNING( " Propagation to sagged surface failed!! ");
	    continue;
	  }
	  parsToBeDeleted.push_back(exPars);
	}
      
	if( msgLvl(MSG::VERBOSE) ) {
	  DistanceToPars distToPars(&pars);
	  double distance = distToPars(exPars->position());
	  msg(MSG::VERBOSE) << std::setprecision(4) 
			    << " radius  " << std::setw(7) << mdtROT->localParameters()[Trk::locR] 
			    << " exPos "  << std::setw(7) << exPars->parameters()[Trk::locR] << " distToPar " << distance << " ";
	}
      
	// pointer to resPull
	const Trk::ResidualPull* resPull = m_pullCalculator->residualPull( mdtROT, exPars, Trk::ResidualPull::Unbiased );
	if( !resPull ){
	  delete mdtROT;
	  continue;
	}
      
      
	double pull = resPull->pull().front();
	double radialResidual = fabs(mdtROT->localParameters()[Trk::locR]) - fabs(exPars->parameters()[Trk::locR]);
      
	unsigned int hitFlag = 1;
	if( mdtPrd.adc() < m_adcCut || mdtPrd.status() != MdtStatusDriftTime ) hitFlag = 3; // noise
	else if( fabs(pull) < m_associationPullCutEta ) hitFlag = 0;  // hit on track
	else if( radialResidual > 0. ) hitFlag = 2; // out of time

	//       else {
	// 	// replace hit by tube hit
	// 	delete mdtROT;
	// 	mdtROT = m_tubeRotCreator->createRIO_OnTrack(mdtPrd,exPars->position());
	//       }
      
	bool inBounds = false;
	// calculate Amg::Vector2D using surf to obtain sign
	Amg::Vector2D locPos;
	if( mdtROT->associatedSurface().globalToLocal(exPars->position(),exPars->momentum(),locPos) ){
	  inBounds = mdtROT->associatedSurface().insideBounds(locPos);
	}
	if( msgLvl(MSG::VERBOSE) ){
	  std::string idString   = m_idHelperTool->toString( id );
	  std::string boundCheck =  inBounds ? "inBounds" : "outBounds"; 
	  msg(MSG::VERBOSE) << m_printer->print( *resPull ) << " "  << idString << " " << boundCheck;
	  std::string hitType = " delta";
	  if( hitFlag == 0 )  hitType = " ROT";
	  else if( hitFlag == 2 ) hitType = " Out of Time";
	  else if( hitFlag == 3 ) hitType = " Noise";
	  msg(MSG::VERBOSE) << hitType << endmsg;
	}
	delete resPull;
	
	// if we get here we are sure that this is not a hole
	associatedHoles.insert(id);
	
	if( !inBounds ) {
	  delete mdtROT;
	  continue;
	}

	Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOS( mdtROT, exPars->clone(), 
									 (hitFlag != 0 || !m_addMeasurements) ? 
									 Trk::TrackStateOnSurface::Outlier : Trk::TrackStateOnSurface::Measurement );
	states.push_back( std::make_pair(true,tsos) );
	++nstates;
	if( nstates == chHoles.size() ) {
	  ATH_MSG_DEBUG(" recovered Mdts " << nstates << " all holes recovered " << nholes );
	  // clean up parameters
	  std::for_each( parsToBeDeleted.begin(), parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );
	  return;
	}
      }

      std::set<Identifier>::iterator hit = chHoles.begin();
      std::set<Identifier>::iterator hit_end = chHoles.end();
      for( ;hit!=hit_end;++hit ){
	if( associatedHoles.count(*hit) ) continue;
	// only create holes in the current chamber
	Identifier ch = m_idHelperTool->chamberId(*hit);
	if( ch != chIdentifier ) continue;

	const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(*hit);
	const Trk::Surface& surf = detEl->surface(*hit);
      
	const Trk::TrackParameters* exPars = 0;
	if(  pars.associatedSurface() == surf ){
	  exPars = &pars;
	}else{
	  exPars = m_extrapolator->extrapolateDirectly(pars,surf,Trk::anyDirection,false,Trk::muon);
	  if( !exPars ) {
	    ATH_MSG_WARNING(" Propagation to MDT hole failed!! ");
	    continue;
	  }
	  parsToBeDeleted.push_back(exPars);
	}

	bool inBounds = false;
	Amg::Vector2D locPos;
	if( surf.globalToLocal(exPars->position(),exPars->momentum(),locPos) ){
	  // perform bound check do not count holes with 100. mm of bound edge
	  inBounds = surf.bounds().insideLoc2(locPos,-100.);
	  if( inBounds ) {
	    if( fabs( locPos[Trk::locR] ) > 14.4 ) inBounds = false;
	  }
	}
	if( !inBounds ){
	  ATH_MSG_VERBOSE(" discarding hole " << m_idHelperTool->toString(*hit) << " dist wire " 
				 << exPars->parameters()[Trk::locR] << " outside bounds " );
	  continue;
	}
	ATH_MSG_VERBOSE(" new hole " << m_idHelperTool->toString(*hit) << " dist wire " 
			       << exPars->parameters()[Trk::locR] );
	Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createHoleTSOS(exPars->clone());
	states.push_back( std::make_pair(true,tsos) );	
	++nholes;
	++nstates;
      } 
    }
    ATH_MSG_DEBUG(" new states " << nstates << " number of holes " << nholes );

    // clean up parameters
    std::for_each( parsToBeDeleted.begin(), parsToBeDeleted.end(), MuonDeleteObject<const Trk::TrackParameters>() );

  }

  MuonChamberHoleRecoveryTool::LayerHoleVec 
  MuonChamberHoleRecoveryTool::holesInClusterChamber( const Trk::TrackParameters& /*pars*/, const Identifier& /*detElId*/,
						      const std::set<Identifier>& layIds, unsigned nGasGaps ) const {
    LayerHoleVec holes;
    
    // if the number of layers is equal to the number of gas gaps times two in the chamber, there are no holes
    if( layIds.size() == 2*nGasGaps ) return holes;

    // create layer matrix
    typedef std::vector< std::pair<int,int> > LayerMatrix;
    LayerMatrix layerMatrix(nGasGaps,std::make_pair(0,0));

    // loop over layer identifiers and fill
    std::set<Identifier>::const_iterator it = layIds.begin();
    std::set<Identifier>::const_iterator it_end = layIds.end();
    for( ;it!=it_end; ++it ){
      int gasGap       = m_idHelperTool->gasGap(*it);
      bool measuresPhi = m_idHelperTool->measuresPhi(*it);
      // sanity check
      if( gasGap < 0 && gasGap >= (int)nGasGaps ){
	ATH_MSG_WARNING(" gas gap index out of range " << m_idHelperTool->toString(*it) );
	continue;
      }

      // counter starts at 1!
      if( measuresPhi ) layerMatrix[gasGap-1].second = 1;
      else              layerMatrix[gasGap-1].first  = 1;
    }

    // loop over the matrix, if a field is zero we have a hole.
    unsigned int gasGap = 1;
    LayerMatrix::iterator lit = layerMatrix.begin();
    LayerMatrix::iterator lit_end = layerMatrix.end();
    for( ;lit!=lit_end;++lit ){
      
      // check whether the eta or phi hit is missing, create a hole if that is the case
      if( !lit->first )  holes.push_back( std::make_pair(gasGap,0) );
      if( !lit->second ) holes.push_back( std::make_pair(gasGap,1) );
      
      ++gasGap;
    }
    return holes; 
  }


  std::set<Identifier> MuonChamberHoleRecoveryTool::holesInMdtChamber( const Amg::Vector3D& position,
								       const Amg::Vector3D& direction,
								       const Identifier& chId,
								       const std::set<Identifier>& tubeIds ) const {

    // calculate crossed tubes
    const MuonStationIntersect& intersect = m_intersectSvc->tubesCrossedByTrack( chId, position, direction );

    // clear hole vector
    std::set<Identifier> holes;
    for( unsigned int ii=0;ii<intersect.tubeIntersects().size();++ii ){
      const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];

      //if( m_idHelperTool->chamberId(tint.tubeId) != chId ) continue;

      if( tubeIds.count( tint.tubeId ) ) {
	continue;
      }
      if( msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << " intersect " << m_idHelperTool->toString(tint.tubeId) 
			  << "  dist wire " << tint.rIntersect << "  dist to tube end " << tint.xIntersect;
      }
      if( fabs( tint.rIntersect ) > 14.4 || tint.xIntersect > -10. ){
	ATH_MSG_VERBOSE(" not counted" );
      }else{
	// check whether there is a hit in this tube 
	
	ATH_MSG_VERBOSE(" hole tube" );
	
	holes.insert( tint.tubeId );
      }
    }
    return holes;
  }



  const MdtPrepDataCollection* MuonChamberHoleRecoveryTool::findMdtPrdCollection( const Identifier& chId ) const {

    SG::ReadHandle<Muon::MdtPrepDataContainer> h_mdtPrdCont(m_key_mdt);
    const Muon::MdtPrepDataContainer *mdtPrdContainer;
    if (h_mdtPrdCont.isValid()) {
      mdtPrdContainer = h_mdtPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve mdtPrepDataContainer " << m_key_mdt.key() );
      return 0;
    }
    IdentifierHash hash_id;
    m_idHelperTool->mdtIdHelper().get_module_hash(chId,hash_id );

    MdtPrepDataContainer::const_iterator colIt = mdtPrdContainer->indexFind(hash_id);
    if( colIt == mdtPrdContainer->end() ){
      ATH_MSG_DEBUG(" MdtPrepDataCollection for:   " << m_idHelperTool->toStringChamber(chId) 
			   << "  not found in container " );
      return 0;
    }
    return *colIt;
  }

  const CscPrepDataCollection* MuonChamberHoleRecoveryTool::findCscPrdCollection( const Identifier& detElId ) const {

    SG::ReadHandle<Muon::CscPrepDataContainer> h_cscPrdCont(m_key_csc);
    const Muon::CscPrepDataContainer *cscPrdContainer;
    if (h_cscPrdCont.isValid()) {
      cscPrdContainer = h_cscPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve cscPrepDataContainer " << m_key_csc.key() );
      return 0;
    }
    IdentifierHash hash_id;
    m_idHelperTool->cscIdHelper().get_module_hash(detElId,hash_id );
      
    CscPrepDataContainer::const_iterator colIt = cscPrdContainer->indexFind(hash_id);
    if( colIt == cscPrdContainer->end() ){
      ATH_MSG_DEBUG(" CscPrepDataCollection for:   " << m_idHelperTool->toStringChamber(detElId) 
			   << "  not found in container " );
      return 0;
    }
    return *colIt;
  }

  const TgcPrepDataCollection* MuonChamberHoleRecoveryTool::findTgcPrdCollection( const Identifier& detElId ) const {

    SG::ReadHandle<Muon::TgcPrepDataContainer> h_tgcPrdCont(m_key_tgc);
    const Muon::TgcPrepDataContainer *tgcPrdContainer;
    if (h_tgcPrdCont.isValid()) {
      tgcPrdContainer = h_tgcPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve tgcPrepDataContainer " << m_key_tgc.key() );
      return 0;
    }
    IdentifierHash hash_id;
    m_idHelperTool->tgcIdHelper().get_module_hash(detElId,hash_id );

    TgcPrepDataContainer::const_iterator colIt = tgcPrdContainer->indexFind(hash_id);
    if( colIt == tgcPrdContainer->end() ){
      ATH_MSG_DEBUG(" TgcPrepDataCollection for:   " << m_idHelperTool->toStringChamber(detElId)
		    << "  not found in container " );
      return 0;
    }
    return *colIt;
  }

  const RpcPrepDataCollection* MuonChamberHoleRecoveryTool::findRpcPrdCollection( const Identifier& detElId ) const {
    
    SG::ReadHandle<Muon::RpcPrepDataContainer> h_rpcPrdCont(m_key_rpc);
    const Muon::RpcPrepDataContainer *rpcPrdContainer;
    if(h_rpcPrdCont.isValid()) {
      rpcPrdContainer = h_rpcPrdCont.cptr();
    }
    else{
      ATH_MSG_WARNING("Cannot retrieve rpcPrepDataContainer " << m_key_rpc.key() );
      return 0;
    }
    IdentifierHash hash_id;
    m_idHelperTool->rpcIdHelper().get_module_hash(detElId,hash_id );
    RpcPrepDataContainer::const_iterator colIt = rpcPrdContainer->indexFind(hash_id);
    if( colIt == rpcPrdContainer->end() ){
      ATH_MSG_DEBUG(" RpcPrepDataCollection for:   " << m_idHelperTool->toStringChamber(detElId) << "  not found in container " );
      return 0;
    }
    return *colIt;
  }



}
