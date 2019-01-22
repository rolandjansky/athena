/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
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
SensorSimPlanarTool::SensorSimPlanarTool(const std::string& type, const std::string& name,const IInterface* parent):
  SensorSimTool(type,name,parent),
  m_radDamageUtil(nullptr),
  m_numberOfSteps(50),
  m_numberOfCharges(10),
  m_diffusionConstant(.0),
  m_doRadDamage(true),
  m_trappingTimeElectrons(0.),
  m_trappingTimeHoles(0.),
  m_fluence(5), 
  m_fluenceB(-1),       //e14 neq/cm^2  //if negative do not use interpolation maps
  m_fluence1(-1),       //if negative do not use interpolation maps but maps precomputed for benchmark indicated in fluence
  m_fluence2(-1), 
  m_voltage (-1),       //Volt
  m_voltageB(-1),       
  m_voltage1(-1), 
  m_voltage2(-1) 
{ 
  declareProperty("RadDamageUtil",      m_radDamageUtil, "Rad Damage utility");
  declareProperty("numberOfSteps",      m_numberOfSteps,"Geant4:number of steps for PixelPlanar");
  declareProperty("numberOfCharges",    m_numberOfCharges,"Geant4:number of charges for PixelPlanar");
  declareProperty("diffusionConstant",  m_diffusionConstant,"Geant4:Diffusion Constant for PixelPlanar");
  declareProperty("doRadDamage",        m_doRadDamage, "doRadDmaage bool: should be flag");
  declareProperty("trappingTimeElectrons", m_trappingTimeElectrons, "Characteristic time till electron is trapped [ns]");
  declareProperty("trappingTimeHoles",  m_trappingTimeHoles, "Characteristic time till hole is trapped [ns]");
  declareProperty("fluence",            m_fluence,  "this is the fluence benchmark, 0-6.  0 is unirradiated, 1 is start of Run 2, 5 is end of 2018 and 6 is projected end of 2018");
  declareProperty("fluenceB",           m_fluenceB, "fluence detector has recieved in neqcm2 at the B layer.");
  declareProperty("fluence1",           m_fluence1, "fluence detector has recieved in neqcm2 at the layer 1.");
  declareProperty("fluence2",           m_fluence2, "fluence detector has recieved in neqcm2 at the layer 2.");
  declareProperty("voltage",            m_voltage,  "this is the bias voltage applied to the IBL - if not set use values from applied at benchmark points according to fluence");
  declareProperty("voltageB",           m_voltageB, "bias voltage applied to the B layer.");
  declareProperty("voltage1",           m_voltage1, "bias voltage applied to the layer 1.");
  declareProperty("voltage2",           m_voltage2, "bias voltage applied to the layer 2.");
}

class DetCondCFloat;

SensorSimPlanarTool::~SensorSimPlanarTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSimPlanarTool::initialize() {
  CHECK(SensorSimTool::initialize()); 
  ATH_MSG_DEBUG ( "SensorSimPlanarTool::initialize()");
 
  CHECK(m_radDamageUtil.retrieve());
  ATH_MSG_DEBUG ( "RadDamageUtil tool retrieved successfully");

  //Calculate trapping times based on fluence (already includes check for fluence=0)
  if(m_doRadDamage){

    std::pair<double,double> trappingTimes = m_radDamageUtil->getTrappingTimes( m_fluence );
    m_trappingTimeElectrons = trappingTimes.first;
    m_trappingTimeHoles = trappingTimes.second;
  }
    //If any fluence or voltage initialized negative use benchmark maps and not interpolation
    bool doInterpolateEfield = (m_fluence > 0. && m_fluenceB > 0.  && m_fluence1 > 0. && m_fluence2 > 0. && m_voltage > 0. && m_voltageB > 0.  && m_voltage1 > 0. && m_voltage2 > 0.);
    std::vector<std::string> mapsPath_list;
    std::vector<std::string> TCADpath_list;

    // Use all TCAD E field files in this directory for creating E field via interpolation (pruned filed excluded) 
    std::string iblFiles        = PathResolverFindCalibDirectory("PixelDigitization/TCAD_IBL_efields/fei4-200um/");
    std::string sensorFiles     = PathResolverFindCalibDirectory("PixelDigitization/TCAD_Blayer_efields/fei4-250um/");

    // For each layer one configuration
    TCADpath_list = {iblFiles, sensorFiles, sensorFiles, sensorFiles};           //IBL - 200um sensor depth, B layer - 20um, layer 1, layer 2
//    Replpicate the sesensorFiles files to afs/GroupData https://twiki.cern.ch/twiki/bin/view/AtlasComputing/PathResolver
//    If you want only certain TCAD E field files to be used to create an interpolated E field, hand over a list like these instead of the directory
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/ibl_TCAD_EfieldProfiles.txt"   );
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");
    
  if(m_fluence==0){

  }
  else if(m_fluence==1){
    ATH_MSG_INFO("Use benchmark point 1!");
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl0em10.root") );  //IBL  PL - Barrel
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl7e13.root") );    //B-Layer - Barrel                                                                                                  
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl3e13.root") );    //Layer-1 - Barrel
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl2e13.root") );    //Layer-2 - Barrel

    m_fluence_layers.push_back(1e-10);
    m_fluence_layers.push_back(7e13);
    m_fluence_layers.push_back(3e13);
    m_fluence_layers.push_back(2e13);

    m_voltage_layers.push_back(80);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
  }
  else if(m_fluence==2){
    ATH_MSG_INFO("Use benchmark point 2!");

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl1e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl1.2e14.root") );                                                                                                            
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl5e13.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl3e13.root") );

    m_fluence_layers.push_back(1e14);
    m_fluence_layers.push_back(1.2e14);
    m_fluence_layers.push_back(5e13);
    m_fluence_layers.push_back(3e13);

    m_voltage_layers.push_back(80);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
  }else if(m_fluence==3){
    ATH_MSG_INFO("Use benchmark point 3!");
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl2e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl1.7e14.root") );                                                                                                            
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl7e13.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl4e13.root") );

    m_fluence_layers.push_back(2e14);
    m_fluence_layers.push_back(1.7e14);
    m_fluence_layers.push_back(7e13);
    m_fluence_layers.push_back(4e13);

    m_voltage_layers.push_back(80);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(150);
  }else if(m_fluence==4){
    ATH_MSG_INFO("Use benchmark point 4!");

    mapsPath_list.push_back(  PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_150V_fl2e14.root") );
    mapsPath_list.push_back(  PathResolverFindCalibFile("PixelDigitization/maps_PIX_350V_fl1.7e14.root") );                                                                                                            
    mapsPath_list.push_back(  PathResolverFindCalibFile("PixelDigitization/maps_PIX_250V_fl7e13.root") );
    mapsPath_list.push_back(  PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl4e13.root") );

    m_fluence_layers.push_back(2e14);
    m_fluence_layers.push_back(1.7e14);
    m_fluence_layers.push_back(7e13);
    m_fluence_layers.push_back(4e13);

    m_voltage_layers.push_back(150);
    m_voltage_layers.push_back(350);
    m_voltage_layers.push_back(250);
    m_voltage_layers.push_back(150);
  }else if(m_fluence==5){
    ATH_MSG_INFO("Use benchmark point 5!");

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_350V_fl5e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_350V_fl3.1e14.root") );                                                                                                            
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_250V_fl1.3e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl8e13.root") );

    m_fluence_layers.push_back(5e14);
    m_fluence_layers.push_back(3.1e14);
    m_fluence_layers.push_back(1.3e14);
    m_fluence_layers.push_back(8e13);

    m_voltage_layers.push_back(350);
    m_voltage_layers.push_back(350);
    m_voltage_layers.push_back(250);
    m_voltage_layers.push_back(150);
  }else if(m_fluence==6){
    ATH_MSG_INFO("Use benchmark point 6!");

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_400V_fl8_7e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_400V_fl4_6e14.root") );
    
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_250V_fl2_1e14.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl1_3e14.root") );

    m_fluence_layers.push_back(8.7e14);
    m_fluence_layers.push_back(4.6e14);
    m_fluence_layers.push_back(2.1e14);
    m_fluence_layers.push_back(1.3e14);

    m_voltage_layers.push_back(400);
    m_voltage_layers.push_back(400);
    m_voltage_layers.push_back(250);
    m_voltage_layers.push_back(150);
  }else if(m_fluence==7){
    ATH_MSG_INFO("Use benchmark point 7!");

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_endLHC.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_blayer_endLHC.root") );

    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_L1_endLHC.root") );
    mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_L2_endLHC.root") );

    m_fluence_layers.push_back(2*8.7e14);
    m_fluence_layers.push_back(2*4.6e14);
    m_fluence_layers.push_back(2*2.1e14);
    m_fluence_layers.push_back(2*1.3e14);

    m_voltage_layers.push_back(350);
    m_voltage_layers.push_back(350);
    m_voltage_layers.push_back(250);
    m_voltage_layers.push_back(150);
  }
 
    if(mapsPath_list.size()==0 && doInterpolateEfield){
        ATH_MSG_INFO("No benchmark value set for fluence. Use interpolation.");
        mapsPath_list.clear();
        m_fluence_layers.clear();  
        m_voltage_layers.clear();
        //Set up default maps for ramoMap,
        //but retrieve Efield from interpolation as well as Lorentz, time and distance map from E field
        mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_IBL_PL_80V_fl0em10.root") );  //IBL  PL - Barrel
        mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl7e13.root") );    //B-Layer - Barrel 
        mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl3e13.root") );    //Layer-1 - Barrel 
        mapsPath_list.push_back( PathResolverFindCalibFile("PixelDigitization/maps_PIX_150V_fl2e13.root") );    //Layer-2 - Barrel 
        m_fluence_layers.push_back(m_fluence*1e14);
        m_fluence_layers.push_back(m_fluenceB*1e14);
        m_fluence_layers.push_back(m_fluence1*1e14);
        m_fluence_layers.push_back(m_fluence2*1e14);
        
        m_voltage_layers.push_back(m_voltage);
        m_voltage_layers.push_back(m_voltageB);
        m_voltage_layers.push_back(m_voltage1);
        m_voltage_layers.push_back(m_voltage2);
    }else{
        if(!doInterpolateEfield){
            ATH_MSG_INFO("m_fluence set to becnhmark value. Use becnhmark values for all layers.");
        }else{
            ATH_MSG_INFO("No BM values found and no appropriate values for fluence/bias voltage set");
            return StatusCode::FAILURE; 
        }
    }
    
    ATH_MSG_INFO("ibl       : Phi=" << m_fluence_layers.at(0) << "e14neq/cm2 U="<< m_voltage_layers.at(0) << "V");
    ATH_MSG_INFO("B layer   : Phi=" << m_fluence_layers.at(1) << "e14neq/cm2 U="<< m_voltage_layers.at(1) << "V");
    ATH_MSG_INFO("layer 1   : Phi=" << m_fluence_layers.at(2) << "e14neq/cm2 U="<< m_voltage_layers.at(2) << "V");
    ATH_MSG_INFO("layer 2   : Phi=" << m_fluence_layers.at(3) << "e14neq/cm2 U="<< m_voltage_layers.at(3) << "V");

  // *****************************
  // *** Setup Maps ****
  // *****************************
  //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented 
  //E field already implemented: needs fluence and bias voltage given as Property m_fluence, m_fluenceB, ...,  m_fluence1, ...
  for(unsigned int i=0; i<mapsPath_list.size(); i++){

    ATH_MSG_INFO("Using maps located in: "<<mapsPath_list.at(i) << " for layer No." << i);
    if(doInterpolateEfield)ATH_MSG_INFO("Create E field via interpolation based on files from: " << TCADpath_list.at(i));
    //std::unique_ptr<TFile>  mapsFile=std::make_unique<TFile>( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.
    TFile* mapsFile=new TFile( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.

    std::pair<int, int> Layer;  // index for layer/end cap position
    Layer.first=0;              //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second=i;             //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2
    //IBL Barrel doesn't exist. So the possible idexes should be: 0-0, 0-1, 0-2, 0-3, 1-1, 1-2, 1-3

    //Setup ramo weighting field map
    TH3F* ramoPotentialMap_hold;
    ramoPotentialMap_hold=0;
    ramoPotentialMap_hold=(TH3F*)mapsFile->Get("hramomap1");
    if (ramoPotentialMap_hold==0) ramoPotentialMap_hold=(TH3F*)mapsFile->Get("ramo3d");
    if (ramoPotentialMap_hold==0){
        ATH_MSG_INFO("Did not find a Ramo potential map.  Will use an approximate form.");
        ATH_MSG_WARNING("Not implemented yet - exit");
        return StatusCode::FAILURE; //Obviously, remove this when gen. code is set up
        //TODO -- is actually implemented?!
        //     CHECK(m_radDamageUtil->generateRamoMap( ramoPotentialMap, p_design_dummy ));
    }
    ramoPotentialMap[Layer]=ramoPotentialMap_hold;
    m_fluence_layersMaps[Layer]=m_fluence_layers.at(i);
    //Now setup the E-field.
    TH1F* eFieldMap_hold;
    eFieldMap_hold= new TH1F() ;
    if(doInterpolateEfield){
        //ATH_MSG_INFO("Generating E field maps using interpolation.");
        CHECK(m_radDamageUtil->generateEfieldMap( eFieldMap_hold, NULL, m_fluence_layers.at(i), m_voltage_layers.at(i), i, TCADpath_list.at(i), true));
    }else{
        //precomputed map
        eFieldMap_hold=(TH1F*)mapsFile->Get("hEfield1D"); 
    }

    if (eFieldMap_hold == 0){ 
         ATH_MSG_INFO("Unable to load sensor e-field map.");
         return StatusCode::FAILURE;
     }
    //eFieldMap.push_back(eFieldMap_hold);
    eFieldMap[Layer]=eFieldMap_hold;

    TH2F* lorentzMap_e_hold    =new TH2F() ;
    TH2F* lorentzMap_h_hold    =new TH2F() ;
    TH2F* distanceMap_h_hold   =new TH2F() ;
    TH2F* distanceMap_e_hold   =new TH2F() ;
    TH1F* timeMap_e_hold       =new TH1F() ;
    TH1F* timeMap_h_hold       =new TH1F() ;

    
    
    if(doInterpolateEfield){
        CHECK(m_radDamageUtil->generateDistanceTimeMap( distanceMap_e_hold, distanceMap_h_hold, timeMap_e_hold, timeMap_h_hold, lorentzMap_e_hold, lorentzMap_h_hold, eFieldMap_hold, NULL ));
        // For debugging and documentation: uncomment to save different maps which are based on the interpolated E field
        //TH2F*   clonelorentzMap_e_hold    =(TH2F*) lorentzMap_e_hold ->Clone()   ;
        //TH2F*   clonelorentzMap_h_hold    =(TH2F*) lorentzMap_h_hold ->Clone()   ;
        //TH2F*   clonedistanceMap_h_hold   =(TH2F*) distanceMap_h_hold->Clone()   ;
        //TH2F*   clonedistanceMap_e_hold   =(TH2F*) distanceMap_e_hold->Clone()   ;
        //TH1F*   clonetimeMap_e_hold       =(TH1F*) timeMap_e_hold    ->Clone()   ;
        //TH1F*   clonetimeMap_h_hold       =(TH1F*) timeMap_h_hold    ->Clone()   ;
        //TString prename = "map_layer_";
        //prename += i;
        //prename += "distance_e.root";
        //clonedistanceMap_e_hold->SaveAs(prename);
        //prename.ReplaceAll("_e", "_h");
        //clonedistanceMap_h_hold->SaveAs(prename);
        //prename.ReplaceAll("distance","time");
        //clonetimeMap_h_hold->SaveAs(prename);
        //prename.ReplaceAll( "_h","_e");
        //clonetimeMap_e_hold->SaveAs(prename);
        //prename.ReplaceAll("time", "lorentz");
        //clonelorentzMap_e_hold->SaveAs(prename);
        //prename.ReplaceAll( "_e","_h");
        //clonelorentzMap_h_hold->SaveAs(prename);
    }else{
        //retrieve precomputed maps
        lorentzMap_e_hold=(TH2F*)mapsFile->Get("lorentz_map_e");
        lorentzMap_h_hold=(TH2F*)mapsFile->Get("lorentz_map_h");
        distanceMap_h_hold=(TH2F*)mapsFile->Get("hdistance");
        distanceMap_e_hold=(TH2F*)mapsFile->Get("edistance");
        timeMap_e_hold=(TH1F*)mapsFile->Get("etimes");
        timeMap_h_hold=(TH1F*)mapsFile->Get("htimes");
    }
    //Safetycheck
    if (distanceMap_e_hold == 0 || distanceMap_h_hold == 0 || timeMap_e_hold == 0 || timeMap_h_hold == 0 || lorentzMap_e_hold == 0 || lorentzMap_h_hold == 0){
        ATH_MSG_INFO("Unable to load at least one of the distance/time/Lorentz angle maps.");
        return StatusCode::FAILURE;//Obviously, remove this when gen. code is set up
    }
    lorentzMap_e[Layer]=lorentzMap_e_hold;
    lorentzMap_h[Layer]=lorentzMap_h_hold;
    distanceMap_e[Layer]=distanceMap_e_hold;
    distanceMap_h[Layer]=distanceMap_h_hold;
    timeMap_e[Layer]=timeMap_e_hold;
    timeMap_h[Layer]=timeMap_h_hold;
  }
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
StatusCode SensorSimPlanarTool::induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection &chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions) {

  // So far, this is only discriminating variable from 3D sensor.
  if (p_design.numberOfCircuits()<2){
    if(!Module.isDBM()) {  //DBM modules also processed here
      return StatusCode::SUCCESS; 
    }
  }

    bool isBarrel = false;

    const PixelID* p_pixelId = static_cast<const PixelID *>(Module.getIdHelper());
    int layer=p_pixelId->layer_disk(Module.identify() );
    int bec=1;
    if(p_pixelId->is_barrel(Module.identify()) ) bec=0 ;
    
    if(Module.isBarrel()) isBarrel = true;

    std::pair<int, int> Layer;        // index for layer/end cap position
    Layer.first=bec;                  //Barrel (0) or End Cap (1)   -    Maps only for Barrel at the moment. isBarrel will avoid zsh
    Layer.second=layer;               //Layer: 0 = IBL Planar, 1=B-Layer, 2=Layer-1, 3=Layer-2
                                      //IBL Barrel doesn't exist. So the possible idexes should be: 0-0, 0-1, 0-2, 0-3, 1-1, 1-2, 1-3

    if(m_doRadDamage && isBarrel && m_fluence>0){
     std::pair<double,double> trappingTimes = m_radDamageUtil->getTrappingTimes( m_fluence_layersMaps[Layer] );
     m_trappingTimeElectrons = trappingTimes.first;
     m_trappingTimeHoles = trappingTimes.second;
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
  const InDet::SiliconProperties & siProperties = m_siPropertiesSvc->getSiProperties(Module.identifyHash());

  int etaCells = p_design.columns();
  int phiCells = p_design.rows();

  double eleholePairEnergy = 0;
  double smearRand = 0;

  if (Module.isDBM()){
    eleholePairEnergy = 1. / (13. * CLHEP::eV); // was 3.62 eV.
    m_diffusionConstant = .00265;//diffusion contant for DBM - not changed wrt previous version of digitization. Does someone know where this comes from?
    smearRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
  }
  else{
    eleholePairEnergy = siProperties.electronHolePairsPerEnergy(); // = 1 / 3.6 eV -> expressed in MeV^-1 = 276243 MeV^-1
    m_diffusionConstant = .007;//diffusion length used in simulation, consistent with current simluation and previous version of digitization. Eventually to be changed to proper Einstein relation
 }

  double collectionDist = 0.2*CLHEP::mm;//kept for consistency with previosu version of digi. Does someone know where this number comes from?
  double smearScale = 1. + 0.35*smearRand;//ditto...
  double tanLorentz = Module.getTanLorentzAnglePhi();
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

    //Find the position of the centre of the pixel in which the charge carriers are created, wrt centre of module
    SiLocalPosition pos_i = Module.hitLocalToLocal(eta_i,phi_i);
    SiCellId pixel_i = Module.cellIdOfPosition( pos_i );

    //Check that the HIT isn't outside of the pixel matrix
    if(  pixel_i.etaIndex() < 0 || pixel_i.phiIndex() < 0 || pixel_i.etaIndex() > std::pow(2.,10) || pixel_i.phiIndex() > std::pow(2.,10) ){
      continue;
    }

    SiLocalPosition centreOfPixel_i;
    double pixelEta_i = 0.;
    double pixelPhi_i =0.;

    int nnLoop_pixelEtaMax =0; 
    int nnLoop_pixelEtaMin=0; 
    int nnLoop_pixelPhiMax=0; 
    int nnLoop_pixelPhiMin=0; 

    int numBins_driftTime_e=0;
    int numBins_driftTime_h=0;
    int numBins_weightingPotential_x=0;
    int numBins_weightingPotential_y=0;
    int numBins_weightingPotential_z=0;

    if (m_doRadDamage && m_fluence>0 && !(Module.isDBM()) && isBarrel) {
        centreOfPixel_i = p_design.positionFromColumnRow(pixel_i.etaIndex(), pixel_i.phiIndex());

        //Find the displacment of the charge carriers from the centre of the pixel in +ve quadrant
        pixelEta_i =  eta_i - centreOfPixel_i.xEta();
        pixelPhi_i =  phi_i - centreOfPixel_i.xPhi();
        
	//Make limits for NN loop
        nnLoop_pixelEtaMax = std::min( 2,pixel_i.etaIndex() );
        nnLoop_pixelEtaMin = std::max( -2, pixel_i.etaIndex() + 1 - etaCells );
	
        nnLoop_pixelPhiMax = std::min( 2,pixel_i.phiIndex() );
        nnLoop_pixelPhiMin = std::max( -2, pixel_i.phiIndex() + 1 - phiCells );
	
	//Setup values to check for overflow when using maps
	numBins_driftTime_e = distanceMap_e[Layer]->GetNbinsY(); //Returns nBins = totalBins - underflow - overflow 
	numBins_driftTime_h = distanceMap_h[Layer]->GetNbinsY();   
	
        numBins_weightingPotential_x = ramoPotentialMap[Layer]->GetNbinsX();
        numBins_weightingPotential_y = ramoPotentialMap[Layer]->GetNbinsY();
        numBins_weightingPotential_z = ramoPotentialMap[Layer]->GetNbinsZ();        

    }
    
    // Distance between charge and readout side.  p_design->readoutSide() is
    // +1 if readout side is in +ve depth axis direction and visa-versa.
    double dist_electrode = 0.5 * sensorThickness - Module.design().readoutSide() * depth_i;
    if (dist_electrode<0) dist_electrode=0;

     // nonTrapping probability: DBM trapping, independant of radDamage 
    double nontrappingProbability = 1.0;
    if (Module.isDBM()){
      nontrappingProbability = exp(-dist_electrode/collectionDist);
    }
 
    // amount of energy to be converted into charges at current step
    double energy_per_step = 1.0*iHitRecord.second/1.E+6/ncharges;

    //Loop over charge-carrier pairs
    for(int j=0 ; j<ncharges ; j++) {

    if( m_doRadDamage && m_fluence>0 && !(Module.isDBM()) && isBarrel){
        double u = CLHEP::RandFlat::shoot(0.,1.);
        double drifttime_e = (-1.)*m_trappingTimeElectrons*TMath::Log(u); //ns
        u = CLHEP::RandFlat::shoot(0.,1.);
        double drifttime_h = (-1.)*m_trappingTimeHoles*TMath::Log(u); //ns

	//Now, need the z-position at the trap.
        int nbin_z_e_xbin = distanceMap_e[Layer]->GetXaxis()->FindBin(dist_electrode);
        int nbin_z_e_ybin = distanceMap_e[Layer]->GetYaxis()->FindBin(drifttime_e);
        if (nbin_z_e_ybin >  numBins_driftTime_e  ) nbin_z_e_ybin = numBins_driftTime_e;
        double depth_f_e = distanceMap_e[Layer]->GetBinContent( nbin_z_e_xbin,nbin_z_e_ybin );
        double dz_e = fabs(dist_electrode - depth_f_e); 
        
	//TODO: the holes map does not currently extend for a drift time long enough that, any hole will reach
	//the corresponding electrode. This needs to be rectified by either (a) extrapolating the current map or
	//(b) making a new map with a y-axis (drift time) that extends to at least 18 ns so all charge carriers reach electrode.
	//However, if choose (b), will need to reduce granularity of map. 
        int nbin_z_h_xbin = distanceMap_h[Layer]->GetXaxis()->FindBin(dist_electrode);
        int nbin_z_h_ybin = distanceMap_h[Layer]->GetYaxis()->FindBin(drifttime_h);
        if (nbin_z_h_ybin >  numBins_driftTime_h  )
        nbin_z_h_ybin = numBins_driftTime_h;
        double depth_f_h = distanceMap_h[Layer]->GetBinContent( nbin_z_h_xbin,nbin_z_h_ybin );
        double dz_h = fabs(depth_f_h - dist_electrode);           

        //Apply drift due to Lorentz force and diffusion
        double phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

        int  nbin_Lorentz_e = lorentzMap_e[Layer]->FindBin(dist_electrode,depth_f_e);
        tanLorentz = lorentzMap_e[Layer]->GetBinContent(nbin_Lorentz_e);       
        coLorentz=sqrt(1+pow(tanLorentz,2));

        //Apply diffusion. rdif is teh max. diffusion
        double rdif_e=this->m_diffusionConstant*sqrt( fabs(dist_electrode - depth_f_e)*coLorentz/0.3);
        double phi_f_e=phi_i + dz_e*tanLorentz + rdif_e*phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
        double eta_f_e=eta_i + rdif_e*etaRand;
  
        phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

        int  nbin_Lorentz_h = lorentzMap_h[Layer]->FindBin(dist_electrode,depth_f_h);
        tanLorentz = lorentzMap_h[Layer]->GetBinContent(nbin_Lorentz_h);       
        coLorentz=sqrt(1+pow(tanLorentz,2));

        double rdif_h=this->m_diffusionConstant*sqrt( fabs(dist_electrode - depth_f_h)*coLorentz/0.3);

        double phi_f_h=phi_i + dz_h*tanLorentz + rdif_h*phiRand;
        etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
        double eta_f_h=eta_i + rdif_h*etaRand;

        // Slim Edge for IBL planar sensors:
        if ( p_design.getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
            CHECK( applySlimEdges( energy_per_step, eta_f_e ) );
            CHECK( applySlimEdges( energy_per_step, eta_f_h ) );
        }
   
        //Loop over nearest neighbours in x and y
        //We assume that the lateral diffusion is minimal
        for (int p=nnLoop_pixelEtaMin; p<=nnLoop_pixelEtaMax; p++){ 
            for (int q=nnLoop_pixelPhiMin; q<=nnLoop_pixelPhiMax; q++){

              //Since both e-h charge carriers start in the same place, they have the same initial ramo value       
              //Centre of nearest neighbour (nn) pixel
              SiLocalPosition centreOfPixel_nn = p_design.positionFromColumnRow( pixel_i.etaIndex() - p, pixel_i.phiIndex() - q );

              //What is the displacement of the nn pixel from the primary pixel. 
              //This is to index the correct entry in the Ramo weighting potential map
              double dPhi_nn_centre = centreOfPixel_nn.xPhi() - centreOfPixel_i.xPhi(); //in mm
              double dEta_nn_centre = centreOfPixel_nn.xEta() - centreOfPixel_i.xEta(); //in mm

              //Find weighting potential in initial position. This all has to be done relative to the (0,0) position since the 
              //Ramo weighting potential is only mapped out for 1/8th of a pixel. Much of this logic is reflecting the charge
              //carrier across the boundaries.
              double dEta_i_e = pixelEta_i - dEta_nn_centre;
              double dPhi_i_e = pixelPhi_i - dPhi_nn_centre;

              int nbin_ramo_i_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( fabs( dPhi_i_e )*1000. );
              int nbin_ramo_i_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( fabs( dEta_i_e )*1000. );
              int nbin_ramo_i_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( dist_electrode*1000 );
              //int nbin_ramo_i = ramoPotentialMap[0]->FindBin( fabs( dEta_i_e )*1000. , fabs( dPhi_i_e )*1000., dist_electrode*1000);
             
	      //Boundary check on maps
              double ramo_i=0.;
              if( nbin_ramo_i_x <= numBins_weightingPotential_x && nbin_ramo_i_y <= numBins_weightingPotential_y && nbin_ramo_i_z <=numBins_weightingPotential_z ){
               ramo_i =ramoPotentialMap[Layer]->GetBinContent( nbin_ramo_i_x,nbin_ramo_i_y,nbin_ramo_i_z );
              }     	
              //Find the displacment of the charge carriers from the centre of the pixel in +ve quadrant
              double pixelEta_f_e = eta_f_e - centreOfPixel_i.xEta() ;
              double pixelPhi_f_e = phi_f_e - centreOfPixel_i.xPhi() ;
        
              double pixelEta_f_h = eta_f_h - centreOfPixel_i.xEta() ;
              double pixelPhi_f_h = phi_f_h - centreOfPixel_i.xPhi() ;
	      
	      //Final position of charge carriers wrt nn centre
	      double dEta_f_e = pixelEta_f_e  - dEta_nn_centre ;
              double dPhi_f_e = pixelPhi_f_e  - dPhi_nn_centre ;

              int nbin_ramo_f_e_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( fabs( dPhi_f_e )*1000. );
              int nbin_ramo_f_e_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( fabs( dEta_f_e )*1000. );
              int nbin_ramo_f_e_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( depth_f_e*1000 );
              //int nbin_ramo_f_e = ramoPotentialMap[0]->FindBin( fabs( dEta_f_e )*1000. , fabs( dPhi_f_e )*1000., depth_f_e*1000);
              double ramo_f_e=0.;
              if( nbin_ramo_f_e_x <= numBins_weightingPotential_x && nbin_ramo_f_e_y <= numBins_weightingPotential_y && nbin_ramo_f_e_z <=numBins_weightingPotential_z ){
               ramo_f_e =ramoPotentialMap[Layer]->GetBinContent( nbin_ramo_f_e_x,nbin_ramo_f_e_y,nbin_ramo_f_e_z );
              }   

              double dEta_f_h = pixelEta_f_h - dEta_nn_centre ;
              double dPhi_f_h = pixelPhi_f_h - dPhi_nn_centre ;
              
              int nbin_ramo_f_h_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( fabs( dPhi_f_h )*1000. );
              int nbin_ramo_f_h_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( fabs( dEta_f_h )*1000. );
              int nbin_ramo_f_h_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( depth_f_h*1000 );
	      //int nbin_ramo_f_h = ramoPotentialMap->FindBin( fabs( dEta_f_h )*1000. , fabs( dPhi_f_h )*1000., depth_f_h*1000);
              //Boundary check on maps
              double ramo_f_h=0.;
              if( nbin_ramo_f_h_x <= numBins_weightingPotential_x && nbin_ramo_f_h_y <= numBins_weightingPotential_y && nbin_ramo_f_h_z <=numBins_weightingPotential_z ){
               ramo_f_h =ramoPotentialMap[Layer]->GetBinContent( nbin_ramo_f_h_x,nbin_ramo_f_h_y,nbin_ramo_f_h_z );
              } 

              //Account for the imperfect binning that would cause charge to be double-counted
              if(ramoPotentialMap[Layer]->GetZaxis()->FindBin(depth_f_h*1000) == ramoPotentialMap[Layer]->GetNbinsZ()+1) ramo_f_h=0;//this means the hole has reached the back end  
              if(ramoPotentialMap[Layer]->GetZaxis()->FindBin(depth_f_e*1000) ==  1){
                if( fabs(dEta_f_e)>=Module.etaPitch()/2.  || fabs(dPhi_f_e)>=Module.phiPitch()/2. ) ramo_f_e=0;
                else if (fabs(dEta_f_e)<Module.etaPitch()/2.  && fabs(dPhi_f_e)<Module.phiPitch()/2.  ) ramo_f_e=1.;
              }


              //Given final position of charge carrier, find induced charge. The difference in Ramo weighting potential gives the fraction of charge induced.
              //The energy_per_step is transformed into charge with the eleholePair per Energy
              double induced_charge_e = (ramo_f_e - ramo_i) * energy_per_step * eleholePairEnergy;
              double induced_charge_h = -(ramo_f_h - ramo_i) * energy_per_step * eleholePairEnergy;
          
	                  
              //Collect charge in centre of each pixel, since location within pixel doesn't matter for record
              SiLocalPosition chargePos = Module.hitLocalToLocal( centreOfPixel_nn.xEta(), centreOfPixel_nn.xPhi() );

              //The following lines are adapted from SiDigitization's Inserter class
              SiSurfaceCharge scharge_e( chargePos,SiCharge( induced_charge_e,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));
              SiSurfaceCharge scharge_h( chargePos,SiCharge( induced_charge_h,hitTime(phit),SiCharge::track,HepMcParticleLink(phit->trackNumber(),phit.eventId())));
              SiCellId diode = Module.cellIdOfPosition(scharge_e.position());
              SiCharge charge_e = scharge_e.charge();
              SiCharge charge_h = scharge_h.charge();
              if (diode.isValid()) {
                chargedDiodes.add(diode,charge_e);
                chargedDiodes.add(diode,charge_h);
              } //IF
            } //For q
        } //for p
      }else { //If no radDamage, run original
        
	      double rdif=this->m_diffusionConstant*sqrt(dist_electrode*coLorentz/0.3);
	      // position at the surface
	      double phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
	      double phi_drifted=phi_i+dist_electrode*tanLorentz+rdif*phiRand;
	      double etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
	      double eta_drifted=eta_i+rdif*etaRand;


	      // Slim Edge for IBL planar sensors:
	      if ( !(Module.isDBM()) && p_design.getReadoutTechnology()==InDetDD::PixelModuleDesign::FEI4) {
		      CHECK( applySlimEdges( energy_per_step, eta_drifted ) );
	      }
	      SiLocalPosition chargePos = Module.hitLocalToLocal(eta_drifted, phi_drifted);

	      // The parametrization of the sensor efficiency
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
        
      } //else: no radDamage, run original    
    }//end cycle for charge
  }//trfHitRecord.size()
  return StatusCode::SUCCESS;
 }


StatusCode SensorSimPlanarTool::applySlimEdges( double &energy_per_step, double &eta_drifted){

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
