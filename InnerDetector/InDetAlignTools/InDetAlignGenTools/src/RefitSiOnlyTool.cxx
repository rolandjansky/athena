/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// ================================================
// RefitSiOnly
// ================================================
//
// RefitSiOnlyTool.cxx
// Source file for RefitSiOnlyTool
//
// namespace InDetAlignment
//
// AlgTool to create refitted tracks with only silicon hits present

#include "DataModel/DataVector.h"

#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"

//Track-things
#include "TrkTrack/Track.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkPrepRawData/PrepRawData.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkParameters/TrackParameters.h"

#include "TrkFitterInterfaces/ITrackFitter.h" 
#include "TrkEventUtils/TrkParametersComparisonFunction.h"

#include "InDetPrepRawData/SiCluster.h"

#include "InDetAlignGenTools/RefitSiOnlyTool.h"

namespace InDetAlignment {
 
  RefitSiOnlyTool::RefitSiOnlyTool (const std::string& type, const std::string& name, const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_ITrkFitter("Trk::KalmanFitter"),
    m_comPar(0),
    m_ParticleHypothesis(Trk::nonInteracting)
  {
    declareInterface<ICollectionProcessor>(this);
    declareProperty("OutlierRemoval" , m_OutlierRemoval = true);
    declareProperty("FitterTool"     , m_ITrkFitter);
    declareProperty("ParticleNumber" , m_ParticleNumber = 2);
    declareProperty("Cosmic"         , m_doCosmic = false);
    declareProperty("StripPixelHits" , m_stripPixelHits = false);
    declareProperty("StripSCTHits"   , m_stripSCTHits   = false);
    declareProperty("StripTRTHits"   , m_stripTRTHits   = true);
  }
  //_________________________________________________________________________________________

  //destructor
  RefitSiOnlyTool::~RefitSiOnlyTool() {}
  //_________________________________________________________________________________________

  StatusCode RefitSiOnlyTool::initialize()
  {
    StatusCode sc;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Initialize() of RefitSiOnlyTool" << endreq;

    // make for cosmic tracks new reference point
    Amg::Vector3D refGP(0.,15000.,0.);
    m_comPar = new Trk::TrkParametersComparisonFunction(refGP);

    //ID Helper
    if (detStore()->retrieve(m_idHelper,"AtlasID").isFailure()) {
      if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Unable to initialize ID helper." << endreq;
      return StatusCode::FAILURE;
    }
    
    //Fitter
    if (m_ITrkFitter.retrieve().isFailure()) {
      if (msgLvl(MSG::FATAL)) msg(MSG::FATAL) << "Can not retrieve Fitter of type "
          << m_ITrkFitter.typeAndName() << endreq;
      return StatusCode::FAILURE;
    } else if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved tool " << m_ITrkFitter.typeAndName() << endreq;

    if (m_ParticleNumber == 0)       m_ParticleHypothesis = Trk::nonInteracting;
    else if (m_ParticleNumber == 1)  m_ParticleHypothesis = Trk::electron;
    else if (m_ParticleNumber == 2)  m_ParticleHypothesis = Trk::muon;
    else if (m_ParticleNumber == 3)  m_ParticleHypothesis = Trk::pion;
    else if (m_ParticleNumber == 4)  m_ParticleHypothesis = Trk::kaon;
    else if (m_ParticleNumber == 5)  m_ParticleHypothesis = Trk::proton;
    else if (m_ParticleNumber == 99) m_ParticleHypothesis = Trk::noHypothesis;
  
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "ParticleNumber: " << m_ParticleNumber << endreq;
      msg(MSG::DEBUG) << "ParticleHypothesis: " << m_ParticleHypothesis << endreq;    
      msg(MSG::DEBUG) << "Initialize() of RefitSiOnlyTool successful" << endreq;
    }

    return StatusCode::SUCCESS;
  }
  //_________________________________________________________________________________________

  StatusCode RefitSiOnlyTool::finalize()
  {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Finalize() of RefitSiOnlyTool" << endreq;
    return StatusCode::SUCCESS;
  }
  //_________________________________________________________________________________________
  StatusCode RefitSiOnlyTool::configure()
   {
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "nothing to configure - everything has already been configured in initialize()" << endreq;
     return StatusCode::SUCCESS;
   }
  //_________________________________________________________________________________________
  const DataVector<Trk::Track>* RefitSiOnlyTool::processTrackCollection(const DataVector<Trk::Track>* trks) {
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "processTrackCollection() of RefitSiOnlyTool" << endreq;

    m_trkindex=0;
    DataVector<Trk::Track>* newTrks = new DataVector<Trk::Track>;
    
    for (DataVector<Trk::Track>::const_iterator t=trks->begin(); t!=trks->end();++t) {
      if (*t) {
  Trk::Track* refittedTrack = refit(*t);
   
  if (refittedTrack==0)
          if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Refit of the track " << m_trkindex << " did not work. Track skipped." << endreq;

  newTrks->push_back(refittedTrack);
      } else {
        newTrks->push_back(0);
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Track " << m_trkindex << " is empty." << endreq;
      }
      
      m_trkindex++;
    }

    if (msgLvl(MSG::DEBUG)) {
       dumpTrackCol(newTrks);
    }

    return newTrks;
  }

  //_________________________________________________________________________________________
  Trk::Track* RefitSiOnlyTool::refit(const Trk::Track* tr) const {
    // Step through all hits on a Trk::Track, pushback SiliconHits into RIO_Collection
    // and Refit Track with SiliconHits
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Entering RefitSiOnlyTool::refit() for track #" << m_trkindex  << endreq;
    
    const Trk::Perigee* initialPerigee = tr->perigeeParameters();
    if (initialPerigee) {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Track # " << m_trkindex << " has momentum before refit: " 
                                              << initialPerigee->momentum().mag() / CLHEP::GeV << " CLHEP::GeV/c" << endreq;
    }
    else { if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Track # " << m_trkindex << " without perigee" << endreq; }

    Trk::Track* SiOnlyTrack = 0;
    bool containsGangedPixels = false;
    std::vector<const Trk::MeasurementBase*> MeasurementBase_Collection;

    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Building stripped RIO_Collection" << endreq;    
    for (DataVector<const Trk::MeasurementBase>::const_iterator measBase = tr->measurementsOnTrack()->begin(); 
      measBase !=tr->measurementsOnTrack()->end(); ++measBase) {

      const Trk::RIO_OnTrack* rio = dynamic_cast <const Trk::RIO_OnTrack*>(*measBase);
      if (rio != 0) {
        const Identifier& surfaceID = (rio->identify());
        if(filterHit(surfaceID)==false) {
          MeasurementBase_Collection.push_back(*measBase);

          // protect against ganged Pixel Hits
          if (rio->prepRawData()) {
            const InDet::SiCluster* siHit = dynamic_cast <const InDet::SiCluster*>(rio->prepRawData());
            if (siHit) {
              if (siHit->gangedPixel()) {
                containsGangedPixels = true;
                if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " Reject track # "  << m_trkindex 
                                                        << " because it contains ganged pixel hits" << endreq;
              }
            }
          }

        }
      }
    }

    // Do Silicon only Refit with Silicon only HitCollection and TrackParameters at StartingPoint of track
    if (!containsGangedPixels) {
      if (m_doCosmic) {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Refitting using Cosmic reference point" << endreq;

        // order the parameters according to the Cosmics reference points
        // works only for Si, not for TRT!
        const Trk::TrackParameters* minPar = *(std::min_element(tr->trackParameters()->begin(),
                tr->trackParameters()->end(),
                *m_comPar));

        SiOnlyTrack = m_ITrkFitter->fit(MeasurementBase_Collection, *minPar, m_OutlierRemoval, m_ParticleHypothesis);
      }
      else 
        SiOnlyTrack = m_ITrkFitter->fit(MeasurementBase_Collection, *(tr->trackParameters()->front()), 
          m_OutlierRemoval, m_ParticleHypothesis);
    }
    else {
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " No refit was done for track # " << m_trkindex << endreq;
      return 0;
    }

    return SiOnlyTrack;
  }


  //_________________________________________________________________________________________
  bool RefitSiOnlyTool::filterHit(const Identifier& id) const
  {
    if (m_idHelper->is_sct(id) && m_stripSCTHits) {return true;}
    else if (m_idHelper->is_pixel(id) && m_stripPixelHits) {return true;}
    else if (m_idHelper->is_trt(id) && m_stripTRTHits) {return true;}
    else {return false;}
  }

  
  //_________________________________________________________________________________________
  void RefitSiOnlyTool::dumpTrackCol(DataVector<Trk::Track>* tracks) { 
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "In dumpTrackCol() with size=" << tracks->size() << endreq;
    
    int itrk = 0;
    for (DataVector<Trk::Track>::const_iterator it=tracks->begin(); 
   it!=tracks->end();++it) {

      if(*it!=0){
  
        const Trk::Perigee* aMeasPer = (*it)->perigeeParameters();
        if (aMeasPer==0){
          if (msgLvl(MSG::ERROR)) msg(MSG::ERROR) << "Could not get Trk::MeasuredPerigee" << endreq;}
        else {
          double d0     = aMeasPer->parameters()[Trk::d0];
          double z0     = aMeasPer->parameters()[Trk::z0];
          double phi0   = aMeasPer->parameters()[Trk::phi0];
          double theta  = aMeasPer->parameters()[Trk::theta];
          double qOverP = aMeasPer->parameters()[Trk::qOverP];
          if (msgLvl(MSG::DEBUG)) {
                  msg(MSG::DEBUG) << itrk << ". Track Parameters (d0,z0,phi0,theta,qOverP)"  << endreq;
                  msg(MSG::DEBUG) << " " << d0 << ", " << z0  << ", " 
                          << phi0 << ", " << theta  << ", " << qOverP << endreq;
                  msg(MSG::DEBUG) << " - momentum: " 
                          << aMeasPer->momentum().mag() / CLHEP::GeV << " CLHEP::GeV/c" << endreq;  
          }
        }
      }
      itrk++;
    }   
  }
} //namespace
