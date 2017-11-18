/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// STL includes
#include <iostream>

#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"


#include "MuonErrorOptimisationTool.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMuonRefitTool.h"
#include "TrkToolInterfaces/ITrackSummaryHelperTool.h"


namespace Muon {

  MuonErrorOptimisationTool::MuonErrorOptimisationTool( const std::string& ty,const std::string& na,const IInterface* pa) : 
    AthAlgTool(ty,na,pa),
    m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
    m_helper("Muon::MuonEDMHelperTool/MuonEDMHelperTool"), 
    m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
    m_trackSummaryTool("Muon::MuonTrackSummaryHelperTool/MuonTrackSummaryHelperTool"),
    m_refitTool("Muon::MuonRefitTool/MuonRefitTool"),
    m_lowPtThreshold(5000.),
    m_nrefitAll(0),
    m_nrefitAllLowPt(0),
    m_nrefitOk(0),
    m_nrefit(0),
    m_nrefitLowPt(0),
    m_nrefitPrecise(0),
    m_nrefitPreciseLowPt(0),
    m_nbetterPreciseFit(0),
    m_nbetterFit(0)
  {
    declareProperty("Printer", m_printer );
    declareProperty("Helper", m_helper );
    declareProperty("IdHelper", m_idHelper );
    declareProperty("TrackSummeryTool", m_trackSummaryTool );
    declareProperty("RefitTool",m_refitTool ); 
    declareProperty("Chi2NDofCutRefit",m_chi2NdofCutRefit = 5. ); 

    declareProperty("PrepareForFit",		 m_refitSettings.prepareForFit = true );
    declareProperty("RecreateStartingParameters",m_refitSettings.recreateStartingParameters = true );
    declareProperty("UpdateErrors",		 m_refitSettings.updateErrors = true );
    declareProperty("RemoveOutliers",            m_refitSettings.removeOutliers = false );
    declareProperty("RemoveOtherSectors",        m_refitSettings.removeOtherSectors = false);
    declareProperty("RemoveBarrelEndcapOverlap", m_refitSettings.removeBarrelEndcapOverlap = false);
    declareProperty("RemoveBEE",                 m_refitSettings.removeBEE = false);
    declareProperty("DeweightOtherSectors",      m_refitSettings.deweightOtherSectors = true);

   declareInterface<IMuonErrorOptimisationTool>(this);
  }

  MuonErrorOptimisationTool::~MuonErrorOptimisationTool()
  {

  }

  StatusCode MuonErrorOptimisationTool::initialize()
  {

    ATH_MSG_INFO( "Initializing MuonErrorOptimisationTool" );
    
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_helper.retrieve() );
    ATH_CHECK( m_idHelper.retrieve() );
    ATH_CHECK( m_trackSummaryTool.retrieve() );
    if( !m_refitTool.empty() ) ATH_CHECK( m_refitTool.retrieve() );

    return StatusCode::SUCCESS;
  }


  StatusCode MuonErrorOptimisationTool::finalize()
  {
  
    if( m_nrefitAll > 0 ){
      double scale = 1./m_nrefitAll;
      double scaleLowPt = m_nrefitAllLowPt > 0 ? 1./m_nrefitAllLowPt : 1.;
      ATH_MSG_INFO(" Number of all refits                          " << m_nrefitAll << " fraction of total calls " << scale*m_nrefitAll );
      ATH_MSG_INFO(" Number of refits with precise errors          " << m_nrefitPrecise << " fraction of total calls " << scale*m_nrefitPrecise );
      ATH_MSG_INFO(" Number of refits with large errors            " << m_nrefit << " fraction of total calls " << scale*m_nrefit );
      ATH_MSG_INFO(" Number of all refits (low pt)                 " << m_nrefitAllLowPt << " fraction of total calls " << scaleLowPt*m_nrefitAllLowPt );
      ATH_MSG_INFO(" Number of refits with precise errors (low pt) " << m_nrefitPreciseLowPt << " fraction of total calls " << scaleLowPt*m_nrefitPreciseLowPt );
      ATH_MSG_INFO(" Number of refits with large errors (low pt)   " << m_nrefitLowPt << " fraction of total calls " << scaleLowPt*m_nrefitLowPt );
      ATH_MSG_INFO(" Precise refit bad but better than input       " << m_nbetterPreciseFit << " fraction of total calls " << scale*m_nbetterPreciseFit );
      ATH_MSG_INFO(" Large error refit bad but better than input   " << m_nbetterFit << " fraction of total calls " << scale*m_nbetterFit );
    }

    return StatusCode::SUCCESS;
  }

  Trk::Track* MuonErrorOptimisationTool::optimiseErrors( const Trk::Track& track ) const {

    if( m_refitTool.empty() ) return 0;
    const Trk::Perigee* pp = track.perigeeParameters();
    bool isLowPt = false;
    if( pp && pp->momentum().mag() < m_lowPtThreshold ) isLowPt = true;
    if( isLowPt ) ++m_nrefitAllLowPt;
    else          ++m_nrefitAll;

 
    const Trk::Track* refittedTrack = 0;
    const Trk::Track* result1 = 0;
    const Trk::Track* result2 = 0;
    
    // first refit with precise errors
    m_refitSettings.broad = false;
    refittedTrack = m_refitTool->refit(track,&m_refitSettings);    
    if( refittedTrack ){
    
      // check whether it is ok
      if( !m_helper->goodTrack(*refittedTrack,m_chi2NdofCutRefit) ) {
	ATH_MSG_VERBOSE("Precise fit bad " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
	
	// if not delete track
	result1 = refittedTrack != &track ? refittedTrack : 0;
	refittedTrack = 0;
      }else{
	ATH_MSG_VERBOSE("Precise fit ok " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
	if( isLowPt ) ++m_nrefitPreciseLowPt;
	else          ++m_nrefitPrecise;
      }
      }else{
      ATH_MSG_VERBOSE("Precise fit failed");
    }
    
    // only do second fit if first is not ok
    if( !refittedTrack ){
      // second refit track
      m_refitSettings.broad = true;
      refittedTrack = m_refitTool->refit(track,&m_refitSettings);    
      if( refittedTrack ) {
      
        // check whether it is ok
        if( !m_helper->goodTrack(*refittedTrack,m_chi2NdofCutRefit) ) {
          ATH_MSG_VERBOSE("Loose fit bad " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
          // if not delete track
          result2 = refittedTrack != &track ? refittedTrack : 0;
          refittedTrack = 0;
        }else{
          ATH_MSG_VERBOSE("Loose fit ok " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
          if( isLowPt ) ++m_nrefitLowPt;
          else          ++m_nrefit;
          ++m_nrefit;
        }
      }else{
        ATH_MSG_VERBOSE("Loose fit failed");
      }
    }

    // if failed to refit or refit returned original track, return 0
    if( !refittedTrack || refittedTrack == &track ){
      
      // check if any refit succeeded
      if( !result1 && !result2 ) return 0;

      // now compare chi2
      const Trk::FitQuality* fq0 = track.fitQuality();
      const Trk::FitQuality* fq1 = result1 ? result1->fitQuality() : 0;
      const Trk::FitQuality* fq2 = result2 ? result2->fitQuality() : 0;
      
      bool doSelection = true;

      // ensure that each track if present has a fit quality
      if( !fq0 || (result1 && !fq1) || (result2  && !fq2) ){

        ATH_MSG_WARNING( "track without fit quality after refit" );
        doSelection = false;
        // chech that ndof didn't change
      }else if( (fq1 && fq0->numberDoF() != fq1->numberDoF()) || (fq2 && fq0->numberDoF() != fq2->numberDoF() ) ){
        
        doSelection = false;
        // ugly bit of code to get the hit counts for the three tracks 
        int nhits0 = -1;
        const Trk::TrackSummary* summary0 = track.trackSummary();
        const Trk::MuonTrackSummary* muonSummary0 = 0;
        if( summary0 ){
          if( summary0->muonTrackSummary() ) muonSummary0 = summary0->muonTrackSummary();
          else{
            Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary0);
            if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(track,*tmpSum);
            if( tmpSum->muonTrackSummary() ) muonSummary0 = tmpSum->muonTrackSummary();
          }
          if( muonSummary0 ) nhits0 = muonSummary0->netaHits()+ muonSummary0->nphiHits();
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(track,tmpSummary);
          if( tmpSummary.muonTrackSummary() ) muonSummary0 = tmpSummary.muonTrackSummary();
          if( muonSummary0 ) nhits0 = muonSummary0->netaHits()+ muonSummary0->nphiHits();
        }

        int nhits1 = -1;
        const Trk::TrackSummary* summary1 = track.trackSummary();
        const Trk::MuonTrackSummary* muonSummary1 = 0;
        if( summary1 ){
          if( summary1->muonTrackSummary() ) muonSummary1 = summary1->muonTrackSummary();
          else{
            Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary1);
            if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(track,*tmpSum);
            if( tmpSum->muonTrackSummary() ) muonSummary1 = tmpSum->muonTrackSummary();
          }
          if( muonSummary1 ) nhits1 = muonSummary1->netaHits()+ muonSummary1->nphiHits();
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(track,tmpSummary);
          if( tmpSummary.muonTrackSummary() ) muonSummary1 = tmpSummary.muonTrackSummary();
          if( muonSummary1 ) nhits1 = muonSummary1->netaHits()+ muonSummary1->nphiHits();
        }

        int nhits2 = -1;
        const Trk::TrackSummary* summary2 = track.trackSummary();
        const Trk::MuonTrackSummary* muonSummary2 = 0;
        if( summary2 ){
          if( summary2->muonTrackSummary() ) muonSummary2 = summary2->muonTrackSummary();
          else{
            Trk::TrackSummary* tmpSum = const_cast<Trk::TrackSummary*>(summary2);
            if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(track,*tmpSum);
            if( tmpSum->muonTrackSummary() ) muonSummary2 = tmpSum->muonTrackSummary();
          }
          if( muonSummary2 ) nhits2 = muonSummary2->netaHits()+ muonSummary2->nphiHits();
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(track,tmpSummary);
          if( tmpSummary.muonTrackSummary() ) muonSummary2 = tmpSummary.muonTrackSummary();
          if( muonSummary2 ) nhits2 = muonSummary2->netaHits()+ muonSummary2->nphiHits();
        }

        if( nhits0 != -1 && nhits1 != -1 && nhits2 != -1 ) {
          if( nhits0 == nhits1 && nhits0 == nhits2 ) doSelection = true;
        }
       
        if( !doSelection ) ATH_MSG_WARNING( "Ndof changed after refit" );

      }
      
      if( doSelection ){

        // check whether the chi2 after refit is smaller than original even if above threshold. If so use refit
        bool firstIsBest = true;
        if( !fq1 && fq2 ) firstIsBest = false;
        if( fq1 && fq2 )  firstIsBest = fq1->chiSquared() < fq2->chiSquared();
        
        double chi2Refit = firstIsBest ? fq1->chiSquared() : fq2->chiSquared();
        if( chi2Refit < fq0->chiSquared() ){
          if( firstIsBest ) {
            ATH_MSG_DEBUG("Keeping precise refit");
            delete result2;
            ++m_nbetterPreciseFit;
            return const_cast<Trk::Track*>(result1);    
          }else{
            ATH_MSG_DEBUG("Keeping loose refit");
            delete result1;
            ++m_nbetterFit;
            return const_cast<Trk::Track*>(result2);    
          }
        }
      }
      // clean up 
      delete result1;
      delete result2;

      return 0;
    }
    // clean up
    if( result1 && result1 != refittedTrack ) delete result1;
//    if( result2 && result2 != refittedTrack ) delete result2;
    return const_cast<Trk::Track*>(refittedTrack);    
  }

}
