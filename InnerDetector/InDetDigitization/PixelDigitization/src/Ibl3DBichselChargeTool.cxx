/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Ibl3DBichselChargeTool.cxx
//   Implementation file for class Ibl3DBichselChargeTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "Ibl3DBichselChargeTool.h"
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

#include "BichselSimTool.h"
#include "TLorentzVector.h"

using namespace InDetDD;


// Constructor with parameters:
Ibl3DBichselChargeTool::Ibl3DBichselChargeTool(const std::string& type, const std::string& name,const IInterface* parent):
				SubChargesTool(type,name,parent),
				m_numberOfSteps(50),
        m_doBichsel(false),
        m_doBichselBetaGammaCut(0.1),        // momentum cut on beta-gamma
        m_doDeltaRay(false),                 // need validation
        m_doPU(true),
        m_BichselSimTool("BichselSimTool"),
				m_chargeCollSvc("ChargeCollProbSvc",name)
{ 
	declareProperty("ChargeCollProbSvc",m_chargeCollSvc);
	declareProperty("numberOfSteps",m_numberOfSteps,"Number of steps for Ibl3D module");
  declareProperty("doBichsel", m_doBichsel, "re-do charge deposition following Bichsel model");
  declareProperty("doBichselBetaGammaCut", m_doBichselBetaGammaCut, "minimum beta-gamma for particle to be re-simulated through Bichsel Model");
  declareProperty("doDeltaRay", m_doDeltaRay, "whether we simulate delta-ray using Bichsel model");
  declareProperty("doPU", m_doPU, "whether we apply Bichsel model on PU");
  declareProperty("BichselSimTool", m_BichselSimTool, "tool that implements Bichsel model");
}

class DetCondCFloat;

// Destructor:
Ibl3DBichselChargeTool::~Ibl3DBichselChargeTool()
{
}

//----------------------------------------------------------------------
// Initialize
//----------------------------------------------------------------------
StatusCode Ibl3DBichselChargeTool::initialize() {
  StatusCode sc = SubChargesTool::initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Ibl3DBichselChargeTool::initialize() failed");
    return sc ;
  }
 	  
  // -- Get ChargeCollProb  Service
  sc = m_chargeCollSvc.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR ( "Could not retrieve ChargeCollProbSvc" );
  }

  ATH_MSG_INFO("You are using Ibl3DBichselChargeTool, not Ibl3DChargeTool");

  if(m_doBichsel){
    ATH_MSG_INFO("Bichsel Digitization is turned ON in Ibl3DBichselChargeTool!");

    sc = m_BichselSimTool.retrieve();
    if(sc.isFailure()){
      ATH_MSG_FATAL("Fail to retrieve BichselSimTool in Ibl3DBichselChargeTool!");
      return sc;
    }
  }
  else{
    ATH_MSG_INFO("Bichsel Digitization is turned OFF in Ibl3DBichselChargeTool!");
  }

  m_doDeltaRay = (m_doBichsel && m_doDeltaRay);    // if we don't do Bichsel model, no re-simulation on delta-ray at all!

  ATH_MSG_DEBUG ( "Ibl3DBichselChargeTool::initialize()");
  return sc ;
}

//----------------------------------------------------------------------
// finalize
//----------------------------------------------------------------------
StatusCode Ibl3DBichselChargeTool::finalize() {
  StatusCode sc = AthAlgTool::finalize();
  if (sc.isFailure()) {
    ATH_MSG_FATAL ( "Ibl3DBichselChargeTool::finalize() failed");
    return sc ;
  }
  ATH_MSG_DEBUG ( "Ibl3DBichselChargeTool::finalize()");
  return sc ;
}

//----------------------------------------------------------------------
// charge
//----------------------------------------------------------------------
StatusCode Ibl3DBichselChargeTool::charge(const TimedHitPtr<SiHit> &phit,
		  SiChargedDiodeCollection& chargedDiodes,
		  const InDetDD::SiDetectorElement &Module)
{ 
  ATH_MSG_VERBOSE("Applying IBL3D charge processor");
  const EBC_EVCOLL evColl = (m_needsMcEventCollHelper) ? getMcEventCollectionHMPLEnumFromTimedHitPtr(phit) : EBC_MAINEVCOLL;
  const bool isEventIndexIsPosition = (phit.eventId()==0);
  HepMcParticleLink McLink(phit->trackNumber(), phit.eventId(), evColl, isEventIndexIsPosition);
  const HepMC::GenParticle* genPart= McLink.cptr(); 
  bool delta_hit = true;
  if (genPart) delta_hit = false;
  double sensorThickness = Module.design().thickness();
  double stepsize = sensorThickness/m_numberOfSteps;
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
  electronHolePairsPerEnergy = siProperties.electronHolePairsPerEnergy();

  // Charge Collection Probability Map bin size
  const double x_bin_size = 0.001;
  const double y_bin_size = 0.001;
 
  std::string readout;
 
  // determine which readout is used
  // FEI4 : 50 X 250 microns
  const PixelModuleDesign *p_design= static_cast<const PixelModuleDesign *>(&(Module.design() ) );
  double pixel_size_x = Module.width()/p_design->rows();
  double pixel_size_y = Module.length()/p_design->columns();
  double module_size_x = Module.width();
  double module_size_y = Module.length();
  
  const CLHEP::Hep3Vector pos=phit->localStartPosition();
  const CLHEP::Hep3Vector cs=phit->localEndPosition();
  
  double xEta=pos[SiHit::xEta];
  double xPhi=pos[SiHit::xPhi];
  const double xDep=pos[SiHit::xDep];
  
  double xEtaf = cs[SiHit::xEta];
  double xPhif = cs[SiHit::xPhi];
  const double xDepf = cs[SiHit::xDep];
  
  double cEta=xEtaf-xEta;
  double cPhi=xPhif-xPhi;
  const double cDep=xDepf-xDep;
 
  // calculate the effective number of steps
  double length=sqrt(cEta*cEta+cPhi*cPhi+cDep*cDep);
  const int nsteps=int(length/stepsize)+1;
  // double es=phit->energyLoss()/static_cast<double>(nsteps);
  // double stepEta = cEta / nsteps;
  // double stepPhi = cPhi / nsteps;
  // double stepDep = cDep / nsteps; 

  //////////////////////////////////////////////////////
  // ***                For Bichsel               *** //
  //////////////////////////////////////////////////////

  double iTotalLength = length*1000.;   // mm -> micrometer

  // ultimate feed in to the diffusion (to surface) part
  std::vector<std::pair<double,double> > trfHitRecord; trfHitRecord.clear();

  // -1 ParticleType means we are unable to run Bichel simulation for this case
  int ParticleType = -1;
  if(m_doBichsel){

    ParticleType = delta_hit ? (m_doDeltaRay ? 4 : -1) : m_BichselSimTool->trfPDG(genPart->pdg_id()); 

    if(ParticleType != -1){ // this is a protection in case delta_hit == true (a delta ray)
      TLorentzVector genPart_4V;

      if(genPart){ // non-delta-ray
        genPart_4V.SetPtEtaPhiM(genPart->momentum().perp(), genPart->momentum().eta(), genPart->momentum().phi(), genPart->momentum().m());
        double iBetaGamma = genPart_4V.Beta() * genPart_4V.Gamma();
        //if(genPart_4V.P()/CLHEP::MeV < m_doBichselMomentumCut) ParticleType = -1;
        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }
      else{ // delta-ray. 
        double k = phit->energyLoss()/CLHEP::MeV;     // unit of MeV
        double m = 0.511;                             // unit of MeV
        double iBetaGamma = TMath::Sqrt(k*(2*m+k))/m;

        if(iBetaGamma < m_doBichselBetaGammaCut) ParticleType = -1;
      }

      // PU
      if(!m_doPU){
        if(phit.eventId() != 0) ParticleType = -1;
      }

    }
  } 

  // bool m_isRealBichsel = false;
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
    //double iTotalLength = length*1000.;   // mm -> micrometer

    // begin simulation
    std::vector<std::pair<double,double> > rawHitRecord = m_BichselSimTool->BichselSim(iBetaGamma, iParticleType, iTotalLength, genPart ? (genPart->momentum().e()/CLHEP::MeV) : (phit->energyLoss()/CLHEP::MeV) );

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
      // m_isRealBichsel = true;
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
  // We split the G4 step into several sub steps. The charge will be deposited at the mid point
  // of these steps
  for(unsigned int istep = 0; istep < trfHitRecord.size(); istep++) {
    std::pair<double,double> iHitRecord = trfHitRecord[istep];

    // This will be the position of the deposited charge
    // double xEta1 = xEta +  stepEta * (istep + 0.5);
    // double xPhi1 = xPhi +  stepPhi * (istep + 0.5);
    // double depD  = xDep +  stepDep * (istep + 0.5);
    double xEta1 = xEta + 1.0*iHitRecord.first/iTotalLength*cEta;
    double xPhi1 = xPhi + 1.0*iHitRecord.first/iTotalLength*cPhi;
    double depD  = xDep + 1.0*iHitRecord.first/iTotalLength*cDep;

    double es_current = 1.0*iHitRecord.second/1.E+6;

    double spess = 0.5 * sensorThickness - Module.design().readoutSide() * depD;
    if (spess<0) spess=0;

    CLHEP::Hep3Vector chargepos;
    chargepos.setX(xPhi1); chargepos.setY(xEta1);  chargepos.setZ(spess);

    bool coord = Module.isModuleFrame();

    ATH_MSG_DEBUG("ismoduleframe "<<coord << " -- pos (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

    // -- change origin of coordinates to the left bottom of module
    double x_new = chargepos.x() + module_size_x/2.;
    double y_new = chargepos.y() + module_size_y/2.;

    // -- change from module frame to pixel frame
    int nPixX = int(x_new/pixel_size_x);
    int nPixY = int(y_new/pixel_size_y);
    ATH_MSG_DEBUG(" -- nPixX = "<<nPixX<<"  nPixY = "<<nPixY);    
    double x_pix = x_new - pixel_size_x*(nPixX);
    double y_pix = y_new - pixel_size_y*(nPixY);
    // -- change origin of coordinates to the center of the pixel
    double x_pix_center = x_pix - pixel_size_x/2;
    double y_pix_center = y_pix - pixel_size_y/2;
    ATH_MSG_DEBUG(" -- current hit position w.r.t. pixel center = "<<x_pix_center<<"  "<<y_pix_center);    

    double x_neighbor;    double y_neighbor;   CLHEP::Hep3Vector pos_neighbor;
    // -- Calculate signal in current pixel and in the neighboring ones
    // -- loop in the x-coordinate
    for (int i=-1; i<=1; i++){
        x_neighbor = x_pix_center - i*pixel_size_x;
        // -- loop in the y-coordinate
        for (int j=-1; j<=1; j++){
          y_neighbor = y_pix_center - j*pixel_size_y;
         
          //ATH_MSG_INFO(" (i, j) "<<i<<" "<<j<<" before cut   x_neighbor  " << x_neighbor << " y_neighbor "<<y_neighbor);
          // -- check if the neighbor falls inside the charge collection prob map window
          if ( (fabs(x_neighbor)<pixel_size_x) && (fabs(y_neighbor)<pixel_size_y) ){

            // -- change origin of coordinates to the bottom left of the charge
            //    collection prob map "window", i.e. shift of 1-pixel twd bottom left
            double x_neighbor_map = x_neighbor + pixel_size_x;
            double y_neighbor_map = y_neighbor + pixel_size_y;

            int x_bin_cc_map = static_cast<int>(x_neighbor_map / x_bin_size);
            int y_bin_cc_map = static_cast<int>(y_neighbor_map / y_bin_size);

            // -- retrieve the charge collection probability from Svc
            // -- swap x and y bins to match Map coord convention
            double ccprob_neighbor = m_chargeCollSvc->getProbMapEntry("FEI4",y_bin_cc_map,x_bin_cc_map);
            if ( ccprob_neighbor == -1. ) return StatusCode::FAILURE;
            //ATH_MSG_INFO(" (i, j) "<<i<<" "<<j<<"  y_bin_cc_map   "<<y_bin_cc_map << "   x_bin_cc_map  " << x_bin_cc_map << " charge_coll neighbor "<<ccprob_neighbor);

            // double ed=es*electronHolePairsPerEnergy*ccprob_neighbor;
            double ed=es_current*electronHolePairsPerEnergy*ccprob_neighbor;

            // -- pixel coordinates --> module coordinates
            //double x_mod = x_neighbor - half_pixel_size_x + pixel_size_x*nPixX -module_size_x/2.;
            //double y_mod = y_neighbor - half_pixel_size_y + pixel_size_y*nPixY -module_size_y/2.;
            double x_mod = x_neighbor + pixel_size_x/2 + pixel_size_x*nPixX -module_size_x/2.;
            double y_mod = y_neighbor + pixel_size_y/2 + pixel_size_y*nPixY -module_size_y/2.;
            SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod,x_mod);
            //ATH_MSG_INFO(" Si3D charge pos "<<chargePos<<"  ed  "<<ed);
            
            SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,McLink));
            SiCellId diode = Module.cellIdOfPosition(scharge.position());
            SiCharge charge = scharge.charge();
            if (diode.isValid()) {
              chargedDiodes.add(diode,charge);
            }
          }
        }
    }
  }

  return StatusCode::SUCCESS;
}

