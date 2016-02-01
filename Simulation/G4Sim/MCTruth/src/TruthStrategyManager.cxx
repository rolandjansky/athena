/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TruthStrategy.h"
#include "MCTruth/TruthStrategyManager.h"
#include "MCTruth/EventInformation.h"
#include "MCTruth/TrackHelper.h"
#include "MCTruth/MCTruthSteppingAction.h"
#include "SimHelpers/SecondaryTracksHelper.h"
#include "HepMC/GenEvent.h"

#include "G4Step.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

TruthStrategyManager* TruthStrategyManager::thePointer=0;

TruthStrategyManager::TruthStrategyManager() : 
  isEmpty(true), 
  secondarySavingLevel(0),
  nSecondaries(0),
  theSteppingAction(NULL), 
  m_log(0)
{;}

TruthStrategyManager* TruthStrategyManager::GetStrategyManager()
{
  if (!thePointer) thePointer=new TruthStrategyManager;
  return thePointer;
}

void TruthStrategyManager::EnableRecordingEnvelopes()
{
  theSteppingAction=new MCTruthSteppingAction;
}

void TruthStrategyManager::AddRecordingEnvelope(std::string name, int lev, std::string sdName)
{
  if (theSteppingAction) 
    theSteppingAction->AddRecordingEnvelope(name,lev,sdName);
  else
    log()<<MSG::WARNING<<" AddRecordingEnvelope: you must enable recording"
         <<" envelopes before adding one!"<<endreq;
}

void TruthStrategyManager::ListRecordingEnvelopes()
{
  if (theSteppingAction)
    theSteppingAction->ListRecordingEnvelopes();
  else
    log()<<MSG::WARNING<<" Envelope recording not enabled!"<<endreq;
}

void TruthStrategyManager::RegisterStrategy(TruthStrategy *strategy)
{
  theStrategies[strategy->StrategyName()]=strategy;
  isEmpty=false;
  log()<<MSG::INFO<<"MCTruth::TruthStrategyManager: registered strategy "<<
         strategy->StrategyName()<<std::endl;
}

TruthStrategy* TruthStrategyManager::GetStrategy(std::string name)
{
  if (theStrategies.find(name) != theStrategies.end()){
    return theStrategies[name];
  }
  log()<<MSG::WARNING<<" strategy "<<name<<" not found: returning 0"<<endreq;
  return 0;
}

void TruthStrategyManager::ListStrategies()
{
  strategyMap::const_iterator it;
  log()<<MSG::INFO<<" List of all defined strategies (an X means active) "<<endreq;
  for (it=theStrategies.begin();it!=theStrategies.end();it++)
  {
    if ((*it).second->IsActivated()) log()<<MSG::INFO<<"---> "<<(*it).first<<"\t\t X"<<endreq;
    else log()<<MSG::INFO<<"---> "<<(*it).first<<endreq;
  }
}

bool TruthStrategyManager::AnalyzeVertex(const G4Step* aStep)
{
  strategyMap::const_iterator it;
  for (it=theStrategies.begin();it!=theStrategies.end();it++)
  {
    TruthStrategy* currentStrategy=(*it).second;
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

void TruthStrategyManager::SetTruthParameter(const std::string n, double val)
{
  truthParams[n]=val;
}

double TruthStrategyManager::GetTruthParameter(const std::string n) 
{
  if (truthParams.find(n) != truthParams.end()) {
    return truthParams[n];
  }
  log()<<MSG::WARNING<<" TruthStrategyManager: parameter "<<n<<
         " not found in the available set "<<std::endl;
  return 0;
}
void TruthStrategyManager::PrintParameterList()
{
  log()<<MSG::INFO<<" List of all MCTruth configuration parameters "<<endreq;
  std::map< std::string, double, std::less<std::string> >::const_iterator it;
  for (it=truthParams.begin();it!=truthParams.end();it++)
    log()<<MSG::INFO<<"---> "<<std::setw(30)<<(*it).first<<"\t\t value= "<<(*it).second<<endreq;
}

HepMC::GenVertex* TruthStrategyManager::StepPoint2Vertex(G4StepPoint* aPoint) const 
{
  G4ThreeVector pos=aPoint->GetPosition();
  double time =aPoint->GetGlobalTime();
  CLHEP::HepLorentzVector mom(pos.x(),pos.y(),pos.z(),time*CLHEP::c_light);
  HepMC::GenVertex *vert=new HepMC::GenVertex(mom);
  return vert;
}
HepMC::GenParticle* TruthStrategyManager::Track2Particle(G4Track* aTrack) const
{
  G4ThreeVector mom=aTrack->GetMomentum();
  double ener=aTrack->GetTotalEnergy();
  CLHEP::HepLorentzVector emom(mom.x(),mom.y(),mom.z(),ener);
  int pdgcode=aTrack->GetDefinition()->GetPDGEncoding();
  HepMC::GenParticle* part=new HepMC::GenParticle(emom,pdgcode,1);
  return part;
}
void TruthStrategyManager::SaveSecondaryVertex(G4Track* primaryTrack, 
			G4StepPoint*stepPoint, 
			std::vector<G4Track*> secondaries) const
{
	// here comes the real fun - the vertex has passed all checks
	// we try and save it in the MC truth
	
// 	std::cout<<"---> this is TruthStrategyManager::SaveSecondaryVertex <----"<<std::endl;
// 	if (primaryTrack==0) std::cout<<"null pointer to primary track"<<std::endl;
// 	else 
// 	{
// 		TrackHelper tempHelper(primaryTrack);
// 		if (tempHelper.IsPrimary()) 
// 			std::cout<<" Primary track"<<std::endl;
// 		else if (tempHelper.IsRegisteredSecondary())
// 			std::cout<<" Registered Secondary"<<std::endl;
// 		else if (tempHelper.IsRegeneratedPrimary())
// 			std::cout<<" Regenerated Primary"<<std::endl;
// 		else if (tempHelper.IsSecondary())
// 			std::cout<<" Secondary "<<std::endl;
// 	}
	// the vertex first
	HepMC::GenVertex* vtx=StepPoint2Vertex(stepPoint);
	int vbcode=GetEventInformation()->SecondaryVertexBarCode();
	vtx->suggest_barcode(vbcode);
	vtx->set_id(stepPoint->GetProcessDefinedStep()->GetProcessSubType()+1000);
//	std::cout << " created new secondary vertex "<<std::endl;
//	std::cout << *vtx <<std::endl;
//	std::cout << "proposed barcode "<<vbcode<<std::endl;
	
	// the old primary
	HepMC::GenParticle* pin=GetEventInformation()->GetCurrentlyTraced();
	vtx->add_particle_in(pin);
	
//	std::cout<<" particle producing the vertex "<<std::endl;
//	std::cout<<*pin <<std::endl;
//	std::cout<<"barcode "<<pin->barcode()<<std::endl;
	
	// let's take care of the primary. if the pointer is null there is
	// nothing we want to do with it
	
	if (primaryTrack)
	{
		TrackHelper tHelper(primaryTrack);
		TrackInformation* tInfo=tHelper.GetTrackInformation();
		int regenerationNr=tInfo->GetRegenerationNr();
		regenerationNr++;
		tInfo->SetRegenerationNr(regenerationNr);
		if (tHelper.IsPrimary()) tInfo->SetClassification(RegeneratedPrimary);
		HepMC::GenParticle* p1=Track2Particle(primaryTrack);
		GetEventInformation()->SetCurrentlyTraced(p1);
		p1->suggest_barcode(regenerationNr*1000000+ tHelper.GetBarcode());
		// eventually we add p1 in the list of the outgoing particles
		vtx->add_particle_out(p1);
		
//		std::cout<<" primary regenerated: gen. nr. "<<regenerationNr<<std::endl;
//		std::cout<<" regenerated particle "<<std::endl;
//		std::cout<<*p1<<std::endl;
//		std::cout<<" proposed barcode "<<regenerationNr*100000+ tHelper.GetBarcode()<<std::endl;
	}
	
	// now all the secondary tracks...
	
	assert (secondaries.size()>0);
	for (unsigned int itrack=0;itrack<secondaries.size();itrack++)
	{
		HepMC::GenParticle* p2=Track2Particle(secondaries[itrack]);
		int pbcode=GetEventInformation()->SecondaryParticleBarCode();
		p2->suggest_barcode(pbcode);
		
		TrackInformation *ti=new TrackInformation(p2);
		ti->SetRegenerationNr(0);
		ti->SetClassification(RegisteredSecondary);

		// see if TrackInformation already exists
		VTrackInformation* trackInfo = dynamic_cast<VTrackInformation*>(secondaries[itrack]->GetUserInformation());		  
		if (trackInfo) {
		  const ISF::ISFParticle* parent=trackInfo->GetBaseISFParticle();
		  ti->SetBaseISFParticle(parent);
		}
		secondaries[itrack]->SetUserInformation(ti);
		vtx->add_particle_out(p2);
		
//		std::cout<<" secondary track "<<std::endl;
//		std::cout<<*p2<<std::endl;
//		std::cout<<" proposed barcode "<<pbcode<<std::endl; 
	}
	
	// we are done...
	
	HepMC::GenEvent* evnt=GetEventInformation()->GetHepMCEvent();
	evnt->add_vertex(vtx);
	
//	std::cout<<"----> end of new vertex procedure <----"<<std::endl;
}

MsgStream TruthStrategyManager::log()
{
  if (m_log) return *m_log;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IMessageSvc* p_msgSvc = 0;
  if (svcLocator->service("MessageSvc", p_msgSvc).isFailure() || !p_msgSvc)
    std::cout << "FadsSensitiveDetector: Trouble getting the message service.  Should never happen.  Will crash now." << std::endl;
  m_log = new MsgStream(p_msgSvc,"TruthStrategyManager");
  return *m_log;
}

