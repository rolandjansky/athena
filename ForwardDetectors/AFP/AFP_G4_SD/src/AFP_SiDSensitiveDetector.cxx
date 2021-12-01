/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "AFP_SiDSensitiveDetector.h"

// Athena headers
#include "AFP_Geometry/AFP_constants.h"
#include "CxxUtils/make_unique.h" // For make unique

// Geant4 headers
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Poisson.hh"
#include "G4VSolid.hh"
#include "G4ReflectedSolid.hh"
#include "G4Material.hh"
#include "G4MaterialPropertyVector.hh"
#include "AFP_Geometry/AFP_constants.h"

// STL header
#include <sstream>
#include <cmath>

#define SIDMAXCNT 1000 //FIXME Try to avoid using preprocessor constants

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AFP_SiDSensitiveDetector::AFP_SiDSensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_nHitID(-1)
  , m_nEventNumber(0)
  , m_nNumberOfSIDSimHits(0)
  , m_delta_pixel_x(0.050)
  , m_delta_pixel_y(0.250)
  , m_HitColl(hitCollectionName)
{
  AFP_CONSTANTS AfpConstants;
  for( int i=0; i < 4; i++){
    m_nNOfSIDSimHits[i] = 0;
    for( int j=0; j < 10; j++){
      m_death_edge[i][j] = AfpConstants.SiT_DeathEdge; //in mm, it is left edge as the movement is horizontal
      m_lower_edge[i][j] = AfpConstants.SiT_LowerEdge; //in mm,
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AFP_SiDSensitiveDetector::StartOfAthenaEvent()
{
  m_nNumberOfSIDSimHits=0;
  for( int i=0; i < 4; i++)
    {
      m_nNOfSIDSimHits[i] = 0;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void AFP_SiDSensitiveDetector::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid())m_HitColl = CxxUtils::make_unique<AFP_SIDSimHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool AFP_SiDSensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory*)
{
  if(verboseLevel>5)
    {
      G4cout << "AFP_SiDSensitiveDetector::ProcessHits" << G4endl;
    }

  int nTrackID=-1;
  int nParticleEncoding=-1;
  float fKineticEnergy=0.0;
  float fEnergyDeposit=0.0;
  float fPreStepX=0.0;
  float fPreStepY=0.0;
  float fPreStepZ=0.0;
  float fPostStepX=0.0;
  float fPostStepY=0.0;
  float fPostStepZ=0.0;
  float fGlobalTime=0.0;
  int nStationID=-1;
  int nDetectorID=-1;
  // int nPixelRow=-1;
  // int nPixelCol=-1;

  bool bIsSIDAuxVSID=false;

  // step, track and particle info
  G4Track* pTrack = pStep->GetTrack();
  G4ParticleDefinition* pParticleDefinition = pTrack->GetDefinition();
  G4StepPoint* pPreStepPoint = pStep->GetPreStepPoint();
  G4StepPoint* pPostStepPoint = pStep->GetPostStepPoint();
  G4ThreeVector PreStepPointPos = pPreStepPoint->GetPosition();
  G4ThreeVector PostStepPointPos = pPostStepPoint->GetPosition();

  nTrackID=pTrack->GetTrackID();
  fKineticEnergy = pPreStepPoint->GetKineticEnergy();
  fEnergyDeposit = pStep->GetTotalEnergyDeposit();

  fPreStepX = PreStepPointPos.x();
  fPreStepY = PreStepPointPos.y();
  fPreStepZ = PreStepPointPos.z();
  fPostStepX = PostStepPointPos.x();
  fPostStepY = PostStepPointPos.y();
  fPostStepZ = PostStepPointPos.z();
  nParticleEncoding = pParticleDefinition->GetPDGEncoding();
  fGlobalTime = pStep->GetPreStepPoint()->GetGlobalTime()/CLHEP::picosecond; // time w.r.t. prestep or poststep ??

  // name of physical volume
  G4TouchableHandle touch1 = pPreStepPoint->GetTouchableHandle();
  G4VPhysicalVolume* volume = touch1->GetVolume();
  G4String VolumeName = volume->GetName();

  //G4ThreeVector ph0 = pStep->GetDeltaPosition().unit();
  //if (fKineticEnergy<10000. && ph0.y()>.2) pTrack->SetTrackStatus(fKillTrackAndSecondaries);
  //if (VolumeName.contains("TDQuarticBar")) return 1;

  if(verboseLevel>5)
    {
      G4cout << "hit volume name is " << VolumeName << G4endl;

      G4cout << "global, x_pre:  " << fPreStepX  << ", y_pre:  " << fPreStepY  << ", z_pre:  " << fPreStepZ << G4endl;
      G4cout << "global, x_post: " << fPostStepX << ", y_post: " << fPostStepY << ", z_post: " << fPostStepZ << G4endl;
    }
  //scan station and detector id
  char* ppv1, *ppv2;
  char szbuff[32];
  memset(&szbuff[0],0,sizeof(szbuff));
  strncpy(szbuff,VolumeName.data(),sizeof(szbuff));
  szbuff[sizeof(szbuff)-1] = '\0'; // idiomatic use of strncpy...
  ppv1=strchr(szbuff,'[');
  ppv2=strchr(szbuff,']');
  if(!ppv2 || !ppv1){
    G4cout << "ERROR: Invalid format of volume name " << VolumeName << G4endl;
    return false;
  }
  else *ppv2='\0';

  nStationID=10*(szbuff[3]-0x30)+(szbuff[4]-0x30);
  nDetectorID=atoi(ppv1+1);

  m_nHitID++;

  /////////////////////////////////////////////////////

  if (VolumeName.contains("SIDSensor") || (bIsSIDAuxVSID=VolumeName.contains("SIDVacuumSensor"))){

    if(!bIsSIDAuxVSID && !(fEnergyDeposit>0.0))
      {
        //hit in SID sensor but with zero deposited energy (transportation)
      }
    else
      {
        if (bIsSIDAuxVSID)
          {
            m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,fEnergyDeposit,
                               fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,fPostStepZ,
                               fGlobalTime,nStationID,nDetectorID,bIsSIDAuxVSID,-1,-1);
            // G4cout << "pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels
            //        << "] will be stored, with energy "<< fEnergyDeposit*(pixel_track_length_XY/track_length_XY)
            //        << G4endl;
            // m_nNumberOfSIDSimHits++;
          }
        else
          {
            // Cut on maximum number of SID hits/station
            if(m_nNOfSIDSimHits[nStationID] >= SIDMAXCNT) return 1;

            // Get the Touchable History:
            G4TouchableHistory*  myTouch = (G4TouchableHistory*)(pPreStepPoint->GetTouchable());
            // Calculate the local step position.
            // From a G4 FAQ:
            // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html

            const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
            const G4ThreeVector localPosition_pre  = transformation.TransformPoint(PreStepPointPos);
            const G4ThreeVector localPosition_post = transformation.TransformPoint(PostStepPointPos);

            const G4ThreeVector normpX( 1., 0., 0.);
            const G4ThreeVector normnX(-1., 0., 0.);
            const G4ThreeVector normpY( 0., 1., 0.);
            const G4ThreeVector normnY( 0.,-1., 0.);
            const G4ThreeVector normpZ( 0., 0., 1.);
            const G4ThreeVector normnZ( 0., 0.,-1.);

            G4double BarpX = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normpX);
            G4double BarnX = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normnX);
            G4double BarpY = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normpY);
            G4double BarnY = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normnY);
            G4double BarpZ = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normpZ);
            G4double BarnZ = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(localPosition_pre, normnZ);

            G4double BarHalfX = .5 * (BarpX+BarnX);
            G4double BarHalfY = .5 * (BarpY+BarnY);
            G4double BarHalfZ = .5 * (BarpZ+BarnZ);

            // x should be 50 mu, y should be 250 mu, z should be 250mu

            G4double x_det = BarHalfX + localPosition_pre.x(); // we will remove death edge later - death_edge[nStationID][nDetectorID];
            G4double y_det = BarHalfY + localPosition_pre.y(); // we will remove lower edge later - lower_edge[nStationID][nDetectorID];
            G4double z_det = BarHalfZ + localPosition_pre.z();

            G4double x_det_post = BarHalfX + localPosition_post.x(); //- death_edge[nStationID][nDetectorID];
            G4double y_det_post = BarHalfY + localPosition_post.y(); //- lower_edge[nStationID][nDetectorID];
            G4double z_det_post = BarHalfZ + localPosition_post.z();

            //G4double track_length_XY = sqrt(pow(fPostStepX-fPreStepX,2)+pow(fPostStepY-fPreStepY,2));
            G4double track_length_XY = sqrt(pow(x_det_post-x_det,2)+pow(y_det_post-y_det,2));

            G4double angle_phi_global = atan2(fPostStepY-fPreStepY,fPostStepX-fPreStepX);
            G4double angle_phi = atan2(y_det_post-y_det,x_det_post-x_det);

            //G4double tan_phi_global = (fPostStepY-fPreStepY)/(fPostStepX-fPreStepX);
            G4double tan_phi = (y_det_post-y_det)/(x_det_post-x_det);

            if(verboseLevel>5)
              {
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: local, x_det: " << x_det << ", y_det: " << y_det  << ", z_det: " << z_det << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: local, x_det_post: " << x_det_post << ", y_det_post: " << y_det_post << ", z_det_post: " << z_det_post << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits:  angle_phi_global in -pi:pi = "  << angle_phi_global << G4endl;
              }
            if (angle_phi < 0.) angle_phi = 2.*M_PI + angle_phi;
            if(verboseLevel>5)
              {
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_phi_global in 0:2pi = "  << angle_phi << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_phi in -pi:pi = "  << angle_phi << G4endl;
              }
            if (angle_phi < 0.) angle_phi = 2.*M_PI + angle_phi;
                if(verboseLevel>5)
              {
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_phi in 0:2pi = "  << angle_phi << G4endl;
              }
            signed int pre_pixel_x = (signed int)(x_det / m_delta_pixel_x);
            signed int pre_pixel_y = (signed int)(y_det / m_delta_pixel_y);
            signed int post_pixel_x = (signed int)(x_det_post / m_delta_pixel_x);
            signed int post_pixel_y = (signed int)(y_det_post / m_delta_pixel_y);

            signed int sign_pixels_x = 0;
            signed int sign_pixels_y = 0;

            int number_pixels_x = (int) (std::abs((post_pixel_x-pre_pixel_x)*1.0));
            int number_pixels_y = (int) (std::abs((post_pixel_y-pre_pixel_y)*1.0));

            if (number_pixels_x > 0)
              {
                sign_pixels_x = (post_pixel_x-pre_pixel_x)/number_pixels_x;
              }
            if (number_pixels_y > 0)
              {
                sign_pixels_y = (post_pixel_y-pre_pixel_y)/number_pixels_y;
              }

            int n_death_pixels = (int)(m_death_edge[nStationID][nDetectorID]/m_delta_pixel_x);
            int n_lower_pixels = (int)(m_lower_edge[nStationID][nDetectorID]/m_delta_pixel_y);

            if(verboseLevel>5)
              {
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: pre: pixel["<< pre_pixel_x - n_death_pixels <<"]["<< pre_pixel_y - n_lower_pixels <<"] was hit" << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: post: pixel["<< post_pixel_x - n_death_pixels<<"]["<< post_pixel_y - n_lower_pixels<<"] was hit" << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: chip's length in x: " << 2.*BarHalfX << ", in y: " << 2.*BarHalfY << ", in z: " << 2.*BarHalfZ << G4endl;
              }
            signed int first = -1;

            G4double x_next_pixel = -9999.;
            G4double y_next_pixel = -9999.;

            G4double x_border = -9999.;
            G4double y_border = -9999.;

            G4double pixel_track_length_XY = -1.;
            G4double angle_2pixel = 10.;

            //number of pixel in death and lower edge

            int act_pixel_x = pre_pixel_x;
            int act_pixel_y = pre_pixel_y;

            //G4double act_pos_x = x_det;
            //G4double act_pos_y = y_det;

            if(verboseLevel>5)
              {
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual pixel in x = " << act_pixel_x << ", in y = " << act_pixel_y << G4endl;
                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual compensated pixel in x = " << act_pixel_x - n_death_pixels << ", in y = " << act_pixel_y - n_lower_pixels << G4endl;
              }
            if ((number_pixels_x == 0) && (number_pixels_y == 0))
              {

                if(verboseLevel>5)
                  {
                    G4cout << "AFP_SiDSensitiveDetector::ProcessHits: pre and post in the same pixel " << G4endl;
                  }
                if (( pre_pixel_y - n_lower_pixels <= 80) && (pre_pixel_x -n_death_pixels <= 336) && ( pre_pixel_y - n_lower_pixels > 0) && (pre_pixel_x - n_death_pixels > 0))
                  {
                    m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,fEnergyDeposit,
                                       fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,fPostStepZ,
                                       fGlobalTime,nStationID,nDetectorID,bIsSIDAuxVSID,
                                       (pre_pixel_y - n_lower_pixels - 1),
                                       (pre_pixel_x - n_death_pixels - 1));
                    m_nNumberOfSIDSimHits++;

                    m_nNOfSIDSimHits[nStationID]++;
                  }
                else if(verboseLevel>5)
                  {
                    G4cout << "AFP_SiDSensitiveDetector::ProcessHits: hit outside of pixel's sensitive area " << G4endl;
                  }
              }
            else
              {
                if(verboseLevel>5)
                  {
                    G4cout << "AFP_SiDSensitiveDetector::ProcessHits: pre and post in diferent pixels " << G4endl;
                  }
                // still not complete logic, last step must be cut

                while ( (number_pixels_x >= 0) && (number_pixels_y >= 0) )
                  {

                    if ((angle_phi >= 0.) && (angle_phi < M_PI_2))
                      {
                        x_next_pixel = (act_pixel_x+1)*m_delta_pixel_x; //- death_edge[nStationID][nDetectorID];
                        y_next_pixel = (act_pixel_y+1)*m_delta_pixel_y; //- death_edge[nStationID][nDetectorID];
                        angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);

                        if (angle_2pixel < 0.) angle_2pixel = 2*M_PI + angle_2pixel;
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_2pixel in 0:2pi = "  << angle_2pixel << G4endl; }

                        if (angle_2pixel > angle_phi)
                          {
                            first = 0; // x border will be hit first
                          }
                        else
                          {
                            first = 1;
                          }
                      }

                    else if ((angle_phi >= M_PI_2) && (angle_phi < M_PI))
                      {
                        x_next_pixel = (act_pixel_x+0)*m_delta_pixel_x; //- death_edge[nStationID][nDetectorID];
                        y_next_pixel = (act_pixel_y+1)*m_delta_pixel_y; //- death_edge[nStationID][nDetectorID];
                        angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);

                        if (angle_2pixel < 0.) angle_2pixel = 2*M_PI + angle_2pixel;
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_2pixel in 0:2pi = "  << angle_2pixel << G4endl; }

                        if (angle_2pixel > angle_phi)
                          {
                            first = 1; // y border will be hit first
                          }
                        else
                          {
                            first = 0;
                          }
                      }

                    else if ((angle_phi >= M_PI) && (angle_phi < 3.*M_PI_2))
                      {
                        x_next_pixel = (act_pixel_x+0)*m_delta_pixel_x; //- death_edge[nStationID][nDetectorID];
                        y_next_pixel = (act_pixel_y+0)*m_delta_pixel_y; //- death_edge[nStationID][nDetectorID];
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: next pixel corner, x = "  << x_next_pixel << ", y =" << y_next_pixel << G4endl; }

                        angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);

                        if (angle_2pixel < 0.) angle_2pixel = 2*M_PI + angle_2pixel;
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_2pixel in 0:2pi = "  << angle_2pixel << G4endl; }

                        if (angle_2pixel > angle_phi)
                          {
                            first = 0; // x border will be hit first
                          }
                        else
                          {
                            first = 1;
                          }
                      }

                    else if ((angle_phi >= 3.*M_PI_2) && (angle_phi < 2.*M_PI))
                      {
                        x_next_pixel = (act_pixel_x+1)*m_delta_pixel_x; //- death_edge[nStationID][nDetectorID];
                        y_next_pixel = (act_pixel_y+0)*m_delta_pixel_y; //- death_edge[nStationID][nDetectorID];
                        angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);

                        if (angle_2pixel < 0.) angle_2pixel = 2*M_PI + angle_2pixel;
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: angle_2pixel in 0:2pi = "  << angle_2pixel << G4endl; }

                        if (angle_2pixel > angle_phi)
                          {
                            first = 1; // y border will be hit first
                          }
                        else
                          {
                            first = 0;
                          }
                      }

                    else
                      {
                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: something is wrong here!!! " << G4endl; }
                      }


                    if (first == -1 ) {
                      if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: something is wrong here!!! " << G4endl; }
                      exit(false);
                    }

                    if(verboseLevel>5)
                      {
                        G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual pixel in x = " << act_pixel_x << ", in y = " << act_pixel_y << G4endl;
                        G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual compensated pixel in x = " << act_pixel_x - n_death_pixels << ", in y = " << act_pixel_y - n_lower_pixels << G4endl;
                      }

                    if (first == 0 )
                      {

                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: cross is x, " << G4endl; }
                        x_border = x_next_pixel;

                        if ((sign_pixels_x >= 0) &&  (x_border > x_det_post)) x_border = x_det_post;
                        if ((sign_pixels_x < 0) &&  (x_border < x_det_post)) x_border = x_det_post;

                        y_border = tan_phi*(x_border-x_det) + y_det;

                        if (( act_pixel_y - n_lower_pixels <= 80) && (act_pixel_x -n_death_pixels <= 336) && ( act_pixel_y - n_lower_pixels > 0) && (act_pixel_x - n_death_pixels > 0))
                          {
                            pixel_track_length_XY = sqrt(pow(x_border-x_det,2)+pow(y_border-y_det,2));

                            if(verboseLevel>5)
                              {
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: overall energy = " << fEnergyDeposit << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: track XY length = " << track_length_XY << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual XY length = " << pixel_track_length_XY << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: deposited energy = " << fEnergyDeposit*(pixel_track_length_XY/track_length_XY) << G4endl;
                              }

                            // this logic has to be still checked, tracks is necessary fully in sensitive area, but logic is probably ok
                            m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,
                                               fEnergyDeposit*(pixel_track_length_XY/track_length_XY),
                                               fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,fPostStepZ,
                                               fGlobalTime,nStationID,nDetectorID,bIsSIDAuxVSID,
                                               (act_pixel_y - n_lower_pixels - 1),
                                               (act_pixel_x - n_death_pixels - 1));

                            if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits:pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels <<"] will be stored, with energy "
                                                        << fEnergyDeposit*(pixel_track_length_XY/track_length_XY) << G4endl; }

                            m_nNumberOfSIDSimHits++;

                            m_nNOfSIDSimHits[nStationID]++;
                          }

                        x_det = x_border;

                        x_next_pixel = x_next_pixel + sign_pixels_x*m_delta_pixel_x;
                        number_pixels_x = number_pixels_x - 1;

                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits:  remaining number of pixels in x = " <<  number_pixels_x << ", in y = " <<  number_pixels_y << G4endl; }

                        act_pixel_x = act_pixel_x + sign_pixels_x;
                      }

                    if (first == 1 )
                      {

                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits: cross is y, " << G4endl; }
                        y_border = y_next_pixel;

                        if ((sign_pixels_y >= 0) &&  (y_border > y_det_post)) y_border = x_det_post;
                        if ((sign_pixels_y < 0) &&  (y_border < y_det_post)) y_border = x_det_post;

                        x_border = (y_border-y_det)/tan_phi + x_det;

                        if (( act_pixel_y - n_lower_pixels <= 80) && (act_pixel_x -n_death_pixels <= 336) && ( act_pixel_y - n_lower_pixels >= 0) && (act_pixel_x - n_death_pixels >= 0))
                          {
                            pixel_track_length_XY = sqrt(pow(x_border-x_det,2)+pow(y_border-y_det,2));

                            if(verboseLevel>5)
                              {
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: overall energy = " << fEnergyDeposit << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: track XY length = " << track_length_XY << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: actual XY length = " << pixel_track_length_XY << G4endl;
                                G4cout << "AFP_SiDSensitiveDetector::ProcessHits: deposited energy = " << fEnergyDeposit*(pixel_track_length_XY/track_length_XY) << G4endl;
                              }

                            // this logic has to be still checked, tracks is necessary fully in sensitive area, but logic is probably ok
                            m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,
                                               fEnergyDeposit*(pixel_track_length_XY/track_length_XY),
                                               fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,fPostStepZ,
                                               fGlobalTime,nStationID,nDetectorID,bIsSIDAuxVSID,
                                               (act_pixel_y - n_lower_pixels),
                                               (act_pixel_x - n_death_pixels));

                            if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits:pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels <<"] will be stored, with energy "
                                                        << fEnergyDeposit*(pixel_track_length_XY/track_length_XY) << G4endl; }

                            m_nNumberOfSIDSimHits++;

                            m_nNOfSIDSimHits[nStationID]++;
                          }

                        y_det = y_border;

                        y_next_pixel = y_next_pixel + sign_pixels_y*m_delta_pixel_y;
                        number_pixels_y = number_pixels_y - 1;

                        if(verboseLevel>5) { G4cout << "AFP_SiDSensitiveDetector::ProcessHits:  remaining number of pixels in x = " <<  number_pixels_x << ", in y = " <<  number_pixels_y << G4endl; }


                        act_pixel_y = act_pixel_y + sign_pixels_y;
                      }
                  }
              }
          }
      }
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AFP_SiDSensitiveDetector::EndOfAthenaEvent()
{
  if(verboseLevel>5)
    {
      G4cout << "AFP_SiDSensitiveDetector::EndOfAthenaEvent: Total number of hits in SiD: " << m_nNumberOfSIDSimHits << G4endl;
      G4cout << "AFP_SiDSensitiveDetector::EndOfAthenaEvent: *************************************************************" << G4endl;
    }
  m_nEventNumber++;
  m_nNumberOfSIDSimHits=0;

  for( int i=0; i < 4; i++){
    m_nNOfSIDSimHits[i] = 0;
  }
}
