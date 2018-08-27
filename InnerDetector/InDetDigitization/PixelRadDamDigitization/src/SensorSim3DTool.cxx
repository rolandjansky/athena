/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
#include "SiDigitization/SiSurfaceCharge.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetIdentifier/PixelID.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SiPropertiesSvc/SiliconProperties.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
#include "CLHEP/Random/RandGaussZiggurat.h"
#include "CLHEP/Random/RandFlat.h"

//TODO: these includes are just for settign up the ModuleDesign ID, shouldn't be needed after stuff is stored in detector store
#include "Identifier/Identifier.h"
#include "InDetIdentifier/PixelID.h"

#include "TLorentzVector.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PathResolver/PathResolver.h"

//Temp includes : check if these are needed TODO
#include "TMath.h"
#include "TFile.h"
#include "TH3F.h"
#include "TH2.h"
#include "TH1.h"

using namespace InDetDD;
using namespace RadDam;

//===============================================
//    C O N S T R U C T O R
//===============================================
SensorSim3DTool::SensorSim3DTool(const std::string& type, const std::string& name,const IInterface* parent):
  SensorSimTool(type,name,parent),
  m_radDamageUtil(nullptr),
  m_numberOfSteps(50), //ALEX
  m_numberOfCharges(50),
  m_diffusionConstant(.0), //ALEX
  m_doRadDamage(true), //ALEX
  m_fluence(5), 
  m_trappingTimeElectrons(0.), //ALEX
  m_trappingTimeHoles(0.), //ALEX
  m_chargeCollSvc("ChargeCollProbSvc",name) //ALEX (COPIED)
{ 
  declareProperty("RadDamageUtil",   m_radDamageUtil, "Rad Damage utility");
  declareProperty("numberOfSteps",m_numberOfSteps,"Geant4:number of steps for PixelPlanar"); //ALEX: Pixel3D? many more in this file to change, etc
  declareProperty("numberOfCharges",m_numberOfCharges,"Geant4:number of charges for PixelPlanar"); //ALEX: Pixel3D?
  declareProperty("diffusionConstant",m_diffusionConstant,"Geant4:Diffusion Constant for PixelPlanar"); //ALEX: Pixel3D?
  declareProperty("doRadDamage",   m_doRadDamage, "doRadDmaage bool: should be flag");
  declareProperty("fluence",   m_fluence, "this is the fluence benchmark, 0-6.  0 is unirradiated, 1 is start of Run 2, 5 is end of 2018 and 6 is projected end of 2018");
  declareProperty("trappingTimeElectrons", m_trappingTimeElectrons, "Characteristic time till electron is trapped [ns]");
  declareProperty("trappingTimeHoles", m_trappingTimeHoles, "Characteristic time till hole is trapped [ns]");
  declareProperty("ChargeCollProbSvc",m_chargeCollSvc);
}

class DetCondCFloat;

SensorSim3DTool::~SensorSim3DTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSim3DTool::initialize() {
  CHECK(SensorSimTool::initialize()); 
  ATH_MSG_DEBUG ( "SensorSim3DTool::initialize()");
 
  CHECK(m_radDamageUtil.retrieve());
  ATH_MSG_DEBUG ( "RadDamageUtil tool retrieved successfully");

  CHECK(m_chargeCollSvc.retrieve());
  ATH_MSG_DEBUG ( "THIS TOOL RETRIEVED SUCCESFFULLY" );

  //Calculate trapping times based on fluence (already includes check for fluence=0)
  if(m_doRadDamage){

    std::pair<double,double> trappingTimes = m_radDamageUtil->getTrappingTimes( m_fluence );
    m_trappingTimeElectrons = trappingTimes.first;
    m_trappingTimeHoles = trappingTimes.second;
  }

  std::vector<std::string> mapsPath_list;

  if(m_fluence==0){

  }
  else if(m_fluence==1){

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl0em10.root") );  //IBL  PL - Barrel

    fluence_layers.push_back(1e-10);

  }
  else if(m_fluence==2){

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl1e14.root") );

    fluence_layers.push_back(1e14);

  }else if(m_fluence==3){

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl2e14.root") );

    fluence_layers.push_back(2e14);

  }else if(m_fluence==4){

    mapsPath_list.push_back(  PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_150V_fl2e14.root") );

    fluence_layers.push_back(2e14);

  }else if(m_fluence==5){

    mapsPath_list.push_back( PathResolverFindCalibFile("/afs/cern.ch/user/v/vewallan/public/TCADmaps/outputfiles/phi_1e14_20V.root") );
    ///cvmfs/atlas.cern.ch/repo/sw/database/GroupData/ <-- location for planar sensor maps
    fluence_layers.push_back(5e14);
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! FLUENCE 5 BEING USED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ATLAS 3D SENSOR !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LOADING THE 3D TCAD FLUENCE MAP !!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }else if(m_fluence==6){

    mapsPath_list.push_back( PathResolver::find_file("PixelDigitization/maps_IBL_400V_fl8_7e14.root", "DATAPATH") );

    fluence_layers.push_back(8.7e14);

  }else if(m_fluence==7){

    mapsPath_list.push_back( PathResolver::find_file("PixelDigitization/maps_IBL_endLHC.root", "DATAPATH") );

    fluence_layers.push_back(2*8.7e14);

  }
  
  // *****************************
  // *** Setup Maps ****
  // *****************************
  //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented 
      
  for(unsigned int i=0; i<mapsPath_list.size(); i++){

    ATH_MSG_INFO("Using maps located in: "<<mapsPath_list.at(i));
    //std::unique_ptr<TFile>  mapsFile=std::make_unique<TFile>( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.
    TFile* mapsFile=new TFile( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.

    std::pair<int, int> Layer;  // index for layer/end cap position
    Layer.first=0;  //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second=i; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2
    //IBL Barrel doesn't exist. So the possible idexes should be: 0-0, 0-1, 0-2, 0-3, 1-1, 1-2, 1-3
    //3D sensors only in IBL. So only possible index is 0-0

    //Setup ramo weighting field map
    TH3F* ramoPotentialMap_hold;
    ramoPotentialMap_hold=0;
    ramoPotentialMap_hold=(TH3F*)mapsFile->Get("ramo");
    //ALEX if (ramoPotentialMap_hold==0) ramoPotentialMap_hold=(TH3F*)mapsFile->Get("ramo3d");
    if (ramoPotentialMap_hold==0){
    ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
    return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
    //TODO
    //     CHECK(m_radDamageUtil->generateRamoMap( ramoPotentialMap, p_design_dummy ));
    }
    //ramoPotentialMap.push_back(ramoPotentialMap_hold);
    ramoPotentialMap[Layer]=ramoPotentialMap_hold;
    fluence_layersMaps[Layer]=fluence_layers.at(i);
    //Now setup the E-field.
    TH2F* eFieldMap_hold;
    eFieldMap_hold=0;
    eFieldMap_hold=(TH2F*)mapsFile->Get("efield");
    if (eFieldMap_hold == 0){ 
    ATH_MSG_INFO("Unable to load sensor e-field map, so generating one using approximations.");
    return StatusCode::FAILURE;//Obviously, remove this when gen. code is set up
    //TODO
    // CHECK(m_radDamageUtil->generateEfieldMap( eFieldMap, p_design_dummy ));
    }
    //eFieldMap.push_back(eFieldMap_hold);
    eFieldMap[Layer]=eFieldMap_hold;

    TH3F* xPositionMap_e_hold;
    TH3F* xPositionMap_h_hold;
    TH3F* yPositionMap_e_hold;
    TH3F* yPositionMap_h_hold;
    TH2F* timeMap_e_hold;
    TH2F* timeMap_h_hold;

    xPositionMap_e_hold=0;
    xPositionMap_h_hold=0;
    yPositionMap_e_hold=0;
    yPositionMap_h_hold=0;
    timeMap_e_hold=0;
    timeMap_h_hold=0;
    xPositionMap_e_hold=(TH3F*)mapsFile->Get("xPosition_e");
    xPositionMap_h_hold=(TH3F*)mapsFile->Get("xPosition_h");
    yPositionMap_e_hold=(TH3F*)mapsFile->Get("yPosition_e");
    yPositionMap_h_hold=(TH3F*)mapsFile->Get("yPosition_h");
    timeMap_e_hold=(TH2F*)mapsFile->Get("etimes");
    timeMap_h_hold=(TH2F*)mapsFile->Get("htimes");
    //Now, determine the time to reach the electrode and the trapping position.
    if (xPositionMap_e_hold == 0 || xPositionMap_h_hold == 0 || yPositionMap_e_hold == 0 || yPositionMap_h_hold == 0 || timeMap_e_hold == 0 || timeMap_h_hold == 0){
    
      ATH_MSG_INFO("Unable to load at least one of teh distance/time maps, so generating all using approximations.");
      return StatusCode::FAILURE;//Obviously, remove this when gen. code is set up
      //TODO
      //CHECK(m_radDamageUtil->generateDistanceTimeMap( distanceMap_e, distanceMap_h, timeMap_e, timeMap_h, eFieldMap, p_design_dummy ));
    }
    xPositionMap_e[Layer]=xPositionMap_e_hold;
    xPositionMap_h[Layer]=xPositionMap_h_hold;
    yPositionMap_e[Layer]=yPositionMap_e_hold;
    yPositionMap_h[Layer]=xPositionMap_h_hold;
    timeMap_e[Layer]=timeMap_e_hold;
    timeMap_h[Layer]=timeMap_h_hold;
    
  }
  std::cout << "!!!!!!! MAP SETUP SUCCESSFUL !!!!!!!!!!!!!" << std::endl;
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
StatusCode SensorSim3DTool::induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection &chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions) {


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
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());
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


StatusCode SensorSim3DTool::applySlimEdges( double &energy_per_step, double &eta_drifted){

        if(fabs(eta_drifted) > 20.440)energy_per_step=0.;
        if(fabs(eta_drifted)< 20.440 && fabs(eta_drifted)> 20.200){
          if(eta_drifted>0){
            energy_per_step=energy_per_step*(68.13-eta_drifted*3.333);            
            eta_drifted = eta_drifted - 0.250;
          }else{  
            energy_per_step=energy_per_step*(68.13+eta_drifted*3.333);            
            eta_drifted = eta_drifted + 0.250;
          }  
        }
        if(fabs(eta_drifted)< 20.200 && fabs(eta_drifted)> 20.100){
          if(eta_drifted>0){
            energy_per_step=energy_per_step*(41.2-eta_drifted*2.);             
            eta_drifted = eta_drifted - 0.250;
          }else{  
            energy_per_step=energy_per_step*(41.2+eta_drifted*2.);            
            eta_drifted = eta_drifted + 0.250;
          }  
        }

  return StatusCode::SUCCESS;

}

StatusCode SensorSim3DTool::getTrappingPositionX(double initX, double initY, double driftTime, bool isHoleBit){
	std::pair<int, int> Layer;  // index for layer/end cap position
	Layer.first=0;  //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
	Layer.second=0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

	double finalX=initX;
	if (!isHoleBit)	{
		int n_binx = xPositionMap_e[Layer]->GetXaxis()->FindBin(initX*1000);
		int n_biny = xPositionMap_e[Layer]->GetYaxis()->FindBin(initY*1000);
		int n_bint = xPositionMap_e[Layer]->GetZaxis()->FindBin(driftTime);
		finalX = xPositionMap_e[Layer]->GetBinContent(n_binx,n_biny,n_bint);
	} else {
		int n_binx = xPositionMap_h[Layer]->GetXaxis()->FindBin(initX*1000);
		int n_biny = xPositionMap_h[Layer]->GetYaxis()->FindBin(initY*1000);
		int n_bint = xPositionMap_h[Layer]->GetZaxis()->FindBin(driftTime);
		finalX = xPositionMap_h[Layer]->GetBinContent(n_binx,n_biny,n_bint);
	}

  return finalX*1e-3; //[mm]
}

StatusCode SensorSim3DTool::getTrappingPositionY(double	initX, double initY, double driftTime, bool isHoleBit){
        std::pair<int, int> Layer;  // index for layer/end cap position
        Layer.first=0;  //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
        Layer.second=0; //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2

        double finalY=initY;
        if (!isHoleBit) {
                int n_binx = yPositionMap_e[Layer]->GetXaxis()->FindBin(initX*1000);
                int n_biny = yPositionMap_e[Layer]->GetYaxis()->FindBin(initY*1000);
               	int n_bint = yPositionMap_e[Layer]->GetZaxis()->FindBin(driftTime);
                finalY = yPositionMap_e[Layer]->GetBinContent(n_binx,n_biny,n_bint);
        } else {
               	int n_binx = yPositionMap_h[Layer]->GetXaxis()->FindBin(initX*1000);
                int n_biny = yPositionMap_h[Layer]->GetYaxis()->FindBin(initY*1000);
                int n_bint = yPositionMap_h[Layer]->GetZaxis()->FindBin(driftTime);
                finalY = yPositionMap_h[Layer]->GetBinContent(n_binx,n_biny,n_bint);
       	}

  return finalY*1e-3; //[mm]
}

//TODO: Lorentz angle: correct calculation. To test at a later time
        /*
        double max_dfe  = (lorentzMap_e->GetNbinsY())*(lorentzMap_e->GetYaxis()->GetBinWidth(2));
        if(depth_f_e >= max_dfe) depth_f_e=max_dfe- 0.5*(lorentzMap_e->GetYaxis()->GetBinWidth(2));

        int  nbin = lorentzMap_e->FindBin(dist_electrode,depth_f_e);
        tanLorentz = lorentzMap_e->GetBinContent(nbin);
        double tanlor_e = tanLorentz;
       
	double coLorentz=sqrt(1+pow(tanLorentz,2));
        double max_dfh  = (lorentzMap_h->GetNbinsY())*(lorentzMap_h->GetYaxis()->GetBinWidth(2));
        double max_di  = (lorentzMap_h->GetNbinsX())*(lorentzMap_h->GetXaxis()->GetBinWidth(2));
        double appo_fh = depth_f_h;
        if(depth_f_h >= max_dfh) appo_fh=max_dfh - 0.5*(lorentzMap_h->GetYaxis()->GetBinWidth(2)); 
        nbin = lorentzMap_h->FindBin(dist_electrode,appo_fh);
        tanLorentz = lorentzMap_h->GetBinContent(nbin) ;
        double tanlor_h = tanLorentz;
        
        coLorentz=sqrt(1+pow(tanLorentz,2));

       */
