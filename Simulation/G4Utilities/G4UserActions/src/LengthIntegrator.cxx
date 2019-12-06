/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/LengthIntegrator.h"
#include "SimHelpers/ServiceAccessor.h"

#include "TProfile.h"
#include "TProfile2D.h"
#include "TTree.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"


#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4Event.hh"
#include "G4Material.hh"
#include "G4StepPoint.hh"
#include "G4TouchableHistory.hh"
#include "G4LogicalVolume.hh"
#include "G4Step.hh"
#include "G4Pow.hh"
#include "G4Types.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"


// System includes
#include <mutex>

// Framework includes
#include "GaudiKernel/GaudiException.h"

// Anonymous namespace for file-global mutexes and helper functions
namespace
{
  /// Mutex used to protect access to the thread-unsafe THistSvc
  std::mutex gHistSvcMutex;

  /// Helper function to attempt a retrieve a hist from the hist svc
  template<class HistType>
  bool getHist(const ServiceHandle<ITHistSvc>& hSvc,
               const std::string& histPath, HistType*& ptr)
  {
    TH1* h = nullptr;
    if(hSvc->exists(histPath)) {
      if(hSvc->getHist(histPath, h).isSuccess()) {
        if((ptr = dynamic_cast<HistType*>(h))) return true;
      }
      else throw GaudiException("Failed to getHist", "GetHistErr", StatusCode::FAILURE);
    }
    return false;
  }

  /// Helper function to register a hist to the hist svc
  template<class HistType>
  void regHist(const ServiceHandle<ITHistSvc>& hSvc,
               const std::string& histPath, HistType*& ptr)
  {
    if(hSvc->regHist(histPath, ptr).isFailure()) {
      delete ptr; // Cleanup here, for convenience.
      throw GaudiException("Failed to regHist", "RegHistErr", StatusCode::FAILURE);
    }
  }

  template<class HistType>
  void regTree(const ServiceHandle<ITHistSvc>& hSvc,
                const std::string& treeName, HistType*& ptr)
  {
    if(hSvc->regTree(treeName,ptr).isFailure()){
      delete ptr; 
      throw GaudiException("Failed to register Tree", "RegTreeErr", StatusCode::FAILURE);
    }
  }


  template<class HistType>
  bool getTree(const ServiceHandle<ITHistSvc>& hSvc,
               const std::string& treeName, HistType*& ptr)
  {
    TTree* t = nullptr;
    if(hSvc->exists(treeName) && hSvc->getTree(treeName, t).isSuccess() &&
       (ptr = dynamic_cast<HistType*>(t)))
    {
      return true;
    }
    else throw GaudiException("Failed to getHist", "GetHistErr", StatusCode::FAILURE);

    return false;
    
  }

}

namespace G4UA
{
  //---------------------------------------------------------------------------
  // Constructor
  //---------------------------------------------------------------------------
  LengthIntegrator::LengthIntegrator(const std::string& histSvcName, const Config& config)
    : m_g4pow(0),
      m_hSvc(histSvcName, "LengthIntegrator"),
      m_tree(nullptr),
      m_config(config)
  {
    
    // Protect concurrent access to the non-thread-safe hist svc
    std::lock_guard<std::mutex> lock(gHistSvcMutex);

    //Try to add a nice little tree
    m_tree = new TTree( "TheLarch", "And now, the Larch" );
    //Add Braches to the tree
    //Particle properties
    m_tree->Branch("genNPart",                     &m_genNPart,      "genNPart/I");
    m_tree->Branch("genEta",                       &m_genEta,        "genEta/F");
    m_tree->Branch("genPhi",                       &m_genPhi,        "genPhi/F");
    m_tree->Branch("genZ",                         &m_genZ,          "genZ/F");
    m_tree->Branch("genR",                         &m_genR,          "genR/F");
    m_tree->Branch("total_X0",                     &m_total_X0,      "total_X0/F");
    m_tree->Branch("total_L0",                     &m_total_L0,      "total_L0/F");

    m_tree->Branch("collected_X0",                 &m_collected_X0); //Vector
    m_tree->Branch("collected_L0",                 &m_collected_L0); //Vector
   
    m_tree->Branch("collected_inhitr",             &m_collected_inhitr); //Vector
    m_tree->Branch("collected_inhitz",             &m_collected_inhitz); //Vector

    m_tree->Branch("collected_outhitr",            &m_collected_outhitr); //Vector
    m_tree->Branch("collected_outhitz",            &m_collected_outhitz); //Vector
      
    m_tree->Branch("collected_material",          &m_collected_material); //Vector
    m_tree->Branch("collected_density",           &m_collected_density); //Vector
    m_tree->Branch("collected_volume",            &m_collected_volume); //Vector
    m_tree->Branch("collected_groupedmaterial",   &m_collected_groupedmaterial); //Vector
    m_tree->Branch("collected_volumetype",        &m_collected_volumetype); //Vector
   
    if(m_config.doElements){
      m_tree->Branch("collected_material_elements",   &m_collected_material_elements); //Vector 
      m_tree->Branch("collected_material_element_X0", &m_collected_material_element_X0); //Vector
      m_tree->Branch("collected_material_element_L0", &m_collected_material_element_L0); //Vector
    }

    regTree(m_hSvc, "/lengths/TheLarch", m_tree );

    m_g4pow = G4Pow::GetInstance();

  }

  //---------------------------------------------------------------------------
  // Cache primary info at beginning of event
  //---------------------------------------------------------------------------
  void LengthIntegrator::BeginOfEventAction(const G4Event* event)
  {
    
    G4PrimaryVertex* vert = event->GetPrimaryVertex(0);
    G4PrimaryParticle* part = vert->GetPrimary();
    G4ThreeVector mom = part->GetMomentum();
  
    // this only works for single-partcle events
    m_genEta = std::isfinite(mom.eta()) ? mom.eta() : -999;
    m_genPhi = mom.phi();
    m_genZ = vert->GetZ0();
    m_genR = sqrt((vert->GetX0()*vert->GetX0())+(vert->GetY0()*vert->GetY0()));
    m_genNPart++;

    //total X0/L0 per event
    m_total_X0 = 0;
    m_total_L0 = 0;

  }


  //---------------------------------------------------------------------------
  // Setup hists for one detector
  //---------------------------------------------------------------------------
  std::string LengthIntegrator::getMaterialClassification(std::string name, std::string volName)
  {

/*
    if(pixelonly){
      if(name.find("SCT") != std::string::npos ) continue;
      if(name.find("PP1_Mat") != std::string::npos ) continue;
      if(name.find("BeamPipe") != std::string::npos ) continue;
    }
*/    

    if((name.find("Atlas_Air") != std::string::npos) || (name.find("Atlas") != std::string::npos) ||
       (name.find("pix::HEX") != std::string::npos)){
      return "NONE";
    }

    if((name.find("Silicon") != std::string::npos) || (name.find("SiMetal") != std::string::npos)){
      return "ActiveSensors";
    }

    //For ATLAS run-2 detector
    if(!m_config.isITk){
      if(name.find("Cooling") != std::string::npos) return "Cooling";
      if(name.find("Cool") != std::string::npos) return "Cooling";
      if(name.find("CO2") != std::string::npos) return "Cooling";
      if(name.find("Cryo") != std::string::npos) return "Cooling";
      if(name.find("Pipe") != std::string::npos) return "Cooling";
      if(name.find("Flange") != std::string::npos) return "SupportStructure";
      if(name.find("Fixation") != std::string::npos) return "SupportStructure";
      if(name.find("Clamp") != std::string::npos) return "SupportStructure";
      if(name.find("Cable") != std::string::npos) return "Services";
      if(name.find("Connector") != std::string::npos) return "Services";
      if(name.find("Srv") != std::string::npos) return "Services";
      if(name.find("Flex") != std::string::npos) return "Services";
      if(name.find("Stave") != std::string::npos) return "SupportStructure";
      if(name.find("Wire") != std::string::npos) return "Services";
      if(name.find("Tape") != std::string::npos) return "Services";
      if(name.find("PPF0") != std::string::npos) return "PP0";
      if(name.find("PPB1") != std::string::npos) return "PP1";
      if(name.find("IBL") != std::string::npos) return "OtherIBL";
      if(name.find("BL") != std::string::npos) return "OtherIBL";
      if(name.find("PST") != std::string::npos) return "PST";
      if(name.find("IST") != std::string::npos) return "IST";
      if(name.find("Disk") != std::string::npos) return "SupportStructure";
      //if(name.find("L0") != std::string::npos) return "SupportStructure";
      //if(name.find("L1") != std::string::npos) return "SupportStructure";
      //if(name.find("L2") != std::string::npos) return "SupportStructure";
      if(name.find("Panel") != std::string::npos) return "Services";
      if(name.find("Radiator") != std::string::npos) return "Radiators";
      //if(name.find("Spine") != std::string::npos) return "ThermalSpine";
      if(name.find("Spine") != std::string::npos) return "Cooling";
      if(name.find("Fibre") != std::string::npos) return "Services";
      if(name.find("Chip") != std::string::npos) return "PixelChips";
      if(name.find("Shell") != std::string::npos) return "SupportStructure";
      if(name.find("Heat") != std::string::npos) return "Cooling";
      if(name.find("Dogleg") != std::string::npos) return "Cooling";
      if(name.find("PigTail") != std::string::npos) return "Cooling";
      if(name.find("Pigtail") != std::string::npos) return "Cooling";
      if(name.find("Aerogel") != std::string::npos) return "Aerogel";
      if(name.find("Faraday") != std::string::npos) return "FaradayCage";
      if(name.find("Prepreg") != std::string::npos) return "SupportStructure";
      if(name.find("Harness") != std::string::npos) return "SupportStructure";
      if(name.find("Spider") != std::string::npos) return "SupportStructure";
      if(name.find("Finger") != std::string::npos) return "SupportStructure";
      if(name.find("Serv") != std::string::npos) return "Services";
      if(name.find("Bulkhead") != std::string::npos) return "SupportStructure";
      if(name.find("EndPlate") != std::string::npos) return "SupportStructure";
      if(name.find("Tray") != std::string::npos) return "SupportStructure";
      if(name.find("Trolley") != std::string::npos) return "SupportStructure";
      if(name.find("Cap") != std::string::npos) return "SupportStructure";
      if(name.find("K13C_16") != std::string::npos) return "SupportStructure";
      if(name.find("AlNitride") != std::string::npos) return "Cooling";
      if(name.find("Copper") != std::string::npos) return "Services";
      if(name.find("G10") != std::string::npos) return "Services";
      if(name.find("DBMRod") != std::string::npos) return "Services";
      if(name.find("Twister") != std::string::npos) return "SupportStructure";
      if(name.find("Wolfram") != std::string::npos) return "SupportStructure";
      if(name.find("Aluminium") != std::string::npos) return "SupportStructure";
    }else{
      //ITk
      if(name.find("SCT_TiMetal_heat") != std::string::npos) return "HeatExchangers";
      if(name.find("pix::HEX") != std::string::npos) return "HeatExchangers";
      if(name.find("HeatExchangers") != std::string::npos) return "PP1"; // comment out this line to see the Heat Exchangers separately in the final plot 
      // New Step 3.0 items from Helen
      //----HYBRID----:
      if(name.find("matEC_Hybrid") != std::string::npos) return "StripHybrids"; //"Hybrid";
      else if(name.find("matB_HybridPCB") != std::string::npos) return "StripHybrids"; //"Hybrid";
      //----SERVICES----:
      else if(name.find("matSV_Endcap") != std::string::npos) return "Services";
      else if(name.find("matSV_Barrel") != std::string::npos) return "Services";
      //----ADHESIVE----
      else if(name.find("BoronNitrideEpoxy") != std::string::npos) return "SupportStructure"; //"Adhesive";
      else if(name.find("SE4445") != std::string::npos) return "SupportStructure"; //"Adhesive";
      //----SUPPORT STRUCTURE----
      else if(name.find("Peek") != std::string::npos) return "SupportStructure";
      else if(name.find("CFRP") != std::string::npos) return "SupportStructure";
      else if(name.find("CFoam") != std::string::npos) return "SupportStructure";
      else if(name.find("K13C2U") != std::string::npos) return "SupportStructure";
      else if(name.find("K13D2U") != std::string::npos) return "SupportStructure";
      else if(name.find("Rohacell110A") != std::string::npos) return "SupportStructure";
      //----SUPPORT HONEYCOMB----
      else if(name.find("Honeycomb10pcf") != std::string::npos) return "SupportStructure"; //"HoneyComb";
      else if(name.find("Honeycomb2pcf") != std::string::npos) return "SupportStructure"; //"HoneyComb";
      else if(name.find("Honeycomb3pcf") != std::string::npos) return "SupportStructure"; //"HoneyComb";
      //----Copper----
      else if(name.find("CuMetal") != std::string::npos) return "Services"; //"Copper";
      else if(name.find("Copper") != std::string::npos) return "Services"; //"Copper";
      //----Active Sensors---- (includes sensors, ABC and HCC chips)
      else if(name.find("SiMetal") != std::string::npos) return "ActiveSensors";
      //----Air----
      else if(name.find("Air") != std::string::npos) return "Air";
      else if(name.find("N2") != std::string::npos) return "Air";
      //----Cooling----
      else if(name.find("CO2Liquid") != std::string::npos) return "Cooling";
      else if(name.find("k9Allcomp") != std::string::npos) return "Cooling";
      else if(name.find("TiMetal") != std::string::npos) return "Cooling";
      //----BusTape----
      else if(name.find("Kapton") != std::string::npos) return "Services"; //"BusTape";
      else if(name.find("matPetalBusKapton") != std::string::npos) return "Services"; //"BusTape";
      //----closeouts and connectors----
      else if(name.find("T300CF") != std::string::npos) return "Cooling"; //"CloseoutsAndConnectors";
      else if(name.find("Torlon") != std::string::npos) return "Cooling"; //"CloseoutsAndConnectors";
      //----strip chips----
      else if(name.find("matDCDC") != std::string::npos) return "StripChips";
      else if(name.find("matEOS") != std::string::npos) return "StripChips";
      //else return "OtherSCT";

      // Older stuff
      if(name.find("CO2") != std::string::npos) return "Cooling";
      //if(name.find("BoronNitrideEpoxy") != std::string::npos) return "Cooling"; <---- ???
      //
      if(name.find("BoronNitrideEpoxy") != std::string::npos) return "SupportStructure";
      if(name.find("FwdSpineOutBase") != std::string::npos) return "SupportStructure";
      if(name.find("Rohacell") != std::string::npos) return "SupportStructure";
      if(name.find("Honeycomb") != std::string::npos) return "SupportStructure";

      if(name.find("matSV") != std::string::npos) return "Services";
      if(name.find("matEOS") != std::string::npos) return "Services";
      if(name.find("matDCDC") != std::string::npos) return "Services";
      if(name.find("PCB") != std::string::npos) return "Services";
      if(name.find("TiMetal") != std::string::npos) return "Services"; //What is the point of this one
      if(name.find("CuMetal") != std::string::npos) return "Services";

      if(name.find("N2") != std::string::npos) return "DryNitrogen";

      if(name.find("Aluminium") != std::string::npos) {
	if(volName.find("BeamPipe") != std::string::npos) return "BeamPipe";
	else if(volName.find("PP1") != std::string::npos) return "PP1";
	else { return "NONE"; }
      }
    } //End of ITk loop

    // Do you want to split cooling and services where possible?
    if(name.find("Cooling") != std::string::npos) return "Cooling";

    if(name.find("Inconel") != std::string::npos) return "BeamPipe";
    if(name.find("Aerogel") != std::string::npos) return "BeamPipe";
    if(name.find("Beryllium") != std::string::npos) return "BeamPipe";
    if(name.find("Getter") != std::string::npos) return "BeamPipe";
    if(name.find("Vacuum") != std::string::npos) return "BeamPipe";
    
    if(name.find("TiMetal") != std::string::npos) return "Titanium";
    if(name.find("CuMetal") != std::string::npos) return "Copper";
    if(name.find("Steel") != std::string::npos) return "Steel";

    if(name.find("BoratedPolyethylene") != std::string::npos) return "Moderator";
    if(name.find("Moderator") != std::string::npos) return "Moderator";
    
    if(name.find("Iron") != std::string::npos) return "SupportStructure";
    if(name.find("Peek") != std::string::npos) return "SupportStructure";
    if(name.find("CFRP") != std::string::npos) return "SupportStructure";
    if(name.find("CFoam") != std::string::npos) return "SupportStructure";
    if(name.find("K13D2U") != std::string::npos) return "SupportStructure";
    if(name.find("Alpine") != std::string::npos) return "SupportStructure"; 
    if(name.find("Support") != std::string::npos) return "SupportStructure";
    if(name.find("Carbon") != std::string::npos) return "SupportStructure";
    if(name.find("Default") != std::string::npos) return "SupportStructure";

    if(name.find("BarrelStrip") != std::string::npos) return "Services";
    if(name.find("Brl") != std::string::npos) return "Services";
    if(name.find("Svc") != std::string::npos) return "Services";
    if(name.find("InnerIST") != std::string::npos) return "Services";
    if(name.find("InnerPST") != std::string::npos) return "Services";
    if(name.find("BarrelPixel") != std::string::npos) return "Services";
    if(name.find("EndcapPixel") != std::string::npos) return "Services";
    if(name.find("InnerPixel") != std::string::npos) return "Services";
    if(name.find("OuterPixel") != std::string::npos) return "Services";
    if(name.find("pix::Chip") != std::string::npos) return "PixelChips";
    if(name.find("pix::Hybrid") != std::string::npos) return "PixelHybrids";

    if(name.find("PP0") != std::string::npos) return "PP0";
    if(name.find("PP1") != std::string::npos) return "PP1";

    //I don't know what these are.... TRT?
    if(!m_config.isITk){
      if(name.find("pix::") != std::string::npos) return "OtherPixel";
      //if(name.find("Titanium") != std::string::npos) return "OtherPixel";
      if(name.find("sct::") != std::string::npos) return "OtherSCT";
      if(name.find("Glass") != std::string::npos) return "OtherSCT";
      if(name.find("Other") != std::string::npos) return "SupportStructure";
      if(name.find("PP0") != std::string::npos) return "PP1";
      if(name.find("FaradayCage") != std::string::npos) return "SupportStructure";
      if(name.find("Radiators") != std::string::npos) return "SupportStructure";
      if(name.find("Titanium") != std::string::npos) return "Services";
     // if(name.find("ArCO2O2") != std::string::npos) return "ActiveSensors"; //Overwritten by some other statement...
     // if(name.find("XeCO2O2") != std::string::npos) return "ActiveSensors";  //Overwritten by some other statement...
    }else{ //FIXME Why though
      if(name.find("PP0") != std::string::npos) return "PP1"; //Grouping PP0 and PP1
    }
      
    if(name.find("PST") != std::string::npos) return "SupportStructure";
    if(name.find("IST") != std::string::npos) return "SupportStructure";
    if(name.find("Silicon") != std::string::npos) return "ActiveSensors";
    if(name.find("Straw") != std::string::npos) return "ActiveSensors";
    if(name.find("Diamond") != std::string::npos) return "ActiveSensors";
    if(name.find("SiMetal") != std::string::npos) return "ActiveSensors";
    if(name.find("Air") != std::string::npos) return "Air";
    
    //FIXME hack while we fix the air->nitrogen issue
    if(name.find("DryNitrogen") != std::string::npos) return "Air";

    if(name.find("CurlyPipeMountain") != std::string::npos) return "SupportStructure";
    if(name.find("Flange") != std::string::npos) return "SupportStructure";
    //if(name2.find("DM_BeamPipe") != std::string::npos) return "BeamPipe"; //Overwritten by some other statement...

    if(name.find("Pigtail") != std::string::npos) return "Services"; //Overwritten by some other statement...
  

    if(name.find("IPT") != std::string::npos) return "IPT";     
    if(name.find("PixType2") != std::string::npos) return "PP1"; // Strip PP1 type2 volumes
    if(name.find("AlMetal") != std::string::npos) return "PP1"; //SCT::pixPP1outer
    if(name.find("PixRed") != std::string::npos) return "PP1"; //SCT::pixSvc PP1 data readout?
    if(name.find("SS304") != std::string::npos) return "SupportStructure"; //SCT::RailSquare ?? Guess this is encasing for pp1?
    if(volName.find("BeamPipe") != std::string::npos) return "BeamPipe"; //Cases of Valmat/VespelRing/PyrogelXT/Pump_Ti



    //TODO Are these necessary?
    /*
    if(searchstring=="M_"){
      if(name.find("Kapton") != std::string::npos) return "BeamPipe";
    }else{
      if(name.find("Kapton") != std::string::npos) return "Services";
    }
    */
    //if(name.find("Iron") != std::string::npos) return "BeamPipe";

    if(std::find(m_material_not_found.begin(),m_material_not_found.end(),name) == m_material_not_found.end()){
      m_material_not_found.push_back(name);
      std::cout << "Material not found: " << name << "  vol:" << volName << std::endl;
    }
    
    return "NONE";

  }

  void LengthIntegrator::fillNtuple(){

      m_tree->Fill();

      //Clean vectors and such
      m_collected_X0.clear();
      m_collected_L0.clear();
      
      m_collected_inhitr.clear();
      m_collected_inhitz.clear();
      m_collected_outhitr.clear();
      m_collected_outhitz.clear();


      m_collected_material.clear();
      m_collected_density.clear();
      m_collected_volume.clear();
      
      m_collected_groupedmaterial.clear();
      m_collected_volumetype.clear();

      m_collected_material_element_X0.clear();
      m_collected_material_element_L0.clear();
      m_collected_material_elements.clear();
  }


  //---------------------------------------------------------------------------
  // Finalize event measurements
  //---------------------------------------------------------------------------
  void LengthIntegrator::EndOfEventAction(const G4Event*)
  {
    // Lazily protect this whole code from concurrent access
    std::lock_guard<std::mutex> lock(gHistSvcMutex);

    fillNtuple();
  }

  //---------------------------------------------------------------------------
  // Accumulate results from one step
  //---------------------------------------------------------------------------
  void LengthIntegrator::UserSteppingAction(const G4Step* aStep)
  {

    //Get information of the step
    G4TouchableHistory* touchHist = (G4TouchableHistory*) aStep->GetPreStepPoint()->GetTouchable();
    G4LogicalVolume* lv = touchHist->GetVolume()->GetLogicalVolume();
    G4ThreeVector hitPoint = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endPoint = aStep->GetPostStepPoint()->GetPosition();
    G4Material* mat = lv->GetMaterial();

    std::string volName = lv->GetName();
    std::string matName = mat->GetName();
    std::string groupmaterial = getMaterialClassification(matName, volName); //Groups material into "general" categories, supports/sensors/pixels/cooling/etc
    std::string volumetype = getVolumeType(matName);

    double radl = mat->GetRadlen();
    double intl = mat->GetNuclearInterLength();
    double stepl = aStep->GetStepLength();
    double density = mat->GetDensity()*CLHEP::cm3/CLHEP::gram;


    // FIXME: using DBL_MAX just ensures double overflow below.
    double thickstepRL = radl != 0 ? stepl/radl : DBL_MAX;
    double thickstepIL = intl != 0 ? stepl/intl : DBL_MAX;

    //if(fabs(hitPoint.z()) > 3450.) return;


    // Get Elements for the material only do this if m_config.doElements is set
    if(m_config.doElements){
	    G4double lambda0 = 35*g/cm2;	
  	  const G4ElementVector* eVec = mat->GetElementVector();
      std::vector<double> tmp_collected_material_element_X0;
      std::vector<double> tmp_collected_material_element_L0;
      std::vector<std::string> tmp_collected_material_element;
	    for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	      double el_thickstepRL = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai();
	      double el_thickstepIL = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );

        tmp_collected_material_element_X0.push_back(el_thickstepRL);
        tmp_collected_material_element_L0.push_back(el_thickstepIL);
        tmp_collected_material_element.push_back((*eVec)[i]->GetName());
      }

      m_collected_material_element_X0.push_back(tmp_collected_material_element_X0);
      m_collected_material_element_L0.push_back(tmp_collected_material_element_L0);
      m_collected_material_elements.push_back(tmp_collected_material_element);
    }


    //Fill information for the step
    m_collected_X0.push_back(thickstepRL);
    m_collected_L0.push_back(thickstepIL);

    m_collected_material.push_back(matName);
    m_collected_density.push_back(density);
    m_collected_volume.push_back(volName);

    m_total_X0+=thickstepRL;
    m_total_L0+=thickstepIL;

    m_collected_inhitr.push_back(hitPoint.perp());
    m_collected_inhitz.push_back(hitPoint.z());

    m_collected_outhitr.push_back(endPoint.perp());
    m_collected_outhitz.push_back(endPoint.z());

    m_collected_groupedmaterial.push_back(groupmaterial);
    m_collected_volumetype.push_back(volumetype);
  
  }


  std::string LengthIntegrator::getVolumeType(std::string s){

    std::string type = "";
    auto colsPos = s.find("::");
    if (colsPos != std::string::npos)
	      type = s.substr(0, colsPos);
    else
	  type=s;
    return type;
  }

} // namespace G4UA
