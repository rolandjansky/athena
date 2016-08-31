/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonTrackQuery
// 
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

//<<<<<< INCLUDES                                                       >>>>>>

#include <cmath>
#include <iomanip>
#include "InDetRIO_OnTrack/TRT_DriftCircleOnTrack.h"
#include "MuidTrackBuilder/MuonTrackQuery.h"
#include "MuonCompetingRIOsOnTrack/CompetingMuonClustersOnTrack.h"
#include "MuonRIO_OnTrack/MdtDriftCircleOnTrack.h"
#include "MuonRecHelperTools/MuonEDMHelperTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonRecToolInterfaces/IMdtDriftCircleOnTrackCreator.h"
#include "TrkDetDescrInterfaces/ITrackingVolumesSvc.h"
#include "TrkEventPrimitives/DriftCircleSide.h"
#include "TrkFitterInterfaces/ITrackFitter.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "muonEvent/CaloEnergy.h"
#include "TrkDetDescrInterfaces/ITrackingGeometrySvc.h"
#include "AthenaKernel/Units.h"

namespace Units = Athena::Units;

namespace Rec
{
    
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

MuonTrackQuery::MuonTrackQuery (const std::string&	type,
				const std::string&	name, 
				const IInterface*	parent)
    :	AthAlgTool		(type, name, parent),
	m_fitter		(""),
	m_helper                ("Muon::MuonEDMHelperTool/MuonEDMHelperTool"),
	m_idHelper              ("Muon::MuonIdHelperTool/MuonIdHelperTool"),
	m_mdtRotCreator		("Muon::MdtDriftCircleOnTrackCreator/MdtDriftCircleOnTrackCreator"),
	m_trackingGeometrySvc   ("TrackingGeometrySvc/AtlasTrackingGeometrySvc",name)
{
    declareInterface<IMuonTrackQuery>(this);
    declareProperty("Fitter",			m_fitter);
    declareProperty("MdtRotCreator",		m_mdtRotCreator);
    declareProperty( "TrackingGeometrySvc", m_trackingGeometrySvc);
}

MuonTrackQuery::~MuonTrackQuery (void) 
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
MuonTrackQuery::initialize()
{
    ATH_MSG_DEBUG( "Initializing MuonTrackQuery - package version " << PACKAGE_VERSION );

    // tool needed to refit slimmed tracks
    if (! m_fitter.empty())
    {
	if (m_fitter.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_fitter );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_DEBUG( "Retrieved tool " << m_fitter );
	}
    }

    // multipurpose and identifier helper tools for spectrometer
    if (m_helper.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_helper );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_DEBUG( "Retrieved tool " << m_helper );
    }
    if (m_idHelper.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_idHelper );
	return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_DEBUG( "Retrieved tool " << m_idHelper );
    }
    
    // tools needed when flipping tracks to outgoing
    if (! m_mdtRotCreator.empty())
    {
	if (m_mdtRotCreator.retrieve().isFailure())
	{
	    ATH_MSG_FATAL( "Failed to retrieve tool " << m_mdtRotCreator );
	    return StatusCode::FAILURE;
	}
	else
	{
	    ATH_MSG_DEBUG( "Retrieved tool " << m_mdtRotCreator );
	}
    }
    
    // need to know which TrackingVolume we are in: indet/calo/spectrometer
    if( m_trackingGeometrySvc.retrieve().isFailure() ){
      ATH_MSG_WARNING(" failed to retrieve geometry Svc " << m_trackingGeometrySvc);
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
}

StatusCode
MuonTrackQuery::finalize()
{
    ATH_MSG_VERBOSE( "Finalizing MuonTrackQuery" );
    
    return StatusCode::SUCCESS;
}

const CaloEnergy*
MuonTrackQuery::caloEnergy (const Trk::Track& track) const
{
    const CaloEnergy* caloEnergy = 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	// select MaterialEffects inside calorimeter volume
	if (! (**s).type(Trk::TrackStateOnSurface::CaloDeposit)
	    || ! (**s).materialEffectsOnTrack())	continue;
	const Trk::MaterialEffectsOnTrack* meot	=
	    dynamic_cast<const Trk::MaterialEffectsOnTrack*> ((**s).materialEffectsOnTrack());
	if (! meot)					continue;
	caloEnergy	= dynamic_cast<const CaloEnergy*>(meot->energyLoss());
	if (caloEnergy)					break;
    }
	    
    ATH_MSG_VERBOSE( "caloEnergy " << caloEnergy );
    
    return caloEnergy;
}

double
MuonTrackQuery::caloEnergyDeposit (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return 0.;
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    const Trk::TrackParameters* indetExitParameters	= 0;
    const Trk::TrackParameters* caloExitParameters	= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).trackParameters()) continue;
	if (indetVolume->inside((**s).trackParameters()->position()))
	{
	    indetExitParameters = (**s).trackParameters();
	    continue;
	}

	if (! calorimeterVolume->inside((**s).trackParameters()->position())
	    && caloExitParameters) break;
	
	caloExitParameters = (**s).trackParameters();
    }

    if (! indetExitParameters || ! caloExitParameters) return 0.;
    
    double energyDeposit = indetExitParameters->momentum().mag() - caloExitParameters->momentum().mag();
    
    ATH_MSG_VERBOSE( " calo energy deposit " );

    return energyDeposit;
}

FieldIntegral
MuonTrackQuery::fieldIntegral (const Trk::Track& track) const
{
    // field integral null for straight tracks + method is not valid for slimmed tracks
    if (isLineFit(track) || isSlimmed(track)) return FieldIntegral();

    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return FieldIntegral();
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    // sum Bdl between measurements
    double betweenInDetMeasurements		= 0.;
    double betweenSpectrometerMeasurements	= 0.;
    bool haveInDet				= false;
    bool haveSpectrometer			= false;
    Amg::Vector3D	integratedMomentumKick	= Amg::Vector3D(0.,0.,0.);
    const Trk::TrackParameters* parameters	= 0;

    // loop over TSOS to integrate vector Bdl
    int hit = 1;
    
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s, ++hit)
    {
	// not going to use trigger hits, outliers or pseudoMeasurements
	bool isPreciseHit	=
	    ((**s).measurementOnTrack()
	     && dynamic_cast<const Trk::MeasurementBase*>((**s).measurementOnTrack())
	     && ! dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())
	     && ! (**s).type(Trk::TrackStateOnSurface::Outlier));

	// skip slimmed measurements
	if (! (**s).trackParameters()
	    || dynamic_cast<const Trk::Perigee*>((**s).trackParameters()))	continue;

	// skip spectrometer phi measurements
	if (isPreciseHit
	    && ! calorimeterVolume->inside((**s).trackParameters()->position()))
	{
	    Identifier id	= m_helper->getIdentifier(*(**s).measurementOnTrack());
	    isPreciseHit	= (id.is_valid() && ! m_idHelper->measuresPhi(id));
	}
	if (! (**s).materialEffectsOnTrack() && ! isPreciseHit)			continue;

	// careful with first hit + skip any flipped parameters
	if (! parameters) parameters		= (**s).trackParameters();
	Amg::Vector3D startMomentum	= parameters->momentum();
	parameters				= (**s).trackParameters();	
	Amg::Vector3D endDirection	= parameters->momentum().unit();
	if (startMomentum.dot(endDirection) < 0.)				continue;
	
	// subtract scattering angle
	if ((**s).materialEffectsOnTrack())
	{
	    const Trk::MaterialEffectsOnTrack* meot =
		dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());
	    if (meot && meot->scatteringAngles())
	    {
		double theta	= endDirection.theta() - meot->scatteringAngles()->deltaTheta();
		double phi	= endDirection.phi() - meot->scatteringAngles()->deltaPhi();
		double cosTheta	= cos(theta);
		double sinTheta	= sqrt(1. - cosTheta*cosTheta);
		endDirection	= Amg::Vector3D(sinTheta*cos(phi),
						       sinTheta*sin(phi),
						       cosTheta);
	    }
	}
	Amg::Vector3D momentumKick	=  startMomentum.cross(endDirection)/(0.3*Units::GeV);	    
	integratedMomentumKick			+= momentumKick;

	// accumulate abs(Bdl) between measurements (distinguish indet and spectrometer)
	if (! isPreciseHit)							continue;
	if (indetVolume->inside(parameters->position()))
	{
	    if (haveInDet)
	    {
		betweenInDetMeasurements	+= integratedMomentumKick.mag();
		integratedMomentumKick		=  Amg::Vector3D(0.,0.,0.);
	    }
	    else
	    {
		haveInDet			=  true;
		integratedMomentumKick		=  Amg::Vector3D(0.,0.,0.);
	    }
	}
	else
	{
	    if (haveSpectrometer)
	    {
		betweenSpectrometerMeasurements	+= integratedMomentumKick.mag();
		integratedMomentumKick		=  Amg::Vector3D(0.,0.,0.);
	    }
	    else
	    {
		haveSpectrometer		=  true;
		integratedMomentumKick		=  Amg::Vector3D(0.,0.,0.);
	    }
	}
    }

    ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
		   << " field integrals for track at eta, phi " << std::setw(6) << std::setprecision(2)
		   << track.perigeeParameters()->momentum().eta()
		   << "," << std::setw(6) << std::setprecision(2)
		   << track.perigeeParameters()->momentum().phi()
		   << " : betweenInDetMeasurements "
		   << std::setw(8) << std::setprecision(3) << betweenInDetMeasurements
		   << "   betweenSpectrometerMeasurements "
		   << std::setw(8) << std::setprecision(3) << betweenSpectrometerMeasurements );
    
    return FieldIntegral(betweenInDetMeasurements,
			 betweenSpectrometerMeasurements,
			 0.,
			 0.);
}

bool
MuonTrackQuery::isCaloAssociated (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return false;
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    bool haveCaloDeposit	= false;
    int numberCaloTSOS		= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	// CaloDeposit?
	if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit))
	{
	    ATH_MSG_VERBOSE( "haveCaloDeposit, " << numberCaloTSOS << " TSOS in calo volume" );
	    haveCaloDeposit = true;
    }
    
	// select MaterialEffects inside calorimeter volume
	if (! (**s).materialEffectsOnTrack())		continue;
	Amg::Vector3D position =
	    (**s).materialEffectsOnTrack()->associatedSurface().globalReferencePoint();
	if (indetVolume->inside(position))		continue;
	if (! calorimeterVolume->inside(position))	break;
	if (++numberCaloTSOS > 2 && haveCaloDeposit)	return true;
    }
	    
    ATH_MSG_VERBOSE( "association false, " << numberCaloTSOS << " TSOS in calo volume" );
    
    return false;
}

bool
MuonTrackQuery::isCombined (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return false;
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    // combined tracks have indet and spectrometer active measurements
    bool indet		= false;
    bool spectrometer	= false;
    
    // find innermost measurement (assume some degree of measurement ordering)
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) continue;
	if (indetVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    indet	= true;
	    break;
	}
	else if (! calorimeterVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    spectrometer= true;
	    break;
	}
    }
       
    // find outermost measurement
    for (DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator
	     s = track.trackStateOnSurfaces()->rbegin();
	 s != track.trackStateOnSurfaces()->rend();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) continue;
	if (indetVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    indet	= true;
	    break;
	}
	else if (! calorimeterVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    spectrometer= true;
	    break;
	}
    }

    if (indet && spectrometer) return true;
    return false;
}

bool
MuonTrackQuery::isExtrapolated (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return false;
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    // perigee needs to be inside indet
    const Trk::TrackParameters* parameters	= track.perigeeParameters();
    if (! parameters || ! indetVolume->inside(parameters->position()))
    {
	return false;
    }
    
    // extrapolated tracks have indet TSOS but not active measurements
    // and spectrometer active measurements
    bool indet		= false;
    bool spectrometer	= false;

    // find innermost measurement (assume some degree of measurement ordering)
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) continue;
	if (indetVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    indet	= true;
	    break;
	}
	else if (! calorimeterVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    spectrometer= true;
	    break;
	}
    }
       
    // find outermost measurement (assume some degree of measurement ordering)
    for (DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator
	     s = track.trackStateOnSurfaces()->rbegin();
	 s != track.trackStateOnSurfaces()->rend();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) continue;
	if (indetVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    indet	= true;
	    break;
	}
	else if (! calorimeterVolume->inside((**s).measurementOnTrack()->globalPosition()))
	{
	    spectrometer= true;
	    break;
	}
    }

    if (! indet && spectrometer) return true;
    return false;
}

bool
MuonTrackQuery::isLineFit (const Trk::Track& track) const
{
    if (track.info().trackProperties(Trk::TrackInfo::StraightTrack))		return true;

    // for backwards compatibility
    const Trk::Perigee* measuredPerigee = track.perigeeParameters();
    if (! measuredPerigee)							return false;
    if (! measuredPerigee->covariance()) {
	ATH_MSG_WARNING( " measured Perigee has no covariance!" );
              return false;} 
    double momentumCovariance	=
	(*measuredPerigee->covariance())(Trk::qOverP,Trk::qOverP);
    double relativeMomentumCovariance	=momentumCovariance*measuredPerigee->momentum().mag2();

    // unphysically small value used to signal SL
    if (momentumCovariance < 1.E-19 || relativeMomentumCovariance < 1.E-6)	return true;

    // or if curvature unmeasured (error > 1000%)
    if (relativeMomentumCovariance > 100.)					return true;
    
    return false;
}

bool
MuonTrackQuery::isProjective (const Trk::Track& track) const
{
    // get start and end measured positions
    Amg::Vector3D startPosition(0.,0.,0.);
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if ((**s).measurementOnTrack()
	    && ! (**s).type(Trk::TrackStateOnSurface::Outlier)
	    && ! dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
	{
	    startPosition = (**s).measurementOnTrack()->globalPosition();
	    break;
	}
    }
    
    Amg::Vector3D endPosition(0.,0.,0.);
    for (DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator
	     s = track.trackStateOnSurfaces()->rbegin();
	 s != track.trackStateOnSurfaces()->rend();
	 ++s)
    {
	if ((**s).measurementOnTrack()
	    && ! (**s).type(Trk::TrackStateOnSurface::Outlier)
	    && ! dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
	{
	    endPosition = (**s).measurementOnTrack()->globalPosition();
	    break;
	}
    }

    // same-side check: kill if through (up-down) track
    double side = endPosition.dot(startPosition);
    if (side < 0.)
    {
	ATH_MSG_DEBUG( "track is not projective: fails same-side check " );
	return false;
    }
    
    // hence change in phi between these measurements
    double norm		= 1./sqrt(startPosition.perp2()*endPosition.perp2());
    double cosDeltaPhi	= norm*(endPosition.x()*startPosition.x() + endPosition.y()*startPosition.y());
    double sinDeltaPhi	= norm*(endPosition.x()*startPosition.y() - endPosition.y()*startPosition.x());

    // cut on change of long phi sector
    if (fabs(sinDeltaPhi) > M_PI/8.)
    {
	ATH_MSG_DEBUG( "track is not projective: sinDeltaPhi " << sinDeltaPhi );
	return false;
    }
    
    // cut on change of hemisphere (except for far forward tracks)
    if (cosDeltaPhi < 0.)
    {
	double cotTheta = startPosition.z()/startPosition.perp();
	if (fabs(startPosition.z()) > fabs(endPosition.z()))
	    cotTheta = endPosition.z()/endPosition.perp();

	// FIXME: isn't this same-side again?
	if (startPosition.z()*endPosition.z() < 0. || fabs(cotTheta) < 2.0)
	{
	    ATH_MSG_DEBUG( "track is not projective: cosDeltaPhi " << cosDeltaPhi );
	    return false;
	}
	ATH_MSG_WARNING( "forward track changes hemisphere: cotTheta " << cotTheta
			 << "  cosDeltaPhi " << cosDeltaPhi );
    }

    // OK - the track is at least roughly projective
    return true;
}

bool
MuonTrackQuery::isSectorOverlap (const Trk::Track& track) const
{
    double sectorOffset	= M_PI/16.;
    bool isOverlap	= true;
    double cosPhi	= 0.;
    double sinPhi	= 0.;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())) continue;
	
	if (isOverlap)
	{
	    isOverlap				= false;
	    Amg::Vector3D position	= (**s).measurementOnTrack()->
						  associatedSurface().globalReferencePoint();
	    cosPhi				= position.x() / position.perp();
	    sinPhi				= position.y() / position.perp();
	}
	else
	{
	    Amg::Vector3D position	= (**s).measurementOnTrack()->
						  associatedSurface().globalReferencePoint();
	    double sinDeltaPhi			= (position.x()*sinPhi - position.y()*cosPhi) /
						  position.perp();
	    if (fabs(sinDeltaPhi) > sectorOffset)
	    {
		ATH_MSG_DEBUG( "found overlap: sinDeltaPhi " << sinDeltaPhi );
		isOverlap	= true;
		break;
	    }
	}
    }
    
    return isOverlap;
}

bool
MuonTrackQuery::isSlimmed (const Trk::Track& track) const
{
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if ((**s).trackParameters()) continue;
	
	ATH_MSG_VERBOSE( "track is slimmed (found TSOS without trackParameters) " );
	return true;
    }

    ATH_MSG_VERBOSE( "track is not slimmed (all TSOS have trackParameters) " );
    
    return false;
}

double
MuonTrackQuery::momentumBalanceSignificance (const Trk::Track& track) const
{
    // significance only defined for combined muons
    if (! isCombined(track))	return 0.;

    // find the TSOS carrying caloEnergy (TSOS type CaloDeposit)
    // compare parameters with those from previous TSOS
    double energyBalance				= 0.;
    const Trk::EnergyLoss* energyLoss			= 0;
    const Trk::TrackParameters* previousParameters	= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).trackParameters())			continue;
	if ((**s).materialEffectsOnTrack())
	{
	    const Trk::MaterialEffectsOnTrack* meot	=
		dynamic_cast<const Trk::MaterialEffectsOnTrack*> ((**s).materialEffectsOnTrack());
	    if (! meot)					continue;
	    energyLoss	= meot->energyLoss();
	    if ((**s).type(Trk::TrackStateOnSurface::CaloDeposit) && energyLoss && previousParameters)
	    {
		energyBalance = previousParameters->momentum().mag() - energyLoss->deltaE() -
				(**s).trackParameters()->momentum().mag();
		if (fabs(energyBalance) < energyLoss->sigmaDeltaE())
		{
		    ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
				   << " momentum balance  "	<< std::setw(6) << std::setprecision(2)
				   << energyBalance/Units::GeV
				   << "   significance " 	<< std::setw(6) << std::setprecision(1)
				   << energyBalance/energyLoss->sigmaDeltaE()
				   << "  p before/after calo"	<< std::setw(7) << std::setprecision(2)
				   << previousParameters->momentum().mag()/Units::GeV
				   << " /"			<< std::setw(7) << std::setprecision(2)
				   << (**s).trackParameters()->momentum().mag()/Units::GeV
				   << "   energy deposit sigma  " 
				   << energyLoss->sigmaDeltaE()/Units::GeV << " GeV");
		    energyBalance /= energyLoss->sigmaDeltaE();
		}
		else if (energyBalance < 0.)
		{
		    ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
				   << " momentum balance  "	<< std::setw(6) << std::setprecision(2)
				   << energyBalance/Units::GeV
				   << "   significance "	<< std::setw(6) << std::setprecision(1)
				   << energyBalance/energyLoss->sigmaDeltaE()
				   << "  p before/after calo"	<< std::setw(7) << std::setprecision(2)
				   << previousParameters->momentum().mag()/Units::GeV
				   << " /"			<< std::setw(7) << std::setprecision(2)
				   << (**s).trackParameters()->momentum().mag()/Units::GeV
				   << "   energy deposit sigma- " 
				   << energyLoss->sigmaMinusDeltaE()/Units::GeV << " GeV");
		    energyBalance /= energyLoss->sigmaMinusDeltaE();
		}
		else
		{
		    ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
				   << " momentum balance  "	<< std::setw(6) << std::setprecision(2)
				   << energyBalance/Units::GeV
				   << "   significance " 	<< std::setw(6) << std::setprecision(1)
				   << energyBalance/energyLoss->sigmaDeltaE()
				   << "  p before/after calo"	<< std::setw(7) << std::setprecision(2)
				   << previousParameters->momentum().mag()/Units::GeV
				   << " /"			<< std::setw(7) << std::setprecision(2)
				   << (**s).trackParameters()->momentum().mag()/Units::GeV
				   << "   energy deposit sigma+ " 
				   << energyLoss->sigmaPlusDeltaE()/Units::GeV << " GeV");
		    energyBalance /= energyLoss->sigmaPlusDeltaE();
		}	
		break;
	    }
	}

	// update previous parameters
	previousParameters = (**s).trackParameters();
    }
	    
    return energyBalance;
}
    
unsigned
MuonTrackQuery::numberPseudoMeasurements (const Trk::Track& track) const
{
    unsigned numberPseudo = 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if ((**s).measurementOnTrack()
	    && dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack()))
	    ++numberPseudo;
    }

    ATH_MSG_VERBOSE( " track has " << numberPseudo << " PseudoMeasurements" );
    
    return numberPseudo;
}

const Trk::Perigee*
MuonTrackQuery::outgoingPerigee (const Trk::Track& track) const
{
    // only flip Perigee
    const Trk::Perigee* perigee		= track.perigeeParameters();
    if (! perigee)
    {
	ATH_MSG_WARNING( " input track without Perigee" );
	return track.perigeeParameters();
    }
    const Trk::PerigeeSurface& surface		= perigee->associatedSurface();
    
    const Trk::TrackStateOnSurface* outerTSOS	= track.trackStateOnSurfaces()->back();
    if (!perigee->covariance()) 
      {
	ATH_MSG_WARNING( " perigee has no covariance" );
	return track.perigeeParameters();
      }
    if (outerTSOS->trackParameters() == perigee
	&& perigee->momentum().dot(perigee->position()) <= 0.)
      {
	AmgSymMatrix(5) * perigeeCov = new AmgSymMatrix(5)(*perigee->covariance());
	// need to flip
	perigee =  new Trk::Perigee(perigee->position(),
					    -perigee->momentum(),
					    -perigee->charge(),
					    surface,
					    perigeeCov);
    }
    
    return perigee;
}

const Trk::Track*
MuonTrackQuery::outgoingTrack (const Trk::Track& track) const
{
    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0;
    }
    
    // check if perigee needs flip
    const Trk::Perigee* perigee	= outgoingPerigee(track);
    if (perigee == track.perigeeParameters()) return &track;
    if (!perigee ) return 0;
  
    // flipping requires rot creation
    if (m_mdtRotCreator.empty())
    {
	ATH_MSG_WARNING( " outgoingTrack: please configure rot creator for this method to work" );
        delete perigee;
	return 0;
    }
    
    // create a flipped track
    ATH_MSG_DEBUG( "outgoingTrack: flip track,"
		   << " new perigee at radius "	<< perigee->position().perp()
		   << "  phi "			<< perigee->momentum().phi()
		   << "  theta "		<< perigee->momentum().theta());

    DataVector<const Trk::TrackStateOnSurface>* flippedTSOS =
	new DataVector<const Trk::TrackStateOnSurface>;
    flippedTSOS->reserve(track.trackStateOnSurfaces()->size());
    const Trk::FitQualityOnSurface*	fitQoS		= 0;
    const Trk::MaterialEffectsBase*	materialEffects	= 0;
    const Trk::MeasurementBase*		measurement	= 0;
    const Trk::TrackParameters*		parameters	= 0;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> defaultPattern;
    std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> type = defaultPattern;

    // start with perigee TSOS
    type.set(Trk::TrackStateOnSurface::Perigee);
    flippedTSOS->push_back(new const Trk::TrackStateOnSurface(measurement,
							      perigee,
							      fitQoS,
							      materialEffects,
							      type));
    Amg::Vector3D direction = perigee->momentum().unit();
    
    // add remaining TSOS in reverse order
    DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator s =
	track.trackStateOnSurfaces()->rbegin();
    for (++s; s != track.trackStateOnSurfaces()->rend(); ++s)
    {
	measurement	= (**s).measurementOnTrack();
	materialEffects	= (**s).materialEffectsOnTrack();
	parameters	= (**s).trackParameters();
	type		= defaultPattern;
	if ((**s).type(Trk::TrackStateOnSurface::Hole)) type.set(Trk::TrackStateOnSurface::Hole);
	
	// if parameters need flip then so will material and drift measurements
	// also remove 'other-side' hits from incorrectly-split tracks
	if (parameters)
	{
	    if (parameters->momentum().dot(direction) > 0.)
	    {
		if (indetVolume->inside(parameters->position()))
		    flippedTSOS->push_back((**s).clone());
		continue;
	    }
	    else
	    {
		parameters = flippedParameters(*parameters);
	    }
	}

	// material: flip scattering angles and make sure energy deposit is +ve
	if (materialEffects)
	{
	    type.set(Trk::TrackStateOnSurface::Scatterer);
	    const Trk::MaterialEffectsOnTrack* meot		=
		dynamic_cast<const Trk::MaterialEffectsOnTrack*>(materialEffects);
	    if (meot)
	    {
		const Trk::EnergyLoss* energyLoss		= meot->energyLoss();
		if (energyLoss)
		{
		    double deltaE	= fabs(energyLoss->deltaE());
		    energyLoss		= new Trk::EnergyLoss(deltaE, energyLoss->sigmaDeltaE());
		}
		const Trk::ScatteringAngles* scatteringAngles	= meot->scatteringAngles();
		if (scatteringAngles)
		{
		    scatteringAngles = new Trk::ScatteringAngles(-scatteringAngles->deltaPhi(),
								 -scatteringAngles->deltaTheta(),
								 scatteringAngles->sigmaDeltaPhi(),
								 scatteringAngles->sigmaDeltaTheta());
		}
		materialEffects	=
		    new Trk::MaterialEffectsOnTrack(materialEffects->thicknessInX0(),
						    scatteringAngles, 
						    energyLoss,
						    materialEffects->associatedSurface());
	    }
	    else 
	    {
		materialEffects = materialEffects->clone();
	    }
	   
	}
	
	if (measurement)
	{
	   measurement        = measurement->clone();
	    type.set(Trk::TrackStateOnSurface::Measurement);
	    if ((**s).type(Trk::TrackStateOnSurface::Outlier))
		type.set(Trk::TrackStateOnSurface::Outlier);

	    const InDet::TRT_DriftCircleOnTrack* trtDriftCircle	=
		dynamic_cast<const InDet::TRT_DriftCircleOnTrack*>(measurement);
	    Muon::MdtDriftCircleOnTrack* mdtDriftCircle		=
		const_cast<Muon::MdtDriftCircleOnTrack*>(
		    dynamic_cast<const Muon::MdtDriftCircleOnTrack*>(measurement));
	    if (mdtDriftCircle)
	    {
		if (mdtDriftCircle->side() == Trk::LEFT)
		{
		    m_mdtRotCreator->updateSign(*mdtDriftCircle, Trk::RIGHT);
		}
		else if (mdtDriftCircle->side() == Trk::RIGHT)
		{
		    m_mdtRotCreator->updateSign(*mdtDriftCircle, Trk::LEFT);
		}
	    }
	    else if (trtDriftCircle)
	    {
		// no update sign method: so directly flip local position
		Trk::LocalParameters lpos(trtDriftCircle->localParameters());
 		if (trtDriftCircle->status() == Trk::DECIDED)
 		    lpos[Trk::driftRadius] = -lpos[Trk::driftRadius];

		if (parameters)
		{
			
		    measurement	=
			new InDet::TRT_DriftCircleOnTrack(trtDriftCircle->prepRawData(),
							  Trk::LocalParameters(lpos),
							  trtDriftCircle->localCovariance(),
							  trtDriftCircle->idDE(),
							  parameters->position().z(),
							  parameters->momentum().unit(),
							  trtDriftCircle->status());
		    delete trtDriftCircle;
		}
		else
		{
		    // FIXME: need updateSign method (like mdt) to avoid const_cast hack
		    Trk::LocalParameters& lposUpdate	=
			const_cast<Trk::LocalParameters&>(trtDriftCircle->localParameters());
 		    lposUpdate[Trk::driftRadius]	= lpos[Trk::driftRadius];
		}
	    }
	}

	flippedTSOS->push_back(new const Trk::TrackStateOnSurface(measurement,
								  parameters,
								  fitQoS,
								  materialEffects,
								  type));
    }

    Trk::Track* flippedTrack	= 0;
    if (track.fitQuality())
    {
	flippedTrack	= new Trk::Track(track.info(),flippedTSOS,track.fitQuality()->clone());
    }
    else
    {
	flippedTrack	= new Trk::Track(track.info(),flippedTSOS,0);
    }
    
    return flippedTrack;
}
 
ScatteringAngleSignificance
MuonTrackQuery::scatteringAngleSignificance (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return false;
    }

    const Trk::TrackingVolume* indetVolume       = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("InDet::Containers::InnerDetector");
    if( !indetVolume ){
      ATH_MSG_WARNING( "Failed to retrieve InDeT volume ");      
      return 0.;
    }

    // provide refit for slimmed tracks
    const Trk::Track* fullTrack		= &track;
    const Trk::Track* refittedTrack	= 0;
    if (isSlimmed(track))
    {		
	if (! m_fitter.empty())
	{
	    ATH_MSG_VERBOSE(" perform refit as track has been slimmed");
	    refittedTrack	= m_fitter->fit(track, false, Trk::muon);
	    fullTrack		= refittedTrack;
	}
	if (! refittedTrack) return ScatteringAngleSignificance(0);
    }
	
    // collect sigma of scatterer up to TSOS carrying caloEnergy
    double charge		= fullTrack->perigeeParameters()->charge();
    bool haveMeasurement	= false;
    unsigned scatterers		= 0;
    std::vector<bool> isMeasurement;
    std::vector<double> sigmas;
    std::vector<double> radii;
    double totalSigma		= 0.;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = fullTrack->trackStateOnSurfaces()->begin();
	 s != fullTrack->trackStateOnSurfaces()->end();
	 ++s)
    {
	// skip leading material
	if (! haveMeasurement)
	{
	    if ((**s).measurementOnTrack())  haveMeasurement	= true;
	    continue;
	}
	
	// select MaterialEffects up to calorimeter volume
	if (! (**s).materialEffectsOnTrack())				continue;
	Amg::Vector3D position =
	    (**s).materialEffectsOnTrack()->associatedSurface().globalReferencePoint();
	if (! calorimeterVolume->inside(position))			break;
	const Trk::MaterialEffectsOnTrack* meot	=
	    dynamic_cast<const Trk::MaterialEffectsOnTrack*> ((**s).materialEffectsOnTrack());
	if (! meot)							continue;
	if (! indetVolume->inside(position) && meot->energyLoss())	break;

	// get scattering angle
	const Trk::ScatteringAngles* scattering	= meot->scatteringAngles();
	if (! scattering) continue;
	++scatterers;
	if ((**s).measurementOnTrack())
	{
	    isMeasurement.push_back(true);
	}
	else
	{
	    isMeasurement.push_back(false);
	}   
	radii.push_back((**s).trackParameters()->position().perp());
	double sigma	=  charge*scattering->deltaPhi()/scattering->sigmaDeltaPhi();
	totalSigma	+= sigma;
	sigmas.push_back(sigma);
	
	ATH_MSG_VERBOSE( scatterers << " radius " << (**s).trackParameters()->position().perp()
			 << "   deltaPhi " << scattering->deltaPhi()
			 << "   significance " << sigma << "  totalSignificance " << totalSigma );
    }

    // quit if no scatterers selected
    delete refittedTrack;
    if (! scatterers) return ScatteringAngleSignificance(0);

    // search for discontinuity point (sign change of integral scattering)
    // and maximum of neighbouring scatterers
    // FIXME: neighbouring means scatterers spanning neighbouring measurements
    double curvatureSignificance	= totalSigma;
    double curvatureRadius		= 0.;
    double neighbourSignificance	= 0.;
    double neighbourRadius		= 0.;
    double previousSignificance		= 0.;
    double previousRadius		= 0.;
    unsigned index			= scatterers - 1;
    for (std::vector<double>::const_reverse_iterator s = sigmas.rbegin();
	 s != sigmas.rend();
	 ++s, --index)
    {
	totalSigma	-= 2.*(*s);
// 	ATH_MSG_VERBOSE( " scatteringAngleSignificance " << totalSigma
// 			 << " at radius " << radii[index] << " at scatterer " << index+1);
	
	if (fabs(totalSigma ) > fabs(curvatureSignificance))
	{		    
	    curvatureSignificance	= totalSigma ;
	    curvatureRadius		= radii[index];
	}
	if (fabs(sigmas[index]+previousSignificance) > fabs(neighbourSignificance))
	{
	    neighbourSignificance	= sigmas[index] + previousSignificance;
	    if (fabs(neighbourSignificance) > 0.)
	    {
		neighbourRadius		= (sigmas[index]*radii[index] +
					   previousSignificance*previousRadius) /
					  neighbourSignificance;
	    }
	    else
	    {
		neighbourRadius		= 0.;
	    }
	    
// 	    ATH_MSG_VERBOSE( " neighbourSignificance " << neighbourSignificance
// 			     << " at radius " << radii[index] << " at scatterer " << index+1);
	}
	
	previousSignificance	=  sigmas[index];
	previousRadius		=  radii[index];
    }

    // normalize
    curvatureSignificance	/= sqrt(static_cast<double>(scatterers));
    neighbourSignificance	/= sqrt(2.);

//     if (fabs(curvatureSignificance) < 3. && fabs(neighbourSignificance) < 3.)
//     {
	ATH_MSG_DEBUG( " scatteringAngleSignificance " << curvatureSignificance
		       << " at radius " << curvatureRadius
		       << " neighbourSignificance " << neighbourSignificance
		       << " at radius " << neighbourRadius
		       << " from " << scatterers << " scatterers");
//     }
//     else
//     {
// 	ATH_MSG_WARNING( " scatteringAngleSignificance " << curvatureSignificance
// 			 << " at radius " << curvatureRadius
// 			 << " neighbourSignificance " << neighbourSignificance
// 			 << " at radius " << neighbourRadius
// 			 << " from " << scatterers << " scatterers");
//     }	
	
    return ScatteringAngleSignificance(scatterers,
				       curvatureRadius,
				       curvatureSignificance,
				       neighbourRadius,
				       neighbourSignificance);
}
      
const Trk::TrackParameters*
MuonTrackQuery::spectrometerParameters (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return 0;
    }

    // find parameters at innermost spectrometer measurement
    // clone perigee if track has been slimmed
    const Trk::TrackParameters* parameters	= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || ! (**s).trackParameters()
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())
	    || calorimeterVolume->inside((**s).trackParameters()->position()))	continue;
	if (parameters
	    && (**s).trackParameters()->position().mag() > parameters->position().mag()) break;
	parameters = (**s).trackParameters();
    }
    
    if (! parameters) parameters = track.perigeeParameters();

    // if necessary, flip to outgoing parameters
    if (parameters->momentum().dot(parameters->position()) > 0.)
    {
	parameters = parameters->clone();
    }
    else
    {
	ATH_MSG_VERBOSE( " flip spectrometer parameters " );
	parameters	= flippedParameters(*parameters);
    }
    
    return parameters;
}
        
/**IMuonTrackQuery interface:
   assess the number of additional phi measurements needed for MS (or SA) track fit */
unsigned
MuonTrackQuery::spectrometerPhiQuality (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return false;
    }

    // quality 0 - no additional phi measurement needed
    //         1 - phi direction un- (or poorly) constrained
    //         2 - no phi hits on track
    const Trk::TrackStateOnSurface* leadingPhiMeasurement	= 0;
    const Trk::TrackStateOnSurface* trailingPhiMeasurement	= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())
	    || ! (**s).trackParameters()
	    || calorimeterVolume->inside((**s).trackParameters()->position()))	continue;
	Identifier id	= m_helper->getIdentifier(*(**s).measurementOnTrack());
	if (! id.is_valid() || ! m_idHelper->measuresPhi(id))				continue;

	// require phi measurement from CSC or CompetingROT
	if (! dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>((**s).measurementOnTrack())
	    && ! m_idHelper->isCsc(id))							continue;
	leadingPhiMeasurement	= *s;
	break;
    }
    if (! leadingPhiMeasurement)	return 2;
    for (DataVector<const Trk::TrackStateOnSurface>::const_reverse_iterator
	     r = track.trackStateOnSurfaces()->rbegin();
	 r != track.trackStateOnSurfaces()->rend();
	 ++r)
    {
	if (! (**r).measurementOnTrack()
	    || (**r).type(Trk::TrackStateOnSurface::Outlier)
	    || dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**r).measurementOnTrack())
	    || ! (**r).trackParameters())						continue;
	if (calorimeterVolume->inside((**r).trackParameters()->position()))		break;
	Identifier id	= m_helper->getIdentifier(*(**r).measurementOnTrack());
	if (! id.is_valid() || ! m_idHelper->measuresPhi(id))				continue;
	if (*r != leadingPhiMeasurement) trailingPhiMeasurement	= *r;
	break;
    }
    if (! trailingPhiMeasurement)	return 1;

    // FIXME: check separation between phi meas
    double separation	= (leadingPhiMeasurement->trackParameters()->position() -
			   trailingPhiMeasurement->trackParameters()->position()).mag();
    if (separation < 2.*Units::meter)	return 1;
    return 0;
}
    
/**IMuonTrackQuery interface:
   trackParameters at innermost trigger chamber TSOS in MS */
const Trk::TrackParameters*
MuonTrackQuery::triggerStationParameters (const Trk::Track& track) const
{
    const Trk::TrackingVolume* calorimeterVolume = m_trackingGeometrySvc->trackingGeometry()->trackingVolume("Calo::Container");
    if( !calorimeterVolume ) {
      ATH_MSG_WARNING( "Failed to retrieve Calo volume ");
      return 0;
    }

    // find parameters at innermost trigger station measurement
    // fails if track has been slimmed
    const Trk::TrackParameters* parameters	= 0;
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	     s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s)
    {
	if (! (**s).measurementOnTrack()
	    || (**s).type(Trk::TrackStateOnSurface::Outlier)
	    || ! (**s).trackParameters()
	    || ! dynamic_cast<const Muon::CompetingMuonClustersOnTrack*>((**s).measurementOnTrack())
	    || calorimeterVolume->inside((**s).trackParameters()->position()))	continue;
	if (parameters
	    && (**s).trackParameters()->position().mag() > parameters->position().mag()) break;
	parameters = (**s).trackParameters();
    }
    
    if (! parameters) return 0;

    // if necessary, flip to outgoing parameters
       if (parameters->momentum().dot(parameters->position()) > 0.)
    {
        parameters = parameters->clone();
    }
    else
    {
	ATH_MSG_VERBOSE( " flip spectrometer parameters " );
	parameters	= flippedParameters(*parameters);
    }

    return parameters;
}

const Trk::TrackParameters*
MuonTrackQuery::flippedParameters (const Trk::TrackParameters& parameters) const
{
    // flip parameters
    const Trk::TrackParameters* flippedParams = 0;
    
    double phi			= parameters.parameters()[Trk::phi0];
    if (phi > 0.)
    {
	phi	-= M_PI;
    }
    else
    {
	phi	+= M_PI;
    }
    double theta		= M_PI - parameters.parameters()[Trk::theta];
    double qOverP		= -parameters.parameters()[Trk::qOverP];
    const Trk::Surface* surface	= &(parameters.associatedSurface());

   flippedParams = surface->createTrackParameters(parameters.parameters()[Trk::d0],
						  parameters.parameters()[Trk::z0],
						  phi,
						  theta,
						  qOverP,
						  parameters.covariance() ? new AmgSymMatrix(5)(*parameters.covariance()) : 0 );
   
    return flippedParams;
}

    
}	// end of namespace
