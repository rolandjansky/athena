/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// J.Wakabayashi start
#include "AthenaKernel/errorcheck.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "muonEvent/Muon.h"

#include "L1TgcTrkExtrapolator.h"

#include "TrkTrack/TrackStateOnSurface.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "CLHEP/Vector/ThreeVector.h"


D3PD::L1TgcTrkExtrapolator::L1TgcTrkExtrapolator(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent)
  : AthAlgTool(type, name, parent),
    m_extrapolator("Trk::Extrapolator/AtlasExtrapolator"),
    m_endcapPivotPlaneMinimumRadius(0.), // minimum radius of pivot plane in endcap region
    m_endcapPivotPlaneMaximumRadius(11977.), // maximum radius of pivot plane in endcap region
    m_barrelPivotPlaneHalfLength(9500.) // half length of pivot plane in barrel region
{  
  declareInterface<L1TgcTrkExtrapolator>(this);

  declareProperty("EndcapPivotPlaneMinimumRadius", m_endcapPivotPlaneMinimumRadius);
  declareProperty("EndcapPivotPlaneMaximumRadius", m_endcapPivotPlaneMaximumRadius);
  declareProperty("BarrelPivotPlaneHalfLength", m_barrelPivotPlaneHalfLength);
}



D3PD::L1TgcTrkExtrapolator::~L1TgcTrkExtrapolator()
{  
}



StatusCode
D3PD::L1TgcTrkExtrapolator::initialize()
{
  ATH_MSG_DEBUG("initialize()");
  
  if (AthAlgTool::initialize().isFailure()) return StatusCode::FAILURE;
  
  // extrapolator
  if (m_extrapolator.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool: " << m_extrapolator);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Retrieved tool: " << m_extrapolator);
  
  ATH_MSG_DEBUG("initialize() successful");

  return StatusCode::SUCCESS;
}



StatusCode
D3PD::L1TgcTrkExtrapolator::finalize()
{
  ATH_MSG_DEBUG("finalize()");
  if (AthAlgTool::finalize().isFailure()) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}



bool
D3PD::L1TgcTrkExtrapolator::extrapolateFromMDT(const Rec::TrackParticle* trackParticle,
                                               const Amg::Vector3D& pos,
                                               const int detector,
                                               Amg::Vector2D& eta,
                                               Amg::Vector2D& phi,
                                               Amg::Vector3D& mom) const
{
  if (!trackParticle) {
    ATH_MSG_WARNING("extrapolateFromMDT: track pointer is null");
    return false;
  }

  const Trk::Track* track = trackParticle->originalTrack();
  
  std::vector<double> vExtEta, vExtPhi, vExtPx, vExtPy, vExtPz, vExtDeltaEta, vExtDeltaPhi, vExtDistance;
  
  const DataVector<const Trk::TrackStateOnSurface>* trackOnSurf = track->trackStateOnSurfaces();
  if(!trackOnSurf) return false;
  
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it = trackOnSurf->begin();
  DataVector<const Trk::TrackStateOnSurface>::const_iterator it_end = trackOnSurf->end();
  
  int counter = 0;
  for ( ; it!=it_end; it++) {
    if (!(*it)->type(Trk::TrackStateOnSurface::Measurement)) continue;
    
    const Trk::TrackParameters* point;
    Amg::Vector2D distance(-1.e30, -1.e30);
    
    if (detector == L1TgcTrkExtrapolator::TGC) {
      point = extrapolateToTGC(*it, pos, distance);
    } else if (detector == L1TgcTrkExtrapolator::RPC) {
      point = extrapolateToRPC(*it, pos, distance);
    } else {
      ATH_MSG_WARNING("extrapolateFromMDT: unknown target: " << detector);
      continue;
    }
    
    if(!point){
      ATH_MSG_DEBUG("Pointer from extrapolation is null.");
      continue;
    }

    vExtEta.push_back(point->position().eta());
    vExtPhi.push_back(point->position().phi());
    vExtPx.push_back(point->momentum().x());
    vExtPy.push_back(point->momentum().y());
    vExtPz.push_back(point->momentum().z());
    vExtDistance.push_back(distance.y());
    counter++;
    delete point;
  }
  if (counter==0) return false;
 
  double minimum = 1.e30;
  int number = -1;
  for (int ii = 0; ii < counter; ii++) {
    if (minimum>vExtDistance.at(ii)) {
      minimum = vExtDistance.at(ii);
      number = ii;
    }
  }
  if (number==-1) return false;
  
  eta[0] = vExtEta.at(number);
  phi[0] = vExtPhi.at(number);
  eta[1] = getError(vExtEta);
  phi[1] = getError(vExtPhi);
  
  mom[0] = vExtPx.at(number);
  mom[1] = vExtPy.at(number);
  mom[2] = vExtPz.at(number);
  
  ATH_MSG_DEBUG("eta = " << vExtEta.at(number) << " phi = " << vExtPhi.at(number) );
  return true;
}



const Trk::TrackParameters*
D3PD::L1TgcTrkExtrapolator::extrapolateToTGC(const Trk::TrackStateOnSurface* tsos,
                                             const Amg::Vector3D& pos,
                                             Amg::Vector2D& distance) const
{
  const Trk::TrackParameters* track = tsos->trackParameters();
  if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
    ATH_MSG_DEBUG("extrapolateToTGC: track pointer is null or wrong type");
    return 0;
  }
  
  double targetZ = pos.z();
  double trackZ = track->position().z();
  if (fabs(trackZ)<fabs(targetZ)-2000. || fabs(trackZ)>fabs(targetZ)+2000.) return 0;
  
  Amg::Transform3D* matrix = new Amg::Transform3D;
  matrix->setIdentity();
  matrix->translation().z() = targetZ;
  
  Trk::DiscSurface* disc = new Trk::DiscSurface(matrix,
                                                m_endcapPivotPlaneMinimumRadius,
                                                m_endcapPivotPlaneMaximumRadius);
  if(!disc){
    ATH_MSG_DEBUG("new Trk::DoscSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }
    
  distance[0] = trackZ;
  distance[1] = fabs(trackZ - targetZ);
  
  const bool boundaryCheck = true;
  const Trk::Surface* surface = disc;
  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*track,
                                                                  *surface,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  delete disc;
  disc = 0;
  surface = 0;
  matrix = 0;
  
  if (!param) {
    ATH_MSG_DEBUG("trackparameters pointer is null");
    return 0;
  }
  const Trk::AtaDisc* ataDisc =
    dynamic_cast<const Trk::AtaDisc*>(param);
  if (!ataDisc){
    ATH_MSG_DEBUG("-->AtaDisc pointer is null.");
  }
  return ataDisc;
}



const Trk::TrackParameters*
D3PD::L1TgcTrkExtrapolator::extrapolateToRPC(const Trk::TrackStateOnSurface* tsos,
                                             const Amg::Vector3D& pos,
                                             Amg::Vector2D& distance) const
{
  const Trk::TrackParameters* track = tsos->trackParameters();
  if (!track || dynamic_cast<const Trk::AtaStraightLine*>(track) == 0) {
    ATH_MSG_DEBUG("extrapolateToTGC: track pointer is null or wrong type");
    return 0;
  }
  
  double radius = pos.perp();
  double trackRadius = track->position().perp();

  if(trackRadius<radius-2000. || trackRadius>radius+2000.) return 0;

  Amg::Transform3D* matrix = new Amg::Transform3D;
  matrix->setIdentity();

  Trk::CylinderSurface* cylinder = 
    new Trk::CylinderSurface(matrix,
                             radius,
                             m_barrelPivotPlaneHalfLength);
  if (!cylinder) {
    ATH_MSG_DEBUG("new Trk::CylinderSurface failed.");
    delete matrix;
    matrix = 0;
    return 0;
  }
  
  distance[0] = trackRadius;
  distance[1] = trackRadius - radius;

  const bool boundaryCheck = true;
  const Trk::Surface* surface = cylinder;
  const Trk::TrackParameters* param = m_extrapolator->extrapolate(*track,
                                                                  *surface,
                                                                  Trk::anyDirection,
                                                                  boundaryCheck,
                                                                  Trk::muon);
  delete cylinder;
  cylinder = 0;
  surface = 0;
  matrix = 0;
  
  if (!param) {
    ATH_MSG_DEBUG("trackparameters pointer is null");
    return 0;
  }
  const Trk::AtaCylinder* ataCylinder =
    dynamic_cast<const Trk::AtaCylinder*>(param);
  if (!ataCylinder){
    ATH_MSG_DEBUG("-->AtaCylinder pointer is null.");
  }
  return ataCylinder;
}



double
D3PD::L1TgcTrkExtrapolator::getError(const std::vector<double>& inputVec) const
{
  const int nSize = inputVec.size();
  double sum = 0;
  double sum2 = 0;
  
  for (int ii = 0; ii < nSize; ii++) {
    sum = sum + inputVec.at(ii);
  }
  
  const double mean = sum/nSize;
  
  for(int jj = 0; jj < nSize; jj++){
    sum2 = sum2 + pow((inputVec.at(jj)-mean),2);
  }
  
  const double stdDev = sqrt(sum2/nSize);
  
  return stdDev;
}
/* eof */
