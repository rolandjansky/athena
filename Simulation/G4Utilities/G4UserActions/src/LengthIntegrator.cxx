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


LengthIntegrator::LengthIntegrator(const std::string& type, const std::string& name, const IInterface* parent): UserActionBase(type,name,parent),m_etaPrimary(0),m_phiPrimary(0),m_rzProfRL(0),m_rzProfIL(0),m_hSvc("THistSvc",name) {

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
  // Phi profile
  TProfile *profPhiRL=new TProfile((detname+"Phi_RL").c_str(),(detname+"Phi_RL").c_str(),500,-M_PI,M_PI);
  TProfile *profPhiIL=new TProfile((detname+"Phi_IL").c_str(),(detname+"Phi_IL").c_str(),500,-M_PI,M_PI);
  
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
    detName="Generic";

  auto it=m_detThick.find(detName);
  if(it!=m_detThick.end()){
    (*it).second.first+=thickstepRL;
    (*it).second.second+=thickstepIL;
  } else {

    m_detThick.insert(std::map<std::string,std::pair<double,double>,std::less<std::string> >::value_type(
													 detName,
													 std::pair<double,double>(
																  thickstepRL,
																  thickstepIL)
													 )
		      );

  }
  
  G4ThreeVector midPoint = (aStep->GetPreStepPoint()->GetPosition()+aStep->GetPostStepPoint()->GetPosition())*0.5;

  m_rzProfRL->Fill( midPoint.z() , midPoint.perp() , thickstepRL , 1. );
  m_rzProfIL->Fill( midPoint.z() , midPoint.perp() , thickstepIL , 1. );

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
