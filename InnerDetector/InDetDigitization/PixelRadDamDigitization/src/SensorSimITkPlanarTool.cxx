/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SensorSimITkPlanarTool.cxx
//   Implementation file for class SensorSimITkPlanarTool
///////////////////////////////////////////////////////////////////
// (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "SensorSimITkPlanarTool.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "PixelReadoutGeometry/PixelModuleDesign.h"
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
SensorSimITkPlanarTool::SensorSimITkPlanarTool(const std::string& type, const std::string& name,const IInterface* parent):
  SensorSimTool(type,name,parent),
  m_radDamageUtil(nullptr),
  m_numberOfSteps(50),
  m_numberOfCharges(10),
  m_diffusionConstant(.0),
  m_doRadDamage(true),
  m_trappingTimeElectrons(0.),
  m_trappingTimeHoles(0.),
  m_fluence(5), 
  m_fluence2(-1),       //e14 neq/cm^2  //if negative do not use interpolation maps
  m_fluence3(-1),       //if negative do not use interpolation maps but maps precomputed for benchmark indicated in fluence
  m_fluence4(-1), 
  m_voltage (-1),       //Volt
  m_voltage2(-1),       
  m_voltage3(-1), 
  m_voltage4(-1) 
{ 
  declareProperty("RadDamageUtil",      m_radDamageUtil, "Rad Damage utility");
  declareProperty("numberOfSteps",      m_numberOfSteps,"Geant4:number of steps for PixelPlanar");
  declareProperty("numberOfCharges",    m_numberOfCharges,"Geant4:number of charges for PixelPlanar");
  declareProperty("diffusionConstant",  m_diffusionConstant,"Geant4:Diffusion Constant for PixelPlanar");
  declareProperty("doRadDamage",        m_doRadDamage, "doRadDmaage bool: should be flag");
  declareProperty("trappingTimeElectrons", m_trappingTimeElectrons, "Characteristic time till electron is trapped [ns]");
  declareProperty("trappingTimeHoles",  m_trappingTimeHoles, "Characteristic time till hole is trapped [ns]");
  declareProperty("fluence",            m_fluence,  "this is the ITk fluence benchmark, 0-9.  0 is unirradiated (original digitizer code), 1 is unirradiated (using the unirradiated maps with the radiation damage implementation), 3 is projected maximum ITk fluence before replacement, 4-9 is a bias voltage scan with this maximum fluence. If an number is passed that does not correspond to one of these benchmark values, this number will instead be interpreted as a fluence value in 1e14 neqcm2 at layer 1 to be used with the Efield interpolator");
  declareProperty("fluence2",           m_fluence2, "fluence detector has recieved in 1e14 neqcm2 at the layer 2, to be used with the Efield interpolator");
  declareProperty("fluence3",           m_fluence3, "fluence detector has recieved in 1e14 neqcm2 at the layer 3, to be used with the Efield interpolator");
  declareProperty("fluence4",           m_fluence4, "fluence detector has recieved in 1e14 neqcm2 at the layer 4, to be used with the Efield interpolator");
  declareProperty("voltage",            m_voltage,  "this is the bias voltage applied to layer 1 to be used with the Efield interpolator- if not set use values from applied at benchmark points according to fluence");
  declareProperty("voltage2",           m_voltage2, "bias voltage applied to the layer 2, to be used with the Efield interpolator");
  declareProperty("voltage3",           m_voltage3, "bias voltage applied to the layer 3, to be used with the Efield interpolator");
  declareProperty("voltage4",           m_voltage4, "bias voltage applied to the layer 4, to be used with the Efield interpolator");
}

class DetCondCFloat;

SensorSimITkPlanarTool::~SensorSimITkPlanarTool() { }

//===============================================
//    I N I T I A L I Z E
//===============================================
StatusCode SensorSimITkPlanarTool::initialize() {
  CHECK(SensorSimTool::initialize()); 
  ATH_MSG_DEBUG ( "SensorSimITkPlanarTool::initialize()");
 
  CHECK(m_radDamageUtil.retrieve());
  ATH_MSG_DEBUG ( "RadDamageUtil tool retrieved successfully");

  //Calculate trapping times based on fluence (already includes check for fluence=0)
  if(m_doRadDamage){

    std::pair<double,double> trappingTimes = m_radDamageUtil->getTrappingTimes( m_fluence );
    m_trappingTimeElectrons = trappingTimes.first;
    m_trappingTimeHoles = trappingTimes.second;
  }
  //If any fluence or voltage initialized negative use benchmark maps and not interpolation
  bool doInterpolateEfield = (m_fluence > 0. && m_fluence2 > 0.  && m_fluence3 > 0. && m_fluence4 > 0. && m_voltage > 0. && m_voltage2 > 0.  && m_voltage3 > 0. && m_voltage4 > 0.);
  std::vector<std::string> mapsPath_list;
  std::vector<std::string> TCADpath_list;


// path for run2 digitizer: : /eos/atlas/atlascerngroupdisk/asg-calib/PixelDigitization
  // Use all TCAD E field files in this directory for creating E field via interpolation (pruned filed excluded)

  std::string layer100umFiles        = PathResolverFindCalibDirectory("/eos/atlas/atlascerngroupdisk/det-slhc/users/PixelRadDamage/itk_100um_50um_x_50um/");
  std::string layer150umFiles     = PathResolverFindCalibDirectory("/eos/atlas/atlascerngroupdisk/det-slhc/users/PixelRadDamage/itk_150um_50um_x_50um/");

  // For each layer one configuration
  TCADpath_list = {layer100umFiles, layer150umFiles, layer150umFiles, layer150umFiles};           //layer1 - 100um sensor depth, layer2/3/4 - 150um
//    Replpicate the sesensorFiles files to afs/GroupData https://twiki.cern.ch/twiki/bin/view/AtlasComputing/PathResolver
//    If you want only certain TCAD E field files to be used to create an interpolated E field, hand over a list like these instead of the directory
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/ibl_TCAD_EfieldProfiles.txt"   );
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");
//    TCADpath_list.push_back(PathResolverFindCalibFile("PixelDigitization/blayer_TCAD_EfieldProfiles.txt");

  if(m_fluence==0){

    return StatusCode::SUCCESS;

  }
  else if(m_fluence==1){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_80V_fl1e-24e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root");
    m_fluence_layers.push_back(1e-10);
    m_fluence_layers.push_back(1e-10);
    m_fluence_layers.push_back(1e-10);
    m_fluence_layers.push_back(1e-10);
  }
  else if(m_fluence==2){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_600V_fl10e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl10e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl10e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl10e14.root");
    m_fluence_layers.push_back(1e15);
    m_fluence_layers.push_back(1e15);
    m_fluence_layers.push_back(1e15);
    m_fluence_layers.push_back(1e15);


  }else if(m_fluence==3){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_600V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_600V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);

  }else if(m_fluence==4){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_500V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_500V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_500V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_500V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);

  }else if(m_fluence==5){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_400V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_400V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_400V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_400V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);


  }else if(m_fluence==6){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_300V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_300V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_300V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_300V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);

  }else if(m_fluence==7){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_200V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_200V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_200V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_200V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);

  }else if(m_fluence==8){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_150V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_150V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_150V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_150V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);

  }else if(m_fluence==9){

    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_140V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_140V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_140V_fl30e14.root");
    mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_140V_fl30e14.root");
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
    m_fluence_layers.push_back(3e15);
}

  if(mapsPath_list.size()==0 ){
      if(doInterpolateEfield){
          ATH_MSG_INFO("No benchmark value set for fluence. Use interpolation.");
          mapsPath_list.clear();
          m_fluence_layers.clear();  
          m_voltage_layers.clear();
          //Set up default maps for ramoMap,
          //but retrieve Efield from interpolation as well as Lorentz, time and distance map from E field
          mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_100_80V_fl1e-24e14.root"); //Layer 1
          mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root"); //Layer 2
          mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root"); //Layer 3
          mapsPath_list.push_back("/eos/atlas/atlascerngroupdisk/asg-calib/ITkPixelDigitization/maps_PIX_150_80V_fl1e-24e14.root"); //Layer 4
          m_fluence_layers.push_back(m_fluence*1e14);
          m_fluence_layers.push_back(m_fluence2*1e14);
          m_fluence_layers.push_back(m_fluence3*1e14);
          m_fluence_layers.push_back(m_fluence4*1e14);
          
          m_voltage_layers.push_back(m_voltage);
          m_voltage_layers.push_back(m_voltage2);
          m_voltage_layers.push_back(m_voltage3);
          m_voltage_layers.push_back(m_voltage4);

      }else{
          ATH_MSG_WARNING("m_fluence could not match becnhmark value but interpolation not indicated.");
          return StatusCode::FAILURE; 
      }
  }else{
      if(!doInterpolateEfield){
          ATH_MSG_INFO("m_fluence set to becnhmark value. Use becnhmark values for all layers.");
      }else{
          ATH_MSG_WARNING("m_fluence set to benchmark value. Fluence and bias voltage for all layers overwritten accordingly.");
      }
  }

  // *****************************
  // *** Setup Maps ****
  // *****************************
  //TODO This is only temporary until remotely stored maps and locally generated maps can be implemented 
  //E field already implemented: needs fluence and bias voltage given as Property m_fluence, m_fluence2, ...,  m_fluence3, ...      
  for(unsigned int i=0; i<mapsPath_list.size(); i++){

    ATH_MSG_INFO("Using maps located in: "<<mapsPath_list.at(i) << " for layer No." << i);
    if(doInterpolateEfield)ATH_MSG_INFO("Create E field via interpolation based on files from: " << TCADpath_list.at(i));
    //std::unique_ptr<TFile>  mapsFile=std::make_unique<TFile>( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.
    TFile mapsFile( (mapsPath_list.at(i)).c_str() ); //this is the ramo potential.

    std::pair<int, int> Layer;  // index for layer/end cap position
    Layer.first=0;  //Barrel (0) or End Cap (1)   -    Now only for Barrel. If we want to add End Caps, put them at Layer.first=1
    Layer.second=i; //Layer.second: 0 = planar layer 1, 1 = planar layer 2, 2 = planar layer 3, 3 = planar layer 4
    //IBL Barrel doesn't exist. So the possible idexes should be: 0-0, 0-1, 0-2, 0-3, 1-1, 1-2, 1-3

    //Setup ramo weighting field map
    TH3F* ramoPotentialMap_hold;
    ramoPotentialMap_hold=0;
    ramoPotentialMap_hold=(TH3F*)mapsFile.Get("my_3DRamoMap"); //in the future this name should be changed to be consistent with the non-ITk version ("hramomap1")
    if (ramoPotentialMap_hold==0) ramoPotentialMap_hold=(TH3F*)mapsFile.Get("ramo3d");
    ramoPotentialMap_hold->SetDirectory(0);
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
    if(doInterpolateEfield){
        //ATH_MSG_INFO("Generating E field maps using interpolation.");
        eFieldMap_hold= new TH1F() ;
        CHECK(m_radDamageUtil->generateEfieldMap( eFieldMap_hold, NULL, m_fluence_layers.at(i), m_voltage_layers.at(i), i, TCADpath_list.at(i), true));
    }else{
        //precomputed map
        eFieldMap_hold=(TH1F*)mapsFile.Get("hEfield1D"); 
        eFieldMap_hold->SetDirectory(0);
    }

    if (eFieldMap_hold == 0){ 
         ATH_MSG_INFO("Unable to load sensor e-field map.");
         return StatusCode::FAILURE;
    }
    //eFieldMap.push_back(eFieldMap_hold);
    eFieldMap[Layer]=eFieldMap_hold;

    TH2F* lorentzMap_e_hold;
    TH2F* lorentzMap_h_hold;
    TH2F* distanceMap_h_hold;
    TH2F* distanceMap_e_hold;
    TH1F* timeMap_e_hold;
    TH1F* timeMap_h_hold;

    if(doInterpolateEfield){
        lorentzMap_e_hold    =new TH2F() ;
        lorentzMap_h_hold    =new TH2F() ;
        distanceMap_h_hold   =new TH2F() ;
        distanceMap_e_hold   =new TH2F() ;
        timeMap_e_hold       =new TH1F() ;
        timeMap_h_hold       =new TH1F() ;

        CHECK(m_radDamageUtil->generateDistanceTimeMap( distanceMap_e_hold, distanceMap_h_hold, timeMap_e_hold, timeMap_h_hold, lorentzMap_e_hold, lorentzMap_h_hold, eFieldMap_hold, NULL ));
        // For debugging and documentation: uncomment to save different maps which are based on the interpolated E field
        if(m_radDamageUtil->m_saveDebugMaps){
            TString prename = "map_layer_";
            prename += i;
            prename += "distance_e.root";
            distanceMap_e_hold->SaveAs(prename);
            prename.ReplaceAll("_e", "_h");
            distanceMap_h_hold->SaveAs(prename);
            prename.ReplaceAll("distance","time");
            timeMap_h_hold->SaveAs(prename);
            prename.ReplaceAll( "_h","_e");
            timeMap_e_hold->SaveAs(prename);
            prename.ReplaceAll("time", "lorentz");
            lorentzMap_e_hold->SaveAs(prename);
            prename.ReplaceAll( "_e","_h");
            lorentzMap_h_hold->SaveAs(prename);
        }
    }else{
        //retrieve precomputed maps
        lorentzMap_e_hold=(TH2F*)mapsFile.Get("lorentz_map_e");
        lorentzMap_e_hold->SetDirectory(0);
        lorentzMap_h_hold=(TH2F*)mapsFile.Get("lorentz_map_h");
        lorentzMap_h_hold->SetDirectory(0);
        distanceMap_h_hold=(TH2F*)mapsFile.Get("hdistance");
        distanceMap_h_hold->SetDirectory(0);
        distanceMap_e_hold=(TH2F*)mapsFile.Get("edistance");
        distanceMap_e_hold->SetDirectory(0);
        timeMap_e_hold=(TH1F*)mapsFile.Get("etimes");
        timeMap_e_hold->SetDirectory(0);
        timeMap_h_hold=(TH1F*)mapsFile.Get("htimes");
        timeMap_h_hold->SetDirectory(0);
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
StatusCode SensorSimITkPlanarTool::finalize() {
  ATH_MSG_DEBUG("SensorSimITkPlanarTool::finalize()");
  return StatusCode::SUCCESS;
}
//===============================================
//    I N D U C E    C H A R G E
//===============================================
StatusCode SensorSimITkPlanarTool::induceCharge(const TimedHitPtr<SiHit> &phit, SiChargedDiodeCollection &chargedDiodes, const InDetDD::SiDetectorElement &Module, const InDetDD::PixelModuleDesign &p_design, std::vector< std::pair<double,double> > &trfHitRecord, std::vector<double> &initialConditions) {

  // So far, this is only discriminating variable from 3D sensor. (disabled for now for ITk)

/*
  if (p_design.numberOfCircuits()<2){				
    if(!Module.isDBM()) {  //DBM modules also processed here
      return StatusCode::SUCCESS; 
    }
  }
*/

  bool isBarrel = false;

  const PixelID* p_pixelId = static_cast<const PixelID *>(Module.getIdHelper());
  int layer=p_pixelId->layer_disk(Module.identify() );

//    I think is3D() doesn't work right now 
//    if (p_design.is3D() && layer != 0) { std::cout << "1 " << p_design.is3D() << " " << layer  << std::endl;}
//    if (!p_design.is3D() && layer == 0) { std::cout << "2 " << p_design.is3D() << " " << layer << std::endl;}
//    if (p_design.is3D()) { std::cout << "3 " << std::endl;}

  int bec=1;
  if(p_pixelId->is_barrel(Module.identify()) ) bec=0 ;
  
  if(Module.isBarrel()) isBarrel = true;

  std::pair<int, int> Layer;        // index for layer/end cap position
  Layer.first=bec;                  //Barrel (0) or End Cap (1)   -    Maps only for Barrel at the moment. isBarrel will avoid zsh
  Layer.second=layer - 1;               //Layer.second: 0 = planar layer 1, 1 = planar layer 2, 2 = planar layer 3, 3 = planar layer 4
                                    //IBL Barrel doesn't exist. So the possible idexes should be: 0-0, 0-1, 0-2, 0-3, 1-1, 1-2, 1-3

  //temporary hack for ITK
  if (Module.isInclined()) return StatusCode::SUCCESS;

  if (Layer.first != 0) return StatusCode::SUCCESS;		//barrel only for now

  if (Layer.second == -1) return StatusCode::SUCCESS; //this means it is a 3D sensor in layer 0 which is not handled here

  // for testing only certain layers
// if (Layer.second != 1) return StatusCode::SUCCESS; //100 x 50 x 50 planar
//  if (Layer.second != 2 && Layer.second != 3 && Layer.second != 4) return StatusCode::SUCCESS;	//150 x 50 x 50 planar

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
  double coLorentz=std::sqrt(1+std::pow(tanLorentz,2));

  double total_charge_event_debug = 0;
  
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

    if (std::abs(eta_i - pos_i.xEta()) > Module.etaPitch() || std::abs(phi_i - pos_i.xPhi()) > Module.phiPitch()) {
      ATH_MSG_DEBUG("This is eta_i " << eta_i << " phi_i " << phi_i << " " << depth_i);
      ATH_MSG_DEBUG("The module is inclined? : " << Module.isInclined());
      ATH_MSG_DEBUG("SiLocalPosition " << pos_i.xEta() << " " << pos_i.xPhi());
      ATH_MSG_DEBUG("Will ignore this hit");
      return StatusCode::SUCCESS;
    }

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

        ATH_MSG_DEBUG("centreofPixel_i.xEta() : " << centreOfPixel_i.xEta() << " centreOfPixel_i.xPhi() " << centreOfPixel_i.xPhi());
        ATH_MSG_DEBUG("pixelEta_i " << pixelEta_i << " pixelPhi_i " << pixelPhi_i);

        
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
      nontrappingProbability = std::exp(-dist_electrode/collectionDist);
    }

    // amount of energy to be converted into charges at current step
    double energy_per_step = 1.0*iHitRecord.second/1.E+6/ncharges;

    // ITK: Are these still correct?
    // scale factors accounting for different pixel sizes
    double scale_i=1., scale_f_e=1., scale_f_h=1.;
    //double columnWidth=p_design.widthFromColumnRange(pixel_i.etaIndex(),pixel_i.etaIndex());

    /*
     if (abs(columnWidth-0.6)<1e-9){scale_i = 4./6.; scale_f_e = 4./6.; scale_f_h = 4./6.;}
else if (abs(columnWidth-0.45)<1e-9){scale_i = 25./45.; scale_f_e = 25./45.; scale_f_h = 25./45.;}
else if (abs(columnWidth-0.05)<1e-9){scale_i = 1.; scale_f_e = 1.; scale_f_h = 1.;}
else if (abs(columnWidth-0.5)<1e-9){scale_i = 25./50.; scale_f_e = 25./50.; scale_f_h = 25./50.;}
    */
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
        double dz_e = std::abs(dist_electrode - depth_f_e); 
        
        //TODO: the holes map does not currently extend for a drift time long enough that, any hole will reach
        //the corresponding electrode. This needs to be rectified by either (a) extrapolating the current map or
        //(b) making a new map with a y-axis (drift time) that extends to at least 18 ns so all charge carriers reach electrode.
        //However, if choose (b), will need to reduce granularity of map. 
        int nbin_z_h_xbin = distanceMap_h[Layer]->GetXaxis()->FindBin(dist_electrode);
        int nbin_z_h_ybin = distanceMap_h[Layer]->GetYaxis()->FindBin(drifttime_h);
        if (nbin_z_h_ybin >  numBins_driftTime_h  )
        nbin_z_h_ybin = numBins_driftTime_h;
        double depth_f_h = distanceMap_h[Layer]->GetBinContent( nbin_z_h_xbin,nbin_z_h_ybin );
        double dz_h = std::abs(depth_f_h - dist_electrode);           

        //Apply drift due to Lorentz force and diffusion
        double phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

        int  nbin_Lorentz_e = lorentzMap_e[Layer]->FindBin(dist_electrode,depth_f_e);
        tanLorentz = lorentzMap_e[Layer]->GetBinContent(nbin_Lorentz_e);       
        coLorentz=std::sqrt(1+std::pow(tanLorentz,2));

        //Apply diffusion. rdif is teh max. diffusion
        double rdif_e=this->m_diffusionConstant*std::sqrt( std::abs(dist_electrode - depth_f_e)*coLorentz/0.3);
        double phi_f_e=phi_i + dz_e*tanLorentz + rdif_e*phiRand;
        double etaRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);
        double eta_f_e=eta_i + rdif_e*etaRand;
  
        phiRand = CLHEP::RandGaussZiggurat::shoot(m_rndmEngine);

        int  nbin_Lorentz_h = lorentzMap_h[Layer]->FindBin(dist_electrode,depth_f_h);
        tanLorentz = lorentzMap_h[Layer]->GetBinContent(nbin_Lorentz_h);       
        coLorentz=std::sqrt(1+std::pow(tanLorentz,2));

        double rdif_h=this->m_diffusionConstant*std::sqrt( std::abs(dist_electrode - depth_f_h)*coLorentz/0.3);

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

        double total_charge_debug = 0;
        for (int p=nnLoop_pixelEtaMin; p<=nnLoop_pixelEtaMax; p++){ 
          for (int q=nnLoop_pixelPhiMin; q<=nnLoop_pixelPhiMax; q++){
            //Since both e-h charge carriers start in the same place, they have the same initial ramo value       
            //Centre of nearest neighbour (nn) pixel
            SiLocalPosition centreOfPixel_nn = p_design.positionFromColumnRow( pixel_i.etaIndex() - p, pixel_i.phiIndex() - q );

            //What is the displacement of the nn pixel from the primary pixel. 
            //This is to index the correct entry in the Ramo weighting potential map
            double dPhi_nn_centre = centreOfPixel_nn.xPhi() - centreOfPixel_i.xPhi(); //in mm
            double dEta_nn_centre = centreOfPixel_nn.xEta() - centreOfPixel_i.xEta(); //in mm

            ATH_MSG_DEBUG("centreofPixel_nn.xEta() : " << centreOfPixel_nn.xEta() << " centreOfPixel_nn.xPhi() " << centreOfPixel_nn.xPhi());

            //Find weighting potential in initial position. This all has to be done relative to the (0,0) position since the 
            //Ramo weighting potential is only mapped out for 1/8th of a pixel. Much of this logic is reflecting the charge
            //carrier across the boundaries.
            double dEta_i_e = pixelEta_i - dEta_nn_centre;
            double dPhi_i_e = pixelPhi_i - dPhi_nn_centre;

            dEta_i_e*=scale_i;

            int nbin_ramo_i_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( std::abs( dPhi_i_e )*1000. );
            int nbin_ramo_i_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( std::abs( dEta_i_e )*1000. );
            int nbin_ramo_i_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( dist_electrode*1000 );

            ATH_MSG_DEBUG(std::abs(dPhi_i_e) * 1000 << " by " << std::abs(dEta_i_e) * 1000 << " by " << dist_electrode*1000 << " in bin " << nbin_ramo_i_x << " by " << nbin_ramo_i_y << " by " << nbin_ramo_i_z);
            ATH_MSG_DEBUG("Overflow: " << numBins_weightingPotential_x << " " << numBins_weightingPotential_y << " " << numBins_weightingPotential_z);
          
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

            dEta_f_e*=scale_f_e;

            int nbin_ramo_f_e_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( std::abs( dPhi_f_e )*1000. );
            int nbin_ramo_f_e_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( std::abs( dEta_f_e )*1000. );
            int nbin_ramo_f_e_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( depth_f_e*1000 );

            ATH_MSG_DEBUG(std::abs(dPhi_f_e) * 1000 << " by " << std::abs(dEta_f_e) * 1000 << " by " << depth_f_e*1000 << " in bin " << nbin_ramo_f_e_x << " by " << nbin_ramo_f_e_y << " by " << nbin_ramo_f_e_z);

            double ramo_f_e=0.;
            if( nbin_ramo_f_e_x <= numBins_weightingPotential_x && nbin_ramo_f_e_y <= numBins_weightingPotential_y && nbin_ramo_f_e_z <=numBins_weightingPotential_z ){
              ramo_f_e =ramoPotentialMap[Layer]->GetBinContent( nbin_ramo_f_e_x,nbin_ramo_f_e_y,nbin_ramo_f_e_z );
            }   

            double dEta_f_h = pixelEta_f_h - dEta_nn_centre ;
            double dPhi_f_h = pixelPhi_f_h - dPhi_nn_centre ;

            dEta_f_h*=scale_f_h;
            
            int nbin_ramo_f_h_x = ramoPotentialMap[Layer]->GetXaxis()->FindBin( std::abs( dPhi_f_h )*1000. );
            int nbin_ramo_f_h_y = ramoPotentialMap[Layer]->GetYaxis()->FindBin( std::abs( dEta_f_h )*1000. );
            int nbin_ramo_f_h_z = ramoPotentialMap[Layer]->GetZaxis()->FindBin( depth_f_h*1000 );

            ATH_MSG_DEBUG(std::abs(dPhi_f_h) * 1000 << " by " << std::abs(dEta_f_h) * 1000 << " by " << depth_f_h*1000 << " in bin " << nbin_ramo_f_h_x << " by " << nbin_ramo_f_h_y << " by " << nbin_ramo_f_h_z);

            //Boundary check on maps
            double ramo_f_h=0.;
            if( nbin_ramo_f_h_x <= numBins_weightingPotential_x && nbin_ramo_f_h_y <= numBins_weightingPotential_y && nbin_ramo_f_h_z <=numBins_weightingPotential_z ){
              ramo_f_h =ramoPotentialMap[Layer]->GetBinContent( nbin_ramo_f_h_x,nbin_ramo_f_h_y,nbin_ramo_f_h_z );
            } 

            //Account for the imperfect binning that would cause charge to be double-counted
            if(ramoPotentialMap[Layer]->GetZaxis()->FindBin(depth_f_h*1000) == ramoPotentialMap[Layer]->GetNbinsZ()+1) ramo_f_h=0;//this means the hole has reached the back end  

            if(ramoPotentialMap[Layer]->GetZaxis()->FindBin(depth_f_h*1000) == ramoPotentialMap[Layer]->GetNbinsZ()+1) ramo_f_h=0;//this means the hole has reached the back end

            if(ramoPotentialMap[Layer]->GetZaxis()->FindBin(depth_f_e*1000) ==  0){	//same thing but with electrons
              ATH_MSG_DEBUG(std::abs(dEta_f_e) << " x " << std::abs(dPhi_f_e) << " vs " << Module.etaPitch()/2 << " x " << Module.phiPitch()/2);
              if( std::abs(dEta_f_e)>=Module.etaPitch()/2.  || std::abs(dPhi_f_e)>=Module.phiPitch()/2. ) ramo_f_e=0;
              else if (std::abs(dEta_f_e)<Module.etaPitch()/2.  && std::abs(dPhi_f_e)<Module.phiPitch()/2.  ) ramo_f_e=1.;
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
              ATH_MSG_DEBUG("Ramo : "<< ramo_f_e << " - " << ramo_i << " , " << energy_per_step << " " << eleholePairEnergy);
              ATH_MSG_DEBUG("Ramo : "<< ramo_f_h << " - " << ramo_i << " , " << energy_per_step << " " << eleholePairEnergy);
              ATH_MSG_DEBUG("FINAL RESULT: " << centreOfPixel_nn.xEta() << " " << centreOfPixel_nn.xPhi()  << " "  << induced_charge_e << " " << induced_charge_h);
              total_charge_debug += induced_charge_e;
              total_charge_debug += induced_charge_h;
              total_charge_event_debug += total_charge_debug;
            } //IF
          } //For q
        } //for p

      }else { //If no radDamage, run original

	      double rdif=this->m_diffusionConstant*std::sqrt(dist_electrode*coLorentz/0.3);
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
          total_charge_event_debug += ed;
	      }

      } //else: no radDamage, run original    
    }//end cycle for charge
  }//trfHitRecord.size()

  return StatusCode::SUCCESS;
 }


StatusCode SensorSimITkPlanarTool::applySlimEdges( double &energy_per_step, double &eta_drifted){

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
