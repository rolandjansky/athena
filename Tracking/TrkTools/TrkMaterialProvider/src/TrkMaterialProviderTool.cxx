/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkMaterialProvider/TrkMaterialProviderTool.h"

#include <algorithm>

// Interfaces
#include "TrkExInterfaces/IExtrapolator.h"
#include "TrkExInterfaces/IEnergyLossUpdator.h"

#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "TrkMaterialOnTrack/ScatteringAngles.h"
#include "TrkGeometry/TrackingGeometry.h"
#include "TrkGeometry/TrackingVolume.h"
#include "AtlasDetDescr/AtlasDetectorID.h"
#include "Identifier/Identifier.h"
#include "TrkEventUtils/IdentifierExtractor.h"
#include "MagFieldInterfaces/IMagFieldSvc.h"

#include "muonEvent/CaloEnergy.h"

// Amg::error
#include "EventPrimitives/EventPrimitivesHelpers.h"

// For measured energy loss
#include "CLHEP/Units/SystemOfUnits.h"
#include "MuidEvent/CaloMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyMeas.h"
#include "MuidInterfaces/IMuidCaloEnergyParam.h"
#include "MuidInterfaces/IMuidTrackIsolation.h"
#include "RecoToolInterfaces/IMuonCaloEnergyTool.h"

//#define DEBUGON //To activate printout for TSOS lists at various stages
// for line-by-line debugging
#define MYDEBUG() std::cout<<__FILE__<<" "<<__func__<<" "<<__LINE__<<std::endl

void myLocal_resetTrack( const Trk::Track& ctrack ){

  Trk::Track& track = const_cast<Trk::Track&>(ctrack);
  if( track.m_cachedParameterVector ){
    delete track.m_cachedParameterVector;
    track.m_cachedParameterVector = 0;
  }

  if( track.m_cachedMeasurementVector ){
    delete track.m_cachedMeasurementVector;
    track.m_cachedMeasurementVector = 0;;
  }
  if( track.m_cachedOutlierVector ){
    delete track.m_cachedOutlierVector;
    track.m_cachedOutlierVector = 0;
  }
}

// constructor
Trk::TrkMaterialProviderTool::TrkMaterialProviderTool(const std::string& t, const std::string& n, const IInterface* p)
  :	AthAlgTool(t,n,p),
	m_muonExtrapolator("Trk::Extrapolator/AtlasExtrapolator"),
	m_elossupdator("Trk::EnergyLossUpdator/AtlasEnergyLossUpdator"),
	m_trackingVolumesSvc("TrackingVolumesSvc/TrackingVolumesSvc",n),
	m_trackingGeometrySvc("TrackingGeometrySvc/AtlasTrackingGeometrySvc",n),
        m_scattool("Trk::MultipleScatteringUpdator/AtlasMultipleScatteringUpdator"),
	m_caloMeasTool		("Rec::MuidCaloEnergyMeas/MuidCaloEnergyMeas"),
	m_caloParamTool		("Rec::MuidCaloEnergyParam/MuidCaloEnergyParam"),
	m_trackIsolationTool	("Rec::MuidTrackIsolation/MuidTrackIsolation"),
	m_muonCaloEnergyTool    ("Rec::MuonCaloEnergyTool/MuonCaloEnergyTool"),	
	m_DetID(0),
	m_calorimeterVolume(0),
	m_indetVolume(0),
        m_magFieldSvc           ("AtlasFieldSvc",n),
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
  declareProperty("MagFieldSvc",                      m_magFieldSvc);
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
  }
  ATH_CHECK(m_trackIsolationTool.retrieve());

  ATH_CHECK(m_magFieldSvc.retrieve());

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

  ATH_CHECK(m_trackingGeometrySvc.retrieve());

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
void Trk::TrkMaterialProviderTool::updateCaloTSOS(Trk::Track& track, const Trk::TrackParameters* startParameters)
{
  ATH_MSG_VERBOSE("updateCaloTSOS(Trk::Track& track, const Trk::TrackParameters* startParameters)");    

  // back extrapolate to perigee, get pAtCaloEntry from list of TSOSs 
  // and update/add calo+ID material to mstrack to be refitted.
  DataVector<const Trk::TrackStateOnSurface>* inputTSOS = const_cast<DataVector<const Trk::TrackStateOnSurface>*>(track.trackStateOnSurfaces());

  // Iterators
  DataVector<const Trk::TrackStateOnSurface>::iterator lastIDwP  = inputTSOS->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator firstCALO = inputTSOS->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator firstMS   = inputTSOS->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator firstMSwP = inputTSOS->end();

  // find first MS TSOS (handling the case of TSOS w/o TP) and the last ID (or perigee)
  DataVector<const Trk::TrackStateOnSurface>::iterator it    = inputTSOS->begin();
  DataVector<const Trk::TrackStateOnSurface>::iterator itEnd = inputTSOS->end();
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
    /** deprecated outputLevel()
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS)
	      printTSOS(m, "DEBUG-TSOS");
    }
    **/
    return;
  }
  if(firstCALO == inputTSOS->end()) {
    ATH_MSG_DEBUG("Track without CALO TSOS!");    
    firstCALO = firstMS;
  }
  if(firstMS == inputTSOS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS");    
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    /**
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS)
	    printTSOS(m, "DEBUG-TSOS");
    }
    **/
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  //bool removeOoC = false;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator firstMSnotPerigee = firstMS;
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
  DataVector<const Trk::TrackStateOnSurface>* caloTSOS = this->getCaloTSOS (*startParameters,
									    track,
									    (*firstMSnotPerigee)->surface(),
									    Trk::alongMomentum, 
									    Trk::muon,
                                                                            Eloss, X0ScaleMS, ElossScaleMS,
									    (firstMSwP == inputTSOS->end()) ? 0 : (*firstMSwP)->trackParameters(),
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
  this->updateVectorMS(inputTSOS,firstMS,X0ScaleMS,ElossScaleMS); 
  // update the original vector
  this->updateVector(inputTSOS, firstCALO, firstMS, caloTSOS);
  myLocal_resetTrack(track);

#ifdef DEBUGON
    ATH_MSG_VERBOSE("NEW TSOS multiplicity : " << inputTSOS->size());
    for(auto m : *inputTSOS) this->printTSOS(m, "NEW TSOS");
#endif

  return;
}


// Update Calorimeter TSOS using TG for the combined fit
void Trk::TrkMaterialProviderTool::updateCaloTSOS(Trk::Track& idTrack, Trk::Track& extrapolatedTrack)
{
  ATH_MSG_VERBOSE("updateCaloTSOS(Trk::Track& idTrack, Trk::Track& extrapolatedTrack)");    
  
  DataVector<const Trk::TrackStateOnSurface>* inputTSOS_ID = const_cast<DataVector<const Trk::TrackStateOnSurface>*>(idTrack.trackStateOnSurfaces());
  DataVector<const Trk::TrackStateOnSurface>* inputTSOS_MS = const_cast<DataVector<const Trk::TrackStateOnSurface>*>(extrapolatedTrack.trackStateOnSurfaces());
  

  // find last ID TSOS
  DataVector<const Trk::TrackStateOnSurface>::iterator lastIDwP  = inputTSOS_ID->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator it        = inputTSOS_ID->end()-1;
  DataVector<const Trk::TrackStateOnSurface>::iterator itFront   = inputTSOS_ID->begin();
  while(*it) {
    if(this->getVolumeByGeo(*it)==1 && (*it)->trackParameters()) {
      lastIDwP = it;
      break;
    }
    if(it==itFront) break;
    --it;
  }
  
  // find first MS TSOS
  DataVector<const Trk::TrackStateOnSurface>::iterator firstCALO = inputTSOS_MS->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator firstMS   = inputTSOS_MS->end();
  DataVector<const Trk::TrackStateOnSurface>::iterator firstMSwP = inputTSOS_MS->end();
  it = inputTSOS_MS->begin();
  DataVector<const Trk::TrackStateOnSurface>::iterator itEnd = inputTSOS_MS->end();
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
    /** deprecated outputLevel()
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS_ID)
	      printTSOS(m, "DEBUG-ID-TSOS");
    }
    **/
    return;
  }
  if(firstMS == inputTSOS_MS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS!");    
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    /** deprecated outputLevel()
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS_MS)
	      printTSOS(m, "DEBUG-MS-TSOS");
    }
    **/
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  //bool removeOoC = false;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator firstMSnotPerigee = firstMS;
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
  DataVector<const Trk::TrackStateOnSurface>* caloTSOS = this->getCaloTSOS (*(*lastIDwP)->trackParameters(),
									    extrapolatedTrack,
									    (*firstMSnotPerigee)->surface(),
									    Trk::alongMomentum, 
									    Trk::muon,
                                                                            Eloss, X0ScaleMS, ElossScaleMS,
									    (firstMSwP == inputTSOS_MS->end()) ? 0 : (*firstMSwP)->trackParameters(),
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
  this->updateVectorMS(inputTSOS_MS,firstMS,X0ScaleMS,ElossScaleMS); 
  // update the original vector
  this->updateVector(inputTSOS_MS, firstCALO, firstMS, caloTSOS);
  
  myLocal_resetTrack(extrapolatedTrack);

#ifdef DEBUGON
    ATH_MSG_VERBOSE("NEW-MS TSOS multiplicity : " << inputTSOS_MS->size());
    for(auto m : *inputTSOS_MS) printTSOS(m, "NEW-MS TSOS");
#endif

  return;
}



// Get Calorimeter MEOT using TG for the combined fit
void Trk::TrkMaterialProviderTool::getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack, 
					  std::vector<MaterialEffectsOnTrack>& calomeots)
{
  ATH_MSG_VERBOSE("getCaloMEOT(const Trk::Track& idTrack, const Trk::Track& msTrack, std::vector<MaterialEffectsOnTrack>& calomeots)");    

  const DataVector<const Trk::TrackStateOnSurface>* inputTSOS_ID = idTrack.trackStateOnSurfaces();
  const DataVector<const Trk::TrackStateOnSurface>* inputTSOS_MS = msTrack.trackStateOnSurfaces();
  
#ifdef DEBUGON
  for(auto m : *inputTSOS_ID) printTSOS(m, "TSOS ID TRACK");
  for(auto m : *inputTSOS_MS) printTSOS(m, "TSOS MS TRACK");
#endif
  
  // find last ID TSOS
  DataVector<const Trk::TrackStateOnSurface>::const_iterator lastIDwP  = inputTSOS_ID->end();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it        = inputTSOS_ID->end()-1;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itFront   = inputTSOS_ID->begin();
  while(*it) {
    if(this->getVolumeByGeo(*it)==1 && (*it)->trackParameters()) {
      lastIDwP = it;
      break;
    }
    if(it==itFront) break;
    --it;
  }
  
  // find first MS TSOS
  DataVector<const Trk::TrackStateOnSurface>::const_iterator firstMS   = inputTSOS_MS->end();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator firstMSwP = inputTSOS_MS->end();
  it = inputTSOS_MS->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = inputTSOS_MS->end();
  for(; it!=itEnd; ++it) {
    if(this->getVolumeByGeo(*it)==3) {// && !(*it)->type(Trk::TrackStateOnSurface::Perigee)) {
      if(firstMS==itEnd) 
	firstMS = it;
      if((*it)->trackParameters() && (*it)->trackParameters()->covariance()) {
	firstMSwP = it;
	break;
      }
    }
  }

  if(lastIDwP == inputTSOS_ID->end()) {
    ATH_MSG_WARNING("Unable to find last ID TSOS with Track Parameters");    
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    /** deprecated outputLevel()
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS_ID)
	      printTSOS(m, "DEBUG-ID-TSOS");
    }    
    **/
    return;
  }
  if(firstMS == inputTSOS_MS->end()) {
    ATH_MSG_WARNING("Unable to find first MS TSOS");    
    ATH_MSG_WARNING("Unable to update Calorimeter TSOS");
    /** deprecated outputLevel()
    if(outputLevel() >= MSG::VERBOSE) {
      for(auto m : *inputTSOS_MS)
	       printTSOS(m, "DEBUG-MS-TSOS");
    }
    **/
    return;
  }

  // check that first MS TSOS is not a PerigeeSurface
  //bool removeOoC = false;
  DataVector<const Trk::TrackStateOnSurface>::const_iterator firstMSnotPerigee = firstMS;
  if( (*firstMS)->type(Trk::TrackStateOnSurface::Perigee) && (firstMS+1)!=inputTSOS_MS->end()) {
    firstMSnotPerigee=firstMS+1;
    //removeOoC = true;
  }

#ifdef DEBUGON
  printTSOS(*lastIDwP,  "LAST IDwP");
  printTSOS(*firstMSnotPerigee,   "FIRST MS");
  if(firstMSwP != inputTSOS_MS->end())
    printTSOS(*firstMSwP, "FIRST MSwP");
  else
    ATH_MSG_WARNING("Unable to find first MS TSOS with Track Parameters!");  
#endif

  double Eloss = 0.; 
  double X0ScaleMS = 0.;
  double ElossScaleMS = 0.;
  // get calorimeter TSOS from TG
  DataVector<const Trk::TrackStateOnSurface>* caloTSOS = this->getCaloTSOS (*(*lastIDwP)->trackParameters(),
									    // idTrack,
									    m_magFieldSvc->toroidOn() ? msTrack : idTrack,
									    (*firstMSnotPerigee)->surface(),
									    Trk::alongMomentum, 
									    Trk::muon,
                                                                            Eloss, X0ScaleMS, ElossScaleMS,
									    (firstMSwP == inputTSOS_MS->end()) ? 0 : (*firstMSwP)->trackParameters(),
									    false,
									    true); 
  
  if(!caloTSOS || caloTSOS->size()!=3) {
    if((!m_magFieldSvc->toroidOn()&&fabs(idTrack.perigeeParameters()->parameters()[Trk::qOverP])*4000.<1)|| (m_magFieldSvc->toroidOn()&&msTrack.perigeeParameters()->parameters()[Trk::qOverP]!=1/100000.&&msTrack.perigeeParameters()->parameters()[Trk::qOverP]!=0)) {
// Warnings only for high momentum ID tracks and MS tracks that have measured curvature (Straight track has pq= 100000)
      if(!m_magFieldSvc->toroidOn()) ATH_MSG_WARNING(" Toroid off q*momentum of ID track " << 1./idTrack.perigeeParameters()->parameters()[Trk::qOverP]);
      if(m_magFieldSvc->toroidOn()) ATH_MSG_WARNING(" Toroid on q*momentum of MS track " << 1./msTrack.perigeeParameters()->parameters()[Trk::qOverP]);
      ATH_MSG_WARNING("Unable to retrieve Calorimeter TSOS from extrapolateM+aggregation (null or !=3)");
      if(!caloTSOS) {
         ATH_MSG_WARNING(" Zero Calorimeter TSOS from extrapolateM+aggregation");
      } else {
         ATH_MSG_WARNING(" nr Calorimeter TSOS from extrapolateM+aggregation not equal to 3 " << caloTSOS->size());
      }
    }    
    if(caloTSOS) deleteTSOS(caloTSOS);
    return;
  }

  for (unsigned int i=0;i<caloTSOS->size(); i++){
    const Trk::MaterialEffectsOnTrack *meot=dynamic_cast<const Trk::MaterialEffectsOnTrack *>((*caloTSOS)[i]->materialEffectsOnTrack());
    if (!meot) {
      throw std::logic_error("TrackStateOnSurface without material effects on track." );
    }
    double sintheta=std::sin((*caloTSOS)[i]->trackParameters()->parameters()[Trk::theta]);
    double qoverp=(*caloTSOS)[i]->trackParameters()->parameters()[Trk::qOverP];
    const CaloEnergy *eloss=0;
    if (meot) eloss=dynamic_cast<const CaloEnergy *>(meot->energyLoss());
    
    Trk::EnergyLoss *neweloss=0;
    Trk::ScatteringAngles *newsa=0;
    if (eloss) neweloss = new CaloEnergy(*eloss);
    else{
      Trk::MaterialProperties matprop(meot->thicknessInX0(),1.,0.,0.,0.,0.);
      double sigmascat = fabs(qoverp)>0.0 ? std::sqrt(m_scattool->sigmaSquare(matprop,std::abs(1./qoverp),1.,Trk::muon)) : 0.0;
      newsa=new Trk::ScatteringAngles(0,0,sigmascat/sintheta,sigmascat);
    }
    Trk::MaterialEffectsOnTrack newmeot(meot->thicknessInX0(),newsa,neweloss,(*caloTSOS)[i]->trackParameters()->associatedSurface());
    calomeots.push_back(newmeot);
    delete (*caloTSOS)[i];
  }
  delete caloTSOS; 
    
  return;
}



// Get Calorimeter TSOS from TG (extrapolateM)
std::vector<const Trk::TrackStateOnSurface*>*
Trk::TrkMaterialProviderTool::getCaloTSOS (const Trk::TrackParameters&	parm, const Trk::Track& muonTrack,
					   const Trk::TrackParameters* parms) const
{
  std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = new std::vector<const Trk::TrackStateOnSurface*>();
  const Trk::TrackingVolume* targetVolume;
  Trk::PropDirection dir;

  const Trk::TrackingGeometry* trackingGeometry =  m_trackingGeometrySvc->trackingGeometry();
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
      DataVector<const Trk::TrackStateOnSurface>::const_iterator it = muonTrack.trackStateOnSurfaces()->begin();
      DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = muonTrack.trackStateOnSurfaces()->end();
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
    DataVector<const Trk::TrackStateOnSurface>* caloTSOSdv = this->getCaloTSOS (parm,
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
    
  } else {

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
      DataVector<const Trk::TrackStateOnSurface>* caloTSOSdv = this->getCaloTSOS (parm,
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
  }

  return caloTSOS;
}



// Get Calorimeter TSOS from TG (extrapolateM)
DataVector<const Trk::TrackStateOnSurface>* 
Trk::TrkMaterialProviderTool::getCaloTSOS (const Trk::TrackParameters&	parm,
					   const Trk::Track&            muonTrack,
					   const Trk::Surface&		surf,
					   Trk::PropDirection		dir,
					   Trk::ParticleHypothesis	mateffects,
                                           double&                      Eloss,
                                           double&                      X0ScaleMS,
                                           double&                      ElossScaleMS,
					   const Trk::TrackParameters*	parms,
					   bool                         boundaryCheck,
					   bool                         removeOoC) const 
{

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
  const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS = m_muonExtrapolator->extrapolateM(parm, surf, dir, boundaryCheck, mateffects);
  
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM, no-removal");

#ifdef DEBUGON
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM, no-removal");
  for(auto m : *caloTSOS) this->printTSOS(m, "BEFORE-REMOVAL CALO TSOS");
#endif
  

  Eloss = 0.; 
  double ElossID = 0.; 
  double ElossCalo = 0.; 
  double ElossMS = 0.; 
  for(auto m : *caloTSOS) {
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
          Eloss += fabs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==1) ElossID   += fabs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==2) ElossCalo += fabs(energyLoss->deltaE());
          if(this->getVolumeByGeo(m)==3) ElossMS   += fabs(energyLoss->deltaE());
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
  if(removeOoC && !caloTSOS->empty()) removeOutOfCalo(const_cast<std::vector<const Trk::TrackStateOnSurface*>*>(caloTSOS));
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM");
// remove MS TSOSs
  if(fremoveMS && !caloTSOS->empty()) removeMS(const_cast<std::vector<const Trk::TrackStateOnSurface*>*>(caloTSOS));
  ATH_MSG_DEBUG("Retrieved " << caloTSOS->size() << " Calorimeter TSOS from extrapolateM");
  
#ifdef DEBUGON
  for(auto m : *caloTSOS) this->printTSOS(m, "ORIGINAL CALO TSOS");
#endif


  DataVector<const Trk::TrackStateOnSurface>*  finalCaloTSOS = 0;
  if(caloTSOS->size()<1||Eloss<1000.) {
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
    double OneOverP = fabs(parm.parameters()[Trk::qOverP]);
    if (OneOverP > 0.0)
      pAtCaloEntry = 1./OneOverP;
    else
      pAtCaloEntry = parm.momentum().norm();    
  }else{
    const Trk::TrackStateOnSurface* tsosAtCaloEntry = this->getCaloEntryTSOS(caloTSOS, dir);
    if(!tsosAtCaloEntry) {
      ATH_MSG_WARNING( name() << " Unable to find Calorimeter Entry TSOS with TrackParameters! Momentum at Calo Entry not available!" );        
    }else{
      double OneOverP = fabs(tsosAtCaloEntry->trackParameters()->parameters()[Trk::qOverP]);
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
    if( fabs(parms->parameters()[Trk::qOverP]) > 0.0 ) {
      double pAtMuonEntry = fabs(1./parms->parameters()[Trk::qOverP]);
      if(!parms->covariance()) {
	ATH_MSG_DEBUG("MS track parameters without covariance, using 10% relative error!" );
	pAtMuonEntryError = pAtMuonEntry*0.1;
      }else{
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
      if(muonTrack.trackParameters() && muonTrack.trackParameters()->size()>0)
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
      ATH_MSG_VERBOSE("Energy measurement from calorimeter: outputs measCaloEnergy, measCaloEnergyError, fsrCaloEnergy, e_exp, E_em_meas, E_em_exp, E_tile_meas, E_tile_exp, E_HEC_meas, E_HEC_exp, E_dead_exp " << measCaloEnergy << " " << measCaloEnergyError << " " << fsrCaloEnergy << " " << e_exp << " " << E_em_meas << " " << E_em_exp << " " << E_tile_meas << " " << E_tile_exp << " " << E_HEC_meas << " " << E_HEC_exp << " " << E_dead_exp);

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
  if(!isIsolatedTrack(eta,phi)) useMeasuredEnergy = false;
 
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

CaloEnergy* Trk::TrkMaterialProviderTool::getParamCaloELoss(Trk::Track* track)
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
	    }else
	      return caloEnergy->clone();
	  }
	}
      }
    }
  }
  return (CaloEnergy*)(0x0);
}


//////////////////////////////////////////////// Privé //////////////////////////////////////////////////////
 
/**Helper to get last calo TSOS with TP*/
const Trk::TrackStateOnSurface* Trk::TrkMaterialProviderTool::getCaloExitTSOS(const std::vector<const Trk::TrackStateOnSurface*>* caloTSOS, Trk::PropDirection dir) const
{
  const Trk::TrackStateOnSurface* tsosAtCaloExit = 0;
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
  const Trk::TrackStateOnSurface* tsosAtCaloEntry = 0;
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
                                    else {
                                      return false;
                                    }
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
                                    else {
                                      return false;
                                    }
                                  } ), 
                   caloTSOS->end());

}


/** Helper to update entries in the vector*/
void Trk::TrkMaterialProviderTool::updateVector(DataVector<const Trk::TrackStateOnSurface>* inputTSOS, 
						DataVector<const Trk::TrackStateOnSurface>::iterator firstCALO, 
						DataVector<const Trk::TrackStateOnSurface>::iterator firstMS, 
						DataVector<const Trk::TrackStateOnSurface>* caloTSOS)
{
  //printTSOS(*firstCALO, "UPD->FIRST CALO");
  //printTSOS(*firstMS, "UPD->FIRST MS");
  unsigned int ntoupdate=0;
  DataVector<const Trk::TrackStateOnSurface>::iterator it = firstCALO;
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
      firstMS--;
    }    
    inputTSOS->insert(firstMS, caloTSOS->begin(), caloTSOS->end());
  }
  // delete the view container
  delete caloTSOS;
}
void Trk::TrkMaterialProviderTool::updateVectorMS(DataVector<const Trk::TrackStateOnSurface>* inputTSOS, 
						  DataVector<const Trk::TrackStateOnSurface>::iterator firstMS, 
                                                  double X0ScaleMS, double ElossScaleMS) 
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

  DataVector<const Trk::TrackStateOnSurface>::iterator it = firstMS;
  int msStates = 0;
  int msMatStates = 0;
  int msMatParStates = 0;

// In the MuonSpectrometer the TSOS for the MaterialEffectsOnTrack do NOT have trackParameters

  for(;it!= inputTSOS->end();it++) {
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
          const Trk::ScatteringAngles *scatNew =new Trk::ScatteringAngles(0,0,sqrt(X0ScaleMS)*scatAngles->sigmaDeltaTheta(),sqrt(X0ScaleMS)*scatAngles->sigmaDeltaPhi());
          double deltaE           = (ElossScaleMS*energyLoss->deltaE());
          double sigmaDeltaE      = (ElossScaleMS*energyLoss->sigmaDeltaE());
          double sigmaPlusDeltaE  = (ElossScaleMS*energyLoss->sigmaPlusDeltaE());
          double sigmaMinusDeltaE = (ElossScaleMS*energyLoss->sigmaMinusDeltaE());
          double deltaE_ioni      = (ElossScaleMS*energyLoss->meanIoni());
          double sigmaDeltaE_ioni = (ElossScaleMS*energyLoss->sigmaIoni());
          double deltaE_rad       = (ElossScaleMS*energyLoss->meanRad());
          double sigmaDeltaE_rad  = (ElossScaleMS*energyLoss->sigmaRad());
          double depth = energyLoss->length();

          if(debug) std::cout << " updateVectorMS Old Eloss " <<  energyLoss->deltaE() << " new Eloss " << deltaE << std::endl;

          Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE, sigmaDeltaE, sigmaMinusDeltaE, sigmaPlusDeltaE, deltaE_ioni, sigmaDeltaE_ioni, deltaE_rad, sigmaDeltaE_rad, depth ) ;
          const Trk::Surface& surf = meot->associatedSurface();
          const Trk::MaterialEffectsOnTrack*  newMeot =  new Trk::MaterialEffectsOnTrack(X0ScaleMS*meot->thicknessInX0(), scatNew, energyLossNew, surf, meotPattern);
          const Trk::TrackParameters* pars = 0;
          if((*it)->trackParameters()) pars = (*it)->trackParameters()->clone();
	// make new TSOS 
          const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface( 0, pars, 0, newMeot, typePattern );
          DataVector<const Trk::TrackStateOnSurface>* newTSOSvector = new DataVector<const Trk::TrackStateOnSurface>(SG::VIEW_ELEMENTS);
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
   for(it = firstMS;it!= inputTSOS->end();it++) {
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

  return;

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
void Trk::TrkMaterialProviderTool::deleteTSOS(const std::vector<const Trk::TrackStateOnSurface*>* vecTSOS) const
{
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator it = vecTSOS->begin();
  std::vector<const Trk::TrackStateOnSurface*>::const_iterator itEnd = vecTSOS->end();
  for (; it != itEnd; ++it) delete *it;
  delete vecTSOS;
}
void Trk::TrkMaterialProviderTool::deleteTSOS(DataVector<const Trk::TrackStateOnSurface>* vecTSOS) const
{
  if(vecTSOS->ownPolicy()==SG::VIEW_ELEMENTS) {
    DataVector<const Trk::TrackStateOnSurface>::const_iterator it = vecTSOS->begin();
    DataVector<const Trk::TrackStateOnSurface>::const_iterator itEnd = vecTSOS->end();
    for (; it != itEnd; ++it) delete *it;
  }
  delete vecTSOS;
}


//** Helper to printout TSOS details*/
void Trk::TrkMaterialProviderTool::printTSOS(const Trk::TrackStateOnSurface* m, std::string tag) const
{
  unsigned int ivolGeo = getVolumeByGeo(m);

  std::string volGeo="ID";
  if(ivolGeo==2) volGeo="CALO";
  else if(ivolGeo==3) volGeo="MS";
  
  ATH_MSG_VERBOSE(tag<<" Type "<<std::left<<std::setw(35)<<m->dumpType()
		<<" Volume "<<std::left<<std::setw(5)<<volGeo
		//<<" Surface "<<std::left<<std::setw(5)<<m->surface()
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
		  //ATH_MSG_VERBOSE(" qOverP "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::qOverP];
		  <<" phi0 "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::phi0]
		  //ATH_MSG_VERBOSE(" d0 "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::d0];
		  //ATH_MSG_VERBOSE(" z0 "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::z0];
		  //ATH_MSG_VERBOSE(" theta "<<std::left<<std::setw(10)<<m->trackParameters()->parameters()[Trk::theta];      
		  <<" eta "<<std::left<<std::setw(10)<<-log(tan(m->trackParameters()->parameters()[Trk::theta]/2)));      
    if(m->trackParameters()->covariance()) {
      ATH_MSG_VERBOSE(" -> Errors: qOverP "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::qOverP)
		    <<" phi0 "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::phi0)
		    <<" theta "<<std::left<<std::setw(10)<<Amg::error(*m->trackParameters()->covariance(),Trk::theta));
    }		     
  }
}

/** Function to modify TSOS doing repositioning, aggregation and corrections*/
DataVector<const Trk::TrackStateOnSurface>* 
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
  DataVector<const Trk::TrackStateOnSurface>* newTSOSvector = new DataVector<const Trk::TrackStateOnSurface>(SG::VIEW_ELEMENTS);
  // int maxsize = 2*matvec.size();
  // if(aggregate) maxsize = 2;
  // newTSOSvector.reserve(maxsize);
  //
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

  const Trk::TrackStateOnSurface* mprevious = 0;
  const Trk::TrackStateOnSurface* mfirst = 0;
  const Trk::TrackStateOnSurface* mlast = 0;
  Amg::Vector3D posFirst(0.,0.,0.);
  //   double distLast = 0.;
  double deltaEFirst = 0.;

  double deltaPhi = 0.; 
  double deltaTheta = 0.;

  double depth_tot = 0.;
  int n_tot = 0; 

  double w_tot = 0.; 
  double wdist2 = 0.;
  Amg::Vector3D wdir(0.,0.,0.);
  Amg::Vector3D wpos(0.,0.,0.);

  std::bitset<Trk::MaterialEffectsBase::NumberOfMaterialEffectsTypes> meotPattern(0);
  meotPattern.set(Trk::MaterialEffectsBase::EnergyLossEffects);
  meotPattern.set(Trk::MaterialEffectsBase::ScatteringEffects);
  // meotPattern.set(Trk::MaterialEffectsBase::FittedMaterialEffects);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePattern(0);
  typePattern.set(Trk::TrackStateOnSurface::InertMaterial);
  typePattern.set(Trk::TrackStateOnSurface::Scatterer);

  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternScat(0);
  typePatternScat.set(Trk::TrackStateOnSurface::Scatterer);
  
  std::bitset<Trk::TrackStateOnSurface::NumberOfTrackStateOnSurfaceTypes> typePatternDeposit(0);
  //typePatternDeposit.set(Trk::TrackStateOnSurface::InertMaterial);
  typePatternDeposit.set(Trk::TrackStateOnSurface::CaloDeposit);
  //typePatternDeposit.set(Trk::TrackStateOnSurface::Scatterer);

  for(auto m : *matvec) {

    if(!m->trackParameters()) {
      ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
      continue;
    }
    if(m->materialEffectsOnTrack()) {
      double X0 = m->materialEffectsOnTrack()->thicknessInX0();
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
      const Trk::EnergyLoss* energyLoss = 0;
      const Trk::ScatteringAngles* scat = 0;
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
      double dotprod = m->trackParameters()->position().unit().x()*m->trackParameters()->momentum().unit().x()+ m->trackParameters()->position().unit().y()*m->trackParameters()->momentum().unit().y() +
                       m->trackParameters()->position().unit().z()*m->trackParameters()->momentum().unit().z();
      ATH_MSG_VERBOSE(" ");
      ATH_MSG_VERBOSE(" original TSOS type " << m->dumpType() << " TSOS surface " << m->trackParameters()->associatedSurface() << " position x " << m->trackParameters()->position().x() << " y " << m->trackParameters()->position().y() << " z " << m->trackParameters()->position().z()   << " direction x " << m->trackParameters()->momentum().unit().x() << " y " << m->trackParameters()->momentum().unit().y() << " z " << m->trackParameters()->momentum().unit().z() << " p " << m->trackParameters()->momentum().mag() << " X0 " << X0 << " deltaE " << energyLoss->deltaE() << " meanIoni " << energyLoss->meanIoni() << " sigmaIoni " << energyLoss->sigmaIoni() << " sigma deltaTheta " <<  scat->sigmaDeltaTheta() << " depth " << depth  << " dotprod " << dotprod );
 
      X0_tot += fabs(scaleX0*X0);

      sigmaDeltaTheta2_tot += scaleX0*scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();
      sigmaDeltaPhi2_tot   += scaleX0*scat->sigmaDeltaPhi()*scat->sigmaDeltaPhi();

      // Eloss sigma values add up linearly for Landau and exponential distributions       
      deltaE_tot           += fabs(scaleEloss*energyLoss->deltaE());
      sigmaDeltaE_tot      += fabs(scaleEloss*energyLoss->sigmaDeltaE());
      sigmaPlusDeltaE_tot  += fabs(scaleEloss*energyLoss->sigmaPlusDeltaE());
      sigmaMinusDeltaE_tot += fabs(scaleEloss*energyLoss->sigmaMinusDeltaE());
      deltaE_ioni_tot      += fabs(scaleEloss*energyLoss->meanIoni()); 
      sigmaDeltaE_ioni_tot += fabs(scaleEloss*energyLoss->sigmaIoni()); 
      deltaE_rad_tot       += fabs(scaleEloss*energyLoss->meanRad()); 
      sigmaDeltaE_rad_tot  += fabs(scaleEloss*energyLoss->sigmaRad());

      depth_tot += depth;
      n_tot++;

      Amg::Vector3D dir = m->trackParameters()->momentum().unit();
      Amg::Vector3D pos = m->trackParameters()->position();
      if(mprevious) {
	dir += mprevious->trackParameters()->momentum().unit();
      } 

      dir = dir/dir.mag();  
      ATH_MSG_VERBOSE(" position at end " << pos.x() << " y " << pos.y() << " z " << pos.z() << " perp " << pos.perp() );
      ATH_MSG_VERBOSE(" direction x " << dir.x() << " y " << dir.y() << " z " << dir.z());
      Amg::Vector3D pos0   = pos - (depth/2.+depth/sqrt(12.))*dir;
      Amg::Vector3D posNew = pos - (depth/2.-depth/sqrt(12.))*dir;

      ATH_MSG_VERBOSE(" position scattering centre0 x " << pos0.x() << " y " << pos0.y() << " z " << pos0.z() << " perp " << pos0.perp() );
      ATH_MSG_VERBOSE(" position scattering centre1 x " << posNew.x() << " y " << posNew.y() << " z " << posNew.z() << " perp " << posNew.perp() << " distance " << (pos0-posNew).mag() << " depth " << depth);  
      if(!mfirst) {
        mfirst = m;
        posFirst = pos0;
        deltaEFirst = energyLoss->deltaE(); 
      } 
      mlast = m;

      //bool use_eweight = false;
      double w = scat->sigmaDeltaTheta()*scat->sigmaDeltaTheta();
      // use_eweight s false, so the following never executes
      //if(use_eweight) w = fabs(scaleEloss*energyLoss->deltaE());
      w_tot += w; 
      wpos += w*pos0/2.; 
      wpos += w*posNew/2.; 
      wdir += w*dir; 

      wdist2 += w*(pos0-posFirst).mag2()/2.;
      wdist2 += w*(posNew-posFirst).mag2()/2.;
      //      distLast = (posNew-posFirst).mag();

      if (!aggregate&&!reposition) {
	
        const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot),sqrt(sigmaDeltaTheta2_tot));
        Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, depth ) ;
	const CaloEnergy* caloEnergyNew = new CaloEnergy(*energyLossNew);
	delete energyLossNew;
        const Trk::Surface& surf = meot->associatedSurface();
        const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot, scatNew, caloEnergyNew, surf, meotPattern);
        const Trk::TrackParameters* pars = m->trackParameters()->clone();

	// make new TSOS 
        const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface( 0, pars, 0, meotLast, typePattern );

        newTSOSvector->push_back(newTSOS);
        ATH_MSG_VERBOSE(" NO aggregation and NO reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << X0_tot << " deltaE " << caloEnergyNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

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

        if(fabs(depth)<10.) {

	  //        Thin scatterer: make only one TSOS 

          const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot),sqrt(sigmaDeltaTheta2_tot));
	  Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, depth ) ;
	  const CaloEnergy* caloEnergyNew = new CaloEnergy(*energyLossNew);
	  delete energyLossNew;
          const Trk::Surface& surf = meot->associatedSurface();
          const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot, scatNew, caloEnergyNew, surf, meotPattern);
          const Trk::TrackParameters* pars = m->trackParameters()->clone();
	  //        make new TSOS 
          const Trk::TrackStateOnSurface* newTSOS = new Trk::TrackStateOnSurface( 0, pars, 0, meotLast, typePattern );
          newTSOSvector->push_back(newTSOS);
          ATH_MSG_VERBOSE(" NO aggregation and WITH reposition Thin Scatterer  TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << caloEnergyNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

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
          const Trk::ScatteringAngles* scatFirst = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));

	  //        prepare for second MaterialEffectsOnTrack with X0 =  X0/2 Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
          const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));
	  Trk::EnergyLoss* energyLossNew = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.) ;
	  const CaloEnergy* caloEnergyNew = new CaloEnergy(*energyLossNew);
	  delete energyLossNew;
          double norm = dir.perp();          
	  //        Rotation matrix representation 
          Amg::Vector3D colx(-dir.y()/norm,        dir.x()/norm,           0);
          Amg::Vector3D coly(-dir.x()*dir.z()/norm, -dir.y()*dir.z()/norm, norm);
          Amg::Vector3D colz( dir.x(),             dir.y(),                dir.z());
        
          Amg::Transform3D* surfaceTransformFirst = new Amg::Transform3D(colx,coly,colz,pos0);
          Amg::Transform3D* surfaceTransformLast = new Amg::Transform3D(colx,coly,colz,posNew);
          Trk::PlaneSurface* surfFirst = new Trk::PlaneSurface( surfaceTransformFirst );
          Trk::PlaneSurface* surfLast = new Trk::PlaneSurface( surfaceTransformLast );
	  //        make MaterialEffectsOnTracks 
          const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   energyLoss0,   *surfFirst,   meotPattern);
          const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, caloEnergyNew, *surfLast, meotPattern);

	  //        calculate TrackParameters at first surface 
          double qOverP0 = m->trackParameters()->charge()/ (m->trackParameters()->momentum().mag()-fabs(energyLoss->deltaE())); 
          if(mprevious) qOverP0 = mprevious->trackParameters()->charge()/mprevious->trackParameters()->momentum().mag();
          const Trk::TrackParameters* parsFirst  = surfFirst->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverP0);
	  //        calculate TrackParameters at second surface 
          double qOverPNew = m->trackParameters()->charge()/m->trackParameters()->momentum().mag();
          const Trk::TrackParameters* parsLast  = surfLast->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);
	  // make TSOS 
	  //      
          const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePattern );
          const Trk::TrackStateOnSurface* newTSOS  = new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePattern );

          ATH_MSG_VERBOSE(" first NO aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE " << energyLoss0->deltaE() << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
          ATH_MSG_VERBOSE(" second NO aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << caloEnergyNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

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

	  delete surfFirst;
	  delete surfLast;

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
      //if(fabs(X0_tot)>50 && fabs(pos.z())<6700 && pos.perp()<4200) threePlanes = true; 
      if(fabs(pos.z())<6700 && pos.perp()<4200) threePlanes = true; // always 3 planes in calo
      //
      const Trk::ScatteringAngles* scatFirst = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));
      const Trk::ScatteringAngles* scatNew = new ScatteringAngles(deltaPhi,deltaTheta,sqrt(sigmaDeltaPhi2_tot/2.),sqrt(sigmaDeltaTheta2_tot/2.));
      Trk::EnergyLoss* energyLoss2 = new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.) ;
      if(threePlanes) ATH_MSG_VERBOSE(" Calorimeter energyLoss2 delta E "  << energyLoss2->deltaE() << " meanIoni " << energyLoss2->meanIoni() << " sigmaIoni " << energyLoss2->sigmaIoni() << " X0_tot " << X0_tot ); 

      int elossFlag = 0; // return Flag for updateEnergyLoss Calorimeter energy (0 = not used) 

      double calE = caloEnergy;
      double calEr = caloEnergyError;

//      if(!useMeasuredEnergy) calE = 0.;
      if(!useMeasuredEnergy) calEr = 0.;
 
      Trk::EnergyLoss* energyLossNew = (updateEloss ? m_elossupdator->updateEnergyLoss(energyLoss2, calE, calEr, pCaloEntry, momentumError, elossFlag):  new EnergyLoss(deltaE_tot, sigmaDeltaE_tot, sigmaMinusDeltaE_tot, sigmaPlusDeltaE_tot, deltaE_ioni_tot, sigmaDeltaE_ioni_tot, deltaE_rad_tot, sigmaDeltaE_rad_tot, 0.)); 
      CaloEnergy* caloEnergyNew = new CaloEnergy(*energyLossNew);
      if(threePlanes) ATH_MSG_VERBOSE(" After update Calorimeter energyLossNew " << energyLossNew->deltaE() << " meanIoni " << energyLossNew->meanIoni() << " sigmaIoni " << energyLossNew->sigmaIoni());
 
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
      double scaleCaloNew = fabs(pos0.z())/6700; 
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = fabs(posNew.z())/6700; 
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = fabs(pos0.perp())/4200; 
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      scaleCaloNew = fabs(posNew.perp())/4200; 
      if(scaleCaloNew>scaleCalo) scaleCalo = scaleCaloNew;
      
      if(scaleCalo>1.) {
        pos0       = pos0/scaleCalo;
        pos        = pos/scaleCalo;
        posNew     = posNew/scaleCalo;
        halflength = halflength/scaleCalo;
        ATH_MSG_VERBOSE(" position scattering planes inside calo scale factor " << scaleCalo);
      }

      if(updateEloss) ATH_MSG_VERBOSE("WITH updateEloss");
  
      ATH_MSG_VERBOSE(" WITH aggregation and WITH reposition center planes x " << pos.x() << " y " << pos.y() << " z " << pos.z() << " halflength " << halflength << " w_tot " << w_tot << " X0_tot " << X0_tot ); 
 
      Amg::Transform3D* surfaceTransformFirst = new Amg::Transform3D(colx,coly,colz,pos0);
      Amg::Transform3D* surfaceTransformLast = new Amg::Transform3D(colx,coly,colz,posNew);
      Trk::PlaneSurface* surfFirst = new Trk::PlaneSurface( surfaceTransformFirst );
      Trk::PlaneSurface* surfLast = new Trk::PlaneSurface( surfaceTransformLast );
      //        calculate TrackParameters at first surface 
      double qOverP0 = mfirst->trackParameters()->charge()/(mfirst->trackParameters()->momentum().mag()+fabs(deltaEFirst)); 
      //        calculate TrackParameters at last surface 
      double qOverPNew = mlast->trackParameters()->charge()/mlast->trackParameters()->momentum().mag();
      const Trk::TrackParameters* parsFirst  = surfFirst->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverP0);
      const Trk::TrackParameters* parsLast  = surfLast->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);

      if(!threePlanes) {
        //
        // make two scattering planes and TSOS 
        //
        //          prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
        const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   0,   *surfFirst,   meotPattern);
        //          prepare for second MaterialEffectsOnTrack with X0 =  X0/2 Eloss = Eloss total and scattering2 = total2 / 2. depth = 0
        const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, caloEnergyNew, *surfLast, meotPattern);
        //
        //      
        const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePattern );
        const Trk::TrackStateOnSurface* newTSOS  =  (elossFlag!=0 ?  new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePatternDeposit ) : new Trk::TrackStateOnSurface( 0, parsLast,   0, meotLast,   typePattern )) ;
        ATH_MSG_VERBOSE(" first WITH aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE 0 " << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
        ATH_MSG_VERBOSE(" second WITH aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << caloEnergyNew->deltaE() << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );
        newTSOSvector->push_back(newTSOSFirst);
        newTSOSvector->push_back(newTSOS);
      } else {
        //
        // make three scattering planes and TSOS in Calorimeter 
        //
        Amg::Transform3D* surfaceTransform = new Amg::Transform3D(colx,coly,colz,pos);
        Trk::PlaneSurface* surf = new Trk::PlaneSurface( surfaceTransform );
        const Trk::TrackParameters* pars  = surf->createParameters<5,Trk::Charged>(0.,0.,dir.phi(),dir.theta(),qOverPNew);

        //        prepare for first MaterialEffectsOnTrack with X0 = X0/2 Eloss = 0 and scattering2 = total2 / 2. depth = 0
        const Trk::MaterialEffectsOnTrack*  meotFirst   =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatFirst,   0,   *surfFirst,   meotPattern);

        //        prepare for middle MaterialEffectsOnTrack with X0 =  0 Eloss = ElossNew and scattering2 = 0. depth = 0
        const Trk::MaterialEffectsOnTrack*  meot =  new Trk::MaterialEffectsOnTrack(0., 0, caloEnergyNew, *surf, meotPattern);

        //        prepare for last MaterialEffectsOnTrack with X0 =  X0/2 Eloss = 0 total and scattering2 = total2 / 2. depth = 0
        const Trk::MaterialEffectsOnTrack*  meotLast =  new Trk::MaterialEffectsOnTrack(X0_tot/2., scatNew, 0, *surfLast, meotPattern);

        const Trk::TrackStateOnSurface* newTSOSFirst = new Trk::TrackStateOnSurface( 0, parsFirst,     0, meotFirst,   typePatternScat );
        const Trk::TrackStateOnSurface* newTSOS  =    new Trk::TrackStateOnSurface( 0, pars,   0, meot,   typePatternDeposit );
        const Trk::TrackStateOnSurface* newTSOSLast = new Trk::TrackStateOnSurface( 0, parsLast,     0, meotLast,   typePatternScat );

        newTSOSvector->push_back(newTSOSFirst);
        newTSOSvector->push_back(newTSOS);
        newTSOSvector->push_back(newTSOSLast);
        ATH_MSG_VERBOSE(" first WITH aggregation and WITH reposition   TSOS type " << newTSOSFirst->dumpType() << " TSOS surface " << newTSOSFirst->trackParameters()->associatedSurface() << " position x " << newTSOSFirst->trackParameters()->position().x() << " y " << newTSOSFirst->trackParameters()->position().y() << " z " << newTSOSFirst->trackParameters()->position().z()   << " direction x " << newTSOSFirst->trackParameters()->momentum().unit().x() << " y " << newTSOSFirst->trackParameters()->momentum().unit().y() << " z " << newTSOSFirst->trackParameters()->momentum().unit().z() << " p " << newTSOSFirst->trackParameters()->momentum().mag() << " X0 " << meotFirst->thicknessInX0()  << " deltaE 0 " << " sigma deltaTheta " <<  scatFirst->sigmaDeltaTheta()  );
        ATH_MSG_VERBOSE(" second WITH aggregation and WITH reposition   TSOS type " << newTSOS->dumpType() << " TSOS surface " << newTSOS->trackParameters()->associatedSurface() << " position x " << newTSOS->trackParameters()->position().x() << " y " << newTSOS->trackParameters()->position().y() << " z " << newTSOS->trackParameters()->position().z()   << " direction x " << newTSOS->trackParameters()->momentum().unit().x() << " y " << newTSOS->trackParameters()->momentum().unit().y() << " z " << newTSOS->trackParameters()->momentum().unit().z() << " p " << newTSOS->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE " << caloEnergyNew->deltaE() << " meanIoni " << caloEnergyNew->meanIoni() << " sigmaIoni " << caloEnergyNew->sigmaIoni() << " sigma deltaTheta 0 " );
        ATH_MSG_VERBOSE(" third WITH aggregation and WITH reposition   TSOS type " << newTSOSLast->dumpType() << " TSOS surface " << newTSOSLast->trackParameters()->associatedSurface() << " position x " << newTSOSLast->trackParameters()->position().x() << " y " << newTSOSLast->trackParameters()->position().y() << " z " << newTSOSLast->trackParameters()->position().z()   << " direction x " << newTSOSLast->trackParameters()->momentum().unit().x() << " y " << newTSOSLast->trackParameters()->momentum().unit().y() << " z " << newTSOSLast->trackParameters()->momentum().unit().z() << " p " << newTSOSLast->trackParameters()->momentum().mag() << " X0 " << meotLast->thicknessInX0() << " deltaE 0 " << " sigma deltaTheta " <<  scatNew->sigmaDeltaTheta()  );

        delete surf;

      }

      delete surfFirst;
      delete surfLast;

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

  for(auto m : *matvec) {    
    if(!m->trackParameters()) {
      ATH_MSG_WARNING("No trackparameters on TrackStateOnSurface ");
      continue;
    }
    if(m->materialEffectsOnTrack()) {
      const Trk::MaterialEffectsOnTrack* meot = dynamic_cast<const Trk::MaterialEffectsOnTrack*>(m->materialEffectsOnTrack());
      const Trk::EnergyLoss* energyLoss = 0;
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
      deltaE_tot           += fabs(scaleEloss*energyLoss->deltaE());
      sigmaDeltaE_tot      += fabs(scaleEloss*energyLoss->sigmaDeltaE());
      sigmaPlusDeltaE_tot  += fabs(scaleEloss*energyLoss->sigmaPlusDeltaE());
      sigmaMinusDeltaE_tot += fabs(scaleEloss*energyLoss->sigmaMinusDeltaE());
      deltaE_ioni_tot      += fabs(scaleEloss*energyLoss->meanIoni()); 
      sigmaDeltaE_ioni_tot += fabs(scaleEloss*energyLoss->sigmaIoni()); 
      deltaE_rad_tot       += fabs(scaleEloss*energyLoss->meanRad()); 
      sigmaDeltaE_rad_tot  += fabs(scaleEloss*energyLoss->sigmaRad());

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
  std::pair<int,double> inner = m_trackIsolationTool->trackIsolation(eta,phi);
  nTracks 	= inner.first;

  ATH_MSG_VERBOSE("Isolation : Number of tracks in cone " << nTracks << " cut < " << m_maxNTracksIso);

  if (nTracks > m_maxNTracksIso) return false;
  return true;
}

/** Function to get calorimeter measured energy loss*/
double Trk::TrkMaterialProviderTool::getCaloMeasuredEnergy(double eta, double phi, 
							   double totalEloss,
							   double meanElossIoni,
							   double& fsrCaloEnergy) const
{
  // Retrieve the measured energy from calorimeter
  Rec::CaloMeas* caloMeas = m_caloMeasTool->energyMeasurement(eta,phi,eta,phi);

  // Check isolation
//  if(!isIsolatedTrack(eta,phi)) {
//    delete caloMeas;
//    return 0.0;
//  }

  // Sum-up components
  double FinalMeasuredEnergy = getFinalMeasuredEnergy(caloMeas, totalEloss, meanElossIoni, eta, fsrCaloEnergy);
  delete caloMeas;

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
  if (fabs(eta)>2. && caloMeas->LArHEC_EnergyMeasured()>100.)
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
  if (fabs(eta)<1.4) {
    if (LArHECEnergy + TileEnergy > 0.1 * MopLoss * HECMaterial)	bHEC= true;
  }	
  else if (fabs(eta)>1.8) {
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
  // int ieta			=  static_cast<int> (fabs(eta)/0.10);
  // if (ieta > 25) ieta		=  25;
    
  double FinalMeasuredEnergy	= MeasCorrected + eOverMipCorrection;// + (fix1FromPeter[ieta] + fix2FromPeter[ieta])*CLHEP::GeV;
  
  ATH_MSG_VERBOSE( "Sum of cells "			<< (TileEnergy + EmEnergy + LArHECEnergy)
		   << " Total energy deposition "	<< TotalMeasuredEnergy
		   << " corrected energy deposition "	<< MeasCorrected
		   << " e/mip corre "			<< FinalMeasuredEnergy << std::endl
		   << "\nFinal Energy Measurement  = " << FinalMeasuredEnergy  
		   //<< "\nMean Energy Deposition    = " << MeanLoss/CLHEP::GeV
		   //<< " - " << MeanErrorLeft/CLHEP::GeV << " + "<< MeanErrorRight/CLHEP::GeV
		   << "\nMop Energy Deposition     = " << MopLoss //<< " +- " << MopError/CLHEP::GeV
		   //<< "\nOld parametrization energy= " << m_caloParamOld->deltaE()/CLHEP::GeV
		   //<< " +- " <<  m_caloParamOld->sigmaDeltaE()/CLHEP::GeV
		   //<< "\nTrack Momentum            = "  <<  trackMomentum/CLHEP::GeV
		   //<< " Eta= " << eta << "  Phi= " << phi
		   << std::endl
		   << "Final Meas = "			<< FinalMeasuredEnergy
		   << " Mop Dep = "			<< MopLoss);//<< " +- " << MopError/CLHEP::GeV );

  return FinalMeasuredEnergy;
}  
  
