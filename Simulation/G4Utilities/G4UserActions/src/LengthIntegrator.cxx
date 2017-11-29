/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/LengthIntegrator.h"
#include "SimHelpers/ServiceAccessor.h"

#include "TProfile.h"
#include "TProfile2D.h"

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

    m_g4pow = G4Pow::GetInstance();

  }

  //---------------------------------------------------------------------------
  // Cache primary info at beginning of event
  //---------------------------------------------------------------------------
  void LengthIntegrator::beginOfEvent(const G4Event* event)
  {
    m_detThickMap.clear();
    G4PrimaryVertex* vert = event->GetPrimaryVertex(0);
    G4PrimaryParticle* part = vert->GetPrimary();
    G4ThreeVector mom = part->GetMomentum();
    m_etaPrimary = mom.eta();
    m_phiPrimary = mom.phi();
  }

  //---------------------------------------------------------------------------
  // Finalize event measurements
  //---------------------------------------------------------------------------
  void LengthIntegrator::endOfEvent(const G4Event*)
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

    /// Putting this here, as normally I'd add the following code into
    /// a finalize function (as has been done above for the
    /// Athena(non-MP) code), but I'm not sure if overloading the
    /// finalize function in AthenaMP will break the histogram merging
    /// at the end of the job. If it wont, then the following code can
    /// be put in a finalize function, which will speed up AthenaMP
    /// jobs.
    /// Adding zeros to TProfile bins, so that each bin contains the
    /// same number of entries, so that a THStack of all the material
    /// TProfile plots (or all the element plots) will equal the
    /// Total_X0 TProfile plot It's because each plot (for each
    /// material, say) is only filled if a Geantion hits it, not if
    /// it's not hit in an event

    TProfile* totalEtaRL = m_etaMapRL["Total_X0"];
    int nbins = totalEtaRL->GetNbinsX();
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

  //---------------------------------------------------------------------------
  // Accumulate results from one step
  //---------------------------------------------------------------------------
  void LengthIntegrator::processStep(const G4Step* aStep)
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
    double thickstepRL = radl != 0 ? stepl/radl *100 : DBL_MAX;
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
