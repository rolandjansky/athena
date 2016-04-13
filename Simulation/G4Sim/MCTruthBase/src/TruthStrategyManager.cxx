/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruthBase/TruthStrategyManager.h"
#include "MCTruthBase/TruthStrategy.h"

#include "G4AtlasInterfaces/IUserAction.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"
#include "SimHelpers/SecondaryTracksHelper.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "HepMC/GenEvent.h"


TruthStrategyManager::TruthStrategyManager()
  : isEmpty(true),
    secondarySavingLevel(0),
    nSecondaries(0),
    m_msg("TruthStrategyManager")
{}

TruthStrategyManager* TruthStrategyManager::GetStrategyManager()
{
  static TruthStrategyManager theMgr;
  return &theMgr;
}

void TruthStrategyManager::RegisterStrategy(TruthStrategy* strategy)
{
  theStrategies[strategy->StrategyName()] = strategy;
  isEmpty = false;
  ATH_MSG_INFO("MCTruth::TruthStrategyManager: registered strategy " <<
               strategy->StrategyName());
}

TruthStrategy* TruthStrategyManager::GetStrategy(const std::string& name)
{
  auto itr = theStrategies.find(name);
  if (itr != theStrategies.end()) {
    return itr->second;
  }
  ATH_MSG_WARNING("Strategy " << name << " not found: returning null");
  return nullptr;
}

void TruthStrategyManager::ListStrategies()
{
  ATH_MSG_INFO("List of all defined strategies (an X means active)");
  for (const auto& strategyPair : theStrategies) {
    if (strategyPair.second->IsActivated())
      ATH_MSG_INFO("---> " << strategyPair.first << "\t\t X");
    else ATH_MSG_INFO("---> " << strategyPair.first);
  }
}

bool TruthStrategyManager::AnalyzeVertex(const G4Step* aStep)
{
  for (const auto& strategyPair : theStrategies) {
    TruthStrategy* currentStrategy = strategyPair.second;
    if (currentStrategy->IsActivated() &&
        currentStrategy->IsApplicable(aStep) &&
        currentStrategy->AnalyzeVertex(aStep)) return true;
  }
  return false;
}

EventInformation* TruthStrategyManager::GetEventInformation() const
{
  return static_cast<EventInformation*>
    (G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetUserInformation());
}

std::vector<G4Track*> TruthStrategyManager::GetSecondaries()
{
  static SecondaryTracksHelper helper;
  return helper.GetSecondaries(nSecondaries);
}

void TruthStrategyManager::SetTruthParameter(const std::string& n, double val)
{
  truthParams[n] = val;
}

double TruthStrategyManager::GetTruthParameter(const std::string& n)
{
  if (truthParams.find(n) != truthParams.end()) {
    return truthParams[n];
  }
  ATH_MSG_WARNING("TruthStrategyManager: parameter " << n <<
                  " not found in the available set");
  return 0;
}
void TruthStrategyManager::PrintParameterList()
{
  ATH_MSG_INFO("List of all MCTruth configuration parameters");
  for (const auto& paramPair : truthParams)
    ATH_MSG_INFO("---> " << std::setw(30) << paramPair.first <<
                 "\t\t value= " << paramPair.second);
}

HepMC::GenVertex* TruthStrategyManager::StepPoint2Vertex(G4StepPoint* aPoint) const
{
  G4ThreeVector pos = aPoint->GetPosition();
  double time = aPoint->GetGlobalTime();
  CLHEP::HepLorentzVector mom(pos.x(), pos.y(), pos.z(), time*CLHEP::c_light);
  HepMC::GenVertex* vert = new HepMC::GenVertex(mom);
  return vert;
}
HepMC::GenParticle* TruthStrategyManager::Track2Particle(G4Track* aTrack) const
{
  G4ThreeVector mom = aTrack->GetMomentum();
  double ener = aTrack->GetTotalEnergy();
  CLHEP::HepLorentzVector emom(mom.x(), mom.y(), mom.z(), ener);
  int pdgcode = aTrack->GetDefinition()->GetPDGEncoding();
  HepMC::GenParticle* part = new HepMC::GenParticle(emom, pdgcode, 1);
  return part;
}

void TruthStrategyManager::
SaveSecondaryVertex(G4Track* primaryTrack, G4StepPoint* stepPoint,
                    const std::vector<G4Track*>& secondaries) const
{
  // here comes the real fun - the vertex has passed all checks
  // we try and save it in the MC truth

  // std::cout<<"---> this is TruthStrategyManager::SaveSecondaryVertex <----"<<std::endl;
  // if (primaryTrack==0) std::cout<<"null pointer to primary track"<<std::endl;
  // else
  // {
  //   TrackHelper tempHelper(primaryTrack);
  //   if (tempHelper.IsPrimary())
  //     std::cout<<" Primary track"<<std::endl;
  //   else if (tempHelper.IsRegisteredSecondary())
  //     std::cout<<" Registered Secondary"<<std::endl;
  //   else if (tempHelper.IsRegeneratedPrimary())
  //     std::cout<<" Regenerated Primary"<<std::endl;
  //   else if (tempHelper.IsSecondary())
  //     std::cout<<" Secondary "<<std::endl;
  // }

  // the vertex first
  HepMC::GenVertex* vtx = StepPoint2Vertex(stepPoint);
  auto eventInformation = GetEventInformation();
  int vbcode = eventInformation->SecondaryVertexBarCode();
  vtx->suggest_barcode(vbcode);
  vtx->set_id(stepPoint->GetProcessDefinedStep()->GetProcessSubType()+1000);
  //std::cout << " created new secondary vertex "<<std::endl;
  //std::cout << *vtx <<std::endl;
  //std::cout << "proposed barcode "<<vbcode<<std::endl;

  // the old primary
  HepMC::GenParticle* pin = eventInformation->GetCurrentlyTraced();
  vtx->add_particle_in(pin);

  //std::cout<<" particle producing the vertex "<<std::endl;
  //std::cout<<*pin <<std::endl;
  //std::cout<<"barcode "<<pin->barcode()<<std::endl;

  // let's take care of the primary. if the pointer is null there is
  // nothing we want to do with it

  if (primaryTrack)
    {
      TrackHelper tHelper(primaryTrack);
      TrackInformation* tInfo = tHelper.GetTrackInformation();
      int regenerationNr = tInfo->GetRegenerationNr();
      regenerationNr++;
      tInfo->SetRegenerationNr(regenerationNr);
      if (tHelper.IsPrimary()) tInfo->SetClassification(RegeneratedPrimary);
      HepMC::GenParticle* p1 = Track2Particle(primaryTrack);
      eventInformation->SetCurrentlyTraced(p1);
      p1->suggest_barcode(regenerationNr*1000000+ tHelper.GetBarcode());
      // eventually we add p1 in the list of the outgoing particles
      vtx->add_particle_out(p1);

      //std::cout<<" primary regenerated: gen. nr. "<<regenerationNr<<std::endl;
      //std::cout<<" regenerated particle "<<std::endl;
      //std::cout<<*p1<<std::endl;
      //std::cout<<" proposed barcode "<<regenerationNr*100000+ tHelper.GetBarcode()<<std::endl;
    }

  // now all the secondary tracks...

  assert (secondaries.size()>0);
  for (auto secondary : secondaries) {
    HepMC::GenParticle* p2 = Track2Particle(secondary);
    int pbcode = eventInformation->SecondaryParticleBarCode();
    p2->suggest_barcode(pbcode);

    TrackInformation* ti = new TrackInformation(p2);
    ti->SetRegenerationNr(0);
    ti->SetClassification(RegisteredSecondary);

    // see if TrackInformation already exists
    VTrackInformation* trackInfo =
      dynamic_cast<VTrackInformation*>( secondary->GetUserInformation() );
    if (trackInfo) {
      const ISF::ISFParticle* parent = trackInfo->GetBaseISFParticle();
      ti->SetBaseISFParticle(parent);
    }
    secondary->SetUserInformation(ti);
    vtx->add_particle_out(p2);

    //std::cout<<" secondary track "<<std::endl;
    //std::cout<<*p2<<std::endl;
    //std::cout<<" proposed barcode "<<pbcode<<std::endl;
  }

  // we are done...

  HepMC::GenEvent* evnt = eventInformation->GetHepMCEvent();
  evnt->add_vertex(vtx);

  //std::cout<<"----> end of new vertex procedure <----"<<std::endl;
}
