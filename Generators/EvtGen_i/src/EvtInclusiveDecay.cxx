//*****************************************************************************
//
// Generators/EvtGen_i/EvtInclusiveDecay.h
//
// $Id: EvtInclusiveDecay.cxx,v 1.6 2007-03-01 23:23:44 binet Exp $
//
// EvtInclusiveDecay is a TopAlg that takes HepMC events from StoreGate and
// generates particle decays using EvtGen. Depending on job options either all or
// only a subset of the particles which have decays defined in the EvtGen
// decay files will be handed to EvtGen. Both undecayed particles and particles
// with an existing decay tree can be handled (in the latter case,
// EvtInclusiveDecay will remove the existing decay tree).
//
// Written in February 2006 by Juerg Beringer, based in part on the existing
// EvtDecay module.
//
//*****************************************************************************

#include "EvtGen_i/EvtInclusiveDecay.h"

#include "EvtGenBase/EvtAbsRadCorr.hh"
#include "EvtGenBase/EvtDecayBase.hh"
#include "EvtGen_i/EvtGenExternal/EvtExternalGenList.hh"

#include "EvtGenBase/EvtVector4R.hh"
#include "EvtGenBase/EvtParticle.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtRandomEngine.hh"
#include "EvtGenBase/EvtDecayTable.hh"

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
#include "AtlasHepMC/GenParticle.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IPartPropSvc.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/DataHandle.h"
#include "GeneratorObjects/McEventCollection.h"
#include "HepPDT/ParticleData.hh"
#include "HepPID/ParticleName.hh"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <stdlib.h>
#include <sstream>
#include <map>


EvtInclusiveDecay::EvtInclusiveDecay(const std::string& name, ISvcLocator* pSvcLocator):GenBase( name, pSvcLocator ) {
  m_evtAtRndmGen = 0;
  m_myEvtGen = 0;

  // Basic EvtGen configuration: decay and particle definition files, random number stream
  declareProperty("pdtFile", m_pdtFile = "inclusive.pdt");
  declareProperty("decayFile", m_decayFile = "2014inclusive.dec");
  declareProperty("userDecayFile", m_userDecayFile = "");
  declareProperty("randomStreamName", m_randomStreamName = "EVTGEN");
  declareProperty("inputKeyName", m_inputKeyName = "GEN_EVENT");
  declareProperty("outputKeyName",m_outputKeyName = "GEN_EVENT_EVTGEN");
  declareProperty("readExisting",m_readExisting=false);

  // Selection of particles to be decayed
  declareProperty("prohibitFinalStateDecay", m_prohibitFinalStateDecay=false);
  declareProperty("prohibitReDecay", m_prohibitReDecay=false);
  declareProperty("prohibitUnDecay", m_prohibitUnDecay=true);
  declareProperty("prohibitRemoveSelfDecay", m_prohibitRemoveSelfDecay=false);
  declareProperty("blackList",m_blackList);
  declareProperty("allowAllKnownDecays", m_allowAllKnownDecays=true);
  declareProperty("allowDefaultBDecays", m_allowDefaultBDecays=true);
  declareProperty("whiteList",m_whiteList);
  
  declareProperty("DecayedParticleStatus", m_decayedStatus=2);
  
  // Level of output
  declareProperty("printHepMCBeforeEvtGen", m_printHepMCBeforeEvtGen=false);
  declareProperty("printHepMCAfterEvtGen", m_printHepMCAfterEvtGen=false);
  declareProperty("printHepMCHighlighted", m_printHepMCHighlighted=true);
  declareProperty("printHepMCHighLightTopLevelDecays", m_printHepMCHighLightTopLevelDecays=true);

  // Optional checks
  declareProperty("checkDecayTree", m_checkDecayTree=false);
  declareProperty("checkDecayChannels", m_checkDecayChannels=false);
  
  // Repeated decays
  declareProperty("maxNRepeatedDecays", m_maxNRepeatedDecays=1);
  
  // User selection
  declareProperty("applyUserSelection", m_applyUserSelection=false);
  declareProperty("userSelRequireOppositeSignedMu", m_userSelRequireOppositeSignedMu=true);
  declareProperty("userSelMu1MinPt", m_userSelMu1MinPt=0.);
  declareProperty("userSelMu2MinPt", m_userSelMu2MinPt=0.);
  declareProperty("userSelMu1MaxEta", m_userSelMu1MaxEta=102.5);
  declareProperty("userSelMu2MaxEta", m_userSelMu2MaxEta=102.5);
  declareProperty("userSelMinDimuMass", m_userSelMinDimuMass=0.);
  declareProperty("userSelMaxDimuMass", m_userSelMaxDimuMass=-1.); // set to negative to not apply cut
  declareProperty("isfHerwig", m_isfHerwig=false); 

  m_atRndmGenSvc = 0;
  m_mcEvtColl = 0;

  // We have decided to blacklist Tau decays because we are not sure whether the polarization
  // would be properly passed to EvtGen
  m_blackList.push_back(15);
}



EvtInclusiveDecay::~EvtInclusiveDecay() {
  delete m_myEvtGen;
  delete m_evtAtRndmGen;
}



StatusCode EvtInclusiveDecay::initialize() {

  msg(MSG::INFO) << "EvtInclusiveDecay initialize" << endmsg;
  msg(MSG::INFO) << "Particle properties definition file = " << m_pdtFile << endmsg;
  msg(MSG::INFO) << "Main decay file                     = " << m_decayFile << endmsg;
  msg(MSG::INFO) << "User decay file                     = " << m_userDecayFile << endmsg;
  msg(MSG::INFO) << "Max number of repeated decays       = " << m_maxNRepeatedDecays << endmsg;
  msg(MSG::INFO) << "EvtInclusiveDecay selection parameters:" << endmsg;
  msg(MSG::INFO) << "* prohibitFinalStateDecay = " << m_prohibitFinalStateDecay << endmsg;
  msg(MSG::INFO) << "* prohibitReDecay         = " << m_prohibitReDecay << endmsg;
  msg(MSG::INFO) << "* prohibitUnDecay         = " << m_prohibitUnDecay << endmsg;
  msg(MSG::INFO) << "* prohibitRemoveSelfDecay = " << m_prohibitRemoveSelfDecay << endmsg;
  msg(MSG::INFO) << "* allowAllKnownDecays     = " << m_allowAllKnownDecays << endmsg;
  msg(MSG::INFO) << "* allowDefaultBDecays     = " << m_allowDefaultBDecays << endmsg;
  msg(MSG::INFO) << "Decayed particle status   = " << m_decayedStatus << endmsg;
  msg(MSG::INFO) << "User selection parameters:" << endmsg;
  msg(MSG::INFO) << "* applyUserSelection             = " << m_applyUserSelection << endmsg;
  msg(MSG::INFO) << "* userSelRequireOppositeSignedMu = " << m_userSelRequireOppositeSignedMu << endmsg;
  msg(MSG::INFO) << "* userSelMu1MinPt                = " << m_userSelMu1MinPt << endmsg;
  msg(MSG::INFO) << "* userSelMu2MinPt                = " << m_userSelMu2MinPt << endmsg;
  msg(MSG::INFO) << "* userSelMu1MaxEta               = " << m_userSelMu1MaxEta << endmsg;
  msg(MSG::INFO) << "* userSelMu2MaxEta               = " << m_userSelMu2MaxEta << endmsg;
  msg(MSG::INFO) << "* userSelMinDimuMass             = " << m_userSelMinDimuMass << endmsg;
  msg(MSG::INFO) << "* userSelMaxDimuMass             = " << m_userSelMaxDimuMass << endmsg;

  // Initialize and print blackList
  m_blackListSet.insert(m_blackList.begin(),m_blackList.end());
  msg(MSG::INFO) << "* blackList;               = ";
  for (std::set<int>::iterator i = m_blackListSet.begin(); i!=m_blackListSet.end(); ++i)
     msg(MSG::INFO) << (*i) << " ";
  msg(MSG::INFO)<< endmsg;

  // Initialize and print whiteList
  m_whiteListSet.insert(m_whiteList.begin(),m_whiteList.end());
  msg(MSG::INFO) << "* whiteList               = ";
  for (std::set<int>::iterator i = m_whiteListSet.begin(); i!=m_whiteListSet.end(); ++i)
    msg(MSG::INFO) << (*i) << " ";
  msg(MSG::INFO) << endmsg;

  // Obtain random number generator for EvtGen
  static const bool CREATEIFNOTTHERE(true);
  CHECK(service("AtRndmGenSvc", m_atRndmGenSvc, CREATEIFNOTTHERE));
  m_evtAtRndmGen = new EvtInclusiveAtRndmGen(m_atRndmGenSvc,m_randomStreamName);

  // Create an instance of EvtGen and read particle properties and decay files
  EvtExternalGenList genList(true,xmlpath(),"gamma");
  EvtAbsRadCorr* radCorrEngine = genList.getPhotosModel();
  std::list<EvtDecayBase*> extraModels = genList.getListOfModels();

   // Create the EvtGen generator object
  // EvtGen myGenerator("decayFile.dec", "evt.pdl", randomEnginePointer,
  //                    radCorrEngine, &extraModels);


  m_myEvtGen = new EvtGen( m_decayFile.c_str(), m_pdtFile.c_str(), m_evtAtRndmGen, radCorrEngine, &extraModels);
  if(!m_userDecayFile.empty())
    m_myEvtGen->readUDecay(m_userDecayFile.c_str());

  m_nRepeatedDecays = 0;

  return StatusCode::SUCCESS;
}



StatusCode EvtInclusiveDecay::execute() {
  ATH_MSG_DEBUG("EvtInclusiveDecay executing");

  std::string   key = m_inputKeyName;

  // retrieve event from Transient Store (Storegate)
  
  const McEventCollection* oldmcEvtColl=0;
  if(m_readExisting) {
    CHECK(evtStore()->retrieve(oldmcEvtColl, key)); 
    // Fill the new McEventCollection with a copy of the initial HepMC::GenEvent
    m_mcEvtColl = new McEventCollection(*oldmcEvtColl);
  }
  else {CHECK(evtStore()->retrieve(m_mcEvtColl, key));}

  if(m_readExisting) {
    if(m_outputKeyName!=key) {
     CHECK(evtStore()->record( m_mcEvtColl,m_outputKeyName));
    }
  }
  

  McEventCollection::iterator mcItr;
  for( mcItr = m_mcEvtColl->begin(); mcItr != m_mcEvtColl->end(); mcItr++ )   {
    HepMC::GenEvent* hepMC = *mcItr;
   
    // Search HepMC record for particles to be decayed by EvtGen
    // NOTE: In order to ensure repeatability, we use a std::set of barcodes to obtain
    //       an ordered list of particles to be decayed by EvtGen.
    std::set<HepMC::GenVertexPtr> visited;
#ifdef HEPMC3
    std::set<HepMC::GenParticlePtr> toBeDecayed;
    for (auto p: hepMC->particles()) {
      if ( (!p->production_vertex()) ||
	   (p->production_vertex()->particles_in().size() == 0) ) {
	StatusCode sc = traverseDecayTree(p,false,visited,toBeDecayed);
	if (sc.isFailure())
	  return StatusCode::FAILURE;
      }
    }
    // Print HepMC in tree format if desired (before doing anything)
    if (m_printHepMCBeforeEvtGen) {
      msg(MSG::INFO) << "Printing HepMC record at " << hepMC << " BEFORE running EvtGen:" << endmsg;
      if (m_printHepMCHighLightTopLevelDecays)
	printHepMC(hepMC,&toBeDecayed);
      else
	printHepMC(hepMC);
    }
#else
    std::set<int> toBeDecayed;
    for (HepMC::GenEvent::particle_iterator itp = hepMC->particles_begin(); itp != hepMC->particles_end(); ++itp) {
      HepMC::GenParticle* p = *itp;
      if ( (!p->production_vertex()) ||
	   (p->production_vertex()->particles_in_size() == 0) ) {
	StatusCode sc = traverseDecayTree(p,false,visited,toBeDecayed);
	if (sc.isFailure())
	  return StatusCode::FAILURE;
      }
    }

    // Print HepMC in tree format if desired (before doing anything)
    if (m_printHepMCBeforeEvtGen) {
      msg(MSG::INFO) << "Printing HepMC record at " << hepMC << " BEFORE running EvtGen:" << endmsg;
      if (m_printHepMCHighLightTopLevelDecays)
	printHepMC(hepMC,&toBeDecayed);
      else
	printHepMC(hepMC);
    }
#endif

    // Decay selected particles
    bool eventPassesCuts(false);
    int loopCounter(0);
    while( !eventPassesCuts && loopCounter < m_maxNRepeatedDecays ) {
#ifdef HEPMC3
      for (auto p: toBeDecayed) {
        if (p==0) {
          msg(MSG::ERROR ) << "Overlapping decay tree for particle" << p <<endmsg;
          return StatusCode::FAILURE;
        }
        decayParticle(hepMC,p);
      }
#else
      for (std::set<int>::iterator itb = toBeDecayed.begin(); itb!=toBeDecayed.end(); ++itb) {
        auto p = hepMC->barcode_to_particle(*itb);
        if (p==0) {
          msg(MSG::ERROR ) << "Overlapping decay tree encountered for barcode " << *itb << endmsg;
          return StatusCode::FAILURE;
        }
        decayParticle(hepMC,p);
      }
#endif
      
      if(m_applyUserSelection)
        eventPassesCuts = passesUserSelection(hepMC);
      else
        eventPassesCuts = true;
      
      m_nRepeatedDecays++;
      loopCounter++;
    } 

    // Store the number of decay attempts in event weights std::map, only if repeated decays enabled
#ifdef HEPMC3
    if(m_maxNRepeatedDecays > 1)
      hepMC->weight("nEvtGenDecayAttempts") = loopCounter;
#else
    if(m_maxNRepeatedDecays > 1)
      hepMC->weights()["nEvtGenDecayAttempts"] = loopCounter;
#endif
    // Print HepMC in tree format if desired (after finishing all EvtGen decays)
    if (m_printHepMCAfterEvtGen) {
      msg(MSG::INFO)  << "Printing HepMC record at " << hepMC << " AFTER running EvtGen:" << endmsg;
      if (m_printHepMCHighLightTopLevelDecays)
	printHepMC(hepMC,&toBeDecayed);
      else
	printHepMC(hepMC);
    }
  }

  if(m_readExisting && m_outputKeyName==key) {
    McEventCollection* newmcEvtColl=0;
    newmcEvtColl = const_cast<McEventCollection*> (oldmcEvtColl);
    newmcEvtColl->clear();
    for (McEventCollection::const_iterator evt = m_mcEvtColl->begin(); evt != m_mcEvtColl->end(); ++evt) {
      newmcEvtColl->push_back(new HepMC::GenEvent(*(*evt)));
    }
    m_mcEvtColl->clear();
    delete m_mcEvtColl;
    m_mcEvtColl=NULL;
  }

  return StatusCode::SUCCESS;
}


StatusCode EvtInclusiveDecay::finalize() {

  if (m_checkDecayChannels) {
    ATH_MSG_INFO("The following particles were checked and didn't have any decay channels:");
    if (msgLvl(MSG::INFO)) {
      std::cout << std::endl;
      std::cout << " Particle code    Name from HepPDT        # Occurences" << std::endl;
      std::cout << "------------------------------------------------------"  << std::endl;
      for (std::map<int,long>::iterator p = m_noDecayChannels.begin(); p!=m_noDecayChannels.end(); ++p) {
	int id = p->first;
	int count = p->second;
	std::cout << std::setw(14) << id
		  << std::setw(20) << HepPID::particleName(id)
		  << std::setw(20) << count
		  << std::endl;
      }
      std::cout << std::endl;
    }
  }
  ATH_MSG_INFO("Total number of repeated decays: " << m_nRepeatedDecays);
  ATH_MSG_INFO("EvtInclusiveDecay finalized");
  return StatusCode::SUCCESS;
}



//
// Recursively traverse the decay tree of a particle p, looking for particles to be
// decayed by EvtGen. Care is taken to traverse each particle only once.
// Note that since in each decay tree only the top-most particle will be decayed
// by EvtGen (with its decay tree being deleted beforehand), we cannot use HepMC's
// "descendant" iterator.
//
#ifdef HEPMC3
StatusCode EvtInclusiveDecay::traverseDecayTree(HepMC::GenParticlePtr p,
						bool isToBeRemoved,
						std::set<HepMC::GenVertexPtr>& visited,
						std::set<HepMC::GenParticlePtr>& toBeDecayed) {
#else
StatusCode EvtInclusiveDecay::traverseDecayTree(HepMC::GenParticlePtr p,
						bool isToBeRemoved,
						std::set<HepMC::GenVertexPtr>& visited,
						std::set<int>& toBeDecayed) {
#endif
  ATH_MSG_VERBOSE("Inspecting: " << pdgName(p) << "   barcode:"<< HepMC::barcode(p));
  if (!isToBeRemoved) {
    if (isToBeDecayed(p,true)) {
#ifdef HEPMC3
      toBeDecayed.insert(p);
#else
      toBeDecayed.insert(HepMC::barcode(p));
#endif
      isToBeRemoved = true;
      ATH_MSG_VERBOSE("Selected particle for decay: " << pdgName(p) << " (barcode " << HepMC::barcode(p) << ")");

      // In principle we could stop the recursion here. However, to prevent
      // pathological cases in certain decay trees (in particular from Herwig),
      // we continue in order to mark all descendants of this particle
      // as visited. Thus none of these descendants can be flagged for further
      // decay, even if it has several mothers.
    }
  }
  auto v = p->end_vertex();
#ifdef HEPMC3
  if (v) {
    if (visited.insert(v).second) {
      if ( isToBeRemoved && (v->particles_in().size()>1) && m_checkDecayTree ) {
	ATH_MSG_WARNING("Found particle to be decayed with vertex with >1 incoming mother particles in decay tree");
	ATH_MSG_WARNING( ([&p, &v](){  std::stringstream ss;   HepMC::Print::line(ss,p); HepMC::Print::line(ss,v);  return ss.str();})());
      }
      for (auto itp: v->particles_out()) {
        ATH_CHECK(traverseDecayTree(itp,isToBeRemoved,visited,toBeDecayed) );      
      }
    }
  }
#else
  if (v) {
    if (visited.insert(v).second) {
      if ( isToBeRemoved && (v->particles_in_size()>1) && m_checkDecayTree ) {
	// This is normal for Herwig but should not occur for Pythia
	ATH_MSG_WARNING("Found particle to be decayed with vertex with >1 incoming mother particles in decay tree");
	ATH_MSG_WARNING( ([&p, &v](){  std::stringstream ss;   HepMC::Print::line(ss,p); HepMC::Print::line(ss,v);  return ss.str();})());
      }
      for (auto itp = v->particles_begin(HepMC::children); itp != v->particles_end(HepMC::children); ++itp) {
        ATH_CHECK(traverseDecayTree(*itp,isToBeRemoved,visited,toBeDecayed) );
      }
    }
  }
#endif
  return StatusCode::SUCCESS;
}



//
// Remove an existing decay tree
//
void EvtInclusiveDecay::removeDecayTree(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr p) {
  auto v = p->end_vertex();
  if (v) {
#ifdef HEPMC3
    //This is recursive in HepMC3. But explicit deletion is allowed as well.
    hepMC->remove_vertex(v);
    p->set_status(1);   // For now, flag particle as undecayed (stable)
    ATH_MSG_DEBUG("Removed existing " << pdgName(p) << " (barcode " << HepMC::barcode(p) << ")" );
#else
    std::set<int> vtxBarCodesToDelete;
    vtxBarCodesToDelete.insert(v->barcode());
    for (HepMC::GenVertex::vertex_iterator itv = v->vertices_begin(HepMC::descendants);
                                           itv != v->vertices_end(HepMC::descendants);
                                           ++itv)
      vtxBarCodesToDelete.insert((*itv)->barcode());
    for (std::set<int>::iterator itb = vtxBarCodesToDelete.begin(); itb != vtxBarCodesToDelete.end(); ++itb) {
      auto vdel = hepMC->barcode_to_vertex(*itb);
      hepMC->remove_vertex(vdel);
      delete vdel;
    }
    p->set_status(1);   // For now, flag particle as undecayed (stable)
    ATH_MSG_DEBUG("Removed existing " << pdgName(p) << " (barcode " << p->barcode() << ")" 
		  << " decay tree with " << vtxBarCodesToDelete.size() << " vertices");
#endif
  }
}



//
// Decay a particle with EvtGen. Any existing decay tree will be removed.
//
// The following status codes are used: 
//
// status == 1     - undecayed particle (also for particles that are not supposed to decay)
// status == m_decayedStatus (default 2)   - particle decayed by EvtGen
// status == 899   - particle was supposed to be decayed by EvtGen, but found no decay channel
//
// Note that if a particle with an existing decay tree but no defined decay channels
// in EvtGen is passed to this routine, the net effect will be to "undecay" this
// particle. The particle will be flagged by status code 899, but MAY BE LOST further
// downstream in the simulation chain. The default job parameters are for routine
// isToBeDecayed() to never enable decays of such particles by EvtGen.
//
void EvtInclusiveDecay::decayParticle(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr part) {
  ATH_MSG_DEBUG("Decaying particle " << pdgName(part) << " (barcode " << HepMC::barcode(part) << ")");
  if (msgLvl(MSG::VERBOSE)) HepMC::Print::line(std::cout,part);

  // Remove existing decay tree, if any, and flag particle as being decayed by EvtGen
  removeDecayTree(hepMC,part);
  part->set_status(899);

  // Create EvtGen version of part and have EvtGen decay it.
  // Since EvtGen uses GeV, convert particles momentum from MeV to GeV.
  int id = part->pdg_id();
  EvtId evtId=EvtPDL::evtIdFromStdHep(id);
  double en =(part->momentum()).e()/1000.;
  double px=(part->momentum()).px()/1000.;
  double py=(part->momentum()).py()/1000.;
  double pz=(part->momentum()).pz()/1000.;
  EvtVector4R evtP(en,px,py,pz);
  EvtParticle* evtPart = EvtParticleFactory::particleFactory(evtId,evtP);
  m_myEvtGen->generateDecay(evtPart);
  if (msgLvl(MSG::VERBOSE)) evtPart->printTree();
  double ct_s = part->production_vertex()->position().t();
  double x_s = part->production_vertex()->position().x();
  double y_s = part->production_vertex()->position().y();
  double z_s = part->production_vertex()->position().z();

  EvtVector4R treeStart(ct_s,x_s,y_s,z_s);
  // Add new decay tree to hepMC, converting back from GeV to MeV.
  addEvtGenDecayTree(hepMC, part, evtPart, treeStart, 1000.);
  if(evtPart->getNDaug() !=0) part->set_status(m_decayedStatus);
  evtPart->deleteTree();
}



void EvtInclusiveDecay::addEvtGenDecayTree(HepMC::GenEvent* hepMC, HepMC::GenParticlePtr part,
					   EvtParticle* evtPart, EvtVector4R treeStart,
                                           double momentumScaleFactor) {  
  if(evtPart->getNDaug()!=0) {  
    // Add decay vertex, starting from production vertex of particle
    double ct=(evtPart->getDaug(0)->get4Pos()).get(0)+treeStart.get(0);
    double x=(evtPart->getDaug(0)->get4Pos()).get(1)+treeStart.get(1);
    double y=(evtPart->getDaug(0)->get4Pos()).get(2)+treeStart.get(2);
    double z=(evtPart->getDaug(0)->get4Pos()).get(3)+treeStart.get(3);

    HepMC::GenVertexPtr end_vtx = HepMC::newGenVertexPtr(HepMC::FourVector(x,y,z,ct));
    hepMC->add_vertex(end_vtx);
    end_vtx->add_particle_in(part);

    // Add decay daughter with their own decay trees
    for(uint it=0; it<evtPart->getNDaug(); it++) {
      double e=(evtPart->getDaug(it)->getP4Lab()).get(0) * momentumScaleFactor;
      double px=(evtPart->getDaug(it)->getP4Lab()).get(1) * momentumScaleFactor;
      double py=(evtPart->getDaug(it)->getP4Lab()).get(2) * momentumScaleFactor;
      double pz=(evtPart->getDaug(it)->getP4Lab()).get(3) * momentumScaleFactor;
      int id=EvtPDL::getStdHep(evtPart->getDaug(it)->getId());
      int status=1;
      if(evtPart->getDaug(it)->getNDaug() != 0) status=m_decayedStatus;
      HepMC::GenParticlePtr daughter = HepMC::newGenParticlePtr(HepMC::FourVector(px,py,pz,e),id,status);
      end_vtx->add_particle_out(daughter);
      addEvtGenDecayTree(hepMC, daughter, evtPart->getDaug(it), treeStart, momentumScaleFactor);
    }
  }
}



//
// isToBeDecayed returns true if we want the particle p to be decayed by
// EvtGen based on the job options selected by the user.
// The parameter doCrossChecks is used to prevent double-counting for cross-checks
// if isToBeDecayed is called more than once for the same particle.
//
bool EvtInclusiveDecay::isToBeDecayed(const HepMC::GenParticlePtr p, bool doCrossChecks) {
  int id = p->pdg_id();
  int stat = p->status();
  int nDaughters = 0;
  auto v = p->end_vertex();
#ifdef HEPMC3
  if (v) nDaughters = v->particles_out().size();
#else
  if (v) nDaughters = v->particles_out_size();
#endif

  // Ignore documentation lines
  if (stat == 3) return false;
  // And any particles that aren't stable or decayed
  if(!m_isfHerwig && stat>2) return false;

  // Particularly for Herwig, try to ignore particles that really should
  // be flagged as documentation lines
  double m2 = p->momentum().m2();
  if (m2 < -1.0E-3) {
    ATH_MSG_DEBUG("Ignoring particle " << pdgName(p) << " with m^2 = " << m2);
    return false;
  }

  // Check whether EvtGen has any decay channels defined for this particle
  EvtId evtId = EvtPDL::evtIdFromStdHep(id);
  // std::cout << "EVTID: " << evtId.getId() << " alias " << evtId.getAlias() << std::endl;
  int nModes = 0;
  if (evtId.getId()>=0)
    //    nModes = EvtDecayTable::getNMode(evtId.getAlias());
    nModes = EvtDecayTable::getInstance()->getNMode(evtId.getAlias());
  if (doCrossChecks) {
    ATH_MSG_VERBOSE("Checking particle " << pdgName(p)
	<< " (status = " << stat
		    <<") -- " << nModes << " decay modes found");
    if (m_checkDecayChannels && nModes==0) {
      std::map<int,long>::iterator pos = m_noDecayChannels.find(id);
      if (pos != m_noDecayChannels.end())
	(pos->second)++;
      else
	m_noDecayChannels[id] = 1;
    }
  }

  // Check prohibit* settings
  if (m_prohibitFinalStateDecay && stat==1) return false;
  if (m_prohibitReDecay && nDaughters>0) return false;
  if (m_prohibitUnDecay && nModes==0) return false;
  if (m_prohibitRemoveSelfDecay && nDaughters>0) {
    // For now, check only children - this should be sufficient and checking all
    // descendants would be very expensive.
#ifdef HEPMC3
    for (auto  itd: v->particles_out()) {
      if (std::abs(itd->pdg_id()) == std::abs(id)) return false;
    }
#else
    for (HepMC::GenVertex::particle_iterator itd = v->particles_begin(HepMC::children);
	                                     itd != v->particles_end(HepMC::children);
                                             ++itd) {
      if (std::abs((*itd)->pdg_id()) == std::abs(id)) return false;
    }
#endif
  }

  // Check blackList
  if (m_blackListSet.count(std::abs(id))>0) return false;

  // Check allow* settings
  if (m_allowAllKnownDecays && nModes>0) return true;
  if (m_allowDefaultBDecays && isDefaultB(id)) return true;

  // Check whiteList
  if (m_whiteListSet.count(std::abs(id))>0) return true;

  return false;   // Default is NOT to decay through EvtGen
}



//
// The following mimicks the particle selection implemented in EvtDecay.
//
bool EvtInclusiveDecay::isDefaultB(const int pId) const {
  int id = std::abs(pId);
  if ( id == 511   || 
       id == 521   ||
       id == 531   ||
       id == 541   || 
       id == 5122  ||      
       id == 5132  ||                         
       id == 5232  || 
       id == 5112  ||        
       id == 5212  ||        
       id == 5222 )
    return true;
  else
    return false;
}

//
// Function to apply the user selection after repeated decay
// Now the selection is based on di-muon kinematics only
// TODO: to be replaced by something more configurable
//
bool EvtInclusiveDecay::passesUserSelection(HepMC::GenEvent* hepMC) {
  bool passed(false);
  std::vector<HepMC::GenParticlePtr> *muons = new std::vector<HepMC::GenParticlePtr>;

  for ( auto p:  *hepMC) {
    if( std::abs(p->pdg_id()) == 13 )
      muons->push_back(p);
  }
  
  for (auto muItr1 = muons->begin(); muItr1 != muons->end(); ++muItr1) {
    for (auto muItr2 = muItr1+1; muItr2 != muons->end(); ++muItr2) {
      if( m_userSelRequireOppositeSignedMu && (*muItr1)->pdg_id() * (*muItr2)->pdg_id() > 0)
        continue;
      if( !( (*muItr1)->momentum().perp() > m_userSelMu1MinPt && std::abs((*muItr1)->momentum().pseudoRapidity()) < m_userSelMu1MaxEta && 
             (*muItr2)->momentum().perp() > m_userSelMu2MinPt && std::abs((*muItr2)->momentum().pseudoRapidity()) < m_userSelMu2MaxEta ) &&
          !( (*muItr2)->momentum().perp() > m_userSelMu1MinPt && std::abs((*muItr2)->momentum().pseudoRapidity()) < m_userSelMu1MaxEta && 
             (*muItr1)->momentum().perp() > m_userSelMu2MinPt && std::abs((*muItr1)->momentum().pseudoRapidity()) < m_userSelMu2MaxEta ) )
        continue;
      double dimuMass = invMass((*muItr1),(*muItr2));
      if( !( dimuMass > m_userSelMinDimuMass && (dimuMass < m_userSelMaxDimuMass || m_userSelMaxDimuMass < 0.) ) )
        continue;
      passed = true;
    }
  }
  
  delete muons;
  
  return passed;
}

double EvtInclusiveDecay::invMass(const HepMC::GenParticlePtr p1, const HepMC::GenParticlePtr p2) {
  double p1Px = p1->momentum().px();
  double p1Py = p1->momentum().py();
  double p1Pz = p1->momentum().pz();
  double p1E  = p1->momentum().e();
  double p2Px = p2->momentum().px();
  double p2Py = p2->momentum().py();
  double p2Pz = p2->momentum().pz();
  double p2E  = p2->momentum().e();
  double dimuE = p2E + p1E;
  double dimuPx = p2Px + p1Px;
  double dimuPy = p2Py + p1Py;
  double dimuPz = p2Pz + p1Pz;
  double invMass = sqrt(dimuE*dimuE - dimuPx*dimuPx - dimuPy*dimuPy - dimuPz*dimuPz);
  
  return invMass;
}

//
// Utility functions to print a HepMC event record in a tree-like format, using
// colors to denote the status of particles and to indicate which particles
// are selected by the job options to be decayed by EvtGen.
//
#ifdef HEPMC3
void EvtInclusiveDecay::printHepMC(HepMC::GenEvent* hepMC, std::set<HepMC::GenParticlePtr>* barcodeList) {
  std::set<HepMC::GenVertexPtr> visited;
  unsigned int nParticlesFound = 0;
  unsigned int nTreesFound = 0;
  for (auto p: *hepMC) {
    if ( (!p->production_vertex()) ||
	 (p->production_vertex()->particles_in().size() == 0) ) {
      nTreesFound++;
      std::cout << "\n    Found new partial decay tree:\n" << std::endl;
      unsigned int nParticlesVisited = printTree(p,visited,1,barcodeList);
      std::cout << "\n    " << nParticlesVisited << " particles in this subtree" << std::endl;
      nParticlesFound += nParticlesVisited;
    }
  }
  std::cout << "\n    Total of " << nParticlesFound << " particles found in "
	    << nTreesFound << " decay subtrees in HepMC event record\n" << std::endl;
}
#else
void EvtInclusiveDecay::printHepMC(HepMC::GenEvent* hepMC, std::set<int>* barcodeList) {
  std::set<HepMC::GenVertexPtr> visited;
  unsigned int nParticlesFound = 0;
  unsigned int nTreesFound = 0;
  for (HepMC::GenEvent::particle_iterator itp = hepMC->particles_begin(); itp != hepMC->particles_end(); ++itp) {
    HepMC::GenParticle* p = *itp;
    if ( (!p->production_vertex()) ||
	 (p->production_vertex()->particles_in_size() == 0) ) {
      nTreesFound++;
      std::cout << "\n    Found new partial decay tree:\n" << std::endl;
      unsigned int nParticlesVisited = printTree(p,visited,1,barcodeList);
      std::cout << "\n    " << nParticlesVisited << " particles in this subtree" << std::endl;
      nParticlesFound += nParticlesVisited;
    }
  }
  std::cout << "\n    Total of " << nParticlesFound << " particles found in "
	    << nTreesFound << " decay subtrees in HepMC event record\n" << std::endl;
}
#endif

#ifdef HEPMC3
unsigned int EvtInclusiveDecay::printTree(HepMC::GenParticlePtr p,
				 std::set<HepMC::GenVertexPtr>& visited, int level, std::set<HepMC::GenParticlePtr>* barcodeList) {
  unsigned int nParticlesVisited = 1;
  for (int i=0; i<level; i++) std::cout << "    ";
  std::cout << pdgName(p,m_printHepMCHighlighted,barcodeList);
  auto v = p->end_vertex();
  if (v) {
    if (v->particles_in().size() > 1)
      std::cout << " [interaction: " << v->particles_in().size() << " particles, barcode " << HepMC::barcode(v) << "]    -->   ";
    else
      std::cout << "   -->   ";
    if (visited.insert(v).second) {
      for (auto itp: v->particles_out()) {
	std::cout << pdgName(itp,m_printHepMCHighlighted,barcodeList) << "   ";
      }
      std::cout << std::endl;
      for (auto itp: v->particles_out()) {
	if (itp->end_vertex())
	  nParticlesVisited += printTree(itp, visited, level+1, barcodeList);
	else
	  nParticlesVisited++;
      }
    } else
      std::cout << "see above" << std::endl;
  } else
    std::cout << "   no decay vertex\n" << std::endl;
  return nParticlesVisited;
}
#else
unsigned int EvtInclusiveDecay::printTree(HepMC::GenParticlePtr p,
				 std::set<HepMC::GenVertexPtr>& visited, int level, std::set<int>* barcodeList) {
  unsigned int nParticlesVisited = 1;
  for (int i=0; i<level; i++) std::cout << "    ";
  std::cout << pdgName(p,m_printHepMCHighlighted,barcodeList);
  auto v = p->end_vertex();
#ifdef HEPMC3
  if (v) {
    if (v->particles_in().size() > 1)
      std::cout << " [interaction: " << v->particles_in().size() << " particles, barcode " << HepMC::barcode(v) << "]    -->   ";
    else
      std::cout << "   -->   ";
    if (visited.insert(v).second) {
      for (auto itp: v->particles_out()) {
	std::cout << pdgName(itp,m_printHepMCHighlighted,barcodeList) << "   ";
      }
      std::cout << std::endl;
      for (auto itp: v->particles_out()) {
	if (itp->end_vertex())
	  nParticlesVisited += printTree(itp, visited, level+1, barcodeList);
	else
	  nParticlesVisited++;
      }
    } else
      std:: cout << "see above" << std::endl;
  } else
    std::cout << "   no decay vertex\n" << std::endl;
#else  
  if (v) {
    if (v->particles_in_size() > 1)
      std::cout << " [interaction: " << v->particles_in_size() << " particles, barcode " << v->barcode() << "]    -->   ";
    else
      std::cout << "   -->   ";
    if (visited.insert(v).second) {
      for (HepMC::GenVertex::particle_iterator itp = v->particles_begin(HepMC::children);
	                                       itp != v->particles_end(HepMC::children);
	                                       ++itp) {
	std::cout << pdgName(*itp,m_printHepMCHighlighted,barcodeList) << "   ";
      }
      std::cout << std::endl;
      for (HepMC::GenVertex::particle_iterator itp = v->particles_begin(HepMC::children);
	                                       itp != v->particles_end(HepMC::children);
                                               ++itp) {
	if ((*itp)->end_vertex())
	  nParticlesVisited += printTree(*itp, visited, level+1, barcodeList);
	else
	  nParticlesVisited++;
      }
    } else
      std:: cout << "see above" << std::endl;
  } else
    std::cout << "   no decay vertex\n" << std::endl;
#endif
  return nParticlesVisited;
}
#endif

#ifdef HEPMC3
std::string EvtInclusiveDecay::pdgName(const HepMC::GenParticlePtr p, bool statusHighlighting, std::set<HepMC::GenParticlePtr>* barcodeList) {
  std::ostringstream buf;
  if (statusHighlighting) {
    if ( ((barcodeList!=0) && (barcodeList->find(p) != barcodeList->end())) ||
         ((barcodeList==0) && isToBeDecayed(p,false)) )
      buf << "\033[7m";   // reverse
    if (p->status() != 1) {
      if (p->status() == m_decayedStatus)
	buf << "\033[33m";   // yellow
      else
	buf << "\033[31m";   // red
    }
  }
  buf << p->pdg_id();
  buf << "/" << HepPID::particleName(p->pdg_id());
  if (statusHighlighting) {
    buf << "\033[0m";   // revert color attributes
  }
  return buf.str();
}
#else
std::string EvtInclusiveDecay::pdgName(const HepMC::GenParticlePtr p, bool statusHighlighting, std::set<int>* barcodeList) {
  std::ostringstream buf;
  if (statusHighlighting) {
    if ( ((barcodeList!=0) && (barcodeList->find(HepMC::barcode(p)) != barcodeList->end())) ||
         ((barcodeList==0) && isToBeDecayed(p,false)) )
      buf << "\033[7m";   // reverse
    if (p->status() != 1) {
      if (p->status() == m_decayedStatus)
	buf << "\033[33m";   // yellow
      else
	buf << "\033[31m";   // red
    }
  }
  buf << p->pdg_id();
  buf << "/" << HepPID::particleName(p->pdg_id());
  if (statusHighlighting) {
    buf << "\033[0m";   // revert color attributes
  }
  return buf.str();
}

#endif


//
// Interface between Athena random number service and EvtGen's EvtRandomEngine class
//
EvtInclusiveAtRndmGen::EvtInclusiveAtRndmGen(IAtRndmGenSvc* atRndmGenSvc, std::string streamName)
  : m_atRndmGenSvc(atRndmGenSvc)
  , m_streamName(streamName)
{}

EvtInclusiveAtRndmGen::~EvtInclusiveAtRndmGen() {
}

double EvtInclusiveAtRndmGen::random() {
  CLHEP::HepRandomEngine* engine = m_atRndmGenSvc->GetEngine(m_streamName);
  return CLHEP::RandFlat::shoot(engine);
}

////////////////////////////////////////////////////////////////////////////////
std::string EvtInclusiveDecay::xmlpath(){

  char *cmtpath = getenv("CMTPATH");
  char *cmtconfig = getenv("CMTCONFIG");

  std::string foundpath = "";

  if(cmtpath != 0 && cmtconfig != 0){

    std::vector<std::string> cmtpaths;
    boost::split(cmtpaths, cmtpath, boost::is_any_of(std::string(":")));

    std::string installPath = "/InstallArea/" + std::string(cmtconfig) + "/share/Pythia8/xmldoc";

    for(std::vector<std::string>::const_iterator path = cmtpaths.begin();
        path != cmtpaths.end() && foundpath == ""; ++path){
      std::string testPath = *path + installPath;
      std::ifstream testFile(testPath.c_str());
      if(testFile.good()) foundpath = testPath;
      testFile.close();
    }

  }

  return foundpath;
}

