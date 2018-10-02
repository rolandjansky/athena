/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Navigator.cxx, (c) ATLAS Detector Software
///////////////////////////////////////////////////////////////////

// Gaudi Kernel
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
// Trk inlcudes
#include "TrkExTools/NavigatorValidation.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkTrack/Track.h"
// Validation mode - TTree includes
#include "TTree.h"
#include "GaudiKernel/ITHistSvc.h"
// Data Model
#include "AthContainers/DataVector.h"
// Amg
#include "EventPrimitives/EventPrimitives.h"
#include "GeoPrimitives/GeoPrimitives.h"

// constructor
Trk::NavigatorValidation::NavigatorValidation(const std::string &t, const std::string &n, const IInterface *p) :
  Navigator(t, n, p),
  m_validationTreeName("NavigatorValidation"),
  m_validationTreeDescription("Boundary Surface hits"),
  m_validationTreeFolder("/val/NavigationValidation"),
  m_validationTree(nullptr),
  m_boundariesCounter(0),
  m_boundaries{},
  m_boundaryHitX{},
  m_boundaryHitY{},
  m_boundaryHitR{},
  m_boundaryHitZ{}
  {
  declareInterface<INavigator>(this);
  // switch for Validation mode
  declareProperty("ValidationTreeName", m_validationTreeName);
  declareProperty("ValidationTreeDescription", m_validationTreeDescription);
  declareProperty("ValidationTreeFolder", m_validationTreeFolder);
}

// destructor
Trk::NavigatorValidation::~NavigatorValidation() {
}

void Trk::NavigatorValidation::validationInitialize() {
   // create the new Tree
    m_validationTree = new TTree(m_validationTreeName.c_str(), m_validationTreeDescription.c_str());
    // counter for boundary surfaces
    m_validationTree->Branch("BoundaySurfacesHit", &m_boundaries, "boundshit/I");
    m_validationTree->Branch("BoundaryHitX", m_boundaryHitX, "boundshitx[boundshit]/F");
    m_validationTree->Branch("BoundaryHitY", m_boundaryHitY, "boundshity[boundshit]/F");
    m_validationTree->Branch("BoundaryHitR", m_boundaryHitR, "boundshitz[boundshit]/F");
    m_validationTree->Branch("BoundaryHitZ", m_boundaryHitZ, "boundshitr[boundshit]/F");
    // now register the Tree
    ITHistSvc *tHistSvc = nullptr;
    if (service("THistSvc", tHistSvc).isFailure()) {
      ATH_MSG_ERROR("Could not find Hist Service -> Switching ValidationMode Off !");
      delete m_validationTree;
      m_validationTree = nullptr;
    }
    if ((tHistSvc->regTree(m_validationTreeFolder, m_validationTree)).isFailure()) {
      ATH_MSG_ERROR("Could not register the validation Tree -> Switching ValidationMode Off !");
      delete m_validationTree;
      m_validationTree = nullptr;
    }
}

void Trk::NavigatorValidation::validationFill(const Trk::TrackParameters* trackPar) const{

  if(m_boundariesCounter < TRKEXTOOLS_MAXNAVSTEPS) {
    const Amg::Vector3D &posOnBoundary = trackPar->position();
    m_boundaryHitX[m_boundariesCounter] = posOnBoundary.x();
    m_boundaryHitY[m_boundariesCounter] = posOnBoundary.y();
    m_boundaryHitR[m_boundariesCounter] = posOnBoundary.perp();
    m_boundaryHitZ[m_boundariesCounter] = posOnBoundary.z();
    m_boundariesCounter++;
  }
}

void Trk::NavigatorValidation::validationAction() const {
  // first record the values
    if (m_validationTree) {
      m_boundaries = long(m_boundariesCounter);
      m_validationTree->Fill();
      // then reset
    }
    m_boundariesCounter = 0;
}

