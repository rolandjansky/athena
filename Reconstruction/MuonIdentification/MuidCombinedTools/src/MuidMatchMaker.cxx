/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuidMatchMaker
//  AlgTool to match all tracks from an extrapolated muon-spectrometer
//  collection to any corresponding tracks from an inner detector collection.
//  Perform a full track-fit to the resultant combined tracks.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include <vector>
#include "CLHEP/GenericFunctions/CumulativeChiSquare.hh"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "MuidCombinedTools/MuidMatchMaker.h"
#include "MuidEvent/IndetMatchCandidate.h"
#include "MuidEvent/MuidTrack.h"
#include "MuidEvent/MuidTrackContainer.h"
#include "MuidInterfaces/ICombinedMuonTrackBuilder.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
#include "MuidInterfaces/IMuidCaloTrackStateOnSurface.h"
#include "MuidInterfaces/IMuidMuonRecovery.h"
#include "MuonCombinedToolInterfaces/IMuonMomentumBalanceSignificance.h"
#include "MuidInterfaces/IMuonMatchQuality.h"
#include "MuonCombinedToolInterfaces/IMuonScatteringAngleSignificance.h"
#include "MuidInterfaces/IMuonTrackQuery.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "TrkEventPrimitives/JacobianThetaPToCotThetaPt.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/ITrackAmbiguityProcessorTool.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackInfo.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrackSummary/TrackSummary.h"
#include "TrkiPatFitterUtils/MessageHelper.h"
#include "iPatTrackParameters/PerigeeParameters.h"
#include "muonEvent/CaloEnergy.h"

namespace Rec
{
 
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

MuidMatchMaker::MuidMatchMaker (const std::string&	type,
				const std::string&	name, 
				const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_ambiguityProcessor	("Trk::SimpleAmbiguityProcessorTool/MuonAmbiProcessor"),
	m_caloEnergyParam	("Rec::MuidCaloEnergyParam/MuidCaloEnergyParam"),
	m_caloTSOS		("Rec::MuidCaloTrackStateOnSurface/MuidCaloTrackStateOnSurface"),
	m_intersector		("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
        m_magFieldSvc           ("AtlasFieldSvc",name),
	m_matchQuality		("Rec::MuonMatchQuality/MuonMatchQuality"),
	m_momentumBalanceTool	("Rec::MuonMomentumBalanceSignificanceTool/MuonMomentumBalanceSignifTool"),
	m_muonRecovery		(""),
	m_outwardsBuilder	(""),
	m_propagator		("Trk::IntersectorWrapper/IntersectorWrapper"),
	m_scatteringAngleTool	("Rec::MuonScatteringAngleSignificanceTool/MuonScatteringSignificance"),
	m_trackBuilder		(""),
	m_trackQuery		("Rec::MuonTrackQuery/MuonTrackQuery"),
	m_deltaEtaPreSelection	(0.5),			// crude deltaEta preselection cut
	m_deltaPhiPreSelection	(1.0),			// crude deltaPhi preselection cut
	m_fieldIntegralCut	(1.0),  		// units: T.m
	m_indetMinEnergy	(2.5*CLHEP::GeV),	// Minimum energy of InDet tracks to be considered
	m_indetMinPt	        (0.*CLHEP::GeV),	// Minimum pt of InDet tracks to be considered
	m_indetPullCut		(6.),			// Cut on indet-combined momentum pull
	m_looseMatchCone	(0.13),			// min radius of eta/phi cone used in loose match
	m_looseMatchScaling	(0.50),			// additional loose match cone scaling (for pT = 1 GeV)
	m_looseMatchSigma	(6.),			// number of sigma to use in loose match checks
	m_matchChiSquaredCut	(30.),			// Chi2 cut for classification as good match 
	m_matchTrtTracks	(false),		// match TRT tracks (without silicon)
	m_maxWarnings		(10),			// MessageHelper configuration
	m_momentumBalanceCut	(6.),			// Cut on momentum balance of combined track
	m_spectrometerTolerance	(5.*CLHEP::mm),		// min tolerance for spectrometerPositionMatch
	m_scatteringAngle	(13.6*CLHEP::MeV),	// Coulomb scattering constant
	m_countAmbigID		(0),			// # of spectrometer with ID matching ambiguity
	m_countAmbigMS		(0),			// # of ID with spectrometer matching ambiguity
	m_countBadLoose1	(0),			// # combined tracks via SA with bad matchChi2 from looseMatch
	m_countBadLoose2	(0),			// # combined tracks via SA with bad matchChi2 from looseMatch
	m_countBestMatch1	(0),			// # combined tracks recorded with best match
	m_countBestMatch2	(0),			// # combined tracks recorded with best match
	m_countGoodLoose1	(0),			// # combined tracks via SA with good matchChi2 from looseMatch
	m_countGoodLoose2	(0),			// # combined tracks via SA with good matchChi2 from looseMatch
	m_countGoodMatch1	(0),			// # combined tracks with good 5-param SA/Indet match
	m_countGoodMatch2	(0),			// # combined tracks with good 5-param SA/Indet match
	m_countInnerMatch1	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countInnerMatch2	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countMSMatch1		(0),			// # combined tracks via SA failure with loose Indet match
	m_countMSMatch2		(0),			// # combined tracks via SA failure with loose Indet match
	m_countOuterMatch1	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countOuterMatch2	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countNO		(0),			// # spectrometer track with no combined track
	m_countSA		(0),			// # spectrometer tracks
	m_countSimpleMatch1	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countSimpleMatch2	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countTightMatch1	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countTightMatch2	(0),			// # combined tracks with simple (diagonal) SA/Indet match
	m_countUN		(0),			// # combined with unique SA/Indet match
	m_countBadBoth		(0),
	m_countBadFit		(0),
	m_countBadMatch		(0),
	m_countBadMatch1	(0),
	m_countBadMatch2	(0),
	m_countFailChargeConflict(0),			// # failing loose match with charge conflict
	m_countFailEnergyGain	(0),			// # failing loose energy gain limit
	m_countFailEnergyLoss	(0),			// # failing loose energy loss limit
	m_countFailMatchWindowMS(0),			// # fail MuidSa and outside match window
	m_countFailMatchWindowSA(0),			// # fail with MuidSA but no match inside window
	m_countFailMomentumBalance(0),			// # failing loose momentum balance (not other reason)
	m_countFailPositionMatch(0),			// # failing loose position match
	m_countFailLooseFit	(0),			// # fail track fit during loose match loop
	m_countFailInnerFit	(0),			// # fail track fit during inner match loop
	m_countFailOuterFit	(0),			// # fail track fit during outer match loop
	m_countFailTightFit	(0),			// # fail track fit during tight match loop
	m_countFailTrackFit	(0),			// # fail final track fit (for otherwise acceptable best match)
	m_countOutwardsTracks	(0),    		// # recovered by outwards track builder
	m_countCSignificance1	(0),
	m_countCSignificance2	(0),
	m_countNSignificance1	(0),
	m_countNSignificance2	(0),
	m_countPSignificance1	(0),
	m_countPSignificance2	(0),
	m_messageHelper		(0)
{
    m_messageHelper	= new MessageHelper(*this);
    declareInterface<IMuidMatchMaker>(this);
    declareProperty("AmbiguityProcessor",	m_ambiguityProcessor);
    declareProperty("CaloEnergyParam",		m_caloEnergyParam);
    declareProperty("CaloTSOS",			m_caloTSOS);
    declareProperty("Intersector",		m_intersector);
    declareProperty("MagFieldSvc",              m_magFieldSvc );
    declareProperty("MatchQuality",		m_matchQuality);
    declareProperty("MomentumBalanceTool",	m_momentumBalanceTool);
    declareProperty("MuonRecovery",		m_muonRecovery);
    declareProperty("OutwardsTrackBuilder",	m_outwardsBuilder);
    declareProperty("Propagator",		m_propagator);
    declareProperty("ScatteringAngleTool",	m_scatteringAngleTool);
    declareProperty("TrackBuilder",		m_trackBuilder);
    declareProperty("TrackQuery",		m_trackQuery);
    declareProperty("DeltaEtaPreSelection",	m_deltaEtaPreSelection);
    declareProperty("DeltaPhiPreSelection",	m_deltaPhiPreSelection);
    declareProperty("FieldIntegralCut", 	m_fieldIntegralCut);
    declareProperty("IndetMinEnergy", 		m_indetMinEnergy);
    declareProperty("IndetMinPt", 		m_indetMinPt);
    declareProperty("IndetPullCut", 		m_indetPullCut);
    declareProperty("looseMatchCone",		m_looseMatchCone);
    declareProperty("looseMatchScaling",	m_looseMatchScaling);
    declareProperty("LooseMatchSigma",		m_looseMatchSigma);
    declareProperty("MatchChiSquaredCut",	m_matchChiSquaredCut);
    declareProperty("MatchTrtTracks",		m_matchTrtTracks);
    declareProperty("MaxNumberOfWarnings",	m_maxWarnings,
		    "Maximum number of permitted WARNING messages per message type.");
    declareProperty("MomentumBalanceCut",	m_momentumBalanceCut);
    declareProperty("SpectrometerTolerance",	m_spectrometerTolerance);
    declareProperty("BadFitChi2",		m_badFitChi2 = 2.5);
    declareProperty("BadMatchChi2",		m_badMatchChi2 = 20.0);
}

MuidMatchMaker::~MuidMatchMaker()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuidMatchMaker::initialize()
{
    ATH_MSG_INFO( "Initializing MuidMatchMaker - package version " << PACKAGE_VERSION );
  
    // fill WARNING messages
    m_messageHelper->setMaxNumberOfMessagesPrinted(m_maxWarnings);
    m_messageHelper->setNumberOfMessages(12);
    m_messageHelper->setMessage(
	0, "execute: ambiguity processor failed to select any tracks");
    m_messageHelper->setMessage(
	1, "execute: best match is not to the spectrometer track selected by ambiguity processor");
    m_messageHelper->setMessage(
	2, "bestMatchChooser: choose worse, but acceptable, matchChiSq as better fitChiSq. ");
    m_messageHelper->setMessage(
	3, "bestMatchChooser: worse second match chosen as good fit chi2");
    m_messageHelper->setMessage(
	4, "bestMatchChooser: choose worse, but acceptable, matchChiSq as better momentum balance. ");
    m_messageHelper->setMessage(
	5, "bestMatchChooser: worse second match chosen on momentum balance");
    m_messageHelper->setMessage(
	6, "bestMatchChooser: choose worse, but acceptable, matchChiSq according to overall quality. ");
    m_messageHelper->setMessage(
	7, "bestMatchChooser: worse second match chosen on overall quality");
    m_messageHelper->setMessage(
	8, "combinedTrackQualityCheck: fail with unphysical momentum covariance");
    m_messageHelper->setMessage(
	9, "combinedTrackQualityCheck: fail with momentum pull above cut: ");
    m_messageHelper->setMessage(
	10, "evaluateMatchProperties: fail re-evaluation of match chi2");
    m_messageHelper->setMessage(
	11, "extrapolatedNeedsRefit: no refit for combined track without CaloEnergy");
    
    // get the Tools
    if (m_ambiguityProcessor.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_ambiguityProcessor );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_ambiguityProcessor );
    }	
    if (m_caloEnergyParam.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_caloEnergyParam );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_caloEnergyParam );
    }
    if (m_caloTSOS.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_caloTSOS );
        return StatusCode::FAILURE;
    }
    else
    {
        ATH_MSG_INFO( "Retrieved tool " << m_caloTSOS );
    }	
    if (m_intersector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_intersector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_intersector );
    }
   if (m_magFieldSvc.retrieve().isFailure())
    {
        ATH_MSG_FATAL("Could not get " << m_magFieldSvc); 
        return StatusCode::FAILURE;
    }
    if (m_matchQuality.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_matchQuality );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_matchQuality );
    }
    if (m_momentumBalanceTool.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_momentumBalanceTool );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_momentumBalanceTool );
    }
    if (! m_muonRecovery.empty())
    {
	if (m_muonRecovery.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_muonRecovery );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_muonRecovery );
	}
    }
    if (! m_outwardsBuilder.empty())
    {
	if (m_outwardsBuilder.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_outwardsBuilder );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_outwardsBuilder );
	}
    }
    else
    {
	ATH_MSG_INFO( "empty tool " << m_outwardsBuilder );
    }	
    if (m_propagator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_propagator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_propagator );
    }
    if (m_scatteringAngleTool.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_scatteringAngleTool );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_scatteringAngleTool );
    }
    if (! m_trackBuilder.empty())
    {
	if (m_trackBuilder.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackBuilder );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_INFO( "Retrieved tool " << m_trackBuilder );
	}
    }
    if (m_trackQuery.retrieve().isFailure())
    {
        ATH_MSG_FATAL( "Failed to retrieve tool " << m_trackQuery );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_trackQuery );
    }
 
    // create the calo barrel surfaces (cylinder) and 2 endcap discs)
    double radius	= 2.0*CLHEP::meter;
    double halfLength	= 4.0*CLHEP::meter;
    m_caloCylinder	= new Trk::CylinderSurface(radius, halfLength);

    // the corresponding max barrel cotTheta
    m_barrelCotTheta	= halfLength/radius;

    // and the forward/backward endcap disks
    m_caloForwardDisc	= 
      new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0., halfLength))), 0., radius);
    m_caloBackwardDisc	= 
      new Trk::DiscSurface(new Amg::Transform3D(Amg::Translation3D(Amg::Vector3D(0.,0.,-halfLength))), 0., radius);
  
    return StatusCode::SUCCESS;
}

StatusCode
MuidMatchMaker::finalize()
{
    double badBoth	= 0.;
    double badFit	= 0.;
    double badMatch	= 0.;
    if (m_countBestMatch1)
    {
	double norm	= 1./static_cast<double>(m_countBestMatch1);
	badBoth		= norm*static_cast<double>(100*m_countBadBoth);
	badFit		= norm*static_cast<double>(100*m_countBadFit);
	badMatch	= norm*static_cast<double>(100*m_countBadMatch);
    }

    ATH_MSG_INFO( "finalized with " << m_countBestMatch1 << " out of " << m_countSA
		  << " MuonSpectrometer tracks having a 'bestMatch' combined fit. "
		  << endmsg
		  << "BestMatches:" << std::setiosflags(std::ios::fixed)
		  << std::setw(6) << std::setprecision(1) << badFit << "% have a bad fit,"
		  << std::setw(5) << std::setprecision(1) << badMatch << "% have a bad match and"
		  << std::setw(5) << std::setprecision(1) << badBoth << "% have both bad."
		  << endmsg
		  << m_countUN
		  << " MuonSpectrometer tracks have a unique combined fit."
		  << endmsg << "               "
		  << m_countAmbigID << " MuonSpectrometer tracks with InDet association ambiguity,"
		  << endmsg << "               "
		  << m_countAmbigMS << " InDet tracks with spectrometer association ambiguity,"
		  << endmsg << "               "
		  << m_countFailTightFit << " tight associations fail track fitting,"
		  << endmsg << "               "
		  << m_countFailInnerFit << " inner associations fail track fitting,"
		  << endmsg << "               "
		  << m_countFailOuterFit << " outer associations fail track fitting,"
		  << endmsg << "               "
		  << m_countFailLooseFit << " loose associations fail track fitting,"
		  << endmsg << "         while "
		  << m_countNO << " MuonSpectrometer tracks with eta < 2.5 have no InDet association:"
		  << endmsg
		  << "               " << m_countFailTrackFit
		  << " fail final track fit for best match "
		  << endmsg
		  << "               " << m_countFailChargeConflict
		  << " fail loose match with charge conflict (and have MuidSA)"
		  << endmsg
		  << "               " << m_countFailEnergyGain
		  << " fail loose match through excessive energy gain (and have MuidSA)"
		  << endmsg
		  << "               " << m_countFailEnergyLoss
		  << " fail loose match through excessive energy loss (and have MuidSA)"
		  << endmsg
		  << "               " << m_countFailMomentumBalance
		  << " fail loose momentum balance with invalid track (and have MuidSA)"
		  << endmsg
		  << "               " << m_countFailPositionMatch
		  << " fail loose position match at calorimeter midpoint (and have MuidSA)"
		  << endmsg
		  << "               " << m_countFailMatchWindowSA
		  << " fail have MuidSA but no match inside window"
		  << endmsg
		  << "               " << m_countFailMatchWindowMS
		  << " fail MuidSA and have no match inside window"
		  << endmsg
		  << "  for " << std::setw(6) << m_countBestMatch1 << " (" << m_countBestMatch2 << ")"
		  << " best matches with a successful combined fit: "
		  << endmsg
		  << "      " << std::setw(6) << m_countTightMatch1 << " (" << m_countTightMatch2 << ")"
		  << " matches with a tight Indet/Spectrometer match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countInnerMatch1 << " (" << m_countInnerMatch2 << ")"
		  << " matches with an inner Indet/Spectrometer match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countOuterMatch1 << " (" << m_countOuterMatch2 << ")"
		  << " matches with an outer Indet/Spectrometer match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countSimpleMatch1 << " (" << m_countSimpleMatch2
		  << ")"
		  << " matches with a simple SA/Indet match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countGoodLoose1 << " (" << m_countGoodLoose2 << ")"
		  << " good matches with a loose Indet/Spectrometer match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countBadLoose1 << " (" << m_countBadLoose2 << ")"
		  << " bad matches with a loose Indet/Spectrometer match (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countMSMatch1 << " (" << m_countMSMatch2 << ")"
		  << " matches recovered from MuonSpectrometer tracks without Standalone fit"
		  << endmsg
		  << "      " << std::setw(6) << m_countBadMatch1 << " (" << m_countBadMatch2 << ")"
		  << " bad inner Indet/Spectrometer matches (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countPSignificance1
		  << " (" << m_countPSignificance2 << ")"
		  << " have momentumBalanceSignificance > 3 (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countCSignificance1
		  << " (" << m_countCSignificance2 << ")"
		  << " pass above with scatteringCurvatureSignificance > 3 (# from outwardsBuilder)"
		  << endmsg
		  << "      " << std::setw(6) << m_countNSignificance1
		  << " (" << m_countNSignificance2 << ")"
		  << " pass above with scatteringNeighbourSignificance > 3 (# from outwardsBuilder)"
	);

    // summarize WARNINGs
    m_messageHelper->printSummary();
    
    delete m_caloBackwardDisc;
    delete m_caloCylinder;
    delete m_caloForwardDisc;
    delete m_messageHelper;
    
    return StatusCode::SUCCESS;
}

PerigeeParameters*
MuidMatchMaker::convertPerigee (const Trk::Perigee* trkPerigee) const
{
    double d0        = trkPerigee->parameters()[Trk::d0];
    double z0        = trkPerigee->parameters()[Trk::z0];
    double phi       = trkPerigee->parameters()[Trk::phi];
    double theta     = trkPerigee->parameters()[Trk::theta];
    double ctgtheta  = 1./tan(trkPerigee->parameters()[Trk::theta]);
    double qOverP    = trkPerigee->parameters()[Trk::qOverP];
    double choverpt  = trkPerigee->charge()/trkPerigee->pT();
    PerigeeParameters* perigee = new PerigeeParameters();
    perigee->fillPerigee(cos(phi),ctgtheta,choverpt, sin(phi),d0,z0);

    // convert covariance - if it exists
    if (!trkPerigee->covariance()) return perigee;
    
    Trk::JacobianThetaPToCotThetaPt jacob(theta,qOverP);
    // newcov = (jacob) * oldcov * (jacob)T
    AmgSymMatrix(5)* newcov =
      new AmgSymMatrix(5)(trkPerigee->covariance()->similarity(jacob)); 
    perigee->covariance(newcov);
    return perigee;
}

MuidTrackContainer*
MuidMatchMaker::execute(const TrackCollection*	extrapolatedTracks,
			const TrackCollection*	indetTracks,
			const TrackCollection*	spectrometerTracks) const
{
    if (msgLvl(MSG::DEBUG)) debugInput(indetTracks,extrapolatedTracks,spectrometerTracks);

    // create container for result
    MuidTrackContainer* muonMatches = new MuidTrackContainer;
    std::vector<MuidTrack*> ambigMatches;
    std::vector<MuidTrack*> bestMatches;
    std::vector<IndetMatchCandidate*> indetMatchCandidates;
    indetMatchCandidates.reserve(10);

    // make collection of extrapolated tracks selected by muon spectrometer ambiguity processor
    TrackCollection* selectedTracks =  m_ambiguityProcessor->process(extrapolatedTracks);
    if (! selectedTracks)
    {
	// ambiguity processor failed to select any tracks
	m_messageHelper->printWarning(0);
	selectedTracks	= new TrackCollection;
    }
    else if (selectedTracks->size() < extrapolatedTracks->size())
    {
	ATH_MSG_VERBOSE( selectedTracks->size() << " tracks selected from "
			 << extrapolatedTracks->size() << " extrapolated tracks" );
    }
    
    // loop over extrapolated (standalone) muons (including extrapolation failures)
    int extrapNumber = 1;
    const Trk::Track* extrapolatedTrack		= 0;
    const Trk::Track* spectrometerTrack		= 0;
    const Trk::Track* inputExtrapolatedTrack	= 0;
    const Trk::Track* inputSpectrometerTrack	= 0;
    TrackCollection::const_iterator spect = spectrometerTracks->begin();
    for (TrackCollection::const_iterator extrap = extrapolatedTracks->begin();
	 extrap != extrapolatedTracks->end();
	 ++extrap, ++extrapNumber, ++spect)
    {
	++m_countSA;
	
	// ignore badly constructed spectrometer tracks -
	// otherwise there's a downstream problem because the corresponding particle doesn't exist
	if (! (**extrap).perigeeParameters()->covariance())
	{
	   ATH_MSG_DEBUG( " skip spectrometer track without measured Perigee" );
	   continue;
	}
	
	// is extrapolated selected by ambiguity processor?
	bool isAmbiguous	= true;
	for (TrackCollection::const_iterator sel = selectedTracks->begin();
	     sel != selectedTracks->end();
	     ++sel)
	{
	    if (*sel != *extrap) continue;
	    isAmbiguous	= false;
	    break;
	}
	
	// re-express muon tracks as outgoing if necessary (for cosmics)
	if (extrapolatedTrack	!= inputExtrapolatedTrack)	delete extrapolatedTrack;
	inputExtrapolatedTrack	= *extrap;
	extrapolatedTrack	= m_trackQuery->outgoingTrack(**extrap);
	if (spectrometerTrack	!= inputSpectrometerTrack)	delete spectrometerTrack;
	inputSpectrometerTrack	= *spect;
	spectrometerTrack	= m_trackQuery->outgoingTrack(**spect);
	if (! spectrometerTrack)	continue;
	
	// extrapolation failure or otherwise unreliable
	bool isExtrapolated	= false;
	if (extrapolatedTrack)
	    isExtrapolated = m_trackQuery->isCaloAssociated(*extrapolatedTrack);

	// indet acceptance cut-off with 0.05 allowance (from vertex spread)
	double maxEta		= 2.45;
	double muonEta		= 0.;
	// double muonPhi		= 0.;

	// get fieldIntegral between spectrometer measurements (units T.m)
	double spectrometerBdl	= 0.;
	
	if (isExtrapolated)
	{
	    muonEta		= extrapolatedTrack->perigeeParameters()->momentum().eta();
	    // muonPhi		= extrapolatedTrack->perigeeParameters()->momentum().phi();
	    spectrometerBdl	=
		m_trackQuery->fieldIntegral(*extrapolatedTrack).betweenSpectrometerMeasurements();
	}
	else
	{
	    ATH_MSG_VERBOSE( " no extrapolated track " );
	    isExtrapolated	= false;
	    if (extrapolatedTrack	!= inputExtrapolatedTrack)	delete extrapolatedTrack;
	    extrapolatedTrack	= 0;
	    muonEta		= spectrometerTrack->perigeeParameters()->position().eta();
	    // muonPhi		= spectrometerTrack->perigeeParameters()->position().phi();
	    spectrometerBdl	=
		m_trackQuery->fieldIntegral(*spectrometerTrack).betweenSpectrometerMeasurements();
	}
	
	// in case of low field integral may skip inner or outer matching
	bool doInnerMatch	= true;
	bool doOuterMatch	= true;
	if (! m_magFieldSvc->solenoidOn())
	{
	    // require momentum determination from indet for outer match
	    doOuterMatch	= false;
	}
	else if (! extrapolatedTrack || spectrometerBdl < m_fieldIntegralCut)
	{
	    // else require precise MS momentum measurement for inner match
	    doInnerMatch	= false;
	    if (extrapolatedTrack)
		ATH_MSG_VERBOSE( " low field integral between spectrometer measurements: "
				 << spectrometerBdl << " T.m" );
	}
					  
	// select indet candidates in loose matching window
	// for cosmics: note candidates store the outgoing extrapolated but input spectrometer tracks
        indetCandidates(indetMatchCandidates,
			extrapolatedTrack,
			indetTracks,
			inputSpectrometerTrack);

	// use indet tracks to help define limit of combined acceptance
	for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
	     cand != indetMatchCandidates.end();
	     ++cand)
	{
	    double indetEta	= (**cand).perigee()->momentum().eta();
	    if (fabs(indetEta) > maxEta) maxEta = fabs(indetEta);
	}
	
	IndetMatchCandidate*	bestCandidate	= 0;
	double			bestMatchChi2	= 99999999.;
	IndetMatchCandidate*	bestMuonMatch  	= 0;
	int			numMatches	= 0;

	// successive loops over indet candidates to make tight,inner,outer,simple and loose matches
	if (doInnerMatch && doOuterMatch)
	{	    
	    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
		 cand != indetMatchCandidates.end();
		 ++cand)
	    {
		ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed | std::ios::right)
				 << "----------- MuidMatchMaker ----------" << endmsg
				 << std::setw(4) << extrapNumber << " /"
				 << std::setw(4) << (**cand).indetIndex() );

		// cuts on inner and out match chi2
		if ((**cand).innerMatchChi2()	> m_matchChiSquaredCut)
		{
		    ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				     << "  fails tight match to inDet "
				     << std::setw(4) << (**cand).indetIndex()
				     << "  with inner chi2 "
				     << std::setw(6) << std::setprecision(1)
				     << (**cand).innerMatchChi2() );
		    continue;
		}
		if ((**cand).outerMatchChi2()	> m_matchChiSquaredCut)
		{
		    ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				     << "  fails tight match to inDet "
				     << std::setw(4) << (**cand).indetIndex()
				     << "  with outer chi2 "
				     << std::setw(6) << std::setprecision(1)
				     << (**cand).outerMatchChi2() );
		    continue;
		}

		// we have a match
		(**cand).matchType(MuonMatch::Tight);
	    
		ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				 << "  Matches inDet " << std::setw(4) << (**cand).indetIndex()
				 << "  with inner(outer) chi2 "
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).innerMatchChi2() << " ("
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).outerMatchChi2() << ")"
				 << ", prob(chi2) "
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).innerMatchProb() << " ("
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).outerMatchProb() << ")" );

		// build (and fit) the combined track
		const Trk::Track* combinedTrack	= buildCombinedTrack((**cand).track(),
								     *extrapolatedTrack,
								     *spectrometerTrack);

		// keep statistics on fit failure
		if (! combinedTrack)
		{
		    ++m_countFailTightFit;
		    continue;
		}
	    
		// update candidate with successful fit
		(**cand).combinedMatch(MuonMatch::Tight, combinedTrack);
		evaluateMatchProperties(**cand);
		if (! bestMuonMatch || bestMatchChooser(**cand,*bestMuonMatch))
		{
		    bestMuonMatch	= *cand;
		    bestMatchChi2	= (**cand).innerMatchChi2();
		}
	    
		// count: muon with indet match ambiguity (if found)
		if (++numMatches == 2) ++m_countAmbigID;
	    }	// end tight matching loop

	    // tight match: fit recovery step?
	    // FIXME: defer recovery until matching loops finished
	}
	
	// try inner chi2 matching
	if (doInnerMatch && ! numMatches)
	{
	    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
		 cand != indetMatchCandidates.end();
		 ++cand)
	    {
		if (! (**cand).isType(MuonMatch::Unmatched))		continue;

		// flag best inner match chi2
		if ((**cand).innerMatchChi2() < bestMatchChi2)
		{
		    bestCandidate	= *cand;
		    bestMatchChi2	= (**cand).innerMatchChi2();
		}

		// cut on inner match chi2
		if ((**cand).innerMatchChi2()	> m_matchChiSquaredCut)	continue;
		(**cand).matchType(MuonMatch::Inner);
		
		ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				 << "  Matches inDet " << std::setw(4) << (**cand).indetIndex()
				 << "  with inner(outer) chi2 "
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).innerMatchChi2() << " ("
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).outerMatchChi2() << ")"
				 << ", prob(chi2) "
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).innerMatchProb() << " ("
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).outerMatchProb() << ")" );

		// build (and fit) the combined track
		const Trk::Track* combinedTrack	= buildCombinedTrack((**cand).track(),
								     *extrapolatedTrack,
								     *spectrometerTrack);

		// keep statistics on fit failure
		if (! combinedTrack)
		{
		    ATH_MSG_DEBUG( "fails inner fit " );
		    ++m_countFailInnerFit;
		    continue;
		}
		
		// update candidate with successful fit
		(**cand).combinedMatch(MuonMatch::Inner, combinedTrack);
		evaluateMatchProperties(**cand);
		if (! bestMuonMatch || bestMatchChooser(**cand,*bestMuonMatch))
		{
		    bestMuonMatch	= *cand;
		    bestMatchChi2	= (**cand).innerMatchChi2();
		}
    
		// count: muon with indet match ambiguity (if found)
		if (++numMatches == 2) ++m_countAmbigID;
	    }	// end inner matching loop 
	}
	
	// try outer chi2 matching
	if (doOuterMatch && ! numMatches)
	{
	    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
		 cand != indetMatchCandidates.end();
		 ++cand)
	    {
		if (! (**cand).isType(MuonMatch::Unmatched))		continue;
	
		// evaluate outer match chi2 (if not already done)
		if (! (**cand).outerMatchDoF())
		    (**cand).outerMatch(
			m_matchQuality->outerMatchChi2(*(**cand).indetTrack(),
						       *inputSpectrometerTrack),
			m_matchQuality->outerMatchDOF(*(**cand).indetTrack(),
						      *inputSpectrometerTrack),
			m_matchQuality->outerMatchProbability(*(**cand).indetTrack(),
							      *inputSpectrometerTrack));

		// flag best match chi2
		if ((**cand).outerMatchChi2() < bestMatchChi2)
		{
		    bestCandidate	= *cand;
		    bestMatchChi2	= (**cand).outerMatchChi2();
		}

		// cut on outer match chi2
		if ((**cand).outerMatchChi2()	> m_matchChiSquaredCut)	continue;
		(**cand).matchType(MuonMatch::Outer);

		ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				 << "  Matches inDet " << std::setw(4) << (**cand).indetIndex()
				 << "  with inner(outer) chi2 "
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).innerMatchChi2() << " ("
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).outerMatchChi2() << ")"
				 << ", prob(chi2) "
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).innerMatchProb() << " ("
				 << std::setw(6) << std::setprecision(3)
				 << (**cand).outerMatchProb() << ")" );

		// build (and fit) a combined track - with material reallocation from indet parameters
		const Trk::Track* combinedTrack = indetExtension((**cand).track(), *spectrometerTrack);

		// keep statistics on fit failure etc
		if (combinedTrack)
		{
		    if (! isExtrapolated)
		    {
			++m_countMSMatch1;
			if (combinedTrack->info().trackFitter() == Trk::TrackInfo::GlobalChi2Fitter)
			    ++m_countMSMatch2;
		    }
		}
		else
		{
		    ++m_countFailOuterFit;
		    continue;
		}
		
		// update candidate with successful fit
		(**cand).combinedMatch(MuonMatch::Outer,combinedTrack);
		evaluateMatchProperties(**cand);
		if (! bestMuonMatch || bestMatchChooser(**cand,*bestMuonMatch))
		{
		    bestMuonMatch	= *cand;
		    bestMatchChi2	= (**cand).outerMatchChi2();
		}
    
		// count: muon with indet match ambiguity (if found)
		if (++numMatches == 2) ++m_countAmbigID;
	    }	// end outer matching loop 
	}

	// try simple chi2 matching (only diagonal terms)
	if (doInnerMatch && ! numMatches)
	{
	    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
		 cand != indetMatchCandidates.end();
		 ++cand)
	    {
		if (! (**cand).isType(MuonMatch::Unmatched))		continue;

		// cut on simple match chi2
		if ((**cand).simpleMatchChi2()	> m_matchChiSquaredCut)	continue;
		(**cand).matchType(MuonMatch::Simple);
		
		ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
				 << "  Matches inDet " << std::setw(4) << (**cand).indetIndex()
				 << "  with simple(inner, outer) chi2 "
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).simpleMatchChi2() << " ("
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).innerMatchChi2() << ", "
				 << std::setw(6) << std::setprecision(1)
				 << (**cand).outerMatchChi2() << ")" );

		// build (and fit) the combined track
		const Trk::Track* combinedTrack	= buildCombinedTrack((**cand).track(),
								     *extrapolatedTrack,
								     *spectrometerTrack);

		// keep statistics on fit failure
		if (! combinedTrack)
		{
		    ++m_countFailLooseFit;
		    continue;
		}
		
		// update candidate with successful fit
		(**cand).combinedMatch(MuonMatch::Simple, combinedTrack);
		evaluateMatchProperties(**cand);
		if (! bestMuonMatch || bestMatchChooser(**cand,*bestMuonMatch))
		{
		    bestMuonMatch	= *cand;
		    bestMatchChi2	= (**cand).innerMatchChi2();
		}
    
		// count: muon with indet match ambiguity (if found)
		if (++numMatches == 2) ++m_countAmbigID;
	    }	// end simple matching loop 
	}

	// try recovery procedure for indetExtension fit failure or unfitted best match
	if (! numMatches && fabs(muonEta) < maxEta + 0.05)
	{
	    ATH_MSG_VERBOSE( "  try recovery before loose match " );
	    
	    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
		 cand != indetMatchCandidates.end();
		 ++cand)
	    {
		if (! (**cand).isType(MuonMatch::Unmatched)
		    && ! (**cand).isType(MuonMatch::Outer))		continue;

		const Trk::Track* combinedTrack	= 0;
		if (! m_muonRecovery.empty())
		{
		    combinedTrack = m_muonRecovery->recoverableMatch((**cand).track(),*spectrometerTrack);
		    if (combinedTrack)
		    {
			// update candidate with successful fit
			if ((**cand).isType(MuonMatch::Unmatched))
			{
			    (**cand).combinedMatch(MuonMatch::Loose, combinedTrack);
			}
			else
			{
			    (**cand).combinedMatch(MuonMatch::Outer, combinedTrack);
			    --m_countFailOuterFit;
			}
			evaluateMatchProperties(**cand);
			if (! bestMuonMatch || bestMatchChooser(**cand,*bestMuonMatch))
			{
			    bestMuonMatch	= *cand;
			    bestMatchChi2	= (**cand).outerMatchChi2();
			}
			++numMatches;
		    }
		}
		// if (! numMatches) ++m_countFailTrackFit;
	    }
	}

	// as a final resort: try the best match even when below cutoff
	// if there is a loose match and the muon is in the indet acceptance
	if (! numMatches && fabs(muonEta) < maxEta + 0.05)
	{
	    PerigeeParameters* perigeeParameters	= 0;
	    if (bestCandidate)
	    {
		if (bestMatchChi2 >= m_matchChiSquaredCut)
		{
		    ATH_MSG_VERBOSE( " no good chi2 matching combination - best match taken: "  
				     << bestMatchChi2 << " to indet " << bestCandidate->indetIndex());
		    
		    if (bestCandidate->perigee())
			perigeeParameters	= convertPerigee(bestCandidate->perigee());
		}
	    }

	    m_failChargeConflict	= false;
	    m_failEnergyGain		= false;
	    m_failEnergyLoss		= false;
	    m_failMomentumBalance	= false;
	    m_failPositionMatch		= false;
	    
	    if (perigeeParameters
		&& momentumBalance(bestCandidate->indetTrack(),
				   bestCandidate->perigee(),
				   spectrometerTrack->perigeeParameters(),
				   spectrometerTrack)
		&& loosePositionMatch(*perigeeParameters, *bestCandidate))
	    {
		ATH_MSG_DEBUG( "     accept as loose position match " );
		const Trk::Track* track		= 0;
		if (bestCandidate)
		{
		    // build (and fit) a combined track (material reallocation from indet parameters)
		    track	= indetExtension(bestCandidate->track(), *spectrometerTrack);
		    if (track && ! isExtrapolated)
		    {
			++m_countMSMatch1;
			if (track->info().trackFitter() == Trk::TrackInfo::GlobalChi2Fitter)
			    ++m_countMSMatch2;
		    }
		}

		// update candidate with successful fit
		if (track)
		{
		    bestCandidate->combinedMatch(MuonMatch::Loose, track);
		    evaluateMatchProperties(*bestCandidate);
		    bestMuonMatch	= bestCandidate;
		    ++numMatches;
		}
		else
		{
		    ++m_countFailLooseFit;
		}
	    }

	    if (! numMatches && fabs(muonEta) < 2.5)
	    {
		++m_countNO;
		if (bestCandidate)
		{
		    if (m_failChargeConflict)
		    {
			++m_countFailChargeConflict;
		    }
		    else if (m_failEnergyGain)
		    {
			++m_countFailEnergyGain;
		    }
		    else if (m_failEnergyLoss)
		    {
			++m_countFailEnergyLoss;
		    }
		    else if (m_failMomentumBalance)
		    {
			++m_countFailMomentumBalance;
		    }
		    else if (m_failPositionMatch)
		    {
			++m_countFailPositionMatch;
		    }
		    else 
		    {
			++m_countFailTrackFit;
		    }
		}
		else 
		{
		    if (isExtrapolated)
		    {
			++m_countFailMatchWindowSA;
		    }
		    else
		    {
			++m_countFailMatchWindowMS;
		    }
		}
		ATH_MSG_DEBUG( " muon# " << extrapNumber << "  has no inDet match " );
	    }
	    delete perigeeParameters;
	}
  
	if (numMatches == 1) ++m_countUN;

	// transfer combined muons to output collection.
	// For each MS track keep muonMatch with bestMatch
	for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
	     cand != indetMatchCandidates.end();
	     ++cand)
	{
	    if (! (**cand).combinedTrack())	continue;

	    // copy successful match into collection.
	    // cosmics: set extrapolated to input extrapolated track
	    MuidTrack* combinedMatch	= new MuidTrack(**cand);
	    combinedMatch->extrapolatedTrack(inputExtrapolatedTrack);
	    muonMatches->push_back(combinedMatch);
	    if (*cand == bestMuonMatch)
	    {
		if (isAmbiguous)
		{
		    ambigMatches.push_back(combinedMatch);
		    // best match not selected by ambiguity processor
		    m_messageHelper->printWarning(1);
		}
		bestMatches.push_back(combinedMatch);
	    }
	}
    }	// end loop over MS tracks

    // delete any remaining flipped tracks
    if (extrapolatedTrack	!= inputExtrapolatedTrack)	delete extrapolatedTrack;
    if (spectrometerTrack	!= inputSpectrometerTrack)	delete spectrometerTrack;

    // delete candidates
    for (std::vector<IndetMatchCandidate*>::iterator cand = indetMatchCandidates.begin();
	 cand != indetMatchCandidates.end();
	 ++cand)
	delete *cand;

    // flag bestMatches from best match per MS track
    for (std::vector<MuidTrack*>::const_iterator best = bestMatches.begin();
	 best != bestMatches.end();
	 ++best)
    {
	bool bestIndet		= true;
	for (std::vector<MuidTrack*>::const_iterator ambig = bestMatches.begin();
	     ambig != bestMatches.end();
	     ++ambig)
	{
	    if (*best == *ambig
		|| (**best).indetTrack() != (**ambig).indetTrack()) continue;

	    ATH_MSG_VERBOSE(" spectrometer ambiguity");
	    bestIndet = bestMatchChooser(**best,**ambig);
	    if (! bestIndet)
	    {
		++m_countAmbigMS;
		break;
	    }
	}

	// set whether this is the overall best match
	(**best).bestMatch(bestIndet);
    }

    // accumulate some statistics:
    //    (# bestMatches with combined fit, # of these with matchChi2 < cut, etc)
    accumulateStatistics (*muonMatches);

    if (msgLvl(MSG::DEBUG)) debugOutput(muonMatches,indetTracks,spectrometerTracks);

    delete selectedTracks;
    return muonMatches;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
MuidMatchMaker::accumulateStatistics (MuidTrackContainer& muonMatches) const
{
    // only consider best matches (i.e. after ambiguity resolution)
    for (const MuidTrack* match : muonMatches)
    {
	if (! match->bestMatch())	continue;
	bool outwardsBuild	= false;
	if (match->combinedTrack()->info().trackFitter() == Trk::TrackInfo::GlobalChi2Fitter)
	    outwardsBuild	= true;
	++m_countBestMatch1;
	if (outwardsBuild) ++m_countBestMatch2;
	if (match->isType(MuonMatch::Tight))
	{
	    ++m_countTightMatch1;
	    if (outwardsBuild) ++m_countTightMatch2;
	}
	else if (match->isType(MuonMatch::Inner))
	{
	    ++m_countInnerMatch1;
	    if (outwardsBuild) ++m_countInnerMatch2;
	}
	else if (match->isType(MuonMatch::Outer))
	{
	    ++m_countOuterMatch1;
	    if (outwardsBuild) ++m_countOuterMatch2;
	}
	else if (match->isType(MuonMatch::Simple))
	{
	    ++m_countSimpleMatch1;
	    if (outwardsBuild) ++m_countSimpleMatch2;
	}
	if (match->extrapolatedTrack())
	{
	    if (match->isType(MuonMatch::Loose))
	    {
                if (match->matchChi2() < m_matchChiSquaredCut)
		{
		    ++m_countGoodLoose1;
		    if (outwardsBuild) ++m_countGoodLoose2;
		}
		else
		{
		    ++m_countBadLoose1;
		    if (outwardsBuild) ++m_countBadLoose2;
		}   
	    }
	    else
	    {
		++m_countGoodMatch1;
		if (outwardsBuild) ++m_countGoodMatch2;
	    }
	}
	else
	{
	    ++m_countMSMatch1;
	    if (outwardsBuild) ++m_countMSMatch2;
	}

	// monitor tracks with poor fit quality
	double bestFitChi2			= 9999.;
	const Trk::FitQuality* fitQuality	= match->combinedTrack()->fitQuality();
	if (fitQuality && fitQuality->numberDoF())
	    bestFitChi2	= fitQuality->chiSquared() / fitQuality->doubleNumberDoF();
	if (bestFitChi2 > m_badFitChi2)
	{		
	    ++m_countBadFit;
	    if (match->matchChi2() > m_badMatchChi2) ++m_countBadBoth;
		
	    ATH_MSG_DEBUG( " bad fit: chi2 " << bestFitChi2 );
	}

	// poor match chi2 (strong and realistic cuts)
	if (match->matchChi2() > m_badMatchChi2)
	{
	    ++m_countBadMatch;
	    ATH_MSG_DEBUG( " bad match: chi2 " << match->matchChi2() );
	    if (match->matchChi2() > m_matchChiSquaredCut)
	    {
		++m_countBadMatch1;
		if (outwardsBuild) ++m_countBadMatch2;
	    }
	}
	
	// other potential quality cuts
	if (fabs(match->momentumBalanceSignificance()) > 3.0)
	{
	    ++m_countPSignificance1;
	    if (outwardsBuild) ++m_countPSignificance2;
	    ATH_MSG_VERBOSE( " bad momentumBalanceSignificance : "
			     << match->momentumBalanceSignificance() );
	}
	else if (fabs(match->scatteringAngleSignificance().curvatureSignificance()) > 3.0)
	{
	    ++m_countCSignificance1;
	    if (outwardsBuild) ++m_countCSignificance2;
	    ATH_MSG_VERBOSE( " bad curvatureSignificance : "
			     << match->scatteringAngleSignificance().curvatureSignificance() );
	}
	else if (fabs(match->scatteringAngleSignificance().neighbourSignificance()) > 3.0)
	{
	    ++m_countNSignificance1;
	    if (outwardsBuild) ++m_countNSignificance2;
	    ATH_MSG_VERBOSE( " bad neighbourSignificance : "
			     << match->scatteringAngleSignificance().neighbourSignificance() );
	}
    }
}
    	    
bool
MuidMatchMaker::bestMatchChooser (const MuonMatch&	match1,
				  const MuonMatch&	match2) const
{
    // returned bool: true means match1 is better; false means match2 is better
    double matchChiSq1			= match1.innerMatchChi2();
    double matchChiSq2			= match2.innerMatchChi2();
    const Trk::TrackSummary* summary1	= match1.combinedTrack()->trackSummary();
    const Trk::TrackSummary* summary2	= match2.combinedTrack()->trackSummary();
    ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2);
    if (summary1 && summary2)
    {
	ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2
			<< "  numTRTHits " << summary1->get(Trk::numberOfTRTHits)
			<< "  " << summary2->get(Trk::numberOfTRTHits)
			<< "  field integrals: ID  "
			<< match1.fieldIntegral().betweenInDetMeasurements()
			<< "  " << match2.fieldIntegral().betweenInDetMeasurements()
			<< "  MS "
			<< match1.fieldIntegral().betweenSpectrometerMeasurements()
			<< "  " << match2.fieldIntegral().betweenSpectrometerMeasurements() );
    }
    else
    {
	ATH_MSG_VERBOSE("bestMatchChooser: matchChiSq " << matchChiSq1 << "  " << matchChiSq2);
    }
    
    // selection when only one match has a good combined fit
    double fitChiSq1	= match1.combinedTrack()->fitQuality()->chiSquared() /
			  match1.combinedTrack()->fitQuality()->doubleNumberDoF();
    int numberDoF1	= match1.combinedTrack()->fitQuality()->numberDoF();
    double fitChiSq2	= match2.combinedTrack()->fitQuality()->chiSquared() /
			  match2.combinedTrack()->fitQuality()->doubleNumberDoF();
    int numberDoF2	= match2.combinedTrack()->fitQuality()->numberDoF();
    ATH_MSG_VERBOSE("bestMatchChooser: fitChiSq " << fitChiSq1 << "  " << fitChiSq2);
    if (fabs(fitChiSq1 - fitChiSq2) > m_badFitChi2)
    {
	if (fitChiSq1 < m_badFitChi2)
	{
	    if (matchChiSq1 > matchChiSq2
		&& matchChiSq2 < m_matchChiSquaredCut
		&& m_messageHelper->wouldPrintWarning(2))
	    {
		// NOT choosing bestMatchChi2:
		std::stringstream ss;
		ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
		   << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
		   << fitChiSq2 << "/" << numberDoF2;
		m_messageHelper->printWarning(2, ss.str());
	    }
	    return true;
	}
	if (fitChiSq2 < m_badFitChi2)
	{
	    if (matchChiSq1 < matchChiSq2
		&& matchChiSq1 < m_matchChiSquaredCut
		&& m_messageHelper->wouldPrintWarning(2))
	    {
		// NOT choosing bestMatchChi2:
		std::stringstream ss;
		ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
		   << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
		   << fitChiSq2 << "/" << numberDoF2;
		m_messageHelper->printWarning(2, ss.str());
	    }
	    return false;
	}
    }
        
    // selection when only one match has a good match chi2
    if (fabs(matchChiSq1 - matchChiSq2) > m_matchChiSquaredCut)
    {
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
    if (match1.indetTrack()
	&& ! match1.indetTrack()->info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	double cutRatio		= 1.5;
	double integral1	= 0.;
	double integral2	= 0.;
	if (match1.extrapolatedTrack()
	    && ! match1.extrapolatedTrack()->info().trackProperties(Trk::TrackInfo::StraightTrack)
	    && match2.extrapolatedTrack()
	    && ! match2.extrapolatedTrack()->info().trackProperties(Trk::TrackInfo::StraightTrack))
	{
	    // selection when only one match has good momentum balance or a significantly better balance
	    ATH_MSG_VERBOSE( "bestMatchChooser: momentumBalanceSignificance "
			     << match1.momentumBalanceSignificance()
			     << "  " << match2.momentumBalanceSignificance());
	    double significanceCut	= 2.0;
	    double significance1	= fabs(match1.momentumBalanceSignificance());
	    double significance2	= fabs(match2.momentumBalanceSignificance());
	    if (fabs(significance1 - significance2) > significanceCut)
	    {
		if (significance1 < significanceCut)
		{
		    if (matchChiSq1	> matchChiSq2
			&& matchChiSq2	< m_matchChiSquaredCut
			&& m_messageHelper->wouldPrintWarning(4))
		    {
			// NOT choosing bestMatchChi2:
			std::stringstream ss;
			ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			   << "   momentumBalanceSignificance 1,2 "
			   << match1.momentumBalanceSignificance()
			   << ", " << match2.momentumBalanceSignificance();
			m_messageHelper->printWarning(4, ss.str());
		    }
		    return true;
		}
		if (significance2 < significanceCut)
		{
		    if (matchChiSq1	< matchChiSq2
			&& matchChiSq1	< m_matchChiSquaredCut
			&& m_messageHelper->wouldPrintWarning(4))
		    {
			// NOT choosing bestMatchChi2:
			std::stringstream ss;
			ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
			   << "   momentumBalanceSignificance 1,2 "
			   << match1.momentumBalanceSignificance()
			   << ", " << match2.momentumBalanceSignificance();
			m_messageHelper->printWarning(4, ss.str());
		    }
		    return false;
		}
	    }

	    // keep significantly larger measured field integral
	    //   for MS
	    ATH_MSG_VERBOSE( "bestMatchChooser: spectrometer field integral ratio ");
	    integral1	= fabs(match1.fieldIntegral().betweenSpectrometerMeasurements());
	    integral2	= fabs(match2.fieldIntegral().betweenSpectrometerMeasurements());
	    if (integral1 > cutRatio*integral2)	return true;
	    if (integral2 > cutRatio*integral1)	return false;
	}
	//   for indet
	ATH_MSG_VERBOSE( "bestMatchChooser: indet field integral ratio ");
	integral1	= fabs(match1.fieldIntegral().betweenInDetMeasurements());
	integral2	= fabs(match2.fieldIntegral().betweenInDetMeasurements());
	if (integral1 > cutRatio*integral2)	return true;
	if (integral2 > cutRatio*integral1)	return false;
    }

    // repeat fit/match quality selection with sharper cuts (times 2)
    ATH_MSG_VERBOSE( "bestMatchChooser: sharp fit chi2 cut " );
    if (fabs(fitChiSq1 - fitChiSq2) > 0.5*m_badFitChi2)
    { 
	if (fitChiSq1 < 0.5*m_badFitChi2)
	{
	    if (matchChiSq1	> matchChiSq2
		&& matchChiSq2	< m_matchChiSquaredCut
		&& m_messageHelper->wouldPrintWarning(6))
	    {
		// NOT choosing bestMatchChi2:
		std::stringstream ss;
		ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
		   << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
		   << fitChiSq2 << "/" << numberDoF2;
		m_messageHelper->printWarning(6, ss.str());
	    }
	    return true;
	}
	if (fitChiSq2 < 0.5*m_badFitChi2)
	{
	    if (matchChiSq1	< matchChiSq2
		&& matchChiSq1	< m_matchChiSquaredCut
		&& m_messageHelper->wouldPrintWarning(6))
	    {
		// NOT choosing bestMatchChi2:
		std::stringstream ss;
		ss << " matchChiSq 1,2 " << matchChiSq1 << ", " << matchChiSq2
		   << "   fitChiSq/DoF 1,2 " << fitChiSq1 << "/" << numberDoF1 << ", "
		   << fitChiSq2 << "/" << numberDoF2;
		m_messageHelper->printWarning(6, ss.str());
	    }
	    return false;
	}
    }

    ATH_MSG_VERBOSE( "bestMatchChooser: sharp match chi2 cut ");		     
    if (fabs(matchChiSq1 - matchChiSq2) > 0.5*m_matchChiSquaredCut)
    {
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
    if (match1.extrapolatedTrack()
	&& match1.extrapolatedTrack()->info().trackProperties(Trk::TrackInfo::StraightTrack)
	&& match2.extrapolatedTrack()
	&& match2.extrapolatedTrack()->info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	// best fit chi2
	if (fitChiSq1 < fitChiSq2) return true;
	return false;
    }
    else
    {	
	// best match chi2
	if (matchChiSq1 < matchChiSq2) return true;
	return false;
    }
}

const Trk::Track*
MuidMatchMaker::buildCombinedTrack (const Trk::Track&	indetTrack,
				    const Trk::Track&	extrapolatedTrack,
				    const Trk::Track&	spectrometerTrack) const
{
    // build and fit the combined track
    const Trk::Track* combinedTrack	= 0;
    double	      combinedFitChi2	= 9999.;
    if (! m_trackBuilder.empty())
    {
	combinedTrack = m_trackBuilder->combinedFit(indetTrack,
						    extrapolatedTrack,
						    spectrometerTrack);
	if (combinedTrack && combinedTrack->fitQuality())
	{
	    combinedTrack->info().addPatternReco(extrapolatedTrack.info());
	    combinedFitChi2		= combinedTrack->fitQuality()->chiSquared() /
					  combinedTrack->fitQuality()->doubleNumberDoF();
	}
    }
    if (combinedFitChi2 > m_badFitChi2 && ! m_outwardsBuilder.empty())
    {
	const Trk::Track* outwardsTrack	= m_outwardsBuilder->combinedFit(indetTrack,
									 extrapolatedTrack,
									 spectrometerTrack);
	if (outwardsTrack &&
	    outwardsTrack->fitQuality()->chiSquared() /
	    outwardsTrack->fitQuality()->doubleNumberDoF()	< combinedFitChi2)
	{
	    ATH_MSG_VERBOSE( "buildCombinedTrack: choose outwards track" );
	    outwardsTrack->info().addPatternReco(spectrometerTrack.info());
	    delete combinedTrack;
	    combinedTrack		= outwardsTrack;
	}
	else
	{
	    delete outwardsTrack;
	}
    }
    
    // filter out rubbish fits
    if (combinedTrack && combinedTrackQualityCheck(*combinedTrack,indetTrack,spectrometerTrack))
    {
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
	if (combinedTrack && combinedTrackQualityCheck(*combinedTrack,indetTrack,spectrometerTrack))
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

bool
MuidMatchMaker::combinedTrackQualityCheck (const Trk::Track&	combinedTrack,
					   const Trk::Track&	indetTrack,
					   const Trk::Track&	/*spectrometerTrack*/) const
{
    // require calo correctly associated to track
    if (! m_trackQuery->isCaloAssociated(combinedTrack))	return false;
    
    // loose cut on momentumBalanceSignificance
    double significance	= m_momentumBalanceTool->momentumBalanceSignificance(combinedTrack);
    if (fabs(significance) > m_momentumBalanceCut)
    {
	ATH_MSG_DEBUG(" combinedTrackQualityCheck fails with momentumBalanceSignificance "
		      << significance );
	return false;
    }
    
    // loose cut on indet/combined q/p pull (not applicable to indet line fit)
    if (! indetTrack.info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	const Trk::Perigee* combinedPerigee = combinedTrack.perigeeParameters();
//      dynamic_cast<const Trk::MeasuredPerigee*>(combinedTrack.perigeeParameters());
	const Trk::Perigee* indetPerigee = indetTrack.perigeeParameters();
	if (combinedPerigee->covariance() && indetPerigee->covariance())
	{
	    double dpOverP2	= (*combinedPerigee->covariance())(4,4) *
				  combinedPerigee->momentum().mag2();
	    if (dpOverP2 < 1.E-6)
	    {
		// fail with unphysical momentum covariance
		m_messageHelper->printWarning(8);
		return false;
	    }
	    double sigma	= sqrt((*indetPerigee->covariance())(4,4));
	    double pull		= (combinedPerigee->parameters()[Trk::qOverP] -
				   indetPerigee->parameters()[Trk::qOverP] ) / sigma;

	    if (fabs(pull) > m_indetPullCut)
	    {
		// fail with too high momentum pull
		std::stringstream ss;
		ss << pull;
		m_messageHelper->printWarning(9, ss.str());
		return false;
	    }
	}
    }

    return true;
}
    
void
MuidMatchMaker::debugInput (const TrackCollection*	/*indetTracks*/,
			    const TrackCollection*	extrapolatedTracks,
			    const TrackCollection*	spectrometerTracks) const
{
    // check there are spectrometer tracks available for matching
    if (! spectrometerTracks->size())
    {
	ATH_MSG_DEBUG( " no spectrometer muons to match " );
	return;
    }

    // summarize extrapolated muons
    msg(MSG::DEBUG) << std::endl << " muon tracks: " << std::endl;
    unsigned extrapCount = 1;
    TrackCollection::const_iterator spect = spectrometerTracks->begin();
    for (TrackCollection::const_iterator extrap = extrapolatedTracks->begin();
	 extrap != extrapolatedTracks->end();
	 ++extrap, ++extrapCount, ++spect)
    {
	const Trk::Perigee* perigee = m_trackQuery->outgoingPerigee(**extrap);
	if (perigee)
	{
	    double deltaPerigee	= ((**extrap).perigeeParameters()->associatedSurface().center() -
				   (**spect).perigeeParameters()->associatedSurface().center()).mag();
	    if (deltaPerigee < 10.*CLHEP::mm)
	    {
		msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
				<< "    spectrometer " << std::setw(3) << extrapCount;
	    }
	    else
	    {
		msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
				<< "    extrapolated " << std::setw(3) << extrapCount;
	    }
	    printPerigee(*perigee);
	    if (perigee != (**extrap).perigeeParameters()) delete perigee;
	}
    }
}

void
MuidMatchMaker::debugOutput (const MuidTrackContainer*	combinedMuons,
			     const TrackCollection*	indetTracks,
			     const TrackCollection*	spectrometerTracks) const
{
    // return if no combined muons
    if (! combinedMuons->size()) return;

    msg(MSG::DEBUG) << std::endl << " combined tracks: " << std::endl;
    int trackCount = 1;
    for (const MuidTrack* comb : *combinedMuons)
    {
	int indetIndex		= 1;
	for (TrackCollection::const_iterator indet = indetTracks->begin();
	     indet != indetTracks->end();
	     ++indet, ++indetIndex)
	    if (*indet == comb->indetTrack()) break;
	
	int spectrometerIndex	= 1;
	for (TrackCollection::const_iterator spect = spectrometerTracks->begin();
	     spect != spectrometerTracks->end();
	     ++spect, ++spectrometerIndex)
	    if (*spect == comb->spectrometerTrack()) break;
	
	const Trk::Track* combinedTrack = comb->combinedTrack();
	msg(MSG::DEBUG) << "    " << trackCount 
			<< "/" << combinedMuons->size()
			<< "  inDet " << indetIndex
			<< "  spectrometer " << spectrometerIndex;
	if (combinedTrack)
	{
	    msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
			    << "   fit: chi2/degf  " << std::setw(6) << std::setprecision(2)
			    << combinedTrack->fitQuality()->chiSquared()/
		combinedTrack->fitQuality()->doubleNumberDoF();
	}
	else
	{
	    msg(MSG::DEBUG) << "   combined-fit failure  ";
	    
	}
	if (comb->matchDoF() > 0)
	    msg(MSG::DEBUG) << ",   matching: chi2 " << std::setw(6) << std::setprecision(1)
			    << comb->matchChi2()
			    << " (5 degf)  prob(chi2) " << std::setw(7) << std::setprecision(4)
			    << comb->matchProb();
	if (comb->isType(MuonMatch::Tight))
	{
	    msg(MSG::DEBUG) << "   Tight ";
	}
	else if (comb->isType(MuonMatch::Inner))
	{
	    msg(MSG::DEBUG) << "   Inner ";
	}
	else if (comb->isType(MuonMatch::Outer))
	{
	    msg(MSG::DEBUG) << "   Outer ";
	}
	else if (comb->isType(MuonMatch::Simple))
	{
	    msg(MSG::DEBUG) << "   Simple";
	}
	else if (comb->isType(MuonMatch::Loose))
	{
	    msg(MSG::DEBUG) << "   Loose ";
	}

	// discriminating variable tails:
	if (comb->fieldIntegral().betweenSpectrometerMeasurements() < 2.0)
	{
	    msg(MSG::DEBUG) << "   lowBdl" << std::setw(4) << std::setprecision(1)
			    << comb->fieldIntegral().betweenSpectrometerMeasurements();
	}
	if (fabs(comb->momentumBalanceSignificance()) > 2.5)
	{
	    msg(MSG::DEBUG) << "   momentumBalance" << std::setw(5) << std::setprecision(1)
			    << comb->momentumBalanceSignificance();
	}
	if (fabs(comb->scatteringAngleSignificance().curvatureSignificance()) > 3.0
	    || fabs(comb->scatteringAngleSignificance().neighbourSignificance()) > 3.0)
	{
	    msg(MSG::DEBUG) << "   scattering C,N" << std::setw(5) << std::setprecision(1)
			    << comb->scatteringAngleSignificance().curvatureSignificance()
			    << "," << std::setw(5) << std::setprecision(1)
			    << comb->scatteringAngleSignificance().neighbourSignificance();
	}
	
	if (comb->bestMatch())
	{
	    msg(MSG::DEBUG) << "   best match";
	}
	else
	{
	    msg(MSG::DEBUG) << "   ambiguous match";
	}    
	if (combinedTrack && combinedTrack->perigeeParameters())
	{
	    msg(MSG::DEBUG) << std::endl << "       parameters:  ";
	    printPerigee(*(combinedTrack->perigeeParameters()));
	}
	msg(MSG::DEBUG) << std::endl;
    }
    msg(MSG::DEBUG) << endmsg;
}

void
MuidMatchMaker::evaluateMatchProperties (MuonMatch& muonMatch) const
{
    // evaluate combined track discriminators:
    //   i.e. field integral, momentum balance and scattering angle significances
    const Trk::Track& track	= *muonMatch.combinedTrack();
    muonMatch.fieldIntegral(m_trackQuery->fieldIntegral(track));
    muonMatch.momentumBalanceSignificance(m_momentumBalanceTool->momentumBalanceSignificance(track));
    muonMatch.scatteringAngleSignificance(m_scatteringAngleTool->scatteringAngleSignificance(track));

    // keep original SA fit if no change to MS or Calo TSOS
    if (! extrapolatedNeedsRefit(muonMatch)) return;

    // include vertex region pseudo for extrapolation failure
    unsigned numberPseudo = 1;
    if (muonMatch.extrapolatedTrack())
	numberPseudo = m_trackQuery->numberPseudoMeasurements(*muonMatch.extrapolatedTrack());

    // make standalone fit from combined track (i.e. after cleaning)
    const Trk::Track* standaloneTrack = 0;
    if (! m_trackBuilder.empty())
	standaloneTrack = m_trackBuilder->standaloneRefit(*muonMatch.combinedTrack());
    if (! standaloneTrack && ! m_outwardsBuilder.empty())
	standaloneTrack = m_outwardsBuilder->standaloneRefit(*muonMatch.combinedTrack());
    if (standaloneTrack)
    {
	double matchChi2	=
	    m_matchQuality->innerMatchChi2(*muonMatch.indetTrack(),*standaloneTrack);
	int matchDoF		=
	    m_matchQuality->innerMatchDOF(*muonMatch.indetTrack(),*standaloneTrack);
	double matchProb	=
	    m_matchQuality->innerMatchProbability(*muonMatch.indetTrack(),*standaloneTrack);
	ATH_MSG_VERBOSE( " evaluateMatchProperties: chi2 re-evaluated from "
			 << muonMatch.innerMatchChi2() << " to " << matchChi2 );
	if (matchChi2 > 1.1*muonMatch.innerMatchChi2())
	    ATH_MSG_DEBUG( " evaluateMatchProperties: chi2 got worse: from "
			   << muonMatch.innerMatchChi2() << " to " << matchChi2 );

	// set re-evaluated chi2
	muonMatch.extrapolatedRefit(standaloneTrack);
	muonMatch.innerMatch(matchChi2,matchDoF,matchProb);
    }
    else if (! numberPseudo)
    {
	// failed re-evaluation of match chi2
	m_messageHelper->printWarning(10);
    }
}

bool
MuidMatchMaker::extrapolatedNeedsRefit (MuonMatch& muonMatch) const
{
    // will refit if extrapolated track was definitely bad
    if (! muonMatch.extrapolatedTrack())					return true;
    if (! m_trackQuery->isCaloAssociated(*muonMatch.extrapolatedTrack()))	return true;
    
    // otherwise will keep original SA fit if no change to MS or Calo TSOS
    const Trk::Track& originalTrack		= *muonMatch.extrapolatedTrack();
    const Trk::Track& combinedTrack 		= *muonMatch.combinedTrack();
    
    // refit if bad extrapolated fit - otherwise no refit if bad combined fit
    const Trk::FitQuality* fitQuality = originalTrack.fitQuality();
    if (! fitQuality
	|| ! fitQuality->numberDoF()
	|| fitQuality->chiSquared() / fitQuality->doubleNumberDoF() > m_badFitChi2)	return true;
    
    if (combinedTrack.fitQuality()->chiSquared()
	/ combinedTrack.fitQuality()->doubleNumberDoF() > m_badFitChi2)			return true;
    
    // check if need to update calo association
    const CaloEnergy* caloEnergyCombined	= m_trackQuery->caloEnergy(combinedTrack);
    const CaloEnergy* caloEnergyExtrapolated	= m_trackQuery->caloEnergy(originalTrack);
    if (! caloEnergyCombined || ! caloEnergyExtrapolated)
    {
        // no refit for combined track without CaloEnergy
	m_messageHelper->printWarning(11);
        return false;
    }
    double deltaE = caloEnergyExtrapolated->deltaE() - caloEnergyCombined ->deltaE();
    if (fabs(deltaE) > 0.3*caloEnergyExtrapolated->sigmaDeltaE())
    {
	ATH_MSG_VERBOSE( "extrapolatedNeedsRefit: caloEnergy difference " << deltaE
		      << "  sigma " << caloEnergyExtrapolated->sigmaDeltaE()
			 << "  ratio " << deltaE/caloEnergyExtrapolated->sigmaDeltaE());
	return true;
    }
    
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator o =
	originalTrack.trackStateOnSurfaces()->rbegin();
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator c =
	combinedTrack.trackStateOnSurfaces()->rbegin();
    for (; o != originalTrack.trackStateOnSurfaces()->rend(); ++o)
    {
	if (dynamic_cast<const Trk::PerigeeSurface*>(&(**o).surface()))	break;
	
	// compare measurements
	if ((**o).measurementOnTrack())
	{
	    // check measurements in phase
	    while (! (**c).measurementOnTrack()) ++c;
	    double separation = ((**o).trackParameters()->associatedSurface().center() -
				 (**c).trackParameters()->associatedSurface().center()).mag();
	    if (fabs(separation) > 1.*CLHEP::mm)
	    {
		ATH_MSG_VERBOSE("extrapolatedNeedsRefit: measurement out-of-phase: "
			     << " separation " << separation << "  extrap "
			     << (**o).trackParameters()->associatedSurface().center()
			     << "   comb " << (**c).trackParameters()->associatedSurface().center());
		return true;
	    }
	    
	    // different outlier 
	    if ((**o).type(Trk::TrackStateOnSurface::Outlier) !=
		(**c).type(Trk::TrackStateOnSurface::Outlier))
	    {
		if ((**c).type(Trk::TrackStateOnSurface::Outlier))
		{
		    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: outlier only on combined track " );
		}
		else
		{
		    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: outlier only on extrapolated track " );
		}
		return true;
	    }
	    	    
	    // drift sign flip 
	    if (dynamic_cast<const Muon::MdtDriftCircleOnTrack*>((**o).measurementOnTrack()))
	    {
		if ((**o).measurementOnTrack()->localParameters()[Trk::driftRadius] *
		    (**c).measurementOnTrack()->localParameters()[Trk::driftRadius] < 0.)
		{
		    ATH_MSG_VERBOSE("extrapolatedNeedsRefit: drift sign flip " );
		    return true;
		}
	    }
	    ++c;
	}
    }

    return false;    
}

void
MuidMatchMaker::indetCandidates (std::vector<IndetMatchCandidate*>&     candidates,
                                 const Trk::Track*                      extrapolatedTrack,
                                 const TrackCollection*                 indetTracks,
                                 const Trk::Track*                      spectrometerTrack) const
{
    // delete any pre-existing candidates (from previous spectrometer track) and clear vector
    for (std::vector<IndetMatchCandidate*>::iterator cand = candidates.begin();
	 cand != candidates.end();
	 ++cand)
	delete *cand;
    candidates.clear();

    // define muon (eta,phi)
    double muonEta      = 0.;
    double muonPhi      = 0.;
    if (extrapolatedTrack)
    {
        muonEta = extrapolatedTrack->perigeeParameters()->momentum().eta();
        muonPhi = extrapolatedTrack->perigeeParameters()->momentum().phi();
    }
    else
    {
        muonEta = spectrometerTrack->perigeeParameters()->position().eta();
        muonPhi = spectrometerTrack->perigeeParameters()->position().phi();
    }

    // select indet candidates (with loose momentum balance) in loose matching window
    // precompute inner and outer match chi2
    // no momentum balance as want to recover in MS
    unsigned index = 1;
    for (TrackCollection::const_iterator indet = indetTracks->begin() ;
         indet != indetTracks->end() ;
         ++indet, ++index)
    {
        // check indet track has sufficient energy to penetrate calo
        double indetEnergy      = fabs(1./(**indet).perigeeParameters()->parameters()[Trk::qOverP]);
        if (! m_trackQuery->isLineFit(**indet) && indetEnergy < m_indetMinEnergy) continue;
        if (! m_trackQuery->isLineFit(**indet) && (**indet).perigeeParameters()->momentum().perp() < m_indetMinPt ) continue;
          
        // check both tracks belong to same eta,phi region
        const Trk::Perigee* indetPerigee        = m_trackQuery->outgoingPerigee(**indet);
        if(!indetPerigee) continue;
        double indetEta                         = indetPerigee->momentum().eta();
        double deltaEta                         = fabs(muonEta - indetEta);
        double deltaPhi                         = muonPhi - indetPerigee->parameters()[Trk::phi];
        if (deltaPhi	> M_PI)  deltaPhi = deltaPhi - 2.*M_PI;
        if (deltaPhi	< -M_PI) deltaPhi = deltaPhi + 2.*M_PI;
        if (deltaEta		> m_deltaEtaPreSelection)  continue;
        if (fabs(deltaPhi)	> m_deltaPhiPreSelection)  continue;

        // express indet track as outgoing
        const Trk::Track* indetTrack            = m_trackQuery->outgoingTrack(**indet);
        if (indetTrack)
        {
            // ignore indet tracks with pseudomeasurements (these are TRT only tracks)
            // or starting with a TRT measurement
            const Trk::MeasurementBase* leadingMeasurement      = 0;
            for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
                     s = indetTrack->trackStateOnSurfaces()->begin();
                 s != indetTrack->trackStateOnSurfaces()->end();
                 ++s)
            {
                if (! (**s).measurementOnTrack()
                    || (**s).type(Trk::TrackStateOnSurface::Outlier)) continue;
                leadingMeasurement = (**s).measurementOnTrack();
                break;
            }

            if (m_matchTrtTracks
                || ! (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(leadingMeasurement)
                      || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(leadingMeasurement)))
            {
                // keep indet track as a candidate match
                candidates.push_back(new IndetMatchCandidate(extrapolatedTrack,
							     index,
                                                             *indet,
                                                             indetPerigee,
                                                             indetTrack,
							     spectrometerTrack));
		// compute inner match for extrapolated tracks
                if (extrapolatedTrack)
                {
                    candidates.back()->innerMatch(
                        m_matchQuality->innerMatchChi2(*extrapolatedTrack,*indetTrack),
                        m_matchQuality->innerMatchDOF(*extrapolatedTrack,*indetTrack),
                        m_matchQuality->innerMatchProbability(*extrapolatedTrack,*indetTrack),
                        m_matchQuality->simpleChi2(*extrapolatedTrack,*indetTrack));
                }
		// skip outer match when solenoid-off
		// compute outer match for all extrapolation failures and any reasonable inner matches
                if (! indetTrack->info().trackProperties(Trk::TrackInfo::StraightTrack)
		    && (! extrapolatedTrack
			|| candidates.back()->innerMatchChi2()      < 2.*m_matchChiSquaredCut
			|| candidates.back()->simpleMatchChi2()     < 2.*m_matchChiSquaredCut))
                {
                    candidates.back()->outerMatch(
                        m_matchQuality->outerMatchChi2(*indetTrack,*spectrometerTrack),
                        m_matchQuality->outerMatchDOF(*indetTrack,*spectrometerTrack),
                        m_matchQuality->outerMatchProbability(*indetTrack,*spectrometerTrack));
                }
            }
            else if (indetTrack != *indet)
            {
                delete indetTrack;
                indetTrack      = 0;
            }
        }       
        else if (indetPerigee   != (**indet).perigeeParameters())
        {
            delete indetPerigee;
            indetPerigee        = 0;
        }
    }

    // debug
    if (! msgLvl(MSG::DEBUG))   return;
    double muonPt       = 0.;
    if (extrapolatedTrack)
    {
        muonPt          = extrapolatedTrack->perigeeParameters()->charge() *
                          extrapolatedTrack->perigeeParameters()->pT();
    }
    else
    {
        muonPt          = spectrometerTrack->perigeeParameters()->charge() *
                          spectrometerTrack->perigeeParameters()->pT();
    }

    if (candidates.size())
    {	
	msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
			<< " inDet candidates for muon with pt "
			<< std::setw(8) << std::setprecision(2) << muonPt/CLHEP::GeV
			<< " at eta,phi "
			<< std::setw(6) << std::setprecision(2) << muonEta << ", "
			<< std::setw(6) << std::setprecision(2) << muonPhi << " :" << std::endl;
    }
    else
    {	
	msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
			<< " no inDet candidate for muon with pt "
			<< std::setw(8) << std::setprecision(2) << muonPt/CLHEP::GeV
			<< " at eta,phi "
			<< std::setw(6) << std::setprecision(2) << muonEta << ", "
			<< std::setw(6) << std::setprecision(2) << muonPhi;
    }
	
    for (std::vector<IndetMatchCandidate*>::iterator i = candidates.begin();
         i != candidates.end();
         ++i)
    {
        const Trk::Perigee* perigee = (**i).perigee();
        if (!perigee) continue;

        // count pseudo and silicon hits
        int pseudo  = 0;
        int silicon = 0;
        for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
                 t = (**i).indetTrack()->trackStateOnSurfaces()->begin();
             t != (**i).indetTrack()->trackStateOnSurfaces()->end();
             ++t)
        {
            const Trk::MeasurementBase* measurement = (**t).measurementOnTrack();
            if (! measurement
                || (**t).type(Trk::TrackStateOnSurface::Outlier)) continue;
            if (dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement))
                continue;
            
            if (dynamic_cast<const Trk::PseudoMeasurementOnTrack*>(measurement))
            {
                ++pseudo;
            }
            else
            {
                ++silicon;
            }
        }

        if (! silicon)
        {
            msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
                            << "        TRT_only " << std::setw(3) << (**i).indetIndex();
        }
        else if (pseudo)
        {
            msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
                            << std::setw(3) << pseudo << "  pseudo hits "
                            << std::setw(3) << (**i).indetIndex();
        }
        else
        {
            msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed)
                            << std::setw(8) << silicon << " si hits "
                            << std::setw(3) << (**i).indetIndex();
        }

        double deltaEta     = perigee->momentum().eta() - muonEta;
        double deltaPhi     = perigee->momentum().phi() - muonPhi;
        double cotTheta     = 1./tan(perigee->momentum().theta());
        double sinTheta     = 1./sqrt(1.+cotTheta*cotTheta);
        msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed | std::ios::right)
                        << "  a0,z0 "
                        << std::setw(7) << std::setprecision(2) << perigee->parameters()[Trk::d0]
                        << std::setw(11) << std::setprecision(2) << perigee->parameters()[Trk::z0]
                        << "  phi,theta "
                        << std::setw(7) << std::setprecision(4) << perigee->parameters()[Trk::phi]
                        << std::setw(8) << std::setprecision(4)
                        << perigee->parameters()[Trk::theta]
                        << "  pt"
                        << std::setw(8) << std::setprecision(2)
                        << perigee->charge()*perigee->pT()/CLHEP::GeV
                        << "  deltaEta, deltaPhi"
                        << std::setw(6) << std::setprecision(2) << deltaEta << ","
                        << std::setw(6) << std::setprecision(2) << deltaPhi;
        if ((**i).simpleMatchChi2() < 100. && (**i).simpleMatchChi2() + 5. < (**i).innerMatchChi2())
            msg(MSG::DEBUG) << "     simpleMatchChi2 " << std::setw(7) << std::setprecision(2)
                            << (**i).simpleMatchChi2();
        if ((**i).innerMatchChi2() < 1000.)
            msg(MSG::DEBUG) << "      innerMatchChi2 " << std::setw(7) << std::setprecision(2)
                            << (**i).innerMatchChi2();
        if ((**i).outerMatchChi2() < 1000.)
            msg(MSG::DEBUG) << "      outerMatchChi2 " << std::setw(7) << std::setprecision(2)
                            << (**i).outerMatchChi2();
        if (perigee->covariance()) {
            msg(MSG::DEBUG) << std::endl << "        diagonal errors: "
                            << std::setw(10) << std::setprecision(2)
                            << sqrt((*perigee->covariance())(0,0))
                            << std::setw(11) << std::setprecision(2)
                            << sqrt((*perigee->covariance())(1,1))
                            << std::setw(19) << std::setprecision(4)
                            << sqrt((*perigee->covariance())(2,2))
                            << std::setw(8) << std::setprecision(4)
                            << sqrt((*perigee->covariance())(3,3))
                            << std::setw(12) << std::setprecision(2)
                            << (sqrt((*perigee->covariance())(4,4)) *
                                sinTheta * perigee->momentum().mag2() / CLHEP::GeV)
                            << std::endl;
        }
    }
    msg(MSG::DEBUG) << endmsg;
}
    
const Trk::Track*
MuidMatchMaker::indetExtension (const Trk::Track&	indetTrack,
				const Trk::Track&	spectrometerTrack) const
{
    // this method requires curvature measurement from indet
    if (indetTrack.info().trackProperties(Trk::TrackInfo::StraightTrack)) return 0;
	       
    // build MeasurementSet for the spectrometer
    Trk::MeasurementSet spectrometerMeasurements;
    const Trk::TrackParameters* innerParameters		= 0;
    const Trk::TrackParameters* middleParameters	= 0;
    const Trk::TrackParameters* outerParameters		= 0;
    
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator s =
	     spectrometerTrack.trackStateOnSurfaces()->begin();
	 s != spectrometerTrack.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
	    continue;
	
	spectrometerMeasurements.push_back((**s).measurementOnTrack());

	if (! (**s).trackParameters())	continue;
	if (innerParameters)
	{
	    outerParameters		= (**s).trackParameters();
	}
	else
	{
	    innerParameters		= (**s).trackParameters();
	}
    } 

    // no track parameters for linefit
    if (spectrometerTrack.info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	innerParameters		= 0;
	outerParameters		= 0;
    }
    
    if (innerParameters && outerParameters)
    {
	double midDistance	= 0.5*(outerParameters->position() - innerParameters->position()).mag();
	double previousDistance	= 0.;
	for (DataVector<const Trk::TrackStateOnSurface>::const_iterator s =
		 spectrometerTrack.trackStateOnSurfaces()->begin();
	     s != spectrometerTrack.trackStateOnSurfaces()->end();
	     ++s)
	{	
	    if (! (**s).measurementOnTrack()
		|| ! (**s).trackParameters()
		|| dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
		continue;

	    double distance = ((**s).trackParameters()->position() - innerParameters->position()).mag();
	    if (distance < midDistance)
	    {
		middleParameters	= (**s).trackParameters();
		previousDistance	= distance;
	    }
	    else
	    {
		if (midDistance - previousDistance > distance - midDistance)
		    middleParameters	= (**s).trackParameters();
		break;
	    }
	}
    }
	
    // fit the combined track
    double combinedFitChi2		= 9999.;
    const Trk::Track* combinedTrack	= 0;
    if (! m_trackBuilder.empty())
    {
	combinedTrack			= m_trackBuilder->indetExtension(indetTrack,
									 spectrometerMeasurements,
									 innerParameters,
									 middleParameters,
									 outerParameters);
	if (combinedTrack && combinedTrack->fitQuality())
	    combinedFitChi2		= combinedTrack->fitQuality()->chiSquared() /
					  combinedTrack->fitQuality()->doubleNumberDoF();
    }
    if (combinedFitChi2 > m_badFitChi2 && ! m_outwardsBuilder.empty())
    {
	const Trk::Track* outwardsTrack	= m_outwardsBuilder->combinedFit(indetTrack,
									 spectrometerTrack,
									 spectrometerTrack);
	if (outwardsTrack &&
	    outwardsTrack->fitQuality()->chiSquared() /
	    outwardsTrack->fitQuality()->doubleNumberDoF()	< combinedFitChi2)
	{
	    ATH_MSG_VERBOSE( "indetExtension: choose outwards track" );
	    delete combinedTrack;
	    combinedTrack		= outwardsTrack;
	}
	else
	{
	    delete outwardsTrack;
	}
    }
    
    // filter out rubbish fits
    if (combinedTrack && combinedTrackQualityCheck(*combinedTrack,indetTrack,spectrometerTrack))
    {
	combinedTrack->info().addPatternReco(indetTrack.info());
	combinedTrack->info().addPatternReco(spectrometerTrack.info());
	combinedTrack->info().setParticleHypothesis(Trk::muon);
	combinedTrack->info().setPatternRecognitionInfo(Trk::TrackInfo::MuidCombined);
	return combinedTrack;
    }

    ATH_MSG_VERBOSE( "  indetExtension: fails fit " );
    delete combinedTrack;
    return 0;
}
        
bool
MuidMatchMaker::loosePositionMatch (const PerigeeParameters&	indet,
				    const MuonMatch&		matchCandidate) const
{
    ATH_MSG_VERBOSE( "   Try loose match" );

    // need indet and spectrometer PerigeeParameters
    m_failPositionMatch				= true;
    if (! matchCandidate.spectrometerTrack())	return false;
    const Trk::Perigee* spectrometerPerigee    	=
	matchCandidate.spectrometerTrack()->perigeeParameters();
    if (! spectrometerPerigee)			return false;
    PerigeeParameters* spectrometerCnvPerigee	= convertPerigee(spectrometerPerigee);
    if (! spectrometerCnvPerigee)		return false;
    const PerigeeParameters&	muon		= *spectrometerCnvPerigee;
    
    // position match taken at the spectrometer (entrance) surface
    const Trk::Surface&		matchingSurface	= spectrometerPerigee->associatedSurface();
    
    // extrapolate indet to calorimeter surface
    double qOverP	= indet.qOverP();
    double cotTheta	= indet.cotTheta();
    const Trk::TrackSurfaceIntersection* idIntersection   =
	new Trk::TrackSurfaceIntersection(indet.position(),indet.direction(),0.);
    const Trk::Surface* surface = m_caloCylinder;
    if (cotTheta > m_barrelCotTheta)
    {
	surface = m_caloForwardDisc;
    }
    else if (cotTheta < -m_barrelCotTheta)
    {
	surface = m_caloBackwardDisc;
    }
    const Trk::TrackSurfaceIntersection* caloIntersection =
	m_intersector->intersectSurface(*surface,idIntersection,qOverP);
    delete idIntersection;
    if (! caloIntersection)
    {
	delete spectrometerCnvPerigee;
	return false;
    }
    
    // parametrized energy deposition
    CaloEnergy* caloEnergy =
	m_caloEnergyParam->mopParametrizedEnergy(fabs(1./indet.qOverP()),
						 caloIntersection->position().eta(),
						 caloIntersection->position().phi());
    if (! caloEnergy )
    {
	delete caloIntersection;
	delete spectrometerCnvPerigee;
	return false;
    }

    // FIXME: apply energy loss
    // etaPhi at calo from MS back tracking
    //std::pair<double, double> etaPhi = caloScatteringModel.etaPhi();
    
    // apply position match (cut on deltaR)
    // use larger tolerance at low pT to account for Coulomb scattering
    // as quick fix relax cut (*1.5) to account for match at MS instead calo mid-point
    const Trk::TrackSurfaceIntersection* spectrometerIntersection =
	m_intersector->intersectSurface(matchingSurface,caloIntersection,qOverP);
    if (! spectrometerIntersection )
    {
	delete caloEnergy;
	delete caloIntersection;
	delete spectrometerCnvPerigee;
	return false;
    }
    double minInversePt	= fabs(indet.qOverPt());
    if (fabs(muon.qOverP()) < fabs(qOverP)) minInversePt *= fabs(muon.qOverP()/qOverP);
    double deltaR_Cut	= 1.5*(m_looseMatchCone + m_looseMatchScaling*minInversePt*CLHEP::GeV);
    double deltaEta	= fabs(spectrometerIntersection->position().eta() - muon.eta());
    double deltaPhi	= fabs(spectrometerIntersection->position().phi() - muon.phi());
    if (deltaPhi > M_PI) deltaPhi = 2.*M_PI - deltaPhi;
    //     if (sqrt(deltaPhi*deltaPhi + deltaEta*deltaEta) > 0.10)
    // 	std::cout << "     loose match with deltaEta "
    // 		  << deltaEta
    // 		  << "  deltaPhi "
    // 		  << deltaPhi
    // 		  << "  deltaR cut " << deltaR_Cut
    // 		  << std::endl;
    delete caloEnergy;
    delete caloIntersection;
    delete spectrometerCnvPerigee;
    delete spectrometerIntersection;
    if (deltaPhi*deltaPhi + deltaEta*deltaEta > deltaR_Cut*deltaR_Cut)
    {
	ATH_MSG_VERBOSE( "     fail loose match with deltaEta "
			 << deltaEta
			 << "  deltaPhi "
			 << deltaPhi
			 << "  deltaR cut " << deltaR_Cut );
	return false;
    }

    m_failPositionMatch = false;
    return true;
}

bool
MuidMatchMaker::momentumBalance (const Trk::Track*	indetTrack,
				 const Trk::Perigee*	outgoingIndetPerigee,
				 const Trk::Perigee*	outgoingSpectrometerPerigee,
				 const Trk::Track*	spectrometerTrack) const
{
    // fail on null track pointer 
    if (! indetTrack || ! spectrometerTrack)
    {
	m_failMomentumBalance = true;
	return false;
    }
    
    // accept if straight line in either indet or spectrometer - but fail if both SL
    if (spectrometerTrack->info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	if (indetTrack->info().trackProperties(Trk::TrackInfo::StraightTrack))
	{
	    m_failMomentumBalance = true;
	    return false;
	}
	
	return true;
    }
    else if (indetTrack->info().trackProperties(Trk::TrackInfo::StraightTrack))
    {
	return true;
    }

    // require MeasuredPerigee to calculate significance
    if (! outgoingIndetPerigee || ! outgoingIndetPerigee->covariance())
    {
	m_failMomentumBalance = true;
	return false;
    }
    if (! outgoingSpectrometerPerigee || ! outgoingSpectrometerPerigee->covariance())
    {
	m_failMomentumBalance = true;
	return false;
    }
    
    // parametrized energy deposition using best determined momentum
    double indetP			= outgoingIndetPerigee->momentum().mag();
    double indetSigma2			= (*outgoingIndetPerigee->covariance())(4,4);
    double indetDeltaP2overP2		= indetSigma2*indetP*indetP;
    double spectrometerP		= outgoingSpectrometerPerigee->momentum().mag();
    double spectrometerSigma2		= (*outgoingSpectrometerPerigee->covariance())(4,4);
    double spectrometerDeltaP2overP2	= spectrometerSigma2*spectrometerP*spectrometerP;
    double momentum			= spectrometerP;
    if (indetDeltaP2overP2 < spectrometerDeltaP2overP2) momentum = indetP;
    const CaloEnergy* caloEnergy =
	m_caloEnergyParam->meanParametrizedEnergy(momentum,
						  outgoingIndetPerigee->momentum().eta(),
						  outgoingIndetPerigee->momentum().phi());
    if (! caloEnergy)
    {
	m_failMomentumBalance = true;
	return false;
    }
    
    // reasonable calorimeter energy deposition limits
    // excluding tails > 2 sigma - which is very asymmetric!
    double caloE		= caloEnergy->deltaE();
    double caloMaxE		= caloE + 2.*caloEnergy->sigmaPlusDeltaE();
    double caloMinE		= caloE - 2.*caloEnergy->sigmaMinusDeltaE();
    delete caloEnergy;
    
    // significance of momentum imbalance (including calo energy deposit correction)
    double indetQoverP		=  outgoingIndetPerigee->parameters()[Trk::qOverP];
    double spectrometerQoverP	=  outgoingSpectrometerPerigee->parameters()[Trk::qOverP];
    double significance		=  fabs(spectrometerQoverP - indetQoverP) /
				   sqrt(indetSigma2 + spectrometerSigma2);

    double deltaP		=  spectrometerP - indetP;
    if (indetQoverP*spectrometerQoverP < 0.) deltaP = spectrometerP + indetP;
    double ratio		=  fabs(1. + caloE/deltaP);
    significance		*= ratio;

    // success if well balanced energies
    if (significance < m_looseMatchSigma)
    {
	if (significance > 3.0) ATH_MSG_DEBUG( "     momentum balance: medium match quality" );
	return true;
    }
    
    // fail conflicting sign of charge
    if (indetQoverP*spectrometerQoverP < 0.)
    {
	if (m_looseMatchSigma*m_looseMatchSigma*indetDeltaP2overP2 > 0.99)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: indet compatible with infinite momentum" );
	}
	if (m_looseMatchSigma*m_looseMatchSigma*spectrometerDeltaP2overP2 > 0.99)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: spectrometer compatible with infinite momentum" );
	}
	else
	{
	    ATH_MSG_VERBOSE( "     momentum balance: fail energy balance with charge conflict,"
			     << " charge significance (sigma) in MS,Indet "
			     << sqrt(spectrometerDeltaP2overP2) << " "
			     << sqrt(indetDeltaP2overP2) );
	    m_failChargeConflict = true;
	    return false;
	}
    }
    
    // make fine-check for excessive energy gain (incl. calo sigma)
    double deltaE	= spectrometerP + caloE - indetP;
    if (indetQoverP*spectrometerQoverP < 0.) deltaE = spectrometerP + caloE + indetP;
    if (deltaE > 0.)
    {
	if (m_looseMatchSigma*m_looseMatchSigma*indetDeltaP2overP2 > 0.99)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: indet compatible with infinite momentum" );
	    return true;
	}
	
	double spectrometerMinP	=
	    1. / (fabs(spectrometerQoverP) + m_looseMatchSigma*sqrt(spectrometerSigma2));
	double indetMaxP	=
	    1. / (fabs(indetQoverP) - m_looseMatchSigma*sqrt(indetSigma2));

	deltaE			= spectrometerMinP + caloMinE - indetMaxP;
	if (deltaE  > 0.)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: fail with too much energy gain "
			     << deltaE/CLHEP::GeV << " GeV"
			     << "  spectrometer min P " << spectrometerMinP/CLHEP::GeV
			     << "  calo min E " << caloMinE/CLHEP::GeV
			     << "  indet max P " << indetMaxP/CLHEP::GeV );

	    m_failEnergyGain = true;
	    return false;
	}
    }
    // or fine-check for too much energy loss (the FP bug)
    else
    {
	if (m_looseMatchSigma*m_looseMatchSigma*spectrometerDeltaP2overP2 > 0.99)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: spectrometer compatible with infinite momentum" );
	    return true;
	}
	
	double spectrometerMaxP	=
	    1. / (fabs(spectrometerQoverP) - m_looseMatchSigma*sqrt(spectrometerSigma2));
	double indetMinP	=
	    1. / (fabs(indetQoverP) + m_looseMatchSigma*sqrt(indetSigma2));

	deltaE			= spectrometerMaxP + caloMaxE - indetMinP;
	if (deltaE  < 0.)
	{
	    ATH_MSG_VERBOSE( "     momentum balance: fail with too much energy loss "
			     << deltaE/CLHEP::GeV << " GeV"
			     << "  spectrometer max P " << spectrometerMaxP/CLHEP::GeV
			     << "  calo max E " << caloMaxE/CLHEP::GeV
			     << "  indet min P " << indetMinP/CLHEP::GeV );

	    m_failEnergyLoss = true;
	    return false;
	}
    }

    if (significance > 3.0) ATH_MSG_DEBUG( "     momentum balance: medium match quality" );
    return true;
}

void
MuidMatchMaker::printPerigee (const Trk::Perigee& perigee) const
{
    double sinTheta = sin(perigee.momentum().theta());
    msg(MSG::DEBUG)  << std::setiosflags(std::ios::fixed | std::ios::right)
		     << "  a0,z0 "
		     << std::setw(7) << std::setprecision(2) << perigee.parameters()[Trk::d0]
		     << std::setw(11) << std::setprecision(2) << perigee.parameters()[Trk::z0]
		     << "  phi,theta "
		     << std::setw(7) << std::setprecision(4) << perigee.parameters()[Trk::phi]
		     << std::setw(8) << std::setprecision(4) << perigee.parameters()[Trk::theta]
		     << "  pt"
		     << std::setw(8) << std::setprecision(2) << perigee.charge()*perigee.pT()/CLHEP::GeV
		     << "  eta"
		     << std::setw(6) << std::setprecision(2) << perigee.momentum().eta()
		     << std::endl;
    if (!perigee.covariance()) return; 
    msg(MSG::DEBUG)  << "        diagonal errors: "
		     << std::setw(10) << std::setprecision(2)
		     << sqrt((*perigee.covariance())(0,0))
		     << std::setw(11) << std::setprecision(2)
		     << sqrt((*perigee.covariance())(1,1))
		     << std::setw(19) << std::setprecision(4)
		     << sqrt((*perigee.covariance())(2,2))
		     << std::setw(8) << std::setprecision(4)
		     << sqrt((*perigee.covariance())(3,3))
		     << std::setw(12) << std::setprecision(2)
		     << (sqrt((*perigee.covariance())(4,4)) *
			 sinTheta * perigee.momentum().mag2() / CLHEP::GeV)
		     << std::endl;    
// 	// add correlation coefficients for verbose debugging:
// 		     << std::endl << "           correlations: ";

//     for (int i = 0; i != 5; ++i)
//     {
// 	msg(MSG::DEBUG) << std::setiosflags(std::ios::fixed|std::ios::right);
	
// 	for (int j = 0; j < i; ++j)
// 	{
// 	    double temp = perigee.localErrorMatrix().covariance()[i][j] /
// 			  sqrt(perigee.localErrorMatrix().covariance()[i][i] *
// 			       perigee.localErrorMatrix().covariance()[j][j]);
// 	    msg(MSG::DEBUG) << std::setw(10) << std::setprecision(3) << temp;
	   
// 	}
// 	msg(MSG::DEBUG) << "        ";
//     }
    msg(MSG::DEBUG) << std::endl;
}

}	// end of namespace
