/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IblPlanarChargeTool.cxx
//   Implementation file for class IblPlanarChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "IblPlanarChargeTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"

using namespace InDetDD;


// Constructor with parameters:
IblPlanarChargeTool::IblPlanarChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
	SubChargesTool(type,name,parent),
	m_numberOfSteps(50),
	m_numberOfCharges(10),
	m_diffusionConstant(.007),
	m_doSlimEdges(true) //this is what is wanted for Run2/3, but not ITk

{ 
	declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for IblPlanar");
	declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for IblPlanar");
	declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for IblPlanar");
	declareProperty("doSlimEdges", m_doSlimEdges, "Whether the dead areas are modeled with hard-coded cuts");
}

class DetCondCFloat;

// Destructor:
IblPlanarChargeTool::~IblPlanarChargeTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode IblPlanarChargeTool::initialize() {
  StatusCode sc = SubChargesTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "IblPlanarChargeTool::initialize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "IblPlanarChargeTool::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode IblPlanarChargeTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "IblPlanarChargeTool::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "IblPlanarChargeTool::finalize()");
  return sc ;
}

//----------------------------------------------------------------------
// charge
//----------------------------------------------------------------------
StatusCode IblPlanarChargeTool::charge(const TimedHitPtr<SiHit> &phit,
		  SiChargedDiodeCollection& chargedDiodes,
		  const InDetDD::SiDetectorElement &Module)
{
  ATH_MSG_DEBUG("Applying IBLPLANAR charge processor");
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
 
  double stepEta = cEta / nsteps;
  double stepPhi = cPhi / nsteps;
  double stepDep = cDep / nsteps; 

  double e1=phit->energyLoss()/static_cast<double>(nsteps*ncharges);

  double coLorentz=sqrt(1+pow(tanLorentz,2));

  for(int istep =0; istep < nsteps; istep++) {
    double xEta1 = xEta +  stepEta * (istep + 0.5);
    double xPhi1 = xPhi +  stepPhi * (istep + 0.5);
    double depD  = xDep +  stepDep * (istep + 0.5);

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

      if(m_doSlimEdges){
	// Slim Edge for IBL planar sensors:
	// TODO: Access these from somewhere          
	if(std::abs(xEtaD) > 20.440)e1=0.;
	if(std::abs(xEtaD)< 20.440 && std::abs(xEtaD)> 20.200){
	  if(xEtaD>0){
	    e1=e1*(68.13-xEtaD*3.333);            
	    xEtaD = xEtaD - 0.250;
	  }else{  
	    e1=e1*(68.13+xEtaD*3.333);            
	    xEtaD = xEtaD + 0.250;
	  }  
	}
	if(std::abs(xEtaD)< 20.200 && std::abs(xEtaD)> 20.100){
	  if(xEtaD>0){
	    e1=e1*(41.2-xEtaD*2.);             
	    xEtaD = xEtaD - 0.250;
	  }else{  
	    e1=e1*(41.2+xEtaD*2.);            
	    xEtaD = xEtaD + 0.250;
	  }  
	}
      }

      // Get the charge position in Reconstruction local coordinates.
      SiLocalPosition chargePos = Module.hitLocalToLocal(xEtaD, xPhiD);
      
      // The parametrization of the sensor efficiency (if needed)
      double ed=e1*this->electronHolePairsPerEnergy;
      
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

void IblPlanarChargeTool::simulateBow(const InDetDD::SiDetectorElement * element,
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

