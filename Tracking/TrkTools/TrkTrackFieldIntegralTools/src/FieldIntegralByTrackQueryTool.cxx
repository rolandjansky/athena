/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FieldIntegralByTrackQueryTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "FieldIntegralByTrackQueryTool.h"
#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkTrack/Track.h"
#include "TrkTrack/TrackStateOnSurface.h"
//#include "TrkParameters/Perigee.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkGeometry/TrackingVolume.h"
#include "GaudiKernel/SystemOfUnits.h"
// ================ Constructor =================================================

Trk::FieldIntegralByTrackQueryTool::FieldIntegralByTrackQueryTool(const std::string& t,
  const std::string& name,
  const IInterface*  poff )
  :
  AthAlgTool(t,name,poff),
  m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc",name),
  m_indetVolume(nullptr),
  m_caloVolume(nullptr)

{
  declareInterface<ITrackMagneticFieldIntegrator>(this);
}

// ================ Destructor =================================================

Trk::FieldIntegralByTrackQueryTool::~FieldIntegralByTrackQueryTool()
  {}


// ================ Initialisation =================================================

StatusCode Trk::FieldIntegralByTrackQueryTool::initialize()
{

  StatusCode sc = AthAlgTool::initialize();
  if (sc.isFailure()) return sc;

  // need to know which TrackingVolume we are in: indet/calo/spectrometer
  if (m_trackingVolumesSvc.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve Svc " << m_trackingVolumesSvc );
    return StatusCode::FAILURE;
  } 
    ATH_MSG_DEBUG( "Retrieved Svc " << m_trackingVolumesSvc );
    m_caloVolume = new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
    m_indetVolume= new Trk::Volume(m_trackingVolumesSvc->volume(ITrackingVolumesSvc::CalorimeterEntryLayer));
  
  ATH_MSG_INFO ("Successfully initialized "<<name());
  return StatusCode::SUCCESS;
}

// ================ Finalisation =================================================

StatusCode Trk::FieldIntegralByTrackQueryTool::finalize()
{
  StatusCode sc = AlgTool::finalize();
  return sc;
}

double Trk::FieldIntegralByTrackQueryTool::indetFieldIntegral(const Trk::Track& track) const {

  return this->fieldIntegral(track, false);

}

double Trk::FieldIntegralByTrackQueryTool::spectrometerFieldIntegral(const Trk::Track& track) const {

  return this->fieldIntegral(track,true);
}

double Trk::FieldIntegralByTrackQueryTool::fieldIntegral(const Trk::Track& track, bool takeMS) const {

  // method is not valid for slimmed tracks
  if (track.info().trackProperties(Trk::TrackInfo::SlimmedTrack)) {
    ATH_MSG_INFO ("configuration mistake? can not calculate field integral for slimmed "<<
		     "tracks -- this tool needs detailed track parameters.");
    return 0.0;
  }

  // field integral null for straight tracks
  if (track.info().trackProperties(Trk::TrackInfo::StraightTrack)) return 0.0;

  // sum Bdl between measurements
  double betweenMeasurements             = 0.;
  bool   haveMeasurements                = false;
  Amg::Vector3D integratedMomentumKick  = Amg::Vector3D(0.,0.,0.);
  const Trk::TrackParameters* parameters      = track.perigeeParameters();
  if (parameters == nullptr) {
    ATH_MSG_INFO ("no perigee on track: convention violation or missing code "<<
		  "in this tool. Will bail out...");
    return 0.0;
  }

  // advance track parameters such that field integration can start in either ID or MS
  if (takeMS && m_indetVolume->inside(parameters->position()) ) {
    parameters = nullptr;

    // there is no defined state for MS entrance? Instead search a bit on the parameter vector...
    for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	   s = track.trackStateOnSurfaces()->begin();
	 s != track.trackStateOnSurfaces()->end();
	 ++s) {
      if ((**s).trackParameters() == nullptr) continue;
      if ( !m_caloVolume->inside((**s).trackParameters()->position()) &&
	   (**s).type(Trk::TrackStateOnSurface::Measurement) &&
	   !(**s).type(Trk::TrackStateOnSurface::Outlier) && 
	   dynamic_cast<const Trk::PseudoMeasurementOnTrack*>((**s).measurementOnTrack())==nullptr) break;
      parameters = (**s).trackParameters(); // gets us the pars before 1st MS meas't
    }
  }
  if (!parameters) {
    ATH_MSG_INFO("no MS perigee on track for MS-field integral: convention violation or "<<
		 "missing code in this tool. Will bail out...");
    return 0.0;
  }
  
    ATH_MSG_VERBOSE ("found starting parameters for MS field calculation at "<<
                     parameters->associatedSurface() );
  

  // loop over TSOS to integrate vector Bdl
  for (DataVector<const Trk::TrackStateOnSurface>::const_iterator
	 s = track.trackStateOnSurfaces()->begin();
       s != track.trackStateOnSurfaces()->end();
       ++s)
    {
      // filter out stated that don't carry parameters or are in the wrong tracking system
      if (! (**s).trackParameters() || (**s).type(Trk::TrackStateOnSurface::Perigee)) continue;
      if (!takeMS && !m_indetVolume->inside((**s).trackParameters()->position()) ) continue;
      if (takeMS && m_caloVolume->inside((**s).trackParameters()->position()) ) continue;
        
      Amg::Vector3D startMomentum       = parameters->momentum();
      parameters                              = (**s).trackParameters();      
      Amg::Vector3D endDirection       = parameters->momentum().unit();
        
      // subtract scattering angle
      if ((**s).materialEffectsOnTrack())
        {
	  const Trk::MaterialEffectsOnTrack* meot =
	    dynamic_cast<const Trk::MaterialEffectsOnTrack*>((**s).materialEffectsOnTrack());
	  if (meot && meot->scatteringAngles())
            {
	      double theta    = endDirection.theta() - meot->scatteringAngles()->deltaTheta();
	      double phi      = endDirection.phi() - meot->scatteringAngles()->deltaPhi();
	      double cosTheta = cos(theta);
	      double sinTheta = sqrt(1. - cosTheta*cosTheta);
	      endDirection    = Amg::Vector3D(sinTheta*cos(phi),
						     sinTheta*sin(phi),
						     cosTheta);
            }
        }
      Amg::Vector3D momentumKick        =  startMomentum.cross(endDirection)/(0.3*Gaudi::Units::GeV);     
      integratedMomentumKick                  += momentumKick;

      // accumulate abs(Bdl) between measurements
      const Trk::MeasurementBase* haveMeasurement = (**s).measurementOnTrack();
      if (! haveMeasurement
	  || (**s).type(Trk::TrackStateOnSurface::Outlier))			continue;

      // skip spectrometer phi measurements
      if (! m_caloVolume->inside((**s).trackParameters()->position()))
      {
	  // Identifier id       = m_helper->getIdentifier(*(**s).measurementOnTrack());
	  // isPreciseHit        = (id.is_valid() && ! m_idHelper->measuresPhi(id)); // avoid MS dependency
	  // detect phi meast, code from GXF
          const Trk::Surface& surface		= haveMeasurement->associatedSurface(); 
	  Amg::Vector3D measuredDirection	= surface.transform().rotation().col(0);
	  double dotprod1			= measuredDirection.dot(Amg::Vector3D(0,0,1));
	  double dotprod2			= measuredDirection.dot(Amg::Vector3D(
										      surface.center().x(),surface.center().y(),0)/surface.center().perp());
	  if (std::abs(dotprod1) < 0.5 && std::abs(dotprod2) < 0.5)		continue; 
      }
      
      if (haveMeasurements)
	{
	  betweenMeasurements      += fabs(integratedMomentumKick.mag());
	  integratedMomentumKick    =  Amg::Vector3D(0.,0.,0.);
	}
      else
	{
	  haveMeasurements          =  true;
	  integratedMomentumKick    =  Amg::Vector3D(0.,0.,0.);
	}
    }

  ATH_MSG_DEBUG( std::setiosflags(std::ios::fixed)
		 << " field integrals for track at eta, phi " << std::setw(6) << std::setprecision(2)
		 << track.perigeeParameters()->momentum().eta()
		 << "," << std::setw(6) << std::setprecision(2)
		 << track.perigeeParameters()->momentum().phi()
		 << " : betweenMeasurements "
		 << std::setw(8) << std::setprecision(3) << betweenMeasurements );
    
  return betweenMeasurements;
}

