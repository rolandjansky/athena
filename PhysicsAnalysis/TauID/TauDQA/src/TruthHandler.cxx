/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../share/TruthHandler.h"
//#include "EventKernel/PdtPdg.h"

/////////////////////////
/** method to determine if a particle is only a G4 particle */
bool TruthHandler::isGenerator(int barcode, int status) {
	//FF, 25.04.2013: this method is old copy of
	// http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Generators/GenAnalysisTools/TruthHelper/src/IsGenerator.cxx
	// but needs to be updated
	// ToDo: either update or better, replace this method by call of TruthHelper::IsGenerator
	return ( barcode < 100000 && ( status<200 || status%1000 == 1 || status%1000 == 2  ) );
}

///////////////////////////////
/** determine if a particle is a final state stable particle  */
bool TruthHandler::isGenStable( const HepMC::GenParticle* p ) {
  //FF, 25.04.2013: this method is a copy of
  // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Generators/GenAnalysisTools/TruthHelper/src/IsGenStable.cxx
  // 1 line is missing
  // --> ToDo: replace this method by TruthHelper call

  // get generator id
  const HepMC::GenEvent* evt = p->parent_event();
  int g_id = evt->signal_process_id();
  int p_id = p->pdg_id();
  int HIJING = 50000000;

  HepMC::GenVertex* vertex = p->end_vertex();
  // we want to keep primary particle with status==2 but without vertex in HepMC
  int vertex_barcode=-999999;
  if (vertex) vertex_barcode=vertex->barcode();
  if ((g_id - HIJING)>=0 && (g_id - HIJING)<1000)  {
    // case of Hjing
    return (
  	    // pi0 and sigma0 not decayed but unstable
  	    ( (p->status()%1000 == 1) && abs(p_id) !=111 && abs(p_id) !=3212 )
  	    && (p->barcode()<200000)
  	     ) ? true:false;
  } else  {
    // other generators
    return (
  	    ( ( p->status()%1000 == 1) ||
  	      (p->status()==2 && vertex_barcode<-200000) ||
  	      (p->status()%1000 == 2 && p->status() > 1000)
  	      ) && (p->barcode()<200000)
  	    ) ? true:false;
  }
}

bool TruthHandler::isGenStable( const xAOD::TruthParticle* part ) {
  //FF, 25.04.2013: this method is a copy of
  // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Generators/GenAnalysisTools/TruthHelper/src/IsGenStable.cxx
  // 1 line is missing
  // --> ToDo: replace this method by TruthHelper call

  // get generator id
  //remove special HIJING treatment for xAOD
  // TODO: but back later if really needed
   if ( part == 0 ) return false;
  const xAOD::TruthVertex* vertex = part->decayVtx(); // was end_vertex()

  // we want to keep primary particle with status==2 but without vertex in HepMC
  int vertex_barcode=-999999;
  if (vertex) vertex_barcode=vertex->barcode();
  return (
          ( ( part->status()%1000 == 1) ||
            (part->status()==2 && vertex_barcode<-200000) ||
            (part->status()%1000 == 2 && part->status() > 1000)
          ) && (part->barcode()<200000)
         ) ? true:false;
}

////////////////////////
/** returns false if particle is only a reference particle in the generator.  Also checks whether particle
    is intermediate (Status==2) or final state and stable (Status==1)*/
bool TruthHandler::isGoodStatus(int Status){
  // status 10902 should be considered as status 2
  // see discussion here https://savannah.cern.ch/bugs/?59047 and linked hypernews threads,
  // e.g. https://groups.cern.ch/group/hn-atlas-Generators/Lists/Archive/Flat.aspx?RootFolder=%2Fgroup%2Fhn-atlas-Generators%2FLists%2FArchive%2FStatus%20Code%2010902%20with%20Pythia&FolderCTID=0x012002006E9F14B8795719469C62A1525BB20B42
  if((Status==2)||(Status==10902)) return true;
  else {
    if( ( Status != 1 ) && ( Status != 3 ) && ( Status < 200 ) && ( Status > 190 ))
      return true;    
  }
  return false;
}

bool TruthHandler::isGoodStatus(const HepMC::GenParticle* part, int StatusToCheck) {
  if(StatusToCheck==1) return isGenStable(part);
  if(StatusToCheck==2) return isGoodStatus(part->status());
  return false;
}

bool TruthHandler::isGoodStatus(const xAOD::TruthParticle* part, int StatusToCheck) {
  if(StatusToCheck==1) return isGenStable(part);
  if(StatusToCheck==2) return isGoodStatus(part->status());
  return false;
}

/////////////////////////
/** return true if xAODTruth particle as a child with pdgID==pdgIDtoCheck. Only direct children are checked. No iteration on children-of-children. */
bool TruthHandler::hasChild(const xAOD::TruthParticle* part, int pdgIDtoCheck) {
  // get decay vertex
  const xAOD::TruthVertex* vertex = part->decayVtx();
  // loop over all outgoing particles of decay vertx
  const std::size_t nChildren = vertex->nOutgoingParticles();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    const xAOD::TruthParticle * child = vertex->outgoingParticle(iChild);
    if ( child && ( pdgIDtoCheck == child->pdgId() ) ) {
      return true;
    }
  }
  return false;
}

/////////////////////////
/** method to determine if a particle is Tau or not */
bool TruthHandler::isTauTruth( const TruthParticle * part ){
  PDG::pidType id = part->pdgId();
  int ida = abs( (int) id );
  return ( ida == PDG::tau_minus && part->status() != 3  && !part->hasChild(id) );
}

/////////////////////////
/** method to determine if a particle is Tau or not */
bool TruthHandler::isTauTruth( const xAOD::TruthParticle* part ){
  int id = part->pdgId();
  return ( abs(id) == PDG::tau_minus && part->status() != 3  && !hasChild(part,id) );
}

//////////////////////
/**  determines if a particle is a true hadronic tau. Option to enforce the pdgID of the parent particle to be of type Mother.*/
bool TruthHandler::isHadTau(const HepMC::GenParticle* part, int Mother ){

  if(abs(part->pdg_id()) != 15 || !isGoodStatus(part,2)) return false;

  HepMC::GenVertex *vtx = part->end_vertex();

  if(!vtx) return false;
  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::children); desc != (vtx)->particles_end(HepMC::children); ++desc) {
      if((abs((*desc)->pdg_id())== 11) || (abs((*desc)->pdg_id())== 13) || (abs((*desc)->pdg_id())== 15) ) return false;//has lpton as child
  }

  if(Mother < 0) return true;

  bool HasMother=false;
  if(!part->production_vertex()) return false;
  for ( HepMC::GenVertex::particle_iterator mother = part->production_vertex()->particles_begin(HepMC::ancestors);
        mother != part->production_vertex()->particles_end(HepMC::ancestors); ++mother ) {
    if(fabs((*mother)->pdg_id())==Mother) HasMother=true;
  }

  return HasMother;
}

//////////////////////
/**  determines if a particle is a true hadronic tau for xAOD case. Option to enforce the pdgID of the parent particle to be of type Mother.*/
bool TruthHandler::isHadTau(const xAOD::TruthParticle* part, int Mother ){

  if(abs(part->pdgId()) != 15 || !isGoodStatus(part,2)) return false;

  const xAOD::TruthVertex* decayvtx = part->decayVtx();

  if(!decayvtx) return false;
  const std::size_t nChildren = decayvtx->nOutgoingParticles();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
    if ((abs(child->pdgId())== 11) || (abs(child->pdgId())== 13) || (abs(child->pdgId())== 15) ) return false;
  }

  if(Mother < 0) return true;

  bool HasMother=false;
  const xAOD::TruthVertex* prodvtx = part->prodVtx();
  if(!prodvtx) return false;
  const std::size_t nProd = prodvtx->nIncomingParticles();
  for ( std::size_t iMom = 0; iMom != nProd; ++iMom ) {
    const xAOD::TruthParticle * mom = prodvtx->incomingParticle(iMom);
    if(fabs(mom->pdgId())==Mother) HasMother=true;
  }

  return HasMother;
}

//////////////////////
/** determine if a particle is charged pion, exclude those which come from kaon decays.  */
bool TruthHandler::isGoodChrgPiDaughter(const xAOD::TruthParticle* Pion){
  if(!isGoodStatus(Pion,1)) return false;

  if( (abs(Pion->pdgId())!= 211) || !isGenStable( Pion) ) return false;
  /*  const xAOD::TruthVertex* prodvtx = Pion->prodVtx();
  if(!prodvtx) return false;
  const std::size_t nProd = prodvtx->nIncomingParticles();
  for ( std::size_t iMom = 0; iMom != nProd; ++iMom ) {
    const xAOD::TruthParticle * mom = prodvtx->incomingParticle(iMom);
    if(fabs(mom->pdgId())==PDG::K_S0) return false;
    }*/
  return true;
}
//////////////////////////////////////////////////////
bool TruthHandler::isGoodChrgPiDaughter(const HepMC::GenParticle* Pion){
  if(!isGoodStatus(Pion,1)) return false;

  if( (abs(Pion->pdg_id())!= 211) || !isGenStable( Pion) ) return false;
  if ( !Pion->production_vertex() ) return false;
  for ( HepMC::GenVertex::particle_iterator mother = Pion->production_vertex()->particles_begin(HepMC::parents);
        mother != Pion->production_vertex()->particles_end(HepMC::parents); ++mother ) {
    if(abs((*mother)->pdg_id())==PDG::K_S0) return false;
  }
  return true;
}

//////////////////////
/** determine whether tau decay includes given pdg_ID particle as an intermediate (Status==2) or
    final state (Status==1) daughter*/
bool TruthHandler::isTauTypeDecay(const HepMC::GenParticle* part, int PDGID, int Status ){
  if(abs(part->pdg_id()) != 15 || !isGoodStatus(part,2)) return false;
  HepMC::GenVertex *vtx = part->end_vertex();
  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::children);
	desc != (vtx)->particles_end(HepMC::children); ++desc) {
    if((abs((*desc)->pdg_id())== PDGID)&& isGoodStatus((*desc), Status)) return true;
  }
  return false;
}

////////////////////////////////////
/// Handle tau truth information. Return a container of all truth taus
//Option to enforce the pdgID of the parent particle to be of type Mother.
TruthParticleContainer* TruthHandler::getHadronicTruth(const TruthParticleContainer *mcpartTES, int Mother) {

  // create an AOD container to return hadronic truth taus
  TruthParticleContainer*  hadTauTES = new TruthParticleContainer(SG::VIEW_ELEMENTS);

  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();

  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles

    const HepMC::GenParticle* Gen_particle =(*mcpartItr)->genParticle();
    if(isHadTau(Gen_particle, Mother)) {
      hadTauTES->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return hadTauTES;
}


////////////////////////////////////
/// Handle tau truth information. Return a container of all hadronic truth taus
//Option to enforce the pdgID of the parent particle to be of type Mother.
xAOD::TruthParticleContainer* TruthHandler::getHadronicTruth(const xAOD::TruthParticleContainer *mcpartTES, int Mother) {

  // create an xAOD container to return hadronic truth taus
  xAOD::TruthParticleContainer*  hadTauTES = new xAOD::TruthParticleContainer(SG::VIEW_ELEMENTS);

  /** Iterate over list of MC particles looking for hadronic taus */
  xAOD::TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  xAOD::TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    xAOD::TruthParticle* hadtau = const_cast<xAOD::TruthParticle*>(*mcpartItr);
    if(isHadTau(hadtau, Mother)) {      
      hadTauTES->push_back(hadtau);
    }
  }
  return hadTauTES;
}


TruthParticleContainer* TruthHandler::getAllFinalTaus(const TruthParticleContainer *mcpartTES, int Mother) {
  // create an AOD container to return hadronic truth taus
  TruthParticleContainer*  hadTauTES = new TruthParticleContainer(SG::VIEW_ELEMENTS);
  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    const HepMC::GenParticle* Gen_particle =(*mcpartItr)->genParticle();
    if(fabs((Gen_particle)->pdg_id()!= 15) || !isGoodStatus(Gen_particle,2)) continue;
    bool HasMother = false;
    if(Mother < 0) HasMother=true;
    else if(Mother > 0){
      if(!Gen_particle->production_vertex()) HasMother= false;
      for ( HepMC::GenVertex::particle_iterator mother =Gen_particle->production_vertex()->particles_begin(HepMC::ancestors);
	    mother != Gen_particle->production_vertex()->particles_end(HepMC::ancestors); ++mother ) {
	if(fabs((*mother)->pdg_id())==Mother) HasMother=true;
      }
    }
    if(!HasMother) continue;

    if(isHadTau(Gen_particle, Mother) || isTauTypeDecay(Gen_particle,11, 1) || isTauTypeDecay(Gen_particle,13, 1))
    {
      // FIXME: cast needed for copying between containers
      hadTauTES->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return hadTauTES;
}

/////////////////////////
int TruthHandler::nProngTruth(const HepMC::GenParticle* hadTau, bool UseKaon ){
  if(!isHadTau(hadTau)) return 0;
  int nProng=0;
  HepMC::GenVertex *vtx = hadTau->end_vertex();

  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::descendants);
	desc != (vtx)->particles_end(HepMC::descendants); ++desc) {
    if(isGoodChrgPiDaughter(*desc))  nProng++;
    else if( (abs((*desc)->pdg_id())== PDG::K_plus) && isGenStable((*desc)) && UseKaon) nProng++;
  }
  return nProng;
}
//////////////////////
int TruthHandler::nProngTruth(const xAOD::TruthParticle* hadTau, bool UseKaon ){

  int nProng=0;

  const xAOD::TruthVertex* vertex = hadTau->decayVtx();
  if (vertex == 0) return nProng;
  // loop over all outgoing particles of decay vertx
  const std::size_t nChildren = vertex->nOutgoingParticles();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    const xAOD::TruthParticle * child = vertex->outgoingParticle(iChild);
    if ( child ) {
       if ( isGoodChrgPiDaughter(child) ) {
	  nProng++;
       }
       else if ( (abs(child->pdgId())== PDG::K_plus) && isGenStable((child)) && UseKaon ) {
	  nProng++;
       }
       else {
	  nProng += nProngTruth(child, UseKaon ); //recursive
       }
    }
  }
  return nProng;
}

int TruthHandler::nProngTruthTau(const xAOD::TruthParticle* hadTau, bool UseKaon ){
    if(abs(hadTau->pdgId()) != 15) return 0;
  return nProngTruth(hadTau,UseKaon);
}
/////////////////////////
/** method to count no. of neutral pions*/
int TruthHandler::numNeutPion(const xAOD::TruthParticle* hadTau){
    //  if(!isGoodStatus(hadTau,2)) return 0;

  int nNeu=0;

  const xAOD::TruthVertex* vertex = hadTau->decayVtx();
  if (vertex == 0) return nNeu;
  // loop over all outgoing particles of decay vertx
  const std::size_t nChildren = vertex->nOutgoingParticles();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    const xAOD::TruthParticle * child = vertex->outgoingParticle(iChild);
    if((abs((child)->pdgId())== 111) /*&& ( ((child)->status()==2) || ((child)->status()==10902) )*/ ){  nNeu++;
    } //else{ return 0;nNeu += numNeutPion(child);}
  }
  return nNeu;
  }

/** method to count no. of neutral pions*/
int TruthHandler::numNeutPion(const HepMC::GenParticle* hadTau ){
  if(!isGoodStatus(hadTau,2)) return 0;

  int nNeu=0;
  HepMC::GenVertex *vtx = hadTau->end_vertex();

  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::descendants);
	desc != (vtx)->particles_end(HepMC::descendants); ++desc) {
    // status 10902 should be considered as status 2
    // see discussion here https://savannah.cern.ch/bugs/?59047 and linked hypernews threads,
    // e.g. https://groups.cern.ch/group/hn-atlas-Generators/Lists/Archive/Flat.aspx?RootFolder=%2Fgroup%2Fhn-atlas-Generators%2FLists%2FArchive%2FStatus%20Code%2010902%20with%20Pythia&FolderCTID=0x012002006E9F14B8795719469C62A1525BB20B42
    if((abs((*desc)->pdg_id())== 111) && ( ((*desc)->status()==2) || ((*desc)->status()==10902) ) )  nNeu++;
  }
  return nNeu;
}

/////////////////////////
/** method to count no. of neutral pions and get the pointer to pi+, rho+, a1+ */
const TruthParticle* TruthHandler::nPi0Truth( const TruthParticle* hadTau, int &nPi0 ) {
  nPi0 = numNeutPion(hadTau);
  return GetIntermDaughter(hadTau);
}

/////////////////////////
/** method to get a pointer to the intermediate daughter (either the pion, rho, or a1) */
const TruthParticle* TruthHandler::GetIntermDaughter(const TruthParticle* hadTau ){
  const TruthParticle* Daughter1 = NULL;

  for (unsigned int i1 = 0; i1 < hadTau->nDecay(); i1++) {// i1 loop over particles

    // decay to Pi+(211) or rho(213) or a1(20213)
    if ( ( abs((int) hadTau->pdgDecay(i1)) == PDG::pi_plus ) ||
         ( abs((int) hadTau->pdgDecay(i1)) == PDG::rho_plus ) ||
         ( abs((int) hadTau->pdgDecay(i1)) == PDG::a_1_plus ) )
      Daughter1 = hadTau->child(i1);
  }
  return Daughter1;
}

/////////////////////////
/** determines whether ``Daughter'' is a hadronic daughter of the decay of the ``TrueTau'',
    (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
bool TruthHandler::isHadTauDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter, int Status){
  if(!isGoodStatus(Daughter, Status) ) return false;
  if ( !TrueTau->end_vertex() ) return false;
  if(!isHadTau(TrueTau)) return false;

  HepMC::GenVertex *vtx = TrueTau->end_vertex();

  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::descendants);
	desc != (vtx)->particles_end(HepMC::descendants); ++desc) {
    if( isGoodStatus((*desc),Status) && ((*desc)==Daughter) && ( ((*desc)->barcode()) == (Daughter->barcode()) )) return true;
  }
  return false;

}

/////////////////////////
/** determines whether ``Daughter'' is a hadronic daughter of the decay of a tau in the  ``TrueTauCollection'',
    (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
bool TruthHandler::isHadTauDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter, int Status){
  for(HepMC::GenEvent::particle_const_iterator MCitr=TrueTauCollection->particles_begin(); MCitr!=TrueTauCollection->particles_end(); ++MCitr ){
    if (TruthHandler::isHadTauDaughter(*MCitr, Daughter, Status)) return true;
  }
  return false;
}
bool TruthHandler::isHadTauDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* TruthDaughter, int Status){
  const HepMC::GenParticle* Daughter = TruthDaughter->genParticle();
  for (TruthParticleContainer::const_iterator MCitr = TrueTauCollection->begin(); MCitr!=TrueTauCollection->end(); ++MCitr) {
    const HepMC::GenParticle* Gen_particle =(*MCitr)->genParticle();
    if(isHadTauDaughter(Gen_particle, Daughter, Status)) return true;
  }
  return false;
}


/////////////////////////
/** returns a collection of the true intermediate daughters  (Status==2) or final daughters (Status==2)
    of the true hadronic tau*/
TruthParticleContainer* TruthHandler::getTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES, int Status) {
  TruthParticleContainer*  hadDaughters = new TruthParticleContainer(SG::VIEW_ELEMENTS);

  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();

  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    if(isHadTauDaughter(TrueTau, *mcpartItr, Status)) {
      // cast needed for copying between containers.
      hadDaughters->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return hadDaughters;
}


/////////////////////////
/** returns a collection of the true hadronics taus which decay to nProngs */
TruthParticleContainer* TruthHandler::getHadronicnProngTruth(const TruthParticleContainer *mcpartTES, int nProngs) {
  // create an AOD container to return hadronic truth taus
  TruthParticleContainer*  hadTaunProngTES = new TruthParticleContainer(SG::VIEW_ELEMENTS);

  TruthParticleContainer*  hadTauTES = getHadronicTruth(mcpartTES);
  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = hadTauTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = hadTauTES->end();

  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles

    const HepMC::GenParticle* Gen_particle =(*mcpartItr)->genParticle();
    if(nProngTruth(Gen_particle)==nProngs) {
      // cast needed for copying between containers
      hadTaunProngTES->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return  hadTaunProngTES;
}


/////////////////////////
/** method to determine the visible et of hadronic truth taus */
CLHEP::HepLorentzVector TruthHandler::getTauVisibleSumTruth( const TruthParticle* hadTau ) {

  CLHEP::HepLorentzVector visibleSum(0.,0.,0.,0.);

  for( unsigned int ii=0; ii < hadTau->nDecay(); ++ii) {
    const HepMC::GenParticle* Gen_child =   hadTau->genChild(ii);
    if (! isGoodStatus(Gen_child, 1) && !  isGoodStatus(Gen_child, 2) ) continue;
    if( abs((int) hadTau->pdgDecay(ii)) != PDG::nu_e  &&
	abs((int) hadTau->pdgDecay(ii)) != PDG::nu_mu &&
	abs((int) hadTau->pdgDecay(ii)) != PDG::nu_tau  ) {
      visibleSum += hadTau->pDecay(ii);
    }
  }

  return visibleSum;
}

/////////////////////////
/** method to determine the visible et of hadronic truth taus */
TLorentzVector TruthHandler::getTauVisibleSumTruth( const xAOD::TruthParticle* hadTau ) {

  TLorentzVector visibleSum(0.,0.,0.,0.);

  const xAOD::TruthVertex* decayvtx = hadTau->decayVtx();

  if(!decayvtx) return visibleSum;
  const std::size_t nChildren = decayvtx->nOutgoingParticles();
  for ( std::size_t iChild = 0; iChild != nChildren; ++iChild ) {
    const xAOD::TruthParticle * child = decayvtx->outgoingParticle(iChild);
    if (! isGoodStatus(child, 1) && !  isGoodStatus(child, 2) ) continue;
    if( abs((int) child->pdgId()) != PDG::nu_e  &&
	abs((int) child->pdgId()) != PDG::nu_mu &&
	abs((int) child->pdgId()) != PDG::nu_tau  ) {
      visibleSum += child->p4();
    }
  }

  return visibleSum;
}


/////////////////////////
/** This method adds the momentum of the daughter neutrinos of the tau, it is called recursively */
CLHEP::HepLorentzVector TruthHandler::sumDaughterNeutrinos( const TruthParticle *part ) {

  CLHEP::HepLorentzVector nu( 0, 0, 0, 0);

  if ((abs((int)part->pdgId()) == PDG::nu_e) || (abs((int)part->pdgId()) == PDG::nu_mu) || (abs((int)part->pdgId()) == PDG::nu_tau)) {
    nu.setPx(part->px());
    nu.setPy(part->py());
    nu.setPz(part->pz());
    nu.setE(part->e());
    return nu;
  }

  for (unsigned int j=0; j<part->nDecay(); j++) {
    nu += sumDaughterNeutrinos((TruthParticle*)part->child(j));
  }

  return nu;
}

/////////////////////////
/** This method returns the HepLorentzVector of leading charged pion */
CLHEP::HepLorentzVector TruthHandler::getPiChTruth( const TruthParticle* hadTau ) {
  
  std::vector<CLHEP::HepLorentzVector> PiCh;
  CLHEP::HepLorentzVector temp;
  
  for (unsigned int i1 = 0; i1 < hadTau->nDecay(); i1++) {// i1 loop over particles
    if (( abs((int) hadTau->pdgDecay(i1)) == PDG::pi_plus ) && (isGenerator(hadTau->child(i1)))){
      temp = hadTau->pDecay(i1);
      PiCh.push_back(temp);
    }
    else {
      for (unsigned int i2 = 0; i2 < hadTau->child(i1)->nDecay(); i2++) {//i2
	if (( abs((int)hadTau->child(i1)->pdgDecay(i2)) == PDG::pi_plus ) && (isGenerator(hadTau->child(i1)->child(i2)))) {
	  temp = hadTau->child(i1)->pDecay(i2) ;
	  PiCh.push_back(temp); 
	}
	else {
	  for (unsigned int i3 = 0; i3 < hadTau->child(i1)->child(i2)->nDecay(); i3++) {//i3
	    if (( abs((int) hadTau->child(i1)->child(i2)->pdgDecay(i3)) == PDG::pi_plus) && (isGenerator(hadTau->child(i1)->child(i2)->child(i3)))) {
	      temp = hadTau->child(i1)->child(i2)->pDecay(i3);
	      PiCh.push_back(temp);
	    }
	    else {
	      for (unsigned int i4 = 0; i4 < hadTau->child(i1)->child(i2)->child(i3)->nDecay(); i4++) {//i4
		if (( abs((int) hadTau->child(i1)->child(i2)->child(i3)->pdgDecay(i4)) == PDG::pi_plus) && (isGenerator(hadTau->child(i1)->child(i2)->child(i3)->child(i4)))) {
		  temp = hadTau->child(i1)->child(i2)->child(i3)->pDecay(i4);
		  PiCh.push_back(temp);
		}
	      }//i4
	    }
	  }//i3
	}
      }//i2
    }
  }//end i1 loop over particles
  
  CLHEP::HepLorentzVector leadPiCh(0);
  if(PiCh.size() > 0) {
    std::sort(PiCh.begin(), PiCh.end(), std::greater<CLHEP::HepLorentzVector>());
    leadPiCh = PiCh[0];
  }
  
  return leadPiCh; 
}

/////////////////////////
