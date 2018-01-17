/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <algorithm>
#include <iostream>

// FrameWork includes
//#include "CLHEP/Units/SystemOfUnits.h"
#include "AthenaKernel/errorcheck.h"

//Collections 
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkEventPrimitives/LocalDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkTrack/AlignmentEffectsOnTrack.h"
//Actual Class 
#include "MuonRefitTool.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonSegmentMakerInterfaces/IDCSLFitProvider.h"


#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonTrackExtrapolationTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonClusterOnTrackCreator.h"
#include "MuonRecToolInterfaces/IMuonCompetingClustersOnTrackCreator.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkExInterfaces/IExtrapolator.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

#include "MuonSegmentMakerUtils/MuonSegmentKey.h"
#include "MuonSegmentMakerUtils/CompareMuonSegmentKeys.h"
#include "MuonTrackMakerUtils/MuonTSOSHelper.h"

#include "EventPrimitives/EventPrimitives.h"
#include "EventPrimitives/EventPrimitivesHelpers.h"

#include "MuonAlignErrorBase/AlignmentTranslationDeviation.h"
#include "MuonAlignErrorBase/AlignmentRotationDeviation.h"
#include "TrkToolInterfaces/ITrkAlignmentDeviationTool.h"

namespace Muon {

  MuonRefitTool::MuonRefitTool( const std::string& ty,const std::string& na,const IInterface* pa) : 
    AthAlgTool(ty,na,pa),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"), 
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_alignErrorTool("MuonAlign::AlignmentErrorTool"),
    m_trackFitter("Trk::GlobalChi2Fitter/MCTBFitterMaterialFromTrack"),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_mdtRotCreator("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
    m_cscRotCreator("Muon::CscClusterOnTrackCreator/CscClusterOnTrackCreator"),
    m_triggerRotCreator("Muon::MuonClusterOnTrackCreator/MuonClusterOnTrackCreator"),
    m_compClusterCreator("Muon::TriggerChamberClusterOnTrackCreator/TriggerChamberClusterOnTrackCreator"),
    m_t0Fitter(""),
    m_muonEntryTrackExtrapolator("Muon::MuonTrackExtrapolationTool/MuonTrackExtrapolationTool"),
    m_errorStrategyBEE(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyEE(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyBIS78(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyBXE(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyEEL1C05(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyBarEnd(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategySL(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategyTwoStations(MuonDriftCircleErrorStrategyInput()),
    m_errorStrategy(MuonDriftCircleErrorStrategyInput()),
    m_muonErrorStrategy(MuonDriftCircleErrorStrategyInput()),
    m_finder(5.,3.,false),
    m_finderCleaning(5.,3.,false),
    m_nrefits(0),
    m_ngoodRefits(0),
    m_failedErrorUpdateOutlierRemoval(0),
    m_failedOutlierRemoval(0),
    m_failedErrorUpdate(0),
    m_failedRefit(0),
    m_failedExtrapolationMuonEntry(0),
    m_nbackExtrapolations(0),
    m_ngoodBackExtrapolations(0),
    m_failedExtrapolationIP(0),
    m_nupdates(0),
    m_ngoodUpdates(0),
    m_failedUpdates(0),
    m_alignmentDelta(0.),
    m_alignmentAngle(0.),
    m_alignmentDeltaError(0.1),
    m_alignmentAngleError(0.001),
    m_alignmentErrors(false),
    m_simpleAEOTs(false),
    m_addAll(false),
    m_addInner(false),
    m_addMiddle(true),
    m_addTwo(false)
  {
    declareInterface<IMuonRefitTool>(this);
    declareProperty("FinderDebugLevel",m_finderDebugLevel = -1 );
    declareProperty("Fitter",m_trackFitter);
    declareProperty("T0Fitter",m_t0Fitter);
    declareProperty("MinMuonMom",m_minMuonMomentum = 4000.);
    declareProperty("UsedFixedError",m_fixedError = 1. );
    declareProperty("FlagT0FitRange",m_flagT0FitRange = 0.00005 );
    declareProperty("Printer", m_printer );
    declareProperty("Helper", m_helper );
    declareProperty("IdHelper", m_idHelper );
    declareProperty("AlignmentErrorTool", m_alignErrorTool);
    declareProperty("Extrapolator", m_extrapolator );
    declareProperty("MuonEntryExtrapolationTool", m_muonEntryTrackExtrapolator );
    declareProperty("MdtRotCreator",m_mdtRotCreator);
    declareProperty("CscRotCreator",m_cscRotCreator);
    declareProperty("TriggerRotCreator",m_triggerRotCreator);
    declareProperty("DeweightBEE", m_deweightBEE = false );
    declareProperty("DeweightEE",  m_deweightEE = false );
    declareProperty("DeweightBIS78", m_deweightBIS78 = true );
    declareProperty("DeweightBME", m_deweightBME = true );
    declareProperty("DeweightBOE", m_deweightBOE = true );
    declareProperty("DeweightEEL1C05", m_deweightEEL1C05 = false );
    declareProperty("DeweightTwoStationTracks",  m_deweightTwoStationTracks = false );
    declareProperty("AlignmentDelta", m_alignmentDelta = 0.);
    declareProperty("AlignmentAngle", m_alignmentAngle = 0.);
    declareProperty("AlignmentDeltaError", m_alignmentDeltaError = 0.1);
    declareProperty("AlignmentAngleError", m_alignmentAngleError = 0.001);
    declareProperty("AlignmentErrors", m_alignmentErrors = false);
    declareProperty("SimpleAEOTs", m_simpleAEOTs = false);
    declareProperty("AddAll", m_addAll = false);
    declareProperty("AddInner", m_addInner = false);
    declareProperty("AddMiddle", m_addMiddle = false);
    declareProperty("AddTwo", m_addTwo = false);
  }

  MuonRefitTool::~MuonRefitTool()
  {

  }

  StatusCode MuonRefitTool::initialize()
  {

    ATH_MSG_INFO( "Initializing MuonRefitTool" );

    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_helper.retrieve() );
    ATH_CHECK( m_idHelper.retrieve() );
    if( !m_alignErrorTool.empty() ) ATH_CHECK(m_alignErrorTool.retrieve());
    ATH_CHECK( m_extrapolator.retrieve() );
    ATH_CHECK( m_trackFitter.retrieve() );

    ATH_MSG_INFO("Retrieved " << m_trackFitter );

    ATH_CHECK( m_mdtRotCreator.retrieve() );
    if( ! m_cscRotCreator.empty() ) ATH_CHECK( m_cscRotCreator.retrieve() );
    if( ! m_triggerRotCreator.empty() ) ATH_CHECK( m_triggerRotCreator.retrieve() );
    if( !m_compClusterCreator.empty() ) ATH_CHECK( m_compClusterCreator.retrieve() );

    if( !m_t0Fitter.empty() ){
      ATH_CHECK( m_t0Fitter.retrieve() );
      ATH_MSG_INFO("Retrieved " << m_t0Fitter );
      m_finder.setFitter(m_t0Fitter->getFitter());
    }

    m_finder.debugLevel(m_finderDebugLevel);
    m_finder.setRecoverMDT(false);
    m_finderCleaning.debugLevel(m_finderDebugLevel);

    ATH_CHECK( m_muonEntryTrackExtrapolator.retrieve() );

    MuonDriftCircleErrorStrategyInput bits;
    MuonDriftCircleErrorStrategy strategy(bits);
    strategy.setParameter(MuonDriftCircleErrorStrategy::BroadError,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::ScaledError,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::FixedError,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::ParameterisedErrors,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::StationError,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::ErrorAtPredictedPosition,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::T0Refit,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::WireSagGeomCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::TofCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::PropCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::TempCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::MagFieldCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::WireSagTimeCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::SlewCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::BackgroundCorrection,false);
    strategy.setParameter(MuonDriftCircleErrorStrategy::Segment,false);

    m_errorStrategyBEE = strategy;
    m_errorStrategyBEE.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategyTwoStations = strategy;
    m_errorStrategyTwoStations.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategyEE = strategy;
    m_errorStrategyEE.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategyBIS78 = strategy;
    m_errorStrategyBIS78.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategyBXE = strategy;
    m_errorStrategyBXE.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategyEEL1C05 = strategy;
    m_errorStrategyEEL1C05.setParameter(MuonDriftCircleErrorStrategy::StationError,true);

    m_errorStrategySL = strategy;
    m_errorStrategySL.setParameter(MuonDriftCircleErrorStrategy::FixedError,true);
    m_errorStrategySL.setParameter(MuonDriftCircleErrorStrategy::BroadError,false);

    m_errorStrategyBarEnd = strategy;
    m_errorStrategyBarEnd.setParameter(MuonDriftCircleErrorStrategy::FixedError,true);
    m_errorStrategyBarEnd.setParameter(MuonDriftCircleErrorStrategy::BroadError,true);

    m_errorStrategy = strategy;
    m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::ScaledError,true);
    m_errorStrategy.setParameter(MuonDriftCircleErrorStrategy::BroadError,false);

    m_muonErrorStrategy.setStrategy(MuonDriftCircleErrorStrategy::Muon);
    m_muonErrorStrategy.setParameter(MuonDriftCircleErrorStrategy::ScaledError,true);
    m_muonErrorStrategy.setParameter(MuonDriftCircleErrorStrategy::BroadError,false);


    msg(MSG::INFO) << MSG::INFO << " Options: ";
    if( m_deweightBEE )                  msg(MSG::INFO) << MSG::INFO << " Deweight BEE";
    if( m_deweightEE )                   msg(MSG::INFO) << MSG::INFO << " Deweight EE";
    if( m_deweightBIS78 )                msg(MSG::INFO) << MSG::INFO << " Deweight BIS78";
    if( m_deweightBME )                  msg(MSG::INFO) << MSG::INFO << " Deweight BME";
    if( m_deweightBOE )                  msg(MSG::INFO) << MSG::INFO << " Deweight BOE";
    if( m_deweightEEL1C05 )              msg(MSG::INFO) << MSG::INFO << " Deweight EEL1C05";
    if( m_deweightTwoStationTracks )     msg(MSG::INFO) << MSG::INFO << " Deweight Two stations";
    msg(MSG::INFO) << endmsg;

    return StatusCode::SUCCESS;
  }


  StatusCode MuonRefitTool::finalize()
  {
    double scaleRefit = m_nrefits != 0 ? 1./(double)m_nrefits : 1.;
    double scaleExtr = m_nbackExtrapolations != 0 ? 1./(double)m_nbackExtrapolations : 1.;
    double scaleUp = m_nupdates != 0 ? 1./(double)m_nupdates : 1.;
    ATH_MSG_INFO(   "Number of refits                        " << m_nrefits << endmsg 
		    << "Good                                    " << scaleRefit*m_ngoodRefits << endmsg
		    << "Failed Error update in outlier removal  " << scaleRefit*m_failedErrorUpdateOutlierRemoval << endmsg
		    << "Failed Outlier removal                  " << scaleRefit*m_failedOutlierRemoval << endmsg
		    << "Failed Error Update                     " << scaleRefit*m_failedErrorUpdate << endmsg
		    << "Failed Refit                            " << scaleRefit*m_failedRefit << endmsg
		    << "Failed Extrapolation to Muon Entry      " << scaleRefit*m_failedExtrapolationMuonEntry);
    ATH_MSG_INFO(   "Number of extrapolations to IP   " << m_nbackExtrapolations << endmsg 
		    << "Good                             " << scaleExtr*m_ngoodBackExtrapolations << endmsg
		    << "Failed Extrapolation to IP       "  << scaleExtr*m_failedExtrapolationIP);

    ATH_MSG_INFO(   "Number of updated tracks   " << m_nupdates << endmsg 
		    << "Good                       " << scaleUp*m_ngoodUpdates << endmsg
		    << "Failed updates             "  << scaleUp*m_failedUpdates);
  
    return StatusCode::SUCCESS;
  }




  const Trk::Track* MuonRefitTool::refit( const Trk::Track& track, const IMuonRefitTool::Settings* set ) const {

    const IMuonRefitTool::Settings& settings = set ? *set : m_defaultSettings;

    ++m_nrefits;
    const Trk::Track* newTrack = &track;
    if( settings.removeOutliers ){
      
      const Trk::Track* cleanedTrack = removeOutliers(*newTrack,settings);
      if( !cleanedTrack ){
	ATH_MSG_DEBUG("Track lost during outlier removal");
	++m_failedOutlierRemoval;
	return newTrack;
      }
      if( cleanedTrack != newTrack ){
	ATH_MSG_DEBUG("Outlier removal removed hits from track");
	if( newTrack != &track ) delete newTrack;
      }      
      newTrack = cleanedTrack;
    }
  
    if( settings.updateErrors ){
      Trk::Track* updateErrorTrack  = m_alignmentErrors ? updateAlignmentErrors(*newTrack,settings)  : updateErrors(*newTrack,settings);
      if( !updateErrorTrack ) {
	ATH_MSG_WARNING("Failed to update errors");
	++m_failedErrorUpdate;
	return newTrack;
      }
      if( updateErrorTrack != newTrack && newTrack != &track ) delete newTrack;
      newTrack = updateErrorTrack;
    }
  
    if( settings.refit ){
      ATH_MSG_DEBUG("Original track" << m_printer->print(*&track) );

// do not put AEOTs on extremely bad chi2 tracks and do not refit them 

      Trk::Track* refittedTrack = track.fitQuality()&&track.fitQuality()->chiSquared()<10000*track.fitQuality()->numberDoF() ? m_trackFitter->fit(*newTrack,false,Trk::muon) : 0 ;
      if( !refittedTrack ){
	ATH_MSG_DEBUG("Failed to refit track");
	++m_failedRefit;
// BUG fix Peter 
	delete newTrack;
        newTrack = &track;
	return newTrack;
      }
      ATH_MSG_DEBUG("Refitted track" << m_printer->print(*refittedTrack) );
      ATH_MSG_DEBUG("Refitted track" << m_printer->printMeasurements(*refittedTrack) );
      if( refittedTrack != newTrack && newTrack != &track ) delete newTrack;
      newTrack = refittedTrack;
    }
    
    if( settings.extrapolateToMuonEntry ){
      Trk::Track* extrapolatedTrack = m_muonEntryTrackExtrapolator->extrapolate(*newTrack);
      if( !extrapolatedTrack ){
	ATH_MSG_WARNING("Failed to back-extrapolate track");
	++m_failedExtrapolationMuonEntry;
	return newTrack;
      }
      ATH_MSG_DEBUG("Entry track " << m_printer->print(*extrapolatedTrack) );
      if( extrapolatedTrack != newTrack && newTrack != &track ) delete newTrack;
      newTrack = extrapolatedTrack;
    }
    ++m_ngoodRefits;
  
    return newTrack;
  }

  std::vector<const Trk::Track*> MuonRefitTool::refit( const std::vector<const Trk::Track*>& tracks, const IMuonRefitTool::Settings* set ) const {


    std::vector<const Trk::Track*> refittedTracks;
    refittedTracks.reserve(tracks.size());

    std::vector<const Trk::Track*>::const_iterator it = tracks.begin();
    std::vector<const Trk::Track*>::const_iterator it_end = tracks.end();
    for( ;it!=it_end;++it ){
      const Trk::Track* refittedTrack = refit(**it,set);
      if( refittedTrack == *it ) refittedTrack = new Trk::Track(**it);

      if(refittedTrack) refittedTracks.push_back(refittedTrack);
    }

    return refittedTracks;
  }

  Trk::Track* MuonRefitTool::updateAlignmentErrors( const Trk::Track& track, const IMuonRefitTool::Settings& settings ) const {


// first scale the Mdt errors 

   const Trk::Track* inputTrack = &track;
   Trk::Track* updatedTrack = updateMdtErrors(*inputTrack,settings);

   Trk::Track* updatedAEOTsTrack = m_simpleAEOTs ? makeSimpleAEOTs(*updatedTrack) : makeAEOTs(*updatedTrack);
   if( updatedAEOTsTrack != updatedTrack ) delete updatedTrack;

   return updatedAEOTsTrack;
  }

  Trk::Track* MuonRefitTool::makeAEOTs( const Trk::Track& track ) const {

//   
// use the new AlignmentEffectsOnTrack class and alignmentErrorTool
//
    if( m_alignErrorTool.empty() ) {
      Trk::Track* newTrack =  new Trk::Track( track );
      return newTrack;
    } 
//
// Use the alignmentErrorTool and store a list of hits with error on position and angle 
//
    std::map< std::vector<Identifier>, std::pair <double, double> > alignerrmap;

      std::vector<Trk::AlignmentDeviation*> align_deviations;
      m_alignErrorTool->makeAlignmentDeviations(track, align_deviations);

      int iok = 0;
      bool isSmallChamber = false;
      bool isLargeChamber = false;
      bool isEndcap = false;
      bool isBarrel = false;
      std::vector <int> usedRotations;

      // loop on deviations
      for(auto it : align_deviations){
         double angleError  = 0.;
         double translationError  = 0.;
         bool differentChambers = false;
         int jdifferent = -1;
         isSmallChamber = false;
         isLargeChamber = false;
         isEndcap = false;
         isBarrel = false;

         if( dynamic_cast<MuonAlign::AlignmentTranslationDeviation*>(it) ) {
           translationError = sqrt(it->getCovariance(0,0));
           // vector to store hit id
           std::vector<Identifier> hitids;
           std::vector<const Trk::RIO_OnTrack*> vec_riowithdev;
           it->getListOfHits(vec_riowithdev);
           // bool to decide if deviation should be skipped (if it's for more than 1 station)
           for(auto riowithdev : vec_riowithdev){
              Identifier id_riowithdev = riowithdev->identify();
              if(m_idHelper->isEndcap(id_riowithdev)) {
                 isEndcap = true;
              } else {
                 isBarrel = true;
              }
              if(m_idHelper->isSmallChamber(id_riowithdev)) {
                isSmallChamber = true;
              } else {
                isLargeChamber = true;
              }
              hitids.push_back(id_riowithdev);
              if( hitids.size()>1 &&  m_idHelper->chamberId(id_riowithdev) !=  m_idHelper->chamberId(hitids[0]) ) {
                differentChambers = true;
                jdifferent = hitids.size()-1; 
              } 
           }
           bool matchFound = false;
           if( hitids.size()>0) {
             int iRot = -1;
             for(auto itRot : align_deviations){
               iRot++;
               if( dynamic_cast<MuonAlign::AlignmentRotationDeviation*>(itRot) ) {
                 std::vector<Identifier> hitidsRot;
                 itRot->getListOfHits(vec_riowithdev);
                 for(auto riowithdev : vec_riowithdev){
                   Identifier id_riowithdev = riowithdev->identify();
                   hitidsRot.push_back(id_riowithdev);
                   if(id_riowithdev==hitids[0]) {
                     angleError = sqrt(itRot->getCovariance(0,0));
                     matchFound = true;
                     break;
                   }
                 }
                 if(matchFound) usedRotations.push_back(iRot);
               }
               if(matchFound) break; 
             }
           }
           // if deviation is accepted (i.e. only on one station) store the hit IDs associated with the deviation and the error

// store (all) translationError with or without a matched angleError 
           iok++;  
           alignerrmap.insert( std::pair < std::vector<Identifier>, std::pair < double, double > > ( hitids, std::pair < double, double > (translationError,angleError) ) );

           if(matchFound) ATH_MSG_DEBUG(" AlignmentMap entry " <<  iok  << " filled with nr hitids " << hitids.size() << " " << m_idHelper->toString(hitids[0]) <<  " translationError " << translationError << " angleError " << angleError );
           if(!matchFound) ATH_MSG_DEBUG(" AlignmentMap entry No angleError" <<  iok  << " filled with nr hitids " << hitids.size() << " " << m_idHelper->toString(hitids[0]) <<  " translationError " << translationError << " angleError " << angleError );
           if(isEndcap) ATH_MSG_DEBUG(" AlignmentMap Endcap Chamber "); 
           if(isBarrel) ATH_MSG_DEBUG(" AlignmentMap Barrel Chamber "); 
           if(isSmallChamber) ATH_MSG_DEBUG(" AlignmentMap Small Chamber ");
           if(isLargeChamber) ATH_MSG_DEBUG(" AlignmentMap Large Chamber ");
           if(differentChambers) ATH_MSG_DEBUG(" AlignmentMap entry " <<  iok  << " for different Chamber " <<  m_idHelper->toString(hitids[jdifferent]) );
        }
      }

// now add the angleErrors that were NOT matched to a translationError

      int iRot = -1;
      for(auto itRot : align_deviations){
         iRot++;
         isSmallChamber = false;
         isLargeChamber = false;
         isEndcap = false;
         isBarrel = false;
         if( dynamic_cast<MuonAlign::AlignmentRotationDeviation*>(itRot) ) {
           bool used = false;
           for (unsigned int i = 0; i < usedRotations.size(); i++) {
            if(iRot == usedRotations[i]) used = true;
           }
           if(used) continue;             
           std::vector<const Trk::RIO_OnTrack*> vec_riowithdev;
           itRot->getListOfHits(vec_riowithdev);

           std::vector<Identifier> hitids;
           // bool to decide if deviation should be skipped (if it's for more than 1 station)
           for(auto riowithdev : vec_riowithdev){
              Identifier id_riowithdev = riowithdev->identify();
              if(m_idHelper->isEndcap(id_riowithdev)) {
                 isEndcap = true;
              } else {
                 isBarrel = true;
              }
              if(m_idHelper->isSmallChamber(id_riowithdev)) {
                isSmallChamber = true;
              } else {
                isLargeChamber = true;
              }
              hitids.push_back(id_riowithdev);
           }

           double translationError = 0.;
           double angleError = sqrt(itRot->getCovariance(0,0));

           iok++;  
           alignerrmap.insert( std::pair < std::vector<Identifier>, std::pair < double, double > > ( hitids, std::pair < double, double > (translationError,angleError) ) );
           ATH_MSG_DEBUG(" AlignmentMap entry No Translation Error " <<  iok  << " filled with nr hitids " << hitids.size() << " " << m_idHelper->toString(hitids[0]) <<  " translationError " << translationError << " angleError " << angleError );
          if(isEndcap) ATH_MSG_DEBUG(" AlignmentMap Endcap Chamber"); 
          if(isBarrel) ATH_MSG_DEBUG(" AlignmentMap Barrel Chamber"); 
          if(isSmallChamber) ATH_MSG_DEBUG(" AlignmentMap Small Chamber ");
          if(isLargeChamber) ATH_MSG_DEBUG(" AlignmentMap Large Chamber ");
         }
      }

      // clean-up of alignment deviations
      for(auto it : align_deviations) delete it;
      align_deviations.clear();

    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Alignment);

    std::vector <int> indexAEOTs;
    std::vector <Trk::TrackStateOnSurface*> tsosAEOTs;

    ATH_MSG_DEBUG(" AlignmentMap size " << alignerrmap.size()); 

    std::set<MuonStationIndex::ChIndex> stationIds;
 
    for(auto itAli : alignerrmap){
      unsigned int imiddle = (itAli.first.size())/2;
      Identifier idMiddle = itAli.first[imiddle];
      tsit     = states->begin();
      int index = -1;
      bool found = false;
      for( ;tsit!=tsit_end ; ++tsit ){
        index++;
        const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
        if( !meas ) {
          continue;
        }
	if(dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(meas)) continue; //exclude pseudos
//        if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
        Identifier id = m_helper->getIdentifier(*meas);
	//std::cout<<"measurement: "<<*meas<<std::endl;

        if( m_idHelper->isMdt(id) ) stationIds.insert( m_idHelper->chamberIndex(id) );

// make Alignment Effect using the surface of the TSOS 

        if(idMiddle==id) { 
          double deltaError = itAli.second.first;   
          double angleError = itAli.second.second; 
          if(deltaError<0.01) deltaError = 0.01;
          if(angleError<0.000001) angleError = 0.000001;
          Trk::AlignmentEffectsOnTrack* aEOT = new Trk::AlignmentEffectsOnTrack(0.,deltaError,0.,angleError,itAli.first,&((*tsit)->measurementOnTrack()->associatedSurface())); 
          Trk::TrackStateOnSurface* tsosAEOT = new Trk::TrackStateOnSurface(0,(*tsit)->trackParameters()->clone(),0,0,typePattern,aEOT); 
          indexAEOTs.push_back(index);
          tsosAEOTs.push_back(tsosAEOT);
          found = true;
          break;
        }
      }
      if(!found) ATH_MSG_WARNING(" This should not happen Identifier from AlignmentErrorTool is not found");
    }
    
//
// clone the TSOSs and add the tsosAEOTs
//
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve(states->size()+indexAEOTs.size());
    tsit     = states->begin();
    int index = -1;
    for( ;tsit!=tsit_end ; ++tsit ){
      index++;
      for(unsigned int i = 0; i<indexAEOTs.size(); i++){
        if(index==indexAEOTs[i]) {
          trackStateOnSurfaces->push_back(tsosAEOTs[i]);
        }
      }
// skip outliers
//      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      trackStateOnSurfaces->push_back( (*tsit)->clone());
    }

    if(indexAEOTs.size()==0 && stationIds.size() > 1) ATH_MSG_WARNING(" Track without AEOT ");

    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );

// dump it
//    const DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfacesNew = newTrack->trackStateOnSurfaces();
//    tsit     = trackStateOnSurfacesNew->begin();
//    tsit_end = trackStateOnSurfacesNew->end();
//    for( ; tsit!=tsit_end ; ++tsit ){
//        if((*tsit)->alignmentEffectsOnTrack()) std::cout << " Peter alignmentEffectsOnTrack found with track pars " << (*tsit)->trackParameters() << std::endl;
//    }

    ATH_MSG_DEBUG(m_printer->print(*newTrack));
    ATH_MSG_DEBUG(m_printer->printMeasurements(*newTrack));

    return newTrack;

  }

  Trk::Track* MuonRefitTool::makeSimpleAEOTs( const Trk::Track& track ) const {


// use the new AlignmentEffectsOnTrack class
   
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }

    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
//
// first clone the TSOSs
//
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve(states->size()+1);
    for( ;tsit!=tsit_end ; ++tsit ){
      trackStateOnSurfaces->push_back( (*tsit)->clone() );
    }


// loop over TSOSs and look for EM or BM chambers 
    tsit     = trackStateOnSurfaces->begin();
    tsit_end = trackStateOnSurfaces->end();
    std::vector<const Trk::TrackStateOnSurface*> indicesOfAffectedTSOS;
    std::vector<const Trk::TrackStateOnSurface*> indicesOfAffectedTSOSInner;    
    std::vector<Identifier> indicesOfAffectedIds;
    std::vector<Identifier> indicesOfAffectedIdsInner;    
    int index = -1;
    int indexFirst = -1;
    int indexFirstInner = -1;
//    const Trk::Surface *surf = 0;
    for( ; tsit!=tsit_end ; ++tsit ){
      index++;      
      if( !*tsit ) continue; //sanity check

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) continue;
    
      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	continue;
      }

      // skip outliers
      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      if( (*tsit)->alignmentEffectsOnTrack()) {
         ATH_MSG_WARNING(" AlignmentEffectOnTrack is already on track skip it");   
        continue;
      } 
      Identifier id = m_helper->getIdentifier(*meas);
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ) continue;
      MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
// skip phi measurements  
      if( (m_idHelper->isTrigger(id)&&m_idHelper->measuresPhi(id)) || (m_idHelper->isCsc(id)&&m_idHelper->measuresPhi(id) ) ) continue;
      if(m_addAll) {
// skip RPC and TGC eta (to avoid code crashes) 
        if( m_idHelper->isTrigger(id)) continue;
//        if( m_idHelper->isTrigger(id) && (stIndex == MuonStationIndex::BM || stIndex == MuonStationIndex::BO) ) continue;
        if(indexFirst==-1) indexFirst = index;
        indicesOfAffectedTSOS.push_back(*tsit);  
        indicesOfAffectedIds.push_back(id);  
      } else {
// skip trigger hits and CSC phi measurements  and select precision hits
        if( m_idHelper->isTrigger(id)) continue;
        if( stIndex == MuonStationIndex::BM || stIndex == MuonStationIndex::EM) {
          if(indexFirst==-1) indexFirst = index;
          indicesOfAffectedTSOS.push_back(*tsit);  
          indicesOfAffectedIds.push_back(id);  
//  two alignment discontinuities   
          if(m_addTwo) {
            if(indexFirstInner==-1) indexFirstInner = index;
            indicesOfAffectedTSOSInner.push_back(*tsit);  
            indicesOfAffectedIdsInner.push_back(id);  
          }  
        }  
//        if( stIndex == MuonStationIndex::BO || stIndex == MuonStationIndex::EO ||
//            stIndex == MuonStationIndex::BI || stIndex == MuonStationIndex::EI) {
        if( stIndex == MuonStationIndex::BI || stIndex == MuonStationIndex::EI) {
          if(indexFirstInner==-1) indexFirstInner = index;
          indicesOfAffectedTSOSInner.push_back(*tsit);  
          indicesOfAffectedIdsInner.push_back(id);  
        }  
      }
    }

    if(indicesOfAffectedTSOS.size()==0&&indicesOfAffectedTSOSInner.size()==0) {
      Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );
      return newTrack;
    }

    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
    typePattern.set(Trk::TrackStateOnSurface::Alignment);

    Trk::TrackStateOnSurface* tsosAEOT = 0;
    Trk::AlignmentEffectsOnTrack* aEOT = 0;
    if(indicesOfAffectedTSOS.size()>0&&(m_addMiddle||m_addAll)) {
      int middle = indicesOfAffectedTSOS.size()/2;
      const Trk::TrackStateOnSurface* tsos = indicesOfAffectedTSOS[middle];
      aEOT = new Trk::AlignmentEffectsOnTrack(m_alignmentDelta,m_alignmentDeltaError,m_alignmentAngle,m_alignmentAngleError,indicesOfAffectedIds,&(tsos->measurementOnTrack()->associatedSurface())); 
      tsosAEOT = new Trk::TrackStateOnSurface(0,tsos->trackParameters()->clone(),0,0,typePattern,aEOT); 
    }

    Trk::TrackStateOnSurface* tsosAEOTInner = 0;
    if(indicesOfAffectedTSOSInner.size()>0&&(m_addInner||m_addTwo)) {
      int middle = indicesOfAffectedTSOSInner.size()/2;
      const Trk::TrackStateOnSurface* tsosInner = indicesOfAffectedTSOSInner[middle];
//      const Trk::Surface *surfInner = dynamic_cast <Trk::Surface*> ((tsosInner->measurementOnTrack()->associatedSurface()).clone());
      Trk::AlignmentEffectsOnTrack* aEOTInner = new Trk::AlignmentEffectsOnTrack(m_alignmentDelta,m_alignmentDeltaError,m_alignmentAngle,m_alignmentAngleError,indicesOfAffectedIdsInner,&(tsosInner->measurementOnTrack()->associatedSurface())); 
      tsosAEOTInner = new Trk::TrackStateOnSurface(0,tsosInner->trackParameters()->clone(),0,0,typePattern,aEOTInner); 
    }

    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfacesAEOT = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfacesAEOT->reserve(states->size()+2);
    tsit     = trackStateOnSurfaces->begin();
    tsit_end = trackStateOnSurfaces->end();
    index = -1;
    for( ; tsit!=tsit_end ; ++tsit ){
      index++;      
      if(index==indexFirst&&tsosAEOT) {
        trackStateOnSurfacesAEOT->push_back(tsosAEOT);
        if(!m_addAll) ATH_MSG_DEBUG(" AlignmentEffectsOnTrack for Middle added to trackStateOnSurfacesAEOT ");
        if(m_addAll) ATH_MSG_DEBUG(" AlignmentEffectsOnTrack for All stations added to trackStateOnSurfacesAEOT ");
      }
      if(index==indexFirstInner&&tsosAEOTInner) {
        trackStateOnSurfacesAEOT->push_back(tsosAEOTInner);
        ATH_MSG_DEBUG(" AlignmentEffectsOnTrack for Inner added to trackStateOnSurfacesAEOT ");
        if(m_addTwo) ATH_MSG_DEBUG(" also AlignmentEffectsOnTrack for Middle added to trackStateOnSurfacesAEOT ");
      }
      trackStateOnSurfacesAEOT->push_back(*tsit);
    } 

    if(!m_addInner&&!m_addTwo&&tsosAEOTInner) delete tsosAEOTInner;
    if(!m_addMiddle&&!m_addAll&&tsosAEOT) delete tsosAEOT;
 
//    trackStateOnSurfacesAEOT->push_back(tsosAEOT);
//    if(m_addInner) trackStateOnSurfacesAEOT->push_back(tsosAEOTInner);
 
    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfacesAEOT, track.fitQuality() ? track.fitQuality()->clone():0 );

// dump it
//    const DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfacesNew = newTrack->trackStateOnSurfaces();
//    tsit     = trackStateOnSurfacesNew->begin();
//    tsit_end = trackStateOnSurfacesNew->end();
//    for( ; tsit!=tsit_end ; ++tsit ){
//        if((*tsit)->alignmentEffectsOnTrack()) std::cout << " Peter alignmentEffectsOnTrack found with track pars " << (*tsit)->trackParameters() << std::endl;
//    }


    if(aEOT) ATH_MSG_DEBUG(" AlignmentEffectsOnTrack on surface " << aEOT->associatedSurface()  << " nr of tsos affected " << indicesOfAffectedTSOS.size());
    ATH_MSG_DEBUG(m_printer->print(*newTrack));
    ATH_MSG_DEBUG(m_printer->printMeasurements(*newTrack));

    return newTrack;
  }

  Trk::Track* MuonRefitTool::updateErrors( const Trk::Track& track, const IMuonRefitTool::Settings& settings ) const {

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }

    // vector to store states, the boolean indicated whether the state was create in this routine (true) or belongs to the track (false)
    // If any new state is created, all states will be cloned and a new track will beformed from them.
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    newStates.reserve(states->size()+5);
     
    Identifier currentMdtChId;

    const Trk::TrackParameters* startPars = 0;
    std::map<int,std::set<MuonStationIndex::StIndex> > stationsPerSector;

    // loop over TSOSs and find start parameters
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) continue;
    
      if( (*tsit)->type(Trk::TrackStateOnSurface::Perigee) ) {
	if( !dynamic_cast<const Trk::Perigee*>(pars) ){
	  if( !startPars ) {
	    startPars = pars;
	  }else{
	    ATH_MSG_WARNING("Track with two fit starting parameters!!!"); 
	  }
	}
      }


      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	continue;
      }

      // skip outliers
      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      
      Identifier id = m_helper->getIdentifier(*meas);
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ) continue;
      if( m_idHelper->isTrigger(id) || (m_idHelper->isCsc(id)&&m_idHelper->measuresPhi(id) ) ) continue;
      MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
      int sector = m_idHelper->sector(id);
      stationsPerSector[sector].insert(stIndex);
    }
  
    if( !startPars ){
      if( !track.trackParameters() || track.trackParameters()->empty() ){
	ATH_MSG_WARNING("Track without parameters, cannot update errors");
	return 0;
      }
      startPars = track.trackParameters()->front();
      ATH_MSG_VERBOSE("Did not find fit starting parameters, using first parameters " << m_printer->print(*startPars));
    }

    // loop over sectors and select the one with most layers
    std::vector<int> sectorsWithMostStations;
    unsigned int nmaxStations = 0;
    std::map<int,std::set<MuonStationIndex::StIndex> >::iterator stit = stationsPerSector.begin();
    std::map<int,std::set<MuonStationIndex::StIndex> >::iterator stit_end = stationsPerSector.end();
    for( ;stit!=stit_end;++stit ){
      if( msgLvl(MSG::VERBOSE) ){
	msg(MSG::VERBOSE) << " sector " << stit->first;
	for( std::set<MuonStationIndex::StIndex>::iterator ssit = stit->second.begin();
	     ssit != stit->second.end(); ++ssit ){
	  msg(MSG::VERBOSE) << " " << MuonStationIndex::stName(*ssit);
	}
      }
      if( stit->second.size() > nmaxStations ) {
	nmaxStations = stit->second.size(); 
	sectorsWithMostStations.clear();
	sectorsWithMostStations.push_back(stit->first);
      }else if( stit->second.size() == nmaxStations ) {
	sectorsWithMostStations.push_back(stit->first);
      }
    }
    int selectedSector = -1;
    if( sectorsWithMostStations.empty() ){
      ATH_MSG_WARNING("No sector selected");
    }else if( sectorsWithMostStations.size() == 1 ){
      selectedSector = sectorsWithMostStations.front();
    }else{
      ATH_MSG_DEBUG("Found track with special sector configuration " << sectorsWithMostStations.size() << " ch per sector " 
		    << nmaxStations << " using first sector" );
      selectedSector = sectorsWithMostStations.front();
      if( selectedSector%2 == 1 && sectorsWithMostStations.back()%2 != 1 ){
	ATH_MSG_DEBUG("Revising sector choice, picking small sector ");
	selectedSector = sectorsWithMostStations.back();
      }
    }

    // no check whether we have a barrel/endcap overlap 
    int nbarrel = 0; 
    int nendcap = 0; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::BI) ) ++nbarrel; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::BM) ) ++nbarrel; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::BO) ) ++nbarrel; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::EI) ) ++nendcap; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::EM) ) ++nendcap; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::EO) ) ++nendcap; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::EE) ) ++nendcap; 
    if( stationsPerSector[selectedSector].count(MuonStationIndex::BE) ) ++nendcap; 
    bool barrelEndcap = false; 
    bool deweightBarrel = false; 
    bool deweightEndcap = false; 
    if( nbarrel > 0 && nendcap > 0 ) { 
      if( nbarrel < nendcap ) deweightBarrel = true; 
      else                    deweightEndcap = true; 
      barrelEndcap = true; 
    } 
    if( msgLvl(MSG::DEBUG) ){ 
      msg() << MSG::DEBUG << " Selected sector " << selectedSector << " nstations " << nmaxStations 
	    << " barrel " << nbarrel << " endcap " << nendcap; 
      if( barrelEndcap ) {
	msg() << " barrel/endcap overlap "  << endmsg; 
	if(deweightEndcap) msg() << " deweight endcap "  << endmsg; 
	if(deweightBarrel) msg() << " deweight barrel "  << endmsg; 
      }
      msg() << MSG::DEBUG << endmsg; 
    } 

    unsigned int deweightHits = 0;
    unsigned int removedSectorHits = 0;
    bool addedPerigee = false;
    // loop over TSOSs
    tsit = states->begin();
    tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // check whether state is a measurement, if not add it, except if we haven't added the perigee surface yet
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if (settings.prepareForFit && ! pars)
      {
	if( addedPerigee ){
	  newStates.push_back( std::make_pair(false,*tsit) );
	  continue;
	}else{
	  ATH_MSG_DEBUG("Dropping TSOS before perigee surface");
	  continue;
	}
      }

      // if preparing for fit and not recreating the starting parameters, add the original perigee before back extrapolation to MS entry
      if( settings.prepareForFit && !settings.recreateStartingParameters && (*tsit)->type(Trk::TrackStateOnSurface::Perigee) ) {
	if( pars == startPars ){
	  ATH_MSG_DEBUG("Found fit starting parameters " << m_printer->print(*pars)); 
	  const Trk::Perigee* perigee = m_helper->createPerigee(*pars);
	  newStates.push_back( std::make_pair(true, MuonTSOSHelper::createPerigeeTSOS(perigee) ) );
	  addedPerigee = true;
	  continue;
	}else{
	  ATH_MSG_DEBUG("Removing perigee");
	}
      }


      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      if( settings.prepareForFit && settings.recreateStartingParameters && !addedPerigee ) {
	// small shift towards the ip
	double sign = pars->position().dot(pars->momentum()) > 0 ? 1. : -1.;
	Amg::Vector3D perpos =  pars->position() - 100.*sign*pars->momentum().unit();
      
 
	// create perigee
	double phi    = pars->momentum().phi();
	double theta  = pars->momentum().theta();
	double qoverp = pars->charge()/pars->momentum().mag();
	Trk::PerigeeSurface persurf(perpos);
	Trk::Perigee* perigee = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);      
	newStates.push_back( std::make_pair(true, MuonTSOSHelper::createPerigeeTSOS(perigee) ) );
	addedPerigee = true;
	ATH_MSG_DEBUG("Adding perigee in front of first measurement");
      }

      Identifier id = m_helper->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ){
	newStates.push_back( std::make_pair(false,*tsit) );	
	continue;
      }

      if( !settings.updateErrors ){
	newStates.push_back( std::make_pair(false,*tsit) );	
      }else{

	Identifier chId = m_idHelper->chamberId(id);
	MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
	if( m_idHelper->isMdt(id)  ) {

	  const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	  if( !mdt ){
	    ATH_MSG_WARNING(" Measurement with MDT identifier that is not a MdtDriftCircleOnTrack ");
	    continue;
	  }

	  bool hasT0Fit = false;
	  MdtDriftCircleOnTrack* cmdt =  const_cast<MdtDriftCircleOnTrack*>(mdt);
	  if( !cmdt ){
	    ATH_MSG_WARNING(" Failed to const_cast MdtDriftCircleOnTrack ");
	  }else{
	    if( cmdt->errorStrategy().creationParameter(Muon::MuonDriftCircleErrorStrategy::T0Refit))  hasT0Fit = true; 
	  }
      
	  const Trk::RIO_OnTrack* rot = 0;
	  int sector = m_idHelper->sector(id);
	  Trk::TrackStateOnSurface::TrackStateOnSurfaceType type = (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ? 
	    Trk::TrackStateOnSurface::Outlier : Trk::TrackStateOnSurface::Measurement;
	  
	  stIndex = m_idHelper->stationIndex(id);

	  // error update for three stations with barrel-endcap and shared sectors
	  if( !m_deweightTwoStationTracks || nmaxStations > 2 ){
	    if( m_deweightEEL1C05 && stIndex == MuonStationIndex::EE && m_idHelper->chamberIndex(id) == MuonStationIndex::EEL &&
		m_idHelper->stationEta(id) < 0 && m_idHelper->stationPhi(id) == 3){
	      //for this chamber the errors are enormous (for a period of time)
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyEEL1C05 ); 

	    }else if( deweightBarrel && ( stIndex == MuonStationIndex::BI || stIndex == MuonStationIndex::BM || stIndex == MuonStationIndex::BO ) ) { 
	      //std::cout << " MUONREFIT deweightBarrel " << std::endl;
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBarEnd ); 
	      if( settings.removeBarrelEndcapOverlap ) type = Trk::TrackStateOnSurface::Outlier;
	      
	    }else if( deweightEndcap && ( stIndex == MuonStationIndex::EI || stIndex == MuonStationIndex::EM || stIndex == MuonStationIndex::EO ||
		      stIndex == MuonStationIndex::EE || stIndex == MuonStationIndex::BE ) ) { // BEE chambers enter the endcap alignment system!
	      //std::cout << " MUONREFIT deweightEndcap " << std::endl;
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBarEnd ); 
	      if( settings.removeBarrelEndcapOverlap ) type = Trk::TrackStateOnSurface::Outlier;

	    }else if( settings.deweightOtherSectors && sector != selectedSector ) {
	      //std::cout << " MUONREFIT deweightOther " << std::endl;
	      ++deweightHits;
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategySL );
	      
	    }else if( m_deweightBEE && stIndex == MuonStationIndex::BE ) {
	      //std::cout << " MUONREFIT deweightBEE " << std::endl;
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBEE );
	      if( settings.removeBEE ) type = Trk::TrackStateOnSurface::Outlier;
	      
	    }else if( m_deweightEE && stIndex == MuonStationIndex::EE ) {
	      //std::cout << " MUONREFIT deweightEE " << std::endl;
	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyEE );

	    }else if( m_deweightBIS78 && stIndex == MuonStationIndex::BI && m_idHelper->chamberIndex(id) == MuonStationIndex::BIS &&
		      abs(m_idHelper->stationEta(id)) > 6 ){

	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBIS78 ); 

	    }else if( m_deweightBME && stIndex == MuonStationIndex::BM && m_idHelper->stationPhi(id) == 7 && 
		      (m_idHelper->mdtIdHelper()).stationName(id) == 53 ){

	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBXE ); 

	    }else if( m_deweightBOE && stIndex == MuonStationIndex::BO && m_idHelper->chamberIndex(id) == MuonStationIndex::BOL &&
		      abs(m_idHelper->stationEta(id)) == 7 && m_idHelper->stationPhi(id) == 7){

	      rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyBXE ); 

	    }else{
	      /** default strategy */
	      //std::cout << " MUONREFIT updateError " << std::endl;
//	      if( !m_alignErrorTool.empty() && !alignerrmap.empty() ) {
//	        rot = m_mdtRotCreator->updateErrorExternal( *mdt, pars, &alignerrmap);
//	      } else {
	        MuonDriftCircleErrorStrategy strat(m_errorStrategy);
	        if( hasT0Fit )       strat.setParameter(MuonDriftCircleErrorStrategy::T0Refit,true);
	        if( settings.broad ) strat.setParameter(MuonDriftCircleErrorStrategy::BroadError,true);
	        rot =  m_mdtRotCreator->updateError( *mdt, pars, &strat );
//	      }
	    }
	  }else{
	    rot = m_mdtRotCreator->updateError( *mdt, pars, &m_errorStrategyTwoStations );
	  }

	  const MdtDriftCircleOnTrack* newMdt = rot ? dynamic_cast<const MdtDriftCircleOnTrack*>(rot) : 0;
	  if( !newMdt ){
	    newMdt = mdt->clone();
	    type = Trk::TrackStateOnSurface::Outlier;
	  }
	  if( settings.removeOtherSectors ) {
	    if( sector != selectedSector ){
	      ++removedSectorHits;
	      type = Trk::TrackStateOnSurface::Outlier;
	    }
	  }
	  if( settings.chambersToBeremoved.count(chId) || settings.precisionLayersToBeremoved.count(stIndex) ){
	    type = Trk::TrackStateOnSurface::Outlier;
	  }
	    

	  if( msgLvl(MSG::DEBUG) ){
	    msg() << m_idHelper->toString(newMdt->identify()) << " radius " << newMdt->driftRadius() 
		  << " new err " << Amg::error(newMdt->localCovariance(),Trk::locR) 
		  << " old err " << Amg::error(mdt->localCovariance(),Trk::locR);
	    if( hasT0Fit ) msg() << " HasT0";
	    else           msg() << " No T0";
	    if( type == Trk::TrackStateOnSurface::Outlier ) msg() << " Outlier";
	    if( fabs(newMdt->driftRadius() - mdt->driftRadius() ) > 0.1 ) msg() << " Bad recalibration: old r " << mdt->driftRadius();
	    msg() << endmsg;
	  }

	  Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOSWithUpdate( **tsit, newMdt, pars->clone(), type );
	  newStates.push_back( std::make_pair(true,tsos) );	
	}else if( m_idHelper->isCsc(id) ) {
	  
	  if( settings.chambersToBeremoved.count(chId) || settings.precisionLayersToBeremoved.count(stIndex) ){

	    Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::cloneTSOS( **tsit, Trk::TrackStateOnSurface::Outlier );
	    newStates.push_back( std::make_pair(true,tsos) );	

	  }else{

	    newStates.push_back( std::make_pair(false,*tsit) );	

	  }
	}else if(  m_idHelper->isTrigger(id) ){

	  if( m_idHelper->measuresPhi(id) ){

	    MuonStationIndex::PhiIndex phiIndex = m_idHelper->phiIndex(id);

	    if( settings.chambersToBeremoved.count(chId) || settings.phiLayersToBeremoved.count(phiIndex) ){

	      Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::cloneTSOS( **tsit, Trk::TrackStateOnSurface::Outlier );
	      newStates.push_back( std::make_pair(true,tsos) );	
	      
	    }else{
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }


	  }else{
	    if( settings.updateTriggerErrors ){
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }else{
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }
	  }
	}else if( m_idHelper->isMM(id) || m_idHelper->issTgc(id) ){
	  newStates.push_back( std::make_pair(false,*tsit) );	

	}else{
	  ATH_MSG_WARNING(" unknown Identifier " << m_idHelper->mdtIdHelper().print_to_string(id) ); 
	}
      }
    }

    if( deweightHits > 0  )       ATH_MSG_DEBUG(" de-weighted " << deweightHits << " MDT hits from neighbouring sectors");
    if( removedSectorHits > 0  )  ATH_MSG_DEBUG(" removed " << removedSectorHits << " MDT hits from neighbouring sectors");


    ATH_MSG_VERBOSE(" original track had " << states->size() 
		    << " TSOS, adding " << newStates.size() - states->size() << " new TSOS ");

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

  Trk::Track* MuonRefitTool::updateMdtErrors( const Trk::Track& track, const IMuonRefitTool::Settings& settings ) const {

// uses the muonErrorStrategy

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }

    // vector to store states, the boolean indicated whether the state was create in this routine (true) or belongs to the track (false)
    // If any new state is created, all states will be cloned and a new track will beformed from them.
    std::vector< std::pair<bool,const Trk::TrackStateOnSurface* > > newStates;
    newStates.reserve(states->size()+5);
     
    Identifier currentMdtChId;

    const Trk::TrackParameters* startPars = 0;

    // loop over TSOSs and find start parameters
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check

      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) continue;
    
      if( (*tsit)->type(Trk::TrackStateOnSurface::Perigee) ) {
	if( !dynamic_cast<const Trk::Perigee*>(pars) ){
	  if( !startPars ) {
	    startPars = pars;
	  }else{
	    ATH_MSG_WARNING("Track with two fit starting parameters!!!"); 
	  }
	}
      }


      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	continue;
      }

      // skip outliers
      if( (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ) continue;
      
      Identifier id = m_helper->getIdentifier(*meas);
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ) continue;
      if( m_idHelper->isTrigger(id) || (m_idHelper->isCsc(id)&&m_idHelper->measuresPhi(id) ) ) continue;
    }
  
    if( !startPars ){
      if( !track.trackParameters() || track.trackParameters()->empty() ){
	ATH_MSG_WARNING("Track without parameters, cannot update errors");
	return 0;
      }
      startPars = track.trackParameters()->front();
      ATH_MSG_VERBOSE("Did not find fit starting parameters, using first parameters " << m_printer->print(*startPars));
    }

    bool addedPerigee = false;
    // loop over TSOSs
    tsit = states->begin();
    tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // check whether state is a measurement, if not add it, except if we haven't added the perigee surface yet
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if (settings.prepareForFit && ! pars)
      {
	if( addedPerigee ){
	  newStates.push_back( std::make_pair(false,*tsit) );
	  continue;
	}else{
	  ATH_MSG_DEBUG("Dropping TSOS before perigee surface");
	  continue;
	}
      }

      // if preparing for fit and not recreating the starting parameters, add the original perigee before back extrapolation to MS entry
      if( settings.prepareForFit && !settings.recreateStartingParameters && (*tsit)->type(Trk::TrackStateOnSurface::Perigee) ) {
	if( pars == startPars ){
	  ATH_MSG_DEBUG("Found fit starting parameters " << m_printer->print(*pars)); 
	  const Trk::Perigee* perigee = m_helper->createPerigee(*pars);
	  newStates.push_back( std::make_pair(true, MuonTSOSHelper::createPerigeeTSOS(perigee) ) );
	  addedPerigee = true;
	  continue;
	}else{
	  ATH_MSG_DEBUG("Removing perigee");
	}
      }


      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	newStates.push_back( std::make_pair(false,*tsit) );
	continue;
      }

      if( settings.prepareForFit && settings.recreateStartingParameters && !addedPerigee ) {
	// small shift towards the ip
	double sign = pars->position().dot(pars->momentum()) > 0 ? 1. : -1.;
	Amg::Vector3D perpos =  pars->position() - 100.*sign*pars->momentum().unit();
      
 
	// create perigee
	double phi    = pars->momentum().phi();
	double theta  = pars->momentum().theta();
	double qoverp = pars->charge()/pars->momentum().mag();
	Trk::PerigeeSurface persurf(perpos);
	Trk::Perigee* perigee = new Trk::Perigee(0,0,phi,theta,qoverp,persurf);      
	newStates.push_back( std::make_pair(true, MuonTSOSHelper::createPerigeeTSOS(perigee) ) );
	addedPerigee = true;
	ATH_MSG_DEBUG("Adding perigee in front of first measurement");
      }

      Identifier id = m_helper->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() || !m_idHelper->isMuon(id) ){
	newStates.push_back( std::make_pair(false,*tsit) );	
	continue;
      }

      if( !settings.updateErrors ){
	newStates.push_back( std::make_pair(false,*tsit) );	
      }else{

	Identifier chId = m_idHelper->chamberId(id);
	MuonStationIndex::StIndex stIndex = m_idHelper->stationIndex(id);
	if( m_idHelper->isMdt(id)  ) {

	  const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	  if( !mdt ){
	    ATH_MSG_WARNING(" Measurement with MDT identifier that is not a MdtDriftCircleOnTrack ");
	    continue;
	  }

	  bool hasT0Fit = false;
	  MdtDriftCircleOnTrack* cmdt =  const_cast<MdtDriftCircleOnTrack*>(mdt);
	  if( !cmdt ){
	    ATH_MSG_WARNING(" Failed to const_cast MdtDriftCircleOnTrack ");
	  }else{
	    if( cmdt->errorStrategy().creationParameter(Muon::MuonDriftCircleErrorStrategy::T0Refit))  hasT0Fit = true; 
	  }
      
	  const Trk::RIO_OnTrack* rot = 0;
//	  int sector = m_idHelper->sector(id);
	  Trk::TrackStateOnSurface::TrackStateOnSurfaceType type = (*tsit)->type(Trk::TrackStateOnSurface::Outlier) ? 
	    Trk::TrackStateOnSurface::Outlier : Trk::TrackStateOnSurface::Measurement;
	  
	  stIndex = m_idHelper->stationIndex(id);

// use the muonErrorStrategy
	  MuonDriftCircleErrorStrategy strat(m_muonErrorStrategy);
	  if( hasT0Fit )       strat.setParameter(MuonDriftCircleErrorStrategy::T0Refit,true);
	  if( settings.broad ) strat.setParameter(MuonDriftCircleErrorStrategy::BroadError,true);
	  rot =  m_mdtRotCreator->updateError( *mdt, pars, &strat );

	  const MdtDriftCircleOnTrack* newMdt = rot ? dynamic_cast<const MdtDriftCircleOnTrack*>(rot) : 0;
	  if( !newMdt ){
	    newMdt = mdt->clone();
	    type = Trk::TrackStateOnSurface::Outlier;
	  }
	  if( settings.removeOtherSectors ) {
//	    if( sector != selectedSector ){
//	      ++removedSectorHits;
//	      type = Trk::TrackStateOnSurface::Outlier;
//	    }
	  }
	  if( settings.chambersToBeremoved.count(chId) || settings.precisionLayersToBeremoved.count(stIndex) ){
	    type = Trk::TrackStateOnSurface::Outlier;
	  }
	    

	  if( msgLvl(MSG::DEBUG) ){
	    msg() << " updateMdtErrors " << m_idHelper->toString(newMdt->identify()) << " radius " << newMdt->driftRadius() 
		  << " new err " << Amg::error(newMdt->localCovariance(),Trk::locR) 
		  << " old err " << Amg::error(mdt->localCovariance(),Trk::locR);
	    if( hasT0Fit ) msg() << " HasT0";
	    else           msg() << " No T0";
	    if( type == Trk::TrackStateOnSurface::Outlier ) msg() << " Outlier";
	    if( fabs(newMdt->driftRadius() - mdt->driftRadius() ) > 0.1 ) msg() << " Bad recalibration: old r " << mdt->driftRadius();
	    msg() << endmsg;
	  }

	  Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::createMeasTSOSWithUpdate( **tsit, newMdt, pars->clone(), type );
	  newStates.push_back( std::make_pair(true,tsos) );	
	}else if( m_idHelper->isCsc(id) ) {
	  
	  if( settings.chambersToBeremoved.count(chId) || settings.precisionLayersToBeremoved.count(stIndex) ){

	    Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::cloneTSOS( **tsit, Trk::TrackStateOnSurface::Outlier );
	    newStates.push_back( std::make_pair(true,tsos) );	

	  }else{

	    newStates.push_back( std::make_pair(false,*tsit) );	

	  }
	}else if(  m_idHelper->isTrigger(id) ){

	  if( m_idHelper->measuresPhi(id) ){

	    MuonStationIndex::PhiIndex phiIndex = m_idHelper->phiIndex(id);

	    if( settings.chambersToBeremoved.count(chId) || settings.phiLayersToBeremoved.count(phiIndex) ){

	      Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::cloneTSOS( **tsit, Trk::TrackStateOnSurface::Outlier );
	      newStates.push_back( std::make_pair(true,tsos) );	
	      
	    }else{
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }


	  }else{
	    if( settings.updateTriggerErrors ){
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }else{
	      
	      newStates.push_back( std::make_pair(false,*tsit) );	
	      
	    }
	  }
	}else if( m_idHelper->isMM(id) || m_idHelper->issTgc(id) ){
	  newStates.push_back( std::make_pair(false,*tsit) );	

	}else{
	  ATH_MSG_WARNING(" unknown Identifier " << m_idHelper->mdtIdHelper().print_to_string(id) ); 
	}
      }
    }


    ATH_MSG_VERBOSE(" original track had " << states->size() 
		    << " TSOS, adding " << newStates.size() - states->size() << " new TSOS ");

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

  const Trk::Track* MuonRefitTool::removeOutliers( const Trk::Track& track, const IMuonRefitTool::Settings& settings ) const {

    // loop over track and calculate residuals
    const DataVector<const Trk::TrackStateOnSurface>* states = track.trackStateOnSurfaces();
    if( !states ){
      ATH_MSG_WARNING(" track without states, discarding track ");
      return 0;
    }

    Identifier currentMdtChId;
    std::set<Identifier> removedIdentifiers;
    std::vector<const MdtDriftCircleOnTrack*> mdts;
    const Trk::TrackParameters* chamberPars = 0;
  
    // loop over TSOSs and find start parameters
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit = states->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check
      
      // check whether state is a measurement
      const Trk::TrackParameters* pars = (*tsit)->trackParameters();
      if( !pars ) {
	continue;
      }
    
      if( !(*tsit)->type(Trk::TrackStateOnSurface::Measurement) ) {
	continue;
      }

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( !meas ) {
	continue;
      }

      Identifier id = m_helper->getIdentifier(*meas);
      
      // Not a ROT, else it would have had an identifier. Keep the TSOS.
      if( !id.is_valid() ){
	continue;
      }

      if( m_idHelper->isMdt(id) ) {
      
	const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(meas);
	if( !mdt ){
	  ATH_MSG_WARNING(" Measurement with MDT identifier that is not a MdtDriftCircleOnTrack ");
	  continue;
	}
	// get ch ID
	Identifier chId = m_idHelper->chamberId(id);

	// if we have a new chambers
	if( chId != currentMdtChId ){
	
	  // check that there are pars (not the case for the first mdt), if so we collected all hits for this chamber so call cleaning
	  if( chamberPars ){
	    if( !removeMdtOutliers(*chamberPars,mdts,removedIdentifiers,settings) ){
	      if( mdts.size() > 4 ) ATH_MSG_WARNING("Problem removing outliers in chamber " << m_idHelper->toStringChamber(currentMdtChId) << " hits " << mdts.size());
	      if( settings.discardNotCleanedTracks ) return 0;
	    }
	  }
	  // update to new chamber
	  chamberPars = pars;
	  mdts.clear();
	  currentMdtChId = chId;
	}

	mdts.push_back(mdt);
      }
    }
  
    // clean the last chamber on the track
    if( chamberPars ){
      if( !removeMdtOutliers(*chamberPars,mdts,removedIdentifiers,settings) ){
	if( mdts.size() > 4 ) ATH_MSG_WARNING("Problem removing outliers in chamber " << m_idHelper->toStringChamber(currentMdtChId) << " hits " << mdts.size());
	if( settings.discardNotCleanedTracks ) return 0;
      }
    }

    if( removedIdentifiers.empty() ){
      ATH_MSG_DEBUG("No hits remove, returning original track");
      return &track;
    }

    // states were added, create a new track
    DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces = new DataVector<const Trk::TrackStateOnSurface>();
    trackStateOnSurfaces->reserve( states->size() );

    ATH_MSG_DEBUG("Removing nhits: " << removedIdentifiers.size() );

    tsit = states->begin();
    tsit_end = states->end();
    for( ; tsit!=tsit_end ; ++tsit ){
      
      if( !*tsit ) continue; //sanity check

      // check whether state is a measurement
      const Trk::MeasurementBase* meas = (*tsit)->measurementOnTrack();
      if( meas ) {

	Identifier id = m_helper->getIdentifier(*meas);
      
	if( removedIdentifiers.count(id) ){
	
	  Trk::TrackStateOnSurface* tsos = MuonTSOSHelper::cloneTSOS( **tsit, Trk::TrackStateOnSurface::Outlier );
	  trackStateOnSurfaces->push_back( tsos );	
	  continue;
	}
      }
      trackStateOnSurfaces->push_back( (*tsit)->clone() );	
    }

    Trk::Track* newTrack =  new Trk::Track( track.info(), trackStateOnSurfaces, track.fitQuality() ? track.fitQuality()->clone():0 );


    return newTrack;
  }

  bool MuonRefitTool::removeMdtOutliers( const Trk::TrackParameters& pars, const std::vector<const MdtDriftCircleOnTrack*>& hits, 
					 std::set<Identifier>& removedIdentifiers, const IMuonRefitTool::Settings& settings ) const {

    if( hits.size() < 3 ){
      ATH_MSG_VERBOSE("Too few hits, cannot perform cleaning");
      return false;
    }
    ATH_MSG_VERBOSE("Performing cleaning, nhits " << hits.size() );

    TrkDriftCircleMath::DCOnTrackVec dcsOnTrack;
    TrkDriftCircleMath::DCVec        dcs;
    /* ********  Mdt hits  ******** */

    const MuonGM::MdtReadoutElement* detEl = 0;

    Amg::Transform3D gToStation;

    // set to get Identifiers of chambers with hits
    std::vector< std::pair<Identifier,bool> > indexIdMap;
    indexIdMap.reserve( hits.size() );

  


    unsigned index = 0;
    std::vector<const MdtDriftCircleOnTrack*>::const_iterator it = hits.begin();
    std::vector<const MdtDriftCircleOnTrack*>::const_iterator it_end = hits.end();
    for(  ;it!=it_end;++it){

      const MdtDriftCircleOnTrack* mdt = dynamic_cast<const MdtDriftCircleOnTrack*>(*it);

      if( !mdt ){
	continue;
      }
      Identifier id = mdt->identify();
      //Identifier elId = m_idHelper->mdtIdHelper().elementID( id );

      if( !detEl ){
	detEl = mdt->prepRawData()->detectorElement();
	if( !detEl ){
	  ATH_MSG_WARNING(" error aborting not detEl found ");
	  break;
	}
	gToStation = detEl->GlobalToAmdbLRSTransform();
      }
      // calculate local AMDB position
      Amg::Vector3D locPos = gToStation*mdt->prepRawData()->globalPosition();
      TrkDriftCircleMath::LocPos lpos(locPos.y(),locPos.z());

      double r  = fabs(mdt->localParameters()[Trk::locR]);
      double dr = Amg::error(mdt->localCovariance(),Trk::locR);
      ATH_MSG_VERBOSE("New MDT " << m_idHelper->toString(id) << "  r " << mdt->localParameters()[Trk::locR] 
		      << " dr  " << dr << "  (original) " << Amg::error(mdt->localCovariance(),Trk::locR) );

      // create identifier
      TrkDriftCircleMath::MdtId mdtid( m_idHelper->mdtIdHelper().isBarrel(id),m_idHelper->mdtIdHelper().multilayer(id)-1, 
				       m_idHelper->mdtIdHelper().tubeLayer(id)-1, m_idHelper->mdtIdHelper().tube(id)-1 );

      // create new DriftCircle
      TrkDriftCircleMath::DriftCircle dc( lpos, r, 1., dr, TrkDriftCircleMath::DriftCircle::InTime, mdtid, index, mdt );
      TrkDriftCircleMath::DCOnTrack dcOnTrack(dc,1.,1.);
      dcs.push_back(dc);
      dcsOnTrack.push_back( dcOnTrack );
      indexIdMap.push_back( std::make_pair(id,false) );


      ++index;
    }    

    // define axis of chamber in global coordinates
    Amg::Transform3D amdbToGlobal = detEl->AmdbLRSToGlobalTransform();

    // create new surface
    Amg::Transform3D* surfaceTransform = new Amg::Transform3D(amdbToGlobal.rotation());
    surfaceTransform->pretranslate(pars.position());
    double surfDim = 500.;
    Trk::PlaneSurface* surf = new Trk::PlaneSurface( surfaceTransform, surfDim, surfDim );
	
    Amg::Vector3D dir = pars.momentum().unit();
    if( dir.y()*pars.position().y() < 0. ) {
      dir *= -1.;
    }
    Trk::LocalDirection locDir;
    surf->globalToLocalDirection(dir,locDir);

    Amg::Vector3D locDirTrack(gToStation.linear()*dir);
    double track_angleYZ = atan2(locDirTrack.z(),locDirTrack.y());
	
    if(!detEl) return false;
    // transform nominal pointing chamber position into surface frame
    Amg::Vector3D dirCh(gToStation.linear()*detEl->center());
    double chamber_angleYZ = atan2(dirCh.z(),dirCh.y());
    double angleYZ = locDir.angleYZ();
    delete surf;

    const Amg::Vector3D lpos = gToStation*pars.position();

    TrkDriftCircleMath::LocPos segPos(lpos.y(),lpos.z());
    TrkDriftCircleMath::Line segPars(segPos,angleYZ);


    ATH_MSG_DEBUG("Seeding angles " << track_angleYZ << " from surf " << angleYZ << " ch angle " << chamber_angleYZ << " pos " << segPos);
    m_finder.setPhiRoad(track_angleYZ,chamber_angleYZ,0.14);

    if( msgLvl(MSG::VERBOSE) ){
      if(m_fitter.fit(segPars,dcsOnTrack)){
	TrkDriftCircleMath::Segment segment = m_fitter.result();
	segment.hitsOnTrack(dcsOnTrack.size());
	ATH_MSG_DEBUG(" segment after fit " << segment.chi2() << " ndof " << segment.ndof() << " local parameters "
		      << segment.line().x0() << " " << segment.line().y0() << "  phi " << segment.line().phi() );
      }else{
	ATH_MSG_DEBUG("Fit failed: hits" << dcsOnTrack.size() );
      }
    }

    TrkDriftCircleMath::SegVec segments = m_finder.findSegments(dcs);
    if( !segments.empty() ){
      ATH_MSG_DEBUG("Found segments " << segments.size() );
    }
    
    if( segments.size() != 1 ){
      if( hits.size() > 3 ) ATH_MSG_WARNING(" Found two solutions ");
      else                  ATH_MSG_DEBUG(" Found two solutions ");
      double dthetaBest = 10000.;
      int index = 0;
      int indexBest = -1;
      TrkDriftCircleMath::SegIt sit = segments.begin();
      TrkDriftCircleMath::SegIt sit_end = segments.end();
      for( ;sit!=sit_end;++sit,++index ) {
      
	double dtheta = fabs(sit->line().phi() - track_angleYZ);
	if( dtheta < dthetaBest ){
	  dthetaBest = dtheta;
	  indexBest = index;
	}
	if( sit->hitsOnTrack() > 4 ){
	  ATH_MSG_DEBUG("Recoverable segment " << *sit);
	}
      }
      if( indexBest != -1 ){
	TrkDriftCircleMath::SegVec selectedSegments;
	selectedSegments.push_back(segments[indexBest]);
	segments = selectedSegments;
	ATH_MSG_DEBUG("Selected segment " << segments.front());
      
      }else{
	return false;
      }
    }

  
    TrkDriftCircleMath::Segment& segment = segments.front();
    if( settings.discardNotCleanedTracks && !segment.hasT0Shift() ) return false;

    if( segment.hasT0Shift() || segment.hitsOnTrack() > 5 ) {
      //     it = hits.begin();
      //     it_end = hits.end();
      //     for(  ;it!=it_end;++it){
      
      //       const Trk::ErrorMatrix& localErrorMatrix = (*it)->localErrorMatrix();
      //       double error = 1.99;
      
      //       double scaledError2 = error*error;
      //       Trk::CovarianceMatrix* covPtr = const_cast<Trk::CovarianceMatrix*>(localErrorMatrix.covariancePointer());
      //       Trk::WeightMatrix* weightPtr = const_cast<Trk::WeightMatrix*>(localErrorMatrix.weightPointer());      
      //       if( covPtr ) {
      // 	(*covPtr)[Trk::locR][Trk::locR] = scaledError2;
      //       }
      //       if( weightPtr ) {
      // 	(*weightPtr)[Trk::locR][Trk::locR] = 1./scaledError2;
      //       }
      //     }
    
      ATH_MSG_DEBUG("Segment with t0 shift " << segment.t0Shift() ); 
    }

    if( dcs.size() ==  segment.hitsOnTrack() ){
      ATH_MSG_DEBUG(" No hits removed ");
      return true;
    }else if( dcs.size() >  segment.hitsOnTrack() + 1 ){
      ATH_MSG_DEBUG(" more than one hit removed ");
      if( segment.hitsOnTrack() < 4 ) return false;
    }

    ATH_MSG_DEBUG(" removed hits: " << dcs.size() - segment.hitsOnTrack() );

    float tubeRadius = detEl->innerTubeRadius();

    TrkDriftCircleMath::MatchDCWithLine matchDC(segment.line(),3.,TrkDriftCircleMath::MatchDCWithLine::Pull,tubeRadius);
    const TrkDriftCircleMath::DCOnTrackVec& matchedDCs = matchDC.match(segment.dcs());
  
    for( TrkDriftCircleMath::DCOnTrackCit dcit = matchedDCs.begin();dcit!=matchedDCs.end();++dcit ){
    
      if( dcit->state() == TrkDriftCircleMath::DCOnTrack::OnTrack ) {
	if( fabs(dcit->r()) - fabs(dcit->rot()->driftRadius()) > 0.1 ){
	  ATH_MSG_DEBUG("Large change in drift radius: r_old " 
		       << dcit->rot()->driftRadius() << "  r_new " 
		       << dcit->r() );
	}
	const Trk::LocalParameters& clpars = dcit->rot()->localParameters();
	Trk::LocalParameters* lpars = const_cast<Trk::LocalParameters*>(&clpars);
	if(lpars){
	  double sign = (*lpars)[Trk::locR] < 0. ? -1. : 1.;
	  (*lpars)[Trk::locR] = sign*dcit->r();
	}
	continue;

      }
      indexIdMap[dcit->index()].second = true;
    }
  
    std::vector< std::pair<Identifier,bool> >::iterator iit = indexIdMap.begin();
    std::vector< std::pair<Identifier,bool> >::iterator iit_end = indexIdMap.end();
    for( ;iit!=iit_end;++iit ){
      if(iit->second){
	ATH_MSG_VERBOSE(" removing hit " << m_idHelper->toString( iit->first ) );
	removedIdentifiers.insert( iit->first );
      }
    }
    return true;
  }

}
