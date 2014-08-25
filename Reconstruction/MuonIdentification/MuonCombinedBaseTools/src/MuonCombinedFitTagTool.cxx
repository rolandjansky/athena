/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedFitTagTool
//  AlgTool performing combined fit of ID and MS tracks (Muid)
//  A CombinedFitTag is added to the InDetCandidate object.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>
#include "TrkTrack/TrackStateOnSurface.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"

#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"
#include "MuidInterfaces/IMuonMatchQuality.h"

#include "MuonCombinedToolInterfaces/IMuonCombinedTool.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"

#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedEvent/CombinedFitTag.h"
#include "TrkToolInterfaces/ITrackScoringTool.h"

#include "muonEvent/CaloEnergy.h"

#include "MuonCombinedFitTagTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCombinedFitTagTool::MuonCombinedFitTagTool(const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool"),
	m_tagTool("MuonCombined::MuonTrackTagTestTool/MuonTrackTagTestTool"),
	m_trackBuilder(""),
	m_outwardsBuilder(""),
	m_trackQuery("Rec::MuonTrackQuery/MuonTrackQuery"),
	m_momentumBalanceTool("Rec::MuonMomentumBalanceSignificanceTool/MuonMomentumBalanceSignifTool"),
	m_muonRecovery(""),
	m_matchQuality("Rec::MuonMatchQuality/MuonMatchQuality"),
	m_trackScoringTool("Muon::MuonTrackScoringTool/MuonTrackScoringTool")
  {
    declareInterface<IMuonCombinedTagTool>(this);
    declareProperty("Printer",                  m_printer);

    declareProperty("TrackBuilder",		m_trackBuilder);
    declareProperty("OutwardsTrackBuilder",	m_outwardsBuilder);
    declareProperty("TrackQuery",		m_trackQuery);
    declareProperty("MomentumBalanceTool",	m_momentumBalanceTool);
    declareProperty("MuonRecovery",		m_muonRecovery);
    declareProperty("MatchQuality",		m_matchQuality);
    declareProperty("TrackScoringTool",         m_trackScoringTool);
    declareProperty("BadFitChi2",		m_badFitChi2 = 2.5);
    declareProperty("MomentumBalanceCut",	m_momentumBalanceCut = 6.0);
    declareProperty("IndetPullCut", 		m_indetPullCut = 6.0);
    declareProperty("MatchChiSquaredCut",	m_matchChiSquaredCut = 30.0);
  }

  MuonCombinedFitTagTool::~MuonCombinedFitTagTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCombinedFitTagTool::initialize() {
    ATH_MSG_INFO( "Initializing MuonCombinedFitTagTool - package version " << PACKAGE_VERSION );

    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_tagTool.retrieve());
    ATH_CHECK(m_trackBuilder.retrieve());
    ATH_CHECK(m_outwardsBuilder.retrieve());
    ATH_CHECK(m_trackQuery.retrieve());
    ATH_CHECK(m_momentumBalanceTool.retrieve());
    ATH_CHECK(m_muonRecovery.retrieve()); 
    ATH_CHECK(m_matchQuality.retrieve());
    ATH_CHECK(m_trackScoringTool.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonCombinedFitTagTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCombinedFitTagTool::combine( const MuonCandidate& muonCandidate, const std::vector<InDetCandidate*>& indetCandidates ) {  

    CombinedFitTag* bestTag=0;
    InDetCandidate* bestCandidate=0;

    // loop over ID candidates
    for( auto idTP : indetCandidates ){
      
      // ensure that the id trackparticle has a track
      if( ! idTP->indetTrackParticle().track() ) continue;
      
      // fit the combined ID-MS track
      const Trk::Track* combinedTrack = buildCombinedTrack (*(idTP->indetTrackParticle().track()),
							    muonCandidate.muonSpectrometerTrack(),
							    muonCandidate.extrapolatedTrack());
      if( !combinedTrack ) {
	ATH_MSG_DEBUG("Combination fit failed");
	continue;
      }

      // calculate track score
      Trk::TrackScore score = m_trackScoringTool->score(*combinedTrack,true);

      // add fit info into tag object
      CombinedFitTag* currentTag = new CombinedFitTag(xAOD::Muon::MuidCo, muonCandidate, *combinedTrack, score);

      // re-fit standalone track (if needed) and store output into tag object
      evaluateMatchProperties(*currentTag, *(idTP->indetTrackParticle().track()));
      
      // select the best combined track
      if(!bestCandidate || bestMatchChooser(*idTP, *currentTag,
					    *bestCandidate, *bestTag)) {
	if( bestTag ) delete bestTag;
	bestCandidate = idTP;
	bestTag       = currentTag;
      }else{
	delete currentTag;
      }
    }
    
    if( bestCandidate ){
      double outerMatchChi2 = 1e19;
      // take the best MS Track, first the update extrapolated, than the extrapolated, last the spectrometer track
      const Trk::Track* extrapolatedTrack = bestTag->updatedExtrapolatedTrack();
      if( !extrapolatedTrack ) extrapolatedTrack = muonCandidate.extrapolatedTrack();
      if( !extrapolatedTrack ) extrapolatedTrack = &muonCandidate.muonSpectrometerTrack();

      if( bestCandidate->indetTrackParticle().trackLink().isValid() && extrapolatedTrack ){
	outerMatchChi2 = m_tagTool->chi2(*bestCandidate->indetTrackParticle().track(),*extrapolatedTrack);
      }
      ATH_MSG_DEBUG("Combined Muon with ID " << m_printer->print(bestCandidate->indetTrackParticle().perigeeParameters())
		    << " match chi2 " << bestTag->matchChi2() << " outer match " << outerMatchChi2 );
      bestCandidate->addTag(*bestTag);
    }   
  }





  const Trk::Track* MuonCombinedFitTagTool::buildCombinedTrack(const Trk::Track& indetTrack,					     
							       const Trk::Track& spectrometerTrack,
							       const Trk::Track* extrapolatedTrack) const
  {
    // if no extrapolation is available
    if(!extrapolatedTrack) extrapolatedTrack = &spectrometerTrack;

    // build and fit the combined track
    const Trk::Track* combinedTrack	= 0;
    double	      combinedFitChi2	= 9999.;
    if (! m_trackBuilder.empty()) {
      combinedTrack = m_trackBuilder->combinedFit(indetTrack,
						  *extrapolatedTrack,
						  spectrometerTrack);
      if (combinedTrack && combinedTrack->fitQuality()) {
	combinedTrack->info().addPatternReco(extrapolatedTrack->info());
	combinedFitChi2 = combinedTrack->fitQuality()->chiSquared() /
	  combinedTrack->fitQuality()->doubleNumberDoF();
      }
    }
    if (combinedFitChi2 > m_badFitChi2 && ! m_outwardsBuilder.empty()) {
      const Trk::Track* outwardsTrack	= m_outwardsBuilder->combinedFit(indetTrack,
									 *extrapolatedTrack,
									 spectrometerTrack);
      if (outwardsTrack &&
	  outwardsTrack->fitQuality()->chiSquared() /
	  outwardsTrack->fitQuality()->doubleNumberDoF() < combinedFitChi2) {
	ATH_MSG_VERBOSE( "buildCombinedTrack: choose outwards track" );
	outwardsTrack->info().addPatternReco(spectrometerTrack.info());
	delete combinedTrack;
	combinedTrack = outwardsTrack;
      }else{
	delete outwardsTrack;
      }
    }
    
    // filter out rubbish fits
    if (combinedTrack && combinedTrackQualityCheck(*combinedTrack,indetTrack)) {
      combinedTrack->info().addPatternReco(indetTrack.info());
      combinedTrack->info().setParticleHypothesis(Trk::muon);
      combinedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidCombined);
      return combinedTrack;
    }
    
    // try recovery
    if (! m_muonRecovery.empty())
      {
	delete combinedTrack;
	combinedTrack	= m_muonRecovery->recoverableMatch(indetTrack,spectrometerTrack);
	if (combinedTrack && combinedTrackQualityCheck(*combinedTrack,indetTrack))
	  {
	    combinedTrack->info().addPatternReco(indetTrack.info());
	    combinedTrack->info().addPatternReco(spectrometerTrack.info());
	    combinedTrack->info().setParticleHypothesis(Trk::muon);
	    combinedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidCombined);
	    return combinedTrack;
	  }
      }
    
    delete combinedTrack;
    return 0;
  }


  bool MuonCombinedFitTagTool::combinedTrackQualityCheck(const Trk::Track& combinedTrack,
							 const Trk::Track& indetTrack) const
  {
    // require calo correctly associated to track
    if (! m_trackQuery->isCaloAssociated(combinedTrack)) return false;
    
    // loose cut on momentumBalanceSignificance
    double significance	= m_momentumBalanceTool->momentumBalanceSignificance(combinedTrack);
    if (fabs(significance) > m_momentumBalanceCut) {
      ATH_MSG_DEBUG(" combinedTrackQualityCheck fails with momentumBalanceSignificance "
		    << significance );
      return false;
    }
    
    // loose cut on indet/combined q/p pull (not applicable to indet line fit)
    if (! indetTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)) {
      const Trk::Perigee* combinedPerigee = combinedTrack.perigeeParameters();
      const Trk::Perigee* indetPerigee = indetTrack.perigeeParameters();
      if (combinedPerigee->covariance() && indetPerigee->covariance()) {
	double dpOverP2	= (*combinedPerigee->covariance())(4,4) *
	  combinedPerigee->momentum().mag2();
	if (dpOverP2 < 1.E-6) {
	  // fail with unphysical momentum covariance
	  ATH_MSG_WARNING( "combinedTrackQualityCheck: fail with unphysical momentum covariance");
	  return false;
	}
	double sigma = sqrt((*indetPerigee->covariance())(4,4));
	double pull  = (combinedPerigee->parameters()[Trk::qOverP] -
			indetPerigee->parameters()[Trk::qOverP] ) / sigma;
	
	if (fabs(pull) > m_indetPullCut) {
	  // fail with too high momentum pull
	  ATH_MSG_WARNING("combinedTrackQualityCheck: fail with momentum pull above cut: " << pull);
	  return false;
	}
      }
    }    
    return true;
  }


  void MuonCombinedFitTagTool::evaluateMatchProperties(CombinedFitTag& tag, const Trk::Track& idTrack) 
  {
    // evaluate field integral and momentum balance significance for combined track
    tag.fieldIntegral(m_trackQuery->fieldIntegral(tag.combinedTrack()));
    tag.momentumBalanceSignificance(m_momentumBalanceTool->momentumBalanceSignificance(tag.combinedTrack()));
    
    // keep original SA fit if no change to MS or Calo TSOS
    if (! extrapolatedNeedsRefit(tag.combinedTrack(), tag.muonCandidate().extrapolatedTrack()) ) {

      // if an extrapolated track is available, evaluate match properties
      if(tag.muonCandidate().extrapolatedTrack()) {
	double matchChi2 = m_matchQuality->innerMatchChi2(idTrack, *tag.muonCandidate().extrapolatedTrack());
	int matchDoF     = m_matchQuality->innerMatchDOF(idTrack, *tag.muonCandidate().extrapolatedTrack());
	double matchProb = m_matchQuality->innerMatchProbability(idTrack, *tag.muonCandidate().extrapolatedTrack());      

	// store the inner matching quality in the tag object
	tag.innerMatch(matchChi2, matchDoF, matchProb);
	return;
      }
    }
    
    // refit extrapolated from combined track (i.e. after cleaning)
    const Trk::Track*  refittedExtrapolatedTrack = 0;
    if (! m_trackBuilder.empty())                                   refittedExtrapolatedTrack = m_trackBuilder->standaloneRefit(tag.combinedTrack());
    if (! refittedExtrapolatedTrack && ! m_outwardsBuilder.empty()) refittedExtrapolatedTrack = m_outwardsBuilder->standaloneRefit(tag.combinedTrack());
    
    // include vertex region pseudo for extrapolation failure
    unsigned numberPseudo = tag.muonCandidate().extrapolatedTrack() ? 
      m_trackQuery->numberPseudoMeasurements(*tag.muonCandidate().extrapolatedTrack()) : 1;

    // get track quality and store
    if (refittedExtrapolatedTrack) {

      // store the refit track in the tag object
      tag.setUpdatedExtrapolatedTrack(refittedExtrapolatedTrack);

      double matchChi2 = m_matchQuality->innerMatchChi2(idTrack, *refittedExtrapolatedTrack);
      int matchDoF     = m_matchQuality->innerMatchDOF(idTrack, *refittedExtrapolatedTrack);
      double matchProb = m_matchQuality->innerMatchProbability(idTrack, *refittedExtrapolatedTrack);
      
      // store the inner matching quality in the tag object
      tag.innerMatch(matchChi2, matchDoF, matchProb);
      
      // print comparison with original track
      if(tag.muonCandidate().extrapolatedTrack()) {
	double oldmatchChi2 = m_matchQuality->innerMatchChi2(idTrack, *tag.muonCandidate().extrapolatedTrack());
	
	ATH_MSG_VERBOSE( " evaluateMatchProperties: chi2 re-evaluated from "
			 << oldmatchChi2 << " to " << matchChi2 );
	
	if (matchChi2 > 1.1*oldmatchChi2)
	  ATH_MSG_DEBUG( " evaluateMatchProperties: chi2 got worse: from "
			 << oldmatchChi2 << " to " << matchChi2 );
      }else
	ATH_MSG_VERBOSE( " evaluateMatchProperties: added new extrapolated track with chi2 "<< matchChi2 );
      
    }else if(!numberPseudo) {
      // failed re-evaluation of match chi2
      ATH_MSG_WARNING( " evaluateMatchProperties: fail re-evaluation of match chi2");
    }
  }
  
  bool MuonCombinedFitTagTool::extrapolatedNeedsRefit(const Trk::Track& combTrack,
						      const Trk::Track* extrTrack) const
  {
    // will refit if extrapolated track was definitely bad
    if (! extrTrack) return true;
    if (! m_trackQuery->isCaloAssociated(*extrTrack)) return true;
    
    // otherwise will keep original SA fit if no change to MS or Calo TSOS
    const Trk::Track& originalTrack = *extrTrack;
    
    // refit if bad extrapolated fit - otherwise no refit if bad combined fit
    const Trk::FitQuality* fitQuality = originalTrack.fitQuality();
    if (! fitQuality
	|| ! fitQuality->numberDoF()
	|| fitQuality->chiSquared() / fitQuality->doubleNumberDoF() > m_badFitChi2)	return true;
    
    if (combTrack.fitQuality()->chiSquared()
	/ combTrack.fitQuality()->doubleNumberDoF() > m_badFitChi2)			return true;
    
    // check if need to update calo association
    const CaloEnergy* caloEnergyCombined	= m_trackQuery->caloEnergy(combTrack);
    const CaloEnergy* caloEnergyExtrapolated	= m_trackQuery->caloEnergy(originalTrack);
    if (! caloEnergyCombined || ! caloEnergyExtrapolated) {
      // no refit for combined track without CaloEnergy
      ATH_MSG_VERBOSE("extrapolatedNeedsRefit: no refit for combined track without CaloEnergy");
      return false;
    }
    double deltaE = caloEnergyExtrapolated->deltaE() - caloEnergyCombined ->deltaE();
    if (fabs(deltaE) > 0.3*caloEnergyExtrapolated->sigmaDeltaE()) {
      ATH_MSG_VERBOSE( "extrapolatedNeedsRefit: caloEnergy difference " << deltaE
		       << "  sigma " << caloEnergyExtrapolated->sigmaDeltaE()
		       << "  ratio " << deltaE/caloEnergyExtrapolated->sigmaDeltaE());
      return true;
    }
    
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator o =
	originalTrack.trackStateOnSurfaces()->rbegin();
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator c =
	combTrack.trackStateOnSurfaces()->rbegin();
    for (; o != originalTrack.trackStateOnSurfaces()->rend(); ++o) {
      if (dynamic_cast<const Trk::PerigeeSurface*>(&(**o).surface()))	break;
      
      // compare measurements
      if ((**o).measurementOnTrack()) {

	// check measurements in phase
	while (! (**c).measurementOnTrack()) ++c;
	double separation = ((**o).trackParameters()->associatedSurface().center() -
			     (**c).trackParameters()->associatedSurface().center()).mag();
	if (fabs(separation) > 1.*CLHEP::mm) {
	  ATH_MSG_VERBOSE("extrapolatedNeedsRefit: measurement out-of-phase: "
			  << " separation " << separation << "  extrap "
			  << (**o).trackParameters()->associatedSurface().center()
			  << "   comb " << (**c).trackParameters()->associatedSurface().center());
	  return true;
	}
	
	// different outlier 
	if ((**o).type(Trk::TrackStateOnSurface::Outlier) !=
	    (**c).type(Trk::TrackStateOnSurface::Outlier)) {
	  if ((**c).type(Trk::TrackStateOnSurface::Outlier)) {
	    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: outlier only on combined track " );
	  }else{
	    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: outlier only on extrapolated track " );
	  }
	  return true;
	}
	
	// drift sign flip 
	if (dynamic_cast<const Muon::MdtDriftCircleOnTrack*>((**o).measurementOnTrack())) {
	  if ((**o).measurementOnTrack()->localParameters()[Trk::driftRadius] *
	      (**c).measurementOnTrack()->localParameters()[Trk::driftRadius] < 0.) {
	    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: drift sign flip " );
	    return true;
	  }
	}
	++c;
      }
    }    
    return false;    
  }


  bool MuonCombinedFitTagTool::bestMatchChooser(const InDetCandidate& curCandidate, const CombinedFitTag& curTag,
						const InDetCandidate& /*bestCandidate*/, const CombinedFitTag& bestTag) const
    
  {
    // pointers to extrapolated track
    const Trk::Track* curExtrTrack = curTag.updatedExtrapolatedTrack() ? 
      curTag.updatedExtrapolatedTrack() : curTag.muonCandidate().extrapolatedTrack();
    const Trk::Track* bestExtrTrack = bestTag.updatedExtrapolatedTrack() ? 
      bestTag.updatedExtrapolatedTrack() : bestTag.muonCandidate().extrapolatedTrack();

    // 1 current
    // 2 best
    // returned bool: true means current is better; false means "best is better"

    double matchChiSq1			= curTag.matchChi2();
    double matchChiSq2			= bestTag.matchChi2();
    const Trk::TrackSummary* summary1	= curTag.combinedTrack().trackSummary();
    const Trk::TrackSummary* summary2	= bestTag.combinedTrack().trackSummary();
    ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2);
    if (summary1 && summary2)
    {
	ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2
			<< "  numTRTHits " << summary1->get(Trk::numberOfTRTHits)
			<< "  " << summary2->get(Trk::numberOfTRTHits)
			<< "  field integrals: ID  "
			<< curTag.fieldIntegral().betweenInDetMeasurements()
			<< "  " << bestTag.fieldIntegral().betweenInDetMeasurements()
			<< "  MS "
			<< curTag.fieldIntegral().betweenSpectrometerMeasurements()
			<< "  " << bestTag.fieldIntegral().betweenSpectrometerMeasurements() );
    }
    else
    {
	ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2);
    }
    
    // selection when only one match has a good combined fit
    double fitChiSq1	= curTag.combinedTrack().fitQuality()->chiSquared() /
			  curTag.combinedTrack().fitQuality()->doubleNumberDoF();
    int numberDoF1	= curTag.combinedTrack().fitQuality()->numberDoF();
    double fitChiSq2	= bestTag.combinedTrack().fitQuality()->chiSquared() /
			  bestTag.combinedTrack().fitQuality()->doubleNumberDoF();
    int numberDoF2	= bestTag.combinedTrack().fitQuality()->numberDoF();
    ATH_MSG_VERBOSE("bestMatchChooser: fitChiSq " << fitChiSq1 << "  " << fitChiSq2);
    if (fabs(fitChiSq1 - fitChiSq2) > m_badFitChi2) {
      if (fitChiSq1 < m_badFitChi2) {
	if (matchChiSq1 > matchChiSq2 && matchChiSq2 < m_matchChiSquaredCut) { // may want to suppress this warning!
	  ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq as better fitChiSq. "
			  << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			  << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
			  << fitChiSq2 << "/" << numberDoF2);
	}
	return true;
      }
      if (fitChiSq2 < m_badFitChi2) {
	if (matchChiSq1 < matchChiSq2
	    && matchChiSq1 < m_matchChiSquaredCut) { // may want to suppress this warning!
	  ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq as better fitChiSq. "
			  << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			  << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
			  << fitChiSq2 << "/" << numberDoF2);
	}
	return false;
      }
    }
        
    // selection when only one match has a good match chi2
    if (fabs(matchChiSq1 - matchChiSq2) > m_matchChiSquaredCut) {
      if (matchChiSq1 < m_matchChiSquaredCut) return true;
      if (matchChiSq2 < m_matchChiSquaredCut) return false;
    }
    
    // energy balance (absolute)
    // track length:
    //          field integral
    // 		# MS stations
    // 		pixel hits (-outliers)
    // 		trt drift hits + outliers

    // protect momentum balance and field integral when magnets off:
    if( ! curCandidate.indetTrackParticle().track()->info().trackProperties(Trk::TrackInfo::StraightTrack) ) {
      double cutRatio   = 1.5;
      double integral1	= 0.;
      double integral2	= 0.;
 
      if (curExtrTrack
	  && ! curExtrTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)
	  && bestExtrTrack
	  && ! bestExtrTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {

	// selection when only one match has good momentum balance or a significantly better balance
	ATH_MSG_VERBOSE( "bestMatchChooser: momentumBalanceSignificance "
			 << curTag.momentumBalanceSignificance()
			 << "  " << bestTag.momentumBalanceSignificance());
	double significanceCut	= 2.0;
	double significance1	= fabs(curTag.momentumBalanceSignificance());
	double significance2	= fabs(bestTag.momentumBalanceSignificance());
	if (fabs(significance1 - significance2) > significanceCut) {
	  if (significance1 < significanceCut) {
	    if (matchChiSq1	> matchChiSq2
		&& matchChiSq2	< m_matchChiSquaredCut) {
	      // NOT choosing bestMatchChi2:
	      ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq as better momentum balance. "
			      << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			      << "   momentumBalanceSignificance 1,2 "
			      << curTag.momentumBalanceSignificance()
			      << ", " << bestTag.momentumBalanceSignificance());
	    }
	    return true;
	  }
	  if (significance2 < significanceCut) {
	    if (matchChiSq1	< matchChiSq2
		&& matchChiSq1	< m_matchChiSquaredCut) {
	      // NOT choosing bestMatchChi2:
	      ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq as better momentum balance. "
			      << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			      << "   momentumBalanceSignificance 1,2 "
			      << curTag.momentumBalanceSignificance()
			      << ", " << bestTag.momentumBalanceSignificance());
	    }
	    return false;
	  }
	}
	
	// keep significantly larger measured field integral
	//   for MS
	ATH_MSG_VERBOSE( "bestMatchChooser: spectrometer field integral ratio ");
	integral1	= fabs(curTag.fieldIntegral().betweenSpectrometerMeasurements());
	integral2	= fabs(bestTag.fieldIntegral().betweenSpectrometerMeasurements());
	if (integral1 > cutRatio*integral2)	return true;
	if (integral2 > cutRatio*integral1)	return false;
      }
      //   for indet
      ATH_MSG_VERBOSE( "bestMatchChooser: indet field integral ratio ");
      integral1	= fabs(curTag.fieldIntegral().betweenInDetMeasurements());
      integral2	= fabs(bestTag.fieldIntegral().betweenInDetMeasurements());
      if (integral1 > cutRatio*integral2)	return true;
      if (integral2 > cutRatio*integral1)	return false;
    }
    
    // repeat fit/match quality selection with sharper cuts (times 2)
    ATH_MSG_VERBOSE( "bestMatchChooser: sharp fit chi2 cut " );
    if (fabs(fitChiSq1 - fitChiSq2) > 0.5*m_badFitChi2) { 
      if (fitChiSq1 < 0.5*m_badFitChi2) {
	if (matchChiSq1	> matchChiSq2
	    && matchChiSq2 < m_matchChiSquaredCut) {
	  // NOT choosing bestMatchChi2:
	  ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq according to overall quality. "
			  << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			  << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
			  << fitChiSq2 << "/" << numberDoF2);
	}
	return true;
      }
      if (fitChiSq2 < 0.5*m_badFitChi2) {
	if (matchChiSq1	< matchChiSq2
	    && matchChiSq1	< m_matchChiSquaredCut) {
	  // NOT choosing bestMatchChi2:
	  ATH_MSG_WARNING("bestMatchChooser: choose worse, but acceptable, matchChiSq according to overall quality. "
			  << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			  << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
			  << fitChiSq2 << "/" << numberDoF2);
	}
	return false;
      }
    }
    
    ATH_MSG_VERBOSE( "bestMatchChooser: sharp match chi2 cut ");		     
    if (fabs(matchChiSq1 - matchChiSq2) > 0.5*m_matchChiSquaredCut) {
      if (matchChiSq1 < 0.5*m_matchChiSquaredCut) return true;
      if (matchChiSq2 < 0.5*m_matchChiSquaredCut) return false;
    }
    
    // track quality:
    // pixel holes (outliers)
    // silicon holes + outliers
    // trt drift hits + outliers

    // kink finding:
    // neighbour signif
    // curvature signif

    // energy balance (absolute)

    // field off protection
    if (curExtrTrack
	&& curExtrTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)
	&& bestExtrTrack
	&& bestExtrTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)) {
      // best fit chi2
      if (fitChiSq1 < fitChiSq2) return true;
      return false;
    }else{	
      // best match chi2
      if (matchChiSq1 < matchChiSq2) return true;
      return false;
    }
  }
  
  
}	// end of namespace
