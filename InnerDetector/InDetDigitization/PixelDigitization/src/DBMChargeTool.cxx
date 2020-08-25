/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// DBMChargeTool.cxx
//   Implementation file for class DBMChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "DBMChargeTool.h"
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

#include "CLHEP/Units/PhysicalConstants.h"

using namespace InDetDD;


// Constructor with parameters:
DBMChargeTool::DBMChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
	SubChargesTool(type,name,parent),
	m_numberOfSteps(50),
	m_numberOfCharges(10),
	m_diffusionConstant(.00265) // Flers: was .007

{ 
	declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for DBM");
	declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for DBM");
	declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for DBM");

}

class DetCondCFloat;

// Destructor:
DBMChargeTool::~DBMChargeTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode DBMChargeTool::initialize() {
  StatusCode sc = SubChargesTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "DBMChargeTool::initialize() failed");
    return sc ;
  }
  ATH_MSG_INFO("numberOfSteps = " << m_numberOfSteps); 
  if (!LoadPixelParameters().isSuccess()) return StatusCode::FAILURE;
  ATH_MSG_DEBUG ( "DBMChargeTool::initialize()");

  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode DBMChargeTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "DBMChargeTool::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "DBMChargeTool::finalize()");
  return sc ;
}

//----------------------------------------------------------------------
// charge
//----------------------------------------------------------------------
StatusCode DBMChargeTool::charge(const TimedHitPtr<SiHit> &phit,
		  SiChargedDiodeCollection& chargedDiodes,
		  const InDetDD::SiDetectorElement &Module)
{
  const EBC_EVCOLL evColl = (m_needsMcEventCollHelper) ? getMcEventCollectionHMPLEnumFromTimedHitPtr(phit) : EBC_MAINEVCOLL;
  const bool isEventIndexIsPosition = (phit.eventId()==0);
  HepMcParticleLink McLink(phit->trackNumber(), phit.eventId(), evColl, isEventIndexIsPosition);
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();
  //const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
  //electronHolePairsPerEnergy = siProperties.electronHolePairsPerEnergy();
  // Flers: hard-code electronHolePairsPerEnergy
  electronHolePairsPerEnergy = 1. / (13. * CLHEP::eV); // was 3.62 eV.
  
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

  double collectionDist = 0.2*CLHEP::mm;
  double smearSigma = 0.35;
  double smearScale = 1. + smearSigma*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

  for(int istep =0; istep < nsteps; istep++) {
    double xEta1 = xEta +  stepEta * (istep + 0.5);
    double xPhi1 = xPhi +  stepPhi * (istep + 0.5);
    double depD  = xDep +  stepDep * (istep + 0.5);

    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    double spess = 0.5 * sensorThickness - Module.design().readoutSide() * depD;
    if (spess<0) spess=0;


    // nonTrapping probability
    double nontrappingProbability = exp(-spess/collectionDist);
      
    for(int i=0 ; i<ncharges ; i++) {
  
      // diffusion sigma
      double rdif=this->m_diffusionConstant*sqrt(spess*coLorentz/0.3);
      
      // position at the surface
      double xPhiD=xPhi1+spess*tanLorentz+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
      double xEtaD=xEta1+rdif*CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

      // Get the charge position in Reconstruction local coordinates.
      SiLocalPosition chargePos = Module.hitLocalToLocal(xEtaD, xPhiD);
      
      // The parametrization of the sensor efficiency (if needed)
      double ed=e1*this->electronHolePairsPerEnergy*nontrappingProbability*smearScale;
      
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

void DBMChargeTool::simulateBow(const InDetDD::SiDetectorElement * element,
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


//----------------------------------------------------------------------
// LoadPixelParameters
//----------------------------------------------------------------------
StatusCode DBMChargeTool::LoadPixelParameters() {
	//This is really just a placeholder for now. It might be a good idea to load electronHolePairsPerEnergy
	//and sensorThickness just once, if possible, but it is not a big deal for now. Other constants may 
	//be loaded here, such as numberOfSteps, if one desires. 
	return StatusCode::SUCCESS;
}

