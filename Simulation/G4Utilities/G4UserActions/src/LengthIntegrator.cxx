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


#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "GeoModelUtilities/DecodeVersionKey.h"

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
      delete ptr; //pie?
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
  LengthIntegrator::LengthIntegrator(const std::string& histSvcName)
    : m_g4pow(0),
      m_doRL(0),m_doIL(0),m_doRZ(0),m_doXY(0),m_doEl(0),m_doDensity(0),m_doEta(0),m_doPhi(0),m_doZ(0),m_doR(0),
      m_step(0),m_splitModerator(0),m_splitPP1(0),m_splitLayers(0),
      m_rz_zmin(0),m_rz_zmax(0),m_rz_rmin(0),m_rz_rmax(0),
      m_hSvc(histSvcName, "LengthIntegrator"),
      m_etaPrimary(0), m_phiPrimary(0),
      m_zPrimary(0),m_rPrimary(0),m_nActiveSensorHits(0),m_PreviousActiveSensorHit(0),m_totalX0(0),m_totalN0(0),
      m_rzProfRL(nullptr), m_rzProfIL(nullptr),m_isITk(false),m_geoModelSvc("GeoModelSvc","LengthIntegrator")
  {
    
    m_doRL = true;        // make radiation length (X0) plots
    m_doIL = false;       // make nuclear interaction length (L0) plots
    m_doRZ = true;        // make 2D r-z plots
    m_doXY = false;       // make 2D x-y plots
    m_doEl = true;        // make plots, splitting by elements of the periodic table
    m_doDensity = false;   // make density plots
    //
    m_doEta = true;       // make 1D plots against starting Eta of particles
    m_doPhi = false;      // make 1D plots against starting Phi of particles
    m_doZ = false;        // make 1D plots against starting Z position of particles
    m_doR = false;        // make 1D plots against starting radial position of particles

    // are we simulating an ITk step 1.x, 2.x, or 3.x layout?
    m_step = 3.0;
    // various plot-splitting switches
    m_splitModerator = true;
    m_splitPP1 = true;
    m_splitLayers = true;

    // plot limits
    m_rz_zmin = -3475.;
    m_rz_zmax =  3475.;
    m_rz_rmin =  0.;
    m_rz_rmax =  1200.;

    if (m_geoModelSvc.retrieve().isFailure()) {
      G4cout<<"ERROR could not get geoModelSvc "<<G4endl;
      m_isITk = false;
    }
    else {
      if(m_geoModelSvc->geoConfig()==GeoModel::GEO_RUN4) { m_isITk = true; }
      G4cout<<"INFO is ITk!" <<G4endl;
    }
/*  }
    if(m_doRL) m_lengthTypes.push_back("RL");
    if(m_doIL) m_lenghtTypes.push_back("IL");

    if(m_doEta) m_histoTypes.push_back("Eta");
    if(m_doPhi) m_histoTypes.push_back("Phi");
    if(m_doZ)   m_histoTypes.push_back("Z");
    if(m_doR)   m_histoTypes.push_back("R");
*/
    // Protect concurrent access to the non-thread-safe hist svc
    std::lock_guard<std::mutex> lock(gHistSvcMutex);

    // Register the RZ profiles. The other profiles need to wait until the end
    // of the events as they are created only if used.
    const char* radName = "/lengths/radLen/RZRadLen";
    if(!getHist(m_hSvc, radName, m_rzProfRL)) {
      m_rzProfRL = new TProfile2D("RZRadLen","RZRadLen",1000,-25000.,25000.,2000,0.,15000.);
      regHist(m_hSvc, radName, m_rzProfRL);
    }

    const char* intName = "/lengths/intLen/RZIntLen";
    if(!getHist(m_hSvc, intName, m_rzProfIL)) {
      m_rzProfIL = new TProfile2D("RZIntLen","RZIntLen",1000,-25000.,25000.,2000,0.,15000.);
      regHist(m_hSvc, intName, m_rzProfIL);
    }

    //Try to add a nice little tree
    m_tree = new TTree( "TheLarch", "And now, the Larch" );
    //Add Braches to the tree
    //Particle properties
    m_tree->Branch("genNPart",                  &m_genNPart,      "genNPart/I");
    m_tree->Branch("genEta",                    &m_genEta,        "genEta/F");
    m_tree->Branch("genPhi",                    &m_genPhi,        "genPhi/F");
    m_tree->Branch("genZ",                      &m_genZ,          "genZ/F");
    m_tree->Branch("genR",                      &m_genR,          "genR/F");
    m_tree->Branch("total_X0",                  &m_total_X0,      "total_X0/F");
    m_tree->Branch("total_L0",                  &m_total_L0,      "total_L0/F");

//    m_tree->Branch("collected_sensitivehit",    &m_collected_sensitivehit); //Vector    

    m_tree->Branch("collected_X0",              &m_collected_X0); //Vector
    m_tree->Branch("collected_L0",              &m_collected_L0); //Vector
   
    m_tree->Branch("collected_hitr",            &m_collected_hitr); //Vector
    m_tree->Branch("collected_hitz",            &m_collected_hitz); //Vector

    m_tree->Branch("collected_material",        &m_collected_material); //Vector
    m_tree->Branch("collected_density",         &m_collected_density); //Vector
    m_tree->Branch("collected_volume",          &m_collected_volume); //Vector
    m_tree->Branch("collected_groupedmaterial", &m_collected_groupedmaterial); //Vector
    m_tree->Branch("collected_volumetype",      &m_collected_volumetype); //Vector

    regTree(m_hSvc, "/lengths/TheLarch", m_tree );
    //TTree *Tpie;
    //getTree(m_hSvc, "/lengths/TheLarch", Tpie);
    //std::cout << "THIS IS A GREEEEEN TREE" << std::endl;
    //Tpie->ls();
    m_g4pow = G4Pow::GetInstance();

  }

  //---------------------------------------------------------------------------
  // Cache primary info at beginning of event
  //---------------------------------------------------------------------------
  void LengthIntegrator::BeginOfEventAction(const G4Event* event)
  {
    m_detThickMap.clear();
    G4PrimaryVertex* vert = event->GetPrimaryVertex(0);
    G4PrimaryParticle* part = vert->GetPrimary();
    G4ThreeVector mom = part->GetMomentum();
    m_etaPrimary=-999;
    if(std::isfinite(mom.eta())) m_etaPrimary=mom.eta();
    m_phiPrimary=mom.phi();
    m_zPrimary=vert->GetZ0();
    m_rPrimary=sqrt((vert->GetX0()*vert->GetX0())+(vert->GetY0()*vert->GetY0()));
    
    // this only works for single-partcle events
    m_nActiveSensorHits = 0;
    m_totalX0 = 0;
    m_totalN0 = 0;
    
    m_PreviousActiveSensorHit = false;

    m_genEta = std::isfinite(mom.eta()) ? mom.eta() : -999;
    m_genPhi = mom.phi();
    m_genZ = vert->GetZ0();
    m_genR = sqrt((vert->GetX0()*vert->GetX0())+(vert->GetY0()*vert->GetY0()));
    m_genNPart++;

    m_total_X0 = 0;
    m_total_L0 = 0;

  }

/*
  void LengthIntegrator::registerAndFill(const std::string& detName, const std::pair<double, double>& thicks){

     for(auto lT : m_lengthTypes){
      for(auto hT : m_histoTypes){
          std::string detHistName = detName + "_" + hT + "_" + lT;
          //Try to register
          registerHist(detHistName);
          //Fill histo
          fillHist(detHistName);
      }  
    }   

  }
*/
/*
  void LengthIntegrator::registerHist(const std::string& histName){
 
    std::string path = "/lengths/";
    std::string xaxis = "";
    std::string yaxis = "";
    int nbins = 1000; float xmin = -6.0; float xmax = 6.0;

    if(histName.find("RL") != std::npos){
      path+="radLen/";
      yaxis = "X^0";
    }
    else if(histName.find("IL") != std::npos){
      path+="intLen/";
      yaxis = "#Lambda^0";
    }
    else {
      //TODO: Warning
      path+= "pieLen/";
      yaxis = "Pies";
    }


    if(existHist(m_hSvc,path+histName)) return;


    if(histName.find("Eta") != std::npos){
      xaxis = "#eta";
    }
    else if(histName.find("Phi") != std::npos){
      xaxis = "#phi [rad]";
      nbins = 500; xmin = -M_PI; xmax = M_PI;
    }
    else if(histName.find("R") != std::npos){
      xaxis = "R [mm]";
      nbins = 1150; xmin = 0; xmax = 1150;
    }
    else if(histName.find("Z") != std::npos){
      xaxis = "Z [mm]";
      nbins = 704; xmin = 0; xmax = 3520;
    }

    TProfile *prof = new TProfile(histName.c_str(), (histName+";"+xaxis+";"+yaxis).c_str(), nbins, xmin, xmax);
	  regHist(m_hSvc, path, prof);
    m_profMap[detVolume] = prof;

  }
*/
  //---------------------------------------------------------------------------
  // Setup hists for one detector
  //---------------------------------------------------------------------------
  std::string LengthIntegrator::getMaterialClassification(std::string name)
  {

/*
    if(pixelonly){
      if(name.find("SCT") != std::string::npos ) continue;
      if(name.find("PP1_Mat") != std::string::npos ) continue;
      if(name.find("BeamPipe") != std::string::npos ) continue;
    }
*/    
    //if(name.find("PST") == std::string::npos ) continue;

    if((name.find("DM_Atlas_Air") != std::string::npos) || (name.find("DM_Atlas") != std::string::npos) ||
       (name.find("pix::HEX") != std::string::npos)){
      return "NONE";
    }

    if((name.find("Silicon") != std::string::npos) || (name.find("SiMetal") != std::string::npos)){
      return "ActiveSensors";
    }

    //For ATLAS run-2 detector
    if(!m_isITk){
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
      //ITk I guess?
      if(name.find("SCT_TiMetal_heat") != std::string::npos) return "HeatExchangers";
      if(name.find("pix::HEX") != std::string::npos) return "HeatExchangers";
      if(name.find("HeatExchangers") != std::string::npos) return "PP1"; // comment out this line to see the Heat Exchangers separately in the final plot
      
      // New Step 3.0 items from Helen
      //----HYBRID----:
      if(name.find("matEC_Hybrid") != std::string::npos) return "StripChips"; //"Hybrid";
      else if(name.find("matB_HybridPCB") != std::string::npos) return "StripChips"; //"Hybrid";
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
      //if(name.find("BoronNitrideEpoxy") != std::string::npos) return "Cooling";
      if(name.find("BoronNitrideEpoxy") != std::string::npos) return "SupportStructure";
      if(name.find("FwdSpineOutBase") != std::string::npos) return "SupportStructure";
      if(name.find("Rohacell") != std::string::npos) return "SupportStructure";
      if(name.find("Honeycomb") != std::string::npos) return "SupportStructure";
      if(name.find("matSV") != std::string::npos) return "Services";
      if(name.find("matEOS") != std::string::npos) return "Services";
      if(name.find("matDCDC") != std::string::npos) return "Services";
      if(name.find("PCB") != std::string::npos) return "Services";

      if(name.find("N2") != std::string::npos) return "DryNitrogen";
      if(name.find("TiMetal") != std::string::npos) return "Services";
      if(name.find("CuMetal") != std::string::npos) return "Services";
      if(name.find("Aluminium") != std::string::npos) return "BeamPipe";
    } //End of ITk loop

    // Do you want to split cooling and services where possible?
    if(name.find("Cooling") != std::string::npos) return "Services";

    if(name.find("Inconel") != std::string::npos) return "BeamPipe";
    if(name.find("Aerogel") != std::string::npos) return "BeamPipe";
    if(name.find("Beryllium") != std::string::npos) return "BeamPipe";
    if(name.find("Getter") != std::string::npos) return "BeamPipe";
    if(name.find("Vacuum") != std::string::npos) return "BeamPipe";
    
    /*
    if(searchstring=="M_"){
      if(name.find("Kapton") != std::string::npos) return "BeamPipe";
    }else{
      if(name.find("Kapton") != std::string::npos) return "Services";
    }
    */
    //if(name.find("Iron") != std::string::npos) return "BeamPipe";
    
    if(name.find("Iron") != std::string::npos) return "SupportStructure";
    if(name.find("Peek") != std::string::npos) return "SupportStructure";
    if(name.find("CFRP") != std::string::npos) return "SupportStructure";
    if(name.find("CFoam") != std::string::npos) return "SupportStructure";
    if(name.find("K13D2U") != std::string::npos) return "SupportStructure";
    if(name.find("BoratedPolyethylene") != std::string::npos) return "Moderator";


    if(name.find("TiMetal") != std::string::npos) return "Titanium";
    if(name.find("CuMetal") != std::string::npos) return "Copper";
      
    if(name.find("Alpine") != std::string::npos) return "SupportStructure";
    
    if(name.find("Support") != std::string::npos) return "SupportStructure";
    if(name.find("Carbon") != std::string::npos) return "SupportStructure";
    if(name.find("Default") != std::string::npos) return "SupportStructure";
    if(name.find("Moderator") != std::string::npos) return "Moderator";
    if(name.find("Steel") != std::string::npos) return "Steel";
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
    if(name.find("pix::Hybrid") != std::string::npos) return "PixelChips";
    if(name.find("PP0") != std::string::npos) return "PP0";
    if(name.find("PP1") != std::string::npos) return "PP1";

    //I don't know what these are.... TRT?
    if(!m_isITk){
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
    }else{
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
   
    return "NONE";

  }
//521

  void LengthIntegrator::fillNtuple(){

//      for(auto v: m_collected_volume) std::cout << v << std::endl;

      //TTree *larch = nullptr;
      //getTree(m_hSvc, "/lengths/TheLarch/", larch);
      m_tree->Fill();

      
      //Clean vectors and such
      m_collected_X0.clear();
      m_collected_L0.clear();
      
      m_collected_hitr.clear();
      m_collected_hitz.clear();

      m_collected_material.clear();
      m_collected_density.clear();
      m_collected_volume.clear();
      
      m_collected_groupedmaterial.clear();
      m_collected_volumetype.clear();

  }

  void LengthIntegrator::regAndFillHist(const std::string& detName,
                                        const std::pair<double, double>& thicks)
  {

/*
    TProfile* profEtaRL = nullptr;
    TProfile* profEtaIL = nullptr;
    TProfile* profPhiRL = nullptr;
    TProfile* profPhiIL = nullptr;
    TProfile* profZRL = nullptr;
    TProfile* profZIL = nullptr;
    TProfile* profRRL = nullptr;
    TProfile* profRIL = nullptr;

    auto pathEtaRL = "/lengths/radLen/" + detName + "_Eta_RL";
    auto pathEtaIL = "/lengths/intLen/" + detName + "_Eta_IL";
    auto pathPhiRL = "/lengths/radLen/" + detName + "_Phi_RL";
    auto pathPhiIL = "/lengths/intLen/" + detName + "_Phi_IL";
    auto pathZRL = "/lengths/radLen/" + detName + "_Z_RL";
    auto pathZIL = "/lengths/intLen/" + detName + "_Z_IL";
    auto pathRRL = "/lengths/radLen/" + detName + "_R_RL";
    auto pathRIL = "/lengths/intLen/" + detName + "_R_IL";






    // Eta
    if(m_doEta){
      // Eta rad profile
      if(m_doRL) {
	if(!getHist(m_hSvc, pathEtaRL, profEtaRL)) {
	  const std::string name(detName+"_Eta_RL");
	  profEtaRL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
	  profEtaRL->GetXaxis()->SetTitle("#eta");
	  profEtaRL->GetYaxis()->SetTitle("X0");
	  regHist(m_hSvc, pathEtaRL, profEtaRL);
	}
      }
      // Eta int profile
      if(m_doIL){	
	if(!getHist(m_hSvc, pathEtaIL, profEtaIL)) {
	  const std::string name(detName+"_Eta_IL");
	  profEtaIL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
	  profEtaIL->GetXaxis()->SetTitle("#eta");
	  profEtaIL->GetYaxis()->SetTitle("#lambda");
	  regHist(m_hSvc, pathEtaIL, profEtaIL);
	}
      }
    }
    // Phi
    if(m_doPhi){
      // Phi rad profile
      if(m_doRL) {
	if(!getHist(m_hSvc, pathPhiRL, profPhiRL)) {
	  const std::string name(detName+"_Phi_RL");
	  profPhiRL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
	  profPhiRL->GetXaxis()->SetTitle("#phi");
	  profPhiRL->GetYaxis()->SetTitle("X0");
	  regHist(m_hSvc, pathPhiRL, profPhiRL);
	}
      }
      // Phi int profile
      if(m_doIL){
	if(!getHist(m_hSvc, pathPhiIL, profPhiIL)) {
	  const std::string name(detName+"_Phi_IL");
	  profPhiIL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
	  profPhiIL->GetXaxis()->SetTitle("#phi");
	  profPhiIL->GetYaxis()->SetTitle("#lambda");
	  regHist(m_hSvc, pathPhiIL, profPhiIL);
	}
      }
    }
    // Z
    if(m_doZ){
      // Z rad profile
      if(m_doRL){
	if(!getHist(m_hSvc, pathZRL, profZRL)) {
	  const std::string name(detName+"_Z_RL");
	  profZRL = new TProfile(name.c_str(), name.c_str(), 704, 0, 3520);
	  profZRL->GetXaxis()->SetTitle("z");
	  profZRL->GetYaxis()->SetTitle("X0");
	  regHist(m_hSvc, pathZRL, profZRL);
	}
      }
      // Z int profile
      if(m_doIL){
	if(!getHist(m_hSvc, pathZIL, profZIL)) {
	  const std::string name(detName+"_Z_IL");
	  profZIL = new TProfile(name.c_str(), name.c_str(), 704, 0, 3520);
	  profZIL->GetXaxis()->SetTitle("z");
	  profZIL->GetYaxis()->SetTitle("#lambda");
	  regHist(m_hSvc, pathZIL, profZIL);
	}
      }
    }
    // R
    if(m_doR){
      // R rad profile
      if(m_doRL){
	if(!getHist(m_hSvc, pathRRL, profRRL)) {
	  const std::string name(detName+"_R_RL");
	  profRRL = new TProfile(name.c_str(), name.c_str(), 1150, 0, 1150);
	  profRRL->GetXaxis()->SetTitle("r");
	  profRRL->GetYaxis()->SetTitle("X0");
	  regHist(m_hSvc, pathRRL, profRRL);
	}
      }
      // R int profile
      if(m_doIL){
	if(!getHist(m_hSvc, pathRIL, profRIL)) {
	  const std::string name(detName+"_R_IL");
	  profRIL = new TProfile(name.c_str(), name.c_str(), 1150, 0, 1150);
	  profRIL->GetXaxis()->SetTitle("r");
	  profRIL->GetYaxis()->SetTitle("#lambda");
	  regHist(m_hSvc, pathRIL, profRIL);
	}
      }
    }
    //
    if(m_doRL){
      if(m_doEta){
	m_etaMapRL[detName] = profEtaRL;
	profEtaRL->Fill(m_etaPrimary, thicks.first, 1.);
      }
      //
      if(m_doPhi){
	m_phiMapRL[detName] = profPhiRL;
	profPhiRL->Fill(m_phiPrimary, thicks.first, 1.);
      }
      //
      if(m_doZ){
	m_zMapRL[detName] = profZRL;
	profZRL->Fill(m_zPrimary, thicks.first, 1.);
      }
      //
      if(m_doR){
	m_rMapRL[detName] = profRRL;
	profRRL->Fill(m_rPrimary, thicks.first, 1.);
      }
    }
    if(m_doIL){
      if(m_doEta){
	m_etaMapIL[detName] = profEtaIL;
	profEtaIL->Fill(m_etaPrimary, thicks.second, 1.);
      }
      //
      if(m_doPhi){
	m_phiMapIL[detName] = profPhiIL;
	profPhiIL->Fill(m_phiPrimary, thicks.second, 1.);
      }
      //
      if(m_doZ){
	m_zMapIL[detName] = profZIL;
	profZIL->Fill(m_zPrimary, thicks.second, 1.);
      }
      //
      if(m_doR){
	m_rMapIL[detName] = profRIL;
	profRIL->Fill(m_rPrimary, thicks.second, 1.);
      }
    }
    //
    */
  }

  //---------------------------------------------------------------------------
  // Finalize event measurements
  //---------------------------------------------------------------------------
  void LengthIntegrator::EndOfEventAction(const G4Event*)
  {
    // Lazily protect this whole code from concurrent access
    std::lock_guard<std::mutex> lock(gHistSvcMutex);

    // Loop over volumes
    for (auto& it : m_detThickMap) {

      // If histos already exist, then fill them
      if (m_etaMapRL.find(it.first) != m_etaMapRL.end()) {
	
	      if(m_doRL){
	        if(m_doEta) m_etaMapRL[it.first]->Fill(m_etaPrimary, it.second.first, 1.);
	        if(m_doPhi) m_phiMapRL[it.first]->Fill(m_phiPrimary, it.second.first, 1.);
	        if(m_doZ)   m_zMapRL[it.first]->Fill(m_zPrimary,it.second.first,1.);
	        if(m_doR)   m_rMapRL[it.first]->Fill(m_rPrimary,it.second.first,1.);
	      }
	//
	      if(m_doIL){
	        if(m_doEta) m_etaMapIL[it.first]->Fill(m_etaPrimary, it.second.second, 1.);
	        if(m_doPhi) m_phiMapIL[it.first]->Fill(m_phiPrimary, it.second.second, 1.);
	        if(m_doZ)   m_zMapIL[it.first]->Fill(m_zPrimary,it.second.second,1.);
	        if(m_doR)   m_rMapIL[it.first]->Fill(m_rPrimary,it.second.second,1.);
	      }
      }
      // New detector volume; register it
      else {
        regAndFillHist(it.first, it.second);
      }

    } // Loop over detectors

    /////////////////////////////////////////////////
    // Putting this here, as normally I'd add the following code into a finalize function (as has been done above for the Athena(non-MP) code), but I'm not sure if overloading the finalize function in AthenaMP will break the histogram merging at the end of the job. If it wont, then the following code can be put in a finalize function, which will speed up AthenaMP jobs.

    // Adding zeros to TProfile bins, so that each bin contains the same number of entries,
    // so that a THStack of all the material TProfile plots (or all the element plots) will equal the Total_X0 TProfile plot
    // It's because each plot (for each material, say) is only filled if a Geantion hits it, not if it's not hit in an event 
    
    int nbins = 0;
    if(m_doRL){
      if(m_doEta){
	      if(m_etaMapRL.find("Total_X0")!=m_etaMapRL.end()){
	        TProfile* totalEtaRL = m_etaMapRL["Total_X0"];
	        nbins = totalEtaRL->GetNbinsX();
	        for (auto it : m_etaMapRL){
	          if( it.first != "Total_X0" ){
	            TProfile* plot = m_etaMapRL[it.first];
	            for(int i=0 ; i<=nbins ; i++){
		            double x_value = plot->GetBinCenter(i);
		            int n_enties_plot = plot->GetBinEntries(i);
		            int n_enties_ref = totalEtaRL->GetBinEntries(i);
		            int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		            for(int j=0 ; j<n_zeros_to_be_added ; j++){
		              plot->Fill(x_value,0.0);
		            }
	            }
	          }   
	        }
	      }
      }
      
      if(m_doPhi){
	      if(m_phiMapRL.find("Total_X0")!=m_phiMapRL.end()){
	        TProfile* totalPhiRL = m_phiMapRL["Total_X0"];
	        nbins = totalPhiRL->GetNbinsX();
	        for (auto it : m_phiMapRL){
	          if( it.first != "Total_X0" ){
	            TProfile* plot = m_phiMapRL[it.first];
	            for(int i=0 ; i<=nbins ; i++){
		            double x_value = plot->GetBinCenter(i);
		            int n_enties_plot = plot->GetBinEntries(i);
		            int n_enties_ref = totalPhiRL->GetBinEntries(i);
		            int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		            for(int j=0 ; j<n_zeros_to_be_added ; j++){
		              plot->Fill(x_value,0.0);
		            }
	            }
	          }
	        }
	      }
      }

      if(m_doZ){
	      if(m_zMapRL.find("Total_X0")!=m_zMapRL.end()){
	        TProfile* totalZRL = m_zMapRL["Total_X0"];
	        int nbins = totalZRL->GetNbinsX();
	        for (auto it : m_zMapRL){
	          if( it.first != "Total_X0" ){
	            TProfile* plot = m_zMapRL[it.first];
	            for(int i=0 ; i<=nbins ; i++){
		            double x_value = plot->GetBinCenter(i);
		            int n_enties_plot = plot->GetBinEntries(i);
		            int n_enties_ref = totalZRL->GetBinEntries(i);
		            int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
	    	        for(int j=0 ; j<n_zeros_to_be_added ; j++){
		              plot->Fill(x_value,0.0);
		            }
	            }
	          }
	        }
	      }
      }
      
      if(m_doR){
	      if(m_rMapRL.find("Total_X0")!=m_rMapRL.end()){
	        TProfile* totalRRL = m_rMapRL["Total_X0"];
	        nbins = totalRRL->GetNbinsX();
	        for (auto it : m_rMapRL){
	          if( it.first != "Total_X0" ){
	            TProfile* plot = m_rMapRL[it.first];
	            for(int i=0 ; i<=nbins ; i++){
		            double x_value = plot->GetBinCenter(i);
		            int n_enties_plot = plot->GetBinEntries(i);
		            int n_enties_ref = totalRRL->GetBinEntries(i);
		            int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		            for(int j=0 ; j<n_zeros_to_be_added ; j++){
		              plot->Fill(x_value,0.0);
		            }
	            }
	          }
	        }
	      }
      }
    }

    if(m_doIL){
      if(m_doEta){
	if(m_etaMapIL.find("Total_X0")!=m_etaMapIL.end()){
	  TProfile* totalEtaIL = m_etaMapIL["Total_X0"];
	  nbins = totalEtaIL->GetNbinsX();
	  for (auto it : m_etaMapIL){
	    if( it.first != "Total_X0" ){
	      TProfile* plot = m_etaMapIL[it.first];
	      for(int i=0 ; i<=nbins ; i++){
		double x_value = plot->GetBinCenter(i);
		int n_enties_plot = plot->GetBinEntries(i);
		int n_enties_ref = totalEtaIL->GetBinEntries(i);
		int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		for(int j=0 ; j<n_zeros_to_be_added ; j++){
		  plot->Fill(x_value,0.0);
		}
	      }
	    }
	  }
	}
      }

      if(m_doPhi){
	if(m_phiMapIL.find("Total_X0")!=m_phiMapIL.end()){
	  TProfile* totalPhiIL = m_phiMapIL["Total_X0"];
	  nbins = totalPhiIL->GetNbinsX();
	  for (auto it : m_phiMapIL){
	    if( it.first != "Total_X0" ){
	      TProfile* plot = m_phiMapIL[it.first];
	      for(int i=0 ; i<=nbins ; i++){
		double x_value = plot->GetBinCenter(i);
		int n_enties_plot = plot->GetBinEntries(i);
		int n_enties_ref = totalPhiIL->GetBinEntries(i);
		int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		for(int j=0 ; j<n_zeros_to_be_added ; j++){
		  plot->Fill(x_value,0.0);
		}
	      }
	    }
	  }
	}
      }
      
      if(m_doZ){
	if(m_zMapIL.find("Total_X0")!=m_zMapIL.end()){
	  TProfile* totalZIL = m_zMapIL["Total_X0"];
	  nbins = totalZIL->GetNbinsX();
	  for (auto it : m_zMapIL){
	    if( it.first != "Total_X0" ){
	      TProfile* plot = m_zMapIL[it.first];
	      for(int i=0 ; i<=nbins ; i++){
		double x_value = plot->GetBinCenter(i);
		int n_enties_plot = plot->GetBinEntries(i);
		int n_enties_ref = totalZIL->GetBinEntries(i);
		int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		for(int j=0 ; j<n_zeros_to_be_added ; j++){
		  plot->Fill(x_value,0.0);
		}
	      }
	    }
	  }
	}
      }

      if(m_doR){	
	if(m_rMapIL.find("Total_X0")!=m_rMapIL.end()){
	  TProfile* totalRIL = m_rMapIL["Total_X0"];
	  nbins = totalRIL->GetNbinsX();
	  for (auto it : m_rMapIL){
	    if( it.first != "Total_X0" ){
	      TProfile* plot = m_rMapIL[it.first];
	      for(int i=0 ; i<=nbins ; i++){
		      double x_value = plot->GetBinCenter(i);
		      int n_enties_plot = plot->GetBinEntries(i);
		      int n_enties_ref = totalRIL->GetBinEntries(i);
		      int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
		      for(int j=0 ; j<n_zeros_to_be_added ; j++){
		        plot->Fill(x_value,0.0);
		      }
	      }
	    }
	  }	
	}
      }
    }
    //m_tree->Fill();
    fillNtuple();
  }
//UserSteppingAction(const G4Step* aStep)

  //---------------------------------------------------------------------------
  // Accumulate results from one step
  //---------------------------------------------------------------------------
  void LengthIntegrator::UserSteppingAction(const G4Step* aStep)
  {

    G4TouchableHistory* touchHist = (G4TouchableHistory*) aStep->GetPreStepPoint()->GetTouchable();
    G4LogicalVolume* lv = touchHist->GetVolume()->GetLogicalVolume();
    G4ThreeVector hitPoint = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endPoint = aStep->GetPostStepPoint()->GetPosition();
    G4Material* mat = lv->GetMaterial();

    std::string volName = lv->GetName();
    std::string matName = mat->GetName();

    double radl = mat->GetRadlen();
    double intl = mat->GetNuclearInterLength();
    double stepl = aStep->GetStepLength();
    double density = mat->GetDensity()*CLHEP::cm3/CLHEP::gram;

    // FIXME: using DBL_MAX just ensures double overflow below.
    double thickstepRL = radl != 0 ? stepl/radl : DBL_MAX;
    double thickstepIL = intl != 0 ? stepl/intl : DBL_MAX;

    //std::cout << "x0: " << thickstepRL << std::endl;
    //Fill information for the step
    m_collected_X0.push_back(thickstepRL);
    m_collected_L0.push_back(thickstepIL);
    
    m_collected_material.push_back(matName);
    m_collected_density.push_back(density);
    m_collected_volume.push_back(volName);

    m_total_X0+=thickstepRL;
    m_total_L0+=thickstepIL;

    m_collected_hitr.push_back(hitPoint.perp());
    m_collected_hitz.push_back(hitPoint.z());

    m_collected_hitr.push_back(endPoint.perp());
    m_collected_hitz.push_back(endPoint.z());

    std::string groupmaterial = getMaterialClassification(matName);
    //std::cout << "NORA: " << groupmaterial << " " << matName << std::endl;

    std::string volumetype = getVolumeType(matName);
    //std::cout << "NORA: " << volumetype << "  " << matName << std::endl;

    m_collected_groupedmaterial.push_back(groupmaterial);
    m_collected_volumetype.push_back(volumetype);
  
  }

   void LengthIntegrator::UserSteppingActionOld(const G4Step* aStep)
  {
    G4TouchableHistory* touchHist = (G4TouchableHistory*) aStep->GetPreStepPoint()->GetTouchable();
    G4LogicalVolume* lv = touchHist->GetVolume()->GetLogicalVolume();
    std::string volName = lv->GetName();
    G4Material* mat = lv->GetMaterial();
    double radl = mat->GetRadlen();
    double intl = mat->GetNuclearInterLength();
    double stepl = aStep->GetStepLength();
    double density = mat->GetDensity()*CLHEP::cm3/CLHEP::gram;

    // FIXME: using DBL_MAX just ensures double overflow below.
    double thickstepRL = radl != 0 ? stepl/radl : DBL_MAX;
    double thickstepIL = intl != 0 ? stepl/intl : DBL_MAX;


    m_totalX0 += thickstepRL;
    m_totalN0 += thickstepIL;
   

  
    bool activeSensorHit = false;
    if( lv->GetMaterial()->GetName().find("Silicon") != std::string::npos || lv->GetMaterial()->GetName().find("SiMetal") != std::string::npos ||
        lv->GetMaterial()->GetName().find("ArCO2O2") != std::string::npos || lv->GetMaterial()->GetName().find("XeCO2O2") != std::string::npos) activeSensorHit = true;
    
    if( activeSensorHit ){
      if( activeSensorHit && m_PreviousActiveSensorHit ){
	      std::cout << "UH OH! Multiple hits in the same sensor? Material = " << lv->GetMaterial()->GetName() << " Volume = " << volName << std::endl;
	      activeSensorHit = false;
      }else{
	      m_nActiveSensorHits++;
      }
    }
    

    std::string detName = getVolumeType(lv->GetMaterial()->GetName());
    
    double zLimit = 3475.0; // For ITk studies: 3475mm is the z-limit of the ID End-Plate Cryostat (cf. talk by Jan Stark at Simu meeting on Dec. 14th,2016).
    //zLimit = 4000.0; // for ITk + HGTD studies 
    double zHit = aStep->GetPreStepPoint()->GetPosition().z();
    double rLimit = 1148.0; // For ITk studies: 1148mm is the R-limit of the ITk. Note, the solenoid coil is outside of this (at an R of ~1300mm, I think)
    double rHit = aStep->GetPreStepPoint()->GetPosition().perp();

    std::string layerNum = getLayerName(rHit, zHit);
   
    std::string matName = "M_" + layerNum + lv->GetMaterial()->GetName();
    std::string detName_plus_matName = "DM_" + detName + "_" + layerNum + lv->GetMaterial()->GetName();
    std::string detName_d = "D_" + detName;
    
    if(fabs(zHit) < zLimit && rHit < rLimit){
      
      LengthIntegrator::addToDetThickMap(detName_d,            thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThickMap(detName_plus_matName, thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThickMap("Total_X0",           thickstepRL, thickstepIL);
      
      if( activeSensorHit ){
	      std::string nHitsPlotName = "ActiveSensorHit_" + std::to_string(m_nActiveSensorHits); // C++11
	      LengthIntegrator::addToDetThickMap(nHitsPlotName, m_totalX0, m_totalN0);
      }

      if(m_doEl){
	      const G4ElementVector* eVec = mat->GetElementVector();
	      for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	        std::string elementName = "E_" + layerNum + (*eVec)[i]->GetName();
	        std::string matName_plus_elementName = "ME_" + layerNum + lv->GetMaterial()->GetName() + "_" + (*eVec)[i]->GetName();
	        std::string detName_plus_elementName = "DE_" + detName + "_" + (*eVec)[i]->GetName();
	        double el_thickstepRL = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai();
	        G4double lambda0 = 35*g/cm2;	
	        double el_thickstepIL = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );
	        LengthIntegrator::addToDetThickMap(matName_plus_elementName, el_thickstepRL, el_thickstepIL);
	      }
      }

      G4ThreeVector hitPoint = aStep->GetPreStepPoint()->GetPosition();
      G4ThreeVector endPoint = aStep->GetPostStepPoint()->GetPosition();

      // Protect concurrent histo filling
	    static std::mutex mutex_instance;
	    std::lock_guard<std::mutex> lock(mutex_instance);
  	  m_rzProfRL->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
	    m_rzProfIL->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
	    m_rzProfRL->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
	    m_rzProfIL->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );

      std::vector<std::string> L;
      L.push_back(detName_d);
      L.push_back("Total_X0");
      if( activeSensorHit ){
	      L.push_back("ActiveSensorHits");
	      std::string nHitsPlotName = "ActiveSensorHits_" + std::to_string(m_nActiveSensorHits); // C++11
	      L.push_back(nHitsPlotName);
      }
    
      std::string specialname = "";
      //
      if(matName.find("Peek") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("CFRP") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("CFoam") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("K13D2U") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("Support") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("Carbon") != std::string::npos) specialname = "SupportStructure";
      if(matName.find("Default") != std::string::npos) specialname = "SupportStructure";
      //
      if(matName.find("Default_Pigtail") != std::string::npos) specialname = "Services";
      //
      if(matName.find("Steel") != std::string::npos) specialname = "Steel";
      if(matName.find("BarrelStrip") != std::string::npos) specialname = "Services";
      //if(matName.find("Brl") != std::string::npos) specialname = "Services";
      if(matName.find("Brl") != std::string::npos && matName.find("Default") == std::string::npos && matName.find("Alpine") == std::string::npos ) specialname = "Services";
      if(matName.find("Svc") != std::string::npos) specialname = "Services";
      if(matName.find("InnerIST") != std::string::npos) specialname = "Services";
      if(matName.find("InnerPST") != std::string::npos) specialname = "Services";
      if(matName.find("BarrelPixel") != std::string::npos) specialname = "Services";
      if(matName.find("EndcapPixel") != std::string::npos) specialname = "Services";
      if(matName.find("InnerPixel") != std::string::npos) specialname = "Services";
      if(matName.find("OuterPixel") != std::string::npos) specialname = "Services";
      if(matName.find("pix::Chip") != std::string::npos) specialname = "PixelChips";
      if(matName.find("pix::Hybrid") != std::string::npos) specialname = "PixelChips";
      if(specialname != ""){
	      L.push_back("M_"+layerNum+specialname);
      }else{
	      L.push_back(matName);
      }

      std::string plotstring = "";
      if(m_doDensity){
	      for (auto it : L) {
	        static std::mutex mutex_register;
	        std::lock_guard<std::mutex> lock(mutex_register);
	        plotstring = it;
	        if(!m_rzMapDensity[plotstring]){
	          TString rzname = "RZDensity_"+plotstring;
	          std::string rznameReg = "/lengths/density/RZDensity_"+plotstring;
	          m_rzMapDensity[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,m_rz_zmin,m_rz_zmax,"R [mm]",1000,m_rz_rmin,m_rz_rmax,"Density [g/cm^3]");
	        }
	        m_rzMapDensity[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , density , 1. );	
	        m_rzMapDensity[plotstring]->Fill( endPoint.z() , endPoint.perp() , density , 1. );	
	      }
      }

      if(m_doRL){
	      for (auto it : L) {
	        static std::mutex mutex_register;
	        std::lock_guard<std::mutex> lock(mutex_register);
	        plotstring = it;
	        if(m_doRZ){
	          if(!m_rzMapRL[plotstring]){  
	            TString rzname = "RZRadLen_"+plotstring;
	            std::string rznameReg = "/lengths/radLen/RZRadLen_"+plotstring;
	            m_rzMapRL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,m_rz_zmin,m_rz_zmax,"R [mm]",1000,m_rz_rmin,m_rz_rmax,"X0");
	          }
	          m_rzMapRL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
	          m_rzMapRL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
	        }
	  //
	        if(m_doXY){
	          if(!m_xyMapRL[plotstring]){  
	            TString xyname = "XYRadLen_"+plotstring;
	            std::string xynameReg = "/lengths/radLen/XYRadLen_"+plotstring;
	            m_xyMapRL[plotstring]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"X0");
	          }
	          m_xyMapRL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepRL , 1. );
	          m_xyMapRL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepRL , 1. );
	        }  
	      }
      }
    
      if(m_doIL){    
	      for (auto it : L) {
	        static std::mutex mutex_instance;
	        std::lock_guard<std::mutex> lock(mutex_instance);
	        plotstring = it;
	        if(m_doRZ){
	          if(!m_rzMapIL[plotstring]){  
	            TString rzname = "RZIntLen_"+plotstring;
	            std::string rznameReg = "/lengths/intLen/RZIntLen_"+plotstring;
	            m_rzMapIL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,m_rz_zmin,m_rz_zmax,"R [mm]",1000,m_rz_rmin,m_rz_rmax,"#lambda");
	          }
	          m_rzMapIL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
	          m_rzMapIL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );
	        }
	  //
	        if(m_doXY){
	          if(!m_xyMapIL[plotstring]){  
	            TString xyname = "XYIntLen_"+plotstring;
	            std::string xynameReg = "/lengths/intLen/XYIntLen_"+plotstring;
	            m_xyMapIL[plotstring]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"#lambda");
	          }
	          m_xyMapIL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepIL , 1. );
	          m_xyMapIL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepIL , 1. );
	        }  
	      }
      }
    
      if(m_doEl){ 
	      const G4ElementVector* eVec = mat->GetElementVector();
      
	      if(m_doRL){
	        for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	  
	          static std::mutex mutex_instance;
	          std::lock_guard<std::mutex> lock(mutex_instance);
	  
	          std::string elementName = "E_" + (*eVec)[i]->GetName();
	          double el_thickstep = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;
	  
	          if(m_doRZ){
	            if(!m_rzMapRL[elementName]){
		            std::string rznameReg = "/lengths/radLen/RZRadLen_"+elementName;
		            TString rzname = "RZRadLen_"+elementName;
	  	          m_rzMapRL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,m_rz_zmin,m_rz_zmax,"R [mm]",1000,m_rz_rmin,m_rz_rmax,"X0");
	            }
	            m_rzMapRL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
	            m_rzMapRL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
	          }
	  
	          if (m_doXY) {
	            if(!m_xyMapRL[elementName]){
		            std::string xynameReg = "/lengths/radLen/XYRadLen_"+elementName;
		            TString xyname = "XYRadLen_"+elementName;
		            m_xyMapRL[elementName]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"X0");
	            }
	            m_xyMapRL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
	            m_xyMapRL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );
	          }
	        }
	      }
      
	      if(m_doIL){
	        for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	  
	          static std::mutex mutex_instance;
	          std::lock_guard<std::mutex> lock(mutex_instance);
	  
	          std::string elementName = "E_" + (*eVec)[i]->GetName();
	          G4double lambda0 = 35*g/cm2;
	          //G4Pow* m_g4pow = G4Pow::GetInstance();
	          double el_thickstep = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );
	   
	          if(m_doRZ){
	            if(!m_rzMapIL[elementName]){  
		            TString rzname = "RZIntLen_"+elementName;
		            std::string rznameReg = "/lengths/intLen/RZIntLen_"+elementName;
		            m_rzMapIL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,m_rz_zmin,m_rz_zmax,"R [mm]",1000,m_rz_rmin,m_rz_rmax,"#lambda");
	            }
	            m_rzMapIL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
	            m_rzMapIL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
	          }
	  
	          if(m_doXY){
	            if(!m_xyMapIL[elementName]){  
		            TString xyname = "XYIntLen_"+elementName;
		            std::string xynameReg = "/lengths/intLen/XYIntLen_"+elementName;
		            m_xyMapIL[elementName]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"#lambda");
	            }
	            m_xyMapIL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
	            m_xyMapIL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );
	          }
	        }
	      }

      }

    }

    m_PreviousActiveSensorHit = activeSensorHit;
  }

  std::string LengthIntegrator::getLayerName(double rHit, double zHit){
    
    std::string layerNum = "";
    if(m_splitLayers){
      
      if(m_step < 3.0){ // r-z cuts for step 2.2 layouts

	      if(rHit < 70 && rHit > 34 && fabs(zHit) < 1200 ){
	        layerNum = "_PixelL0_";  // updated
	      }else if(rHit < 140 && rHit > 70 && fabs(zHit) < 1200 ){
	        layerNum = "_PixelL1_";  // updated
	      }else if(rHit < 190 && rHit > 148 && fabs(zHit) < 1200 ){
	        layerNum = "_PixelL2_";  // IST at R145-145.5, so start at 148 to exclude
	      }else if(rHit < 240 && rHit > 190 && fabs(zHit) < 1200 ){
	        layerNum = "_PixelL3_";  // continuous because L2/L3 are linked
	      }else if(rHit < 330 && rHit > 260 && fabs(zHit) < 1200 ){
	        layerNum = "_PixelL4_";  // updated
	  //}else if(rHit < 330 && rHit > 150 && fabs(zHit) < 3174 && fabs(zHit) > 1200 ){
	  //layerNum = "_PixelEC123_";
	      }else if(rHit < 330 && rHit > 265 && fabs(zHit) < 3174 && fabs(zHit) > 1200 ){
	        layerNum = "_PixelEC3_";
	      }else if(rHit < 265 && rHit > 205 && fabs(zHit) < 3174 && fabs(zHit) > 1200 ){
	        layerNum = "_PixelEC2_";
	      }else if(rHit < 205 && rHit > 150 && fabs(zHit) < 3174 && fabs(zHit) > 1200 ){
	        layerNum = "_PixelEC1_";
	      }else if(rHit < 130 && rHit > 45 && fabs(zHit) < 3174 && fabs(zHit) > 1200 ){
	        layerNum = "_PixelEC0_";
	      }

      }else{ // r-z cuts for step 3.0 layout // TO DO

	      if(rHit < 96   && rHit > 34    && fabs(zHit) < 257 ){
	        layerNum = "_PixelL0_";
	      }else if(rHit < 128  && rHit > 96    && fabs(zHit) < 257 ){
	        layerNum = "_PixelL1_";
	      }else if(rHit < 213  && rHit > 146.5 && fabs(zHit) < 1070 ){
	        layerNum = "_PixelL2_";
	      }else if(rHit < 275  && rHit > 213   && fabs(zHit) < 1070 ){
	        layerNum = "_PixelL3_";
	      }else if(rHit < 338  && rHit > 275   && fabs(zHit) < 1070 ){
	        layerNum = "_PixelL4_";
	      }else if(rHit < 338  && rHit > 265   && fabs(zHit) < 3019 && fabs(zHit) > 1120 ){
	        layerNum = "_PixelEC4_";
	      }else if(rHit < 265  && rHit > 205   && fabs(zHit) < 3019 && fabs(zHit) > 1120 ){
	        layerNum = "_PixelEC3_";
	      }else if(rHit < 205  && rHit > 146.5 && fabs(zHit) < 3019 && fabs(zHit) > 1120 ){
	        layerNum = "_PixelEC2_";
	      }else if(rHit < 128  && rHit > 79.2  && fabs(zHit) < 3019 && fabs(zHit) > 257 ){
	        layerNum = "_PixelEC1_";
	      }else if(rHit < 79.2 && rHit > 57.6  && fabs(zHit) < 3019 && fabs(zHit) > 257 ){
	        layerNum = "_PixelECShortys_";
	      }else if(rHit < 57.6 && rHit > 34    && fabs(zHit) < 3019 && fabs(zHit) > 257 ){
	        layerNum = "_PixelEC0_";
	      }

      } // end of pixel r-z cuts

      // strips
      if(m_step < 3.0){ // step 1.x and 2.x

	      if(rHit < 450   && rHit > 362 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL0_";
	      }else if(rHit < 610   && rHit > 515 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL1_";
	      }else if(rHit < 810   && rHit > 715 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL2_";
	      }else if(rHit < 1035  && rHit > 950 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL3_";
	      }else if(rHit > 1050 ){
	        layerNum = "_StripsOuterSupport_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1525 && fabs(zHit) > 1495 ){
	        layerNum = "_StripsEC0_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1715 && fabs(zHit) > 1685 ){
	        layerNum = "_StripsEC1_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1968 && fabs(zHit) > 1938 ){
	        layerNum = "_StripsEC2_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 2270 && fabs(zHit) > 2230 ){
	        layerNum = "_StripsEC3_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 2614 && fabs(zHit) > 2584 ){
	        layerNum = "_StripsEC4_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 3014 && fabs(zHit) > 2984 ){
	        layerNum = "_StripsEC5_";
	      }

      }else{ // step 3.0

	      if(      rHit < 450   && rHit > 345 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL0_";
	      }else if(rHit < 610   && rHit > 500 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL1_";
	      }else if(rHit < 810   && rHit > 700 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL2_";
	      }else if(rHit < 1035  && rHit > 920 && fabs(zHit) < 1400 ){
	        layerNum = "_StripsL3_";
	      }else if(rHit > 1050 ){
	        layerNum = "_StripsOuterSupport_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1540 && fabs(zHit) > 1490 ){
	        layerNum = "_StripsEC0_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1730 && fabs(zHit) > 1680 ){
	        layerNum = "_StripsEC1_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1980 && fabs(zHit) > 1930 ){
	        layerNum = "_StripsEC2_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 2260 && fabs(zHit) > 2210 ){
	        layerNum = "_StripsEC3_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 2560 && fabs(zHit) > 2500 ){
	        layerNum = "_StripsEC4_";
	      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 3880 && fabs(zHit) > 2820 ){
	        layerNum = "_StripsEC5_";
	      }

      }

    }
    return layerNum;

  }

  std::string LengthIntegrator::getVolumeType(std::string s){

    std::string type = "";
    if(m_splitModerator && ( s.find("Moderator") != std::string::npos || s.find("BoratedPolyethylene") != std::string::npos)) type = "Moderator";
    else if(m_splitPP1 && ( s.find("PP0") != std::string::npos || s.find("PP1") != std::string::npos)) type = "PP1";
    else{
      auto colsPos = s.find("::");
      if (colsPos != std::string::npos)
	      type = s.substr(0, colsPos);
      else
	      type=s;
    }
    return type;
  }
/*
  //---------------------------------------------------------------------------
  // Accumulate results from one step
  //---------------------------------------------------------------------------
  void LengthIntegrator::UserSteppingAction(const G4Step* aStep)
  {
    G4TouchableHistory* touchHist =
      (G4TouchableHistory*) aStep->GetPreStepPoint()->GetTouchable();
    G4LogicalVolume* lv = touchHist->GetVolume()->GetLogicalVolume();
    std::string volName = lv->GetName();
    G4Material* mat = lv->GetMaterial();
    double radl = mat->GetRadlen();
    double intl = mat->GetNuclearInterLength();
    double stepl = aStep->GetStepLength();

    // FIXME: using DBL_MAX just ensures double overflow below.
    double thickstepRL = radl != 0 ? stepl/radl : DBL_MAX; //random factor 100 removed
    double thickstepIL = intl != 0 ? stepl/intl : DBL_MAX;

    std::string detName;
    auto colsPos = volName.find("::");
    if (colsPos != std::string::npos)
      detName = volName.substr(0, colsPos);
    else
      detName=volName;
    //detName="Generic";

    std::string matName = "M_" + lv->GetMaterial()->GetName();
    std::string detName_plus_matName = "DM_" + detName + "_" + lv->GetMaterial()->GetName();
    std::string detName_d = "D_" + detName;
    double zLimit = 3512.0; // For ITk studies: 3512mm is the z-limit of the ID End-Plate Cryostat.
    double zHit = aStep->GetPreStepPoint()->GetPosition().z();
    double rLimit = 1152.0; // For ITk studies: 1150mm is the R-limit of the ITk. Note, the solenoid coil is outside of this (at an R of ~1300mm, I think)
    double rHit = aStep->GetPreStepPoint()->GetPosition().perp();

    if(fabs(zHit) < zLimit && rHit < rLimit){

      LengthIntegrator::addToDetThickMap(detName_d,            thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThickMap(matName,              thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThickMap(detName_plus_matName, thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThickMap("Total_X0",           thickstepRL, thickstepIL);

      const G4ElementVector* eVec = mat->GetElementVector();
      for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	std::string elementName = "E_" + (*eVec)[i]->GetName();
	std::string matName_plus_elementName = "ME_" + lv->GetMaterial()->GetName() + "_" + (*eVec)[i]->GetName();
	std::string detName_plus_elementName = "DE_" + detName + "_" + (*eVec)[i]->GetName();
	double el_thickstepRL = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;
	G4double lambda0 = 35*g/cm2;	
	double el_thickstepIL = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );
	LengthIntegrator::addToDetThickMap(elementName,              el_thickstepRL, el_thickstepIL);
	LengthIntegrator::addToDetThickMap(matName_plus_elementName, el_thickstepRL, el_thickstepIL);
	LengthIntegrator::addToDetThickMap(detName_plus_elementName, el_thickstepRL, el_thickstepIL);
      }

    }

    G4ThreeVector hitPoint = aStep->GetPreStepPoint()->GetPosition();
    G4ThreeVector endPoint = aStep->GetPostStepPoint()->GetPosition();

    // Protect concurrent histo filling
    {
      static std::mutex mutex_instance;
      std::lock_guard<std::mutex> lock(mutex_instance);
      m_rzProfRL->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
      m_rzProfIL->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
      m_rzProfRL->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
      m_rzProfIL->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );
    }

    std::vector<std::string> L;
    L.push_back(detName_d);
    L.push_back("Total_X0");

    std::string specialname = "";
    if(matName.find("Support") != std::string::npos) specialname = "CarbonFiber";
    if(matName.find("Carbon") != std::string::npos) specialname = "CarbonFiber";
    if(matName.find("Steel") != std::string::npos) specialname = "Steel";
    if(matName.find("BarrelStrip") != std::string::npos) specialname = "Services";
    if(matName.find("Brl") != std::string::npos) specialname = "Services";
    if(matName.find("Svc") != std::string::npos) specialname = "Services";
    if(matName.find("InnerIST") != std::string::npos) specialname = "Services";
    if(matName.find("InnerPST") != std::string::npos) specialname = "Services";
    if(matName.find("BarrelPixel") != std::string::npos) specialname = "Services";
    if(matName.find("EndcapPixel") != std::string::npos) specialname = "Services";
    if(matName.find("InnerPixel") != std::string::npos) specialname = "Services";
    if(matName.find("OuterPixel") != std::string::npos) specialname = "Services";
    if(matName.find("pix::Chip") != std::string::npos) specialname = "PixelChips";
    if(matName.find("pix::Hybrid") != std::string::npos) specialname = "PixelChips";
    if(specialname != ""){
      L.push_back("M_"+specialname);
    }else{
      L.push_back(matName);
    }

    std::string plotstring = "";
    for (auto it : L) {

      static std::mutex mutex_register;
      std::lock_guard<std::mutex> lock(mutex_register);

      plotstring = it;

      if(!m_rzMapRL[plotstring]){

	TString rzname = "RZRadLen_"+plotstring;
	std::string rznameReg = "/lengths/radLen/RZRadLen_"+plotstring;
	
	TString xyname = "XYRadLen_"+plotstring;
	std::string xynameReg = "/lengths/radLen/XYRadLen_"+plotstring;
	
	m_rzMapRL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3512.,3512.,"R [mm]",1000,0.,1200.,"%X0");
	m_xyMapRL[plotstring]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"%X0");
	
      }

      m_rzMapRL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
      m_rzMapRL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
      m_xyMapRL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepRL , 1. );
      m_xyMapRL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepRL , 1. );

    }

    for (auto it : L) {

      static std::mutex mutex_instance;
      std::lock_guard<std::mutex> lock(mutex_instance);

      plotstring = it;

      if(!m_rzMapIL[plotstring]){
	
	std::string rznameReg = "/lengths/intLen/RZIntLen_"+plotstring;
	TString rzname = "RZIntLen_"+plotstring;
	std::string xynameReg = "/lengths/intLen/XYIntLen_"+plotstring;
	TString xyname = "XYIntLen_"+plotstring;

	m_rzMapIL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3512.,3512.,"R [mm]",1000,0.,1200.,"#lambda");
	m_xyMapIL[plotstring]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"#lambda");
	
      }

      m_rzMapIL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
      m_rzMapIL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );
      m_xyMapIL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepIL , 1. );
      m_xyMapIL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepIL , 1. );

    }


    const G4ElementVector* eVec = mat->GetElementVector();
    for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {

      static std::mutex mutex_instance;
      std::lock_guard<std::mutex> lock(mutex_instance);

      std::string elementName = "E_" + (*eVec)[i]->GetName();
      double el_thickstep = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;

      if(!m_rzMapRL[elementName]){

	std::string rznameReg = "/lengths/radLen/RZRadLen_"+elementName;
	TString rzname = "RZRadLen_"+elementName;
	TString xyname = "XYRadLen_"+elementName;
	std::string xynameReg = "/lengths/radLen/XYRadLen_"+elementName;

	m_rzMapRL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3512.,3512.,"R [mm]",1000,0.,1200.,"%X0");
	m_xyMapRL[elementName]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"%X0");
	
      }

      m_rzMapRL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
      m_rzMapRL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
      m_xyMapRL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
      m_xyMapRL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );

    }

    for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {

      static std::mutex mutex_instance;
      std::lock_guard<std::mutex> lock(mutex_instance);

      std::string elementName = "E_" + (*eVec)[i]->GetName();
      G4double lambda0 = 35*g/cm2;
      double el_thickstep = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );

      if(!m_rzMapIL[elementName]){

	TString rzname = "RZIntLen_"+elementName;
	std::string rznameReg = "/lengths/intLen/RZIntLen_"+elementName;
	TString xyname = "XYIntLen_"+elementName;
	std::string xynameReg = "/lengths/intLen/XYIntLen_"+elementName;

	m_rzMapIL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3512.,3512.,"R [mm]",1000,0.,1200.,"#lambda");
	m_xyMapIL[elementName]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"#lambda");

      }

      m_rzMapIL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
      m_rzMapIL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
      m_xyMapIL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
      m_xyMapIL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );

    }

  }
*/
  /// note that this should be called from a section protected by a mutex, since it talks to the THitSvc

  TProfile2D* LengthIntegrator::getOrCreateProfile(std::string regName, TString histoname, TString xtitle, int nbinsx, float xmin, float xmax, TString ytitle, int nbinsy,float ymin, float ymax,TString ztitle){

    if(m_hSvc->exists(regName)){
      TH2* histo=nullptr;
      if(m_hSvc->getHist(regName,histo).isSuccess())
	return dynamic_cast<TProfile2D*>(histo);
    } else {
      TProfile2D* result= new TProfile2D(histoname,histoname,nbinsx,xmin,xmax,nbinsy,ymin,ymax);
      result->GetXaxis()->SetTitle(xtitle);
      result->GetYaxis()->SetTitle(ytitle);
      result->GetZaxis()->SetTitle(ztitle);

      if (m_hSvc && m_hSvc->regHist(regName,result).isFailure()){
	throw GaudiException("Registration of histogram " + regName + " failed", "RegHistErr", StatusCode::FAILURE);
      }
      return result;
    }

    // should never be here
    G4cout<<"ERROR something went wrong in handling of THistSvc "<<regName <<" "<<histoname<<G4endl;
    return nullptr;
  }

  //---------------------------------------------------------------------------
  // Add elements and values to the map
  //---------------------------------------------------------------------------
  void LengthIntegrator::addToDetThickMap(std::string name, double thickstepRL, double thickstepIL)
  {
    auto it=m_detThickMap.find(name);
    if(it!=m_detThickMap.end()){
      (*it).second.first+=thickstepRL;
      (*it).second.second+=thickstepIL;
    } else {
      m_detThickMap.insert(std::map<std::string,std::pair<double,double>,std::less<std::string> >::value_type( name, std::pair<double,double>( thickstepRL, thickstepIL) ) );
    }
  }
/*
  //---------------------------------------------------------------------------
  // Setup hists for one detector
  //---------------------------------------------------------------------------
  void LengthIntegrator::regAndFillHist(const std::string& detName, const std::pair<double, double>& thicks)
  {
    TProfile* profEtaRL = nullptr;
    TProfile* profEtaIL = nullptr;
    TProfile* profPhiRL = nullptr;
    TProfile* profPhiIL = nullptr;

    auto pathEtaRL = "/lengths/radLen/" + detName + "_RL";
    auto pathEtaIL = "/lengths/intLen/" + detName + "_IL";
    auto pathPhiRL = "/lengths/radLen/" + detName + "Phi_RL";
    auto pathPhiIL = "/lengths/intLen/" + detName + "Phi_IL";

    // Eta rad profile
    if(!getHist(m_hSvc, pathEtaRL, profEtaRL)) {
      const std::string name(detName+"_RL");
      profEtaRL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
      profEtaRL->GetXaxis()->SetTitle("#eta");
      profEtaRL->GetYaxis()->SetTitle("%X0");
      regHist(m_hSvc, pathEtaRL, profEtaRL);
    }
    // Eta int profile
    if(!getHist(m_hSvc, pathEtaIL, profEtaIL)) {
      const std::string name(detName+"_IL");
      profEtaIL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
      profEtaIL->GetXaxis()->SetTitle("#eta");
      profEtaIL->GetYaxis()->SetTitle("#lambda");
      regHist(m_hSvc, pathEtaIL, profEtaIL);
    }
    // Phi rad profile
    if(!getHist(m_hSvc, pathPhiRL, profPhiRL)) {
      const std::string name(detName+"Phi_RL");
      profPhiRL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
      profPhiRL->GetXaxis()->SetTitle("#phi");
      profPhiRL->GetYaxis()->SetTitle("%X0");
      regHist(m_hSvc, pathPhiRL, profPhiRL);
    }
    // Phi int profile
    if(!getHist(m_hSvc, pathPhiIL, profPhiIL)) {
      const std::string name(detName+"Phi_IL");
      profPhiIL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
      profPhiIL->GetXaxis()->SetTitle("#phi");
      profPhiIL->GetYaxis()->SetTitle("#lambda");
      regHist(m_hSvc, pathPhiIL, profPhiIL);
    }

    m_etaMapRL[detName] = profEtaRL;
    m_etaMapIL[detName] = profEtaIL;
    m_phiMapRL[detName] = profPhiRL;
    m_phiMapIL[detName] = profPhiIL;

    profEtaRL->Fill(m_etaPrimary, thicks.first, 1.);
    profEtaIL->Fill(m_etaPrimary, thicks.second, 1.);
    profPhiRL->Fill(m_phiPrimary, thicks.first, 1.);
    profPhiIL->Fill(m_phiPrimary, thicks.second, 1.);
  }

*/


} // namespace G4UA
