/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TruthHepMCEventConverter.h"
#include "G4Event.hh"

#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4LorentzVector.hh"
#include "G4Geantino.hh"
#include "G4ChargedGeantino.hh"
#include "G4EventManager.hh"
#include "G4ParticleTable.hh"

#include "FadsKinematics/VertexManipulator.h"
#include "FadsKinematics/ParticleManipulator.h"
#include "FadsKinematics/GeneratorCenter.h"

#include "MCTruth/EventInformation.h"
#include "MCTruth/TruthEvent.h"
#include "MCTruth/PrimaryParticleInformation.h"

#include "SimHelpers/ServiceAccessor.h"

#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "StoreGate/StoreGateSvc.h"
#include "SGTools/BuiltinsClids.h"

#include <map>

typedef HepMC::GenEvent Hevent;
typedef HepMC::GenVertex Hvertex;
typedef HepMC::GenParticle Hparticle;
typedef HepMC::GenEvent::vertex_const_iterator Hv_iterator;
typedef HepMC::GenVertex::particle_iterator Hp_iterator;
// A note on unit conventions 26/10/2012
//
// HepMC objects store spatial coordinates in millimetres and time
// coordinates are stored as c_light*t, also recorded in mm.
//
// G4PrimaryVertex objects store spatial coordinates in millimetres
// and time coordinates are stored in ns.
//
// Therefore it is necessary to divide the time component by
// CLHEP::c_light when going from HepMC to Geant4 objects and multiply
// by CLHEP::c_light when going from Geant4 to HepMC objects.
//
// TODO: In the future we should include full conversion of units from
// HepMC to G4 (CLHEP) i.e. G4_pos = HepMC_pos / HepMC::mm * CLHEP::mm
// and G4_time = HepMC_time / HepMC::mm * CLHEP::mm / CLHEP::c_light.
// Need to determine how best to get the HepMC::mm factor. Similary
// for energies and momenta.


// Why is this a class???
class IsFinalState {
// borrowed from one of the HepMC examples
// this predicate returns true if the input has no decay vertex
public:
    bool operator()( const HepMC::GenParticle* p ) {
        if ( !p->end_vertex() && p->status()==1 ) return 1;
        return 0;
    }
};

TruthHepMCEventConverter::TruthHepMCEventConverter()
{
  m_printLev=0;
  m_modded = new std::vector<int>(10);
}

TruthHepMCEventConverter::~TruthHepMCEventConverter()
{
  if (m_modded) delete m_modded;
}

bool TruthHepMCEventConverter::ValidHepMCVertex(HepMC::GenVertex *testVertex) const
{
  bool vertexOK(true);
  double coordinate(testVertex->position().x());
  vertexOK&=!(std::isnan(coordinate) || std::isinf(coordinate));
  coordinate = testVertex->position().y();
  vertexOK&=!(std::isnan(coordinate) || std::isinf(coordinate));
  coordinate = testVertex->position().z();
  vertexOK&=!(std::isnan(coordinate) || std::isinf(coordinate));
  coordinate = testVertex->position().t();
  vertexOK&=!(std::isnan(coordinate) || std::isinf(coordinate));
  return vertexOK;
}

bool TruthHepMCEventConverter::SeparateVertices(HepMC::GenVertex *v1,
                                                HepMC::GenVertex *v2) const
{
  const double distMin=0.01*CLHEP::mm;
  G4LorentzVector vv1(v1->position().x(), v1->position().y(),
                      v1->position().z(), v1->position().t());
  G4LorentzVector vv2(v2->position().x(), v2->position().y(),
                      v2->position().z(), v2->position().t());
  return ((sqrt(pow(vv1.x()-vv2.x(),2)+pow(vv1.y()-vv2.y(),2)+
           pow(vv1.z()-vv2.z(),2))>distMin) || (vv1.t()!=vv2.t()));
}

G4PrimaryVertex* TruthHepMCEventConverter::ConstructG4PrimaryVertex(HepMC::GenVertex *v) const
{
  if(!ValidHepMCVertex(v))
    {
      std::cout << "TruthHepMCEventConverter FATAL ConstructG4PrimaryVertex: Received invalid GenVertex object:" << *v
                << " - Job will die now..." << std::endl;
      throw;
    }
  G4LorentzVector vv(v->position().x(),v->position().y(),v->position().z(),v->position().t());
  G4PrimaryVertex *vert= new G4PrimaryVertex (vv.x()*CLHEP::mm,
                                              vv.y()*CLHEP::mm,
                                              vv.z()*CLHEP::mm,
                                              vv.t()*CLHEP::mm/CLHEP::c_light);
  return vert;
}

void TruthHepMCEventConverter::TransformHepMCParticle(HepMC::GenParticle *hp, G4PrimaryParticle *gp) const
{
  hp->set_pdg_id(gp->GetPDGcode());  //this should not change at all, right?
  CLHEP::Hep3Vector gpv=gp->GetMomentum();
  const double pmass=gp->GetMass();
  const double pe = sqrt(gpv.mag2() + pmass*pmass);  // this does only change for boosts, etc.
  hp->set_momentum(CLHEP::HepLorentzVector(gpv.x(),gpv.y(),gpv.z(),pe));
}

void TruthHepMCEventConverter::TransformHepMCVertex(HepMC::GenVertex *hv,G4PrimaryVertex *gv) const
{
  CLHEP::HepLorentzVector hvlv(hv->position().x(),hv->position().y(),hv->position().z(),hv->position().t());
  if (hvlv.x()!=gv->GetX0()) hvlv.setX(gv->GetX0());
  if (hvlv.y()!=gv->GetY0()) hvlv.setY(gv->GetY0());
  if (hvlv.z()!=gv->GetZ0()) hvlv.setZ(gv->GetZ0());
  const double cT0(gv->GetT0()*CLHEP::c_light);
  if (hvlv.t()!=cT0) hvlv.setT(cT0);
  hv->set_position(hvlv);
}

G4PrimaryParticle* TruthHepMCEventConverter::ConstructG4PrimaryParticle(HepMC::GenParticle *hp) const
{
  // check if stable
  if(hp->status()!=1) return 0;

  int pdgcode=hp->pdg_id();
  CLHEP::HepLorentzVector lp(hp->momentum().px(),hp->momentum().py(),hp->momentum().pz(),hp->momentum().e());
  G4ThreeVector& p=lp;

  if (m_printLev>1)
  {
    std::cout << "GenParticle being converted :"<<std::endl;
    std::cout << "momentum components (in MeV): "<<p<<std::endl;
    std::cout << "Energy (in MeV): "<<lp.e()<<std::endl;
    std::cout << "Mass (in MeV): "<<lp.m()<<std::endl;
  }

  G4PrimaryParticle *part;
  if (pdgcode==999){
    G4ParticleDefinition* pdef = G4Geantino::GeantinoDefinition();
    part = new G4PrimaryParticle(pdef,p.x(),p.y(),p.z());

  } else if (pdgcode==998){
    G4ParticleDefinition* pdef = G4ChargedGeantino::ChargedGeantinoDefinition();
    part = new G4PrimaryParticle(pdef, p.x(), p.y(), p.z());
  } else {
    part = new G4PrimaryParticle(pdgcode, p.x(), p.y(), p.z());
  }
  return part;
}

HepMC::GenVertex* TruthHepMCEventConverter::ConstructHepMCVertex(G4PrimaryVertex *v) const
{
  G4ThreeVector pos=v->GetPosition();
  double t0=v->GetT0();
  G4LorentzVector lv(pos.x(),pos.y(),pos.z(),t0*CLHEP::c_light);
  HepMC::GenVertex *hv=new HepMC::GenVertex(lv);
  return hv;
}

struct CompositeVertex {
  HepMC::GenVertex* primary;
  std::vector<HepMC::GenVertex*> secondaries;
};

typedef std::vector<CompositeVertex> vertexVector;

void TruthHepMCEventConverter::HepMC2G4(const HepMC::GenEvent* evt, G4Event * anEvent, const bool IncludeParentsInG4Event=false) const
{

  m_modded->clear();
  int n_vertices=0;
  int n_particles=0;

  std::vector<G4PrimaryVertex*> vertexVector;
  std::vector<G4PrimaryParticle*> particleVector;

  if (m_printLev>3) std::cout<<" This is the Truth HepMC event converter "<<std::endl;

  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();

  FADS::vtxModifiers::const_iterator vit;
  for (vit=gc->BeginVertexManipulator();vit!=gc->EndVertexManipulator();vit++)
    if ((*vit).second->IsOn()) (*vit).second->EventInitialization();

  FADS::particleModifiers::const_iterator pit;
  for (pit=gc->BeginParticleManipulator();pit!=gc->EndParticleManipulator();pit++)
    if ((*pit).second->IsOn()) (*pit).second->EventInitialization();

  if (m_printLev>=3) std::cout<<" creating the Truth event "<<std::endl;
  HepMC::GenEvent* newEvt = new HepMC::GenEvent(*evt);

  StoreGateSvc* storeGate=0;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode st = svcLocator->service("StoreGateSvc", storeGate);
  if (st.isFailure())
     std::cout<<" storeGateSvc(); could not access StoreGateSvc"<<std::endl;
  // Grab an instance of the particle table
  G4ParticleTable *partTable = G4ParticleTable::GetParticleTable();
  // If we failed, yell
  if (partTable==0)
    std::cout << "Could not find a particle table!" << std::endl;

  // store the time in SG
  double vtxTime = 0;
  std::vector<HepMC::GenParticle*> incoming; incoming.clear();
  for (HepMC::GenEvent::particle_iterator p = newEvt->particles_begin(); p != newEvt->particles_end(); ++p){
    if ( ((*p)->pdg_id() >= 2212) && (!(*p)->production_vertex())) incoming.push_back( *p );
    if (incoming.size() > 1) break;
  }
  if (incoming.size() == 2 &&
      newEvt->vertices_size() != 0){
    vtxTime = (*(newEvt->vertices_begin()))->position().z()/CLHEP::c_light;

    // sign issue
    if (incoming[0]->momentum().z() + incoming[1]->momentum().z() < 0) vtxTime = -vtxTime;
  }
  double *p_vtxTime = new double(vtxTime);
  st = storeGate->record( p_vtxTime, "PrimarySimEventTime" );
  if (st.isFailure())
    std::cout << "TruthHepMCEventConverter: could not record primary vertex time to storeGate!"<<std::endl;

  // loop on the vertices
  // only those which have stable particles will be translated into G4PrimaryVertex
  // vertices will be displaced if their final position differs from the original

  // set signal process vertex if currently unset
  if( !newEvt->signal_process_vertex() ) {
    HepMC::GenVertex *signalVertex = *(newEvt->vertices_begin());
    newEvt->set_signal_process_vertex( signalVertex );
  }
  // Get the signal process vertex, just in case...
  HepMC::FourVector old_signal_spot = newEvt->signal_process_vertex()->position();

  //std::cout << " primary vertex being processed " << std::endl
  //          << *(*(newEvt->vertices_begin())) << std::endl;
  //std::cout << " primary vertex time set at: " << vtxInfo->GetPrimaryVertexTime()
  //          << "ns" << std::endl;

  for ( Hv_iterator v = newEvt->vertices_begin();v != newEvt->vertices_end(); ++v )
  {
    if (m_printLev>4)
      std::cout << " new vertex being processed " << std::endl << *(*v) << std::endl;

    // Check if we've already modified this vertex
    bool IsUsed=false;
    for (unsigned int i=0;i<m_modded->size() && IncludeParentsInG4Event;i++){
      if ( (*m_modded)[i] == (*v)->barcode() ){
        IsUsed=true;
        break;
      }
    }
    if (IsUsed){
      if (m_printLev>3)
        std::cout <<  "Cutting a vertex because it's been used." << std::endl;
      continue;
    }

    int nStableParticles=0;
    IsFinalState isStable;

    // create a G4PrimaryVertex - destroyed afterwards if not to be kept
    //
    G4PrimaryVertex *vert = ConstructG4PrimaryVertex((*v));
    G4ThreeVector vtxPos1 = vert->GetPosition();
    FADS::vtxModifiers::const_iterator vit;
    bool vGood=true;
    for (vit=gc->BeginVertexManipulator();vit!=gc->EndVertexManipulator();++vit)
    {
      if ((*vit).second->IsOn())
      vGood=vGood&&(*vit).second->EditVertex(vert);
      if (!vGood) break;
    }

    for (vit=gc->BeginVertexManipulator();vit!=gc->EndVertexManipulator();++vit)
    {
      if ((*vit).second->GetName()=="VertexRangeChecker" && (*vit).second->IsOn()){
        vGood=vGood&&(*vit).second->EditVertex(vert);
        break;
      }
    }

    if (!vGood)
    {
      // the vertex must be discarded
      delete vert;
    }


    if (vGood)
    {
      TransformHepMCVertex((*v),vert);

      std::map< int , HepMC::GenParticle*, std::less<int> > pMap;
      std::map< int , HepMC::GenParticle*, std::less<int> >::const_iterator iit;

      for (Hp_iterator it=(*v)->particles_begin(HepMC::children);
           it!=(*v)->particles_end(HepMC::children); ++it)
      {
        int bc=(*it)->barcode();
        pMap[bc]=(*it);
      }

      for (iit=pMap.begin();iit!=pMap.end();iit++)
      {
        // If it is stable or it is known
        bool IsKnown = partTable &&
                       partTable->FindParticle( ((*iit).second)->pdg_id() );
        if (isStable((*iit).second) || (IsKnown && IncludeParentsInG4Event))
        {
          G4PrimaryParticle *part =ConstructG4PrimaryParticle((*iit).second);
          if (0==part) continue;

          bool pGood=true;
          FADS::particleModifiers::const_iterator jt;

          for (jt=gc->BeginParticleManipulator(); jt!=gc->EndParticleManipulator();
               ++jt)
          {
            if ((*jt).second->IsOn())
            pGood=pGood&&(*jt).second->EditParticle(part);
            if (!pGood) break;
          }
          if (pGood)
          {
            if (m_printLev>3) std::cout<<" particle accepted "<<std::endl;
            TransformHepMCParticle((*iit).second, part);
            nStableParticles++;
            PrimaryParticleInformation* ppi=new PrimaryParticleInformation;

            ppi->SetParticle((*iit).second);
            ppi->SetRegenerationNr(0);
            part->SetUserInformation(ppi);
            vert->SetPrimary(part);
            particleVector.push_back(part);

            n_particles++;
            if (IncludeParentsInG4Event &&
                IsKnown &&
                ( (*iit).second )->end_vertex() ){
               ModifyVertex( &(*part), ((*iit).second)->end_vertex() );
            }
            if ( !((*iit).second)->end_vertex() && ((*iit).second)->status() != 1){
              if (m_printLev>2)
                std::cout << "Used a known(" << IsKnown << ") part with EV("
                          << ((*iit).second)->end_vertex() << ") and status code "
                          << ((*iit).second)->status() << std::endl;
            }
          }
          else {
            delete part;
          }
        }
      }
      if (nStableParticles){
        n_vertices++;
        anEvent->AddPrimaryVertex(vert);
        vertexVector.push_back(vert);
      } else {
        delete vert;
      }
    }
  }
  
  EventInformation* eventInfo = new EventInformation;
  eventInfo->SetNrOfPrimaryParticles(n_particles);
  eventInfo->SetNrOfPrimaryVertices(n_vertices);
  eventInfo->SetHepMCEvent(newEvt);
  anEvent->SetUserInformation(eventInfo);

  // Signal process vertex is a pointer.  There is some risk that the pointer points 
  //  to a vertex somewhere else in the event, rather than a unique / new vertex, in
  //  which case we will have already modified its position in the loop above.  That
  //  is the reason for hanging on to an old position and seeing if we've moved...
  // I would love it if HepMC had a minus operator defined for FourVectors... or
  //  even a get for the three vector component :(
  HepMC::FourVector why_no_minus(
    newEvt->signal_process_vertex()->position().x() - old_signal_spot.x(),
    newEvt->signal_process_vertex()->position().y() - old_signal_spot.y(),
    newEvt->signal_process_vertex()->position().z() - old_signal_spot.z(),
    newEvt->signal_process_vertex()->position().t() - old_signal_spot.t() );

  if ( why_no_minus.rho() < 0.000001 && why_no_minus.m2() < 0.000001 ){
    G4PrimaryVertex* g4pv = ConstructG4PrimaryVertex(newEvt->signal_process_vertex());
    for (vit=gc->BeginVertexManipulator();vit!=gc->EndVertexManipulator();++vit)
    {
      if ((*vit).second->GetName()!="VertexRangeChecker" && (*vit).second->IsOn()){
        (*vit).second->EditVertex(g4pv);
        break;
      }
    }
    double t0=g4pv->GetT0();
    G4LorentzVector lv(g4pv->GetPosition().x(), g4pv->GetPosition().y(),
                       g4pv->GetPosition().z(), t0*CLHEP::c_light);
    newEvt->signal_process_vertex()->set_position( lv );
  } // Manipulated the signal process vertex

  // publish the truth to SG
  if (m_printLev>=3) std::cout<<" recording the truth event to SG "<<std::endl;

  std::string key = "TruthEvent";
  McEventCollection* mcEvtColl;

  if (storeGate->contains<McEventCollection>(key))
  {
    StatusCode status = storeGate->retrieve(mcEvtColl,key);
    if (status.isSuccess())
    std::cout << "found an McEventCollection in store" << std::endl;
  }
  else
  {
    // McCollection doesn't exist. Create it and fill it with the truth
    mcEvtColl = new McEventCollection;
    mcEvtColl->push_back(newEvt);
    StatusCode status = storeGate->record( mcEvtColl, key );
    if (status.isFailure())
      std::cout << "TruthHepMCEventConverter: could not record MC evt "
                << "collection to storeGate!" << std::endl;
  }

}

// Propagating a change using a primary particle and its decay vertex
void TruthHepMCEventConverter::ModifyVertex(G4PrimaryParticle *pp, HepMC::GenVertex *v) const
{
  FADS::GeneratorCenter *gc = FADS::GeneratorCenter::GetGeneratorCenter();

  // One incoming particle
  G4LorentzVector lv(v->position().x(),
                     v->position().y(),
                     v->position().z(),
                     v->position().t());
  if ( v->particles_in_size() < 1 ){
    // Bark
    std::cout << "ERROR: No particles going into a decay vertex!" << std::endl;
    return;
  }

  G4LorentzVector lv0(
    (*(v->particles_begin(HepMC::parents)))->production_vertex()->position().x(),
    (*(v->particles_begin(HepMC::parents)))->production_vertex()->position().y(),
    (*(v->particles_begin(HepMC::parents)))->production_vertex()->position().z(),
    (*(v->particles_begin(HepMC::parents)))->production_vertex()->position().t() );
  pp->SetProperTime( (lv-lv0).mag()/CLHEP::c_light );

  // Loop over daughters
  for (HepMC::GenVertex::particle_iterator it = v->particles_begin(HepMC::children);
       it != v->particles_end(HepMC::children);
       it++)
  {
    // Either way we will add this daughter to the pp's daughters...
    G4PrimaryParticle *daughter = ConstructG4PrimaryParticle( (*it) );
    bool pGood=true;
    FADS::particleModifiers::const_iterator jt;

    for (jt=gc->BeginParticleManipulator(); jt!=gc->EndParticleManipulator();
         ++jt)
    {
      if ((*jt).second->IsOn())
      pGood = pGood&&(*jt).second->EditParticle(daughter);
      if (!pGood) break;
    }
    if (pGood){

      pp->SetDaughter( daughter );

      if ( (*it)->end_vertex() ) {
        // Send the daughter with its end vertex back to this method
        ModifyVertex( daughter, (*it)->end_vertex() );
      }
    }
  }

  // add the vertex to our list
  m_modded->push_back( v->barcode() );

}
