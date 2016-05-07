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

  for (auto& it : m_detThick) {

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


//=============================================================================
// New design below for multithreading
//=============================================================================

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
    : m_hSvc(histSvcName, "LengthIntegrator"),
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
      detName = "Generic";

    auto it = m_detThickMap.find(detName);

    // If measurement already started, add to it.
    if(it != m_detThickMap.end()) {
      it->second.first += thickstepRL;
      it->second.second += thickstepIL;
    }

    // If new detector measurement, then insert it in the map
    else {
      using std::make_pair;
      m_detThickMap.insert( make_pair(detName, make_pair(thickstepRL, thickstepIL)) );
    }

    // Midpoint of step
    G4ThreeVector midPoint = ( aStep->GetPreStepPoint()->GetPosition() +
                               aStep->GetPostStepPoint()->GetPosition() )*0.5;

    // Protect concurrent histo filling
    {
      static std::mutex m;
      std::lock_guard<std::mutex> lock(m);
      m_rzProfRL->Fill( midPoint.z(), midPoint.perp(), thickstepRL, 1. );
      m_rzProfIL->Fill( midPoint.z(), midPoint.perp(), thickstepIL, 1. );
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
      auto name = (detName+"_RL").c_str();
      profEtaRL = new TProfile(name, name, 1000, -6., 6.);
      regHist(m_hSvc, pathEtaRL, profEtaRL);
    }
    // Eta int profile
    if(!getHist(m_hSvc, pathEtaIL, profEtaIL)) {
      auto name = (detName+"_IL").c_str();
      profEtaIL = new TProfile(name, name, 1000, -6., 6.);
      regHist(m_hSvc, pathEtaIL, profEtaIL);
    }
    // Phi rad profile
    if(!getHist(m_hSvc, pathPhiRL, profPhiRL)) {
      auto name = (detName+"Phi_RL").c_str();
      profPhiRL = new TProfile(name, name, 500, -M_PI, M_PI);
      regHist(m_hSvc, pathPhiRL, profPhiRL);
    }
    // Phi int profile
    if(!getHist(m_hSvc, pathPhiIL, profPhiIL)) {
      auto name = (detName+"Phi_IL").c_str();
      profPhiIL = new TProfile(name, name, 500, -M_PI, M_PI);
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
