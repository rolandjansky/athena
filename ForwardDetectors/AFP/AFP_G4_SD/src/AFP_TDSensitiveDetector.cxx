/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Class header
#include "AFP_TDSensitiveDetector.h"

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

// STL header
#include <sstream>

#define TDMAXQEFF 0.15 //FIXME Try to avoid using preprocessor constants
#define TDMAXCNT  4000 //FIXME Try to avoid using preprocessor constants

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AFP_TDSensitiveDetector::AFP_TDSensitiveDetector(const std::string& name, const std::string& hitCollectionName)
  : G4VSensitiveDetector( name )
  , m_nHitID(-1)
  , m_nEventNumber(0)
  , m_nNumberOfTDSimHits(0)
  , m_HitColl(hitCollectionName)
{
  for( int i=0; i < 4; i++){
    for( int j=0; j < 32; j++){
      m_nNOfTDSimHits[i][j] = 0;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AFP_TDSensitiveDetector::StartOfAthenaEvent()
{
  m_nNumberOfTDSimHits=0;
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 32; j++)
        {
          m_nNOfTDSimHits[i][j] = 0;
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Initialize from G4 - necessary to new the write handle for now
void AFP_TDSensitiveDetector::Initialize(G4HCofThisEvent *)
{
  if (!m_HitColl.isValid()) m_HitColl = CxxUtils::make_unique<AFP_TDSimHitCollection>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool AFP_TDSensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory*)
{
  if(verboseLevel>5)
    {
      G4cout << "AFP_TDSensitiveDetector::ProcessHits" << G4endl;
    }

  bool bRes=false;

  int nTrackID=-1;
  int nParticleEncoding=-1;
  float fKineticEnergy=0.0;
  float fEnergyDeposit=0.0;
  float fWaveLength=0.0;
  float fPreStepX=0.0;
  float fPreStepY=0.0;
  float fPreStepZ=0.0;
  float fPostStepX=0.0;
  float fPostStepY=0.0;
  float fPostStepZ=0.0;
  float fGlobalTime=0.0;
  int nStationID=-1;
  int nDetectorID=-1;
  int nQuarticID=-1;
  // int nPixelRow=-1;
  // int nPixelCol=-1;

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

  /*
    if (VolumeName.contains("TDSensor") || (bRes=VolumeName.contains("TDQuarticBar["))){
    nQuarticID=szbuff[7]-0x30;

    if(VolumeName.contains("TDSensor") && pStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()!="OpAbsorption" )
    {
    //hit in TD sensor but with no OpAbsorption (transportation)
    }
    else{

    fWaveLength = 2.*M_PI*CLHEP::hbarc/(CLHEP::MeV*CLHEP::nm)/fKineticEnergy;

    if (fWaveLength > 800. || fWaveLength < 200.) return 1; // 200-800 nm cut
    AFP_TDSimHit* pHit = new AFP_TDSimHit();
    pHit->m_nHitID=m_nHitID;
    pHit->m_nTrackID=nTrackID;
    pHit->m_nParticleEncoding=nParticleEncoding;
    pHit->m_fKineticEnergy=fKineticEnergy;
    pHit->m_fEnergyDeposit=fEnergyDeposit;
    pHit->m_fWaveLength=fWaveLength;
    pHit->m_fPreStepX=fPreStepX;
    pHit->m_fPreStepY=fPreStepY;
    pHit->m_fPreStepZ=fPreStepZ;
    pHit->m_fPostStepX=fPostStepX;
    pHit->m_fPostStepY=fPostStepY;
    pHit->m_fPostStepZ=fPostStepZ;
    pHit->m_fGlobalTime=fGlobalTime;

    pHit->m_nStationID=nStationID;
    pHit->m_nDetectorID=nDetectorID;
    pHit->m_nSensitiveElementID=(bRes? 2:1)+2*nQuarticID;//Q1: 1-2, Q2: 3-4

    m_HitColl->Insert(*pHit);
    m_nNumberOfTDSimHits++;
    }
    }
  */

  if ( (VolumeName.contains("TDQuarticBarVacBorder")) && pParticleDefinition->GetPDGCharge() !=0 )
    {
      nQuarticID=szbuff[7]-0x30;
      /*
        m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,fEnergyDeposit,
        fWaveLength,fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,
        fPostStepZ,fGlobalTime,nStationID,nDetectorID,(2+2*nQuarticID));//Q1: 1-2, Q2: 3-4
        // m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,fEnergyDeposit,
        //                                fWaveLength,fPreStepX,fPreStepY,fPreStepZ,fPostStepX,fPostStepY,
        //                                fPostStepZ,fGlobalTime,nStationID,nDetectorID,((bRes? 2:1)+2*nQuarticID));//Q1: 1-2, Q2: 3-4
        m_nNumberOfTDSimHits++;
      */
    }

  //////////////// Fast Cherenkov ///////////////////
  if ( (bRes=VolumeName.contains("TDQuarticBar[")) )
    {
      nQuarticID=szbuff[7]-0x30;

      // Cut on maximum number of generated photons/bar
      if     (nStationID==0 && nQuarticID==0){ if (m_nNOfTDSimHits[0][nDetectorID] >= TDMAXCNT) return 1;}
      else if(nStationID==0 && nQuarticID==1){ if (m_nNOfTDSimHits[1][nDetectorID] >= TDMAXCNT) return 1;}
      else if(nStationID==3 && nQuarticID==0){ if (m_nNOfTDSimHits[2][nDetectorID] >= TDMAXCNT) return 1;}
      else if(nStationID==3 && nQuarticID==1){ if (m_nNOfTDSimHits[3][nDetectorID] >= TDMAXCNT) return 1;}

      // Get the Touchable History:
      G4TouchableHistory*  myTouch = (G4TouchableHistory*)(pPreStepPoint->GetTouchable());
      // Calculate the local step position.
      // From a G4 FAQ:
      // http://geant4-hn.slac.stanford.edu:5090/HyperNews/public/get/geometry/17/1.html
      const G4AffineTransform transformation = myTouch->GetHistory()->GetTopTransform();
      G4ThreeVector PreStepPointPos2  = transformation.TransformPoint(PreStepPointPos);
      G4ThreeVector PostStepPointPos2 = transformation.TransformPoint(PostStepPointPos);

      G4String shape( myTouch->GetSolid()->GetEntityType() );

      G4ThreeVector normpX( 1., 0., 0.);
      G4ThreeVector normnX(-1., 0., 0.);
      G4ThreeVector normpY( 0., 1., 0.);
      G4ThreeVector normnY( 0.,-1., 0.);
      G4ThreeVector normpZ( 0., 0., 1.);
      G4ThreeVector normnZ( 0., 0.,-1.);

      //G4double BarpX = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normpX);
      //G4double BarnX = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normnX);
      //G4double BarpY = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normpY);
      //G4double BarnY = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normnY);
      //G4double BarpZ = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normpZ);
      //G4double BarnZ = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PreStepPointPos2, normnZ);

      //G4double BarHalfX = .5 * (BarpX+BarnX);
      //G4double BarHalfY = .5 * (BarpY+BarnY);
      //G4double BarHalfZ = .5 * (BarpZ+BarnZ);

      G4double PreProtonX =  PreStepPointPos2.x();
      G4double PreProtonY =  PreStepPointPos2.y();
      G4double PreProtonZ =  PreStepPointPos2.z();

      G4double PostProtonX =  PostStepPointPos2.x();
      G4double PostProtonY =  PostStepPointPos2.y();
      G4double PostProtonZ =  PostStepPointPos2.z();

      G4ThreeVector p0 = pStep->GetDeltaPosition().unit();

      G4Material* mat = pStep->GetTrack()->GetMaterial();
      G4MaterialPropertiesTable *matPropTable = mat->GetMaterialPropertiesTable();
      // Refractive index
      G4MaterialPropertyVector* Rind = matPropTable->GetProperty("RINDEX");
      // Absorbtion length
      G4MaterialPropertyVector* Alen = matPropTable->GetProperty("ABSLENGTH");

      const G4double charge  = pParticleDefinition->GetPDGCharge();
      const G4double beta    = (pPreStepPoint->GetBeta() + pPostStepPoint->GetBeta()) / 2.;
      G4double BetaInverse   = 1. / beta;

      //G4int Rsize   = Rind->Entries() - 1;
      //G4double Pmin = Rind->GetMinPhotonEnergy();     // 800 nm
      G4double Pmin = Rind->GetMinLowEdgeEnergy();
      //G4double Pmax = Rind->GetMaxPhotonEnergy();  // 200 nm
      G4double Pmax = Rind->GetMaxLowEdgeEnergy();
      G4double dp   = Pmax - Pmin;
      //G4double maxCosTheta  = BetaInverse / Rind->GetMinProperty();
      G4double maxCosTheta  = BetaInverse / Rind->GetMinValue();
      G4double maxSin2Theta = (1.0 - maxCosTheta) * (1.0 + maxCosTheta);

      //G4double meanRI = .5*(Rind->GetMinProperty() + Rind->GetMaxProperty());
      G4double meanRI = .5*(Rind->GetMinValue() + Rind->GetMaxValue());

      // Formula taken from G4 docu (to be changed since the integral approximation)
      G4double MeanNumberOfPhotons = 370.*(charge/CLHEP::eplus)*(charge/CLHEP::eplus)* (1.0 - 1.0/(beta * meanRI * beta * meanRI)) / (CLHEP::cm*CLHEP::eV);
      if (MeanNumberOfPhotons <= 0.0) return 1;

      G4double step_length = pStep->GetStepLength();

      MeanNumberOfPhotons = MeanNumberOfPhotons * step_length * dp;
      G4int NumPhotons = (G4int) G4Poisson( MeanNumberOfPhotons );
      if (NumPhotons <= 0) return 1;
      //ATH_MSG_INFO("number of photons: " << NumPhotons);

      G4int NumPhotonsCuts=0;
      for (G4int I = 0; I < NumPhotons; I++) {

        G4double rand;
        G4double sampledEnergy, sampledRI;
        G4double cosTheta, sin2Theta;

        // Sample an energy for photon (using MC elimination method)
        do {
          rand = G4UniformRand();
          sampledEnergy = Pmin + rand * dp;
          //sampledRI = Rind->GetProperty(sampledEnergy);
          sampledRI = Rind->Value(sampledEnergy);
          cosTheta = BetaInverse / sampledRI;

          sin2Theta = (1.0 - cosTheta)*(1.0 + cosTheta);
          rand = G4UniformRand();

        } while (rand * maxSin2Theta > sin2Theta);

        // Generate random position of photon on the cone surface defined by Theta
        rand = G4UniformRand();
        G4double phi = 2.*M_PI*rand;
        G4double sinPhi = sin(phi);
        G4double cosPhi = cos(phi);

        // Calculate x,y,z coordinates of photon momentum
        // in coordinate system with primary particle direction aligned with the z-axis
        // + Rotate momentum direction back to the global coordinate system
        G4double sinTheta = sqrt(sin2Theta);
        G4double px = sinTheta*cosPhi;
        G4double py = sinTheta*sinPhi;
        G4double pz = cosTheta;
        G4ParticleMomentum photonMomentum(px, py, pz);
        photonMomentum.rotateUz(p0);

        G4double PX = photonMomentum.getX();
        G4double PY = photonMomentum.getY();
        G4double PZ = photonMomentum.getZ();

        // calculate projections coordinates
        //G4double PXp = PX/sqrt(PX*PX+PY*PY+PZ*PZ);
        G4double PYp = PY/sqrt(PX*PX+PY*PY+PZ*PZ);
        G4double PZp = PZ/sqrt(PX*PX+PY*PY+PZ*PZ);

        G4double PYt = PY/sqrt(PY*PY+PZ*PZ);
        G4double PZt = PZ/sqrt(PY*PY+PZ*PZ);

        // Cosines (alpha, delta)
        cosPhi = (PYp*PYt + PZp*PZt);
        if (nStationID == 0)    cosTheta = ( -PYt*sin(48.*CLHEP::deg) + PZt*cos(48.*CLHEP::deg) );
        else                    cosTheta = ( -PYt*sin(48.*CLHEP::deg) - PZt*cos(48.*CLHEP::deg) );

        // Total internal reflection conditions
        G4double cosThetaC = sqrt(1.-1./sampledRI/sampledRI);
        if (sqrt(1.-cosPhi*cosPhi)>cosThetaC) continue;
        if (sqrt(1.-cosTheta*cosTheta)*cosPhi>cosThetaC) continue;

        // Parametric equation of line where photons are generated
        rand = G4UniformRand();
        G4double PhotonX = PreProtonX + (PostProtonX-PreProtonX)*rand;
        G4double PhotonY = PreProtonY + (PostProtonY-PreProtonY)*rand;
        G4double PhotonZ = PreProtonZ + (PostProtonZ-PreProtonZ)*rand;
        G4ThreeVector PhotonPos(PhotonX,PhotonY,PhotonZ);
        G4double PhotonR = sqrt( (PreProtonX-PhotonX)*(PreProtonX-PhotonX) + (PreProtonY-PhotonY)*(PreProtonY-PhotonY) + (PreProtonZ-PhotonZ)*(PreProtonZ-PhotonZ) );

        G4double Y0;
        // including the scattering from the top edge of the bar (perfect absorber now)
        if (cosTheta>=0)
          {
            Y0 = ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PhotonPos, normnY);
            Y0 = Y0/cosTheta/cosPhi;
          }
        else
          {
            continue;
            //Y0 = 2.*((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PhotonPos, normpY) + ((G4ReflectedSolid *)(myTouch->GetSolid()))->DistanceToOut(PhotonPos, normnY);
            //Y0 = -Y0/cosTheta/cosPhi;
          }

        // absorption of photons inside the crystal
        //float Pabs = 1. - exp( - Y0/Alen->GetProperty(sampledEnergy) );
        float Pabs = 1. - exp( - Y0/Alen->Value(sampledEnergy) );
        rand = G4UniformRand();
        if (Pabs>rand) continue;

        // maximum PMT efficiency cut (15%) to to avoid crashes due to the too large memory consumption
        rand = G4UniformRand();
        if (rand>TDMAXQEFF) continue;
        ///////////////////////////////////////////////////////////////////////////////////////////////

        NumPhotonsCuts++;

        float fGlobalTime2 = fGlobalTime;
        fGlobalTime2 += ( PhotonR * BetaInverse / CLHEP::c_light )/CLHEP::picosecond;

        // for group velocity of light: Edn/dE
        float EdndE;
        //if (sampledEnergy > (Pmin+.5*dp)) EdndE = (sampledRI - Rind->GetProperty(sampledEnergy-0.0001*CLHEP::eV))/0.0001*sampledEnergy/CLHEP::eV;
        if (sampledEnergy > (Pmin+.5*dp)) EdndE = (sampledRI - Rind->Value(sampledEnergy-0.0001*CLHEP::eV))/0.0001*sampledEnergy/CLHEP::eV;
        //else                            EdndE = (Rind->GetProperty(sampledEnergy+0.0001*CLHEP::eV) - sampledRI)/0.0001*sampledEnergy/CLHEP::eV;
        else                              EdndE = (Rind->Value(sampledEnergy+0.0001*CLHEP::eV) - sampledRI)/0.0001*sampledEnergy/CLHEP::eV;
        fGlobalTime2 += ( (sampledRI + EdndE)* Y0 * BetaInverse / CLHEP::c_light )/CLHEP::picosecond;

        if (verboseLevel>5)
          {
            G4cout << "FastCher EdndE: " << EdndE << G4endl;
          }
        fWaveLength = 2.*M_PI*CLHEP::hbarc/sampledEnergy/(CLHEP::MeV*CLHEP::nm);

        // Cut on maximum number of generated photons/bar
        if     (nStationID==0 && nQuarticID==0){ if (m_nNOfTDSimHits[0][nDetectorID] >= TDMAXCNT) return 1;}
        else if(nStationID==0 && nQuarticID==1){ if (m_nNOfTDSimHits[1][nDetectorID] >= TDMAXCNT) return 1;}
        else if(nStationID==3 && nQuarticID==0){ if (m_nNOfTDSimHits[2][nDetectorID] >= TDMAXCNT) return 1;}
        else if(nStationID==3 && nQuarticID==1){ if (m_nNOfTDSimHits[3][nDetectorID] >= TDMAXCNT) return 1;}

        int nSensitiveElementID=-1;
        if(nQuarticID==0) { nSensitiveElementID=1; }
        else if(nQuarticID==1) { nSensitiveElementID=3; }

        m_HitColl->Emplace(m_nHitID,nTrackID,nParticleEncoding,fKineticEnergy,fEnergyDeposit,
                           fWaveLength,PhotonX,PhotonY,PhotonZ,(PhotonX+PX),(PhotonY+PY),(PhotonZ+PZ),
                           fGlobalTime2,nStationID,nDetectorID,nSensitiveElementID);
        m_nNumberOfTDSimHits++;

        if     (nStationID==0 && nQuarticID==0) m_nNOfTDSimHits[0][nDetectorID]++;
        else if(nStationID==0 && nQuarticID==1) m_nNOfTDSimHits[1][nDetectorID]++;
        else if(nStationID==3 && nQuarticID==0) m_nNOfTDSimHits[2][nDetectorID]++;
        else if(nStationID==3 && nQuarticID==1) m_nNOfTDSimHits[3][nDetectorID]++;
      }
      if(verboseLevel>5)
        {
          G4cout << "FastCher number of photons: " << NumPhotonsCuts << G4endl;
        }
    }
  /////////////////////////////////////////////////////
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AFP_TDSensitiveDetector::EndOfAthenaEvent()
{
  if(verboseLevel>5)
    {
      G4cout << "AFP_TDSensitiveDetector::EndOfAthenaEvent: Total number of hits in TD:  " << m_nNumberOfTDSimHits  << G4endl;
      G4cout << "AFP_TDSensitiveDetector::EndOfAthenaEvent: *************************************************************" << G4endl;
    }
  m_nEventNumber++;
  m_nNumberOfTDSimHits=0;

  for( int i=0; i < 4; i++){
    for( int j=0; j < 32; j++){
      m_nNOfTDSimHits[i][j] = 0;
    }
  }
}
