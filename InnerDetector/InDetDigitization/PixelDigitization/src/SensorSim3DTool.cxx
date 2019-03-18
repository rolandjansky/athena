/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSim3DTool.cxx
//   Implementation file for class SensorSim3DTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SensorSim3DTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetReadoutGeometry/PixelModuleDesign.h"
#include "InDetSimEvent/SiHit.h"
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesTool/SiliconProperties.h"

#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Random/RandGaussZiggurat.h"

#include "TLorentzVector.h"

using namespace InDetDD;


//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSim3DTool::SensorSim3DTool(const std::string& type, const std::string& name,const IInterface* parent):
  SensorSimTool(type,name,parent),
  m_numberOfSteps(50),
  m_chargeCollSvc("ChargeCollProbSvc",name)
{ 
  declareProperty("numberOfSteps",m_numberOfSteps,"Number of steps for Pixel3D module");
  declareProperty("ChargeCollProbSvc",m_chargeCollSvc);
}

class DetCondCFloat;

// Destructor:
SensorSim3DTool::~SensorSim3DTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSim3DTool::initialize() {
  ATH_CHECK(SensorSimTool::initialize());
 	  
  // -- Get ChargeCollProb  Service
  ATH_CHECK(m_chargeCollSvc.retrieve());

  ATH_MSG_DEBUG("SensorSim3DTool::initialize()");
  return StatusCode::SUCCESS;
}

//===============================================
//    F I N A L I Z E
//===============================================
StatusCode SensorSim3DTool::finalize() {
  ATH_MSG_DEBUG("SensorSim3DTool::finalize()");
  return StatusCode::SUCCESS;
}

//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSim3DTool::induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection &chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions, CLHEP::HepRandomEngine * /*rndmEngine*/) {


  if (!Module.isBarrel()) { return StatusCode::SUCCESS; }
  if (p_design.getReadoutTechnology()!=InDetDD::PixelModuleDesign::FEI4) { return StatusCode::SUCCESS; }
  if (p_design.numberOfCircuits()>1) { return StatusCode::SUCCESS; }

 ATH_MSG_DEBUG("Applying SensorSim3D charge processor");
  if( initialConditions.size() != 8 ){
	  ATH_MSG_INFO("ERROR! Starting coordinates were not filled correctly in EnergyDepositionSvc.");
	  return StatusCode::FAILURE;
  }

  double eta_0 = initialConditions[0];
  double phi_0 = initialConditions[1];
  double depth_0  = initialConditions[2];
  double dEta = initialConditions[3];
  double dPhi = initialConditions[4];
  double dDepth = initialConditions[5];
  double iTotalLength = initialConditions[7];


  ATH_MSG_VERBOSE("Applying 3D sensor simulation.");
  double sensorThickness = Module.design().thickness();
  const InDet::SiliconProperties & siProperties = m_siPropertiesTool->getSiProperties(Module.identifyHash());
  double eleholePairEnergy = siProperties.electronHolePairsPerEnergy();

  // Charge Collection Probability Map bin size
  const double x_bin_size = 0.001;
  const double y_bin_size = 0.001;
 
  std::string readout;
 
  // determine which readout is used
  // FEI4 : 50 X 250 microns
  double pixel_size_x = Module.width()/p_design.rows();
  double pixel_size_y = Module.length()/p_design.columns();
  double module_size_x = Module.width();
  double module_size_y = Module.length();
  

  //**************************************//
  //*** Now diffuse charges to surface *** //
  //**************************************//
  for(unsigned int istep = 0; istep < trfHitRecord.size(); istep++) {
    std::pair<double,double> iHitRecord = trfHitRecord[istep];

    double eta_i = eta_0;
    double phi_i = phi_0;
    double depth_i = depth_0;

    if (iTotalLength) {
      eta_i += 1.0*iHitRecord.first/iTotalLength*dEta;
      phi_i += 1.0*iHitRecord.first/iTotalLength*dPhi;
      depth_i  += 1.0*iHitRecord.first/iTotalLength*dDepth;
    }

    double es_current = 1.0*iHitRecord.second/1.E+6;

    double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
    if (dist_electrode<0) dist_electrode=0;

    CLHEP::Hep3Vector chargepos;
    chargepos.setX(phi_i); chargepos.setY(eta_i);  chargepos.setZ(dist_electrode);

    bool coord = Module.isModuleFrame();

    ATH_MSG_DEBUG("ismoduleframe "<<coord << " -- startPosition (x,y,z) = " << chargepos.x() << ", " << chargepos.y() << ", " << chargepos.z());

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

          double ed=es_current*eleholePairEnergy*ccprob_neighbor;

          // -- pixel coordinates --> module coordinates
          //double x_mod = x_neighbor - half_pixel_size_x + pixel_size_x*nPixX -module_size_x/2.;
          //double y_mod = y_neighbor - half_pixel_size_y + pixel_size_y*nPixY -module_size_y/2.;
          double x_mod = x_neighbor + pixel_size_x/2 + pixel_size_x*nPixX -module_size_x/2.;
          double y_mod = y_neighbor + pixel_size_y/2 + pixel_size_y*nPixY -module_size_y/2.;
          SiLocalPosition chargePos = Module.hitLocalToLocal(y_mod,x_mod);

          SiSurfaceCharge scharge(chargePos,SiCharge(ed,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));
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
