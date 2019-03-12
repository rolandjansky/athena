/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSimPlanarTool.cxx
//   Implementation file for class SensorSimPlanarTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SensorSimPlanarTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesTool/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"

using namespace InDetDD;

//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSimPlanarTool::SensorSimPlanarTool(const std::string& type, const std::string& name,const IInterface* parent):
  SensorSimTool(type,name,parent),
  m_numberOfSteps(50),
  m_numberOfCharges(10),
  m_diffusionConstant(.0)
{ 
  declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelPlanar");
  declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelPlanar");
  declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for PixelPlanar");
}

//TODO: do we need this fwd declaration??
class DetCondCFloat;

SensorSimPlanarTool::~SensorSimPlanarTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSimPlanarTool::initialize() {
  ATH_MSG_DEBUG ( "SensorSimPlanarTool::initialize()");
  ATH_CHECK(SensorSimTool::initialize()); 
  ATH_CHECK(m_lorentzAngleTool.retrieve());
  return StatusCode::SUCCESS;
}

//===============================================
//    F I N A L I Z E
//===============================================
StatusCode SensorSimPlanarTool::finalize() {
  ATH_MSG_DEBUG("SensorSimPlanarTool::finalize()");
  return StatusCode::SUCCESS;
}
//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSimPlanarTool::induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection &chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine *rndmEngine) {

  // So far, this is only discriminating variable from 3D sensor.
  if (p_design.numberOfCircuits()<2){
	  if(!Module.isDBM()) {  //DBM modules also processed here
		  return StatusCode::SUCCESS; 
	  }
  }

  //Load values from energyDeposition
  double eta_0 = initialConditions[0];
  double phi_0 = initialConditions[1];
  double depth_0  = initialConditions[2];
  double dEta = initialConditions[3];
  double dPhi = initialConditions[4];
  double dDepth = initialConditions[5];
  double ncharges = initialConditions[6];
  double iTotalLength = initialConditions[7];

  //Set up physical detector properties, switch on detector material
  ATH_MSG_DEBUG("Applying planar sensor simulation");

  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties & siProperties = m_siPropertiesTool->getSiProperties(Module.identifyHash());
  double eleholePairEnergy = 0;
  double smearRand = 0;

  if (Module.isDBM()){
    eleholePairEnergy = 1. / (13. * CLHEP::eV); // was 3.62 eV.
    m_diffusionConstant = .00265;
    smearRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
  }
  else{
    eleholePairEnergy = siProperties.electronHolePairsPerEnergy();
    m_diffusionConstant = .007;
  }

  double collectionDist = 0.2*CLHEP::mm;
  double smearScale = 1. + 0.35*smearRand;
  double tanLorentz = m_lorentzAngleTool->getTanLorentzAngle(Module.identifyHash());
  double coLorentz=sqrt(1+pow(tanLorentz,2));

  //**************************************//
  //*** Now diffuse charges to surface *** //
  //**************************************//
  for(unsigned int i = 0; i < trfHitRecord.size(); i++){
    std::pair<double,double> iHitRecord = trfHitRecord[i];

    double eta_i = eta_0;
    double phi_i = phi_0;
    double depth_i = depth_0;
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
    double nontrappingProbability = 1.0;
    if (Module.isDBM()){
      nontrappingProbability = exp(-dist_electrode/collectionDist);
    }

    for(int j=0 ; j<ncharges ; j++) {

      // diffusion sigma
      double rdif=this->m_diffusionConstant*sqrt(dist_electrode*coLorentz/0.3);

      // position at the surface
      double phiRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
      double phi_drifted=phi_i+dist_electrode*tanLorentz+rdif*phiRand;
      double etaRand = CLHEP::RandGaussZiggurat::shoot(rndmEngine);
      double eta_drifted=eta_i+rdif*etaRand;

      // amount of energy to be converted into charges at current step
      double energy_per_step = 1.0*iHitRecord.second/1.E+6/ncharges;


      // Slim Edge for IBL planar sensors:
      if ( !(Module.isDBM()) && p_design.getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
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
