/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// OutwardsCombinedMuonTrackBuilder
//  AlgTool gathering  material effects along a combined muon track, in 
//  particular the TSOS'es representing the calorimeter energy deposit and
//  Coulomb scattering.
//  The resulting track is fitted at the IP using the ITrackFitter interface.
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "CLHEP/Units/SystemOfUnits.h"

#include "MuidTrackBuilder/OutwardsCombinedMuonTrackBuilder.h"

#include "MuonRecToolInterfaces/IMuonTrackCleaner.h"
#include "MuonRecToolInterfaces/IMuonHoleRecoveryTool.h"
#include "MuonRecToolInterfaces/IMuonErrorOptimisationTool.h"
#include "TrkToolInterfaces/ITrackSummaryTool.h"
#include "TrkTrack/Track.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkMeasurementBase/MeasurementBase.h"

namespace Rec
{
    
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

OutwardsCombinedMuonTrackBuilder::OutwardsCombinedMuonTrackBuilder (const std::string&	type,
								    const std::string&	name, 
								    const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_cleaner		(""),
	m_fitter		(""),
	m_trackSummary		(""),
        m_muonHoleRecovery      (""),
        m_muonErrorOptimizer    (""),
	m_allowCleanerVeto	(true),
	m_cleanCombined		(true),
	m_recoverCombined	(false)
{
    declareInterface<ICombinedMuonTrackBuilder>(this);
    declareInterface<Trk::ITrackFitter>(this);
    declareProperty("Cleaner",			m_cleaner);
    declareProperty("Fitter",			m_fitter);
    declareProperty("TrackSummaryTool",		m_trackSummary);
    declareProperty("MuonHoleRecovery",         m_muonHoleRecovery);
    declareProperty("AllowCleanerVeto",		m_allowCleanerVeto);
    declareProperty("CleanCombined",		m_cleanCombined);
    declareProperty("RecoverCombined",		m_recoverCombined);
    declareProperty("MuonErrorOptimizer",       m_muonErrorOptimizer);
}

OutwardsCombinedMuonTrackBuilder::~OutwardsCombinedMuonTrackBuilder (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
OutwardsCombinedMuonTrackBuilder::initialize()
{
    ATH_MSG_INFO( "Initializing OutwardsCombinedMuonTrackBuilder"
		   << " - package version " << PACKAGE_VERSION );
    msg(MSG::INFO) << " with options: ";
    if (m_allowCleanerVeto)	msg(MSG::INFO) << " AllowCleanerVeto";
    if (m_cleanCombined)	msg(MSG::INFO) << " CleanCombined";
    msg(MSG::INFO) << endreq;

    if (!m_cleaner.empty()){
    if (m_cleaner.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_cleaner );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_INFO( "Retrieved tool " << m_cleaner );
    }
    }
    if (! m_muonHoleRecovery.empty())
    {
        if (m_muonHoleRecovery.retrieve().isFailure())
        {
            ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonHoleRecovery );
            return StatusCode::FAILURE;
        }
        else
        {
            ATH_MSG_INFO( "Retrieved tool " << m_muonHoleRecovery );
        }
    }

    if (! m_muonErrorOptimizer.empty())
    {
        if (m_muonErrorOptimizer.retrieve().isFailure())
        {
            ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonErrorOptimizer );
            return StatusCode::FAILURE;
        }
        else
        {
            ATH_MSG_INFO( "Retrieved tool " << m_muonErrorOptimizer );
        }
    }


    if (!m_fitter.empty()){
    if (m_fitter.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_fitter );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_INFO( "Retrieved tool " << m_fitter );
    }
    }
    if (!m_trackSummary.empty()){
    if (m_trackSummary.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackSummary );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackSummary );
    }
    }
    return StatusCode::SUCCESS;
}

StatusCode
OutwardsCombinedMuonTrackBuilder::finalize()
{
 
    
    return StatusCode::SUCCESS;
}

/**ICombinedMuonTrackBuilder interface: build and fit combined ID/Calo/MS track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::combinedFit (const Trk::Track& indetTrack,
					       const Trk::Track& /*extrapolatedTrack*/,
				       const Trk::Track& spectrometerTrack) const
{
    ATH_MSG_VERBOSE( "combinedFit:: " );


    
    Trk::Track* combinedTrack	= fit(indetTrack, spectrometerTrack, m_cleanCombined, Trk::muon);

 
    // add the track summary
   
    if (combinedTrack) m_trackSummary->updateTrack(*combinedTrack);
    //if (combinedTrack) std::cout << "in builder, track: " << *combinedTrack << " summary: " << *combinedTrack->trackSummary() << std::endl;
    return combinedTrack;
}

/**ICombinedMuonTrackBuilder interface:
   build and fit indet track extended to include MS Measurement set.
   Adds material effects as appropriate plus calo energy-loss treatment */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::indetExtension (const Trk::Track&	      indetTrack,
						  const Trk::MeasurementSet&  spectrometerMeas,
						  const Trk::TrackParameters* /*innerParameters*/,
						  const Trk::TrackParameters* /*middleParameters*/,
						  const Trk::TrackParameters* /*outerParameters*/) const
{
    ATH_MSG_VERBOSE( "indetExtension fit::" );
    DataVector<const Trk::TrackStateOnSurface> *trajectory=new DataVector<const Trk::TrackStateOnSurface>; 
    for (int i=0;i<(int)spectrometerMeas.size();i++) {
      std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typeM;
      typeM.set(Trk::TrackStateOnSurface::Measurement);
      Trk::TrackStateOnSurface *tsos=new Trk::TrackStateOnSurface(spectrometerMeas[i]->clone(),0,0,0,typeM);
      trajectory->push_back(tsos);
    }
    Trk::TrackInfo trackInfo(Trk::TrackInfo::Unknown,Trk::muon);
    Trk::Track inputtrack2(trackInfo,trajectory,0);
    Trk::Track *track=fit(indetTrack,inputtrack2,m_cleanCombined, Trk::muon);    
    return track;
}

/**ICombinedMuonTrackBuilder interface:
   propagate to perigee adding calo energy-loss and material to MS track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::standaloneFit	(const Trk::Track&	/*spectrometerTrack*/,
						 const Trk::Vertex*	/*vertex*/) const
{
  return 0;
}

/**ICombinedMuonTrackBuilder interface:
   refit a track removing any indet measurements with optional addition of pseudoMeasurements
       according to original extrapolation */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::standaloneRefit (const Trk::Track& /*track*/) const
{
    ATH_MSG_DEBUG( "standalone refit not implemented" );

    return 0;
}

/**ITrackFitter interface: refit a track */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::Track&		track,
			       const Trk::RunOutlierRemoval	runOutlier,
			       const Trk::ParticleHypothesis	particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting)
    {
	ATH_MSG_WARNING( " invalid particle hypothesis " << particleHypothesis
			 << " requested. Must be 0 or 2 (nonInteracting or muon) " );
	return 0;
    }

   
    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    
   
   
    
    // fit
    Trk::Track* fittedTrack = fitter->fit(track, false, particleHypothesis);
    if (! fittedTrack)	return 0;

    // track cleaning
    if (runOutlier)
    {
	// muon cleaner
	ATH_MSG_VERBOSE( " perform track cleaning... " );
        Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
	if (! cleanTrack)
	{
	    ATH_MSG_DEBUG( " cleaner veto " );
	    
	    if (m_allowCleanerVeto)
	    {
		delete fittedTrack;
		fittedTrack = 0;
	    }
	}
        else if (cleanTrack != fittedTrack)
        {
	    ATH_MSG_VERBOSE( " found and removed spectrometer outlier(s) " );

            delete fittedTrack;
            fittedTrack = cleanTrack;
        }

	//FIXME: provide indet cleaner
	ATH_MSG_VERBOSE( " finished cleaning" );
    }
    
    return fittedTrack;
}

/**ITrackFitter interface:
   fit a set of MeasurementBase objects with starting value for perigeeParameters */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::MeasurementSet&	/*measurementSet*/,
				       const Trk::TrackParameters&	/*perigeeStartValue*/,
				       const Trk::RunOutlierRemoval	/*runOutlier*/,
				       const Trk::ParticleHypothesis	/*particleHypothesis*/) const
{
    return 0;
}


/**ITrackFitter interface:  combined muon fit */
Trk::Track*
OutwardsCombinedMuonTrackBuilder::fit (const Trk::Track&		indetTrack,
				       const Trk::Track&		extrapolatedTrack,
				       const Trk::RunOutlierRemoval	runOutlier,
				       const Trk::ParticleHypothesis	particleHypothesis) const
{
    // check valid particleHypothesis
    if (particleHypothesis != Trk::muon && particleHypothesis != Trk::nonInteracting)
    {
	ATH_MSG_WARNING( " invalid particle hypothesis " << particleHypothesis
			 << " requested. Must be 0 or 2 (nonInteracting or muon) " );
	return 0;
    }

    ToolHandle<Trk::ITrackFitter> fitter = m_fitter;
    
    // fit
    Trk::Track* fittedTrack = fitter->fit(indetTrack, extrapolatedTrack, false, particleHypothesis);
    if (! fittedTrack)	return 0;
    if( !fittedTrack->perigeeParameters() ) {
      ATH_MSG_WARNING( " Fitter returned a track without perigee, failing fit");
      delete fittedTrack;
      return 0;
    }
    // track cleaning
    if (runOutlier)
    {
	// muon cleaner
	ATH_MSG_VERBOSE( " perform track cleaning... " );
        Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
	if (! cleanTrack)
	{
	    ATH_MSG_DEBUG( " cleaner veto " );
	    
	    if (m_allowCleanerVeto)
	    {
		delete fittedTrack;
		fittedTrack = 0;
	    }
	}
        else if (cleanTrack != fittedTrack)
        {
	    ATH_MSG_VERBOSE( "  found and removed spectrometer outlier(s) " );

            delete fittedTrack;
            fittedTrack = cleanTrack;
        }

	ATH_MSG_VERBOSE( " finished cleaning" );
    }
    if (m_recoverCombined && fittedTrack){
      Trk::Track* recoveredTrack = m_muonHoleRecovery->recover(*fittedTrack);
      double oldfitqual=fittedTrack->fitQuality()->chiSquared()/fittedTrack->fitQuality()->numberDoF();
      if (recoveredTrack && recoveredTrack!=fittedTrack){
        double newfitqual=recoveredTrack->fitQuality()->chiSquared()/recoveredTrack->fitQuality()->numberDoF();
        if (newfitqual<oldfitqual || newfitqual<1.5 || (newfitqual<2 && newfitqual<oldfitqual+.5)){
          delete fittedTrack;
          fittedTrack=recoveredTrack;
          /*if (runOutlier) { 
          Trk::Track* cleanTrack = m_cleaner->clean(*fittedTrack);
          if (cleanTrack && cleanTrack!=fittedTrack){
            delete fittedTrack;
            fittedTrack=cleanTrack;
          }
          }*/
        }
        else delete recoveredTrack;
      }   
    }
    if( fittedTrack && !fittedTrack->perigeeParameters() ) {
      ATH_MSG_WARNING( " Fitter returned a track without perigee, failing fit");
      delete fittedTrack;
      return 0;
    }
    if (runOutlier && fittedTrack) { 
      double fitqual=fittedTrack->fitQuality()->chiSquared()/fittedTrack->fitQuality()->numberDoF();
      if (fitqual>5 || (fittedTrack->perigeeParameters()->pT()<20000 && fitqual>2.5)) {
        delete fittedTrack;
        fittedTrack=0;
      }
      else {
	  //        double prevp=0;
        DataVector<const Trk::TrackStateOnSurface>::const_iterator itStates = fittedTrack->trackStateOnSurfaces()->begin();
        DataVector<const Trk::TrackStateOnSurface>::const_iterator endStates = fittedTrack->trackStateOnSurfaces()->end();
        for (;itStates!=endStates;itStates++) {
          if ((*itStates)->materialEffectsOnTrack()) {
            const Trk::MaterialEffectsOnTrack *meot=dynamic_cast<const Trk::MaterialEffectsOnTrack *>((*itStates)->materialEffectsOnTrack());
            if (meot->scatteringAngles() && !meot->energyLoss()) {
              double pullphi=std::abs(meot->scatteringAngles()->deltaPhi()/meot->scatteringAngles()->sigmaDeltaPhi());
              double pulltheta=std::abs(meot->scatteringAngles()->deltaTheta()/meot->scatteringAngles()->sigmaDeltaTheta());
              if (pullphi>7 || pulltheta>7) {
                delete fittedTrack;
                fittedTrack=0;
                break;
              }
            }
            /* if (!meot->scatteringAngles() && meot->energyLoss()) {
              double eloss=std::abs(meot->energyLoss()->deltaE())-prevp+std::abs(1/(*itStates)->trackParameters()->parameters()[Trk::qOverP]);
              double sigmae= (eloss<0) ? meot->energyLoss()->sigmaPlusDeltaE() : meot->energyLoss()->sigmaDeltaE();
              //std::cout << "eloss: " << eloss << " sigma: " << sigmae << " pull: " << std::abs(eloss)/sigmae << std::endl; 
              if (std::abs(eloss)/sigmae>3) {
                delete fittedTrack;
                fittedTrack=0;
                break;
              }
            } */
          }
          // if ((*itStates)->trackParameters()) prevp=std::abs(1/(*itStates)->trackParameters()->parameters()[Trk::qOverP]);
        }
      }
    }
    // final fit with optimized spectrometer errors 
    if (! m_muonErrorOptimizer.empty() && fittedTrack && ! fittedTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) { 
      ATH_MSG_VERBOSE( " perform spectrometer error optimization... " ); 
      Trk::Track* optimizedTrack = m_muonErrorOptimizer->optimiseErrors(*fittedTrack); 
      if (optimizedTrack) { 
        delete fittedTrack; 
        fittedTrack = optimizedTrack; 
      } 
    } 
    return fittedTrack;
}
    


}	// end of namespace
