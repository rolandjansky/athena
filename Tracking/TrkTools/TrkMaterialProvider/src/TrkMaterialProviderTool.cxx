/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialProviderTool.h"

#include <algorithm>

// Interfaces

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "TrkEventUtils/IdentifierExtractor.h"

#include "muonEvent/CaloEnergy.h"

// Amg::error
#include "EventPrimitives/EventPrimitivesHelpers.h"

// For measured energy loss
#include "CLHEP/Units/SystemOfUnits.h"
#include "MuidEvent/CaloMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
#include "MuidInterfaces/IMuidTrackIsolation.h"

// #define DEBUGON //To activate printout for TSOS lists at various stages
// for line-by-line debugging
#define MYDEBUG() std::cout<<__FILE__<<" "<<__func__<<" "<<__LINE__<<std::endl

void myLocal_resetTrack(Trk::Track& track )
{
  track.reset();
}

// constructor
Trk::TrkMaterialProviderTool::TrkMaterialProviderTool(const std::string& t, const std::string& n, const IInterface* p)
  :	AthAlgTool(t,n,p),
	m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc",n),
        m_scattool("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
	m_caloMeasTool		("Rec::MuidCaloEnergyMeas/MuidCaloEnergyMeas"),
	m_caloParamTool		("Rec::MuidCaloEnergyParam/MuidCaloEnergyParam"),
	m_trackIsolationTool	("Rec::MuidTrackIsolation/MuidTrackIsolation"),
	m_DetID(nullptr),
	m_calorimeterVolume(nullptr),
	m_indetVolume(nullptr),
	m_maxNTracksIso(2),
	m_paramPtCut(15.0*Gaudi::Units::GeV),
	m_useCaloEnergyMeasurement(true),
	m_useMuonCaloEnergyTool(true),
        m_overwriteElossParam(false),
        m_infoExtrapolation(false)
{
  declareInterface<ITrkMaterialProviderTool>(this);

  declareProperty("ApplyTGScaling", m_applyTGScaling = true );
  declareProperty("RpositionTSOS",  m_repositionTSOS = true );
  declareProperty("AggregateTSOS",  m_aggregateTSOS  = true );
  declareProperty("UpdateTSOS",     m_updateTSOS     = true );
  declareProperty("MultipleScatteringTool",m_scattool);
  declareProperty("CaloMeasTool",		m_caloMeasTool);
  declareProperty("CaloParamTool",		m_caloParamTool);
  declareProperty("TrackIsolationTool",	m_trackIsolationTool);
  declareProperty("MaxNTracksIso", m_maxNTracksIso);
  declareProperty("ParamPtCut", m_paramPtCut);
  declareProperty("UseCaloEnergyMeasurement", m_useCaloEnergyMeasurement);
  declareProperty("UseMuonCaloEnergyTool", m_useMuonCaloEnergyTool);
// this is a temporary solution to write Eloss information in the muon to validate the Eloss
// default value should be false
  declareProperty("OverwriteElossParam", m_overwriteElossParam);
  declareProperty("InfoExtrapolation", m_infoExtrapolation);
}

// destructor
Trk::TrkMaterialProviderTool::~TrkMaterialProviderTool()
{}

// Athena standard methods
// initialize
StatusCode
Trk::TrkMaterialProviderTool::initialize()
{
  ATH_CHECK(m_muonExtrapolator.retrieve());
  ATH_CHECK(m_elossupdator.retrieve());
  ATH_CHECK(m_scattool.retrieve());
  if(m_useCaloEnergyMeasurement) {
    ATH_CHECK(m_trackIsolationTool.retrieve());
    if(m_useMuonCaloEnergyTool) {
      ATH_CHECK(m_muonCaloEnergyTool.retrieve());
      m_caloMeasTool.disable();
      m_caloParamTool.disable();
    }else{
      ATH_CHECK(m_caloMeasTool.retrieve());
      ATH_CHECK(m_caloParamTool.retrieve());
      m_muonCaloEnergyTool.disable();
    }
  }
  else{
    m_caloMeasTool.disable();
    m_caloParamTool.disable();
    m_muonCaloEnergyTool.disable();
    m_trackIsolationTool.disable();
  }

  /// handle to the magnetic field cache
  ATH_CHECK( m_fieldCacheCondObjInputKey.initialize() );

  // need an Atlas id-helper to identify sub-detectors, take the one from detStore
  if (detStore()->retrieve(m_DetID, "AtlasID").isFailure()) {
    ATH_MSG_ERROR ("Could not get AtlasDetectorID helper" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK(m_trackingVolumesSvc.retrieve());

  m_calorimeterVolume = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::MuonSpectrometerEntryLayer));
  if(!m_calorimeterVolume) {
    ATH_MSG_ERROR("Unable to retrieve MuonSpectrometerEntryLayer volume");
    return StatusCode::FAILURE;
  }

  m_indetVolume = new Trk::Volume(m_trackingVolumesSvc->volume(Trk::ITrackingVolumesSvc::CalorimeterEntryLayer));
  if(!m_indetVolume) {
    ATH_MSG_ERROR("Unable to retrieve CalorimeterEntryLayer volume");
    return StatusCode::FAILURE;
  }

#ifdef LEGACY_TRKGEOM
  
  if (m_trackingGeometryReadKey.key().empty()) {
     ATH_CHECK(m_trackingGeometrySvc.retrieve());
  } else
#endif
  ATH_CHECK( m_trackingGeometryReadKey.initialize() );

  return StatusCode::SUCCESS;
}

// finalize
StatusCode
Trk::TrkMaterialProviderTool::finalize()
{
  ATH_MSG_DEBUG( name() << " finalize() successful" );
  return StatusCode::SUCCESS;
}


// Update Calorimeter TSOS using TG
void Trk::TrkMaterialProviderTool::updateCaloTSOS(Trk::Track& track, const Trk::TrackParameters* startParameters) const
{
  ATH_MSG_VERBOSE("updateCaloTSOS(Trk::Track& track, const Trk::TrackParameters* startParameters)");
 
  // back extrapolate to perigee, get pAtCaloEntry from list of TSOSs
  // and update/add calo+ID material to mstrack to be refitted.
  const Trk::TrackStates* inputTSOS_orig = track.trackStateOnSurfaces();

  auto inputTSOS = std::make_unique<Trk::TrackStates>();
  for (const Trk::TrackStateOnSurface* tsos : *inputTSOS_orig) {
    inputTSOS->push_back (tsos->clone());
  }

  // Iterators
  Trk::TrackStates::iterator lastIDwP  = inputTSOS->end();
  Trk::TrackStates::iterator firstCALO = inputTSOS->end();
  Trk::TrackStates::iterator firstMS   = inputTSOS->end();
  Trk::TrackStates::iterator firstMSwP = inputTSOS->end();

  // find first MS TSOS (handling the case of TSOS w/o TP) and the last ID (or perigee)
  Trk::TrackStates::iterator it    = inputTSOS->begin();
  Trk::TrackStates::iterator itEnd = inputTSOS->end();
  for(; it!=itEnd; ++it) {

#ifdef DEBUGON
    printTSOS(*it, "TSOS ON TRACK");
#endif

    if(this->getVolumeByGeo(*it)==1 && (*it)->trackParameters())
      lastIDwP = it;
    if(firstCALO==inputTSOS->end() && this->getVolumeByGeo(*it)==2 && firstMS==itEnd)
      firstCALO = it;
    else if(this->getVolumeByGeo(*it)==3 && firstCALO!=inputTSOS->end()) {//&& !(*it)->type(Trk::TrackStateOnSurface::Perigee)) {
      if(firstMS==itEnd)
	firstMS = it;
      if((*it)->trackParameters() && (*it)->trackParameters()->covariance()) {
	firstMSwP = it;
	break;
      }
    }
  }

  // if we have last ID TSOS from the list on track (i.e. combined muon) then use this as start parameters
  if(lastIDwP != inputTSOS->end()) {
#ifdef DEBUGON
    printTSOS(*lastIDwP, "LAST IDwP");
#endif
    startParameters = (*lastIDwP)->trackParameters();
  }

  if(lastIDwP == inputTSOS->end() && !startParameters) {
    ATH_MSG_WARNING("Unable to find starting parameters for extrapolation");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    return;
  }
  if(firstCALO == inputTSOS->end()) {
    ATH_MSG_DEBUG("Track without CALO TSOS!");
    firstCALO = firstMS;
  }
  if(firstMS == inputTSOS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  //bool removeOoC = false;
  Trk::TrackStates::const_iterator firstMSnotPerigee = firstMS;
  if( (*firstMS)->type(Trk::TrackStateOnSurface::Perigee) && (firstMS+1)!=inputTSOS->end()) {
    firstMSnotPerigee=firstMS+1;
    //removeOoC = true;
  }

#ifdef DEBUGON
  printTSOS(*firstCALO, "FIRST CALO");
  printTSOS(*firstMSnotPerigee, "FIRST MS");
#endif
  double Eloss = 0.;
  double X0ScaleMS = 0.;
  double ElossScaleMS = 0.;
  // get calorimeter TSOS from TG extrapolating from last ID to MS
  Trk::TrackStates* caloTSOS = this->getCaloTSOS (*startParameters,
									    track,
									    (*firstMSnotPerigee)->surface(),
									    Trk::alongMomentum,
									    Trk::muon,
                                                                            Eloss, X0ScaleMS, ElossScaleMS,
									    (firstMSwP == inputTSOS->end()) ? nullptr : (*firstMSwP)->trackParameters(),
									    false,
									    true);

  if(!caloTSOS || caloTSOS->size()!=3) {
    ATH_MSG_WARNING("Unable to retrieve Calorimeter TSOS from extrapolateM (null or <3)");
    if(caloTSOS) deleteTSOS(caloTSOS);
    return;
  }

#ifdef DEBUGON
  ATH_MSG_VERBOSE("OLD TSOS multiplicity : " << inputTSOS->size());
  for(auto m : *inputTSOS) this->printTSOS(m, "OLD TSOS");
#endif

  // apply X0 and Eloss scale to MuonSpectrometer
  this->updateVectorMS(inputTSOS.get(),firstMS,X0ScaleMS,ElossScaleMS);
  // update the original vector
  Trk::TrkMaterialProviderTool::updateVector(inputTSOS.get(), firstCALO, firstMS, caloTSOS);
  track.setTrackStateOnSurfaces (std::move (*inputTSOS));
  myLocal_resetTrack(track);
}


// Update Calorimeter TSOS using TG for the combined fit
void Trk::TrkMaterialProviderTool::updateCaloTSOS(const Trk::Track& idTrack, Trk::Track& extrapolatedTrack) const
{
  ATH_MSG_VERBOSE("updateCaloTSOS(Trk::Track& idTrack, Trk::Track& extrapolatedTrack)");

  const Trk::TrackStates* inputTSOS_ID = idTrack.trackStateOnSurfaces();
  const Trk::TrackStates* inputTSOS_MS_orig = extrapolatedTrack.trackStateOnSurfaces();

  auto inputTSOS_MS = std::make_unique<Trk::TrackStates>();
  for (const Trk::TrackStateOnSurface* tsos : *inputTSOS_MS_orig) {
    inputTSOS_MS->push_back (tsos->clone());
  }


  // find last ID TSOS
  Trk::TrackStates::const_iterator lastIDwP  = inputTSOS_ID->end();
  Trk::TrackStates::const_iterator itID        = inputTSOS_ID->end()-1;
  Trk::TrackStates::const_iterator itFront   = inputTSOS_ID->begin();
  while(*itID) {
    if(this->getVolumeByGeo(*itID)==1 && (*itID)->trackParameters()) {
      lastIDwP = itID;
      break;
    }
    if(itID==itFront) break;
    --itID;
  }

  // find first MS TSOS
  Trk::TrackStates::iterator firstCALO = inputTSOS_MS->end();
  Trk::TrackStates::iterator firstMS   = inputTSOS_MS->end();
  Trk::TrackStates::iterator firstMSwP = inputTSOS_MS->end();
  Trk::TrackStates::iterator itEnd = inputTSOS_MS->end();
  Trk::TrackStates::iterator it = inputTSOS_MS->begin();
  for(; it!=itEnd; ++it) {

#ifdef DEBUGON
    printTSOS(*it, "TSOS ON TRACK");
#endif

    if(firstCALO==inputTSOS_MS->end() && this->getVolumeByGeo(*it)==2)
      firstCALO = it;
    if(this->getVolumeByGeo(*it)==3) {// && !(*it)->type(Trk::TrackStateOnSurface::Perigee)) {
      if(firstMS==itEnd)
	firstMS = it;
      if((*it)->trackParameters() && (*it)->trackParameters()->covariance()) {
	firstMSwP = it;
	break;
      }
    }
  }

  if(firstCALO == inputTSOS_MS->end()) {
    ATH_MSG_DEBUG("Track without CALO TSOS!");
    firstCALO = firstMS;
  }
  if(lastIDwP == inputTSOS_ID->end()) {
    ATH_MSG_WARNING("Unable to find last ID TSOS with Track Parameters");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS" );
    return;
  }
  if(firstMS == inputTSOS_MS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS!");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  //bool removeOoC = false;
  Trk::TrackStates::const_iterator firstMSnotPerigee = firstMS;
  if( (*firstMS)->type(Trk::TrackStateOnSurface::Perigee) && (firstMS+1)!=inputTSOS_MS->end()) {
    firstMSnotPerigee=firstMS+1;
    //removeOoC = true;
  }

#ifdef DEBUGON
  printTSOS(*lastIDwP,  "LAST IDwP");
  printTSOS(*firstCALO, "FIRST CALO");
  printTSOS(*firstMSnotPerigee,   "FIRST MS");
  if(firstMSwP != inputTSOS_MS->end())
    printTSOS(*firstMSwP, "FIRST MSwP");
  else
    ATH_MSG_WARNING("Unable to find first MS TSOS with Track Parameters");
#endif
  double Eloss = 0.;
  double X0ScaleMS = 0.;
  double ElossScaleMS = 0.;
  // get calorimeter TSOS from TG
  Trk::TrackStates* caloTSOS = this->getCaloTSOS (*(*lastIDwP)->trackParameters(),
                                                  extrapolatedTrack,
                                                  (*firstMSnotPerigee)->surface(),
                                                  Trk::alongMomentum,
                                                  Trk::muon,
                                                  Eloss, X0ScaleMS, ElossScaleMS,
                                                  (firstMSwP == inputTSOS_MS->end()) ? nullptr : (*firstMSwP)->trackParameters(),
                                                  false,
                                                  true);


  if(!caloTSOS || caloTSOS->size()!=3) {
    ATH_MSG_WARNING("Unable to retrieve Calorimeter TSOS from extrapolateM (null or <3)");
    if(caloTSOS) deleteTSOS(caloTSOS);
    return;
  }

#ifdef DEBUGON
  ATH_MSG_VERBOSE("ID TSOS multiplicity : " << inputTSOS_ID->size());
  for(auto m : *inputTSOS_ID) printTSOS(m, "ID TSOS");
  ATH_MSG_VERBOSE("OLD-MS TSOS multiplicity : " << inputTSOS_MS->size());
  for(auto m : *inputTSOS_MS) printTSOS(m, "OLD-MS TSOS");
#endif

  // apply X0 and Eloss scale to MuonSpectrometer
  this->updateVectorMS(inputTSOS_MS.get(),firstMS,X0ScaleMS,ElossScaleMS);
  // update the original vector
  Trk::TrkMaterialProviderTool::updateVector(inputTSOS_MS.get(), firstCALO, firstMS, caloTSOS);

  extrapolatedTrack.setTrackStateOnSurfaces (std::move (*inputTSOS_MS));
  myLocal_resetTrack(extrapolatedTrack);
}



// Get Calorimeter MEOT using TG for the combined fit
void Trk::TrkMaterialProviderTool::getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack,
					  std::vector<MaterialEffectsOnTrack>& calomeots) const
{
  ATH_MSG_VERBOSE("getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack, std::vector<MaterialEffectsOnTrack>& calomeots)");

  const Trk::TrackStates* inputTSOS_ID = idTrack.trackStateOnSurfaces();
  const Trk::TrackStates* inputTSOS_MS = msTrack.trackStateOnSurfaces();

#ifdef DEBUGON
  for(auto m : *inputTSOS_ID) printTSOS(m, "TSOS ID TRACK");
  for(auto m : *inputTSOS_MS) printTSOS(m, "TSOS MS TRACK");
#endif

  // find last ID TSOS
  Trk::TrackStates::const_reverse_iterator lastIDwP  = inputTSOS_ID->rbegin();
  for (auto it = inputTSOS_ID->rbegin(); it != inputTSOS_ID->rend(); ++it) {
    if(this->getVolumeByGeo(*it)==1 && (*it)->trackParameters()) {
      lastIDwP = it;
      break;
    }
  }

  if(lastIDwP == inputTSOS_ID->rend()) {
    ATH_MSG_WARNING("Unable to find last ID TSOS with Track Parameters");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    return;
  }

  // find first MS TSOS
  Trk::TrackStates::const_iterator firstMS   = inputTSOS_MS->end();
  Trk::TrackStates::const_iterator firstMSwP = inputTSOS_MS->end();
  Trk::TrackStates::const_iterator itEnd = inputTSOS_MS->end();
  for(auto it = inputTSOS_MS->begin(); it!=itEnd ; ++it) {
    if(this->getVolumeByGeo(*it)==3) {// && !(*it)->type(Trk::TrackStateOnSurface::Perigee)) {
      if(firstMS==itEnd)
	      firstMS = it;
      if((*it)->trackParameters() && (*it)->trackParameters()->covariance()) {
	      firstMSwP = it;
      	break;
      }
    }
  }

  if(firstMS == inputTSOS_MS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS");
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  Trk::TrackStates::const_iterator firstMSnotPerigee = firstMS;
  if( (*firstMS)->type(Trk::TrackStateOnSurface::Perigee) && (firstMS+1)!=inputTSOS_MS->end()) {
    firstMSnotPerigee=firstMS+1;
  }

#ifdef DEBUGON
  printTSOS(*lastIDwP,  "LAST IDwP");
  printTSOS(*firstMSnotPerigee,   "FIRST MS");
  if(firstMSwP != inputTSOS_MS->end())
    printTSOS(*firstMSwP, "FIRST MSwP");
  else
    ATH_MSG_WARNING("Unable to find first MS TSOS with Track Parameters!");
#endif

  MagField::AtlasFieldCache    fieldCache;
  // Get field cache object
  const EventContext& ctx = Gaudi::Hive::currentContext();
  SG::ReadCondHandle<AtlasFieldCacheCondObj> readHandle{m_fieldCacheCondObjInputKey, ctx};
  const AtlasFieldCacheCondObj* fieldCondObj{*readHandle};

  if (fieldCondObj == nullptr) {
    ATH_MSG_ERROR("Failed to retrieve AtlasFieldCacheCondObj with key " << m_fieldCacheCondObjInputKey.key());
    return;
  }
  fieldCondObj->getInitializedCache (fieldCache);

  double Eloss = 0.;
  double X0ScaleMS = 0.;
  double ElossScaleMS = 0.;
  // get calorimeter TSOS from TG
  Trk::TrackStates* caloTSOS = this->getCaloTSOS (*(*lastIDwP)->trackParameters(),
									    // idTrack,
									    fieldCache.toroidOn() ? msTrack : idTrack,
									    (*firstMSnotPerigee)->surface(),
									    Trk::alongMomentum,
									    Trk::muon,
                      Eloss, X0ScaleMS, ElossScaleMS,
									    (firstMSwP == inputTSOS_MS->end()) ? nullptr : (*firstMSwP)->trackParameters(),
									    false,
									    true);

    if (!caloTSOS || caloTSOS->size() != 3)
    {
      double idqOverP = std::abs(idTrack.perigeeParameters()->parameters()[Trk::qOverP]);
      double msqOverP = msTrack.perigeeParameters() ? msTrack.perigeeParameters()->parameters()[Trk::qOverP] : (*firstMS)->trackParameters()->parameters()[Trk::qOverP] ;

      if ((!fieldCache.toroidOn() && idqOverP * 4000. < 1) || (fieldCache.toroidOn() && msqOverP != 1 / 100000. && msqOverP != 0))
      {
        // Warnings only for high momentum ID tracks and MS tracks that have measured curvature (Straight track has pq= 100000)
        if (!fieldCache.toroidOn())
          ATH_MSG_WARNING(" Toroid off q*momentum of ID track " << 1. / idqOverP);
        if (fieldCache.toroidOn())
          ATH_MSG_WARNING(" Toroid on q*momentum of MS track " << 1. / msqOverP);
        ATH_MSG_WARNING("Unable to retrieve Calorimeter TSOS from extrapolateM+aggregation (null or !=3)");
        if (!caloTSOS) {
          ATH_MSG_WARNING(" Zero Calorimeter TSOS from extrapolateM+aggregation");
        } else {
          ATH_MSG_WARNING(" nr Calorimeter TSOS from extrapolateM+aggregation not equal to 3 " << caloTSOS->size());
        }
      }

      if (caloTSOS) deleteTSOS(caloTSOS);

      return;
    }

  for (unsigned int i=0;i<caloTSOS->size(); i++){
    const Trk::MaterialEffectsOnTrack *meot=dynamic_cast<const Trk::MaterialEffectsOnTrack *>((*caloTSOS)[i]->materialEffectsOnTrack());
    if (!meot) {
      throw std::logic_error("TrackStateOnSurface without material effects on track." );
    }
    double sintheta=std::sin((*caloTSOS)[i]->trackParameters()->parameters()[Trk::theta]);
    double qoverp=(*caloTSOS)[i]->trackParameters()->parameters()[Trk::qOverP];
    const CaloEnergy *eloss=nullptr;
    if (meot) eloss=dynamic_cast<const CaloEnergy *>(meot->energyLoss());

    Trk::EnergyLoss *neweloss=nullptr;
    std::optional<Trk::ScatteringAngles> newsa= std::nullopt;
    if (eloss) neweloss = new CaloEnergy(*eloss);
    else{
      Trk::MaterialProperties matprop(meot->thicknessInX0(),1.,0.,0.,0.,0.);
      double sigmascat = std::abs(qoverp)>0.0 ? std::sqrt(m_scattool->sigmaSquare(matprop,std::abs(1./qoverp),1.,Trk::muon)) : 0.0;
      newsa=Trk::ScatteringAngles(0,0,sigmascat/sintheta,sigmascat);
    }
    Trk::MaterialEffectsOnTrack newmeot(
      meot->thicknessInX0(),
      newsa,
      neweloss,
      (*caloTSOS)[i]->trackParameters()->associatedSurface());
    calomeots.push_back(newmeot);
    delete (*caloTSOS)[i];
  }
  delete caloTSOS;
}



// Get Calorimeter TSOS from TG (extrapolateM)
std::vector<const Trk::TrackStateOnSurface*>*
Trk::TrkMaterialProviderTool::getCaloTSOS (const Trk::TrackParameters&	parm, const Trk::Track& muonTrack,
					   const Trk::TrackParameters* parms) const
{
  std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = new std::vector<const Trk::TrackStateOnSurface*>();
  const Trk::TrackingVolume* targetVolume;
  Trk::PropDirection dir;
  const EventContext& ctx = Gaudi::Hive::currentContext();

  const Trk::TrackingGeometry* trackingGeometry =  retrieveTrackingGeometry( ctx );
  if(!trackingGeometry) {
    ATH_MSG_WARNING("Unable to retrieve tracking geometry");
    return caloTSOS;
  }

  // check if parm is expressed at ID or MS and set extrapolation direction and target volume
  // ID
  if( m_indetVolume->inside(parm.position()) ) {
    dir = Trk::alongMomentum;
    targetVolume = trackingGeometry->trackingVolume("MuonSpectrometerEntrance");
    if(!targetVolume) {
      ATH_MSG_WARNING("Unable to get target volume for calo material collection!");
      return caloTSOS;
    }
    ATH_MSG_VERBOSE("TP inside ID -> extrapolating TP "<<parm<<" to MS entry volume "<<*targetVolume);
  }
  // MS
  else if( !m_calorimeterVolume->inside(parm.position()) ) {
    dir = Trk::oppositeMomentum;
    targetVolume = trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
    if(!targetVolume) {
      ATH_MSG_WARNING("Unable to get target volume for calo material collection!");
      return caloTSOS;
    }
    ATH_MSG_VERBOSE("TP inside MS -> extrapolating TP "<<parm<<" to ID exit volume "<<*targetVolume);
  }
  // CALO, it happens when called leadingTSOS from MS, backward extrapolation ok
  else{
    dir = Trk::oppositeMomentum;
    targetVolume = trackingGeometry->trackingVolume("InDet::Containers::InnerDetector");
    if(!targetVolume) {
      ATH_MSG_WARNING("Unable to get target volume for calo material collection!");
      return caloTSOS;
    }
    ATH_MSG_DEBUG("TP inside CALO or between CALO and MS -> assuming ID as target : "<<*targetVolume);
  }

  // When doing backward extrapolation the starting TP are defined at MS entrance
  // so we use them to get the MS momentum error needed later on when doing TSOS aggregation
  if(!parms) {
    if(dir == Trk::oppositeMomentum && parm.covariance())
      parms = &parm;
    else{
      Trk::TrackStates::const_iterator it = muonTrack.trackStateOnSurfaces()->begin();
      Trk::TrackStates::const_iterator itEnd = muonTrack.trackStateOnSurfaces()->end();
      for(; it!=itEnd; ++it) {
	if(this->getVolumeByGeo(*it)==3) //&& !(*it)->type(Trk::TrackStateOnSurface::Perigee)) {
	  if((*it)->trackParameters() && (*it)->trackParameters()->covariance()) {
	    parms = (*it)->trackParameters();
	    break;
	  }
      }
    }
  }

  if(dir == Trk::oppositeMomentum) {
//
// go to Beam line
//
    Amg::Vector3D  globPos(0.,0.,0.);
    PerigeeSurface surface(globPos);
    double Eloss = 0.;
    double X0ScaleMS = 0.;
    double ElossScaleMS = 0.;
    // Collect calorimeter TSOS
    Trk::TrackStates* caloTSOSdv = this->getCaloTSOS (parm,
										muonTrack,
										surface,
										dir,
										Trk::muon,
                                                                                Eloss, X0ScaleMS, ElossScaleMS,
										parms,
										true,
										false);  // remove only MS TSOS and keep ID+CALO
    if(m_infoExtrapolation) {
      if(parms) ATH_MSG_INFO(" go to Beam Line parms position radius " << parms->position().perp() << " z " << parms->position().z());
      ATH_MSG_INFO(" go to Beam Line destination surface position radius " << surface.center().perp() << " z " << surface.center().z());
    }
    if(caloTSOSdv) {
      for(unsigned int i=0; i<caloTSOSdv->size(); ++i)
        caloTSOS->push_back(caloTSOSdv->get(i));
      delete caloTSOSdv;
    }
    return caloTSOS;

  }

    // get boundary surfaces of the target volume
    auto boundaryIntersections = targetVolume->boundarySurfacesOrdered<Trk::TrackParameters>(parm,dir,false);

  // loop over surfaces
    double Eloss_previous = 0.;
    for (auto& boundaryCandidate : boundaryIntersections){

      // get the surface object at the boundary
      const Trk::BoundarySurface<Trk::TrackingVolume>* surfaceTV = boundaryCandidate.object;
      if(!surfaceTV) continue;

      // get the Trk::Surface
      const Trk::Surface& surface = surfaceTV->surfaceRepresentation();
      double Eloss = 0.;
      double X0ScaleMS = 0.;
      double ElossScaleMS = 0.;
    // Collect calorimeter TSOS
      Trk::TrackStates* caloTSOSdv = this->getCaloTSOS (parm,
										muonTrack,
										surface,
										dir,
										Trk::muon,
                    Eloss, X0ScaleMS, ElossScaleMS,
										parms,
										true,
										true);
    // hack to convert DataVector to stl vector and delete the DataVector ...
      if(caloTSOSdv&&Eloss>Eloss_previous) {
//
//      keep the TSOS vector with the largest Eloss
//      only one of the surfaces gives the rights intersection
//      partial crossing with a plane or a cylinder give less Eloss
//
        if(m_infoExtrapolation) {
          ATH_MSG_INFO(" getCaloTSOS: Previous solution had Eloss " << Eloss_previous << " latest " << Eloss);
          if(parms) ATH_MSG_INFO(" parms position radius " << parms->position().perp() << " z " << parms->position().z());
          /*if(&surface)*/ ATH_MSG_INFO(" destination surface position radius " << surface.center().perp() << " z " << surface.center().z());
        } else {
          ATH_MSG_DEBUG(" getCaloTSOS: Previous solution had Eloss " << Eloss_previous << " latest " << Eloss);
          if(parms) ATH_MSG_DEBUG(" parms position radius " << parms->position().perp() << " z " << parms->position().z());
          /*if(&surface)*/ ATH_MSG_DEBUG(" destination surface position radius " << surface.center().perp() << " z " << surface.center().z());
        }
        for(unsigned int i=0; i<caloTSOSdv->size(); ++i)
  	  caloTSOS->push_back(caloTSOSdv->get(i));
        delete caloTSOSdv;
        return caloTSOS;
      }
    }


  return caloTSOS;
}



// Get Calorimeter TSOS from TG (extrapolateM)
Trk::TrackStates*
Trk::TrkMaterialProviderTool::getCaloTSOS(const Trk::TrackParameters& parm,
                                          const Trk::Track& muonTrack,
                                          const Trk::Surface& surf,
                                          Trk::PropDirection dir,
                                          Trk::ParticleHypothesis mateffects,
                                          double& Eloss,
                                          double& X0ScaleMS,
                                          double& ElossScaleMS,
                                          const Trk::TrackParameters* parms,
                                          bool boundaryCheck,
                                          bool removeOoC) const
{
  const EventContext& ctx = Gaudi::Hive::currentContext();
  bool fremoveMS = false;
  if(!removeOoC) fremoveMS = true;

  if(m_infoExtrapolation) {
    ATH_MSG_INFO("Retrieving Calorimeter TSOS from extrapolateM (dir=" << dir << ") with starting parameters : "
		<< parm << " to surface "<<surf);
    if(parms) ATH_MSG_INFO("Parameters in MS provided : "<< *parms);
  } else {
    ATH_MSG_DEBUG("Retrieving Calorimeter TSOS from extrapolateM (dir=" << dir << ") with starting parameters : "
		<< parm << " to surface "<<surf);
    if(parms) ATH_MSG_DEBUG("Parameters in MS provided : "<< *parms);
  }

  double pOri  = parm.momentum().mag();

  // Get TSOS from extrapolateM (from TG)
  std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = m_muonExtrapolator->extrapolateM(ctx,
                                                                                            parm, 
                                                                                            surf, dir,
                                                                                            boundaryCheck, 
                                                                                            mateffects);

  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM, no-removal");

#ifdef DEBUGON
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM, no-removal");
  for(auto m : *caloTSOS) this->printTSOS(m, "BEFORE-REMOVAL CALO TSOS");
#endif


  Eloss = 0.;
  double ElossID = 0.;
  double ElossCalo = 0.;
  double ElossMS = 0.;
  for(const auto *m : *caloTSOS) {
    if(m->materialEffectsOnTrack()) {
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
      if(meot) {
        const Trk::EnergyLoss* energyLoss = meot->energyLoss();
        if (energyLoss) {
        if(m_infoExtrapolation) {
          ATH_MSG_INFO(" volume " << this->getVolumeByGeo(m) << " Eloss from extrapolateM TG " << energyLoss->deltaE());
        } else {
          ATH_MSG_DEBUG(" volume " << this->getVolumeByGeo(m) << " Eloss from extrapolateM TG " << energyLoss->deltaE());
        }
          Eloss += std::abs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==1) ElossID   += std::abs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==2) ElossCalo += std::abs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==3) ElossMS   += std::abs(energyLoss->deltaE());
        }
      }
    }
  }

  if(m_infoExtrapolation) {
    ATH_MSG_INFO("Total Eloss on TSOS from extrapolateM " << Eloss << " ElossID " << ElossID << " ElossMS " << ElossMS <<" Elosscalo " << ElossCalo);
    if(fremoveMS) ATH_MSG_INFO(" ID Eloss will be added to Calo Eloss " << ElossID+ElossCalo);
  } else {
    ATH_MSG_DEBUG("Total Eloss on TSOS from extrapolateM " << Eloss << " ElossID " << ElossID << " ElossMS " << ElossMS <<" Elosscalo " << ElossCalo);
    if(fremoveMS) ATH_MSG_DEBUG(" ID Eloss will be added to Calo Eloss " << ElossID+ElossCalo);
  }

  Eloss = ElossCalo;

// remove ID and MS TSOSs
  if(removeOoC && !caloTSOS->empty()) removeOutOfCalo(caloTSOS);
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM");
// remove MS TSOSs
  if(fremoveMS && !caloTSOS->empty()) removeMS(caloTSOS);
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM");

#ifdef DEBUGON
  for(auto m : *caloTSOS) this->printTSOS(m, "ORIGINAL CALO TSOS");
#endif


  Trk::TrackStates*  finalCaloTSOS = nullptr;
  if(caloTSOS->empty() || Eloss<=0) {
    if(dir==Trk::alongMomentum&&pOri>4000.) {
        ATH_MSG_WARNING("Unable to retrieve Calorimeter TSOS from extrapolateM caloTSOS->size() "<< caloTSOS->size() << " Eloss " << Eloss );
        ATH_MSG_WARNING(" momentum of track " << pOri);
        ATH_MSG_WARNING(" momentum extrapolated of track " << parm.momentum().mag() << " radius " << parm.position().perp() << " z " << parm.position().z() );
        ATH_MSG_WARNING(" surface radius " << surf.center().perp() << " z " << surf.center().z() );
        if(parms) ATH_MSG_WARNING(" momentum of MS track " << parms->momentum().mag());
    } // else track did not have enough momentum
    deleteTSOS(caloTSOS);
    return finalCaloTSOS;
  }

  // Apply TG scaling to G4
  double X0ScaleCALO=1.0;
  double ElossScaleCALO=1.0;
  X0ScaleMS=1.0;
  ElossScaleMS=1.0;
  const Trk::TrackStateOnSurface* tsosAtCaloExit = this->getCaloExitTSOS(caloTSOS, dir);
  if(m_applyTGScaling) {
    if(!tsosAtCaloExit) {
      ATH_MSG_WARNING( name() << " Unable to find Calorimeter Exit TSOS with TrackParameters! No TG Scaling applied!" );
    }else{
      m_elossupdator->getX0ElossScales(1,
				       -log(tan(tsosAtCaloExit->trackParameters()->position().theta()/2)),
				       tsosAtCaloExit->trackParameters()->position().phi(),
				       X0ScaleCALO, ElossScaleCALO);
      m_elossupdator->getX0ElossScales(0,
				       -log(tan(tsosAtCaloExit->trackParameters()->position().theta()/2)),
				       tsosAtCaloExit->trackParameters()->position().phi(),
				       X0ScaleMS, ElossScaleMS);
    }
  }
  ATH_MSG_DEBUG("Eloss/X0 scaling corrections set to : " << ElossScaleCALO << " " << X0ScaleCALO);

  // Get momentum at calorimeter entrance
  // Note that for SA fit i'm taking the pAtCaloEntry from the perigee parameters
  double pAtCaloEntry = 0;
  if(dir == Trk::alongMomentum) {
    double OneOverP = std::abs(parm.parameters()[Trk::qOverP]);
    if (OneOverP > 0.0)
      pAtCaloEntry = 1./OneOverP;
    else
      pAtCaloEntry = parm.momentum().norm();
  }else{
    const Trk::TrackStateOnSurface* tsosAtCaloEntry = this->getCaloEntryTSOS(caloTSOS, dir);
    if(!tsosAtCaloEntry) {
      ATH_MSG_WARNING( name() << " Unable to find Calorimeter Entry TSOS with TrackParameters! Momentum at Calo Entry not available!" );
    }else{
      double OneOverP = std::abs(tsosAtCaloEntry->trackParameters()->parameters()[Trk::qOverP]);
      if (OneOverP > 0.0)
	pAtCaloEntry = 1./OneOverP;
      else
	pAtCaloEntry = tsosAtCaloEntry->trackParameters()->momentum().norm();
    }
  }

  ATH_MSG_DEBUG("Momentum at Calo Entry : " << pAtCaloEntry );

  // If parameters at MS are not provided then try to get them out of extrapolated Calo TSOSs (likely w/o covariance!)
  if(!parms) {
    if(!tsosAtCaloExit) {
      ATH_MSG_DEBUG("Unable to find Calorimeter Exit TSOS with TrackParameters! Momentum at MS not available!" );
    }else{
      parms = tsosAtCaloExit->trackParameters();
      ATH_MSG_DEBUG("MS track parameters taken from calorimeter TSOS");
    }
  }

  // Get momentum error in muon spectrometer
  double pAtMuonEntryError = 0.0;
  if(parms) {
    if( std::abs(parms->parameters()[Trk::qOverP]) > 0.0 ) {
      double pAtMuonEntry = std::abs(1./parms->parameters()[Trk::qOverP]);
      if (!parms->covariance() ||
          !Amg::saneCovarianceDiagonal(*parms->covariance())) {
        ATH_MSG_DEBUG(
          "MS track parameters without covariance, using 10% relative error!");
        pAtMuonEntryError = pAtMuonEntry*0.1;
      } else {
        double qOverpAtMuonEntryError = Amg::error(*parms->covariance(),Trk::qOverP);
        pAtMuonEntryError = pAtMuonEntry*pAtMuonEntry*qOverpAtMuonEntryError;
      }
    }
  }

  ATH_MSG_DEBUG("Momentum Error at MS Entry : " << pAtMuonEntryError);

  // Get calorimeter measured energy for pT>m_paramPtCut (15 GeV)
  double measCaloEnergy = 0.0;
  double measCaloEnergyError = 0.0;
  double fsrCaloEnergy = 0.0;
  double eta = -log(tan(parm.parameters()[Trk::theta]/2));
  double phi = parm.parameters()[Trk::phi0];
  double totalEloss=0.0;
  double meanElossIoni=0.0;
  double sigmaElossIoni=0.0;
  if(m_useCaloEnergyMeasurement) {

    // Get Mop and Mean (ionization) energy loss from TG after scaling
    double e_exp=0.0;
    this->getMopAndIoniEnergyLoss(caloTSOS,
				  pAtCaloEntry,
				  ElossScaleCALO,
				  totalEloss,
				  meanElossIoni,
				  sigmaElossIoni);

    double E_em_meas   = 0.;
    double E_em_exp    = 0.;
    double E_tile_meas = 0.;
    double E_tile_exp  = 0.;
    double E_HEC_meas  = 0.;
    double E_HEC_exp   = 0.;
    double E_dead_exp  = 0.;

    // Get measured energy in calorimeter (run2 tool)
    if(m_useMuonCaloEnergyTool) {
//
//  sigmaElossIoni should be scaled by 0.45 to go to Landau this is later done in updateEloss
//
      if(muonTrack.trackParameters() && !muonTrack.trackParameters()->empty())
	m_muonCaloEnergyTool->calculateMuonEnergies( &muonTrack,
						     totalEloss, meanElossIoni, 0.45*sigmaElossIoni,
						     measCaloEnergy, measCaloEnergyError, fsrCaloEnergy, e_exp,
						     E_em_meas,E_em_exp,E_tile_meas,E_tile_exp,E_HEC_meas,E_HEC_exp,E_dead_exp);


      if(m_infoExtrapolation) {
        ATH_MSG_INFO(" eta " << eta << " Energy measurement from calorimeter: inputs totalEloss, meanElossIoni, sigmaElossIoni "
	<< totalEloss << " " << meanElossIoni << " " << sigmaElossIoni << " e_exp Ioni from TG " << e_exp << " e_exp original " << e_exp*totalEloss/(meanElossIoni+0.001));
      } else {
        ATH_MSG_DEBUG(" eta " << eta << " Energy measurement from calorimeter: inputs totalEloss, meanElossIoni, sigmaElossIoni "
	<< totalEloss << " " << meanElossIoni << " " << sigmaElossIoni << " e_exp Ioni from TG " << e_exp << " e_exp original " << e_exp*totalEloss/(meanElossIoni+0.001));
      }

    }
    // (run1 tool) used for debugging purposes
    else{
      measCaloEnergy = this->getCaloMeasuredEnergy(eta,phi,
						   totalEloss,
						   meanElossIoni,
						   fsrCaloEnergy);
      measCaloEnergyError = 0.50 * sqrt(measCaloEnergy/CLHEP::GeV) * CLHEP::GeV;
    }

    ATH_MSG_DEBUG("Final measured energy in calorimeter : " << measCaloEnergy << " +- " << measCaloEnergyError << " MeV"
		  << " momentum fraction " << measCaloEnergy/pAtCaloEntry);
  }

  // Check if we can use the measured eloss in the fit
  bool useMeasuredEnergy = m_useCaloEnergyMeasurement;
  if(pAtCaloEntry*sin(parm.parameters()[Trk::theta]) < m_paramPtCut) useMeasuredEnergy = false;
  if(useMeasuredEnergy && !isIsolatedTrack(eta,phi)) useMeasuredEnergy = false;

  // Total eloss
  double Eloss_tot=0.0;

  // Aggregate TSOS, applying scaling corrections (if available) and mean-to-mop correction (cloning!)
  // This function checks if the measured energy loss has to be used assuming the track is isolated.
  // Need to check for isolation before and pass 0 values for not-isolated tracks.
  finalCaloTSOS  = modifyTSOSvector(caloTSOS, X0ScaleCALO, ElossScaleCALO,
				    m_repositionTSOS, m_aggregateTSOS, m_updateTSOS,
				    measCaloEnergy, measCaloEnergyError, fsrCaloEnergy,
				    pAtCaloEntry, pAtMuonEntryError,
				    Eloss_tot, useMeasuredEnergy,
                                    totalEloss, meanElossIoni, sigmaElossIoni);

  if(m_infoExtrapolation) {
    ATH_MSG_INFO( " after modifyTSOSvector X0ScaleCALO " << X0ScaleCALO << " ElossScaleCALO " << ElossScaleCALO <<
                 " pAtCaloEntry " << pAtCaloEntry << " pAtMuonEntryError " << pAtMuonEntryError << " total Eloss from TG through MuonEnergyTool " << Eloss_tot );
  }
  ATH_MSG_DEBUG("Aggregating and correcting TSOS down to : " << finalCaloTSOS->size() << " with total Eloss " << Eloss_tot);

#ifdef DEBUGON
    ATH_MSG_VERBOSE("FINAL CALO TSOS multiplicity : " << finalCaloTSOS->size());
    for(auto m : *finalCaloTSOS) this->printTSOS(m, "FINAL CALO TSOS");
#endif

  // delete tsos from extrapolator
  deleteTSOS(caloTSOS);

  return finalCaloTSOS;
}

CaloEnergy* Trk::TrkMaterialProviderTool::getParamCaloELoss(Trk::Track* track) const
{
  for (const Trk::TrackStateOnSurface* tsos : *track->trackStateOnSurfaces()) {
    if(tsos->materialEffectsOnTrack()) {
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(tsos->materialEffectsOnTrack());
      if(meot) {
	const Trk::EnergyLoss* energyLoss = meot->energyLoss();
	if (energyLoss) {
	  const CaloEnergy* caloEnergy = dynamic_cast<const CaloEnergy*>(meot->energyLoss());
	  if(caloEnergy) {
	    if(caloEnergy->energyLossType()==CaloEnergy::Tail) {
	      CaloEnergy* paramCaloEnergy = new CaloEnergy(caloEnergy->deltaEParam(),
							   0.5*(caloEnergy->sigmaMinusDeltaEParam()+caloEnergy->sigmaPlusDeltaEParam()),
							   caloEnergy->sigmaMinusDeltaEParam(),
							   caloEnergy->sigmaPlusDeltaEParam(),
							   CaloEnergy::NotIsolated,
							   caloEnergy->caloLRLikelihood(),
							   caloEnergy->caloMuonIdTag());
	      paramCaloEnergy->set_measEnergyLoss(caloEnergy->deltaEMeas(), caloEnergy->sigmaDeltaEMeas());
	      paramCaloEnergy->set_paramEnergyLoss(caloEnergy->deltaEParam(), caloEnergy->sigmaMinusDeltaEParam(), caloEnergy->sigmaPlusDeltaEParam());
	      return paramCaloEnergy;
	    }
	      return caloEnergy->clone();
	  }
	}
      }
    }
  }
  return (CaloEnergy*)nullptr;
}


//////////////////////////////////////////////// Privat //////////////////////////////////////////////////////

/**Helper to get last calo TSOS with TP*/
const Trk::TrackStateOnSurface* Trk::TrkMaterialProviderTool::getCaloExitTSOS(const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS, Trk::PropDirection dir) const
{
  const Trk::TrackStateOnSurface* tsosAtCaloExit = nullptr;
  if(dir==Trk::alongMomentum) {
    std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator it = caloTSOS->rbegin();
    std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator itEnd = caloTSOS->rend();
    for(; it!=itEnd; ++it) {
      if((*it)->trackParameters()) {
	ATH_MSG_DEBUG("Parameters at CALO exit found : "<< *((*it)->trackParameters()));
	tsosAtCaloExit = *it;
	return tsosAtCaloExit;
      }
    }
  }else{
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = caloTSOS->begin();
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd = caloTSOS->end();
    for(; it!=itEnd; ++it) {
	if((*it)->trackParameters()) {
	  ATH_MSG_DEBUG("Parameters at CALO exit found : "<< *((*it)->trackParameters()));
	  tsosAtCaloExit = *it;
	  return tsosAtCaloExit;
	}
    }
  }
   return tsosAtCaloExit;
}


/**Helper to get first calo TSOS with TP*/
const Trk::TrackStateOnSurface* Trk::TrkMaterialProviderTool::getCaloEntryTSOS(const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS, Trk::PropDirection dir) const
{
  const Trk::TrackStateOnSurface* tsosAtCaloEntry = nullptr;
  if(dir==Trk::alongMomentum) {
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = caloTSOS->begin();
    std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd = caloTSOS->end();
    for(; it!=itEnd; ++it) {
      if((*it)->trackParameters()) {
	ATH_MSG_DEBUG("Parameters at CALO entry found : "<< *((*it)->trackParameters()));
	tsosAtCaloEntry = *it;
	return tsosAtCaloEntry;
      }
    }
  }else{
    std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator it = caloTSOS->rbegin();
    std::vector<const Trk::TrackStateOnSurface*>::const_reverse_iterator itEnd = caloTSOS->rend();
    for(; it!=itEnd; ++it) {
      if((*it)->trackParameters()) {
	ATH_MSG_DEBUG("Parameters at CALO entry found : "<< *((*it)->trackParameters()));
	tsosAtCaloEntry = *it;
	return tsosAtCaloEntry;
      }
    }
  }
  return tsosAtCaloEntry;
}


/** Helper to remove MS and ID TSOS */
void Trk::TrkMaterialProviderTool::removeOutOfCalo(std::vector<const Trk::TrackStateOnSurface*>* caloTSOS) const
{
  // remove all track states on surface with getVolumeByGeo(state)!=2  ( ID = 1 Calo = 2 MS = 3)
  // and group all other track states at the  beginning of the vector.
  // finally erase from the vector all track state pointer of the
  // deleted objects, which are after remove_if at the end of the vector.
  const TrkMaterialProviderTool *this_=this;
  caloTSOS->erase( std::remove_if(caloTSOS->begin(),
                                  caloTSOS->end(),
                                  [this_](const Trk::TrackStateOnSurface *&state) {
                                    if (state && this_->getVolumeByGeo(state)!=2) {
                                      delete state;
                                      state=nullptr;
                                      return true;
                                    }

                                      return false;

                                  } ),
                   caloTSOS->end());

}

/** Helper to remove MS TSOS */
void Trk::TrkMaterialProviderTool::removeMS(std::vector<const Trk::TrackStateOnSurface*>* caloTSOS) const
{
  // remove all track states on surface with getVolumeByGeo(state)==3  ( ID = 1 Calo = 2 MS = 3)
  // and group all other track states at the  beginning of the vector.
  // finally erase from the vector all track state pointer of the
  // deleted objects, which are after remove_if at the end of the vector.
  const TrkMaterialProviderTool *this_=this;
  caloTSOS->erase( std::remove_if(caloTSOS->begin(),
                                  caloTSOS->end(),
                                  [this_](const Trk::TrackStateOnSurface *&state) {
                                    if (state && this_->getVolumeByGeo(state)==3) {
                                      delete state;
                                      state=nullptr;
                                      return true;
                                    }

                                      return false;

                                  } ),
                   caloTSOS->end());

}


/** Helper to update entries in the vector*/
void Trk::TrkMaterialProviderTool::updateVector(Trk::TrackStates* inputTSOS,
						Trk::TrackStates::iterator firstCALO,
						Trk::TrackStates::iterator firstMS,
						Trk::TrackStates* caloTSOS) 
{
  //printTSOS(*firstCALO, "UPD->FIRST CALO");
  //printTSOS(*firstMS, "UPD->FIRST MS");
  unsigned int ntoupdate=0;
  Trk::TrackStates::iterator it = firstCALO;
  while(it!=firstMS) {
    ++it;
    ntoupdate++;
  }

  // replace calo TSOS elements in input vector
  if(ntoupdate==caloTSOS->size()) {
    std::copy(caloTSOS->begin(), caloTSOS->end(), firstCALO);
  }else{
    it = firstCALO;
    unsigned int i=0;
    while(i<ntoupdate) {
      it = inputTSOS->erase(it);
      ++i;
      --firstMS;
    }
    inputTSOS->insert(firstMS, caloTSOS->begin(), caloTSOS->end());
  }
  // delete the view container
  delete caloTSOS;
}
void Trk::TrkMaterialProviderTool::updateVectorMS(Trk::TrackStates* inputTSOS,
						  const Trk::TrackStates::iterator& firstMS,
                                                  double X0ScaleMS, double ElossScaleMS) const
{

  bool debug = false;

// Scale the X0 and Energy loss in the Muon Spectrometer

  std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
  meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
  meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
  typePattern.set(Trk::TrackStateOnSurface::InertMaterial);
  typePattern.set(Trk::TrackStateOnSurface::Scatterer);

  if(X0ScaleMS<0.5||ElossScaleMS<0.5||X0ScaleMS>2.||ElossScaleMS>2.) {
    ATH_MSG_WARNING("Too large or too small X0ScaleMS " << X0ScaleMS << " ElossScaleMS " << ElossScaleMS);
  }

  Trk::TrackStates::iterator it = firstMS;
  int msStates = 0;
  int msMatStates = 0;
  int msMatParStates = 0;

// In the MuonSpectrometer the TSOS for the MaterialEffectsOnTrack do NOT have trackParameters

  for(;it!= inputTSOS->end();++it) {
    msStates++;
    if((*it)->materialEffectsOnTrack()) {
      msMatStates++;
//      if((*it)->trackParameters()) {
//        ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
//        continue;
//      }
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((*it)->materialEffectsOnTrack());
      if(meot) {
        msMatParStates++;
        const Trk::EnergyLoss* energyLoss = meot->energyLoss();
        if(energyLoss) {
         const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();
         if(scatAngles) {
           auto scatNew = Trk::ScatteringAngles(
             0,
             0,
             std::sqrt(X0ScaleMS) * scatAngles->sigmaDeltaTheta(),
             std::sqrt(X0ScaleMS) * scatAngles->sigmaDeltaPhi());
           double deltaE = (ElossScaleMS * energyLoss->deltaE());
           double sigmaDeltaE = (ElossScaleMS * energyLoss->sigmaDeltaE());
           double sigmaPlusDeltaE =
             (ElossScaleMS * energyLoss->sigmaPlusDeltaE());
           double sigmaMinusDeltaE =
             (ElossScaleMS * energyLoss->sigmaMinusDeltaE());
           double deltaE_ioni = (ElossScaleMS * energyLoss->meanIoni());
           double sigmaDeltaE_ioni = (ElossScaleMS * energyLoss->sigmaIoni());
           double deltaE_rad = (ElossScaleMS * energyLoss->meanRad());
           double sigmaDeltaE_rad = (ElossScaleMS * energyLoss->sigmaRad());
           double depth = energyLoss->length();

           if (debug)
             std::cout << " updateVectorMS Old Eloss " << energyLoss->deltaE()
                       << " new Eloss " << deltaE << std::endl;

           Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE,
                                                           sigmaDeltaE,
                                                           sigmaMinusDeltaE,
                                                           sigmaPlusDeltaE,
                                                           deltaE_ioni,
                                                           sigmaDeltaE_ioni,
                                                           deltaE_rad,
                                                           sigmaDeltaE_rad,
                                                           depth);
           const Trk::Surface& surf = meot->associatedSurface();
           auto newMeot =
             std::make_unique<Trk::MaterialEffectsOnTrack>(X0ScaleMS * meot->thicknessInX0(),
                                             scatNew,
                                             energyLossNew,
                                             surf,
                                             meotPattern);
           std::unique_ptr<const Trk::TrackParameters> pars{};
           if ((*it)->trackParameters())
             pars = (*it)->trackParameters()->uniqueClone();
           // make new TSOS
           const Trk::TrackStateOnSurface* newTSOS =
             new Trk::TrackStateOnSurface(
               nullptr, std::move(pars), nullptr, std::move(newMeot), typePattern);
           Trk::TrackStates* newTSOSvector =
             new Trk::TrackStates(SG::VIEW_ELEMENTS);
           newTSOSvector->push_back(&(*newTSOS));
           // replace TSOS in MS with new one
           std::copy(newTSOSvector->begin(), newTSOSvector->end(), it);
           delete newTSOSvector;
         }
        }
      }
    }
  }


  if(debug) {
   std::cout << " msStates " <<   msStates << " msMatStates " << msMatStates << " msMatParStates " << msMatParStates << std::endl;

// dump (new) energy loss
   for(it = firstMS;it!= inputTSOS->end();++it) {
    if((*it)->materialEffectsOnTrack()) {
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>((*it)->materialEffectsOnTrack());
      if(meot) {
        const Trk::EnergyLoss* energyLoss = meot->energyLoss();
        if(energyLoss) {
         const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();
         if(scatAngles) {
          std::cout << " updateVectorMS dump NEW Eloss " <<  energyLoss->deltaE() << std::endl;
         }
        }
      }
    }
   }

  } // end debug


}

//* Helper to indentify detector volume**/
unsigned int Trk::TrkMaterialProviderTool::getVolumeByGeo(const Trk::TrackStateOnSurface* m) const
{
  unsigned int vol = 0;
  const Trk::MeasurementBase* mot = m->measurementOnTrack();
  if(mot) {
    Identifier id = Trk::IdentifierExtractor::extract(mot);
    if(id.is_valid()) {
      if(m_DetID->is_indet(id)) vol=1;
      else if(m_DetID->is_muon(id)) vol=3;
      else vol=2;
    }
  }
  if(!vol) {
    vol=3;
    if(m_indetVolume->inside(m->surface().globalReferencePoint())) vol=1;
    else if(m_calorimeterVolume->inside(m->surface().globalReferencePoint())) vol=2;
  }
  return vol;
}


//** Helper to delete TSOS vectors*/
void Trk::TrkMaterialProviderTool::deleteTSOS(const std::vector<const Trk::TrackStateOnSurface*>* vecTSOS) 
{
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = vecTSOS->begin();
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd = vecTSOS->end();
  for (; it != itEnd; ++it) delete *it;
  delete vecTSOS;
}
void Trk::TrkMaterialProviderTool::deleteTSOS(Trk::TrackStates* vecTSOS) 
{
  if(vecTSOS->ownPolicy()==SG::VIEW_ELEMENTS) {
    Trk::TrackStates::const_iterator it = vecTSOS->begin();
    Trk::TrackStates::const_iterator itEnd = vecTSOS->end();
    for (; it != itEnd; ++it) delete *it;
  }
  delete vecTSOS;
}


//** Helper to printout TSOS details*/
void Trk::TrkMaterialProviderTool::printTSOS(const Trk::TrackStateOnSurface* m, const std::string& tag) const
{
  unsigned int ivolGeo = getVolumeByGeo(m);
  std::string volGeo="ID";
  if(ivolGeo==2) volGeo="CALO";
  else if(ivolGeo==3) volGeo="MS";
  ATH_MSG_VERBOSE(tag<<" Type "<<std::left<<std::setw(35)<<m->dumpType()
		<<" Volume "<<std::left<<std::setw(5)<<volGeo
		<<" r "<<std::left<<std::setw(10)<<m->surface().globalReferencePoint().perp()
		<<" z "<<std::left<<std::setw(10)<<m->surface().globalReferencePoint().z());
  if(m->materialEffectsOnTrack()) {
    ATH_MSG_VERBOSE(" -> Material: X0 "<<std::left<<std::setw(10)<<m->materialEffectsOnTrack()->thicknessInX0());
    const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
    if(meot) {
      const Trk::EnergyLoss* energyLoss = meot->energyLoss();
      if (energyLoss) {
	ATH_MSG_DEBUG(" geo " << volGeo << " radius " <<  m->surface().globalReferencePoint().perp() << " z " << m->surface().globalReferencePoint().z()  << " TSOS Eloss " <<energyLoss->deltaE());
	std::string type="P";
	const CaloEnergy* caloEnergy = dynamic_cast<const CaloEnergy*>(meot->energyLoss());
	if(caloEnergy && caloEnergy->energyLossType()==CaloEnergy::Tail) type="M";
	ATH_MSG_VERBOSE(" -> Eloss "<<type<<" "<<std::left<<std::setw(10)<<energyLoss->deltaE()<<" +- "<<std::left<<std::setw(10)<<energyLoss->sigmaDeltaE()
		      <<" + "<<energyLoss->sigmaPlusDeltaE()<<" - "<<energyLoss->sigmaMinusDeltaE()
		      <<" MopIoni "<<std::left<<std::setw(10)<<energyLoss->meanIoni()<<" +- "<<std::left<<std::setw(10)<<energyLoss->sigmaIoni()
		      <<" MeanRad "<<std::left<<std::setw(10)<<energyLoss->meanRad()<<" +- "<<std::left<<std::setw(10)<<energyLoss->sigmaRad() );
      }
      const Trk::ScatteringAngles* scatAngles = meot->scatteringAngles();
      if(scatAngles) {
	ATH_MSG_VERBOSE(" -> Scattering: dPhi "<<std::left<<std::setw(10)<<scatAngles->deltaPhi()<<" +- "<<std::left<<std::setw(10)<<scatAngles->sigmaDeltaPhi()<<
		      " dTheta "<<std::left<<std::setw(10)<<scatAngles->deltaTheta()<<" +- "<<std::left<<std::setw(10)<<scatAngles->sigmaDeltaTheta());
      }
    }
  }
  if(m->trackParameters()) {
    ATH_MSG_VERBOSE(" -> TP: r "<<std::left<<std::setw(10)<<m->trackParameters()->position().perp()
		  <<" z "<<std::left<<std::setw(10)<<m->trackParameters()->position().z()
		  <<" phi0 "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::phi0]
		  <<" eta "<<std::left<<std::setw(10)<<-std::log(std::tan(m->trackParameters()->parameters()[Trk::theta]/2)));
    if(m->trackParameters()->covariance()) {
      ATH_MSG_VERBOSE(" -> Errors: qOverP "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::qOverP)
		    <<" phi0 "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::phi0)
		    <<" theta "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::theta));
    }
  }
}

/** Function to modify TSOS doing repositioning, aggregation and corrections*/
Trk::TrackStates*
Trk::TrkMaterialProviderTool::modifyTSOSvector(const std::vector<const Trk::TrackStateOnSurface*>* matvec,
					       double scaleX0,
					       double scaleEloss,
					       bool reposition,
					       bool aggregate,
					       bool updateEloss,
					       double caloEnergy,
					       double caloEnergyError,
					       double fsrCaloEnergy,
					       double pCaloEntry,
					       double momentumError,
					       double& Eloss_tot,
                                               bool useMeasuredEnergy,
                                               double totalEloss, double meanElossIoni, double sigmaElossIoni) const
{
  //
  // inputs: TSOSs for material (matvec) and scale factors for X0 (scaleX0) and Eloss (scaleEloss)
  //
  // returns: new vector of TSOSs including scaling of X0 and Eloss;
  //
  // options:
  // bool reposition    correct repositioning of the scattering centers in space
  // bool aggregate     put scattering centra together in two planes
  // bool update Eloss  correct energy loss 1) including the measured calorimeter Eloss 2) include smearing of the muon momentum
  //
  // the routine should NOT be called for the ID
  // for best use in the Calorimeter:      bool reposition = true, bool aggregate = true and updateEloss = true (measured caloEnergy and caloEnergyError should be passed)
  //                                       note that the updateEloss is only active with aggregate = true
  // current version will NOT run correctly for MS (because the Eloss is made positive in the Calorimeter) needs fixing!
  // for best use in the Muon Specrometer: bool reposition = true, bool aggregate = true and updateEloss = false
  //
  // if one runs with reposition = false the scattering centra are kept at the END of the thick/dense material: that is not right for thick material for thin it is OK
  ATH_MSG_VERBOSE("Modifying TSOS vector size " << matvec->size()
		  << " with X0, Eloss scales " << scaleX0 << " " << scaleEloss
		  << " rep agg upd " << reposition << " " << aggregate << " " << updateEloss
		  << " caloE " << caloEnergy << " +- " << caloEnergyError
		  << " fsrCaloEnergy "<< fsrCaloEnergy
		  << " p " << pCaloEntry << " dp " << momentumError);

  //
  Trk::TrackStates* newTSOSvector = new Trk::TrackStates(SG::VIEW_ELEMENTS);
  
  // initialize total sum variables
  //
  //
  Eloss_tot = 0.;

  double X0_tot = 0.;

  double sigmaDeltaPhi2_tot = 0.;
  double sigmaDeltaTheta2_tot = 0.;
  double deltaE_tot = 0.;
  double sigmaDeltaE_tot = 0.;
  double sigmaPlusDeltaE_tot = 0.;
  double sigmaMinusDeltaE_tot = 0.;
  double deltaE_ioni_tot = 0.;
  double sigmaDeltaE_ioni_tot=0.;
  double deltaE_rad_tot = 0.;
  double sigmaDeltaE_rad_tot =0.;

  const Trk::TrackStateOnSurface* mprevious = nullptr;
  const Trk::TrackStateOnSurface* mfirst = nullptr;
  const Trk::TrackStateOnSurface* mlast = nullptr;
  Amg::Vector3D posFirst(0.,0.,0.);
  double deltaEFirst = 0.;

  double deltaPhi = 0.;
  double deltaTheta = 0.;

  int n_tot = 0;

  double w_tot = 0.;
  double wdist2 = 0.;
  Amg::Vector3D wdir(0.,0.,0.);
  Amg::Vector3D wpos(0.,0.,0.);

  std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
  meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
  meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
  typePattern.set(Trk::TrackStateOnSurface::InertMaterial);
  typePattern.set(Trk::TrackStateOnSurface::Scatterer);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
  typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternDeposit(0);
  typePatternDeposit.set(Trk::TrackStateOnSurface::CaloDeposit);
  const auto sqrt12=std::sqrt(12.);
  for(const auto *m : *matvec) {

    if(!m->trackParameters()) {
      ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
      continue;
    }
    if(m->materialEffectsOnTrack()) {
      double X0 = m->materialEffectsOnTrack()->thicknessInX0();
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
      const Trk::EnergyLoss* energyLoss = nullptr;
      const Trk::ScatteringAngles* scat = nullptr;
      if(meot) {
	energyLoss = meot->energyLoss();
	if (!energyLoss) {
	  ATH_MSG_WARNING("No energyLoss on TrackStateOnSurface ");
	  continue;
	}
	scat = meot->scatteringAngles();
	if(!scat) {
	  ATH_MSG_WARNING("No scatteringAngles on TrackStateOnSurface ");
	  continue;
	}
      } else {
        ATH_MSG_WARNING("No materialEffectsOnTrack on TrackStateOnSurface ");
        continue;
      }

      double depth = energyLoss->length();
      X0_tot += std::abs(scaleX0 * X0);

      sigmaDeltaTheta2_tot += scaleX0*scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();
      sigmaDeltaPhi2_tot   += scaleX0*scat->sigmaDeltaPhi()*scat->sigmaDeltaPhi();

      // Eloss sigma values add up linearly for Landau and exponential distributions
      deltaE_tot           += std::abs(scaleEloss*energyLoss->deltaE());
      sigmaDeltaE_tot      += std::abs(scaleEloss*energyLoss->sigmaDeltaE());
      sigmaPlusDeltaE_tot  += std::abs(scaleEloss*energyLoss->sigmaPlusDeltaE());
      sigmaMinusDeltaE_tot += std::abs(scaleEloss*energyLoss->sigmaMinusDeltaE());
      deltaE_ioni_tot      += std::abs(scaleEloss*energyLoss->meanIoni());
      sigmaDeltaE_ioni_tot += std::abs(scaleEloss*energyLoss->sigmaIoni());
      deltaE_rad_tot       += std::abs(scaleEloss*energyLoss->meanRad());
      sigmaDeltaE_rad_tot  += std::abs(scaleEloss*energyLoss->sigmaRad());

      n_tot++;

      Amg::Vector3D dir = m->trackParameters()->momentum().unit();
      Amg::Vector3D pos = m->trackParameters()->position();
      if(mprevious) {
        dir += mprevious->trackParameters()->momentum().unit();
      }

      dir = dir/dir.mag();
      Amg::Vector3D pos0   = pos - (depth/2.+depth/sqrt12)*dir;
      Amg::Vector3D posNew = pos - (depth/2.-depth/sqrt12)*dir;
      if(!mfirst) {
        mfirst = m;
        posFirst = pos0;
        deltaEFirst = energyLoss->deltaE();
      }
      mlast = m;

      double w = scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();

      w_tot += w;
      wpos += w*pos0/2.;
      wpos += w*posNew/2.;
      wdir += w*dir;

      wdist2 += w*(pos0-posFirst).mag2()/2.;
      wdist2 += w*(posNew-posFirst).mag2()/2.;

      if (!aggregate&&!reposition) {

        auto scatNew = ScatteringAngles(deltaPhi,
                                        deltaTheta,
                                        std::sqrt(sigmaDeltaPhi2_tot),
                                        std::sqrt(sigmaDeltaTheta2_tot));
        auto energyLossNew = Trk::EnergyLoss(deltaE_tot,
                                             sigmaDeltaE_tot,
                                             sigmaMinusDeltaE_tot,
                                             sigmaPlusDeltaE_tot,
                                             deltaE_ioni_tot,
                                             sigmaDeltaE_ioni_tot,
                                             deltaE_rad_tot,
                                             sigmaDeltaE_rad_tot,
                                             depth);
        const CaloEnergy* caloEnergyNew = new CaloEnergy(energyLossNew);
        const Trk::Surface& surf = meot->associatedSurface();
        auto meotLast =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(
            X0_tot, scatNew, caloEnergyNew, surf, meotPattern);
        auto pars = m->trackParameters()->uniqueClone();

        // make new TSOS
        const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface(
          nullptr, std::move(pars), nullptr, std::move(meotLast), typePattern);

        newTSOSvector->push_back(newTSOS);
        Eloss_tot += caloEnergyNew->deltaE();

        X0_tot = 0.;
        sigmaDeltaTheta2_tot = 0.;
        sigmaDeltaPhi2_tot = 0.;
        deltaE_tot = 0.;
        sigmaDeltaE_tot = 0;
        sigmaPlusDeltaE_tot = 0.;
        sigmaMinusDeltaE_tot = 0.;
        deltaE_ioni_tot = 0.;
        sigmaDeltaE_ioni_tot = 0.;
        deltaE_rad_tot = 0.;
        sigmaDeltaE_rad_tot = 0.;


      } else if(!aggregate&&reposition) {

        if(std::abs(depth)<10.) {

	  //        Thin scatterer: make only one TSOS

          auto scatNew =
            ScatteringAngles(deltaPhi,
                                 deltaTheta,
                                 sqrt(sigmaDeltaPhi2_tot),
                                 sqrt(sigmaDeltaTheta2_tot));
          auto energyLossNew = Trk::EnergyLoss(deltaE_tot,
                                               sigmaDeltaE_tot,
                                               sigmaMinusDeltaE_tot,
                                               sigmaPlusDeltaE_tot,
                                               deltaE_ioni_tot,
                                               sigmaDeltaE_ioni_tot,
                                               deltaE_rad_tot,
                                               sigmaDeltaE_rad_tot,
                                               depth);
          const CaloEnergy* caloEnergyNew = new CaloEnergy(energyLossNew);
          const Trk::Surface& surf = meot->associatedSurface();
          auto meotLast =
            std::make_unique<const Trk::MaterialEffectsOnTrack>(
              X0_tot, scatNew, caloEnergyNew, surf, meotPattern);
          auto pars = m->trackParameters()->uniqueClone();
          //        make new TSOS
          const Trk::TrackStateOnSurface* newTSOS =
            new Trk::TrackStateOnSurface(
              nullptr, std::move(pars), nullptr, std::move(meotLast), typePattern);
          newTSOSvector->push_back(newTSOS);
          Eloss_tot += caloEnergyNew->deltaE();

          X0_tot = 0.;
          sigmaDeltaTheta2_tot = 0.;
          sigmaDeltaPhi2_tot = 0.;
          deltaE_tot = 0.;
          sigmaDeltaE_tot = 0;
          sigmaPlusDeltaE_tot = 0.;
          sigmaMinusDeltaE_tot = 0.;
          deltaE_ioni_tot = 0.;
          sigmaDeltaE_ioni_tot = 0.;
          deltaE_rad_tot = 0.;
          sigmaDeltaE_rad_tot = 0.;

        } else {
	  //
	  //        Thick scatterer: make two TSOSs
	  //
	  //        prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
          const Trk::EnergyLoss* energyLoss0 = new EnergyLoss(0.,0.,0.,0.);
          auto scatFirst = ScatteringAngles(deltaPhi,
                                            deltaTheta,
                                            std::sqrt(sigmaDeltaPhi2_tot / 2.),
                                            std::sqrt(sigmaDeltaTheta2_tot / 2.));

          //        prepare for second MaterialEffectsOnTrack with X0 =  X0/2 Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
          auto scatNew =
            ScatteringAngles(deltaPhi,
                                 deltaTheta,
                                 std::sqrt(sigmaDeltaPhi2_tot / 2.),
                                 std::sqrt(sigmaDeltaTheta2_tot / 2.));
          auto energyLossNew = Trk::EnergyLoss(deltaE_tot,
                                               sigmaDeltaE_tot,
                                               sigmaMinusDeltaE_tot,
                                               sigmaPlusDeltaE_tot,
                                               deltaE_ioni_tot,
                                               sigmaDeltaE_ioni_tot,
                                               deltaE_rad_tot,
                                               sigmaDeltaE_rad_tot,
                                               0.);
          const CaloEnergy* caloEnergyNew = new CaloEnergy(energyLossNew);
          double norm = dir.perp();
          //        Rotation matrix representation
          Amg::Vector3D colx(-dir.y() / norm, dir.x() / norm, 0);
          Amg::Vector3D coly(
            -dir.x() * dir.z() / norm, -dir.y() * dir.z() / norm, norm);
          Amg::Vector3D colz(dir.x(), dir.y(), dir.z());

          Amg::Transform3D surfaceTransformFirst(colx, coly, colz, pos0);
          Amg::Transform3D surfaceTransformLast(colx, coly, colz, posNew);
          auto surfFirst =
            Trk::PlaneSurface(surfaceTransformFirst);
          auto surfLast =
            Trk::PlaneSurface(surfaceTransformLast);
          //        make MaterialEffectsOnTracks
          auto meotFirst =
            std::make_unique<const Trk::MaterialEffectsOnTrack>(
              X0_tot / 2., scatFirst, energyLoss0, surfFirst, meotPattern);
          auto meotLast =
            std::make_unique<const Trk::MaterialEffectsOnTrack>(
              X0_tot / 2., scatNew, caloEnergyNew, surfLast, meotPattern);


          //        calculate TrackParameters at first surface
          double qOverP0 = m->trackParameters()->charge()/ (m->trackParameters()->momentum().mag()-std::abs(energyLoss->deltaE()));
          if(mprevious) qOverP0 = mprevious->trackParameters()->charge()/mprevious->trackParameters()->momentum().mag();

          std::unique_ptr<const Trk::TrackParameters> parsFirst =
            surfFirst.createUniqueParameters<5, Trk::Charged>(
              0., 0., dir.phi(), dir.theta(), qOverP0);
          //        calculate TrackParameters at second surface
          double qOverPNew = m->trackParameters()->charge() /
                             m->trackParameters()->momentum().mag();
          std::unique_ptr<const Trk::TrackParameters> parsLast =
            surfLast.createUniqueParameters<5, Trk::Charged>(
              0., 0., dir.phi(), dir.theta(), qOverPNew);
          // make TSOS
          //
          const Trk::TrackStateOnSurface* newTSOSFirst =
            new Trk::TrackStateOnSurface(
              nullptr, std::move(parsFirst), nullptr, std::move(meotFirst), typePattern);
          const Trk::TrackStateOnSurface* newTSOS =
            new Trk::TrackStateOnSurface(
              nullptr, std::move(parsLast), nullptr, std::move(meotLast), typePattern);


          newTSOSvector->push_back(newTSOSFirst);
          newTSOSvector->push_back(newTSOS);
          Eloss_tot += caloEnergyNew->deltaE();

          X0_tot = 0.;
          sigmaDeltaTheta2_tot = 0.;
          sigmaDeltaPhi2_tot = 0.;
          deltaE_tot = 0.;
          sigmaDeltaE_tot = 0;
          sigmaPlusDeltaE_tot = 0.;
          sigmaMinusDeltaE_tot = 0.;
          deltaE_ioni_tot = 0.;
          sigmaDeltaE_ioni_tot = 0.;
          deltaE_rad_tot = 0.;
          sigmaDeltaE_rad_tot = 0.;
        }
      }

      mprevious = m;


    }
  }
  if (aggregate&&reposition) {

    if (n_tot>0) {
      //
      //        Make three scattering planes in Calorimeter else make two
      //
      Amg::Vector3D pos = wpos/w_tot;
      bool threePlanes = false;
      if (std::abs(pos.z()) < 6700 && pos.perp() < 4200)
        threePlanes = true; // always 3 planes in calo
      //
      auto scatFirst =
        ScatteringAngles(deltaPhi,
                             deltaTheta,
                             std::sqrt(sigmaDeltaPhi2_tot / 2.),
                             std::sqrt(sigmaDeltaTheta2_tot / 2.));
      auto  scatNew =
        ScatteringAngles(deltaPhi,
                             deltaTheta,
                             std::sqrt(sigmaDeltaPhi2_tot / 2.),
                             std::sqrt(sigmaDeltaTheta2_tot / 2.));
      Trk::EnergyLoss* energyLoss2 = new EnergyLoss(deltaE_tot,
                                                    sigmaDeltaE_tot,
                                                    sigmaMinusDeltaE_tot,
                                                    sigmaPlusDeltaE_tot,
                                                    deltaE_ioni_tot,
                                                    sigmaDeltaE_ioni_tot,
                                                    deltaE_rad_tot,
                                                    sigmaDeltaE_rad_tot,
                                                    0.);
      if (threePlanes)
        ATH_MSG_VERBOSE(" Calorimeter energyLoss2 delta E "
                        << energyLoss2->deltaE() << " meanIoni "
                        << energyLoss2->meanIoni() << " sigmaIoni "
                        << energyLoss2->sigmaIoni() << " X0_tot " << X0_tot);

      int elossFlag =
        0; // return Flag for updateEnergyLoss Calorimeter energy (0 = not used)

      double calE = caloEnergy;
      double calEr = caloEnergyError;

      //      if(!useMeasuredEnergy) calE = 0.;
      if (!useMeasuredEnergy)
        calEr = 0.;

      Trk::EnergyLoss* energyLossNew =
        (updateEloss
           ? m_elossupdator->updateEnergyLoss(
               energyLoss2, calE, calEr, pCaloEntry, momentumError, elossFlag)
           : new EnergyLoss(deltaE_tot,
                            sigmaDeltaE_tot,
                            sigmaMinusDeltaE_tot,
                            sigmaPlusDeltaE_tot,
                            deltaE_ioni_tot,
                            sigmaDeltaE_ioni_tot,
                            deltaE_rad_tot,
                            sigmaDeltaE_rad_tot,
                            0.));
      CaloEnergy* caloEnergyNew = new CaloEnergy(*energyLossNew);
      if (threePlanes)
        ATH_MSG_VERBOSE(" After update Calorimeter energyLossNew "
                        << energyLossNew->deltaE() << " meanIoni "
                        << energyLossNew->meanIoni() << " sigmaIoni "
                        << energyLossNew->sigmaIoni());

      caloEnergyNew->set_measEnergyLoss(caloEnergy, caloEnergyError);
      // Store FSR calo energy
      caloEnergyNew->set_fsrCandidateEnergy(fsrCaloEnergy);
      // Store both measured and parametrised eloss on CaloEnergy object
      if(elossFlag!=0) {
        caloEnergyNew->set_energyLossType(CaloEnergy::Tail);
      } else {
        if(!useMeasuredEnergy) caloEnergyNew->set_energyLossType(CaloEnergy::NotIsolated);
      }

      int eLossFlagTmp = 0;
      Trk::EnergyLoss* energyLossParam = m_elossupdator->updateEnergyLoss(energyLoss2, 0.0, 0.0, pCaloEntry, 0., eLossFlagTmp);

      caloEnergyNew->set_paramEnergyLoss(energyLossParam->deltaE(), energyLossParam->sigmaMinusDeltaE(), energyLossParam->sigmaPlusDeltaE());
      if(m_overwriteElossParam&&m_useCaloEnergyMeasurement) caloEnergyNew->set_paramEnergyLoss(totalEloss,meanElossIoni,0.45*sigmaElossIoni);

      ATH_MSG_DEBUG( " modifyTSOSvector energyLossParam Eloss " << energyLossParam->deltaE() << " on TSOS " << energyLossNew->deltaE() << " calE " << calE);

      delete energyLossParam;


      delete energyLossNew;
      delete energyLoss2;

      //        direction of plane
      Amg::Vector3D dir = wdir/w_tot;
      dir = dir/dir.mag();
      double norm = dir.perp();
      //        Rotation matrix representation
      Amg::Vector3D colx(-dir.y()/norm,        dir.x()/norm,           0);
      Amg::Vector3D coly(-dir.x()*dir.z()/norm, -dir.y()*dir.z()/norm, norm);
      Amg::Vector3D colz( dir.x(),             dir.y(),                dir.z());
      //        Centre position of the two planes
      double halflength2 = wdist2/w_tot - (pos-posFirst).mag()*(pos-posFirst).mag();
      double halflength = 0.;
      if(halflength2>0) halflength = sqrt(halflength2);
      Amg::Vector3D pos0   = pos - halflength*dir;
      Amg::Vector3D posNew = pos + halflength*dir;
//
// force the planes to be inside the Calorimeter
//
      double scaleCalo = 1.;
      double scaleCaloNew = std::abs(pos0.z())/6700;
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = std::abs(posNew.z())/6700;
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = std::abs(pos0.perp())/4200;
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = std::abs(posNew.perp())/4200;
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;

      if(scaleCalo>1.) {
        pos0       = pos0/scaleCalo;
        pos        = pos/scaleCalo;
        posNew     = posNew/scaleCalo;
        halflength = halflength/scaleCalo;
        ATH_MSG_VERBOSE(" position scattering planes inside calo scale factor " << scaleCalo);
      }

      if(updateEloss) ATH_MSG_VERBOSE("WITH updateEloss");
      Amg::Transform3D surfaceTransformFirst(colx,coly,colz,pos0);
      Amg::Transform3D surfaceTransformLast(colx,coly,colz,posNew);
      auto surfFirst = Trk::PlaneSurface( surfaceTransformFirst );
      auto surfLast= Trk::PlaneSurface( surfaceTransformLast );
      //        calculate TrackParameters at first surface
      double qOverP0 = mfirst->trackParameters()->charge()/(mfirst->trackParameters()->momentum().mag()+std::abs(deltaEFirst));
      //        calculate TrackParameters at last surface
      double qOverPNew = mlast->trackParameters()->charge()/mlast->trackParameters()->momentum().mag();
      std::unique_ptr<Trk::TrackParameters> parsFirst =
        surfFirst.createUniqueParameters<5, Trk::Charged>(
          0., 0., dir.phi(), dir.theta(), qOverP0);
      std::unique_ptr<const Trk::TrackParameters> parsLast =
        surfLast.createUniqueParameters<5, Trk::Charged>(
          0., 0., dir.phi(), dir.theta(), qOverPNew);

      if(!threePlanes) {
        //
        // make two scattering planes and TSOS
        //
        //          prepare for first MaterialEffectsOnTrack with X0 = X0/2
        //          Eloss = 0 and scattering2 = total2 / 2. depth = 0
        auto meotFirst =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(X0_tot / 2.,
                                          scatFirst,
                                          nullptr,
                                          surfFirst,
                                          meotPattern);
        //          prepare for second MaterialEffectsOnTrack with X0 =  X0/2
        //          Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
        auto meotLast =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(X0_tot / 2.,
                                          scatNew,
                                          caloEnergyNew,
                                          surfLast,
                                          meotPattern);
        //
        //
        const Trk::TrackStateOnSurface* newTSOSFirst =
          new Trk::TrackStateOnSurface(
            nullptr, std::move(parsFirst), nullptr, std::move(meotFirst), typePattern);
        const Trk::TrackStateOnSurface* newTSOS =
          (elossFlag != 0
             ? new Trk::TrackStateOnSurface(
                 nullptr, std::move(parsLast), nullptr, std::move(meotLast), typePatternDeposit)
             : new Trk::TrackStateOnSurface(
                 nullptr, std::move(parsLast), nullptr, std::move(meotLast), typePattern));
        newTSOSvector->push_back(newTSOSFirst);
        newTSOSvector->push_back(newTSOS);
      } else {
        //
        // make three scattering planes and TSOS in Calorimeter
        //
        Amg::Transform3D surfaceTransform(colx, coly, colz, pos);
        auto surf = Trk::PlaneSurface(surfaceTransform);
        std::unique_ptr<Trk::TrackParameters> pars =
          surf.createUniqueParameters<5, Trk::Charged>(
            0., 0., dir.phi(), dir.theta(), qOverPNew);

        //        prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss
        //        = 0 and scattering2 = total2 / 2. depth = 0
        auto meotFirst =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(
            X0_tot / 2., scatFirst, nullptr, surfFirst, meotPattern);

        //        prepare for middle MaterialEffectsOnTrack with X0 =  0 Eloss =
        //        ElossNew and scattering2 = 0. depth = 0
        auto meot =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(
            0.,std::nullopt, caloEnergyNew, surf, meotPattern);

        //        prepare for last MaterialEffectsOnTrack with X0 =  X0/2 Eloss
        //        = 0 total and scattering2 = total2 / 2. depth = 0
        auto meotLast =
          std::make_unique<const Trk::MaterialEffectsOnTrack>(
            X0_tot / 2., scatNew, nullptr, surfLast, meotPattern);


        const Trk::TrackStateOnSurface* newTSOSFirst =
          new Trk::TrackStateOnSurface(
            nullptr, std::move(parsFirst), nullptr, std::move(meotFirst), typePatternScat);
        const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface(
          nullptr, std::move(pars), nullptr, std::move(meot), typePatternDeposit);
        const Trk::TrackStateOnSurface* newTSOSLast =
          new Trk::TrackStateOnSurface(
            nullptr, std::move(parsLast), nullptr, std::move(meotLast), typePatternScat);

        newTSOSvector->push_back(newTSOSFirst);
        newTSOSvector->push_back(newTSOS);
        newTSOSvector->push_back(newTSOSLast);
      }

      Eloss_tot += caloEnergyNew->deltaE();

    }

  }

  return newTSOSvector;
}


/** Function to get mop and mean (ionization) energy loss*/
void Trk::TrkMaterialProviderTool::getMopAndIoniEnergyLoss(const std::vector<const Trk::TrackStateOnSurface*>* matvec,
							   double pCaloEntry,
							   double scaleEloss,
							   double& totalEloss,
							   double& meanElossIoni,
							   double& sigmaElossIoni) const
{
// routine calculates the totalEloss    = meanIoni + meanRad
//                        meanElossIoni = meanIoni
//                        sigmaElossIoni = sigmaIoni
//

//  Be carefull with using deltaE() that number starts as totalEloss but is updated to values close to meanIoni depending on the fit

  double deltaE_tot = 0.;
  double sigmaDeltaE_tot = 0.;
  double sigmaPlusDeltaE_tot = 0.;
  double sigmaMinusDeltaE_tot = 0.;
  double deltaE_ioni_tot = 0.;
  double sigmaDeltaE_ioni_tot=0.;
  double deltaE_rad_tot = 0.;
  double sigmaDeltaE_rad_tot =0.;

  for(const auto *m : *matvec) {
    if(!m->trackParameters()) {
      ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
      continue;
    }
    if(m->materialEffectsOnTrack()) {
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
      const Trk::EnergyLoss* energyLoss = nullptr;
      if(meot) {
        energyLoss = meot->energyLoss();
        if (!energyLoss) {
          ATH_MSG_WARNING("No energyLoss on TrackStateOnSurface ");
          continue;
        }
      } else {
        ATH_MSG_WARNING("No materialEffectsOnTrack on TrackStateOnSurface ");
        continue;
      }
      deltaE_tot           += std::abs(scaleEloss*energyLoss->deltaE());
      sigmaDeltaE_tot      += std::abs(scaleEloss*energyLoss->sigmaDeltaE());
      sigmaPlusDeltaE_tot  += std::abs(scaleEloss*energyLoss->sigmaPlusDeltaE());
      sigmaMinusDeltaE_tot += std::abs(scaleEloss*energyLoss->sigmaMinusDeltaE());
      deltaE_ioni_tot      += std::abs(scaleEloss*energyLoss->meanIoni());
      sigmaDeltaE_ioni_tot += std::abs(scaleEloss*energyLoss->sigmaIoni());
      deltaE_rad_tot       += std::abs(scaleEloss*energyLoss->meanRad());
      sigmaDeltaE_rad_tot  += std::abs(scaleEloss*energyLoss->sigmaRad());

      ATH_MSG_DEBUG(" position x " <<  m->trackParameters()->position().x() << " y " <<  m->trackParameters()->position().y()  << " perp " << m->trackParameters()->position().perp() << " z " <<  m->trackParameters()->position().z() );
      ATH_MSG_DEBUG(" deltaE " << (scaleEloss*energyLoss->deltaE())  << " deltaE_ioni " << (scaleEloss*energyLoss->meanIoni()) << " sigmaDeltaE_ioni " << (scaleEloss*energyLoss->sigmaIoni()));
      ATH_MSG_DEBUG(" deltaE_tot " << deltaE_tot  << " deltaE_ioni_tot " << deltaE_ioni_tot << " sigmaDeltaE_ioni_tot " << sigmaDeltaE_ioni_tot);
      if(energyLoss->sigmaIoni()<0) ATH_MSG_DEBUG(" ALARM sigmaIoni negative " << scaleEloss*energyLoss->sigmaIoni());
      if(energyLoss->sigmaRad()<0)  ATH_MSG_DEBUG(" ALARM sigmaRad negative " << scaleEloss*energyLoss->sigmaRad());

    }
  }

  EnergyLoss* eLoss = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot,
				     deltaE_ioni_tot, sigmaDeltaE_ioni_tot,
				     deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.) ;

  int elossFlag=0;

  std::unique_ptr<EnergyLoss> eLoss2 ( m_elossupdator->updateEnergyLoss(eLoss, 0, 0, pCaloEntry, 0, elossFlag) );

  totalEloss = eLoss2->meanIoni() + eLoss2->meanRad();
  meanElossIoni = eLoss2->meanIoni();
  sigmaElossIoni = eLoss2->sigmaIoni();

  ATH_MSG_DEBUG("Mop Energy Loss " << totalEloss << " mean ionization energy loss " << meanElossIoni << " sigmaElossIoni " << sigmaElossIoni);

  delete eLoss;
}

/** Function to check isolation */
bool Trk::TrkMaterialProviderTool::isIsolatedTrack(double eta, double phi) const
{
  int nTracks 	= 0;
  std::pair<int,double> inner = m_trackIsolationTool->trackIsolation(Gaudi::Hive::currentContext(),eta,phi);
  nTracks 	= inner.first;

  ATH_MSG_VERBOSE("Isolation : Number of tracks in cone " << nTracks << " cut < " << m_maxNTracksIso);

  return nTracks <= m_maxNTracksIso;
}

/** Function to get calorimeter measured energy loss*/
double Trk::TrkMaterialProviderTool::getCaloMeasuredEnergy(double eta, double phi,
							   double totalEloss,
							   double meanElossIoni,
							   double& fsrCaloEnergy) const
{
  // Retrieve the measured energy from calorimeter
  std::unique_ptr<Rec::CaloMeas> caloMeas = m_caloMeasTool->energyMeasurement(Gaudi::Hive::currentContext(),eta,phi,eta,phi);


  // Sum-up components
  double FinalMeasuredEnergy = getFinalMeasuredEnergy(caloMeas.get(), totalEloss, meanElossIoni, eta, fsrCaloEnergy);

  return FinalMeasuredEnergy;
}

/** Function to get corrected calorimeter measured energy loss*/
double Trk::TrkMaterialProviderTool::getFinalMeasuredEnergy(Rec::CaloMeas* caloMeas,
							    double totalEloss,
							    double meanElossIoni,
							    double eta,
							    double& fsrCaloEnergy) const
{
  double MopLossCorrected = totalEloss;
  double MopLoss = totalEloss;

  // FSR candidate energy
  fsrCaloEnergy = MopLoss*m_caloParamTool->emMopFraction(eta);

  // percentage of inert material
  const double InertMaterial		 = m_caloParamTool->x0mapInertMaterial(eta);
  // percentage of em calorimeter material
  const double EmMaterial		 = m_caloParamTool->x0mapEmMaterial(eta);
  // percentage of hec calorimeter material
  const double HECMaterial		 = m_caloParamTool->x0mapHecMaterial(eta);
  // correction for the inert material
  double MaterialCorrection		 = InertMaterial * MopLossCorrected;

  // fraction of Tile used for the measurement
  const double TileMeasurementMaterial	 = caloMeas->Tile_SamplingFraction();
  // fraction of LArHEC used for the measurement
  const double LArHECMeasurementMaterial = caloMeas->LArHEC_SamplingFraction();
  // fraction of LArEM used for the measurement
  const double LArEmMeasurementMaterial	 = caloMeas->LArEM_SamplingFraction();
  // Measured energy deposition in Tile
  const double TileEnergy		 = caloMeas->Tile_EnergyMeasured();
  // Measured energy deposition in E/M
  const double EmEnergy			 = caloMeas->LArEM_EnergyMeasured();

  // Correction for forward calorimetry
  double ForwardHECCorrection	= 0.;
  if (std::abs(eta)>2. && caloMeas->LArHEC_EnergyMeasured()>100.)
    ForwardHECCorrection	= (1. - LArHECMeasurementMaterial) * HECMaterial * MopLossCorrected;
  const double LArHECEnergy	= caloMeas->LArHEC_EnergyMeasured() + ForwardHECCorrection;	// Measured energy deposition in LArHEC

  double TotalMeasuredEnergy	= TileEnergy + EmEnergy + LArHECEnergy;

  ATH_MSG_VERBOSE( "Energy Deposition:Tile " 	<< TileEnergy
		   << " LArHEC "		<< LArHECEnergy
		   << " EM "			<< EmEnergy);
  ATH_MSG_VERBOSE("ForwardHECCorrection "	<< ForwardHECCorrection
		  << " HECMaterial "		<< HECMaterial
		  << " MopLossCorrected "	<< MopLossCorrected );

  bool bHEC	= false; // performed HEC measurement?
  bool bEM	= false; // performed Em measurement?

  // If muon isolated, and no significant measurement is made then use the mop parameterization, else the mean
  if (std::abs(eta)<1.4) {
    if (LArHECEnergy + TileEnergy > 0.1 * MopLoss * HECMaterial)	bHEC= true;
  }
  else if (std::abs(eta)>1.8) {
    if (LArHECEnergy + TileEnergy > 0.2 * MopLoss * HECMaterial)	bHEC= true;
  }else{
    if (LArHECEnergy + TileEnergy > 0.25 * MopLoss * HECMaterial)	bHEC= true;
  }
  if (EmEnergy > 0.5 * MopLoss * EmMaterial)				bEM = true;

  double MeasCorrected = TotalMeasuredEnergy + MaterialCorrection;
  // Need to calculate the corresponding mip energy deposition
  // Muons of 10 GeV are already in the relativistic rise region
  // in order to obtain the mip deposition from the mean energy deposition of 10 GeV muons
  // should divide by approximately 1.4 (Review of Particle Physics Figure 27.3 p.243)
  const double IonizationLoss	= (1./1.4) * meanElossIoni;
  double eOverMipCorrectionEm   = 0.;
  double eOverMipCorrectionHEC  = 0.;
  // Etrue = emip * Emeas
  // -DE = Emeas - Etrue = Etrue ( 1./emip -1.)
  if (bEM) {
    const double emipEM    = 0.78;
    eOverMipCorrectionEm   = - (1./emipEM-1.) * IonizationLoss * EmMaterial * LArEmMeasurementMaterial;
    if (EmEnergy + eOverMipCorrectionEm<0.)eOverMipCorrectionEm=0.;
  }
  if (bHEC) {
    const double emipTile  = 0.86;
    const double emipLAr   = 0.94;
    const double HECEnergy = TileEnergy + LArHECEnergy;
    const double eOverMipCorrectionTile = - (1./emipTile-1.) * TileEnergy / HECEnergy * IonizationLoss * HECMaterial * TileMeasurementMaterial;
    const double eOverMipCorrectionLAr  = - (1./emipLAr-1.) * LArHECEnergy / HECEnergy * IonizationLoss * HECMaterial * LArHECMeasurementMaterial;
    eOverMipCorrectionHEC  = eOverMipCorrectionTile + eOverMipCorrectionLAr;
    if (LArHECEnergy + TileEnergy + eOverMipCorrectionHEC < 0.0) eOverMipCorrectionHEC=0.;
  }
  const double eOverMipCorrection = eOverMipCorrectionEm + eOverMipCorrectionHEC;

  // //  additional offset from high-statistics Z->mumu MC (measured by Peter K 30/11/2011)
  // double fix1FromPeter[26]	= {  0.424104 ,  0.479637 ,  0.483419 ,  0.490242 ,  0.52806  ,
  // 				     0.573582 ,  0.822098 ,  0.767301 ,  0.809919 ,  0.658745 ,
  // 				     0.157187 ,  0.413214 ,  0.771074 ,  0.61815  ,  0.350113 ,
  // 				     0.322785 ,  0.479294 ,  0.806183 ,  0.822161 ,  0.757731 ,
  // 				     -0.0857186, -0.0992693, -0.0492252,  0.0650174,  0.261538 ,
  // 				     0.360413 };
  // //  (update from Peter K 09/12/2011)
  // double fix2FromPeter[26]	= { -0.647703 , -0.303498 , -0.268645 , -0.261292 , -0.260152 ,
  // 				    -0.269253 , -0.266212 , -0.240837 , -0.130172 , -0.111638 ,
  // 				    -0.329423 , -0.321011 , -0.346050 , -0.305592 , -0.313293 ,
  // 				    -0.317111 , -0.428393 , -0.524839 , -0.599547 , -0.464013 ,
  // 				    -0.159663 , -0.140879 , -0.0975618,  0.0225352,  0.0701925,
  // 				    -0.24778 };
  // int ieta			=  static_cast<int> (std::abs(eta)/0.10);
  // if (ieta > 25) ieta		=  25;

  double FinalMeasuredEnergy	= MeasCorrected + eOverMipCorrection;// + (fix1FromPeter[ieta] + fix2FromPeter[ieta])*CLHEP::GeV;

  return FinalMeasuredEnergy;
}

void Trk::TrkMaterialProviderTool::throwFailedToGetTrackingGeomtry() const {
   std::stringstream msg;
   msg << "Failed to get conditions data " << m_trackingGeometryReadKey.key() << ".";
   throw std::runtime_error(msg.str());
}

