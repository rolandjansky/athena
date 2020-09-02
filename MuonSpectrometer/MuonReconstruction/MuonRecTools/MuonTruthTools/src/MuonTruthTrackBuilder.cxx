/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// package include
#include "MuonTruthTrackBuilder.h"
// Trk includes
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkSurfaces/PerigeeSurface.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "TrkDetElementBase/TrkDetElementBase.h"

#include "TrkPrepRawData/PrepRawData.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "Identifier/Identifier.h"
#include "TrackRecord/TrackRecordCollection.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/MuonClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"
#include "MuonPrepRawData/MdtPrepData.h"
#include "MuonPrepRawData/MMPrepData.h"
#include "MuonPrepRawData/sTgcPrepData.h"
#include "MuonPrepRawData/MuonCluster.h"
#include "MuonSegment/MuonSegment.h"
// HepMC
#include "AtlasHepMC/GenParticle.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonTrackToSegmentTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"

#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"

#include <map>
#include <vector>

namespace Muon {

  /** Constructor **/
  MuonTruthTrackBuilder::MuonTruthTrackBuilder(const std::string& t, const std::string& n, const IInterface* p) : 
    AthAlgTool(t,n,p),
    m_trackFitter("Trk::GlobalChi2Fitter/MCTBFitter"),
    m_slTrackFitter("Trk::GlobalChi2Fitter/MCTBSLFitter"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_trackToSegmentTool("Muon::MuonTrackToSegmentTool/MuonTrackToSegmentTool"),
    m_mdtCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_muonClusterCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_muonCompRotCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_trackExtrapolationTool("Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"),
    m_trackCleaner("Muon::MuonTrackCleaner/MuonTrackCleaner"),
    m_minNdof(6),
    m_onlyPrimaries(false),
    m_primaryBarcodeCutOff(100000)
  {
    declareInterface<Trk::ITruthTrackBuilder>(this);
    // TrackFitter
    declareProperty("TrackFitter",                       m_trackFitter);
    declareProperty("MdtRotCreator",                     m_mdtCreator);
    declareProperty("MuonClusterCreator",                m_muonClusterCreator);
    declareProperty("MuonCompRotCreator",                m_muonCompRotCreator);
    declareProperty("TrackExtrapolationTool",            m_trackExtrapolationTool);
    declareProperty("TrackCleaner",                      m_trackCleaner);
    declareProperty("MinDegreesOfFreedom",               m_minNdof);
    declareProperty("OnlyPrimaries",                     m_onlyPrimaries); 
    declareProperty("PrimaryBarcodeCutOff",              m_primaryBarcodeCutOff);
    declareProperty("BuildCompRots",              m_buildCompRots = true);
    declareProperty("MDT_SDO_Container",          m_MDT_SimDataMapName = "MDT_SDO" );
    declareProperty("MM_SDO_Container",           m_MM_SimDataMapName  = "MM_SDO" );
    declareProperty("STGC_SDO_Container",         m_STGC_SimDataMapName = "STGC_SDO" );
    declareProperty("CSC_SDO_Container",          m_CSC_SimDataMapName = "CSC_SDO" );
    declareProperty("RPC_SDO_Container",          m_RPC_SimDataMapName = "RPC_SDO" );
    declareProperty("TGC_SDO_Container",          m_TGC_SimDataMapName = "TGC_SDO" );  
    declareProperty("UsePadPhi",                  m_usePadPhiHits = true );
  }


  // Athena algtool's Hooks
  StatusCode  MuonTruthTrackBuilder::initialize()
  {
    ATH_MSG_VERBOSE("Initializing ...");
    ATH_CHECK( m_mdtCreator.retrieve() );
    ATH_CHECK( m_muonClusterCreator.retrieve() );
    ATH_CHECK( m_idHelperSvc.retrieve() );
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_edmHelperSvc.retrieve() );
    ATH_CHECK( m_muonCompRotCreator.retrieve() );
    ATH_CHECK( m_trackFitter.retrieve() );
    ATH_CHECK( m_slTrackFitter.retrieve() );
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_trackToSegmentTool.retrieve() );
    ATH_CHECK( m_trackExtrapolationTool.retrieve() );
    ATH_CHECK( m_trackCleaner.retrieve() ); 

    m_nsegmentFits.resize(MuonStationIndex::StIndexMax);
    m_nsegmentCleaned.resize(MuonStationIndex::StIndexMax);
    m_nfailedSegmentFits.resize(MuonStationIndex::StIndexMax);
    m_nfailedSegmentCleaning.resize(MuonStationIndex::StIndexMax);
    m_ngoodSegmentFits.resize(MuonStationIndex::StIndexMax);
    m_nallSegments.resize(MuonStationIndex::StIndexMax);
    m_truth = 0;
    m_trackFits = 0;
    m_trackCleaned = 0;
    m_failedTrackFits = 0;
    m_failedTrackCleaning = 0;
    m_goodTrackFits = 0;
    m_truthSL = 0;
    m_trackSLFits = 0;
    m_trackCleanedSL = 0;
    m_failedTrackSLFits = 0;
    m_failedTrackCleaningSL = 0;
    m_goodTrackSLFits = 0;
    return StatusCode::SUCCESS;
  }

                                    
  StatusCode  MuonTruthTrackBuilder::finalize()
  {
    ATH_MSG_INFO("Finalizing ...");
    double scaleT = m_truth != 0 ? 1./m_truth : 1;
    ATH_MSG_INFO(" Tracks: " << m_truth << " fits " << m_trackFits << " good " << m_goodTrackFits << " cleaned " << m_trackCleaned
		 << " failed " << m_failedTrackFits << " cleaning " << m_failedTrackCleaning << std::endl
		 << " fractions: fit " << scaleT*m_trackFits << " good " << scaleT*m_goodTrackFits  << " cleaned " << scaleT*m_trackCleaned
		 << " failed " << scaleT*m_failedTrackFits << " cleaning " << scaleT*m_failedTrackCleaning );
    scaleT = m_truthSL != 0 ? 1./m_truthSL : 1;
    ATH_MSG_INFO(" SL Tracks: " << m_truthSL << " fits " << m_trackSLFits << " good " << m_goodTrackSLFits  << " cleaned " << m_trackCleanedSL
		 << " failed " << m_failedTrackSLFits << " cleaning " << m_failedTrackCleaningSL << std::endl
		 << " fractions: fit " << scaleT*m_trackSLFits << " good " << scaleT*m_goodTrackSLFits  << " cleaned " << scaleT*m_trackCleanedSL
		 << " failed " << scaleT*m_failedTrackSLFits << " cleaning " << scaleT*m_failedTrackCleaningSL );
    for( int i=0;i<(int)MuonStationIndex::StIndexMax;++i ){
      MuonStationIndex::StIndex stIndex = (MuonStationIndex::StIndex)(i);
      if( m_nallSegments[stIndex] == 0 ) continue;
      scaleT = 1./m_nallSegments[stIndex];
      ATH_MSG_INFO( " Layer " <<  MuonStationIndex::stName(stIndex) << " truth: " << m_nallSegments[stIndex] 
		    << " fits " << m_nsegmentFits[stIndex] << " good " << m_ngoodSegmentFits[stIndex] << " cleaned " << m_nsegmentCleaned[stIndex]
		    << " fractions: fit " << scaleT*m_nsegmentFits[stIndex] << " good " << scaleT*m_ngoodSegmentFits[stIndex]
		    << " failed " << scaleT*m_nfailedSegmentFits[stIndex] << " cleaning " << scaleT*m_nfailedSegmentCleaning[stIndex] );
    }

    return StatusCode::SUCCESS;
  }

  const TrackRecord* MuonTruthTrackBuilder::getEntryRecord( const HepMC::GenParticle& genPart ) const { 
    const TrackRecordCollection* truthCollection = 0;
    std::string location = "MuonEntryLayer";
    for( int i=0;i<4;++i ){
      if( i == 1 )       location = "MuonEntryLayerFilter";
      else if( i == 2 )  location = "MuonExitLayer";
      else if( i == 3 )  location = "MuonExitLayerFilter";
      else               location = "MuonEntryLayer";
      if ( evtStore()->contains<TrackRecordCollection>(location) ) {
	if( evtStore()->retrieve(truthCollection,location ).isSuccess() ){
	  if( !truthCollection->empty() ) {
	    ATH_MSG_VERBOSE("TrackRecordCollection retrieved at location " << location);
	    break;
	  }
	}
	ATH_MSG_VERBOSE("location " << location << " discarded");
      }
    }
    // nothing found return 0
    if( !truthCollection ) return 0;
    
    TrackRecordCollection::const_iterator it = truthCollection->begin();
    TrackRecordCollection::const_iterator it_end = truthCollection->end();
    for( ;it!=it_end;++it){
      int barcode = (*it).GetBarCode();     
      if( barcode == genPart.barcode() ){

	ATH_MSG_DEBUG("Matched muon in track record: bc " << barcode 
		      << " pt " << (*it).GetMomentum().perp() << " eta " << (*it).GetMomentum().eta() );	
	return &(*it);
      }else if( abs((*it).GetPDGCode()) == 13 ) {
	ATH_MSG_VERBOSE("Found muon in track record: bc " << barcode 
			<< " pt " << (*it).GetMomentum().perp() << " eta " << (*it).GetMomentum().eta() );
      }
    }
    ATH_MSG_DEBUG("No match found for muon: bc " << genPart.barcode() 
		  << " pt " << genPart.momentum().perp() << " eta " << genPart.momentum().eta() );
    return 0;
  }	


  Trk::Track* MuonTruthTrackBuilder::createTrack(const Trk::PRD_TruthTrajectory& prdTraj, Trk::SegmentCollection* segs ) const 
  {

    ATH_MSG_VERBOSE("The PRD Truth trajectory contains " << prdTraj.prds.size() << " PRDs.");
    if( segs ){
      ATH_MSG_VERBOSE(" adding segments ");
    }

    // get the associated GenParticle
    const HepMC::GenParticle* genPart = prdTraj.genParticle;
    if (!genPart) {
      ATH_MSG_WARNING("No GenParticle associated to this PRD_TruthTrajectory. Ignoring track creation.");
      return 0;
    }    
    ATH_MSG_VERBOSE("Got PRD Truth trajectory with " << prdTraj.nDoF << " degrees of freedom, PdgID " << genPart->pdg_id() );
    // check min degrees of freedom
    if ( abs( genPart->pdg_id()) != 13 || (m_minNdof > 0 &&  prdTraj.nDoF < m_minNdof) ) return 0;

    const TrackRecord* trackRecord = getEntryRecord(*genPart);
    if (!trackRecord) {
      ATH_MSG_WARNING("No TrackRecord associated to this PRD_TruthTrajectory.");
      return 0;
    }    

    const MuonSimDataCollection* mdtSimDataMap = retrieveTruthCollection( m_MDT_SimDataMapName );
    if( !mdtSimDataMap) {
      ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << m_MDT_SimDataMapName );
      return 0;
    }
    const MuonSimDataCollection* mmSimDataMap = retrieveTruthCollection( m_MM_SimDataMapName );
    if( !mmSimDataMap) {
      ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << m_MM_SimDataMapName );
      return 0;
    }
    const MuonSimDataCollection* stgcSimDataMap = retrieveTruthCollection( m_STGC_SimDataMapName );
    if( !stgcSimDataMap) {
      ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << m_STGC_SimDataMapName );
    }
    // const CscSimDataCollection* cscSimDataMap = retrieveCscTruthCollection( m_CSC_SimDataMapName );
    // if( !cscSimDataMap) {
    //   ATH_MSG_WARNING(" failed to retrieve CscSimDataCollection: " << m_CSC_SimDataMapName );
    //   return 0;
    // }
    // const MuonSimDataCollection* rpcSimDataMap = retrieveTruthCollection( m_RPC_SimDataMapName );
    // if( !rpcSimDataMap) {
    //   ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << m_RPC_SimDataMapName );
    //   return 0;
    // }
    // const MuonSimDataCollection* tgcSimDataMap = retrieveTruthCollection( m_TGC_SimDataMapName );
    // if( !tgcSimDataMap) {
    //   ATH_MSG_WARNING(" failed to retrieve MuonSimDataCollection: " << m_TGC_SimDataMapName );
    //   return 0;
    // }

    // get the startPosition : math library madness as usual
    Amg::Vector3D startPos(trackRecord->GetPosition().x(),trackRecord->GetPosition().y(),trackRecord->GetPosition().z() );
    Amg::Vector3D startMom(trackRecord->GetMomentum().x(),trackRecord->GetMomentum().y(),trackRecord->GetMomentum().z() );

    //!< get the charge via the particle table ...
    double charge = genPart->pdg_id() == -13 ? 1 : -1;
    
    // the start parameter
    Trk::Perigee per(startPos,startMom,charge,startPos);

    // ----------------------- get the PRDS and start 
    const std::vector<const Trk::PrepRawData*> & prds = prdTraj.prds;


    // here comes the challenge, sorting this hits correctly....
    
    // First sort the hits per detector layer
    std::map<MuonStationIndex::StIndex,DetectorLayer> hitsPerLayer;
    std::map< const Trk::TrkDetElementBase*, std::pair< std::list<const Trk::PrepRawData*>,std::list<const Trk::PrepRawData*> > > clustersPerDetEl;
    unsigned int nbarrel = 0;
    unsigned int nendcap = 0;

    std::vector<const Trk::PrepRawData*>::const_iterator pit = prds.begin();
    std::vector<const Trk::PrepRawData*>::const_iterator pit_end = prds.end();
    for ( ;pit!=pit_end;++pit ) {
      const Trk::PrepRawData& prd = **pit;
      const Identifier& id = prd.identify();
      MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
      bool isEndcap = m_idHelperSvc->isEndcap(id);

      if( isEndcap ) ++nendcap;
      else           ++nbarrel;

      DetectorLayer& detLayer = hitsPerLayer[stIndex];
      detLayer.isEndcap = isEndcap;
      detLayer.stIndex = stIndex;

      // const Trk::MeasurementBase* meas = 0;
      if( m_idHelperSvc->isMdt(id) ){
	const MdtPrepData* mprd = dynamic_cast<const MdtPrepData*>(&prd);
	if( !mprd ) {
	  ATH_MSG_WARNING(" MDT PRD not of type MdtPrepData " << m_idHelperSvc->toString(id) );
	  continue;
	}
	const MuonSimData::Deposit* deposit = getDeposit(*mdtSimDataMap,*genPart,id);
	if( !deposit ){
	  ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
	  continue;
	}

	Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
	const Amg::Vector3D* gpos = prd.detectorElement()->surface(id).localToGlobal(lp);
	if( !gpos ) {
	  ATH_MSG_WARNING(" LocalToGlobal failed " << m_idHelperSvc->toString(id) );
	  continue;
	}

	double val = isEndcap ? fabs(gpos->z()) : gpos->perp();
	// nasty comparisons to figure out which MDT hit comes first
	if( val < detLayer.minPos ){
	  if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
	    detLayer.last3D = detLayer.first3D;
	    detLayer.maxPos = detLayer.minPos;
	  }
	  detLayer.first3D = *gpos;
	  detLayer.minPos = val;
	}else if( val > detLayer.maxPos ){
	  detLayer.last3D = *gpos;
	  detLayer.maxPos = val;
	}
	
	const MdtDriftCircleOnTrack* mdt = m_mdtCreator->createRIO_OnTrack(*mprd,*gpos);
	delete gpos;
	if( !mdt ) {
	  ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
	  continue;
	}
	Trk::DriftCircleSide side = deposit->second.firstEntry() < 0 ? Trk::LEFT : Trk::RIGHT;
	m_mdtCreator->updateSign(*const_cast<MdtDriftCircleOnTrack*>(mdt),side);
	double pull = (mdt->driftRadius()-deposit->second.firstEntry())/Amg::error(mdt->localCovariance(),Trk::locR) ;
	ATH_MSG_VERBOSE(" new MDT    " << m_idHelperSvc->toString(id) << " radius " << mdt->driftRadius() << " true radius " << deposit->second.firstEntry()
			<< " pull " << pull );
	if( fabs(pull)>3. ) ATH_MSG_VERBOSE(" hit with large pull ");
	detLayer.meas.push_back(mdt);
	if( m_idHelperSvc->isSmallChamber(id) ) ++detLayer.nmdtS;
	else                                 ++detLayer.nmdtL;
	// meas = mdt;
      }else if( m_idHelperSvc->isMM(id) ){

	const MMPrepData* mm = dynamic_cast<const MMPrepData*>(&prd);
	if( !mm ) {
	  ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
	  continue;	
	}

	const MuonSimData::Deposit* deposit = getDeposit(*mmSimDataMap,*genPart,id);
	if( !deposit ){
	  ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
	  continue;
	}

	Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
	const Amg::Vector3D* gpos = prd.detectorElement()->surface(id).localToGlobal(lp);
	if( !gpos ) {
	  ATH_MSG_WARNING(" LocalToGlobal failed " <<  m_idHelperSvc->toString(id) );
	  continue;
	}

	double val = isEndcap ? fabs(gpos->z()) : gpos->perp();
	// nasty comparisons to figure out which MDT hit comes first
	if( val < detLayer.minPos ){
	  if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
	    detLayer.last3D = detLayer.first3D;
	    detLayer.maxPos = detLayer.minPos;
	  }
	  detLayer.first3D = *gpos;
	  detLayer.minPos = val;
	}else if( val > detLayer.maxPos ){
	  detLayer.last3D = *gpos;
	  detLayer.maxPos = val;
	}
	const MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack(*mm,*gpos);
	if( !rot ) {
	  delete gpos;
	  ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
	  continue;
	}
	double residual = rot->localParameters().get(Trk::locX)-lp.x();
	double pull = residual / Amg::error(rot->localCovariance(),Trk::locX);
	ATH_MSG_DEBUG( "Adding r " << gpos->perp() << " z " << gpos->z() << "  " << m_idHelperSvc->toString(id) << " " << residual << " pull " << pull  );
	detLayer.meas.push_back(rot);
	// meas = rot;
	++detLayer.nnsw;
	delete gpos;

      }else if( m_idHelperSvc->issTgc(id) ) {

	// skip pads in outer most two chambers as here the wires are more precise
	if( m_idHelperSvc->stgcIdHelper().channelType(id) == 0 && abs(m_idHelperSvc->stationEta(id)) > 2 ) continue;

	if( !stgcSimDataMap ) continue;

	const sTgcPrepData* stgc = dynamic_cast<const sTgcPrepData*>(&prd);
	if( !stgc ) {
	  ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );	  
	  continue;
	}
	
	const MuonSimData::Deposit* deposit = getDeposit(*stgcSimDataMap,*genPart,id);
	if( !deposit ){
	  ATH_MSG_WARNING(" Deposit for GenParticle not found " << m_idHelperSvc->toString(id) );
	  continue;
	}

	Amg::Vector2D lp(deposit->second.firstEntry(),deposit->second.secondEntry());
	const Amg::Vector3D* gpos = prd.detectorElement()->surface(id).localToGlobal(lp);
	if( !gpos ) {
	  ATH_MSG_WARNING(" LocalToGlobal failed " <<  m_idHelperSvc->toString(id) );
	  continue;
	}

	double val = isEndcap ? fabs(gpos->z()) : gpos->perp();
	// // nasty comparisons to figure out which STGC hit comes first
	if( val < detLayer.minPos ){
	  if( detLayer.maxPos < -1e8 && detLayer.minPos < 1e8 ){
	    detLayer.last3D = detLayer.first3D;
	    detLayer.maxPos = detLayer.minPos;
	  }
	  detLayer.first3D = *gpos;
	  detLayer.minPos = val;
	}else if( val > detLayer.maxPos ){
	  detLayer.last3D = *gpos;
	  detLayer.maxPos = val;
	}
      
	const MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack(*stgc,*gpos);
	if( !rot ) {
	  delete gpos;
	  ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
	  continue;
	}
	double residual = rot->localParameters().get(Trk::locX) - lp.x();
	double pull = residual / Amg::error(rot->localCovariance(),Trk::locX);
	ATH_MSG_DEBUG( "Adding r " << gpos->perp() << " z " << gpos->z() << "  " << m_idHelperSvc->toString(id) << " " << residual << " pull " << pull  );
	detLayer.meas.push_back(rot);
	// meas = rot;
	++detLayer.nnsw;
	delete gpos;

      }else{
	const MuonCluster* cl = dynamic_cast<const MuonCluster*>(&prd);
	if( !cl ) {
	  ATH_MSG_WARNING(" cluster PRD not of type MuonCluster " << m_idHelperSvc->toString(id) );
	  continue;
	}
	if( m_buildCompRots ){
	  if( m_idHelperSvc->measuresPhi(id) ) clustersPerDetEl[cl->detectorElement()].first.push_back(cl);
	  else                              clustersPerDetEl[cl->detectorElement()].second.push_back(cl);
	}else{
	  const MuonClusterOnTrack* rot = m_muonClusterCreator->createRIO_OnTrack(*cl,prd.detectorElement()->surface(id).center());
	  if( !rot ) {
	    ATH_MSG_WARNING(" ROT creation failed " << m_idHelperSvc->toString(id) );
	    continue;
	  }
	  ATH_MSG_VERBOSE(" new Clu    " << m_idHelperSvc->toString(id) );
	  detLayer.meas.push_back(rot);
	  // meas = rot;
	}
      }
    }

    if( m_buildCompRots ){
      std::map< const Trk::TrkDetElementBase*, std::pair< std::list<const Trk::PrepRawData*>, std::list<const Trk::PrepRawData*> > >::const_iterator 
	cit = clustersPerDetEl.begin();
      std::map< const Trk::TrkDetElementBase*, std::pair< std::list<const Trk::PrepRawData*>, std::list<const Trk::PrepRawData*> > >::const_iterator 
	cit_end = clustersPerDetEl.end();
      for( ;cit!=cit_end;++cit ){

	const Identifier& id = cit->first->identify();
	bool isEndcap = m_idHelperSvc->isEndcap(id);
	MuonStationIndex::StIndex stIndex = m_idHelperSvc->stationIndex(id);
	DetectorLayer& detLayer = hitsPerLayer[stIndex];
	detLayer.isEndcap = isEndcap;
	ATH_MSG_VERBOSE("  " << m_idHelperSvc->toString(id) << " phi " << cit->second.first.size() << " eta " << cit->second.second.size() );
	const Trk::MeasurementBase* measPhi = 0;
	if( !cit->second.first.empty() ){
	  if( cit->second.first.size() == 1 ){
	    const MuonCluster* cl = dynamic_cast<const MuonCluster*>(cit->second.first.front());
	    if( cl ) measPhi = m_muonClusterCreator->createRIO_OnTrack(*cl,cl->detectorElement()->surface(cl->identify()).center());
	  }else{
	    measPhi = m_muonCompRotCreator->createBroadCluster(cit->second.first,0);
	  }
	}

	const Trk::MeasurementBase* measEta = 0;
	if( !cit->second.second.empty() ){
	  if( cit->second.second.size() == 1 ){
	    const MuonCluster* cl = dynamic_cast<const MuonCluster*>(cit->second.second.front());
	    if( cl ) measEta = m_muonClusterCreator->createRIO_OnTrack(*cl,cl->detectorElement()->surface(cl->identify()).center());
	  }else measEta = m_muonCompRotCreator->createBroadCluster(std::list<const Trk::PrepRawData*>(cit->second.second),0);
	}
	if( measPhi ) {
	  detLayer.meas.push_back(measPhi);
	  ++detLayer.nphi;
	}
	if( measEta ) detLayer.meas.push_back(measEta);

	ATH_MSG_VERBOSE(" Adding compRot for  " << m_idHelperSvc->toStringDetEl(id) << " phi " << cit->second.first.size() << " eta " << cit->second.second.size()  );
      }
    }

    std::map<MuonStationIndex::StIndex,DetectorLayer>::iterator lit = hitsPerLayer.begin();
    std::map<MuonStationIndex::StIndex,DetectorLayer>::iterator lit_end = hitsPerLayer.end();
    for( ;lit!=lit_end;++lit ) {
      sortLayer(lit->second);
      if( segs ) createSegments(lit->second,*segs);
    }

    std::vector<const Trk::MeasurementBase*> measurements;
    measurements.reserve(prds.size());
    bool useEndcap = nendcap > nbarrel;
    bool hasInner = false;
    bool hasEM = false;

    // insert the detector layers into a vector and sort them according to r or z depending whether we are in the barrel or endcap
    std::vector<DetectorLayer*> detLayerVec;
    detLayerVec.reserve(hitsPerLayer.size());
    std::map<MuonStationIndex::StIndex,DetectorLayer>::iterator hlit = hitsPerLayer.begin();
    std::map<MuonStationIndex::StIndex,DetectorLayer>::iterator hlit_end = hitsPerLayer.end();
    for( ;hlit != hlit_end;++hlit ) {
      // skip layers with less than 2 hits
      if( hlit->second.meas.size() < 2 ) continue;
      detLayerVec.push_back(&hlit->second);
    }
    std::stable_sort(detLayerVec.begin(),detLayerVec.end(),SortDetectorLayers(useEndcap) );

    // loop over the layers and extract the hits
    std::set<MuonStationIndex::StIndex> layers;
    std::vector<DetectorLayer*>::iterator dit = detLayerVec.begin();
    std::vector<DetectorLayer*>::iterator dit_end = detLayerVec.end();
    for( ;dit!=dit_end;++dit ){
      measurements.insert(measurements.end(),(*dit)->meas.begin(),(*dit)->meas.end());
      if( (*dit)->stIndex == MuonStationIndex::EM ) hasEM = true;
      if(  (*dit)->stIndex == MuonStationIndex::BI || (*dit)->stIndex == MuonStationIndex::EI ) hasInner = true;
      layers.insert((*dit)->stIndex);
      ATH_MSG_DEBUG("Adding layer " << MuonStationIndex::stName((*dit)->stIndex) << " hits " << (*dit)->meas.size() );
    }
    int nlayers = layers.size();
    ATH_MSG_DEBUG("Creating track with layers " << nlayers );

    Trk::Track* track = 0;
    std::vector<const Trk::MeasurementBase*> finalMeasurements;
    if( measurements.size() < 6 ){
      ATH_MSG_DEBUG("Too few hits to create track ");
    }else{
      

      if( extractMeasurements(per,measurements,finalMeasurements,-1,-1,useEndcap,400.) ){

	// single station track or EM/EO track
	if( (nlayers == 1 && hasEM) || (useEndcap && !hasInner) ){
	  
	  std::map<MuonStationIndex::StIndex,DetectorLayer>::iterator pos = hitsPerLayer.find(MuonStationIndex::EM);
	  if( pos != hitsPerLayer.end() ){
	    DetectorLayer& layer = pos->second;
	    bool hasDir = layer.minPos < 1e8 && layer.maxPos > -1e8;
	    if( !hasDir ){
	      ATH_MSG_DEBUG(" no truth direction available, returning ");
	    }else{
	      Amg::Vector3D dir = 1000000*(layer.last3D - layer.first3D);
	      Trk::Perigee per2(layer.first3D-dir.unit(),dir,-1,layer.first3D);
	      
	      ++m_truthSL;
	      // create the refitted track 
	      track = m_slTrackFitter->fit(finalMeasurements,per2,false,Trk::muon);
	      if( track ) {
		++m_trackSLFits;
		ATH_MSG_VERBOSE("Track fit of truth trajectory successful, track created. " << std::endl 
				<< m_printer->print(*track) << std::endl << m_printer->printStations(*track) );
		std::unique_ptr<Trk::Track> cleanedTrack = m_trackCleaner->clean(*track);
		if( cleanedTrack && !(cleanedTrack->perigeeParameters() == track->perigeeParameters()) ){
		  ATH_MSG_DEBUG("SL Before cleaner " << m_printer->print(*track) << std::endl 
				<< " After cleaner " << m_printer->print(*cleanedTrack) );
		  delete track;
		  //using release until the entire code can be migrated to use smart pointers
		  track = cleanedTrack.release();
		  ++m_trackCleanedSL;
		}else if( !cleanedTrack ) {
		  ++m_failedTrackCleaningSL;
		}
		if( !m_edmHelperSvc->goodTrack(*track,2) ){
		  ATH_MSG_DEBUG(" SL Track with large fit chi2!! " );
		}else ++m_goodTrackSLFits;
	      }else{ 
		ATH_MSG_WARNING("Track fit of truth trajectory NOT successful, NO track created. ");
		++m_failedTrackSLFits;
	      }
	    }
	  }

	}else{
	  ++m_truth;
	  // create the refitted track 
	  track = m_trackFitter->fit(finalMeasurements,per,false,Trk::muon);
	  if( track ) {
	    ATH_MSG_VERBOSE("Track fit of truth trajectory successful, track created. " << std::endl 
			    << m_printer->print(*track) << std::endl << m_printer->printStations(*track) );
	    ++m_trackFits;

	    std::unique_ptr<Trk::Track> cleanedTrack = m_trackCleaner->clean(*track);
	    if( cleanedTrack && !(cleanedTrack->perigeeParameters() == track->perigeeParameters()) ){
	      ATH_MSG_DEBUG("Before cleaner " << m_printer->print(*track) << std::endl 
			    << " After cleaner " << m_printer->print(*cleanedTrack) );
	      delete track;
	      //using release until the entire code can be migrated to use smart pointers
	      track = cleanedTrack.release();
	      ++m_trackCleaned;
	    }else{
	      ++m_failedTrackCleaning;
	    }
	    if( !m_edmHelperSvc->goodTrack(*track,2) ){
	      ATH_MSG_DEBUG(" Track with large fit chi2!! " );
	    }else ++m_goodTrackFits;

	    Trk::Track* entryRecordTrack = m_trackExtrapolationTool->extrapolate( *track );
	    if( !entryRecordTrack ) {
	      ATH_MSG_WARNING("Failed to extrapolate track to muon entry record");
	    }else{
	      delete track;
	      track = entryRecordTrack;
	    }
	  }else{ 
	    ATH_MSG_WARNING("Track fit of truth trajectory NOT successful, NO track created. ");
	    ++m_failedTrackFits;
	  }
	}
      }
    }
    // return what you have

    lit = hitsPerLayer.begin();
    lit_end = hitsPerLayer.end();
    for( ;lit!=lit_end;++lit ) {
      std::vector<const Trk::MeasurementBase*>::iterator hit = lit->second.meas.begin();
      std::vector<const Trk::MeasurementBase*>::iterator hit_end = lit->second.meas.end();
      for( ;hit!=hit_end;++hit ) delete *hit;
    }
    
    return track;
  }


  void MuonTruthTrackBuilder::sortLayer( MuonTruthTrackBuilder::DetectorLayer& layer ) const {
    ATH_MSG_VERBOSE(" Sorting Detector Layer: meas " << layer.meas.size() );
    std::stable_sort(layer.meas.begin(),layer.meas.end(),SortMeasurementsByPosition(layer.isEndcap));
    if( msgLvl(MSG::VERBOSE) ){
      if( layer.minPos < 1e8 )  msg(MSG::VERBOSE) << " first " << layer.first3D.perp() << " z " << layer.first3D.z() << std::endl;
      if( layer.maxPos > -1e8 ) msg(MSG::VERBOSE) << " second " << layer.last3D.perp() << " z " << layer.last3D.z() << std::endl;
      std::vector<const Trk::MeasurementBase*>::const_iterator it = layer.meas.begin();
      std::vector<const Trk::MeasurementBase*>::const_iterator it_end = layer.meas.end();
      for( ;it!=it_end;++it ){
	msg(MSG::VERBOSE) << " r  " << (*it)->globalPosition().perp() <<  " z  " << (*it)->globalPosition().z() 
			  << "   " <<  m_idHelperSvc->toString( m_edmHelperSvc->getIdentifier(**it) ) << std::endl;
      }
      msg(MSG::VERBOSE) << endmsg;
    }
  }

  void MuonTruthTrackBuilder::createSegments( MuonTruthTrackBuilder::DetectorLayer& layer, Trk::SegmentCollection& segs ) const {
    if(  layer.meas.size() < 3 ) return;

    bool hasDir = layer.minPos < 1e8 && layer.maxPos > -1e8;
    if( !hasDir ){
      ATH_MSG_DEBUG(" no truth direction available, returning ");
      return;
    }
    Amg::Vector3D dir = 1000000*(layer.last3D - layer.first3D);
    Trk::Perigee per(layer.first3D-dir.unit(),dir,-1,layer.first3D);

    bool splitNSWEI = layer.nnsw > 0 && layer.nmdtL + layer.nmdtS > 0;
    int nseg = 2;
    if( splitNSWEI ) nseg = 4;
    ATH_MSG_VERBOSE(" creating segments Layer: meas " << layer.meas.size() << " nphi " << layer.nphi << " nmdt " << layer.nmdtL + layer.nmdtS
		    << " nsw " << layer.nnsw << " " << m_printer->print(per) << " trials " << nseg );

    // loop over possible segments to be made 
    for( int i=0;i<nseg;++i ){

      std::vector<const Trk::MeasurementBase*> hits;
      int splitSL = i%2 == 0 ? 0 : 1;
      int splitOpt = -1;
      if( layer.nnsw > 0 && i < 2 )  splitOpt = 0;
      if( layer.nnsw > 0 && i >= 2 ) splitOpt = 1;
      if( !extractMeasurements( per, layer.meas, hits, splitSL, splitOpt, layer.isEndcap, 200 ) ) continue;


      ++m_nallSegments[layer.stIndex];

      Trk::Track* track = m_slTrackFitter->fit(hits,per,true,Trk::nonInteracting);
      if( !track ){
	ATH_MSG_WARNING("Segment fit of truth trajectory NOT successful, NO segment created. ");
      }else{

	std::unique_ptr<Trk::Track> cleanedTrack = m_trackCleaner->clean(*track);
	if( cleanedTrack && !(cleanedTrack->perigeeParameters() == track->perigeeParameters()) ){
	  ATH_MSG_DEBUG("Segment Before cleaner " << m_printer->print(*track) << std::endl 
			<< " After cleaner " << m_printer->print(*cleanedTrack) );
	  delete track;
	  //using release until the entire code can be migrated to use smart pointers
	  track = cleanedTrack.release();
	  ++m_nsegmentCleaned[layer.stIndex];
	}else if( !cleanedTrack ) {
	  ATH_MSG_WARNING("Segment cleaning failed, NO segment created. ");
	  ++m_nfailedSegmentCleaning[layer.stIndex];
	}
	if( !m_edmHelperSvc->goodTrack(*track,2) ){
	  ATH_MSG_DEBUG(" Segment with large fit chi2!! " );
	}
      }

      MuonSegment* seg = track ? m_trackToSegmentTool->convert(*track) : 0;
      delete track;
      if( seg ){
	ATH_MSG_VERBOSE("Segment  created. " << m_printer->print(*seg));
	segs.push_back(seg);
	++m_nsegmentFits[layer.stIndex];
	if( seg->fitQuality() && seg->fitQuality()->numberDoF() != 0 && seg->fitQuality()->chiSquared()/seg->fitQuality()->numberDoF() < 3 ){
	  ++m_ngoodSegmentFits[layer.stIndex];
	}
      }else{
	++m_nfailedSegmentFits[layer.stIndex];
      }
    }
  }

  Trk::PseudoMeasurementOnTrack* MuonTruthTrackBuilder::createPseudo( const Trk::TrackParameters& pars, const Trk::MeasurementBase& meas ) const {
    double ly = 0.;
    if( meas.associatedSurface().isOnSurface(pars.position(),false) ){
      const Amg::Vector2D* lpos = meas.associatedSurface().globalToLocal(pars.position());
      if( !lpos ){
	ATH_MSG_WARNING("Failed globalToLocal ");
	return 0; 
      }
      ly = (*lpos)[Trk::locY];
      delete lpos;
    }else{
      const Trk::TrackParameters* measPars = m_extrapolator->extrapolateDirectly(pars,meas.associatedSurface(),Trk::anyDirection,false,Trk::nonInteracting);
      if( !measPars ){
	ATH_MSG_WARNING("Failed to reach measurement, cannot create PseudoMeasurement, z_meas " << meas.globalPosition().z()
			<< " z_pars " << pars.position().z() << " p " << pars.momentum().mag() );
	return 0; 
      }
      ly = measPars->parameters()[Trk::locY];
      delete measPars;
    }

    // Error matrix
    double errPos = 0.1;
    Amg::MatrixX cov(1,1);
    cov(0,0) = errPos*errPos;
    ATH_MSG_VERBOSE("Creating pseudo: r " << meas.globalPosition().perp() << " z " << meas.globalPosition().z() << " local position " << ly);
    return new Trk::PseudoMeasurementOnTrack(Trk::LocalParameters( Trk::DefinedParameter(ly,Trk::locY) ),
					     cov,meas.associatedSurface());
  }

  MuonSegment* MuonTruthTrackBuilder::createSegment( const Trk::TrackParameters& /*pars*/, std::vector<const Trk::MeasurementBase*>& measurements) const {
    ATH_MSG_VERBOSE("createSegment " << measurements.size() );
    return 0;
  }

  const MuonSimDataCollection* MuonTruthTrackBuilder::retrieveTruthCollection( std::string colName ) const {
    // Retrieve SDO map for this event
    if(!evtStore()->contains<MuonSimDataCollection>(colName)) return 0;

    const MuonSimDataCollection* truthCol(0);
    if(!evtStore()->retrieve(truthCol, colName).isSuccess()) {
      ATH_MSG_VERBOSE("Could NOT find the MuonSimDataMap map key = "<< colName);
    }   else {
      ATH_MSG_VERBOSE("Retrieved MuonSimDataCollection for key = " << colName);
    }
    return truthCol;
  }

  const CscSimDataCollection* MuonTruthTrackBuilder::retrieveCscTruthCollection( std::string colName ) const {
    // Retrieve SDO map for this event
    if(!evtStore()->contains<CscSimDataCollection>(colName)) return 0;
    const CscSimDataCollection* truthCol(0);
    if(!evtStore()->retrieve(truthCol, colName).isSuccess()) {
      ATH_MSG_VERBOSE("Could NOT find the CscSimDataMap map key = "<< colName);
    }   else {
      ATH_MSG_VERBOSE("Retrieved CscSimDataCollection for key = " << colName);
    }
    return truthCol;
  }

  const MuonSimData::Deposit* MuonTruthTrackBuilder::getDeposit( const MuonSimDataCollection& simCol, const HepMC::GenParticle& genPart, const Identifier& id ) const {
    MuonSimDataCollection::const_iterator it = simCol.find(id);
    if( it == simCol.end() ) {
      ATH_MSG_WARNING(" Truth PRD not found in simdata collection: " << m_idHelperSvc->toString(id) );
      return 0;
    }
    
    const MuonSimData& simData = it->second;
    const MuonSimData::Deposit* deposit = 0;
    std::vector<MuonSimData::Deposit>::const_iterator dit = simData.getdeposits().begin();
    std::vector<MuonSimData::Deposit>::const_iterator dit_end = simData.getdeposits().end();
    for( ; dit!=dit_end;++dit ){
      const HepMC::GenParticle* gp = dit->first;
      if( gp == &genPart ){
	deposit = &*dit;
	break;
      }
    }
    return deposit;
  }

  bool MuonTruthTrackBuilder::extractMeasurements( const Trk::TrackParameters& per, 
						   const std::vector<const Trk::MeasurementBase*>& hitsIn,
						   std::vector<const Trk::MeasurementBase*>& hitsOut,
						   int splitSL, int splitNSWEI, bool isEndcap, double phiDist ) const {
    int nprec = 0;
    int ntrigEta = 0;
    const Trk::MeasurementBase* firstPhi = 0;
    const Trk::MeasurementBase* lastPhi = 0;
    const Trk::MeasurementBase* firstMeas = 0;
    const Trk::MeasurementBase* lastMeas = 0;

    std::vector<const Trk::MeasurementBase*>::const_iterator it = hitsIn.begin();
    std::vector<const Trk::MeasurementBase*>::const_iterator it_end = hitsIn.end();
    for( ;it!=it_end;++it ){
      // skip 
      Identifier id = m_edmHelperSvc->getIdentifier(**it);
      if( splitSL != -1 ){
	bool isSmall = !m_idHelperSvc->isSmallChamber(id);
	bool isTgc = m_idHelperSvc->isTgc(id);
	if( isSmall != splitSL && !isTgc ) continue;
      }

      // for the first two trails build the NSW segments, for the second two the MDT ones
      if( splitNSWEI == 0 && (m_idHelperSvc->isMdt(id) || m_idHelperSvc->isTgc(id) ) ) continue;
      if( splitNSWEI == 1 && (m_idHelperSvc->issTgc(id) || m_idHelperSvc->isMM(id) ) ) continue;

      hitsOut.push_back(*it);

      bool isTrigger = m_idHelperSvc->isTrigger(id);
      bool measPhi =  m_idHelperSvc->measuresPhi(id);
      
      // for now exclude STGC pads as phi measurements
      if( !m_usePadPhiHits && m_idHelperSvc->issTgc(id) && m_idHelperSvc->stgcIdHelper().channelType(id) == 0 ) measPhi = false;

      if( !isTrigger && !measPhi ) ++nprec;
      if( measPhi ){ 
	if( !firstPhi ) firstPhi = *it;
	else            lastPhi  = *it;
      }else if( isTrigger ) ++ntrigEta;

      if( !measPhi && !isTrigger ){
	if( !firstMeas ) firstMeas = *it;
	else             lastMeas  = *it;
      }
    }
    
    if( !lastMeas ) return false;
    Identifier id = m_edmHelperSvc->getIdentifier(*firstMeas);
    MuonStationIndex::StIndex stFirst = m_idHelperSvc->stationIndex( id );
    bool isEM = stFirst != MuonStationIndex::EM;
    if( (!isEM && nprec < 3 ) ||  ( isEM && (nprec < 2 || nprec + ntrigEta < 4 ) ) ) return false;
    if( (m_idHelperSvc->issTgc(id) || m_idHelperSvc->isMM(id)) && nprec < 5 ) return false;

    ATH_MSG_DEBUG(" prec hits: " << nprec << " trigEta " << ntrigEta << " tot hits " << hitsOut.size() );
    double dist = 0.;
    bool addedFakePhiFirst = false;
    if( firstPhi && lastPhi ){
      Amg::Vector3D diff = lastPhi->globalPosition()-firstPhi->globalPosition();
      dist = isEndcap ? diff.z() : diff.perp();
      ATH_MSG_DEBUG(" layer with sufficient phi hits: dist " << dist << " diff " << diff );
    }
    if( fabs(dist) < phiDist ){
      ATH_MSG_DEBUG(" poor phi constraint, adding pseudo measurements " );

      // we have too few phi measurements so will have to invent some
      if( firstPhi ){
	// case where we do have some phi measurements but these are too close together
	// figure out whether to add the fake hit before or after the phi hit(s)
	  
	double posFirstPhi  = isEndcap ? firstPhi->globalPosition().z() : firstPhi->globalPosition().perp();
	double posLastPhi   = lastPhi ? (isEndcap ? lastPhi->globalPosition().z() : lastPhi->globalPosition().perp()) : posFirstPhi;
	double posFirstMeas = isEndcap ? firstMeas->globalPosition().z() : firstMeas->globalPosition().perp();
	double posLastMeas  = isEndcap ? lastMeas->globalPosition().z() : lastMeas->globalPosition().perp();

	//Amg::Vector3D diff = firstPhi->globalPosition()-lastMeas->globalPosition();
	double distLastMeas =  posLastMeas - posFirstPhi;
	double distFirstMeas = posFirstMeas - posLastPhi;
	// std::cout << " pos phif " << posFirstPhi << " last " << posLastPhi << " measf " << posFirstMeas << " last " << posLastMeas
	// 	    << " distF " << distFirstMeas << " last " << distLastMeas << std::endl;
	// inserting after the last hit
	addedFakePhiFirst = fabs(distLastMeas) < fabs(distFirstMeas);
	Trk::PseudoMeasurementOnTrack* pseudo = createPseudo( per, addedFakePhiFirst ? *firstMeas : *lastMeas );
	if( !pseudo ){
	  ATH_MSG_WARNING("Failed to create pseudo measurement");
	  return false;
	}
	hitsOut.insert( std::find(hitsOut.begin(),hitsOut.end(), addedFakePhiFirst ? firstMeas : lastMeas), pseudo);
      }else{
	Trk::PseudoMeasurementOnTrack* pseudo =  createPseudo( per, *lastMeas );
	if( !pseudo ){
	  ATH_MSG_WARNING("Failed to create pseudo measurement");
	  return false;
	}
	hitsOut.insert( std::find(hitsOut.begin(),hitsOut.end(), lastMeas), pseudo);
	//hitsOut.push_back(pseudo);
	pseudo =  createPseudo( per, *firstMeas );
	if( !pseudo ){
	  ATH_MSG_WARNING("Failed to create pseudo measurement");
	  return false;
	}
	hitsOut.insert( std::find(hitsOut.begin(),hitsOut.end(), firstMeas), pseudo);
	//hitsOut.insert(hitsOut.begin(),pseudo);
	addedFakePhiFirst = true;
      }
    }

    // final check to ensure that we have a phi measurement or a pseudo at the start of the full track
    if( splitNSWEI == -1 && !addedFakePhiFirst ){
      // check if the is a phi measurement and it is in the same station layer
      if( !firstPhi || stFirst != m_idHelperSvc->stationIndex(  m_edmHelperSvc->getIdentifier(*firstPhi) ) ){
	Trk::PseudoMeasurementOnTrack* pseudo =  createPseudo( per, *firstMeas );
	if( !pseudo ){
	  ATH_MSG_WARNING("Failed to create pseudo measurement");
	  return false;
	}
	hitsOut.insert( std::find(hitsOut.begin(),hitsOut.end(), firstMeas), pseudo);
	//hitsOut.insert(hitsOut.begin(),pseudo);
      }
    }

    return true;
  }
}
