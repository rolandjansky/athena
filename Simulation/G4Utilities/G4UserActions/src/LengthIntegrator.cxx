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
      m_doRL(0),m_doIL(0),m_doRZ(0),m_doXY(0),m_doEl(0),
      m_hSvc(histSvcName, "LengthIntegrator"),
      m_etaPrimary(0), m_phiPrimary(0),
      m_zPrimary(0),m_rPrimary(0),m_nActiveSensorHits(0),m_PreviousActiveSensorHit(0),m_totalX0(0),m_totalN0(0),
      m_rzProfRL(nullptr), m_rzProfIL(nullptr)
  {
    
    m_doRL = true;
    m_doIL = true;
    m_doRZ = true;
    m_doXY = false;
    m_doEl = true;
    
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
    m_detThick.clear();
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
    TProfile* profZRL = nullptr;
    TProfile* profZIL = nullptr;
    TProfile* profRRL = nullptr;
    TProfile* profRIL = nullptr;

    auto pathEtaRL = "/lengths/radLen/" + detName + "_RL";
    auto pathEtaIL = "/lengths/intLen/" + detName + "_IL";
    auto pathPhiRL = "/lengths/radLen/" + detName + "Phi_RL";
    auto pathPhiIL = "/lengths/intLen/" + detName + "Phi_IL";
    auto pathZRL = "/lengths/radLen/" + detName + "Z_RL";
    auto pathZIL = "/lengths/intLen/" + detName + "Z_IL";
    auto pathRRL = "/lengths/radLen/" + detName + "R_RL";
    auto pathRIL = "/lengths/intLen/" + detName + "R_IL";

    // Eta rad profile
    if(m_doRL) {
      if(!getHist(m_hSvc, pathEtaRL, profEtaRL)) {
	const std::string name(detName+"_RL");
	profEtaRL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
	profEtaRL->GetXaxis()->SetTitle("#eta");
	profEtaRL->GetYaxis()->SetTitle("%X0");
	regHist(m_hSvc, pathEtaRL, profEtaRL);
      }
    }
    // Eta int profile
    if(m_doIL){	
      if(!getHist(m_hSvc, pathEtaIL, profEtaIL)) {
	const std::string name(detName+"_IL");
	profEtaIL = new TProfile(name.c_str(), name.c_str(), 1000, -6., 6.);
	profEtaIL->GetXaxis()->SetTitle("#eta");
	profEtaIL->GetYaxis()->SetTitle("#lambda");
	regHist(m_hSvc, pathEtaIL, profEtaIL);
      }
    }
    // Phi rad profile
    if(m_doRL) {
      if(!getHist(m_hSvc, pathPhiRL, profPhiRL)) {
	const std::string name(detName+"Phi_RL");
	profPhiRL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
	profPhiRL->GetXaxis()->SetTitle("#phi");
	profPhiRL->GetYaxis()->SetTitle("%X0");
	regHist(m_hSvc, pathPhiRL, profPhiRL);
      }
    }
    // Phi int profile
    if(m_doIL){
      if(!getHist(m_hSvc, pathPhiIL, profPhiIL)) {
	const std::string name(detName+"Phi_IL");
	profPhiIL = new TProfile(name.c_str(), name.c_str(), 500, -M_PI, M_PI);
	profPhiIL->GetXaxis()->SetTitle("#phi");
	profPhiIL->GetYaxis()->SetTitle("#lambda");
	regHist(m_hSvc, pathPhiIL, profPhiIL);
      }
    }
    // Z rad profile
    if(m_doRL){
      if(!getHist(m_hSvc, pathZRL, profZRL)) {
	const std::string name(detName+"Z_RL");
	profZRL = new TProfile(name.c_str(), name.c_str(), 704, 0, 3520);
	profZRL->GetXaxis()->SetTitle("z");
	profZRL->GetYaxis()->SetTitle("%X0");
	regHist(m_hSvc, pathZRL, profZRL);
      }
    }
    // Z int profile
    if(m_doIL){
      if(!getHist(m_hSvc, pathZIL, profZIL)) {
	const std::string name(detName+"Z_IL");
	profZIL = new TProfile(name.c_str(), name.c_str(), 704, 0, 3520);
	profZIL->GetXaxis()->SetTitle("z");
	profZIL->GetYaxis()->SetTitle("#lambda");
	regHist(m_hSvc, pathZIL, profZIL);
      }
    }
    // R rad profile
    if(m_doRL){
      if(!getHist(m_hSvc, pathRRL, profRRL)) {
	const std::string name(detName+"R_RL");
	profRRL = new TProfile(name.c_str(), name.c_str(), 1150, 0, 1150);
	profRRL->GetXaxis()->SetTitle("r");
	profRRL->GetYaxis()->SetTitle("%X0");
	regHist(m_hSvc, pathRRL, profRRL);
      }
    }
    // R int profile
    if(m_doIL){
      if(!getHist(m_hSvc, pathRIL, profRIL)) {
	const std::string name(detName+"R_IL");
	profRIL = new TProfile(name.c_str(), name.c_str(), 1150, 0, 1150);
	profRIL->GetXaxis()->SetTitle("r");
	profRIL->GetYaxis()->SetTitle("#lambda");
	regHist(m_hSvc, pathRIL, profRIL);
      }
    }
    //
    if(m_doRL){
      m_etaMapRL[detName] = profEtaRL;
      profEtaRL->Fill(m_etaPrimary, thicks.first, 1.);
      //
      m_phiMapRL[detName] = profPhiRL;
      profPhiRL->Fill(m_phiPrimary, thicks.first, 1.);
      //
      m_zMapRL[detName] = profZRL;
      profZRL->Fill(m_zPrimary, thicks.first, 1.);
      //
      m_rMapRL[detName] = profRRL;
      profRRL->Fill(m_rPrimary, thicks.first, 1.);
    }
    if(m_doIL){
      m_etaMapIL[detName] = profEtaIL;
      profEtaIL->Fill(m_etaPrimary, thicks.second, 1.);
      //
      m_phiMapIL[detName] = profPhiIL;
      profPhiIL->Fill(m_phiPrimary, thicks.second, 1.);
      //
      m_zMapIL[detName] = profZIL;
      profZIL->Fill(m_zPrimary, thicks.second, 1.);
      //
      m_rMapIL[detName] = profRIL;
      profRIL->Fill(m_rPrimary, thicks.second, 1.);
    }
    //
  }

  //---------------------------------------------------------------------------
  // Finalize event measurements
  //---------------------------------------------------------------------------
  void LengthIntegrator::endOfEvent(const G4Event*)
  {
    // Lazily protect this whole code from concurrent access
    std::lock_guard<std::mutex> lock(gHistSvcMutex);

    // Loop over volumes
    for (auto& it : m_detThick) {

      // If histos already exist, then fill them
      if (m_etaMapRL.find(it.first) != m_etaMapRL.end()) {
	
	if(m_doRL){
	  m_etaMapRL[it.first]->Fill(m_etaPrimary, it.second.first, 1.);
	  m_phiMapRL[it.first]->Fill(m_phiPrimary, it.second.first, 1.);
	  m_zMapRL[it.first]->Fill(m_zPrimary,it.second.first,1.);
	  m_rMapRL[it.first]->Fill(m_rPrimary,it.second.first,1.);
	}
	//
	if(m_doIL){
	  m_etaMapIL[it.first]->Fill(m_etaPrimary, it.second.second, 1.);
	  m_phiMapIL[it.first]->Fill(m_phiPrimary, it.second.second, 1.);
	  m_zMapIL[it.first]->Fill(m_zPrimary,it.second.second,1.);
	  m_rMapIL[it.first]->Fill(m_rPrimary,it.second.second,1.);
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
    
    if(m_doIL){
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
    double thickstepRL = radl != 0 ? stepl/radl : DBL_MAX;
    double thickstepIL = intl != 0 ? stepl/intl : DBL_MAX;

    m_totalX0 += thickstepRL;
    m_totalN0 += thickstepIL;
    
    bool activeSensorHit = false;
    if( lv->GetMaterial()->GetName().find("Silicon") != std::string::npos || lv->GetMaterial()->GetName().find("SiMetal") != std::string::npos ) activeSensorHit = true;
    if( activeSensorHit ) m_nActiveSensorHits++;
    if( activeSensorHit && m_PreviousActiveSensorHit ) std::cout << "UH OH! Multiple hits in the same sensor?" << std::endl;
    
    bool splitModerator = true;
    bool splitPP1 = true;
    bool splitLayers = true;
    
    std::string detName;
    
    if(splitModerator && ( lv->GetMaterial()->GetName().find("Moderator") != std::string::npos || lv->GetMaterial()->GetName().find("BoratedPolyethylene") != std::string::npos ) ){
      detName="Moderator";
    }else if(splitPP1 && ( lv->GetMaterial()->GetName().find("PP0") != std::string::npos || lv->GetMaterial()->GetName().find("PP1") != std::string::npos ) ){
      detName="PP1";
    }else{
      auto colsPos = volName.find("::");
      if (colsPos != std::string::npos)
	detName = volName.substr(0, colsPos);
      else
	detName=volName;
      //detName="Generic";
    }
    
    double zLimit = 3475.0; // For ITk studies: 3475mm is the z-limit of the ID End-Plate Cryostat (cf. talk by Jan Stark at Simu meeting on Dec. 14th,2016).
    double zHit = aStep->GetPreStepPoint()->GetPosition().z();
    double rLimit = 1148.0; // For ITk studies: 1148mm is the R-limit of the ITk. Note, the solenoid coil is outside of this (at an R of ~1300mm, I think)
    double rHit = aStep->GetPreStepPoint()->GetPosition().perp();

    std::string layerNum = "";
    if(splitLayers){
      if(rHit < 70 && rHit > 34 && fabs(zHit) < 1225 ){
	layerNum = "_PixelL0_";
      }else if(rHit < 110 && rHit > 70 && fabs(zHit) < 1225 ){
	layerNum = "_PixelL1_";
      }else if(rHit < 185 && rHit > 145 && fabs(zHit) < 740 ){
	layerNum = "_PixelL2_";
      }else if(rHit < 245 && rHit > 205 && fabs(zHit) < 740 ){
	layerNum = "_PixelL3_";
      }else if(rHit < 300 && rHit > 260 && fabs(zHit) < 740 ){
	layerNum = "_PixelL4_";
      }else if(rHit < 450 && rHit > 362 && fabs(zHit) < 1400 ){
	layerNum = "_StripsL0_";
      }else if(rHit < 610 && rHit > 515 && fabs(zHit) < 1400 ){
	layerNum = "_StripsL1_";
      }else if(rHit < 810 && rHit > 715 && fabs(zHit) < 1400 ){
	layerNum = "_StripsL2_";
      }else if(rHit < 1035 && rHit > 950 && fabs(zHit) < 1400 ){
	layerNum = "_StripsL3_";
      }else if(rHit > 1050 ){
	layerNum = "_StripsOuterSupport_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1525 && fabs(zHit) > 1495 ){
	layerNum = "_StripsEC0_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1715 && fabs(zHit) > 1685 ){
	layerNum = "_StripsEC1_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1968 && fabs(zHit) > 1938 ){
	layerNum = "_StripsEC2_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 1270 && fabs(zHit) > 1240 ){
	layerNum = "_StripsEC3_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 2614 && fabs(zHit) > 2584 ){
	layerNum = "_StripsEC4_";
      }else if(rHit < 975.6 && rHit > 379 && fabs(zHit) < 3014 && fabs(zHit) > 2984 ){
	layerNum = "_StripsEC5_";
      }
    }
    
    std::string matName = "M_" + layerNum + lv->GetMaterial()->GetName();
    //std::string matName = "M_" + lv->GetMaterial()->GetName();
    std::string detName_plus_matName = "DM_" + detName + "_" + lv->GetMaterial()->GetName();
    std::string detName_d = "D_" + detName;
    
    if(fabs(zHit) < zLimit && rHit < rLimit){
      
      LengthIntegrator::addToDetThick(detName_d,            thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThick(matName,              thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThick(detName_plus_matName, thickstepRL, thickstepIL);
      LengthIntegrator::addToDetThick("Total_X0",           thickstepRL, thickstepIL);
      
      if( activeSensorHit ){
	//std::string nHitsPlotName = "ActiveSensorHit_" + std::string(std::itoa(m_nActiveSensorHits); // Old
	std::string nHitsPlotName = "ActiveSensorHit_" + std::to_string(m_nActiveSensorHits); // C++11
	LengthIntegrator::addToDetThick(nHitsPlotName, m_totalX0, m_totalN0);
      }

      if(m_doEl){
	const G4ElementVector* eVec = mat->GetElementVector();
	for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
	  //std::string elementName = "E_" + (*eVec)[i]->GetName();
	  std::string elementName = "E_" + layerNum + (*eVec)[i]->GetName();
	  //std::string matName_plus_elementName = "ME_" + lv->GetMaterial()->GetName() + "_" + (*eVec)[i]->GetName();
	  std::string matName_plus_elementName = "ME_" + layerNum + lv->GetMaterial()->GetName() + "_" + (*eVec)[i]->GetName();
	  std::string detName_plus_elementName = "DE_" + detName + "_" + (*eVec)[i]->GetName();
	  double el_thickstepRL = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;
	  G4double lambda0 = 35*g/cm2;	
	  double el_thickstepIL = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );
	  LengthIntegrator::addToDetThick(elementName,              el_thickstepRL, el_thickstepIL);
	  LengthIntegrator::addToDetThick(matName_plus_elementName, el_thickstepRL, el_thickstepIL);
	  //LengthIntegrator::addToDetThick(detName_plus_elementName, el_thickstepRL, el_thickstepIL);              // Some users may want to put this series of plots back in
	}
      }

    }

    //G4ThreeVector midPoint = (aStep->GetPreStepPoint()->GetPosition()+aStep->GetPostStepPoint()->GetPosition())*0.5;
    //m_rzProfRL->Fill( midPoint.z() , midPoint.perp() , thickstepRL , 1. );
    //m_rzProfIL->Fill( midPoint.z() , midPoint.perp() , thickstepIL , 1. );

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
    //L.push_back(matName);
    //L.push_back(detName_plus_matName);
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
    if(m_doRL){
      for (auto it : L) {
	static std::mutex mutex_register;
	std::lock_guard<std::mutex> lock(mutex_register);
	plotstring = it;
	//G4cout<<"processing string "<<plotstring<<G4endl;;
	if(m_doRZ){
	  if(!m_rzMapRL[plotstring]){  
	    TString rzname = "RZRadLen_"+plotstring;
	    std::string rznameReg = "/lengths/radLen/RZRadLen_"+plotstring;
	    m_rzMapRL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3475.,3475.,"R [mm]",1000,0.,1200.,"%X0");
	  }
	  m_rzMapRL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
	  m_rzMapRL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
	}
	//
	if(m_doXY){
	  if(!m_xyMapRL[plotstring]){  
	    TString xyname = "XYRadLen_"+plotstring;
	    std::string xynameReg = "/lengths/radLen/XYRadLen_"+plotstring;
	    m_xyMapRL[plotstring]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"%X0");
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
	    m_rzMapIL[plotstring]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3475.,3475.,"R [mm]",1000,0.,1200.,"#lambda");
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
	      m_rzMapRL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3475.,3475.,"R [mm]",1000,0.,1200.,"%X0");
	    }
	    m_rzMapRL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
	    m_rzMapRL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
	  }
	  
	  if (m_doXY) {
	    if(!m_xyMapRL[elementName]){
	      std::string xynameReg = "/lengths/radLen/XYRadLen_"+elementName;
	      TString xyname = "XYRadLen_"+elementName;
	      m_xyMapRL[elementName]=getOrCreateProfile(xynameReg, xyname, "X [mm]", 1000,-1200.,1200.,"Y [mm]",1000,-1200.,1200.,"%X0");
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
	      m_rzMapIL[elementName]=getOrCreateProfile(rznameReg, rzname, "Z [mm]", 1000,-3475.,3475.,"R [mm]",1000,0.,1200.,"#lambda");
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

      m_PreviousActiveSensorHit = activeSensorHit;
    }

  }  


  /// note that this should be called from a section protected by a mutex, since it talks to the THistSvc
  
  TProfile2D* LengthIntegrator::getOrCreateProfile(std::string regName, TString histoname, TString xtitle, int nbinsx, float xmin, float xmax, TString ytitle, int nbinsy,float ymin, float ymax,TString ztitle){

    //G4cout<<"histo "<<histoname<<" not found. checking for  "<<regName<<G4endl;

    if(m_hSvc->exists(regName)){
      TH2* histo=nullptr;
      if(m_hSvc->getHist(regName,histo).isSuccess())
	return dynamic_cast<TProfile2D*>(histo);
    } else {
      //G4cout<<"...... creating  "<<regName<<G4endl;
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
  void LengthIntegrator::addToDetThick(std::string name, double thickstepRL, double thickstepIL) 
  {
    auto it=m_detThick.find(name);
    if(it!=m_detThick.end()){
      (*it).second.first+=thickstepRL;
      (*it).second.second+=thickstepIL;
    } else {
      m_detThick.insert(std::map<std::string,std::pair<double,double>,std::less<std::string> >::value_type( name, std::pair<double,double>( thickstepRL, thickstepIL) ) );
    }
  }

} // namespace G4UA
