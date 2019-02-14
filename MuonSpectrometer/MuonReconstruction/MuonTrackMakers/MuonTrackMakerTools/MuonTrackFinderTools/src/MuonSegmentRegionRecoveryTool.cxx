/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonSegmentRegionRecoveryTool.h"

#include "MuonChamberHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonSeededSegmentFinder.h"
#include "MuonTrackMakerUtils/MuonGetClosestParameters.h"
#include "MuonTrackMakerUtils/SortTracksByHitNumber.h"
#include "MuonTrackMakerUtils/SortMeasurementsByPosition.h"
#include "MuonTrackMakerUtils/MuonTrackMakerStlTools.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/MuonStationIndex.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonPrepRawData/MdtPrepDataCollection.h"
#include "MuonPrepRawData/RpcPrepDataCollection.h"
#include "MuonPrepRawData/TgcPrepDataCollection.h"
#include "MuonPrepRawData/CscPrepDataCollection.h"
// New Small Wheel
#include "MuonPrepRawData/sTgcPrepDataCollection.h"
#include "MuonPrepRawData/MMPrepDataCollection.h"

#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRIO_OnTrack/RpcClusterOnTrack.h"
#include "MuonRIO_OnTrack/TgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/CscClusterOnTrack.h"
// New Small Wheel
#include "MuonRIO_OnTrack/sTgcClusterOnTrack.h"
#include "MuonRIO_OnTrack/MMClusterOnTrack.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"

#include "MuonStationIntersectSvc/MuonStationIntersectSvc.h"
#include "MuonStationIntersectSvc/MuonStationIntersect.h"

#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkGeometry/MagneticFieldProperties.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"

#include "TrkExInterfaces/IExtrapolator.h"

#include "TrkToolInterfaces/ITrackHoleSearchTool.h"
#include "TrkToolInterfaces/IResidualPullCalculator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"

#include "TrkEventPrimitives/ResidualPull.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkEventUtils/TrackStateOnSurfaceComparisonFunction.h"

#include "IRegionSelector/IRegSelSvc.h"
#include "RoiDescriptor/RoiDescriptor.h"

#include "EventPrimitives/EventPrimitivesHelpers.h"
#include "EventPrimitives/EventPrimitivesToStringConverter.h"
#include "TrkSegment/SegmentCollection.h"

#include <map>

namespace Muon {

MuonSegmentRegionRecoveryTool::MuonSegmentRegionRecoveryTool(const std::string& ty, const std::string& na, const IInterface* pa)
  : AthAlgTool(ty, na, pa),
    m_seededSegmentFinder("Muon::MuonSeededSegmentFinder/MuonSeededSegmentFinder"),
    m_trackSegmentMatchingTool("Muon::MooCandidateMatchingTool/MooCandidateMatchingTool"),
    m_chamberHoleRecoveryTool("Muon::MuonChamberHoleRecoveryTool/MuonChamberHoleRecoveryTool"),
    m_extrapolator("Trk::Extrapolator/MuonExtrapolator"),
    m_fitter("Rec::CombinedMuonTrackBuilder/CombinedMuonTrackBuilder"),
    m_intersectSvc("MuonStationIntersectSvc", name()),
    m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_helperTool("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_hitSummaryTool("Muon::MuonHitSummaryTool/MuonHitSummaryTool"),
    m_regionSelector("RegSelSvc", name())
{
  declareInterface<IMuonHoleRecoveryTool>(this);
  declareProperty("SeededSegmentFinder",     m_seededSegmentFinder);
  declareProperty("TrackSegmentMatchingTool", m_trackSegmentMatchingTool);
  declareProperty("ChamberHoleRecoveryTool", m_chamberHoleRecoveryTool);
  declareProperty("Extrapolator",            m_extrapolator);
  declareProperty("Fitter",                  m_fitter );
  declareProperty("MuonStationIntersectSvc", m_intersectSvc);
  declareProperty("IdHelper",                m_idHelperTool);
  declareProperty("EDMHelper",               m_helperTool);
  declareProperty("EDMPrinter",              m_printer);
  declareProperty("HitSummaryTool",          m_hitSummaryTool);
  declareProperty("RegionSelector",          m_regionSelector);

  declareProperty("DeltaEtaRegion",  m_deta = 0.05);
  declareProperty("DeltaPhiRegion",  m_dphi = 0.1);
  declareProperty("ExcludeEES",  m_excludeEES = true);
  declareProperty("OnlyEO",      m_onlyEO = false);
  declareProperty("UseFitterOutlierLogic", m_useFitterOutlierLogic = true );
  declareProperty("DoNSW", m_doNSW = true); // assume that if NSW no CSC chambers are present
}

MuonSegmentRegionRecoveryTool::~MuonSegmentRegionRecoveryTool() {}

StatusCode MuonSegmentRegionRecoveryTool::initialize()
{

  ATH_CHECK( detStore()->retrieve( m_detMgr ) );
  ATH_CHECK( m_helperTool.retrieve() );
  ATH_CHECK( m_intersectSvc.retrieve() );
  ATH_CHECK( m_printer.retrieve() );
  ATH_CHECK( m_seededSegmentFinder.retrieve() );

  if ( !m_trackSegmentMatchingTool.empty() ) {
    ATH_CHECK( m_trackSegmentMatchingTool.retrieve() );
    ATH_MSG_INFO("Using matching tool " << m_trackSegmentMatchingTool );
  } else {
    ATH_MSG_DEBUG("No matching tool selected " );
  }

  ATH_CHECK( m_chamberHoleRecoveryTool.retrieve() );
  ATH_CHECK( m_extrapolator.retrieve() );
  ATH_CHECK( m_fitter.retrieve() );
  ATH_CHECK( m_idHelperTool.retrieve() );
  ATH_CHECK( m_hitSummaryTool.retrieve() );
  ATH_CHECK( m_regionSelector.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode MuonSegmentRegionRecoveryTool::finalize()
{
  return StatusCode::SUCCESS;
}


Trk::Track* MuonSegmentRegionRecoveryTool::recover( const Trk::Track& track ) const {

  Trk::Track* result = recoverImp(track);;
  if ( result ) ATH_MSG_DEBUG("Final track " << m_printer->print(*result) << std::endl << m_printer->printStations(*result) );
  else         ATH_MSG_DEBUG("Track lost during recovery");
  return result;
}

/**Algorithmic part
   Strategy:
   1) Extrapolate back to MS entry record
   2) Collect hashes of chambers close to where the track passed
   3) Compare collected Hashes with the ones already on track
   4) Recover segments on the missing chambers by launching the SeededSegmentFinder
*/

Trk::Track* MuonSegmentRegionRecoveryTool::recoverImp( const Trk::Track& track ) const {
  //Welcome into the implementation of the recovery method
  ATH_MSG_VERBOSE(" Entering segment recovery method " << m_printer->print(track) << std::endl << m_printer->printStations(track) );

  //First run the MuonChamberHoleRecoveryTool on the input track
  const Trk::Track* chRecTrack = m_chamberHoleRecoveryTool->recover(track);
  if (!chRecTrack) {
    ATH_MSG_DEBUG(" MuonChamberHoleRecoveryTool failed to create a new track "
                  << " Returning input (unrecovered) track " );
    return new Trk::Track( track );
  }
  ATH_MSG_VERBOSE(" After chamber hole recovery " << m_printer->print(track) << std::endl << m_printer->printStations(track) );

  // only run this on single station EM tracks
  if ( m_onlyEO ) {
    // should be a sl track
    if ( !m_helperTool->isSLTrack(*chRecTrack) ) return const_cast<Trk::Track*>(chRecTrack);

    // get hit summary
    IMuonHitSummaryTool::CompactSummary hitSummary = m_hitSummaryTool->summary(*chRecTrack);
    // should be single station
    if ( hitSummary.stationLayers.size() != 1 ||
         !hitSummary.stationLayers.count(MuonStationIndex::EM) ) return const_cast<Trk::Track*>(chRecTrack);
    ATH_MSG_DEBUG("Single station track, checking for EO hits");
  }

  //1) Extrapolate back to MS entry record
//     const Trk::TrackParameters* msEntryPars = goToMSEntryRecord( *chRecTrack );
//     if(!msEntryPars) return chRecTrack;

  /** 2)  Collect hashes of chambers close to where the track passed
  2a) Extrapolate forward to the next station
  2b) Launch selector to get IDHashes
  2c) push_back hashes for this station into a set
  2d) stop where outer layers are
  2e) the outcome of all of this is a std::set of collected hashes
      (some are already on the track, no need to recover)  */


  MuonData muonData;
  // extract the hashes already on track
  fillOnTrackChambers ( *chRecTrack, muonData );

  // collect the hashes not in track close to the track
  collectCrossedChambers( *chRecTrack, muonData );

  //3b) compare the two std::sets and make a final std::set of not-yet-on-track Hashes
  const Trk::Track* triggerRecTrack = addMissingChambers( *chRecTrack, muonData, false );
  if ( triggerRecTrack ) {
    if ( chRecTrack != &track ) delete chRecTrack;
    chRecTrack = triggerRecTrack;
  }

  const Trk::Track* mdtRecTrack = addMissingChambers( *chRecTrack, muonData, true );
  if ( mdtRecTrack ) {
    if ( chRecTrack != &track ) delete chRecTrack;
    chRecTrack = mdtRecTrack;
  }


  const Trk::Track* mdtRecTrackWithHoles = findHoles( *chRecTrack, muonData );
  if ( mdtRecTrackWithHoles ) {
    if ( chRecTrack != &track ) delete chRecTrack;
    chRecTrack = mdtRecTrackWithHoles;
  }

  //recovered track, success!
  return const_cast<Trk::Track*>(chRecTrack);
}


//// NO, NO, NO, should pass in an IRoiDescriptor
// void MuonSegmentRegionRecoveryTool::addHashes( DETID type, double etamin, double etamax, double phimin, double phimax,
void MuonSegmentRegionRecoveryTool::addHashes( DETID type, const IRoiDescriptor& roi,
    std::set<IdentifierHash>& hashes, const std::set<IdentifierHash>& exclusion ) const {

  // if only looking at EO, skip all but MDT chambers
  if ( m_onlyEO && type != MDT ) return;

  std::vector<IdentifierHash> crossed;

  m_regionSelector->DetHashIDList(type, roi, crossed );

  //if( type == MDT ) ATH_MSG_VERBOSE(" adding hashes for type " << type << " size " << crossed.size() << " eta,phi " << eta << "  " << phi );
  for ( std::vector<IdentifierHash>::iterator it = crossed.begin(); it != crossed.end(); ++it ) {
    if ( !exclusion.count(*it) && !hashes.count(*it) ) {
      if ( type == MDT ) {
        Identifier chId;
        IdContext otCont = m_idHelperTool->mdtIdHelper().module_context();
        m_idHelperTool->mdtIdHelper().get_id(*it, chId, &otCont);

        if ( m_excludeEES && m_idHelperTool->chamberIndex(chId) == MuonStationIndex::EES ) {
          ATH_MSG_VERBOSE("  excluding " << *it << " " << m_idHelperTool->toStringChamber(chId) );
          continue;
        }
        if ( m_onlyEO &&  m_idHelperTool->stationIndex(chId) != MuonStationIndex::EO ) {
          ATH_MSG_VERBOSE("  excluding " << *it << " " << m_idHelperTool->toStringChamber(chId) );
          continue;
        }
        if ( m_doNSW &&  m_idHelperTool->stationIndex(chId) == MuonStationIndex::EI && abs(m_idHelperTool->stationEta(chId)) < 4) {
          ATH_MSG_VERBOSE("  NSW excluding " << *it << " " << m_idHelperTool->toStringChamber(chId) );
          continue;
        }
        ATH_MSG_VERBOSE("  -- hash " << *it << " " << m_idHelperTool->toStringChamber(chId) );
      }
      if ( type == CSC ) {
        Identifier chId;
        IdContext otCont = m_idHelperTool->cscIdHelper().module_context();
        m_idHelperTool->cscIdHelper().get_id(*it, chId, &otCont);
        ATH_MSG_VERBOSE("  -- csc hash " << *it << " " << m_idHelperTool->toStringChamber(chId) );
      }
      // New Small Wheel
      if ( type == STGC ) {
        Identifier chId;
//        IdContext otCont = m_idHelperTool->stgcIdHelper().module_context();
        IdContext otCont = m_idHelperTool->stgcIdHelper().detectorElement_context();
        m_idHelperTool->stgcIdHelper().get_id(*it, chId, &otCont);
        ATH_MSG_VERBOSE("  -- stgc hash " << *it << " " << m_idHelperTool->toStringChamber(chId) );
      }
      if ( type == MM ) {
        Identifier chId;
//        IdContext otCont = m_idHelperTool->mmIdHelper().module_context();
        IdContext otCont = m_idHelperTool->mmIdHelper().detectorElement_context();
        m_idHelperTool->mmIdHelper().get_id(*it, chId, &otCont);
        ATH_MSG_VERBOSE("  -- mm hash " << *it << " " << m_idHelperTool->toStringChamber(chId) );
      }
      hashes.insert(*it);
    }
  }
}


void MuonSegmentRegionRecoveryTool::collectCrossedChambers( const Trk::Track& track,
    MuonSegmentRegionRecoveryTool::MuonData& data ) const {
  ATH_MSG_VERBOSE(" Collecting all crossed chambers" );


  const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
  if ( !states ) {
    ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
    return;
  }

  std::set<MuonStationIndex::StIndex> stations;
  double etamin = 1e9;
  double etamax = -1e9;
  double phimin = 1e9;
  double phimax = -1e9;
  // loop over TSOSs
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

  for ( ; tsit != tsit_end ; ++tsit ) {

    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if ( !pars ) continue;

    double rpos = pars->position().perp();
    double zpos = fabs(pars->position().z());
    double eta    = pars->position().eta();
    double phi    = pars->position().phi();
    if ( msgLvl(MSG::DEBUG) ) {
      msg() << "All State parameters: eta: " << eta << " ,phi: " << phi << " pos r " << rpos << " z " << zpos;
      if ( pars->covariance() ) msg() << MSG::DEBUG << " extrapolation error " << Amg::error(*pars->covariance(), Trk::locX);
      msg() << endmsg;
    }
    if ( rpos < 2500 && zpos < 4000 ) continue;

    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    if ( !meas ) continue;

    Identifier id = m_helperTool->getIdentifier(*meas);
    bool pseudo = !id.is_valid();

    if ( pseudo || !m_idHelperTool->mdtIdHelper().is_muon(id) ) continue;

    if ( eta > etamax ) etamax = eta;
    if ( eta < etamin ) etamin = eta;
    if ( phi > phimax ) phimax = phi;
    if ( phi < phimin ) phimin = phi;
    if ( msgLvl(MSG::DEBUG) ) {
      msg() << "Selected State parameters: eta: " << eta << " ,phi: " << phi << " pos r " << rpos << " z " << zpos;
      if ( pars->covariance() ) msg() << MSG::DEBUG << " extrapolation error " << Amg::error(*pars->covariance(), Trk::locX);
      msg() << endmsg;
    }

    MuonStationIndex::StIndex stIndex = m_idHelperTool->stationIndex(id);
    if ( stations.count(stIndex) ) continue;
    stations.insert(stIndex);


    //At each station launch the Reg Sel and stuff the Id hashes in a vector
  }
  etamin -= m_deta;
  etamax += m_deta;
  phimin -= m_dphi;
  phimax += m_dphi;

  ATH_MSG_DEBUG("Eta range: " << etamin << " " << etamax << " Phi range " << phimin << " " << phimax );
  if (etamin > etamax) { //something went wrong with the fit and no hits were selected in the loop
    ATH_MSG_DEBUG("no hits selected, nothing further will be done");
    return;
  }

  RoiDescriptor roi( etamin, etamax, phimin, phimax );

  //    addHashes(MDT,etamin,etamax,phimin,phimax,data.mdt,data.mdtTrack);
  //    addHashes(RPC,etamin,etamax,phimin,phimax,data.rpc,data.rpcTrack);
  //    addHashes(TGC,etamin,etamax,phimin,phimax,data.tgc,data.tgcTrack);
  //    addHashes(CSC,etamin,etamax,phimin,phimax,data.csc,data.cscTrack);

  addHashes(MDT, roi, data.mdt, data.mdtTrack);
  addHashes(RPC, roi, data.rpc, data.rpcTrack);
  addHashes(TGC, roi, data.tgc, data.tgcTrack);
  if (!m_doNSW) addHashes(CSC, roi, data.csc, data.cscTrack);
  // New Small Wheel
  addHashes(STGC, roi, data.stgc, data.stgcTrack);
  addHashes(MM, roi, data.mm, data.mmTrack);


  std::set<IdentifierHash>::iterator hsit = data.mdt.begin();
  std::set<IdentifierHash>::iterator hsit_end = data.mdt.end();
  for ( ; hsit != hsit_end; ++hsit ) {
    Identifier chId;
    IdContext otCont = m_idHelperTool->mdtIdHelper().module_context();
    m_idHelperTool->mdtIdHelper().get_id(*hsit, chId, &otCont);
    MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
    data.mdtPerStation[chIndex].insert(*hsit);
    ATH_MSG_VERBOSE(" chamberlayer  " << MuonStationIndex::chName(chIndex) << "  " << m_idHelperTool->toStringChamber(chId) );
  }
  hsit = data.csc.begin();
  hsit_end = data.csc.end();
  for ( ; hsit != hsit_end; ++hsit ) {
    Identifier chId;
    IdContext otCont = m_idHelperTool->cscIdHelper().module_context();
    m_idHelperTool->cscIdHelper().get_id(*hsit, chId, &otCont);
    chId = m_idHelperTool->chamberId(chId);
    MuonStationIndex::ChIndex chIndex = m_idHelperTool->chamberIndex(chId);
    ATH_MSG_VERBOSE(" chamberlayer  " << MuonStationIndex::chName(chIndex) << "  " << m_idHelperTool->toString(chId) );
  }
}

void MuonSegmentRegionRecoveryTool::fillOnTrackChambers ( const Trk::Track& track, MuonData& data ) const {
  ATH_MSG_VERBOSE(" making set of already-on-track chambers" );

  //Collect track PRD's
  const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
  if ( !states ) {
    ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
    return;
  }

  std::set<MuonStationIndex::StIndex> stations;

  // loop over TSOSs
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
  for ( ; tsit != tsit_end ; ++tsit ) {

    const Trk::TrackParameters* pars = (*tsit)->trackParameters();
    if ( !pars ) continue;

    // check whether state is a measurement
    const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
    if ( !meas ) continue;

    const Muon::MdtDriftCircleOnTrack* mdt = dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(meas);
    if (mdt && !data.mdtTrack.count(mdt->collectionHash()) ) {
      data.mdtTrack.insert( mdt->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(mdt->identify()) << " hash " << mdt->collectionHash() );
      continue;
    }

    const MuonClusterOnTrack* clus = 0;
    const CompetingMuonClustersOnTrack* compRot = dynamic_cast<const CompetingMuonClustersOnTrack*>(meas);
    if ( compRot ) {
      clus = &compRot->rioOnTrack(0);
    } else {
      clus = dynamic_cast<const MuonClusterOnTrack*>(meas);
    }
    if ( !clus ) continue;

    const RpcClusterOnTrack* rpc = dynamic_cast<const RpcClusterOnTrack*>(clus);
    if (rpc  && !data.rpcTrack.count(rpc->collectionHash()) ) {
      data.rpcTrack.insert( rpc->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(clus->identify()) << " hash " << clus->collectionHash() );
      continue;
    }
    const TgcClusterOnTrack* tgc = dynamic_cast<const TgcClusterOnTrack*>(clus);
    if (tgc && !data.tgcTrack.count(tgc->collectionHash()) ) {
      data.tgcTrack.insert( tgc->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(clus->identify()) << " hash " << clus->collectionHash() );
      continue;
    }
    const CscClusterOnTrack* csc = dynamic_cast<const CscClusterOnTrack*>(clus);
    if (csc && !data.cscTrack.count(csc->collectionHash())) {
      data.cscTrack.insert( csc->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(clus->identify()) << " hash " << clus->collectionHash() );
      continue;
    }
    // New Small Wheel
    const sTgcClusterOnTrack* stgc = dynamic_cast<const sTgcClusterOnTrack*>(clus);
    if (stgc && !data.stgcTrack.count(stgc->collectionHash()) ) {
      data.stgcTrack.insert( stgc->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(clus->identify()) << " hash " << clus->collectionHash() );
      continue;
    }
    const MMClusterOnTrack* mm = dynamic_cast<const MMClusterOnTrack*>(clus);
    if (mm && !data.mmTrack.count(mm->collectionHash()) ) {
      data.mmTrack.insert( mm->collectionHash() );
      ATH_MSG_VERBOSE("Adding " << m_idHelperTool->toStringChamber(clus->identify()) << " hash " << clus->collectionHash() );
      continue;
    }
  }
}


const Trk::Track* MuonSegmentRegionRecoveryTool::findHoles( const Trk::Track& track, MuonData& data ) const {

  const DataVector<const Trk::TrackStateOnSurface>* oldStates = track.trackStateOnSurfaces();
  if ( !oldStates ) {
    ATH_MSG_WARNING(" track without states, cannot perform mdt hole search ");
    return 0;
  }

  std::vector<const Trk::TrackStateOnSurface*> states;
  unsigned int nholes = 0;
  std::set<Identifier> chambersInSearch;
  std::set<IdentifierHash>::const_iterator ith = data.mdt.begin();
  std::set<IdentifierHash>::const_iterator ith_end = data.mdt.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.mdtTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->mdtIdHelper().module_context();
    m_idHelperTool->mdtIdHelper().get_id(*ith, chId, &otCont);

    // ignore chambers that already had a hole search
    if ( chambersInSearch.count(chId) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith << " " << m_idHelperTool->toStringChamber(chId) );
      continue;
    }
    const MuonGM::MdtReadoutElement* detEl = m_detMgr->getMdtReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith );
      continue;
    }
    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );

    // calculate crossed tubes
    const MuonStationIntersect& intersect = m_intersectSvc->tubesCrossedByTrack(chId , exPars->position(), exPars->momentum().unit() );

    // clear hole vector
    for ( unsigned int ii = 0; ii < intersect.tubeIntersects().size(); ++ii ) {
      const MuonTubeIntersect& tint = intersect.tubeIntersects()[ii];
      const Identifier& id = tint.tubeId;
      const MuonGM::MdtReadoutElement* detElLoc = m_detMgr->getMdtReadoutElement(id);

      // addition skip for cases when the track crosses inbetween two chambers
      if ( data.mdtTrack.count(detElLoc->collectionHash()) ) continue;

      Identifier ch = m_idHelperTool->chamberId(id);
      chambersInSearch.insert( ch );

      const Trk::Surface& surf = detElLoc->surface(id);
      const Trk::TrackParameters* tubePars = m_extrapolator->extrapolateDirectly(*exPars, surf, Trk::anyDirection, false, Trk::muon);
      if ( !tubePars ) {
        ATH_MSG_WARNING( "Failed to extrapolate to tube " << m_idHelperTool->toString(id) );
        continue;
      }
      int lay = m_idHelperTool->mdtIdHelper().tubeLayer(id);
      int tube = m_idHelperTool->mdtIdHelper().tube(id);
      double tubeLen = detElLoc->getActiveTubeLength(lay, tube);
      double distEdge = fabs(tubePars->parameters()[Trk::locZ]) - 0.5 * tubeLen;
      double pullEdge = tubePars->covariance() ? distEdge / Amg::error(*tubePars->covariance(), Trk::locZ) : distEdge / 20.;
      const Amg::Vector2D* locPos = surf.Trk::Surface::globalToLocal(tubePars->position());
      bool inBounds = false;
      if ( locPos ) {

        // perform bound check do not count holes with 100. mm of bound edge
        inBounds = surf.bounds().insideLoc2(*locPos, -100.);
        if ( inBounds ) {
          if ( fabs( (*locPos)[Trk::locR] ) > 14.4 ) inBounds = false;
        }
        delete locPos;
      }
      if ( !inBounds ) {
        ATH_MSG_VERBOSE(" discarding hole " << m_idHelperTool->toString(id) << " dist wire "
                        << tubePars->parameters()[Trk::locR] << " outside bounds " );
        delete tubePars;
        continue;
      }
      ATH_MSG_VERBOSE(" new hole " << m_idHelperTool->toString(id) << " dist wire " << tubePars->parameters()[Trk::locR]
                      << " dist tube edge " << distEdge << " pullEdge " << pullEdge);
      ++nholes;
      Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createHoleTSOS(tubePars);
      //std::cout << " adding TSOS " << tsos << std::endl;
      states.push_back( tsos );
    }
    if ( !nholes ) ATH_MSG_DEBUG("found holes " << nholes );
    delete exPars;
  }

  ith = data.csc.begin();
  ith_end = data.csc.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.cscTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->cscIdHelper().module_context();
    m_idHelperTool->cscIdHelper().get_id(*ith, chId, &otCont);
    chId = m_idHelperTool->chamberId(chId);

    const MuonGM::CscReadoutElement* detEl = m_detMgr->getCscReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith << " " << m_idHelperTool->toString(chId) );
      continue;
    }

    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    Identifier detElId = m_idHelperTool->detElId( chId );
    std::set<Identifier> layIds;
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > cscstates;
    m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, cscstates );
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator cscit = cscstates.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator cscit_end = cscstates.end();
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toString(chId) << " hash " << *ith << " holes " << cscstates.size() );

    for ( ; cscit != cscit_end; ++cscit ) {
      if ( cscit->first ) {
        ATH_MSG_VERBOSE("Adding " << cscit->second );
        states.push_back( cscit->second );
      }
    }
    delete exPars;
  }
  ith = data.tgc.begin();
  ith_end = data.tgc.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.tgcTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->tgcIdHelper().module_context();
    m_idHelperTool->tgcIdHelper().get_id(*ith, chId, &otCont);
    chId = m_idHelperTool->chamberId(chId);

    const MuonGM::TgcReadoutElement* detEl = m_detMgr->getTgcReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith << " " << m_idHelperTool->toString(chId) );
      continue;
    }

    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    Identifier detElId = m_idHelperTool->detElId( chId );
    std::set<Identifier> layIds;
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > tgcstates;
    m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, tgcstates );
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator tgcit = tgcstates.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator tgcit_end = tgcstates.end();
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toString(chId) << " hash " << *ith << " holes " << tgcstates.size() );

    for ( ; tgcit != tgcit_end; ++tgcit ) {
      if ( tgcit->first ) {
        ATH_MSG_VERBOSE("Adding " << tgcit->second );
        states.push_back( tgcit->second );
      }
    }
    delete exPars;
  }

  ith = data.rpc.begin();
  ith_end = data.rpc.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.rpcTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->rpcIdHelper().module_context();
    m_idHelperTool->rpcIdHelper().get_id(*ith, chId, &otCont);
    chId = m_idHelperTool->chamberId(chId);

    const MuonGM::RpcReadoutElement* detEl = m_detMgr->getRpcReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith << " " << m_idHelperTool->toString(chId) );
      continue;
    }

    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    Identifier detElId = m_idHelperTool->detElId( chId );
    std::set<Identifier> layIds;
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > rpcstates;
    m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, rpcstates );
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator rpcit = rpcstates.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator rpcit_end = rpcstates.end();
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toString(chId) << " hash " << *ith << " holes " << rpcstates.size() );

    for ( ; rpcit != rpcit_end; ++rpcit ) {
      if ( rpcit->first ) {
        ATH_MSG_VERBOSE("Adding " << rpcit->second );
        states.push_back( rpcit->second );
      }
    }
    delete exPars;
  }


  // New Small Wheel
  // sTGC
  ith = data.stgc.begin();
  ith_end = data.stgc.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.stgcTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->stgcIdHelper().module_context();
    m_idHelperTool->stgcIdHelper().get_id(*ith, chId, &otCont);

    if(!chId.is_valid()) {
      ATH_MSG_VERBOSE("invalid chId for stgc data " << *ith );
      continue;
    } 

    chId = m_idHelperTool->chamberId(chId);

    const MuonGM::sTgcReadoutElement* detEl = m_detMgr->getsTgcReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith << " " << m_idHelperTool->toString(chId) );
      continue;
    }

    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    Identifier detElId = m_idHelperTool->detElId( chId );
    std::set<Identifier> layIds;
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > stgcstates;
    m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, stgcstates );
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator stgcit = stgcstates.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator stgcit_end = stgcstates.end();
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toString(chId) << " hash " << *ith << " holes " << stgcstates.size() );

    for ( ; stgcit != stgcit_end; ++stgcit ) {
      if ( stgcit->first ) {
        ATH_MSG_VERBOSE("Adding " << stgcit->second );
        states.push_back( stgcit->second );
      }
    }
    delete exPars;
  }

  // MM
  ith = data.mm.begin();
  ith_end = data.mm.end();
  for ( ; ith != ith_end; ith++ ) {

    // ignore hashes already on track
    if ( data.mmTrack.count(*ith) ) {
      ATH_MSG_VERBOSE("Chamber already on track " << *ith );
      continue;
    }
    Identifier chId;
    IdContext otCont = m_idHelperTool->mmIdHelper().module_context();
    m_idHelperTool->mmIdHelper().get_id(*ith, chId, &otCont);

    if(!chId.is_valid()) {
      ATH_MSG_VERBOSE("invalid chId for mm data " << *ith );
      continue;
    } 

    chId = m_idHelperTool->chamberId(chId);

    const MuonGM::MMReadoutElement* detEl = m_detMgr->getMMReadoutElement(chId);
    if ( !detEl ) {
      ATH_MSG_WARNING("Found no detector element for " << *ith << " " << m_idHelperTool->toString(chId) );
      continue;
    }

    const Trk::TrackParameters* exPars = reachableDetEl(track, *detEl, true );
    if ( !exPars ) {
      ATH_MSG_DEBUG("Did not reach " << m_idHelperTool->toStringChamber(chId) << " hash " << *ith );
      continue;
    }
    //Identifier chId = m_idHelperTool->chamberId(detEl->identify());
    Identifier detElId = m_idHelperTool->detElId( chId );
    std::set<Identifier> layIds;
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > mmstates;
    m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, mmstates );
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator mmit = mmstates.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator mmit_end = mmstates.end();
    ATH_MSG_DEBUG("Reached " << m_idHelperTool->toString(chId) << " hash " << *ith << " holes " << mmstates.size() );

    for ( ; mmit != mmit_end; ++mmit ) {
      if ( mmit->first ) {
        ATH_MSG_VERBOSE("Adding " << mmit->second );
        states.push_back( mmit->second );
      }
    }
    delete exPars;
  }



  if ( !states.empty() ) {
    //std::cout << " adding new states " << states.size() << std::endl;

    // states were added, create a new track
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( oldStates->size() + states.size() );

    std::vector<const Trk::TrackStateOnSurface*> toBeSorted;
    toBeSorted.reserve( oldStates->size() + states.size() );

    DataVector<const Trk::TrackStateOnSurface>::const_iterator sit = oldStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator sit_end = oldStates->end();
    for ( ; sit != sit_end; ++sit ) toBeSorted.push_back( (*sit)->clone() );

    std::vector<const Trk::TrackStateOnSurface*>::iterator sit1 = states.begin();
    std::vector<const Trk::TrackStateOnSurface*>::iterator sit1_end = states.end();
    //for( ;sit1!=sit1_end;++sit1 ) trackStateOnSurfaces->push_back( *sit1 );
    for ( ; sit1 != sit1_end; ++sit1 ) toBeSorted.push_back( *sit1 );

    std::stable_sort(toBeSorted.begin(), toBeSorted.end(), SortTSOSs(&*m_helperTool, &*m_idHelperTool));

    trackStateOnSurfaces->insert(trackStateOnSurfaces->begin(), toBeSorted.begin(), toBeSorted.end());
    Trk::Track* trackWithHoles = new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone() : 0 );
    ATH_MSG_DEBUG("Track with holes " << m_printer->print(*trackWithHoles) << std::endl << m_printer->printStations(*trackWithHoles) );
    return trackWithHoles;
  }
  return 0;
}

const Trk::Track* MuonSegmentRegionRecoveryTool::addMissingChambers( const Trk::Track& track, MuonData& data, bool addMdt ) const {

  // states were added, create a new track
  std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > > states;
  std::set<IdentifierHash> newMdtHashes;
  std::set<IdentifierHash> newRpcHashes;
  std::set<IdentifierHash> newTgcHashes;
  std::set<IdentifierHash> newCscHashes;
  // New Small Wheel
  std::set<IdentifierHash> newMMHashes;
  std::set<IdentifierHash> newsTgcHashes;
  if ( addMdt) {
    ATH_MSG_DEBUG("Adding Missing MDT chambers: regions " << data.mdtPerStation.size() );
    std::map<MuonStationIndex::ChIndex, std::set<IdentifierHash> >::iterator chit = data.mdtPerStation.begin();
    std::map<MuonStationIndex::ChIndex, std::set<IdentifierHash> >::iterator chit_end = data.mdtPerStation.end();
    std::vector<const MdtPrepDataCollection*> newmcols;
    for ( ; chit != chit_end; ++chit ) {
      ATH_MSG_VERBOSE("Region " << MuonStationIndex::chName(chit->first) << " size  " << chit->second.size() );
      std::vector<const MdtPrepDataCollection*> cols;
      m_seededSegmentFinder->extractMdtPrdCols( chit->second, cols );
      std::vector<const MdtPrepDataCollection*>::iterator mit = cols.begin();
      std::vector<const MdtPrepDataCollection*>::iterator mit_end = cols.end();
      std::map<int, std::vector<const MdtPrepData*> > mdtPrds;
      const Trk::TrackParameters* exParsFirst = 0;
      for ( ; mit != mit_end; ++mit ) {
        const Trk::TrackParameters* exPars = reachableDetEl( track, *(*mit)->front()->detectorElement() );
        if ( exPars ) {
          int sector = m_idHelperTool->sector((*mit)->identify());
          ATH_MSG_DEBUG("New chamber " << m_idHelperTool->toStringChamber((*mit)->identify()) << " hash " << (*mit)->identifyHash() << " sector " << sector );
          newmcols.push_back(*mit);
          std::vector<const MdtPrepData*>& col = mdtPrds[sector];
          col.insert(col.end(), (*mit)->begin(), (*mit)->end());
          if ( !exParsFirst ) exParsFirst = exPars;
          else delete exPars;
        } else {
          ATH_MSG_DEBUG("Did not reach chamber " << m_idHelperTool->toStringChamber((*mit)->identify()) << " hash " << (*mit)->identifyHash() );
        }
      }
      std::vector<const MdtPrepData*>* prds = 0;
      std::map<int, std::vector<const MdtPrepData*> >::iterator sectorIt = mdtPrds.begin();
      if ( mdtPrds.empty() ) {
        ATH_MSG_VERBOSE("No hits selected");
      } else if ( mdtPrds.size() == 1 ) {
        prds = &sectorIt->second;
      } else {
        IMuonHitSummaryTool::CompactSummary hitSummary = m_hitSummaryTool->summary(track);
        ATH_MSG_VERBOSE("Multiple sectors selected, using main sector: " << hitSummary.mainSector);
        std::map<int, std::vector<const MdtPrepData*> >::iterator pos = mdtPrds.find(hitSummary.mainSector);
        if ( pos != mdtPrds.end() ) prds = &pos->second;
        else {
          ATH_MSG_DEBUG("Failed to find prds in main sector: " << hitSummary.mainSector);
        }
      }
      if ( prds && exParsFirst) {
        std::unique_ptr<Trk::SegmentCollection> segments = m_seededSegmentFinder->find(*exParsFirst, *prds );
        if ( segments ) {
          if ( !segments->empty() ) ATH_MSG_DEBUG("found segments " << segments->size() );
          Trk::SegmentCollection::iterator sit = segments->begin();
          Trk::SegmentCollection::iterator sit_end = segments->end();

          MuonSegment* bestSegment = 0;
          const Trk::TrackParameters* bestSegmentPars = 0;
          for ( ; sit != sit_end; ++sit ) {

	    Trk::Segment* tseg=*sit;
	    MuonSegment* mseg=dynamic_cast<MuonSegment*>(tseg);

            if ( m_trackSegmentMatchingTool.empty() ) ATH_MSG_VERBOSE("No track/segment matching");
            else if ( !m_trackSegmentMatchingTool->match(track, *mseg, true) ) {
              ATH_MSG_DEBUG(" Segment does not match with track ");
              continue;
            } else {
              ATH_MSG_DEBUG(" Segment/track matched successfully using " << m_trackSegmentMatchingTool );
            }
            const Trk::TrackParameters* segPars = m_extrapolator->extrapolateDirectly(*exParsFirst, mseg->associatedSurface(),
                                                  Trk::anyDirection, false, Trk::muon);
            if ( segPars ) {
              double resy = mseg->localParameters()[Trk::locY] - segPars->parameters()[Trk::locY];
              Trk::LocalDirection locDir;
              mseg->associatedSurface().globalToLocalDirection(segPars->momentum(), locDir);
              double dangleYZ = mseg->localDirection().angleYZ() - locDir.angleYZ();
              ATH_MSG_DEBUG("resy " << resy << " dangleYZ " << dangleYZ << " "  << m_printer->print(*mseg) );
              if ( fabs(dangleYZ) < 0.05 ) {
                bestSegment = mseg;
                if ( bestSegmentPars ) delete bestSegmentPars;
                bestSegmentPars = segPars->clone();
              }
            } else {
              ATH_MSG_DEBUG("Did not reach " << m_printer->print(*mseg) );
            }
            delete segPars;
          }
          if ( bestSegment ) {
            std::vector< const Trk::MeasurementBase* >::const_iterator hit = bestSegment->containedMeasurements().begin();
            std::vector< const Trk::MeasurementBase* >::const_iterator hit_end = bestSegment->containedMeasurements().end();
            for ( ; hit != hit_end; ++hit ) {
              const Trk::TrackParameters* hitPars = m_extrapolator->extrapolateDirectly(*bestSegmentPars, (*hit)->associatedSurface(),
                                                    Trk::anyDirection, false, Trk::muon);
              if ( hitPars ) {
                std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern;
                typePattern.set(Trk::TrackStateOnSurface::Measurement);
                Trk::TrackStateOnSurface* tsos = new Trk::TrackStateOnSurface( (*hit)->clone(), hitPars, 0, 0, typePattern );
                states.push_back( std::make_pair(true, tsos) );
                const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*hit);
                if ( mdt ) newMdtHashes.insert( mdt->collectionHash() );
              } else {
                ATH_MSG_WARNING("Failed to extrapolate to MDT hit ");
              }
            }
          }
          if ( bestSegmentPars ) delete bestSegmentPars;
        }
      }
      delete exParsFirst;
    }
    data.mdtCols = newmcols;
  } else {
    unsigned int nstates = states.size();
    m_seededSegmentFinder->extractRpcPrdCols( data.rpc, data.rpcCols );
    std::vector<const RpcPrepDataCollection*>::iterator rit = data.rpcCols.begin();
    std::vector<const RpcPrepDataCollection*>::iterator rit_end = data.rpcCols.end();
    std::vector<const RpcPrepDataCollection*> newrcols;
    for ( ; rit != rit_end; ++rit ) {
      const Trk::TrackParameters* exPars = reachableDetEl( track, *(*rit)->front()->detectorElement() );
      if ( exPars ) {
        Identifier detElId = m_idHelperTool->detElId( (*rit)->identify() );
        std::set<Identifier> layIds;
        m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, states );
        if ( states.size() != nstates ) {
          nstates = states.size();
          newRpcHashes.insert( (*rit)->identifyHash() );
        }
        delete exPars;
      }
    }
    data.rpcCols = newrcols;

    m_seededSegmentFinder->extractTgcPrdCols( data.tgc, data.tgcCols );
    std::vector<const TgcPrepDataCollection*>::const_iterator tit = data.tgcCols.begin();
    std::vector<const TgcPrepDataCollection*>::const_iterator tit_end = data.tgcCols.end();
    std::vector<const TgcPrepDataCollection*> newtcols;
    for ( ; tit != tit_end; ++tit ) {
      const Trk::TrackParameters* exPars = reachableDetEl( track, *(*tit)->front()->detectorElement() );
      if ( exPars ) {
        newtcols.push_back(*tit);
        Identifier detElId = m_idHelperTool->detElId( (*tit)->identify() );
        std::set<Identifier> layIds;
        m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, states );
        if ( states.size() != nstates ) {
          nstates = states.size();
          newTgcHashes.insert( (*tit)->identifyHash() );
        }

        delete exPars;
      }
    }
    data.tgcCols = newtcols;

    if (!m_doNSW) {
      m_seededSegmentFinder->extractCscPrdCols( data.csc, data.cscCols );
      std::vector<const CscPrepDataCollection*>::const_iterator cit = data.cscCols.begin();
      std::vector<const CscPrepDataCollection*>::const_iterator cit_end = data.cscCols.end();
      std::vector<const CscPrepDataCollection*> newccols;
      for ( ; cit != cit_end; ++cit ) {
        const Trk::TrackParameters* exPars = reachableDetEl( track, *(*cit)->front()->detectorElement() );
        if ( exPars ) {
          newccols.push_back(*cit);
          Identifier detElId = m_idHelperTool->detElId( (*cit)->identify() );
          std::set<Identifier> layIds;
          m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, states );
          if ( states.size() != nstates ) {
            nstates = states.size();
            newCscHashes.insert( (*cit)->identifyHash() );
          }

          delete exPars;
        }
      }
      data.cscCols = newccols;
    }
  }

  if(m_doNSW) {  
  // New Small Wheel
    // sTGC
    unsigned int nstates = states.size();
    m_seededSegmentFinder->extractsTgcPrdCols( data.stgc, data.stgcCols );
    std::vector<const sTgcPrepDataCollection*>::const_iterator stit = data.stgcCols.begin();
    std::vector<const sTgcPrepDataCollection*>::const_iterator stit_end = data.stgcCols.end();
    std::vector<const sTgcPrepDataCollection*> newstcols;
    ATH_MSG_DEBUG(" extractsTgcPrdCols data.stgcCols.size() " << data.stgcCols.size());
    for ( ; stit != stit_end; ++stit ) {
      const Trk::TrackParameters* exPars = reachableDetEl( track, *(*stit)->front()->detectorElement() );
      if ( exPars ) {
        newstcols.push_back(*stit);
        Identifier detElId = m_idHelperTool->detElId( (*stit)->identify() );
        std::set<Identifier> layIds;
        m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, states );
        ATH_MSG_DEBUG("Collected new sTgc states: " << states.size());
        if ( states.size() != nstates ) {
          nstates = states.size();
          newsTgcHashes.insert( (*stit)->identifyHash() );
        }

        delete exPars;
      }
    }
    data.stgcCols = newstcols;

    // MM
    m_seededSegmentFinder->extractMMPrdCols( data.mm, data.mmCols );
    ATH_MSG_DEBUG(" extractMMPrdCols data.mmCols.size() " << data.mmCols.size());
    std::vector<const MMPrepDataCollection*>::const_iterator mit = data.mmCols.begin();
    std::vector<const MMPrepDataCollection*>::const_iterator mit_end = data.mmCols.end();
    std::vector<const MMPrepDataCollection*> newmcols;
    for ( ; mit != mit_end; ++mit ) {
      const Trk::TrackParameters* exPars = reachableDetEl( track, *(*mit)->front()->detectorElement() );
      if ( exPars ) {
        newmcols.push_back(*mit);
        Identifier detElId = m_idHelperTool->detElId( (*mit)->identify() );
        std::set<Identifier> layIds;
        m_chamberHoleRecoveryTool->createHoleTSOSsForClusterChamber( detElId, *exPars, layIds, states );
        ATH_MSG_DEBUG("Collected new Mm states: " << states.size());
        if ( states.size() != nstates ) {
          nstates = states.size();
          newMMHashes.insert( (*mit)->identifyHash() );
        }

        delete exPars;
      }
    }
    data.mmCols = newmcols;

  }

  if ( !states.empty() ) {
    ATH_MSG_DEBUG("Collected new states: " << states.size());

    const DataVector<const Trk::TrackStateOnSurface>* oldStates = track.trackStateOnSurfaces();
    if ( !oldStates ) {
      ATH_MSG_WARNING(" track without states, cannot perform cleaning ");
      std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator nit = states.begin();
      std::vector< std::pair<bool, const Trk::TrackStateOnSurface* > >::iterator nit_end = states.end();
      for ( ; nit != nit_end; ++nit ) {
        if ( nit->first ) delete nit->second;
      }
      return 0;
    }


    // states were added, create a new track
    std::vector<const Trk::TrackStateOnSurface*> newStates;
    newStates.reserve( oldStates->size() + states.size() );

    ATH_MSG_DEBUG("Copying old TSOSs " << oldStates->size());
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = oldStates->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = oldStates->end();
    for ( ; tsit != tsit_end; ++tsit ) newStates.push_back( (*tsit)->clone() );


    ATH_MSG_DEBUG("Adding new TSOSs " << states.size() << "  current size " << newStates.size());
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface*> >::iterator nit = states.begin();
    std::vector< std::pair<bool, const Trk::TrackStateOnSurface*> >::iterator nit_end = states.end();
    for ( ; nit != nit_end; ++nit ) {
      // add states. If nit->first is true we have a new state. If it is false the state is from the old track and has to be cloned
      newStates.push_back( nit->first ? nit->second : nit->second->clone() );
    }
    std::stable_sort(newStates.begin(), newStates.end(), SortTSOSs(&*m_helperTool, &*m_idHelperTool));

    ATH_MSG_DEBUG("Filling DataVector with TSOSs " << newStates.size());
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( newStates.size() );
    trackStateOnSurfaces->insert( trackStateOnSurfaces->begin(), newStates.begin(), newStates.end() );

    ATH_MSG_DEBUG("Creating new Track " << newStates.size());
    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone() : 0 );
    const Trk::Track* refittedTrack = m_fitter->fit(*newTrack, m_useFitterOutlierLogic, Trk::muon);
    delete newTrack;
    if ( refittedTrack ) {
      ATH_MSG_DEBUG("New Track " << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack) );

      // fit ok, add new hashs to lists
      data.mdtTrack.insert(newMdtHashes.begin(), newMdtHashes.end());
      data.rpcTrack.insert(newRpcHashes.begin(), newRpcHashes.end());
      data.tgcTrack.insert(newTgcHashes.begin(), newTgcHashes.end());
      data.cscTrack.insert(newCscHashes.begin(), newCscHashes.end());
      // New Small Wheel
      data.stgcTrack.insert(newsTgcHashes.begin(), newsTgcHashes.end());
      data.mmTrack.insert(newMMHashes.begin(), newMMHashes.end());
    }
    return refittedTrack;
  }
  return 0;
  //delete newStates;
}

const Trk::TrackParameters* MuonSegmentRegionRecoveryTool::reachableDetEl( const Trk::Track& track, const Trk::TrkDetElementBase& detEl, bool smallerBounds ) const {

  ATH_MSG_VERBOSE( "reachableDetEl() " << m_idHelperTool->toStringDetEl( detEl.identify() ) << " at " << detEl.center() );
  const Trk::TrackParameters* exPars = 0;
  Trk::TrackParameters* closest = MuonGetClosestParameters::closestParameters(track, detEl.surface(), true);
  if ( closest ) {
    if ( msgLvl(MSG::VERBOSE) ) {
      ATH_MSG_VERBOSE( "Extrapolating from closest point:\n" << m_printer->print( *closest ) );
    }
    exPars = m_extrapolator->extrapolateDirectly(*closest, detEl.surface(), Trk::anyDirection, false, Trk::muon);
    delete closest;
  } else {
    ATH_MSG_VERBOSE( "Extrapolating from track (no closest point found):\n" << m_printer->print( track ) );
    exPars = m_extrapolator->extrapolate(track, detEl.surface(), Trk::anyDirection, false, Trk::muon);
  }
  if ( !exPars ) return 0;
  Amg::Vector2D locPos( exPars->parameters()[Trk::locX], exPars->parameters()[Trk::locY] );

  double tolx = 100.; // positive -> large surface
  double toly = 100.; // positive -> large surface
  const AmgSymMatrix(5)* errMat = exPars->covariance();
  if ( errMat ) {
    ATH_MSG_VERBOSE( "Extrapolated local Position & Error matrix\n:" << locPos << "\n" << Amg::toString(*errMat) );
    // MJW check validity of errors before using them (fix for bug #82920)
    double covx = (*errMat)(Trk::locX, Trk::locX);
    double covy = (*errMat)(Trk::locY, Trk::locY);
    if ( covx > 0.0 && covy > 0.0 ) {
      tolx = 3 * covx ;
      toly = 3 * covy;
    }
  }

  // in case we are looking for holes, only ignore stations if we are within 100 mm of the chamber edge
  // all other holes are counted even if the extrapolation errors are large.
  if ( smallerBounds ) {
    if ( tolx > 10. ) tolx = 10.;
    if ( toly > 10. ) toly = 10.;
    // refer sign to check 'inside' the bounds
    tolx *= -1.;
    toly *= -1.;
  }
  bool inbounds = detEl.surface().insideBounds(locPos, tolx, toly);

  if ( msgLvl(MSG::DEBUG) ) {

    std::ostringstream parsType;
    parsType << "pos=(" << locPos[Trk::locX]
             << "," << locPos[Trk::locY] << ")";
    if ( errMat ) {
      parsType << "  exError=(" << Amg::error(*errMat, Trk::locX) << "," << Amg::error(*errMat, Trk::locY) << ")";
    }
    parsType << "  tolerance=(" << tolx << "," << toly << ")";
    if ( inbounds ) parsType << " => inbounds";
    else           parsType << " => outbounds";
    ATH_MSG_DEBUG(" " << m_idHelperTool->toStringChamber( detEl.identify() ) << " pars  " << parsType.str()  );
  }
  if ( !inbounds ) {
    delete exPars;
    return 0;
  }

  return exPars;
}


}

