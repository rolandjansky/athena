/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonErrorOptimisationTool.h"

#include <iostream>

#include "TrkTrack/Track.h"
#include "TrkEventPrimitives/FitQuality.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkTrackSummary/MuonTrackSummary.h"

namespace Muon {

  MuonErrorOptimisationTool::MuonErrorOptimisationTool( const std::string& ty,const std::string& na,const IInterface* pa) : 
    AthAlgTool(ty,na,pa)
  {
    declareProperty("PrepareForFit",		         m_refitSettings.prepareForFit = true );
    declareProperty("RecreateStartingParameters",m_refitSettings.recreateStartingParameters = true );
    declareProperty("UpdateErrors",		           m_refitSettings.updateErrors = true );
    declareProperty("RemoveOutliers",            m_refitSettings.removeOutliers = false );
    declareProperty("RemoveOtherSectors",        m_refitSettings.removeOtherSectors = false);
    declareProperty("RemoveBarrelEndcapOverlap", m_refitSettings.removeBarrelEndcapOverlap = false);
    declareProperty("RemoveBEE",                 m_refitSettings.removeBEE = false);
    declareProperty("DeweightOtherSectors",      m_refitSettings.deweightOtherSectors = true);

   declareInterface<IMuonErrorOptimisationTool>(this);
  }


  StatusCode MuonErrorOptimisationTool::initialize()
  {

    ATH_MSG_INFO( "Initializing MuonErrorOptimisationTool" );
    
    ATH_CHECK( m_printer.retrieve() );
    ATH_CHECK( m_edmHelperSvc.retrieve() );
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

  std::unique_ptr<Trk::Track> MuonErrorOptimisationTool::optimiseErrors( Trk::Track* track ) const {

    if( m_refitTool.empty() ) return std::unique_ptr<Trk::Track>();
    const Trk::Perigee* pp = track->perigeeParameters();
    bool isLowPt = false;
    if( pp && pp->momentum().mag() < m_lowPtThreshold ) isLowPt = true;
    if( isLowPt ) ++m_nrefitAllLowPt;
    else          ++m_nrefitAll;

    std::unique_ptr<Trk::Track> result1;
    std::unique_ptr<Trk::Track> result2;
    
    // first refit with precise errors
    IMuonRefitTool::Settings settings = m_refitSettings;
    settings.broad = false;
    std::unique_ptr<Trk::Track> refittedTrack = m_refitTool->refit(track,&settings);    
    if( refittedTrack ){
    
      // check whether it is ok
      if( !m_edmHelperSvc->goodTrack(*refittedTrack,m_chi2NdofCutRefit) ) {
        ATH_MSG_VERBOSE("Precise fit bad " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
	
        // if not delete track
	result1.swap(refittedTrack);
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
      settings.broad = true;
      refittedTrack = m_refitTool->refit(track,&settings);    
      if( refittedTrack ) {
      
        // check whether it is ok
        if( !m_edmHelperSvc->goodTrack(*refittedTrack,m_chi2NdofCutRefit) ) {
          ATH_MSG_VERBOSE("Loose fit bad " << std::endl << m_printer->print(*refittedTrack) << std::endl << m_printer->printStations(*refittedTrack));
          // if not delete track
	  result2.swap(refittedTrack);
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
    if( !refittedTrack || *refittedTrack->perigeeParameters() == *track->perigeeParameters() ){
      
      // check if any refit succeeded
      if( !result1 && !result2 ) return std::unique_ptr<Trk::Track>();

      // now compare chi2
      const Trk::FitQuality* fq0 = track->fitQuality();
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
        Trk::TrackSummary* summary0 = track->trackSummary();
        Trk::MuonTrackSummary* muonSummary0 = 0;
        if( summary0 ){
          if( summary0->muonTrackSummary() ) {
            muonSummary0 = summary0->muonTrackSummary();
            if( muonSummary0 ) nhits0 = muonSummary0->netaHits()+ muonSummary0->nphiHits();
          }else{
            Trk::TrackSummary tmpSum(*summary0);
            m_trackSummaryTool->addDetailedTrackSummary(*track,tmpSum);
            if( tmpSum.muonTrackSummary() ) nhits0 = muonSummary0->netaHits()+ muonSummary0->nphiHits();
          }
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(*track,tmpSummary);
          if( tmpSummary.muonTrackSummary() ) muonSummary0 = tmpSummary.muonTrackSummary();
          if( muonSummary0 ) nhits0 = muonSummary0->netaHits()+ muonSummary0->nphiHits();
        }

        int nhits1 = -1;
        Trk::TrackSummary* summary1 = track->trackSummary();
        Trk::MuonTrackSummary* muonSummary1 = 0;
        if( summary1 ){
          if( summary1->muonTrackSummary() ) muonSummary1 = summary1->muonTrackSummary();
          else{
            Trk::TrackSummary* tmpSum = summary1;
            if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(*track,*tmpSum);
            if( tmpSum->muonTrackSummary() ) muonSummary1 = tmpSum->muonTrackSummary();
          }
          if( muonSummary1 ) nhits1 = muonSummary1->netaHits()+ muonSummary1->nphiHits();
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(*track,tmpSummary);
          if( tmpSummary.muonTrackSummary() ) muonSummary1 = tmpSummary.muonTrackSummary();
          if( muonSummary1 ) nhits1 = muonSummary1->netaHits()+ muonSummary1->nphiHits();
        }

        int nhits2 = -1;
        Trk::TrackSummary* summary2 = track->trackSummary();
        Trk::MuonTrackSummary* muonSummary2 = 0;
        if( summary2 ){
          if( summary2->muonTrackSummary() ) muonSummary2 = summary2->muonTrackSummary();
          else{
            Trk::TrackSummary* tmpSum = summary2;
            if( tmpSum ) m_trackSummaryTool->addDetailedTrackSummary(*track,*tmpSum);
            if( tmpSum->muonTrackSummary() ) muonSummary2 = tmpSum->muonTrackSummary();
          }
          if( muonSummary2 ) nhits2 = muonSummary2->netaHits()+ muonSummary2->nphiHits();
        }else{
          Trk::TrackSummary tmpSummary;
          m_trackSummaryTool->addDetailedTrackSummary(*track,tmpSummary);
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
            ++m_nbetterPreciseFit;
            return result1;    
          }else{
            ATH_MSG_DEBUG("Keeping loose refit");
            ++m_nbetterFit;
            return result2;
          }
        }
      }

      return std::unique_ptr<Trk::Track>();
    }
    return refittedTrack;
  }

}
