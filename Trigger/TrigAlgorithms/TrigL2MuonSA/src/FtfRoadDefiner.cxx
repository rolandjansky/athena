/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "FtfRoadDefiner.h"

TrigL2MuonSA::FtfRoadDefiner::FtfRoadDefiner(const std::string& type,
                                             const std::string& name,
                                             const IInterface*  parent):
  AthAlgTool(type, name, parent)
{
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::FtfRoadDefiner::initialize()
{

  ATH_CHECK( m_extrapolator.retrieve() );

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

StatusCode TrigL2MuonSA::FtfRoadDefiner::defineRoad(const xAOD::TrackParticle* idtrack,
						    TrigL2MuonSA::MuonRoad&    muonRoad)
{
  ATH_MSG_DEBUG("FtfRoadDefiner::defineRoad");

  const int N_SECTOR = 2; // 0: normal, 1:overlap

  double extFtfInnerEta=0.,  extFtfInnerZ=0.,  extFtfInnerR=0.,  extFtfInnerPhi=0.;
  double extFtfMiddleEta=0., extFtfMiddleZ=0., extFtfMiddleR=0., extFtfMiddlePhi=0.;
  double extFtfOuterEta=0.,  extFtfOuterZ=0.,  extFtfOuterR=0.,  extFtfOuterPhi=0.;

  double aw_ftf[3]={0.,0.,0.};
  double bw_ftf[3]={0.,0.,0.};

  // Inner
  const Trk::TrackParameters* extFtfInner = extTrack( idtrack, 5000., 6000., muonRoad.ext_ftf_flag[0][0]); //Large secotr
  if( !extFtfInner ) {
    ATH_MSG_DEBUG("extrapolated track parameters on BarrelInner is null");
  } else {
    extFtfInnerEta = extFtfInner->eta();
    extFtfInnerPhi = extFtfInner->position().phi();
    extFtfInnerZ = extFtfInner->position().z();
    extFtfInnerR = sqrt((extFtfInner->position().x())*(extFtfInner->position().x()) + (extFtfInner->position().y())*(extFtfInner->position().y()));
    ATH_MSG_DEBUG("extFtfInnerEta: " << extFtfInnerEta << ", extFtfInnerPhi: " << extFtfInnerPhi << ", extFtfInnerZ: " << extFtfInnerZ << ", extFtfInnerR: " << extFtfInnerR);
    aw_ftf[0] = tan(2*atan(exp(-extFtfInnerEta)));
    bw_ftf[0] = extFtfInnerR - (aw_ftf[0])*extFtfInnerZ;
    muonRoad.r_ftf[0][0] = extFtfInnerR;
    muonRoad.z_ftf[0][0] = extFtfInnerZ;
  }

  // Middle
  const Trk::TrackParameters* extFtfMiddle = extTrack( idtrack , 8000., 9000., muonRoad.ext_ftf_flag[1][0]);
  if( !extFtfMiddle ) {
    ATH_MSG_DEBUG("extrapolated track parameters on BarrelMiddle is null");
  } else {
    extFtfMiddleEta = extFtfMiddle->eta();
    extFtfMiddlePhi = extFtfMiddle->position().phi();
    extFtfMiddleZ = extFtfMiddle->position().z();
    extFtfMiddleR = sqrt((extFtfMiddle->position().x())*(extFtfMiddle->position().x()) + (extFtfMiddle->position().y())*(extFtfMiddle->position().y()));
    ATH_MSG_DEBUG("extFtfMiddleEta: " << extFtfMiddleEta << ", extFtfMiddlePhi: " << extFtfMiddlePhi << ", extFtfMiddleZ: " << extFtfMiddleZ << ", extFtfMiddleR: " << extFtfMiddleR);
    aw_ftf[1] = tan(2*atan(exp(-extFtfMiddleEta)));
    bw_ftf[1] = extFtfMiddleR - (aw_ftf[1])*extFtfMiddleZ;
    muonRoad.r_ftf[1][0] = extFtfMiddleR;
    muonRoad.z_ftf[1][0] = extFtfMiddleZ;
  }

  // Outer
  const Trk::TrackParameters* extFtfOuter = extTrack( idtrack , 10000., 12500., muonRoad.ext_ftf_flag[2][0]);
  if( !extFtfOuter ) {
    ATH_MSG_DEBUG("extrapolated track parameters on BarrelOuter is null");
  } else {
    extFtfOuterEta = extFtfOuter->eta();
    extFtfOuterPhi = extFtfOuter->position().phi();
    extFtfOuterZ = extFtfOuter->position().z();
    extFtfOuterR = sqrt((extFtfOuter->position().x())*(extFtfOuter->position().x()) + (extFtfOuter->position().y())*(extFtfOuter->position().y()));
    ATH_MSG_DEBUG("extFtfOuterEta: " << extFtfOuterEta << ", extFtfOuterPhi: " << extFtfOuterPhi << ", extFtfOuterZ: " << extFtfOuterZ << ", extFtfOuterR: " << extFtfOuterR);
    aw_ftf[2] = tan(2*atan(exp(-extFtfOuterEta)));
    bw_ftf[2] = extFtfOuterR - (aw_ftf[2])*extFtfOuterZ;
    muonRoad.r_ftf[2][0] = extFtfOuterR;
    muonRoad.z_ftf[2][0] = extFtfOuterZ;
  }

  muonRoad.extFtfMiddleEta = extFtfMiddleEta;
  muonRoad.extFtfMiddlePhi = extFtfMiddlePhi;

  for (int i_sector=0; i_sector<N_SECTOR; i_sector++) {
    muonRoad.aw_ftf[0][i_sector]  = aw_ftf[0];
    muonRoad.bw_ftf[0][i_sector]  = bw_ftf[0];
    muonRoad.aw_ftf[1][i_sector]  = aw_ftf[1];
    muonRoad.bw_ftf[1][i_sector]  = bw_ftf[1];
    muonRoad.aw_ftf[2][i_sector]  = aw_ftf[2];
    muonRoad.bw_ftf[2][i_sector]  = bw_ftf[2];
    muonRoad.aw_ftf[3][i_sector]  = aw_ftf[0];
    muonRoad.bw_ftf[3][i_sector]  = bw_ftf[0];
    muonRoad.aw_ftf[4][i_sector]  = aw_ftf[1];
    muonRoad.bw_ftf[4][i_sector]  = bw_ftf[1];
    muonRoad.aw_ftf[5][i_sector]  = aw_ftf[2];
    muonRoad.bw_ftf[5][i_sector]  = bw_ftf[2];
    muonRoad.aw_ftf[6][i_sector]  = aw_ftf[0];
    muonRoad.bw_ftf[6][i_sector]  = bw_ftf[0];
    muonRoad.aw_ftf[7][i_sector]  = aw_ftf[0];
    muonRoad.bw_ftf[7][i_sector]  = bw_ftf[0];
    muonRoad.aw_ftf[8][i_sector]  = aw_ftf[0];
    muonRoad.bw_ftf[8][i_sector]  = bw_ftf[0];
    muonRoad.aw_ftf[9][i_sector]  = aw_ftf[1];//BME
    muonRoad.bw_ftf[9][i_sector]  = bw_ftf[1];
    muonRoad.aw_ftf[10][i_sector] = aw_ftf[1];//BMG
    muonRoad.bw_ftf[10][i_sector] = bw_ftf[1];

    muonRoad.eta_ftf[0][i_sector]  = extFtfInnerEta;
    muonRoad.phi_ftf[0][i_sector]  = extFtfInnerPhi;
    muonRoad.eta_ftf[1][i_sector]  = extFtfMiddleEta;
    muonRoad.phi_ftf[1][i_sector]  = extFtfMiddlePhi;
    muonRoad.eta_ftf[2][i_sector]  = extFtfOuterEta;
    muonRoad.phi_ftf[2][i_sector]  = extFtfOuterPhi;
    muonRoad.eta_ftf[3][i_sector]  = extFtfInnerEta;
    muonRoad.phi_ftf[3][i_sector]  = extFtfInnerPhi;
    muonRoad.eta_ftf[4][i_sector]  = extFtfMiddleEta;
    muonRoad.phi_ftf[4][i_sector]  = extFtfMiddlePhi;
    muonRoad.eta_ftf[5][i_sector]  = extFtfOuterEta;
    muonRoad.phi_ftf[5][i_sector]  = extFtfOuterPhi;
    muonRoad.eta_ftf[6][i_sector]  = extFtfInnerEta;
    muonRoad.phi_ftf[6][i_sector]  = extFtfInnerPhi;
    muonRoad.eta_ftf[7][i_sector]  = extFtfInnerEta;
    muonRoad.phi_ftf[7][i_sector]  = extFtfInnerPhi;
    muonRoad.eta_ftf[8][i_sector]  = extFtfInnerEta;
    muonRoad.phi_ftf[8][i_sector]  = extFtfInnerPhi;
    muonRoad.eta_ftf[9][i_sector]  = extFtfMiddleEta;//BME
    muonRoad.phi_ftf[9][i_sector]  = extFtfMiddlePhi;
    muonRoad.eta_ftf[10][i_sector] = extFtfMiddleEta;//BMG
    muonRoad.phi_ftf[10][i_sector] = extFtfMiddlePhi;
  }

  return StatusCode::SUCCESS;
}

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

const Trk::TrackParameters* TrigL2MuonSA::FtfRoadDefiner::extTrack( const xAOD::TrackParticle* trk, const double R, const double halflength, int& extFlag ) {

  const bool boundaryCheck = true;
  bool bCylinder = false;
  bool bDisk = false;

  double Z = ( trk->eta()>0 )? halflength:-halflength;

  // Cylinder
  Trk::CylinderSurface* barrel = new Trk::CylinderSurface( R, Z );
  const Trk::Surface* surface1 = barrel;
  const Trk::TrackParameters* param1 = m_extrapolator->extrapolate(*trk, *surface1, Trk::anyDirection, boundaryCheck, Trk::muon);

  // Disk
  Transform3D* matrix = new Transform3D( Amg::Vector3D( 0.,0.,Z ) );

  Trk::DiscSurface* disc = new Trk::DiscSurface( matrix, 0, R );
  const Trk::Surface* surface2 = disc;
  const Trk::TrackParameters* param2 = m_extrapolator->extrapolate(*trk, *surface2, Trk::anyDirection, boundaryCheck, Trk::muon);

  ATH_MSG_DEBUG("R: " << R << ", Z: " << Z);
  if( !param1 ) {
    ATH_MSG_DEBUG("Cylinder -> extrapolated track parameters on Cylinder is null");
  } else {
    bCylinder = true;
    double Rms = sqrt((param1->position().x())*(param1->position().x()) + (param1->position().y())*(param1->position().y()));
    ATH_MSG_DEBUG("Cylinder -> eta: " << param1->eta() << ", phi: " << param1->position().phi() << ", Z: " << param1->position().z() << ", Rms: " << Rms);
  }

  if( !param2 ) {
    ATH_MSG_DEBUG("Disk     -> extrapolated track parameters on Disk is null");
  } else {
    bDisk = true;
    double Rms = sqrt((param2->position().x())*(param2->position().x()) + (param2->position().y())*(param2->position().y()));
    ATH_MSG_DEBUG("Disk     -> eta: " << param2->eta() << ", phi: " << param2->position().phi() << ", Z: " << param2->position().z() << ", Rms: " << Rms);
  }


  surface1 = 0;
  surface2 = 0;
  disc = 0;
  barrel = 0;
  matrix = 0;

  ATH_MSG_DEBUG("bCylinder:" << bCylinder << ", bDisk:" << bDisk );

  delete matrix;
  delete disc;
  delete barrel;

  if ( bCylinder == true && bDisk == true ){
    extFlag = 0;
    return param1;
  } else if ( bCylinder == true && bDisk == false){
    extFlag = 1;
    return param1;
  } else if ( bCylinder == false && bDisk == true){
    extFlag = 2;
    return param2;
  } else {
    extFlag = 3;
    return param1;
  }

  return param1;
}
