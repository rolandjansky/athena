/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelECBichselChargeTool.cxx
//   Implementation file for class PixelECBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelECBichselChargeTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
//#include "GeoPrimitives/GeoPrimitivesHelpers.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

#include "BichselSimTool.h"
#include "TLorentzVector.h"

using namespace InDetDD;


// Constructor with parameters:
PixelECBichselChargeTool::PixelECBichselChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
	SubChargesTool(type,name,parent),
	m_numberOfSteps(50),
	m_numberOfCharges(10),
	m_diffusionConstant(.007),
  m_doBichsel(false),
  //m_doBichselMomentumCut(1000.),
  m_doBichselBetaGammaCut(0.1),
  m_doPU(true),
  m_BichselSimTool("BichselSimTool")
{ 
	declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelEC");
	declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelEC");
	declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for PixelEC");
  declareProperty("doBichsel", m_doBichsel, "re-do charge deposition following Bichsel model");
  //declareProperty("doBichselMomentumCut", m_doBichselMomentumCut, "minimum MOMENTUM for particle to be re-simulated through Bichsel Model");
  declareProperty("doBichselBetaGammaCut", m_doBichselBetaGammaCut, "minimum beta-gamma for particle to be re-simulated through Bichsel Model");
  declareProperty("doPU", m_doPU, "whether we apply Bichsel model on PU");
  declareProperty("BichselSimTool", m_BichselSimTool, "tool that implements Bichsel model");
}

class DetCondCFloat;

// Destructor:
PixelECBichselChargeTool::~PixelECBichselChargeTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode PixelECBichselChargeTool::initialize() {
  StatusCode sc = SubChargesTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelECBichselChargeTool::initialize() failed");
    return sc ;
  }

  ATH_MSG_INFO("You are using PixelECBichselChargeTool, not PixelECChargeTool");

  if(m_doBichsel){
    ATH_MSG_INFO("Bichsel Digitization is turned ON in PixelECBichselChargeTool!");

    sc = m_BichselSimTool.retrieve();
    if(sc.isFailure()){
      ATH_MSG_FATAL("Fail to retrieve BichselSimTool in PixelECBichselChargeTool!");
      return sc;
    }
  }
  else{
    ATH_MSG_INFO("Bichsel Digitization is turned OFF in PixelECBichselChargeTool!");
  }

  ATH_MSG_DEBUG ( "PixelECBichselChargeTool::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode PixelECBichselChargeTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "PixelECBichselChargeTool::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "PixelECBichselChargeTool::finalize()");
  return sc ;
}

//----------------------------------------------------------------------
// charge
//----------------------------------------------------------------------
StatusCode PixelECBichselChargeTool::charge(const TimedHitPtr<SiHit> &phit,
		  SiChargedDiodeCollection& chargedDiodes,
		  const InDetDD::SiDetectorElement &Module)
{
  ATH_MSG_DEBUG("Applying PixelEC charge processor");
  const EBC_EVCOLL evColl = (m_needsMcEventCollHelper) ? getMcEventCollectionHMPLEnumFromTimedHitPtr(phit) : EBC_MAINEVCOLL;
  const bool isEventIndexIsPosition = (phit.eventId()==0);
  HepMcParticleLink McLink(phit->trackNumber(), phit.eventId(), evColl, isEventIndexIsPosition);
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
  electronHolePairsPerEnergy = siProperties.electronHolePairsPerEnergy();

  double stepsize = sensorThickness/m_numberOfSteps;
  double tanLorentz = Module.getTanLorentzAnglePhi();
  const CLHEP::Hep3Vector pos=phit->localStartPosition();
  const CLHEP::Hep3Vector cs=phit->localEndPosition();
  
  double xEta=pos[SiHit::xEta];
  double xPhi=pos[SiHit::xPhi];
  const double xDep=pos[SiHit::xDep];
  
  double xEtaf = cs[SiHit::xEta];
  double xPhif = cs[SiHit::xPhi];
  const double xDepf = cs[SiHit::xDep];

  if (!m_disableDistortions && !delta_hit) simulateBow(&Module,xPhi,xEta,xDep,xPhif,xEtaf,xDepf);
  
  double cEta=xEtaf-xEta;
  double cPhi=xPhif-xPhi;
  const double cDep=xDepf-xDep;

  double length=sqrt(cEta*cEta+cPhi*cPhi+cDep*cDep);
  const int nsteps=int(length/stepsize)+1; 
  const int ncharges=this->m_numberOfCharges*this->m_numberOfSteps/nsteps+1; 
 
  //double stepEta = cEta / nsteps;
  //double stepPhi = cPhi / nsteps;
  //double stepDep = cDep / nsteps; 

  //double e1=phit->energyLoss()/static_cast<double>(nsteps*ncharges);

  double coLorentz=sqrt(1+pow(tanLorentz,2));

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////

  double iTotalLength = length*1000.;   // mm -> micrometer

  // ultimate feed in to the diffusion (to surface) part
  std::vector<std::pair<double,double> > trfHitRecord; trfHitRecord.clear();

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if(m_doBichsel){
    ParticleType = delta_hit ? -1 : m_BichselSimTool->trfPDG(genPart->pdg_id()); 
    if(ParticleType != -1){ // this is a protection in case delta_hit == true (a delta ray)
      //if(genPart->momentum().perp()/CLHEP::MeV < m_doBichselMomentumCut) ParticleType = -1;

      TLorentzVector genPart_4V;
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
      double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();

      //if(genPart_4V.P()/CLHEP::MeV < m_doBichselMomentumCut) ParticleType = -1;
      if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
    }

    if(!m_doPU){
      if(phit.eventId() != 0) ParticleType = -1;
    }
  } 

  if(ParticleType != -1){ // yes, good to go with Bichsel
    // parameter initialization
    //double iBetaGamma = genPart->momentum().mag()/genPart->momentum().m();   // use 4-vector mass
    //double iBetaGamma = genPart->momentum().beta() * genPart->momentum().gamma();

    // I don't know why genPart->momentum() goes crazy ... 
    TLorentzVector genPart_4V;
    genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
    double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();

    int iParticleType = ParticleType;
    //double iTotalLength = length*1000.;   // mm -> micrometer

    // begin simulation
    std::vector<std::pair<double,double> > rawHitRecord = m_BichselSimTool->BichselSim(iBetaGamma, iParticleType, iTotalLength, genPart->momentum().e()/CLHEP::MeV);

    // check if returned simulation result makes sense
    if(rawHitRecord.size() == 0){ // deal with rawHitRecord==0 specifically -- no energy deposition
      std::pair<double,double> specialHit;
      specialHit.first = 0.; specialHit.second = 0.;
      trfHitRecord.push_back(specialHit);      
    }
    else if( (rawHitRecord.size() == 1) && (rawHitRecord[0].first == -1.) && (rawHitRecord[0].second == -1.) ){ // special flag returned from BichselSim meaning it FAILs
      for(int istep = 0; istep < nsteps; istep++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (istep + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
      }
    }
    else{ // cluster thousands hits to ~20 groups
      trfHitRecord = m_BichselSimTool->ClusterHits(rawHitRecord, nsteps);
    }
  }
  else{  // same as old digitization method
    //double iTotalLength = length*1000.;   // mm -> micrometer
    for(int istep = 0; istep < nsteps; istep++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (istep + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
    }
  }

  // *** Finsih Bichsel *** //

  // *** Now diffuse charges to surface *** //

  for(unsigned int istep = 0; istep < trfHitRecord.size(); istep++){
    std::pair<double,double> iHitRecord = trfHitRecord[istep];

    // double xEta1 = xEta +  stepEta * (istep + 0.5);
    // double xPhi1 = xPhi +  stepPhi * (istep + 0.5);
    // double depD  = xDep +  stepDep * (istep + 0.5);
    double xEta1 = xEta + 1.0*iHitRecord.first/iTotalLength*cEta;
    double xPhi1 = xPhi + 1.0*iHitRecord.first/iTotalLength*cPhi;
    double depD  = xDep + 1.0*iHitRecord.first/iTotalLength*cDep;

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    double spess = 0.5 * sensorThickness - Module.design().readoutSide() * depD;
    if (spess<0) spess=0;
      
    for(int i=0 ; i<ncharges ; i++) {
  
      // diffusion sigma
      double rdif=this->m_diffusionConstant*sqrt(spess*coLorentz/0.3);
      
      // position at the surface
      double xPhiD=xPhi1+spess*tanLorentz+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
      double xEtaD=xEta1+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

      // Get the charge position in Reconstruction local coordinates.
      SiLocalPosition chargePos = Module.hitLocalToLocal(xEtaD, xPhiD);
      
      // The parametrization of the sensor efficiency (if needed)
      // double ed=e1*this->electronHolePairsPerEnergy;
      double ed=(1.0*iHitRecord.second/1.E+6/ncharges)*this->electronHolePairsPerEnergy;
      
      //The following lines are adapted from SiDigitization's Inserter class
      SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,McLink));
    
      SiCellId diode = Module.cellIdOfPosition(scharge.position());
       
	    SiCharge charge = scharge.charge();

	    if (diode.isValid()) {
        chargedDiodes.add(diode,charge);
      }
    }									
  }

	return StatusCode::SUCCESS;
}

void PixelECBichselChargeTool::simulateBow(const InDetDD::SiDetectorElement * element,
                                        double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const {

  // The corrections are assumed to be in the reconstruction local frame, so
  // we must convertfrom the hit local frame to the  reconstruction local frame.
  // In fact the frames are the same for the pixel barrel so these gymnastics are not
  // really needed but its safer to do it properly.

  // If tool is NONE we apply no correction.
  if (m_pixDistoTool.empty()) return;
  Amg::Vector3D dir(element->hitPhiDirection() * (xf - xi),
                    element->hitEtaDirection() * (yf - yi),
                    element->hitDepthDirection() * (zf - zi));

  Amg::Vector2D locposi = element->hitLocalToLocal(yi, xi);
  Amg::Vector2D locposf = element->hitLocalToLocal(yf, xf);
   
  Amg::Vector2D newLocposi = m_pixDistoTool->correctSimulation(element->identify(), locposi, dir);
  Amg::Vector2D newLocposf = m_pixDistoTool->correctSimulation(element->identify(), locposf, dir);
 
  // Extract new coordinates and convert back to hit frame.
  xi = newLocposi[Trk::x] * element->hitPhiDirection();
  yi = newLocposi[Trk::y] * element->hitEtaDirection();

  xf = newLocposf[Trk::x] * element->hitPhiDirection();
  yf = newLocposf[Trk::y] * element->hitEtaDirection();
 
}

