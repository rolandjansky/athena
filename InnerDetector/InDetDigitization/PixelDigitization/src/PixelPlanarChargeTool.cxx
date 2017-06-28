/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PixelPlanarChargeTool.cxx
//   Implementation file for class PixelPlanarChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "PixelPlanarChargeTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace InDetDD;

PixelPlanarChargeTool::PixelPlanarChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
  SubChargesTool(type,name,parent),
  m_numberOfSteps(50),
  m_numberOfCharges(10),
  m_diffusionConstant(.007),
  m_doBichsel(false),
  m_doBichselBetaGammaCut(0.1),        // replace momentum cut
  m_doDeltaRay(false),                 // need validation
  m_doPU(true),                        
  m_BichselSimTool("BichselSimTool")
{ 
  declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelPlanar");
  declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelPlanar");
  declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for PixelPlanar");
  declareProperty("doBichsel", m_doBichsel, "re-do charge deposition following Bichsel model");
  declareProperty("doBichselBetaGammaCut", m_doBichselBetaGammaCut, "minimum beta-gamma for particle to be re-simulated through Bichsel Model");
  declareProperty("doDeltaRay", m_doDeltaRay, "whether we simulate delta-ray using Bichsel model");
  declareProperty("doPU", m_doPU, "Whether we apply Bichsel model on PU");
  declareProperty("BichselSimTool", m_BichselSimTool, "tool that implements Bichsel model");
}

class DetCondCFloat;

PixelPlanarChargeTool::~PixelPlanarChargeTool() { }

StatusCode PixelPlanarChargeTool::initialize() {
  CHECK(SubChargesTool::initialize()); 
  ATH_MSG_INFO("You are using PixelPlanarChargeTool, not PixelPlanarChargeTool");

  if (m_doBichsel) {
    ATH_MSG_INFO("Bichsel Digitization is turned ON in PixelPlanarChargeTool!");
    CHECK(m_BichselSimTool.retrieve());
  }
  else {
    ATH_MSG_INFO("Bichsel Digitization is turned OFF in PixelPlanarChargeTool!");
  }

  m_doDeltaRay = (m_doBichsel && m_doDeltaRay);    // if we don't do Bichsel model, no re-simulation on delta-ray at all!

  ATH_MSG_DEBUG ( "PixelPlanarChargeTool::initialize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelPlanarChargeTool::finalize() {
  ATH_MSG_DEBUG("PixelPlanarChargeTool::finalize()");
  return StatusCode::SUCCESS;
}

StatusCode PixelPlanarChargeTool::charge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection& chargedDiodes, const InDetDD::SiDetectorElement &Module) {

  // if (!Module.isBarrel()) { return StatusCode::SUCCESS; }
  const PixelModuleDesign *p_design= static_cast<const PixelModuleDesign*>(&(Module.design()));

  // So far, this is only discriminating variable from 3D sensor.
  if (p_design->numberOfCircuits()<2 && !Module.isDBM()) { return StatusCode::SUCCESS; }

  ATH_MSG_DEBUG("Applying PixelPlanar charge processor");
  const HepMcParticleLink McLink = HepMcParticleLink(phit->trackNumber(),phit.eventId());
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
  double eleholePairEnergy = 0;
  if (Module.isDBM()){
    eleholePairEnergy = 1. / (13. * CLHEP::eV); // was 3.62 eV.
  }
  else{
    eleholePairEnergy = siProperties.electronHolePairsPerEnergy();
  }
  double collectionDist = 0.2*CLHEP::mm;
  double smearScale = 1. + 0.35*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

  double stepsize = sensorThickness/m_numberOfSteps;
  double tanLorentz = Module.getTanLorentzAnglePhi();
  const CLHEP::Hep3Vector startPosition=phit->localStartPosition();
  const CLHEP::Hep3Vector endPosition=phit->localEndPosition();

  double eta_0=startPosition[SiHit::xEta];
  double phi_0=startPosition[SiHit::xPhi];
  const double depth_0=startPosition[SiHit::xDep];
  
  double eta_f = endPosition[SiHit::xEta];
  double phi_f = endPosition[SiHit::xPhi];
  const double depth_f = endPosition[SiHit::xDep];

  if (!m_disableDistortions && !delta_hit) simulateBow(&Module,phi_0,eta_0,depth_0,phi_f,eta_f,depth_f);
  
  double dEta=eta_f-eta_0;
  double dPhi=phi_f-phi_0;
  const double dDepth=depth_f-depth_0;

  double pathLength=sqrt(dEta*dEta+dPhi*dPhi+dDepth*dDepth);
  const int nsteps=int(pathLength/stepsize)+1; 
  const int ncharges=this->m_numberOfCharges*this->m_numberOfSteps/nsteps+1; 
 
  //double stepEta = dEta / nsteps;
  //double stepPhi = dPhi / nsteps;
  //double stepDep = dDepth / nsteps; 

  double coLorentz=sqrt(1+pow(tanLorentz,2));

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////
  double iTotalLength = pathLength*1000.;   // mm -> micrometer

  // ultimate feed in to the diffusion (to surface) part
  std::vector<std::pair<double,double> > trfHitRecord; trfHitRecord.clear();

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if(m_doBichsel && !(Module.isDBM())){

    ParticleType = delta_hit ? (m_doDeltaRay ? 4 : -1) : m_BichselSimTool->trfPDG(genPart->pdg_id()); 

    if(ParticleType != -1){ // this is a protection in case delta_hit == true (a delta ray)
      TLorentzVector genPart_4V;

      if(genPart){ // non-delta-ray
        genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
        double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }
      else{ // delta-ray. 
        double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
        double m = 0.511;                             // unit of MeV
        double iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;

        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }

      // In-time PU
      if(!m_doPU){
        if(phit.eventId() != 0) ParticleType = -1;
      }

      // Out-of-time PU
      // We don't cut on the out-of-time PU, since studies show that the fraction is too small
    }
  } 

  if(ParticleType != -1){ // yes, good to go with Bichsel
    // I don't know why genPart->momentum() goes crazy ... 
    TLorentzVector genPart_4V;
    double iBetaGamma;

    if(genPart){
      genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
      iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
    }
    else{
      double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
      double m = 0.511;                             // unit of MeV
      iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;
    }

    int iParticleType = ParticleType;
    //double iTotalLength = pathLength*1000.;   // mm -> micrometer

    // begin simulation
    std::vector<std::pair<double,double> > rawHitRecord = m_BichselSimTool->BichselSim(iBetaGamma, iParticleType, iTotalLength, genPart ? (genPart->momentum().e()/CLHEP::MeV) : (phit->energyLoss()/CLHEP::MeV) );

    // check if returned simulation result makes sense
    if(rawHitRecord.size() == 0){ // deal with rawHitRecord==0 specifically -- no energy deposition
      std::pair<double,double> specialHit;
      specialHit.first = 0.; specialHit.second = 0.;
      trfHitRecord.push_back(specialHit);      
    }
    else if( (rawHitRecord.size() == 1) && (rawHitRecord[0].first == -1.) && (rawHitRecord[0].second == -1.) ){ // special flag returned from BichselSim meaning it FAILs
      for(int j = 0; j < nsteps; j++){ // do the same thing as old digitization method
        std::pair<double,double> specialHit;
        specialHit.first = 1.0*iTotalLength/nsteps * (j + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
        trfHitRecord.push_back(specialHit);
      }
    }
    else{ // cluster thousands hits to ~20 groups
      trfHitRecord = m_BichselSimTool->ClusterHits(rawHitRecord, nsteps);
    }
  }
  else{  // same as old digitization method
    //double iTotalLength = pathLength*1000.;   // mm -> micrometer
    for(int j = 0; j < nsteps; j++){ // do the same thing as old digitization method
      std::pair<double,double> specialHit;
      specialHit.first = 1.0*iTotalLength/nsteps * (j + 0.5); specialHit.second = phit->energyLoss()*1.E+6/nsteps;
      trfHitRecord.push_back(specialHit);
    }
  }

  // *** Finsih Bichsel *** //

  // *** Now diffuse charges to surface *** //
  for(unsigned int i = 0; i < trfHitRecord.size(); i++){
    std::pair<double,double> iHitRecord = trfHitRecord[i];

    // double eta_i = eta_0 +  stepEta * (j + 0.5);
    // double phi_i = phi_0 +  stepPhi * (j + 0.5);
    // double depth_i  = depth_0 +  stepDep * (j + 0.5);

    double eta_i = eta_0;
    double phi_i = phi_0;
    double depth_i  = depth_0;
    if (iTotalLength) {
      eta_i += 1.0*iHitRecord.first/iTotalLength*dEta;
      phi_i += 1.0*iHitRecord.first/iTotalLength*dPhi;
      depth_i  += 1.0*iHitRecord.first/iTotalLength*dDepth;
    }

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
    if (dist_electrode<0) dist_electrode=0;
    
    // nonTrapping probability
    double nontrappingProbability = exp(-dist_electrode/collectionDist);

    for(int j=0 ; j<ncharges ; j++) {

      // diffusion sigma
      double rdif=this->m_diffusionConstant*sqrt(dist_electrode*coLorentz/0.3);

      // position at the surface
      double phi_drifted=phi_i+dist_electrode*tanLorentz+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
      double eta_drifted=eta_i+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

      // amount of energy to be converted into charges at current step
      double energy_per_step = 1.0*iHitRecord.second/1.E+6/ncharges;

      // Slim Edge for IBL planar sensors:
      if ( !(Module.isDBM()) && p_design->getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
        if(std::abs(eta_drifted) > 20.440)energy_per_step=0.;
        if(std::abs(eta_drifted)< 20.440 && std::abs(eta_drifted)> 20.200){
          if(eta_drifted>0){
            energy_per_step=energy_per_step*(68.13-eta_drifted*3.333);            
            eta_drifted = eta_drifted - 0.250;
          }else{  
            energy_per_step=energy_per_step*(68.13+eta_drifted*3.333);            
            eta_drifted = eta_drifted + 0.250;
          }  
        }
        if(std::abs(eta_drifted)< 20.200 && std::abs(eta_drifted)> 20.100){
          if(eta_drifted>0){
            energy_per_step=energy_per_step*(41.2-eta_drifted*2.);             
            eta_drifted = eta_drifted - 0.250;
          }else{  
            energy_per_step=energy_per_step*(41.2+eta_drifted*2.);            
            eta_drifted = eta_drifted + 0.250;
          }  
        }
      }

      // Get the charge position in Reconstruction local coordinates.
      SiLocalPosition chargePos = Module.hitLocalToLocal(eta_drifted, phi_drifted);

      // The parametrization of the sensor efficiency (if needed)
      double ed = 0;
      if (Module.isDBM()){
        ed=energy_per_step*eleholePairEnergy*nontrappingProbability*smearScale;
      }
      else {
        ed=energy_per_step*eleholePairEnergy;
      }

      //The following lines are adapted from SiDigitization's Inserter class
      SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));

      SiCellId diode = Module.cellIdOfPosition(scharge.position());

      SiCharge charge = scharge.charge();

      if (diode.isValid()) {
        chargedDiodes.add(diode,charge);
      }
    }									
  }
  return StatusCode::SUCCESS;
}

void PixelPlanarChargeTool::simulateBow(const InDetDD::SiDetectorElement * element, double& xi, double& yi, const double zi, double& xf, double& yf, const double zf) const {

  // The corrections are assumed to be in the reconstruction local frame, so
  // we must convertfrom the hit local frame to the  reconstruction local frame.
  // In fact the frames are the same for the pixel barrel so these gymnastics are not
  // really needed but its safer to do it properly.

  // If tool is NONE we apply no correction.
  if (m_pixDistoTool.empty()) return;
  Amg::Vector3D dir(element->hitPhiDirection()*(xf-xi), element->hitEtaDirection()*(yf-yi), element->hitDepthDirection()*(zf-zi));

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

