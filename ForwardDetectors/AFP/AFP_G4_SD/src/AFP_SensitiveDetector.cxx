/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AFP_G4_SD/AFP_SensitiveDetector.h"

#include "FadsSensitiveDetector/SensitiveDetectorEntryT.h"
#include "Randomize.hh" 
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "G4ParticleDefinition.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"

//#include "GeoModelKernel/GeoBox.h"
#include "G4Poisson.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4ReflectedSolid.hh"
#include "G4Material.hh"
#include "G4MaterialPropertyVector.hh"

#include <sstream>
#include <iostream>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "AFP_Geometry/AFP_constants.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

static FADS::SensitiveDetectorEntryT<AFP_SensitiveDetector> sd("AFP_SensitiveDetector");

AFP_SensitiveDetector::AFP_SensitiveDetector(std::string name): FADS::FadsSensitiveDetector(name)
{
  ATH_MSG_VERBOSE("AFP_SensitiveDetector::Constructor");
	
  m_nHitID=-1;
  m_nEventNumber=0;
  m_nNumberOfTDSimHits=0;
  m_nNumberOfSIDSimHits=0;
  
  for( int i=0; i < 4; i++)
    {
      m_nNOfSIDSimHits[i] = 0;
    }
  
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 32; j++)
	{
	  m_nNOfTDSimHits[i][j] = 0;
	}
    }
	
  //*************************************************************
  // tom: this piece of code is draft - has to be changed !!!
	
  delta_pixel_x = 0.050; // size of pixel along x, in mm
  delta_pixel_y = 0.250; // size of pixel along y, in mm
	
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 10; j++)
        {
	  // death_edge[i][j] = 1.9; //in mm, it is left edge as the movement is horizontal
	  // lower_edge[i][j] = 0.1; //in mm,
	  death_edge[i][j] = SID_DEATH_EDGE; //in mm, it is left edge as the movement is horizontal
	  lower_edge[i][j] = SID_LOWER_EDGE; //in mm,
        } 
    } 
  //*************************************************************	

  m_pTDSimHitCollection=NULL;
  m_pSIDSimHitCollection=NULL;
}

AFP_SensitiveDetector::~AFP_SensitiveDetector()
{
  ATH_MSG_VERBOSE(" AFP_SensitiveDetector::Destructor ");
}

void AFP_SensitiveDetector::Initialize(G4HCofThisEvent*)
{
  m_pTDSimHitCollection = new AFP_TDSimHitCollection("AFP_TDSimHitCollection"); 
  m_pSIDSimHitCollection = new AFP_SIDSimHitCollection("AFP_SIDSimHitCollection");

  m_nEventNumber=0;
  m_nNumberOfTDSimHits=0;
  m_nNumberOfSIDSimHits=0;
	
  for( int i=0; i < 4; i++)
    {
      m_nNOfSIDSimHits[i] = 0;
    }
	
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 32; j++)
	{
	  m_nNOfTDSimHits[i][j] = 0;
	}
    }
}

bool AFP_SensitiveDetector::ProcessHits(G4Step* pStep, G4TouchableHistory*)
{
  ATH_MSG_DEBUG("AFP_SensitiveDetector::ProcessHits");
	
  bool bRes=false;
  G4double pi = 3.141592654; 
	
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
  //	int nPixelRow=-1;
  //	int nPixelCol=-1;
	
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
	
  ATH_MSG_DEBUG("hit volume name is " << VolumeName);
	
  ATH_MSG_DEBUG("global, x_pre:  " << fPreStepX  << ", y_pre:  " << fPreStepY  << ", z_pre:  " << fPreStepZ);
  ATH_MSG_DEBUG("global, x_post: " << fPostStepX << ", y_post: " << fPostStepY << ", z_post: " << fPostStepZ);

  //scan station and detector id
  char* ppv1, *ppv2;
  char szbuff[32];
  memset(&szbuff[0],0,sizeof(szbuff));
  strncpy(szbuff,VolumeName.data(),sizeof(szbuff));
  szbuff[sizeof(szbuff)-1] = '\0'; // idiomatic use of strncpy...
  ppv1=strchr(szbuff,'[');
  ppv2=strchr(szbuff,']');
  if(!ppv2 || !ppv1){
    ATH_MSG_ERROR("Invalid format of volume name " << VolumeName);
    return false;
  }
  else ppv2='\0';
	
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
		
    m_pTDSimHitCollection->Insert(*pHit);
    m_nNumberOfTDSimHits++;
    }
    }
  */
  
  if ( (VolumeName.contains("TDQuarticBarVacBorder")) && pParticleDefinition->GetPDGCharge() !=0 )
    {
      nQuarticID=szbuff[7]-0x30;
			
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
      pHit->m_nSensitiveElementID=(2+2*nQuarticID);//Q1: 1-2, Q2: 3-4
      //pHit->m_nSensitiveElementID=(bRes? 2:1)+2*nQuarticID;//Q1: 1-2, Q2: 3-4
		
      //m_pTDSimHitCollection->Insert(*pHit);
      //m_nNumberOfTDSimHits++;
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
      //G4double Pmin = Rind->GetMinPhotonEnergy(); 	// 800 nm
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
	if (nStationID == 0) 	cosTheta = ( -PYt*sin(48.*CLHEP::deg) + PZt*cos(48.*CLHEP::deg) );
	else 		     	cosTheta = ( -PYt*sin(48.*CLHEP::deg) - PZt*cos(48.*CLHEP::deg) );
		 
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
	//else 				  EdndE = (Rind->GetProperty(sampledEnergy+0.0001*CLHEP::eV) - sampledRI)/0.0001*sampledEnergy/CLHEP::eV;	
	else 				  EdndE = (Rind->Value(sampledEnergy+0.0001*CLHEP::eV) - sampledRI)/0.0001*sampledEnergy/CLHEP::eV;
	fGlobalTime2 += ( (sampledRI + EdndE)* Y0 * BetaInverse / CLHEP::c_light )/CLHEP::picosecond;   		
			
	ATH_MSG_DEBUG("FastCher EdndE: " << EdndE);
 	   		
	fWaveLength = 2.*M_PI*CLHEP::hbarc/sampledEnergy/(CLHEP::MeV*CLHEP::nm);
			
	// Cut on maximum number of generated photons/bar
	if     (nStationID==0 && nQuarticID==0){ if (m_nNOfTDSimHits[0][nDetectorID] >= TDMAXCNT) return 1;}
	else if(nStationID==0 && nQuarticID==1){ if (m_nNOfTDSimHits[1][nDetectorID] >= TDMAXCNT) return 1;}
	else if(nStationID==3 && nQuarticID==0){ if (m_nNOfTDSimHits[2][nDetectorID] >= TDMAXCNT) return 1;}
	else if(nStationID==3 && nQuarticID==1){ if (m_nNOfTDSimHits[3][nDetectorID] >= TDMAXCNT) return 1;}
			
	AFP_TDSimHit* pHit = new AFP_TDSimHit();
	pHit->m_nHitID=m_nHitID;
	pHit->m_nTrackID=nTrackID;
	pHit->m_nParticleEncoding=nParticleEncoding;
	pHit->m_fKineticEnergy=fKineticEnergy;
	pHit->m_fEnergyDeposit=fEnergyDeposit;
	pHit->m_fWaveLength=fWaveLength;
	pHit->m_fPreStepX=PhotonX;
	pHit->m_fPreStepY=PhotonY;
	pHit->m_fPreStepZ=PhotonZ;			
	pHit->m_fPostStepX=(PhotonX+PX);
	pHit->m_fPostStepY=(PhotonY+PY);
	pHit->m_fPostStepZ=(PhotonZ+PZ);
	pHit->m_fGlobalTime=fGlobalTime2;
	pHit->m_nStationID=nStationID;
	pHit->m_nDetectorID=nDetectorID;
	if(nQuarticID==0) pHit->m_nSensitiveElementID=1;
	else if(nQuarticID==1) pHit->m_nSensitiveElementID=3;
	else pHit->m_nSensitiveElementID=-1;
		
	m_pTDSimHitCollection->Insert(*pHit);
	m_nNumberOfTDSimHits++;
			
	if     (nStationID==0 && nQuarticID==0) m_nNOfTDSimHits[0][nDetectorID]++;
	else if(nStationID==0 && nQuarticID==1) m_nNOfTDSimHits[1][nDetectorID]++;
	else if(nStationID==3 && nQuarticID==0) m_nNOfTDSimHits[2][nDetectorID]++;
	else if(nStationID==3 && nQuarticID==1) m_nNOfTDSimHits[3][nDetectorID]++;
      }
      ATH_MSG_DEBUG("FastCher number of photons: " << NumPhotonsCuts);
    }
  /////////////////////////////////////////////////////
  
  else if (VolumeName.contains("SIDSensor") || (bIsSIDAuxVSID=VolumeName.contains("SIDVacuumSensor"))){
		
    if(!bIsSIDAuxVSID && !(fEnergyDeposit>0.0))
      {
	//hit in SID sensor but with zero deposited energy (transportation)
      }
    else
      {
	if (bIsSIDAuxVSID) 
	  {
	    AFP_SIDSimHit* pHit = new AFP_SIDSimHit();
	    pHit->m_nHitID=m_nHitID;
	    pHit->m_nTrackID=nTrackID;
	    pHit->m_nParticleEncoding=nParticleEncoding;
	    pHit->m_fKineticEnergy=fKineticEnergy;
	    pHit->m_fEnergyDeposit=fEnergyDeposit;
	    pHit->m_fPreStepX=fPreStepX;
	    pHit->m_fPreStepY=fPreStepY;
	    pHit->m_fPreStepZ=fPreStepZ;
	    pHit->m_fPostStepX=fPostStepX;
	    pHit->m_fPostStepY=fPostStepY;
	    pHit->m_fPostStepZ=fPostStepZ;
	    pHit->m_fGlobalTime=fGlobalTime;

	    pHit->m_nStationID=nStationID;
	    pHit->m_nDetectorID=nDetectorID;
	    pHit->m_bIsAuxVSID=bIsSIDAuxVSID;
				
	    //				nPixelRow=-1;
	    //				nPixelCol=-1;
				
	    pHit->m_nPixelRow = -1;
	    pHit->m_nPixelCol = -1;
	    
	    //ATH_MSG_DEBUG("pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels <<"] will be stored, with energy " 
	    //<< fEnergyDeposit*(pixel_track_length_XY/track_length_XY));

	    m_pSIDSimHitCollection->Insert(*pHit);
	    //m_nNumberOfSIDSimHits++;
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
	    G4ThreeVector localPosition_pre  = transformation.TransformPoint(PreStepPointPos);
	    G4ThreeVector localPosition_post = transformation.TransformPoint(PostStepPointPos);
			
	    G4ThreeVector normpX( 1., 0., 0.);
	    G4ThreeVector normnX(-1., 0., 0.);
	    G4ThreeVector normpY( 0., 1., 0.);
	    G4ThreeVector normnY( 0.,-1., 0.);
	    G4ThreeVector normpZ( 0., 0., 1.);
	    G4ThreeVector normnZ( 0., 0.,-1.);
  	    
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
			
	    AFP_SIDSimHit* pHit = new AFP_SIDSimHit();
	    pHit->m_nHitID=m_nHitID;
	    pHit->m_nTrackID=nTrackID;
	    pHit->m_nParticleEncoding=nParticleEncoding;
	    pHit->m_fKineticEnergy=fKineticEnergy;
	    pHit->m_fEnergyDeposit=fEnergyDeposit;
	    pHit->m_fPreStepX=fPreStepX;
	    pHit->m_fPreStepY=fPreStepY;
	    pHit->m_fPreStepZ=fPreStepZ;
	    pHit->m_fPostStepX=fPostStepX;
	    pHit->m_fPostStepY=fPostStepY;
	    pHit->m_fPostStepZ=fPostStepZ;
	    pHit->m_fGlobalTime=fGlobalTime;

	    pHit->m_nStationID=nStationID;
	    pHit->m_nDetectorID=nDetectorID;
	    pHit->m_bIsAuxVSID=bIsSIDAuxVSID;
			
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
			
	    ATH_MSG_DEBUG("local, x_det: " << x_det << ", y_det: " << y_det  << ", z_det: " << z_det);
	    ATH_MSG_DEBUG("local, x_det_post: " << x_det_post << ", y_det_post: " << y_det_post << ", z_det_post: " << z_det_post);
			
	    ATH_MSG_DEBUG(" angle_phi_global in -pi:pi = "  << angle_phi_global);
	    if (angle_phi < 0.) angle_phi = 2.*pi + angle_phi;			  
	    ATH_MSG_DEBUG(" angle_phi_global in 0:2pi = "  << angle_phi);
			
	    ATH_MSG_DEBUG(" angle_phi in -pi:pi = "  << angle_phi);
	    if (angle_phi < 0.) angle_phi = 2.*pi + angle_phi;			  
	    ATH_MSG_DEBUG(" angle_phi in 0:2pi = "  << angle_phi);
			
	    signed int pre_pixel_x = (signed int)(x_det / delta_pixel_x);
	    signed int pre_pixel_y = (signed int)(y_det / delta_pixel_y);
	    signed int post_pixel_x = (signed int)(x_det_post / delta_pixel_x);
	    signed int post_pixel_y = (signed int)(y_det_post / delta_pixel_y);
			
	    signed int sign_pixels_x = 0;
	    signed int sign_pixels_y = 0;
			
	    int number_pixels_x = (int) (abs((post_pixel_x-pre_pixel_x)*1.0));
	    int number_pixels_y = (int) (abs((post_pixel_y-pre_pixel_y)*1.0));
			
	    if (number_pixels_x > 0) 
	      {
		sign_pixels_x = (post_pixel_x-pre_pixel_x)/number_pixels_x;
	      }
	    if (number_pixels_y > 0) 
	      { 
		sign_pixels_y = (post_pixel_y-pre_pixel_y)/number_pixels_y;			 
	      }

	    int n_death_pixels = (int)(death_edge[nStationID][nDetectorID]/delta_pixel_x);
	    int n_lower_pixels = (int)(lower_edge[nStationID][nDetectorID]/delta_pixel_y);
			
	    ATH_MSG_DEBUG("pre: pixel["<< pre_pixel_x - n_death_pixels <<"]["<< pre_pixel_y - n_lower_pixels <<"] was hit");
	    ATH_MSG_DEBUG("post: pixel["<< post_pixel_x - n_death_pixels<<"]["<< post_pixel_y - n_lower_pixels<<"] was hit");
	    ATH_MSG_DEBUG("chip's length in x: " << 2.*BarHalfX << ", in y: " << 2.*BarHalfY << ", in z: " << 2.*BarHalfZ);			
			
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
			  
	    ATH_MSG_DEBUG(" actual pixel in x = " << act_pixel_x << ", in y = " << act_pixel_y);
	    ATH_MSG_DEBUG(" actual compensated pixel in x = " << act_pixel_x - n_death_pixels << ", in y = " << act_pixel_y - n_lower_pixels);
			
	    if ((number_pixels_x == 0) && (number_pixels_y == 0))
	      {
			  
		ATH_MSG_DEBUG(" pre and post in the same pixel ");
			   
		if (( pre_pixel_y - n_lower_pixels <= 80) && (pre_pixel_x -n_death_pixels <= 336) && ( pre_pixel_y - n_lower_pixels > 0) && (pre_pixel_x - n_death_pixels > 0))
		  {
		    pHit->m_nPixelRow = pre_pixel_y - n_lower_pixels - 1;
		    pHit->m_nPixelCol = pre_pixel_x - n_death_pixels - 1;
			    
		    m_pSIDSimHitCollection->Insert(*pHit);
		    m_nNumberOfSIDSimHits++;
			    
		    m_nNOfSIDSimHits[nStationID]++;
		  }
		else
		  {
		    ATH_MSG_DEBUG(" hit outside of pixel's sensitive area ");
		  }  
	      }
	    else 
	      {
		ATH_MSG_DEBUG(" pre and post in diferent pixels ");

		// still not complete logic, last step must be cut

		while ( (number_pixels_x >= 0) && (number_pixels_y >= 0) )
		  {
			     
		    if ((angle_phi >= 0.) && (angle_phi < pi/2.))
		      {
			x_next_pixel = (act_pixel_x+1)*delta_pixel_x; //- death_edge[nStationID][nDetectorID];
			y_next_pixel = (act_pixel_y+1)*delta_pixel_y; //- death_edge[nStationID][nDetectorID]; 
			angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);
			    
			if (angle_2pixel < 0.) angle_2pixel = 2*pi + angle_2pixel;
			ATH_MSG_DEBUG(" angle_2pixel in 0:2pi = "  << angle_2pixel);
			    
			if (angle_2pixel > angle_phi)
			  {
			    first = 0; // x border will be hit first
			  }
			else 
			  {
			    first = 1;
			  }	
		      }
			    
		    else if ((angle_phi >= pi/2.) && (angle_phi < pi))
		      {
			x_next_pixel = (act_pixel_x+0)*delta_pixel_x; //- death_edge[nStationID][nDetectorID];
			y_next_pixel = (act_pixel_y+1)*delta_pixel_y; //- death_edge[nStationID][nDetectorID]; 
			angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);
			    
			if (angle_2pixel < 0.) angle_2pixel = 2*pi + angle_2pixel;
			ATH_MSG_DEBUG(" angle_2pixel in 0:2pi = "  << angle_2pixel);
			      
			if (angle_2pixel > angle_phi)
			  {
			    first = 1; // y border will be hit first
			  }
			else 
			  {
			    first = 0;
			  }
		      }
			   
		    else if ((angle_phi >= pi) && (angle_phi < 3.*pi/2.))
		      {
			x_next_pixel = (act_pixel_x+0)*delta_pixel_x; //- death_edge[nStationID][nDetectorID];
			y_next_pixel = (act_pixel_y+0)*delta_pixel_y; //- death_edge[nStationID][nDetectorID]; 
			ATH_MSG_DEBUG(" next pixel corner, x = "  << x_next_pixel << ", y =" << y_next_pixel);
			    
			angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);
			    
			if (angle_2pixel < 0.) angle_2pixel = 2*pi + angle_2pixel;
			ATH_MSG_DEBUG(" angle_2pixel in 0:2pi = "  << angle_2pixel);
			    
			if (angle_2pixel > angle_phi)
			  {
			    first = 0; // x border will be hit first
			  }
			else 
			  { 
			    first = 1;
			  }
		      }			
			    
		    else if ((angle_phi >= 3.*pi/2.) && (angle_phi < 2.*pi))
		      {
			x_next_pixel = (act_pixel_x+1)*delta_pixel_x; //- death_edge[nStationID][nDetectorID];
			y_next_pixel = (act_pixel_y+0)*delta_pixel_y; //- death_edge[nStationID][nDetectorID]; 
			angle_2pixel = atan2(y_next_pixel-y_det,x_next_pixel-x_det);
			    
			if (angle_2pixel < 0.) angle_2pixel = 2*pi + angle_2pixel;
			ATH_MSG_DEBUG(" angle_2pixel in 0:2pi = "  << angle_2pixel);
			    
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
			ATH_MSG_DEBUG(" something is wrong here!!! ");
		      }
			  
			  
		    if (first == -1 ) { 
		      ATH_MSG_DEBUG(" something is wrong here!!! ");
		      exit(false);
		    }
			  
		    ATH_MSG_DEBUG(" actual pixel in x = " << act_pixel_x << ", in y = " << act_pixel_y);
		    ATH_MSG_DEBUG(" actual compensated pixel in x = " << act_pixel_x - n_death_pixels << ", in y = " << act_pixel_y - n_lower_pixels);
			     
		    if (first == 0 ) 
		      { 
			       
			ATH_MSG_DEBUG(" cross is x, ");
			x_border = x_next_pixel;
				 
			if ((sign_pixels_x >= 0) &&  (x_border > x_det_post)) x_border = x_det_post;  
			if ((sign_pixels_x < 0) &&  (x_border < x_det_post)) x_border = x_det_post; 
				 
			y_border = tan_phi*(x_border-x_det) + y_det;
				 
			if (( act_pixel_y - n_lower_pixels <= 80) && (act_pixel_x -n_death_pixels <= 336) && ( act_pixel_y - n_lower_pixels > 0) && (act_pixel_x - n_death_pixels > 0))
			  {
			    pixel_track_length_XY = sqrt(pow(x_border-x_det,2)+pow(y_border-y_det,2));
				   
			    ATH_MSG_DEBUG(" overall energy = " << fEnergyDeposit);
			    ATH_MSG_DEBUG(" track XY length = " << track_length_XY);
			    ATH_MSG_DEBUG(" actual XY length = " << pixel_track_length_XY);
			    ATH_MSG_DEBUG(" deposited energy = " << fEnergyDeposit*(pixel_track_length_XY/track_length_XY));
				   
			    // this logic has to be still checked, tracks is necessary fully in sensitive area, but logic is probably ok
			    pHit->m_fEnergyDeposit=fEnergyDeposit*(pixel_track_length_XY/track_length_XY); 
			    pHit->m_nPixelRow = act_pixel_y - n_lower_pixels - 1;
			    pHit->m_nPixelCol = act_pixel_x - n_death_pixels - 1;
				   
			    ATH_MSG_DEBUG("pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels <<"] will be stored, with energy " 
					  << fEnergyDeposit*(pixel_track_length_XY/track_length_XY));				   
				   
			    m_pSIDSimHitCollection->Insert(*pHit);
			    m_nNumberOfSIDSimHits++;
			         
			    m_nNOfSIDSimHits[nStationID]++;
			  }
				 
			x_det = x_border;
				 
			x_next_pixel = x_next_pixel + sign_pixels_x*delta_pixel_x;
			number_pixels_x = number_pixels_x - 1;
				 
			ATH_MSG_DEBUG("  remaining number of pixels in x = " <<  number_pixels_x << ", in y = " <<  number_pixels_y);
				 
			act_pixel_x = act_pixel_x + sign_pixels_x;
		      }
			     
		    if (first == 1 ) 
		      { 
			     
			ATH_MSG_DEBUG(" cross is y, ");
			y_border = y_next_pixel;
				 
			if ((sign_pixels_y >= 0) &&  (y_border > y_det_post)) y_border = x_det_post;  
			if ((sign_pixels_y < 0) &&  (y_border < y_det_post)) y_border = x_det_post; 				 
				 
			x_border = (y_border-y_det)/tan_phi + x_det;
				 
			if (( act_pixel_y - n_lower_pixels <= 80) && (act_pixel_x -n_death_pixels <= 336) && ( act_pixel_y - n_lower_pixels >= 0) && (act_pixel_x - n_death_pixels >= 0))
			  {
			    pixel_track_length_XY = sqrt(pow(x_border-x_det,2)+pow(y_border-y_det,2));

			    ATH_MSG_DEBUG(" overall energy = " << fEnergyDeposit);
			    ATH_MSG_DEBUG(" track XY length = " << track_length_XY);
			    ATH_MSG_DEBUG(" actual XY length = " << pixel_track_length_XY);
			    ATH_MSG_DEBUG(" deposited energy = " << fEnergyDeposit*(pixel_track_length_XY/track_length_XY));

			    // this logic has to be still checked, tracks is necessary fully in sensitive area, but logic is probably ok
			    pHit->m_fEnergyDeposit=fEnergyDeposit*(pixel_track_length_XY/track_length_XY); 
			    pHit->m_nPixelRow = act_pixel_y - n_lower_pixels;
			    pHit->m_nPixelCol = act_pixel_x - n_death_pixels;
				   
			    ATH_MSG_DEBUG("pixel["<< act_pixel_x - n_death_pixels <<"]["<< act_pixel_y - n_lower_pixels <<"] will be stored, with energy " 
					  << fEnergyDeposit*(pixel_track_length_XY/track_length_XY));				   
				   
			    m_pSIDSimHitCollection->Insert(*pHit);
			    m_nNumberOfSIDSimHits++;
			         
			    m_nNOfSIDSimHits[nStationID]++;
			  }		 
				 
			y_det = y_border;
				 
			y_next_pixel = y_next_pixel + sign_pixels_y*delta_pixel_y;
			number_pixels_y = number_pixels_y - 1;
				 
			ATH_MSG_DEBUG("  remaining number of pixels in x = " <<  number_pixels_x << ", in y = " <<  number_pixels_y);
				 
				 
			act_pixel_y = act_pixel_y + sign_pixels_y;				
		      }
		  }
	      }
	  } 
      }
  }
	
  return true;
}

void AFP_SensitiveDetector::EndOfEvent(G4HCofThisEvent* )
{
  ATH_MSG_INFO(" Total number of hits in TD:  " << m_nNumberOfTDSimHits );  
  ATH_MSG_INFO(" Total number of hits in SiD: " << m_nNumberOfSIDSimHits);  
  ATH_MSG_INFO("*************************************************************");

  m_HitCollHelp.ExportCollection<AFP_TDSimHitCollection>(m_pTDSimHitCollection);
  m_HitCollHelp.ExportCollection<AFP_SIDSimHitCollection>(m_pSIDSimHitCollection);

  m_nEventNumber++;
  m_nNumberOfTDSimHits=0;
  m_nNumberOfSIDSimHits=0;
	
  for( int i=0; i < 4; i++)
    {
      m_nNOfSIDSimHits[i] = 0;
    }
	
  for( int i=0; i < 4; i++)
    {
      for( int j=0; j < 32; j++)
	{
	  m_nNOfTDSimHits[i][j] = 0;
	}
    }
}

