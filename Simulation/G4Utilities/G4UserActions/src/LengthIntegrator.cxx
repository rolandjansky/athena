/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "G4UserActions/LengthIntegrator.h"
#include <string>
#include <map>
#include "SimHelpers/ServiceAccessor.h"

#include "TH1.h"
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


LengthIntegrator::LengthIntegrator(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent),m_g4pow(0),m_etaPrimary(0),m_phiPrimary(0),m_rzProfRL(0),m_rzProfIL(0),m_hSvc("THistSvc",name) {

  declareProperty("HistoSvc",m_hSvc);

}

StatusCode LengthIntegrator::initialize(){

  CHECK(m_hSvc.retrieve());

  // register RZ profile
  // other profiles need to wait until the end of the events
  // as they are created only if used

  m_rzProfRL = new TProfile2D("RZRadLen","RZRadLen",1000,-25000.,25000.,2000,0.,15000.);
  CHECK(m_hSvc->regHist("/lengths/radLen/RZRadLen",m_rzProfRL));
  
  m_rzProfIL = new TProfile2D("RZIntLen","RZIntLen",1000,-25000.,25000.,2000,0.,15000.);
  CHECK(m_hSvc->regHist("/lengths/intLen/RZIntLen",m_rzProfIL));

  m_g4pow = G4Pow::GetInstance();
  
  return StatusCode::SUCCESS;
  
}

void LengthIntegrator::BeginOfEvent(const G4Event* anEvent)
{
  m_detThick.clear();
  G4PrimaryVertex *vert=anEvent->GetPrimaryVertex(0);
  G4PrimaryParticle *part=vert->GetPrimary();
  G4ThreeVector mom=part->GetMomentum();
  m_etaPrimary=mom.eta();
  m_phiPrimary=mom.phi();
  
}

void LengthIntegrator::regAndFillHist(const std::string& detname, const std::pair<double,double>& thicks){
  
  // Eta profile
  TProfile *profRL=new TProfile((detname+"_RL").c_str(),(detname+"_RL").c_str(),1000,-6.,6.);
  TProfile *profIL=new TProfile((detname+"_IL").c_str(),(detname+"_IL").c_str(),1000,-6.,6.);
  profRL->GetXaxis()->SetTitle("#eta");
  profRL->GetYaxis()->SetTitle("%X0");
  profIL->GetXaxis()->SetTitle("#eta");
  profIL->GetYaxis()->SetTitle("#lambda");
  // Phi profile
  TProfile *profPhiRL=new TProfile((detname+"Phi_RL").c_str(),(detname+"Phi_RL").c_str(),500,-M_PI,M_PI);
  TProfile *profPhiIL=new TProfile((detname+"Phi_IL").c_str(),(detname+"Phi_IL").c_str(),500,-M_PI,M_PI);
  profPhiRL->GetXaxis()->SetTitle("#phi");
  profPhiRL->GetYaxis()->SetTitle("%X0");
  profPhiIL->GetXaxis()->SetTitle("#phi");
  profPhiIL->GetYaxis()->SetTitle("#lambda");
  
  if(m_hSvc->regHist("/lengths/radLen/"+detname+"_RL",profRL).isSuccess()&&
     m_hSvc->regHist("/lengths/intLen/"+detname+"_IL",profIL).isSuccess()&&
     m_hSvc->regHist("/lengths/radLen/"+detname+"Phi_RL",profPhiRL).isSuccess()&&
     m_hSvc->regHist("/lengths/intLen/"+detname+"Phi_IL",profPhiIL).isSuccess()){
    
    m_etaMapRL[detname]=profRL;
    m_etaMapIL[detname]=profIL;
    m_phiMapRL[detname]=profPhiRL;
    m_phiMapIL[detname]=profPhiIL;
    
    profRL->Fill(m_etaPrimary,thicks.first,1.);
    profIL->Fill(m_etaPrimary,thicks.second,1.);
    profPhiRL->Fill(m_phiPrimary,thicks.first,1.);
    profPhiIL->Fill(m_phiPrimary,thicks.second,1.);

  } else {

    ATH_MSG_FATAL("could not register needed histograms");

  }
  
  
}

void LengthIntegrator::EndOfEvent(const G4Event*)
{
  
  for (auto it : m_detThick) {

    if (m_etaMapRL.find(it.first)!=m_etaMapRL.end()) {
      m_etaMapRL[it.first]->Fill(m_etaPrimary,it.second.first,1.);
      m_phiMapRL[it.first]->Fill(m_phiPrimary,it.second.first,1.);

      m_etaMapIL[it.first]->Fill(m_etaPrimary,it.second.second,1.);
      m_phiMapIL[it.first]->Fill(m_phiPrimary,it.second.second,1.);

    } else {
     
      regAndFillHist(it.first,it.second);
      
    } // First event, or not?
  } // Loop over detectors
  
}

void LengthIntegrator::Step(const G4Step* aStep)
{
  G4TouchableHistory* touchHist = (G4TouchableHistory*)aStep->GetPreStepPoint()->GetTouchable();
  G4LogicalVolume *lv=touchHist->GetVolume()->GetLogicalVolume();
  std::string volName=lv->GetName();
  G4Material *mat=lv->GetMaterial();
  double radl=mat->GetRadlen();
  double intl=mat->GetNuclearInterLength();
  double stepl=aStep->GetStepLength();

  double thickstepRL= radl != 0 ? stepl/radl *100 : DBL_MAX;
  double thickstepIL= intl != 0 ? stepl/intl : DBL_MAX;

  std::string::size_type npos;
  std::string detName;
  npos=volName.find("::");

  if (npos!=std::string::npos)
    detName=volName.substr(0,npos);
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

    LengthIntegrator::addToDetThick(detName_d,            thickstepRL, thickstepIL);
    LengthIntegrator::addToDetThick(matName,              thickstepRL, thickstepIL);
    LengthIntegrator::addToDetThick(detName_plus_matName, thickstepRL, thickstepIL);
    LengthIntegrator::addToDetThick("Total_X0",           thickstepRL, thickstepIL);

    const G4ElementVector* eVec = mat->GetElementVector();
    for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
      std::string elementName = "E_" + (*eVec)[i]->GetName();
      std::string matName_plus_elementName = "ME_" + lv->GetMaterial()->GetName() + "_" + (*eVec)[i]->GetName();
      std::string detName_plus_elementName = "DE_" + detName + "_" + (*eVec)[i]->GetName();
      double el_thickstepRL = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;
      G4double lambda0 = 35*g/cm2;
      double el_thickstepIL = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );
      LengthIntegrator::addToDetThick(elementName,              el_thickstepRL, el_thickstepIL);
      LengthIntegrator::addToDetThick(matName_plus_elementName, el_thickstepRL, el_thickstepIL);
      LengthIntegrator::addToDetThick(detName_plus_elementName, el_thickstepRL, el_thickstepIL);
    }

  }
  
  //G4ThreeVector midPoint = (aStep->GetPreStepPoint()->GetPosition()+aStep->GetPostStepPoint()->GetPosition())*0.5;
  //m_rzProfRL->Fill( midPoint.z() , midPoint.perp() , thickstepRL , 1. );
  //m_rzProfIL->Fill( midPoint.z() , midPoint.perp() , thickstepIL , 1. );

  G4ThreeVector hitPoint = aStep->GetPreStepPoint()->GetPosition();
  G4ThreeVector endPoint = aStep->GetPostStepPoint()->GetPosition();

  m_rzProfRL->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
  m_rzProfIL->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
  m_rzProfRL->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
  m_rzProfIL->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );

  std::vector<std::string> L;
  L.push_back(detName_d);
  //L.push_back(matName);
  //L.push_back(detName_plus_matName);
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
    plotstring = it;

    if(!m_rzMapRL[plotstring]){  

      TString rzname = "RZRadLen_"+plotstring;
      m_rzMapRL[plotstring] = new TProfile2D(rzname,rzname,1000,-3512.,3512.,1000,0.,1200.);
      m_rzMapRL[plotstring]->GetXaxis()->SetTitle("Z [mm]");
      m_rzMapRL[plotstring]->GetYaxis()->SetTitle("R [mm]");
      m_rzMapRL[plotstring]->GetZaxis()->SetTitle("%X0");
      std::string rznameReg = "/lengths/radLen/RZRadLen_"+plotstring;
      if (m_hSvc && m_hSvc->regHist(rznameReg,m_rzMapRL[plotstring]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << rznameReg << " failed" );
      }
      TString xyname = "XYRadLen_"+plotstring;
      m_xyMapRL[plotstring] = new TProfile2D(xyname,xyname,1000,-1200.,1200.,1000,-1200.,1200.);
      m_xyMapRL[plotstring]->GetXaxis()->SetTitle("X [mm]");
      m_xyMapRL[plotstring]->GetYaxis()->SetTitle("Y [mm]");
      m_xyMapRL[plotstring]->GetZaxis()->SetTitle("%X0");
      std::string xynameReg = "/lengths/radLen/XYRadLen_"+plotstring;
      if (m_hSvc && m_hSvc->regHist(xynameReg,m_xyMapRL[plotstring]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << xynameReg << " failed" );
      }

    }

    m_rzMapRL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepRL , 1. );
    m_rzMapRL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepRL , 1. );
    m_xyMapRL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepRL , 1. );
    m_xyMapRL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepRL , 1. );

  }  

  for (auto it : L) {
    plotstring = it;

    if(!m_rzMapIL[plotstring]){  

      TString rzname = "RZIntLen_"+plotstring;
      m_rzMapIL[plotstring] = new TProfile2D(rzname,rzname,1000,-3512.,3512.,1000,0.,1200.);
      m_rzMapIL[plotstring]->GetXaxis()->SetTitle("Z [mm]");
      m_rzMapIL[plotstring]->GetYaxis()->SetTitle("R [mm]");
      m_rzMapIL[plotstring]->GetZaxis()->SetTitle("#lambda");
      std::string rznameReg = "/lengths/intLen/RZIntLen_"+plotstring;
      if (m_hSvc && m_hSvc->regHist(rznameReg,m_rzMapIL[plotstring]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << rznameReg << " failed" );
      }
      TString xyname = "XYIntLen_"+plotstring;
      m_xyMapIL[plotstring] = new TProfile2D(xyname,xyname,1000,-1200.,1200.,1000,-1200.,1200.);
      m_xyMapIL[plotstring]->GetXaxis()->SetTitle("X [mm]");
      m_xyMapIL[plotstring]->GetYaxis()->SetTitle("Y [mm]");
      m_xyMapIL[plotstring]->GetZaxis()->SetTitle("#lambda");
      std::string xynameReg = "/lengths/intLen/XYIntLen_"+plotstring;
      if (m_hSvc && m_hSvc->regHist(xynameReg,m_xyMapIL[plotstring]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << xynameReg << " failed" );
      }

    }

    m_rzMapIL[plotstring]->Fill( hitPoint.z() , hitPoint.perp() , thickstepIL , 1. );
    m_rzMapIL[plotstring]->Fill( endPoint.z() , endPoint.perp() , thickstepIL , 1. );
    m_xyMapIL[plotstring]->Fill( hitPoint.x() , hitPoint.y() , thickstepIL , 1. );
    m_xyMapIL[plotstring]->Fill( endPoint.x() , endPoint.y() , thickstepIL , 1. );

  }  

  const G4ElementVector* eVec = mat->GetElementVector();
  for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {
    std::string elementName = "E_" + (*eVec)[i]->GetName();
    double el_thickstep = stepl * (mat->GetVecNbOfAtomsPerVolume())[i] * (*eVec)[i]->GetfRadTsai() * 100.0;

    if(!m_rzMapRL[elementName]){  

      TString rzname = "RZRadLen_"+elementName;
      m_rzMapRL[elementName] = new TProfile2D(rzname,rzname,1000,-3512.,3512.,1000,0.,1200.);
      m_rzMapRL[elementName]->GetXaxis()->SetTitle("Z [mm]");
      m_rzMapRL[elementName]->GetYaxis()->SetTitle("R [mm]");
      m_rzMapRL[elementName]->GetZaxis()->SetTitle("%X0");
      std::string rznameReg = "/lengths/radLen/RZRadLen_"+elementName;
      if (m_hSvc && m_hSvc->regHist(rznameReg,m_rzMapRL[elementName]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << rznameReg << " failed" );
      }

      TString xyname = "XYRadLen_"+elementName;
      m_xyMapRL[elementName] = new TProfile2D(xyname,xyname,1000,-1200.,1200.,1000,-1200.,1200.);
      m_xyMapRL[elementName]->GetXaxis()->SetTitle("X [mm]");
      m_xyMapRL[elementName]->GetYaxis()->SetTitle("Y [mm]");
      m_xyMapRL[elementName]->GetZaxis()->SetTitle("%X0");
      std::string xynameReg = "/lengths/radLen/XYRadLen_"+elementName;
      if (m_hSvc && m_hSvc->regHist(xynameReg,m_xyMapRL[elementName]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << xynameReg << " failed" );
      }

    }

    m_rzMapRL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
    m_rzMapRL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
    m_xyMapRL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
    m_xyMapRL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );

  }

  for (size_t i=0 ; i < mat->GetNumberOfElements() ; ++i) {

    std::string elementName = "E_" + (*eVec)[i]->GetName();
    G4double lambda0 = 35*g/cm2;
    //G4Pow* m_g4pow = G4Pow::GetInstance();
    double el_thickstep = stepl * amu/lambda0 * (mat->GetVecNbOfAtomsPerVolume())[i] * m_g4pow->Z23( G4int( (*eVec)[i]->GetN() + 0.5 ) );

    if(!m_rzMapIL[elementName]){  

      TString rzname = "RZIntLen_"+elementName;
      m_rzMapIL[elementName] = new TProfile2D(rzname,rzname,1000,-3512.,3512.,1000,0.,1200.);
      m_rzMapIL[elementName]->GetXaxis()->SetTitle("Z [mm]");
      m_rzMapIL[elementName]->GetYaxis()->SetTitle("R [mm]");
      m_rzMapIL[elementName]->GetZaxis()->SetTitle("#lambda");
      std::string rznameReg = "/lengths/intLen/RZIntLen_"+elementName;
      if (m_hSvc && m_hSvc->regHist(rznameReg,m_rzMapIL[elementName]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << rznameReg << " failed" );
      }

      TString xyname = "XYIntLen_"+elementName;
      m_xyMapIL[elementName] = new TProfile2D(xyname,xyname,1000,-1200.,1200.,1000,-1200.,1200.);
      m_xyMapIL[elementName]->GetXaxis()->SetTitle("X [mm]");
      m_xyMapIL[elementName]->GetYaxis()->SetTitle("Y [mm]");
      m_xyMapIL[elementName]->GetZaxis()->SetTitle("#lambda");
      std::string xynameReg = "/lengths/intLen/XYIntLen_"+elementName;
      if (m_hSvc && m_hSvc->regHist(xynameReg,m_xyMapIL[elementName]).isFailure()){
	ATH_MSG_FATAL( "Registration of histogram " << xynameReg << " failed" );
      }

    }

    m_rzMapIL[elementName]->Fill( hitPoint.z() , hitPoint.perp() , el_thickstep , 1. );
    m_rzMapIL[elementName]->Fill( endPoint.z() , endPoint.perp() , el_thickstep , 1. );
    m_xyMapIL[elementName]->Fill( hitPoint.x() , hitPoint.y() , el_thickstep , 1. );
    m_xyMapIL[elementName]->Fill( endPoint.x() , endPoint.y() , el_thickstep , 1. );

  }

}

StatusCode LengthIntegrator::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IUserAction::interfaceID().versionMatch(riid) ) {
    *ppvInterface = dynamic_cast<IUserAction*>(this);
    addRef();
  } else {
    // Interface is not directly available : try out a base class
    return UserActionBase::queryInterface(riid, ppvInterface);
  }
  return StatusCode::SUCCESS;
}

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

StatusCode LengthIntegrator::finalize(){

  // Adding zeros to TProfile bins, so that each bin contains the same number of entries,
  // so that a THStack of all the material TProfile plots (or all the element plots) will equal the Total_X0 TProfile plot
  // It's because each plot (for each material, say) is only filled if a Geantion hits it, not if it's not hit in an event 

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

  // To-do: Need to check if this can be used as-is for the AthenaMP version below (or will overloading the function break the histogram merging at the end of the job?
  // Edit: I've added the above code into the AthenaMP endOfEvent function. This is a bit slower (per-job) than if it was in a finalize function, but for now at least it's safer.

  return StatusCode::SUCCESS;

}
