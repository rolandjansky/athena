/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "LengthIntegrator.h"
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
               const std::string& histPath, HistType*& ptr){
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
      m_hSvc(histSvcName, "LengthIntegrator"),
      m_etaPrimary(0), m_phiPrimary(0),
      m_rzProfRL(nullptr), m_rzProfIL(nullptr)
  {
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

    //find a good way to configure this?
    m_splitModerator = true;
    m_splitPP1 = true;


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
    m_etaPrimary = mom.eta();
    m_phiPrimary = mom.phi();

    m_genEta = std::isfinite(mom.eta()) ? mom.eta() : -999;
    m_genPhi = mom.phi();
    m_genZ = vert->GetZ0();
    m_genR = sqrt((vert->GetX0()*vert->GetX0())+(vert->GetY0()*vert->GetY0()));
    m_genNPart++;

    m_total_X0 = 0;
    m_total_L0 = 0;


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
        m_etaMapRL[it.first]->Fill(m_etaPrimary, it.second.first, 1.);
        m_phiMapRL[it.first]->Fill(m_phiPrimary, it.second.first, 1.);

        m_etaMapIL[it.first]->Fill(m_etaPrimary, it.second.second, 1.);
        m_phiMapIL[it.first]->Fill(m_phiPrimary, it.second.second, 1.);
      }
      // New detector volume; register it
      else {
        regAndFillHist(it.first, it.second);
      }

    } // Loop over detectors


    fillNtuple(); 

 }


  std::string LengthIntegrator::getMaterialClassification(std::string name)
  {

    if((name.find("DM_Atlas_Air") != std::string::npos) || (name.find("DM_Atlas") != std::string::npos) ||
       (name.find("pix::HEX") != std::string::npos)){
      return "NONE";
    }

    if((name.find("Silicon") != std::string::npos) || (name.find("SiMetal") != std::string::npos)){
      return "ActiveSensors";
    }


    //These material categories and groupings are based on the ITk
    //for other purposes these need to be different

    if(name.find("SCT_TiMetal_heat") != std::string::npos) return "HeatExchangers";
    if(name.find("pix::HEX") != std::string::npos) return "HeatExchangers";
    if(name.find("HeatExchangers") != std::string::npos) return "PP1"; // comment out this line to see the Heat Exchangers separately in the final plot
    
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

    // Do you want to split cooling and services where possible?
    if(name.find("Cooling") != std::string::npos) return "Services";

    if(name.find("Inconel") != std::string::npos) return "BeamPipe";
    if(name.find("Aerogel") != std::string::npos) return "BeamPipe";
    if(name.find("Beryllium") != std::string::npos) return "BeamPipe";
    if(name.find("Getter") != std::string::npos) return "BeamPipe";
    if(name.find("Vacuum") != std::string::npos) return "BeamPipe";
    
    
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

    if(name.find("PP0") != std::string::npos) return "PP1"; //Grouping PP0 and PP1
    
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

    if(name.find("Pigtail") != std::string::npos) return "Services"; //Overwritten by some other statement...
   
    return "NONE";

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
  


  void LengthIntegrator::fillNtuple(){
      
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

  }


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

    std::string volumetype = getVolumeType(matName);

    m_collected_groupedmaterial.push_back(groupmaterial);
    m_collected_volumetype.push_back(volumetype);

    // Protect concurrent histo filling
    {
      static std::mutex mutex_instance;
      std::lock_guard<std::mutex> lock(mutex_instance);
      m_rzProfRL->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
      m_rzProfIL->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
      m_rzProfRL->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
      m_rzProfIL->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );
    }


  }

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
	//ATH_MSG_FATAL( "Registration of histogram " << rznameReg << " failed" );
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

  //---------------------------------------------------------------------------
  // Setup hists for one detector
  //---------------------------------------------------------------------------
  void LengthIntegrator::regAndFillHist(const std::string& detName,
                                        const std::pair<double, double>& thicks)
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

} // namespace G4UA
