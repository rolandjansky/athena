/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 AlgTool for TRT data access and pattern recognition for a single Track.
 adapted from code by Jim Loken
 ***************************************************************************/

#include <iomanip>
#include "GaudiKernel/SystemOfUnits.h"
#include "TRT_Rec/TRT_Prediction.h"
#include "TRT_Rec/TRT_Recon.h"
#include "TRT_Rec/TRT_RoadData.h"
#include "TrkExInterfaces/IIntersector.h"
#include "TrkExInterfaces/IPropagator.h"
#include "TrkExUtils/TrackSurfaceIntersection.h"
#include "TrkGeometry/Layer.h"
#include "TrkGeometry/LayerMaterialProperties.h"
#include "TrkGeometry/MaterialProperties.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/StraightLineSurface.h"
#include "TrkSurfaces/Surface.h"
#include "TrkToolInterfaces/IRIO_OnTrackCreator.h"
#include "iPatInterfaces/IDetectorSelection.h"
#include "iPatInterfaces/ITRT_DataSelector.h"
#include "iPatInterfaces/ITRT_Histogrammer.h"
#include "iPatInterfaces/ITRT_Predictor.h"
#include "iPatTrack/HitOnTrack.h"
#include "iPatTrackParameters/PerigeeParameters.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

TRT_Recon::TRT_Recon(const std::string&	type,
		     const std::string&	name, 
		     const IInterface* 	parent)
    :	AthAlgTool			(type, name, parent),
	m_dataSelector			("TRT_DataSelector/TRT_DataSelector"),
	m_detectorSelection		("DetectorSelection/DetectorSelection"),
	m_driftCircleOnTrackCreator	(
	    "InDet::TRT_DriftCircleOnTrackTool/TRT_DriftCircleOnTrackTool"),
	m_histogrammer			("TRT_Histogrammer/TRT_Histogrammer"),
	m_intersector			("Trk::RungeKuttaIntersector/RungeKuttaIntersector"),
	m_predictor			("TRT_Predictor/TRT_Predictor"),
	m_propagator			("Trk::IntersectorWrapper/IntersectorWrapper"),
	m_strawOnTrackCreator		(
	    "InDet::TRT_DriftCircleOnTrackNoDriftTimeTool/TRT_DriftCircleOnTrackNoDriftTimeTool"),
	m_minFractionExpected		(0.60),      // fraction of expected straws actually found
	m_minStraws			(10),        // min number of found straws
	m_muonTrThreshold		(120.*Gaudi::Units::GeV),  // tr threshold for muons
	m_sigmaDriftHit			(0.170*Gaudi::Units::mm),  // current rms from G4
	m_sigmaStrawHit			(1.155*Gaudi::Units::mm),  // (straw diameter)/sqrt12
	m_sigmaWireHit			(0.220*Gaudi::Units::mm ), // when drift hit remains left/right ambiguous
	m_trackBremLike			(0),
	m_trackCount			(0),
	m_trackTrRich			(0),
	m_trackVeto			(0)
{
    declareInterface<ITRT_Recon>(this);
    declareProperty("TRT_DataSelector",			m_dataSelector);
    declareProperty("DetectorSelection",		m_detectorSelection);
    declareProperty("DriftCircleOnTrackCreator",	m_driftCircleOnTrackCreator);
    declareProperty("TRT_Histogrammer",			m_histogrammer);
    declareProperty("Intersector",			m_intersector);
    declareProperty("TRT_Predictor",			m_predictor);
    declareProperty("StrawOnTrackCreator",		m_strawOnTrackCreator);
    declareProperty("minFractionExpected",		m_minFractionExpected);
    declareProperty("minStraws",			m_minStraws);
    declareProperty("muonTrThreshold",			m_muonTrThreshold);
    declareProperty("sigmaDriftHit",			m_sigmaDriftHit);
    declareProperty("sigmaStrawHit",			m_sigmaStrawHit);
    declareProperty("sigmaWireHit",			m_sigmaWireHit);
}

TRT_Recon::~TRT_Recon()
{}

//<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

StatusCode
TRT_Recon::initialize()
{
    // print where you are
    ATH_MSG_INFO( "TRT_Recon::initialize() - package version " << PACKAGE_VERSION );

    // get the detectorSelection Tool
    if (m_detectorSelection.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_detectorSelection );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_detectorSelection );
    }

    // warn and quit if the TRT (hence TRT_Recon) is not selected for processing
    if (! m_detectorSelection->processTRT())
    {
	ATH_MSG_INFO( " **** NOTE: TRT_Recon is deselected "  );
	return StatusCode::SUCCESS;
    }

    // get the remaining Tools
    if (m_dataSelector.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_dataSelector );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_dataSelector );
    }
    if (m_driftCircleOnTrackCreator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_driftCircleOnTrackCreator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_driftCircleOnTrackCreator );
    }
    if (m_histogrammer.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_histogrammer );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_histogrammer );
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
    if (m_predictor.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_predictor );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_predictor );
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
    if (m_strawOnTrackCreator.retrieve().isFailure())
    {
	ATH_MSG_FATAL( "Failed to retrieve tool " << m_strawOnTrackCreator );
        return StatusCode::FAILURE;
    }
    else
    {
	ATH_MSG_INFO( "Retrieved tool " << m_strawOnTrackCreator );
    }

    return StatusCode::SUCCESS;
}

StatusCode
TRT_Recon::finalize()
{
    ATH_MSG_INFO( "finalised after " << m_trackCount << " tracks confirmed, " 
	<< m_trackBremLike << " brem-like tracks, " << m_trackTrRich << " tr-rich tracks, and "
	<< m_trackVeto << " tracks vetoed " );
  
    for (std::vector<HitOnTrack*>::reverse_iterator h = m_hitList.rbegin(); h != m_hitList.rend(); ++h)
	m_hitList.pop_back();
    
    return StatusCode::SUCCESS;
}

bool
TRT_Recon::bremLike(void) const
{ return m_bremLike; }

const std::vector<HitOnTrack*>*
TRT_Recon::reconstructSegment(const PerigeeParameters&		perigee,
			      const TrackParameters&		params,
			      const Trk::TrackParameters&	trkParameters)
{
    // note return possibilities:
    //   null pointer   - outside TRT acceptance (or TRT deselected)
    //   empty list     - TRT veto (i.e. no TRT association)
    //   populated list - TRT confirmation (at least minStraws associated)
    if (! m_detectorSelection->processTRT())
    {
	ATH_MSG_DEBUG( "reconstructTrack *** TRT not selected" );
	return 0;
    }
    else
    {
	ATH_MSG_DEBUG( "reconstructTrack from start parameters:  "
		       << std::setiosflags(std::ios::fixed|std::ios::right)
		       << std::setw(10) << std::setprecision(0) << params.position().perp()
		       << " r"
		       << std::setw(7) << std::setprecision(3) << params.position().phi()
		       << " phi"
		       << std::setw(7) << std::setprecision(0) << params.position().z()
		       << " z"
		       << std::setw(8) << std::setprecision(3) << params.position().theta()
		       << " theta" 
		       << std::setw(13) << std::setprecision(3) << params.pt()/Gaudi::Units::GeV
		       << " pT (GeV)" );
    }
    
    // remove stuff referring to any previous track
    for (std::vector<HitOnTrack*>::reverse_iterator h = m_hitList.rbegin(); h != m_hitList.rend(); ++h)
	m_hitList.pop_back();

    // make predictions in a narrow road about the input track:
    const std::vector<TRT_Prediction*>& predictions = m_predictor->makePredictions(perigee,params);
    
    // crude acceptance check
    if (m_predictor->intersectedLayers() < m_minStraws)
    {
	ATH_MSG_DEBUG( " OUTSIDE acceptance " );
	return 0;
    }
    
    // store deflection at the end of the road to make 'brem-like' check
    m_bremLike	= false;
    m_rMax	= (**predictions.rbegin()).rRoad();
    m_rPhiMax	= (**predictions.rbegin()).rPhiBrem();
    
    // find the data compatible with the predictions (i.e. in the road)
    const std::vector<TRT_RoadData*>& roadDataList =
	m_dataSelector->roadData(predictions,params.position().perp());

    // acceptance/veto checks
    //     std::cout << std::setiosflags(std::ios::fixed)
    //      	      << " expectedStraws,intersectedLayers"
    //     	      << std::setw(7) << std::setprecision(1)  << m_predictor->expectedStraws() 
    //     	      << std::setw(4) << m_predictor->intersectedLayers() << std::endl
    //     	      << " selectedStraws,selectedLayers   "
    //      	      << std::setw(5) << m_dataSelector->selectedStraws() 
    //     	      << std::setw(6) << m_dataSelector->selectedLayers()
    //     	      << std::endl;
    if (m_dataSelector->selectedLayers() < m_minStraws)
    {
	if (m_minFractionExpected*m_predictor->expectedStraws() < m_minStraws)
	{
	    ATH_MSG_DEBUG( " OUTSIDE acceptance " );
	    return 0;
	}
	else
	{
	    // 	    std::cout << " VETO " << std::endl;
	    // 	    std::cout << std::setiosflags(std::ios::fixed) << " VETO " 
	    //          	      << " expectedStraws,intersectedLayers"
	    //         	      << std::setw(7) << std::setprecision(1)  << m_predictor->expectedStraws() 
	    //         	      << std::setw(4) << m_predictor->intersectedLayers()
	    //         	      << "   selectedStraws,selectedLayers   "
	    //          	      << std::setw(5) << m_dataSelector->selectedStraws() 
	    //         	      << std::setw(6) << m_dataSelector->selectedLayers()
	    //         	      << std::endl;
	    ++m_trackVeto;
	    ATH_MSG_DEBUG( " VETO " );
	    return &m_hitList;
	}
    }
    
    // find the best track in the standard narrow road
    if (! m_histogrammer->confirmSegment(trkParameters, roadDataList))
    {
	ATH_MSG_DEBUG( " VETO " );
        ++m_trackVeto;
	return &m_hitList;
    }
	
    m_bremLike = m_histogrammer->bremLike(m_rMax,m_rPhiMax);

    // return without electron recovery - temporarily during migration phase
    if (! m_bremLike
	&& m_histogrammer->numLayers() + m_histogrammer->numTrHits() <
	m_minFractionExpected*m_predictor->expectedStraws())
    {
	ATH_MSG_DEBUG( " VETO " );
        ++m_trackVeto;
	return &m_hitList;
    }

    // keep some statistics, debug if requested
    ++m_trackCount;
    if (m_bremLike) ++m_trackBremLike;
    if (m_histogrammer->trRich()) ++m_trackTrRich;
    makeHitList(roadDataList,trkParameters);
    

    if (msgLvl(MSG::DEBUG))
    {
	std::string text = " straws";
	if (m_bremLike) text += " brem-like ";
	if (m_histogrammer->trRich()) text += " tr-rich ";
	ATH_MSG_DEBUG( " confirmed with " << m_hitList.size() << text );
    }

    
    // include scattering centres for TRT material treatment
    //if (m_hitList.size()) allocateMaterial(trkParameters);
    return &m_hitList;

    // keep old electron recovery code - this will need to be migrated soon
//     // Decide whether there's a good association.
//     std::vector<TRT_RoadData::StrawStatus> strawStatus;
//     bool electronRecover = true;
//     int good = 0;
//     int goodAndClose = 0;
//     int trCount = 0;
//     int trInRoad = 0;
//     Identifier lastId;
//     for (std::vector<TRT_RoadData*>::const_iterator dit = roadDataList.begin(); 
// 	 dit != roadDataList.end();
// 	 ++dit)
//     {
// 	const TRT_RoadData& roadData = **dit;
// 	if (roadData.trThreshold() > 0 && roadData.identify() != lastId)
// 	{
// 	    ++trInRoad;
// 	    lastId = roadData.identify();
// 	}
// 	if (roadData.strawStatus() > TRT_RoadData::notConfirmed) {
// 	    ++goodAndClose;
// 	    if (roadData.trThreshold() > 0) ++trCount;
// 	    if (roadData.strawStatus() < TRT_RoadData::confirmedStraw) ++good;
// 	}
//     }
//     //     std::cout << "Number of hits in track road: total " << trtData.size()
//     //  	      << " goodAndClose " << goodAndClose 
//     // 	      << " good " << good
//     // 	      << " trCount " << trCount
//     // 	      << " trInRoad " << trInRoad << std::endl;

//     if (electronRecover &&
// 	trInRoad >= 6 &&
// 	trInRoad > trCount)
//     {
// 	// Now, repeat and look for electrons in a wider road.
// 	for(std::vector<TRT_RoadData*>::const_iterator dit = roadDataList.begin(); 
// 	    dit != roadDataList.end();
// 	    ++dit)
// 	{
// 	    TRT_RoadData& roadData = **dit;
// 	    // Save the confirm data from the original fit.
// 	    strawStatus.push_back(roadData.strawStatus());
// 	    // Reset the strawStatus flag.
// 	    roadData.strawStatus(TRT_RoadData::notConfirmed);
// 	}
// 	m_histo->confirm(3);

// 	// Decide whether there's a better association.
// 	int goodAndClose_E = 0;
// 	int trCount_E = 0;
// 	for (std::vector<TRT_RoadData*>::const_iterator dit = roadDataList.begin(); 
// 	     dit != roadDataList.end();
// 	     ++dit)
// 	{
// 	    const TRT_RoadData& roadData = **dit;
// 	    if (roadData.strawStatus() > TRT_RoadData::notConfirmed)
// 	    {
// 		++goodAndClose_E;
// 		if (roadData.trThreshold() > 0) ++trCount_E;
// 	    }
// 	}	
// 	// 	std::cout << "Number of hits in track road: total " << trtData.size()
// 	// 		  << " goodAndClose " << goodAndClose_E 
// 	// 		  << "        "
// 	// 		  << " trCount " << trCount_E << std::endl;

// 	// Decide whether electronRecover mode gave a better track.
// 	if (trCount_E > trCount && goodAndClose_E >= goodAndClose)
// 	{
// 	    // 	    std::cout << "electronRecover track used" << std::endl;
// 	    goodAndClose = goodAndClose_E;
// 	} else 
// 	{
// 	    // Revert back to the normal fit;
// 	    std::vector<TRT_RoadData::StrawStatus>::iterator sit = strawStatus.begin();
// 	    for (std::vector<TRT_RoadData*>::const_iterator dit = roadDataList.begin(); 
// 		 dit != roadDataList.end();
// 		 ++dit)
// 	    {
// 		TRT_RoadData& roadData = **dit;
// 		roadData.strawStatus((*sit));
// 		++sit;
// 	    }
// 	    // 	    std::cout << "Revert to original fit" << std::endl;
// 	}
//     }

//     //    if (goodAndClose < minStraws) return 0;

//     hits = hitList(roadDataList);
//     if (m_debug)
//     {
// 	*m_log << std::endl << " confirmed with " << hits->size() << " straws ";
// 	if (m_bremLike) *m_log << " brem-like ";
// 	if (m_histo->trRich()) *m_log << " tr-rich ";
// 	*m_log );
//     }
//     return hits;
}

//<<<<<< PRIVATE MEMBER FUNCTION DEFINITIONS                            >>>>>>

void
TRT_Recon::makeHitList(const std::vector<TRT_RoadData*>& roadDataList,
		       const Trk::TrackParameters& trkParameters)
{
    // store hits corresponding to selected roadData
    ATH_MSG_VERBOSE( std::endl << "selected hits:    straw r      phi         z"
		     << "   rPhiRoad   driftSigma driftDist  residual normResid" );

    int i = 0;
    for (std::vector<TRT_RoadData*>::const_iterator dit = roadDataList.begin(); 
	 dit != roadDataList.end();
	 ++dit)
    {
	const TRT_RoadData& roadData = **dit;
	if (roadData.strawStatus() <= TRT_RoadData::notConfirmed) continue;
	HitStatus	status			= drift_wire;
	double	driftDistance			= 0.0;
	double	driftSigma			= m_sigmaDriftHit;
	const Trk::MeasurementBase* rotBroad	= 0;
	const Trk::MeasurementBase* rotPrecise	= 0;
	const Identifier& id   			= roadData.identify();
	const Trk::StraightLineSurface* surface	= dynamic_cast<const Trk::StraightLineSurface*>
						  (&roadData.rio()->detectorElement()->surface(id));
	if (! surface)		continue;
	Amg::Vector3D position	= surface->center();
	// temp hack
	double x		= position.x();
	double y		= position.y();
	double z		= position.z();
	
	if (roadData.inBarrel())
	{	
	    double cotTheta	= trkParameters.momentum().z()/trkParameters.momentum().perp();
	    double deltaR	= position.perp() - trkParameters.position().perp();
	    z			= trkParameters.position().z() + deltaR*cotTheta;
	}
	else
	{
	    double tanTheta	= trkParameters.momentum().perp()/trkParameters.momentum().z();
	    double deltaZ	= position.z() - trkParameters.position().z();
	    double ratio       	= (trkParameters.position().perp() + deltaZ*tanTheta)/position.perp();
	    x			*= ratio;
	    y			*= ratio;
	}
    Trk::AtaStraightLine parameters(Amg::Vector3D(x,y,z),
				    trkParameters.momentum(),
				    trkParameters.charge(),
				    *surface);
	rotBroad		= m_strawOnTrackCreator->correct(*roadData.rio(),trkParameters);
	if (roadData.strawStatus() == TRT_RoadData::confirmedDrift)
	{
	    status		= drift_time;
	    driftDistance	= roadData.driftDistance();
	    // default: driftSigma = m_sigmaDriftHit;
	    Amg::Vector3D drift(-position.y()*driftDistance/position.perp(),
				position.x()*driftDistance/position.perp(),
				0.); 
	    Trk::AtaStraightLine driftParameters(position+drift,
						 trkParameters.momentum(),
						 trkParameters.charge(),
						 *surface);
	    rotPrecise		= m_driftCircleOnTrackCreator->correct(*roadData.rio(),driftParameters);

	    // set drift sign with new EDM convention
	    if (driftDistance*rotPrecise->localParameters()[Trk::driftRadius] < 0.)
		driftDistance = -driftDistance;
	}
	else if (roadData.strawStatus() == TRT_RoadData::confirmedWire)
	{
	    status		= drift_time;
	    driftDistance	= 0.0;
	    driftSigma		= m_sigmaWireHit;
	    rotPrecise		= m_driftCircleOnTrackCreator->correct(*roadData.rio(),parameters);
	}
	else if (roadData.strawStatus() == TRT_RoadData::confirmedStraw)
	{
	    status		= drift_wire;
	    driftDistance	= 0.0;
	    driftSigma		= m_sigmaStrawHit;
	}
	
	m_hitList.push_back( new HitOnTrack(roadData.inBarrel(),
					    id,
					    roadData.rio(),
					    rotBroad,
					    rotPrecise,
					    status,
					    roadData.trThreshold()));
	ATH_MSG_VERBOSE( std::setiosflags(std::ios::fixed)
			 << std::setw(13) << i++ << "   "
			 << std::setw(10) << std::setprecision(1)
			 << std::setw(9)  << std::setprecision(4)
			 << surface->center().perp()
			 << std::setw(9)  << std::setprecision(4)
			 << surface->center().phi()
			 << std::setw(10) << std::setprecision(1)
			 << surface->center().z()
			 << std::setw(11)  << std::setprecision(2) << roadData.rPhiRoad()
			 << std::setw(13) << std::setprecision(3) << driftSigma
			 << std::setw(10)  << std::setprecision(2) << driftDistance
			 << std::setw(10)  << std::setprecision(2) << m_histogrammer->residual(roadData)
			 << std::setw(10)  << std::setprecision(2)
			 << m_histogrammer->residual(roadData)/driftSigma );
    }
}
