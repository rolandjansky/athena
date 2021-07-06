/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MaterialOnTrackValidation.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TrkValAlgs/MaterialOnTrackValidation.h"
#include "TrkTrack/TrackCollection.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkMaterialOnTrack/EstimatedBremOnTrack.h"
#include "TrkMaterialOnTrack/MaterialEffectsOnTrack.h"
#include "TrkMaterialOnTrack/EnergyLoss.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "TTree.h"


//================ Constructor =================================================

Trk::MaterialOnTrackValidation::MaterialOnTrackValidation(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator),
  m_inputTrackCollection("Tracks"),
  m_Tree(nullptr),
  m_updates(0), // Coverity complaints about this and the following members not to be initialised, although they are before usage.
  m_currentPathInX0(0),
  m_stepInX0(0)
{


  // Get Parameter values from JobOptions file
  declareProperty("InputTrackCollection",       m_inputTrackCollection);
}

//================ Destructor =================================================

Trk::MaterialOnTrackValidation::~MaterialOnTrackValidation()
{}


//================ Initialisation =================================================

StatusCode Trk::MaterialOnTrackValidation::initialize()
{
  ATH_MSG_INFO( "initialize() successful in " << name() );

  // Book the Tree
  bookTree();

  return StatusCode::SUCCESS;

}

//================ Finalisation =================================================

StatusCode Trk::MaterialOnTrackValidation::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Trk::MaterialOnTrackValidation::execute()
{
  // Code entered here will be executed once per event

  // Retrieving the Trackcollection specified via m_inputTrackCollection
  StatusCode sc = StatusCode::SUCCESS;
  const TrackCollection* trackCollection = nullptr;

  if (m_inputTrackCollection!="") {
      sc = evtStore()->retrieve(trackCollection,m_inputTrackCollection);
      if (sc.isFailure())
         ATH_MSG_ERROR( "TrackCollection "<<m_inputTrackCollection<<" not found!" );
      else
         ATH_MSG_VERBOSE("TrackCollection " << m_inputTrackCollection<<" found." );

    } else {
         ATH_MSG_ERROR("No Track collection given!" );
         return StatusCode::FAILURE;
  }

  // Looping over the tracks of retrieved trackcollection

  // Creating the iterators for the loop
  TrackCollection::const_iterator itTracks = trackCollection->begin();
  TrackCollection::const_iterator endTracks = trackCollection->end();

  for (;itTracks!=endTracks;++itTracks) {

    // Getting the TrackStateOnSurface for every track
    const DataVector<const Trk::TrackStateOnSurface > *currentTSOSList =(*itTracks)->trackStateOnSurfaces();

    // Iterators for loop inside track
    DataVector< const Trk::TrackStateOnSurface >::const_iterator itTSOS = currentTSOSList->begin();
    DataVector< const Trk::TrackStateOnSurface >::const_iterator endTSOS = currentTSOSList->end();

    // Initialize variables
    m_updates            = 0;
    m_currentPathInX0    = 0.;

    // Loop over all TrackStatesOnSurface
    for (;itTSOS!=endTSOS;++itTSOS) {

      // Get the Pointer to a trackStateOnSurface
      const MaterialEffectsBase* mEffect = (*itTSOS)->materialEffectsOnTrack();

      // Get the Pointer to Trackparameters
      const TrackParameters* tParameter = (*itTSOS)->trackParameters();

      // Getting all the parameters
      if (mEffect&&tParameter) {                          //protection against zero pointers

        m_stepInX0         = mEffect->thicknessInX0();    //traversed thickness in radiation length
        m_currentPathInX0 += m_stepInX0;                  //accumulated traversed path in rlength

        // Get hit parameters
        m_positionR[m_updates] = tParameter->position().perp();
        m_positionX[m_updates] = tParameter->position().x();
        m_positionY[m_updates] = tParameter->position().y();
        m_positionZ[m_updates] = tParameter->position().z();

        // Get momentum parameters
        m_momentum[m_updates]          = tParameter->momentum().mag();
        const Trk::EstimatedBremOnTrack* eb = 
          dynamic_cast<const Trk::EstimatedBremOnTrack*>(mEffect);
        const Trk::MaterialEffectsOnTrack* mef =
          dynamic_cast<const Trk::MaterialEffectsOnTrack*>(mEffect);
        if (eb!=nullptr) {
          m_momentumChange[m_updates]    = (1.0-eb->retainedEnFraction()) * tParameter->momentum().mag();
        } else if ( (mef!=nullptr) && (mef->energyLoss() != nullptr) ) {
          m_momentumChange[m_updates] = mef->energyLoss()->deltaE();
        } else m_momentumChange[m_updates] = 0.0;

        m_eta[m_updates]               = tParameter->position().eta();
        m_traversedStepInX0[m_updates] = m_stepInX0;        //current traversed thickness/radiation length
        m_traversedPathInX0[m_updates] = m_currentPathInX0; //accumulated traversed path/radiation length

        ++m_updates;                     //increases m_updates everytime the trackparameters are nonzero
        if (m_updates>=MAXUPDATES) {
          ATH_MSG_ERROR("Maximum number of updates reached!");
          // @TODO still fill the tree ? 
          return StatusCode::FAILURE;
        } 
        
      } //protection end


    } //loop TSOS end

    if (m_Tree)
      m_Tree->Fill();

  }   //loop tracks end


  return StatusCode::SUCCESS;
}

//============================================================================================

void Trk::MaterialOnTrackValidation::bookTree() {

  // Booking the Tree
  ATH_MSG_VERBOSE("Booking the tree ...");
  m_Tree = new TTree("MaterialOnTrackValidation","Material on track validation");

  // Adding the branches
  m_Tree->Branch("Updates"        ,&m_updates           , "updates/I");
  m_Tree->Branch("UpdatePositionX", m_positionX         , "updateX[updates]/F");
  m_Tree->Branch("UpdatePositionR", m_positionR         , "updateR[updates]/F");
  m_Tree->Branch("PreMomentum"    , m_momentum          , "preMomentum[updates]/F");
  m_Tree->Branch("MomentumChange" , m_momentumChange    , "momentumChange[updates]/F");
  m_Tree->Branch("PathInX0"       , m_traversedPathInX0 , "PathInX0[updates]/F");
  m_Tree->Branch("StepInX0"       , m_traversedStepInX0 , "StepInX0[updates]/F");
  m_Tree->Branch("Eta"            , m_eta               , "Eta[updates]/F");


  // Registering the Tree
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  ITHistSvc* tHistSvc = nullptr;
  if ((svcLocator->service("THistSvc", tHistSvc)).isFailure()) {
    ATH_MSG_ERROR("initialize() Could not find Hist Service -> Switching ValidationMode Off !");
    delete m_Tree; m_Tree = nullptr;
    return;
  }

  if ((tHistSvc->regTree("/val/MaterialOnTrackValidation", m_Tree)).isFailure()) {
    ATH_MSG_ERROR("initialize() Could not register the Tree -> Switching ValidationMode Off !");
    delete m_Tree; m_Tree = nullptr;
    return;
  }

}
