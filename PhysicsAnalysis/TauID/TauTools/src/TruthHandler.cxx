/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////////////////////////////////// 
/// Name    : TruthHandler.cxx
/// Package : offline/PhysicsAnalysis/TauID/TauTools
/// Authors: Jyothsna Komaragiri, Dugan O'Neil
/// Date: April 2008
///
/// Last modified: Jennifer Godfrey
///
/// DESCRIPTION:
///
/// This class is a simplified example of accessing truth information
/// for taus reconstructed in release 14.x.x
///
//////////////////////////////////////////////////////////////////////////////////
#include "TauTools/TruthHandler.h"
#include "EventKernel/PdtPdg.h"

using namespace std;
using std::cout;
using std::endl;
using std::setw;

namespace{
  static const double GeV = 1000.0;
}

//////////////////////////////////////////////////////////////////////////////////////
/// Constructor

TruthHandler::TruthHandler() {
  
  /** initialize some simple counters */
  m_nochild_counter = 0;
  m_totaltruthtau_counter = 0;
  m_totalHadCand_counter = 0;
  
  m_particleDataTable = 0;
  
  m_debug = false;
  m_Kaon  = false;
}

/////////////////////////////////////////////////////////////////////////////////////
/// Destructor - check up memory allocation
/// delete any memory allocation on the heap

TruthHandler::~TruthHandler() {
  delete m_particleDataTable;
}

///////////////////////////////////////////////////////////////////////////////////
/// Finalize - delete any memory allocation from the heap

void TruthHandler::finalize() {
  
  // Rather than dumping summary information to the message log send it to 
  // special summary file. This way it won't get lost in error messages.
  std::ofstream* outfile = new std::ofstream("TruthHandler.log");
  
  *outfile << " +++ TruthHandler Stats +++ \n" << std::endl;
  *outfile << " total number of truth taus: " << m_totaltruthtau_counter << std::endl;
  *outfile << " total number of truth taus with no children: " << m_nochild_counter << std::endl;
  *outfile << " total number of hadronic candidates: " << m_totalHadCand_counter << std::endl;
  
  outfile->close();
  delete outfile;
  
}

///////////////////////////////////////////////////////////////////////////////////
void TruthHandler::readPDT() {
  
  m_particleDataTable = new HepPDT::ParticleDataTable;  
  
  HepPDT::TableBuilder  tb( *m_particleDataTable);
  // read the input - put as many here as you want
  std::string pdgname = PathResolver::find_file ("PDGTABLE.MeV", "DATAPATH");
  
  if (pdgname == "")  {
    cout << "TruthHandler FATAL: Could not locate PDGTABLE.MeV  data file" << endl;
  }
  const char * pdgfile= pdgname.c_str() ;
  
  std::ifstream pdfile(pdgfile);
  
  if (!pdfile)
    cout << "TruthHandler FATAL: Could not load PDG data file. Check that PDGTABLE file is there" << endl;
  else {
    cout << "TruthHandler INFO: Load PDGTABLE " << endl;
    if( !addPDGParticles( pdfile, tb ) ) { std::cout << "TruthHandler error reading PDG file " << std::endl; }
  }
  
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


TruthParticleContainer* TruthHandler::getAllFinalTaus(const TruthParticleContainer *mcpartTES, int Mother) {
  // create an AOD container to return hadronic truth taus
  TruthParticleContainer*  hadTauTES = new TruthParticleContainer(SG::VIEW_ELEMENTS);
  //bool HasMother=false; <== this should not be here. creates compiler warning
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

int TruthHandler::nProngTruth(const HepMC::GenParticle* hadTau, bool UseKaon ){
  if(!isHadTau(hadTau)) return 0;
  int nProng=0;
  HepMC::GenVertex *vtx = hadTau->end_vertex();
  
  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::descendants);
	desc != (vtx)->particles_end(HepMC::descendants); ++desc) {
    if(isGoodChrgPiDaughter(*desc))  nProng++;
    else if( (abs((*desc)->pdg_id())== PDG::K_plus) && IsGenStable((*desc)) && UseKaon) nProng++;
  }
  return nProng;
}


/////////////////////////
int TruthHandler::nProngTruth( const TruthParticle* hadTau, bool UseKaon ) {
  
  const HepMC::GenParticle* TrueTau = hadTau->genParticle();
  return nProngTruth(TrueTau, UseKaon);
}
/////////////////////////
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
/** method to count no. of neutral pions */
int TruthHandler::numNeutPion( const TruthParticle* hadTau ) {
  const HepMC::GenParticle* TrueTau = hadTau->genParticle();
  return numNeutPion(TrueTau);
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
TruthParticleContainer* TruthHandler::getZTauDecayMode(const TruthParticleContainer *mcpartTES, int decayMode) {//getZTauDecayMode
  
  string tau1;
  // create an AOD container to return hadronic truth taus
  TruthParticleContainer*  truthTES = new TruthParticleContainer(SG::VIEW_ELEMENTS);
  TruthParticleContainer*  blank = new TruthParticleContainer();
  
  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();
  
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    
    if( ! isGenerator(*mcpartItr) ) continue; // remove G4 particles
    // remove taus which decay to tau. We want the last tau in the decay chain
    if( ! isTauTruth(*mcpartItr)) continue;
    
    // tau either comes from Z or W or from another tau
    if ( (*mcpartItr)->hasMother(PDG::Z0) || (*mcpartItr)->hasMother(PDG::W_plus) || (*mcpartItr)->hasMother(PDG::W_minus) || 
	 (*mcpartItr)->hasMother(PDG::tau_plus) || (*mcpartItr)->hasMother(PDG::tau_minus) ) {  
      // OK, we have a true tau, increment counter
      
      if ( (*mcpartItr)->nDecay() == 0 ) {
	m_nochild_counter++;
	cout << "TruthHandler ERROR: This tau has no child! " << m_nochild_counter << endl;  
      }
      
      bool had = false;
      bool ele = false;
      bool muon = false;
      if ( (*mcpartItr)->hasChild(PDG::e_minus) || (*mcpartItr)->hasChild(PDG::e_plus) ) ele = true; 
      
      if ( (*mcpartItr)->hasChild(PDG::mu_minus) || (*mcpartItr)->hasChild(PDG::mu_plus) ) muon = true;
      
      
      if ( !(*mcpartItr)->hasChild(PDG::e_minus) && !(*mcpartItr)->hasChild(PDG::e_plus) && 
	   !(*mcpartItr)->hasChild(PDG::nu_e) && !(*mcpartItr)->hasChild(PDG::anti_nu_e) && 
	   !(*mcpartItr)->hasChild(PDG::mu_minus) && !(*mcpartItr)->hasChild(PDG::mu_plus) && 
	   !(*mcpartItr)->hasChild(PDG::nu_mu) && !(*mcpartItr)->hasChild(PDG::anti_nu_mu) ) {// non-leptonic	  
	
	for (unsigned int i1 = 0; i1 < (*mcpartItr)->nDecay(); i1++) {// i1 loop over particles
	  if ( (( abs((int) (*mcpartItr)->pdgDecay(i1)) == PDG::pi_plus ) || (*mcpartItr)->child(i1)->status() == 1) 
	       && (isGenerator((*mcpartItr)->child(i1))))
	    had = true;
	  else {
	    for (unsigned int i2 = 0; i2 < (*mcpartItr)->child(i1)->nDecay(); i2++) {//i2
	      if (( (abs((int)(*mcpartItr)->child(i1)->pdgDecay(i2)) == PDG::pi_plus ) || (*mcpartItr)->child(i1)->child(i2)->status() == 1) 
		  && (isGenerator((*mcpartItr)->child(i1)->child(i2))))
		had = true;
	      else {
		for (unsigned int i3 = 0; i3 < (*mcpartItr)->child(i1)->child(i2)->nDecay(); i3++) {//i3
		  if (( (abs((int) (*mcpartItr)->child(i1)->child(i2)->pdgDecay(i3)) == PDG::pi_plus) || (*mcpartItr)->child(i1)->child(i2)->child(i3)->status() == 1 )
		      && (isGenerator((*mcpartItr)->child(i1)->child(i2)->child(i3))))
		    had = true;
		  else {
		    for (unsigned int i4 = 0; i4 < (*mcpartItr)->child(i1)->child(i2)->child(i3)->nDecay(); i4++) {//i4
		      if (( (abs((int) (*mcpartItr)->child(i1)->child(i2)->child(i3)->pdgDecay(i4)) == PDG::pi_plus) 
			    || (*mcpartItr)->child(i1)->child(i2)->child(i3)->child(i4)->status() == 1 )
			  && (isGenerator((*mcpartItr)->child(i1)->child(i2)->child(i3)->child(i4))) )
			had = true;
		    }//i4
		  }
		}//i3
	      }
	    }//i2
	  }
	} // end i1 loop over particles
	/** if we have a good hadronic tau then put it in the output container*/
      }//end rid leptonic
      // FIXME: cast needed for copying between containers.
      TruthParticle* tp = const_cast<TruthParticle*> (*mcpartItr);
      switch (decayMode){
	case 1: 
	  if(had==true){ truthTES->push_back(tp); tau1 = "had";}
	  break;
	case 2:
	  if(ele==true) {truthTES->push_back(tp); tau1 = "ele";}
	  break;
	case 3:
	  if(muon==true){ truthTES->push_back(tp); tau1 = "muon";}
	  break;
	case 4:
	  if(truthTES->size()==0){
	    if(had==true || ele==true){ truthTES->push_back(tp); if(had) tau1 = "had"; if(ele) tau1 = "ele";}
	  }
	  else{
	    if(ele==true && tau1=="had") truthTES->push_back(tp);
	    if(had==true && tau1=="ele") truthTES->push_back(tp);
	  }
	  break;
	case 5:
	  if(truthTES->size()==0){
	    if(had==true || muon==true){ truthTES->push_back(tp); if(had) tau1 = "had"; if(muon) tau1 = "muon";}
	  }
	  else{
	    if(muon==true && tau1=="had") truthTES->push_back(tp);
	    if(had==true && tau1=="muon") truthTES->push_back(tp);
	  }
	  break;
	case 6:
	  if(truthTES->size()==0){
	    if(muon==true || ele==true){ truthTES->push_back(tp); if(muon) tau1 = "muon"; if(ele) tau1 = "ele";}
	  }
	  else{
	    if(ele==true && tau1=="muon") truthTES->push_back(tp);
	    if(muon==true && tau1=="ele") truthTES->push_back(tp);
	  }
	  break;
	  
      }
      
      
    } // end motherhood=Z or tau if
  } // end loop over all MC particles
  
  //cout << "TruthHandler DEBUG: getHadronicTruth() succeeded" << endl;
  if(truthTES->size()==2)  return truthTES;
  else return blank;
}//getZtauDecayMode

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
/** method to determine if a particle is only a G4 particle */

bool TruthHandler::isGenerator( const TruthParticle * part ){
  //FF, 25.04.2013: this method is old copy of 
  // http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Generators/GenAnalysisTools/TruthHelper/src/IsGenerator.cxx
  // but needs to be updated
  // ToDo: either update or better, replace this method by call of TruthHelper::IsGenerator
  return ( part->barcode() < 100000 && ( part->status()<200 ||
					 part->status()%1000 == 1 || 
					 part->status()%1000 == 2  ) );
}

///////////////////////////////
/** determine if a particle is a final state stable particle  */
bool TruthHandler::isGenerator( const HepMC::GenParticle* part ){
  //FF, 25.04.2013: see comment above
  return ( part->barcode() < 100000 && ( part->status()%1000 == 1 || part->status()%1000 == 2 ));
}
bool TruthHandler::IsGenStable( const HepMC::GenParticle* p ) {
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

////////////////////////
/** returns false if particle is only a reference particle in the generator.  Also checks whether particle
    is intermediate (Status==2) or final state and stable (Status==1)*/
bool TruthHandler::isGoodStatus(const HepMC::GenParticle* part, int Status){
  if(Status==1) return IsGenStable(part);
  if(Status==2) {
    // status 10902 should be considered as status 2
    // see discussion here https://savannah.cern.ch/bugs/?59047 and linked hypernews threads, 
    // e.g. https://groups.cern.ch/group/hn-atlas-Generators/Lists/Archive/Flat.aspx?RootFolder=%2Fgroup%2Fhn-atlas-Generators%2FLists%2FArchive%2FStatus%20Code%2010902%20with%20Pythia&FolderCTID=0x012002006E9F14B8795719469C62A1525BB20B42
    if((part->status()==2)||(part->status()==10902)) return true; 
    else if( ( part->status() != 1 ) && ( part->status() != 3 ) && ( part->status() < 200 ) && ( part->status() > 190 )) return true;
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

//////////////////////
/**  determines if a particle is a true hadronic tau. Option to enforce the pdgID of the parent particle to be of type Mother.*/
bool TruthHandler::isHadTau(const HepMC::GenParticle* part, int Mother ){
  
  if(abs(part->pdg_id()) != 15 || !isGoodStatus(part,2)) return false;
  
  HepMC::GenVertex *vtx = part->end_vertex();
  if(!vtx) return false;  
  for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::children); desc != (vtx)->particles_end(HepMC::children); ++desc) {
    if((abs((*desc)->pdg_id())== 11) || (abs((*desc)->pdg_id())== 13) || (abs((*desc)->pdg_id())== 15) ) return false;
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
bool TruthHandler::isHadTau(const TruthParticle* part, int Mother ){
  const HepMC::GenParticle* TrueTau = part->genParticle();
  return isHadTau(TrueTau, Mother);
}
//////////////////////
/** determine if a particle is charged pion, exclude those which come from kaon decays.  */
bool TruthHandler::isGoodChrgPiDaughter(const HepMC::GenParticle* Pion){
  if(!isGoodStatus(Pion,1)) return false;
  
  if( (abs(Pion->pdg_id())!= 211) || !IsGenStable( Pion) ) return false;
  if ( !Pion->production_vertex() ) return false;
  for ( HepMC::GenVertex::particle_iterator mother = Pion->production_vertex()->particles_begin(HepMC::parents);
        mother != Pion->production_vertex()->particles_end(HepMC::parents); ++mother ) {
    if(abs((*mother)->pdg_id())==PDG::K_S0) return false;
  }
  /*
    if ( !Pion->end_vertex() ) return true;
    
    for ( HepMC::GenVertex::particle_iterator child = Pion->end_vertex()->particles_begin(HepMC::children);
    child != Pion->end_vertex()->particles_end(HepMC::children); ++child ) {
    if(abs((*child)->pdg_id())==211) return false;
    }
  */
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

//////////////////////
/** determine whether tau decay includes given pdg_ID particle as an intermediate (Status==2) or 
    final state (Status==1) daughter*/
bool TruthHandler::isTauTypeDecay(const TruthParticle* part, int PDGID, int Status ){
  const HepMC::GenParticle* TrueTau = part->genParticle();
  return isTauTypeDecay(TrueTau, PDGID, Status);
}

//////////////////////
/** determine whether tau decay is an electron decay*/
bool TruthHandler::isTauElecDecay(const HepMC::GenParticle* part ){
  return isTauTypeDecay(part, 11, 1);
}

//////////////////////
/** determine whether tau decay is an electron decay*/
bool TruthHandler::isTauElecDecay(const  TruthParticle* part){
  return isTauTypeDecay(part, 11, 1);
}

//////////////////////
/** determine whether tau decay is a muon decay*/
bool TruthHandler::isTauMuonDecay(const HepMC::GenParticle* part ){
  return isTauTypeDecay(part, 13, 1);
}

//////////////////////
/** determine whether tau decay is a muon decay*/
bool TruthHandler::isTauMuonDecay(const TruthParticle* part ){
  return isTauTypeDecay(part, 13, 1);
}
//////////////////////
bool TruthHandler::isTauRhoDecay(const HepMC::GenParticle* part ){
  return isTauTypeDecay(part, PDG::rho_plus, 2);
}
//////////////////////
bool TruthHandler::isTauRhoDecay(const TruthParticle* part ){
  return isTauTypeDecay(part, PDG::rho_plus, 2);
}

//////////////////////
/** determine whether tau decays directly to a rho (does not include a1->rho)*/
bool TruthHandler::isTauA1Decay(const HepMC::GenParticle* part ){
  return isTauTypeDecay(part, PDG::a_1_plus,2);
}
//////////////////////
/** determine whether tau decays directly to a rho (does not include a1->rho)*/
bool TruthHandler::isTauA1Decay(const TruthParticle* part ){
  return isTauTypeDecay(part, PDG::a_1_plus,2);
}


/////////////////////////
/** method to determine if a particle is charged pion or not */
bool TruthHandler::isPiChTruth( const TruthParticle * part ){
  
  PDG::pidType id = part->pdgId();
  int ida = abs( (int) id );
  
  return (ida == PDG::pi_plus);
  
}

/////////////////////////
float TruthHandler::getChargeTruth( const TruthParticle * part ){
  
  if (!m_particleDataTable)
    readPDT();
  const HepPDT::ParticleData* ap = m_particleDataTable->particle( HepPDT::ParticleID( abs( (int) part->pdgId() ) ) );
  float charge = ap->charge();
  if ( (int) (part->pdgId()) < 0 ) charge = -charge;
  return charge;
  
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
/** determines whether ``Daughter'' is a hadronic daughter of the decay of the ``TrueTau'', 
    (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
bool TruthHandler::isHadTauDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter, int Status){
  const HepMC::GenParticle* TrueTau = TruthTau->genParticle();
  const HepMC::GenParticle* Daughter = TruthDaughter->genParticle();
  
  if ( !TrueTau->end_vertex() ) return false;
  if(isHadTauDaughter(TrueTau, Daughter, Status)) return true;
  
  return false;
  
}



/////////////////////////
/** determines whether ``Daughter'' is a hadronic daughter of the decay of a tau in the  ``TrueTauCollection'', 
    (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1))*/
bool TruthHandler::isHadTauDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter, int Status){
  
  if(!isGoodStatus(Daughter, Status) ) return false;
  for(HepMC::GenEvent::particle_const_iterator MCitr=TrueTauCollection->particles_begin(); MCitr!=TrueTauCollection->particles_end(); ++MCitr ){
    if(!isHadTau(*MCitr)) continue;
    HepMC::GenVertex *vtx = (*MCitr)->end_vertex();
    for ( HepMC::GenVertex::particle_iterator desc = (vtx)->particles_begin(HepMC::descendants);
	  desc != (vtx)->particles_end(HepMC::descendants); ++desc) {
      if( isGoodStatus((*desc),Status) && ((*desc)==Daughter) && ( ((*desc)->barcode()) == (Daughter->barcode()) )) return true;
    }
  }
  return false;
}

/////////////////////////
/** determines whether ``Daughter'' is a hadronic daughter of the decay of a tau in the  ``TrueTauCollection'', 
    (where status specifies intermediate daughter (Status==2) or final state daughter (Status==1)).
    One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this ``TrueTauCollection''.*/
bool TruthHandler::isHadTauDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* TruthDaughter, int Status){
  const HepMC::GenParticle* Daughter = TruthDaughter->genParticle();
  if(!isGoodStatus(Daughter, Status) ) return false;
  
  for (TruthParticleContainer::const_iterator MCitr = TrueTauCollection->begin(); MCitr!=TrueTauCollection->end(); ++MCitr) {
    const HepMC::GenParticle* Gen_particle =(*MCitr)->genParticle();
    
    if(!isHadTau(Gen_particle)) continue;
    if(isHadTauDaughter(Gen_particle, Daughter, Status)) return true;
    
  }
  return false;
}

/////////////////////////
/** determines whether ``Daughter'' is a final stable daughter of the decay of the ``TrueTau'' */
bool TruthHandler::isHadTauFinalDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter){
  return isHadTauDaughter(TrueTau,Daughter,1);
}

/////////////////////////
/** determines whether ``Daughter'' is a final stable daughter of the decay of the ``TrueTau'' */
bool TruthHandler::isHadTauFinalDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter){
  return isHadTauDaughter(TruthTau, TruthDaughter, 1);
}

/////////////////////////
/** determines whether ``Daughter'' is a final stable daughter of the decay of a tau in the ``TrueTauCollection.
    One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection.'' */
bool TruthHandler::isHadTauFinalDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter){
  return isHadTauDaughter(TrueTauCollection, Daughter, 1);
}

/////////////////////////
/** determines whether ``Daughter'' is a final stable daughter of the decay of a tau in the ``TrueTauCollection''.
    One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection. */
bool TruthHandler::isHadTauFinalDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* TruthDaughter){
  return isHadTauDaughter(TrueTauCollection, TruthDaughter, 1);
}

/////////////////////////
/** determines whether ``Daughter'' is an intermediate  daughter of the decay of the ``TrueTau'' */
bool TruthHandler::isHadTauIntermDaughter(const HepMC::GenParticle* TrueTau,const HepMC::GenParticle* Daughter){
  return isHadTauDaughter(TrueTau,Daughter,2);
}

/////////////////////////
/** determines whether ``Daughter'' is an intermediate  daughter of the decay of the ``TrueTau'' */
bool TruthHandler::isHadTauIntermDaughter(const TruthParticle* TruthTau,const TruthParticle* TruthDaughter){
  return isHadTauDaughter(TruthTau, TruthDaughter, 2);
}

/////////////////////////
/** determines whether ``Daughter'' is an intermediate  daughter of the decay of a tau in the ``TrueTauCollection''. 
    One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection. */
bool TruthHandler::isHadTauIntermDaughter(const HepMC::GenEvent* TrueTauCollection,const HepMC::GenParticle* Daughter){
  return isHadTauDaughter(TrueTauCollection, Daughter, 2);
}

/////////////////////////
/** determines whether ``Daughter'' is an intermediate  daughter of the decay of a tau in the ``TrueTauCollection''. 
    One should use getHadronicTruth(const TruthParticleContainer *mcpartTES) to obtain this collection. */
bool TruthHandler::isHadTauIntermDaughter(const TruthParticleContainer *TrueTauCollection, const TruthParticle* TruthDaughter){
  return isHadTauDaughter(TrueTauCollection, TruthDaughter, 2);
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
      // FIXME: cast needed for copying between containers.
      hadDaughters->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return hadDaughters;
}

/////////////////////////
/** returns a collection of the true  final daughters of the true hadronic tau*/
TruthParticleContainer* TruthHandler::getFinalTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES) {
  
  return getTrueHadDaughters(TrueTau, mcpartTES, 1);
}

/////////////////////////
/** returns a collection of the true intermediate daughters of the true hadronic tau*/
TruthParticleContainer* TruthHandler::getIntermTrueHadDaughters(const TruthParticle *TrueTau, const TruthParticleContainer *mcpartTES) {
  
  return getTrueHadDaughters(TrueTau, mcpartTES, 2);
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
      // FIXME: cast needed for copying between containers
      hadTaunProngTES->push_back(const_cast<TruthParticle*> (*mcpartItr));
    }
  }
  return  hadTaunProngTES;
}

/////////////////////////
/** returns a collection of the true hadronics taus which decay to 1 Prong */
TruthParticleContainer* TruthHandler::getHadronic1ProngTruth(const TruthParticleContainer *mcpartTES){
  return getHadronicnProngTruth(mcpartTES, 1);
}

/////////////////////////
/** returns a collection of the true hadronics taus which decay to 3 Prongs */
TruthParticleContainer* TruthHandler::getHadronic3ProngTruth(const TruthParticleContainer *mcpartTES){
  return getHadronicnProngTruth(mcpartTES, 3);
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
/** ATauTau filter: this method determines whether the generated taus passed the Atau tau filter as implemented for dataset 5188 */
bool TruthHandler::AtautauFilter( const TruthParticleContainer *mcpartTES){
  
  const TruthParticle *tau; 
  const TruthParticle *atau;
  const TruthParticle *nutau;
  TruthParticle *anutau;
  TruthParticle *taulep;
  TruthParticle *ataulep;
  
  tau = 0;               // will contain the tau once found by id only 
  atau = 0;
  taulep = 0;            // lepton that has the genvertex in common with the endvertex of tau
  ataulep = 0;
  nutau = 0;             // nutau that has the genvertex in common with the endvertex of tau
  anutau = 0;
  
  /** Iterate over list of MC particles */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();
  
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    
    if( ! isGenerator(*mcpartItr) ) continue; // remove G4 particles
    
    // remove taus which decay to tau. We want the last tau in the decay chain
    if( ! isTauTruth(*mcpartItr)) continue;
    
    if ( (*mcpartItr)->nDecay() == 0 ) {
      cout << "TruthHandler ERROR: This tau has no child! " << m_nochild_counter << endl;  
    }
    
    if (((*mcpartItr)->pdgId() == PDG::tau_minus) && (tau == 0)) { // we have a tau
      tau = (*mcpartItr);
    }
    
    if (((*mcpartItr)->pdgId() == PDG::tau_plus) && (atau == 0)) { // we have a atau
      atau = (*mcpartItr);
    }
    
    std::vector<unsigned int> indices;
    indices.clear();
    // lets look for his neutrino
    if  ( tau != 0 )  {
      tau->hasChild(PDG::nu_tau, indices);
      
      if (indices.size() == 1) {// we found the nu_tau
        nutau = (TruthParticle*)tau->child(indices[0]);
      }
    }
    
    if (( atau != 0 ) && (anutau == 0)) {
      indices.clear();
      atau->hasChild(PDG::anti_nu_tau,indices);
      
      if (indices.size() == 1) {// we found the nu_tau
        anutau = (TruthParticle*)atau->child(indices[0]);
	//printf("found tau_plus anti_nu_tau pdg id %d) %d\n", 0,(int)atau->child(indices[0])->pdgId());
      }
    }
    
    // if tau was already found look for his lepton
    if ((tau != 0) && (taulep == 0)) {
      indices.clear();
      tau->hasChild(PDG::mu_minus, indices);
      
      if (indices.size() == 1) {// we found the mu_minus
	//printf(" found mu_minus pt %lf\n", tau->child(indices[0])->pt());
        taulep = (TruthParticle*)tau->child(indices[0]);
      }
      else {// it must be an electron
        indices.clear();
        tau->hasChild(PDG::e_minus, indices);
        if (indices.size() == 1) {// we found the e_minus
	  //printf(" found e_minus pt %lf\n", tau->child(indices[0])->pt());
          taulep = (TruthParticle*)tau->child(indices[0]);
        }
      }
    }
    
    // if atau was already found look for his lepton
    if ((atau != 0) && (ataulep == 0)) {
      indices.clear();
      atau->hasChild(PDG::mu_plus, indices);
      
      if (indices.size() == 1) {// we found the mu_plus
	//printf(" found mu_plus pt %lf\n", atau->child(indices[0])->pt());
        ataulep = (TruthParticle*)atau->child(indices[0]);
      }
      else {// it must be an electron
        indices.clear();
        atau->hasChild(PDG::e_plus, indices);
        if (indices.size() == 1) {// we found the e_plus
	  //printf(" found e_plus pt %lf\n", (int)atau->child(indices[0])->pt());
          ataulep = (TruthParticle*)atau->child(indices[0]);
        }
      }
    }
    
  }//loop over MC particles
  
  // tau == 0 (atau == 0) mean that we did not find a tau (atau)
  if ( tau == 0 ) {
    cout << "TruthHandler: No tau found !" << endl;
    m_eventsnotau_2tauf++;
    return false;
  }
  if ( atau == 0 ) {
    cout << "TruthHandler: No atau found !" << endl;
    m_eventsnotau_2tauf++;
    return false;
  }
  
  //For tau->had decays modes
  CLHEP::HepLorentzVector mom_tauprod(0.,0.,0.,0.);   // will contain the momentum of the products of the tau decay
  CLHEP::HepLorentzVector mom_atauprod(0.,0.,0.,0.);  //   calculated with mom_tauprod = mom_tau - mom_nutau
  
  // tau -> X + nu, calculate the momentum of X from p_X = p_tau - p_nu
  if ( tau != 0 && nutau != 0) {//tau
    mom_tauprod.setPx(tau->px() - nutau->px());
    mom_tauprod.setPy(tau->py() - nutau->py());
    mom_tauprod.setPz(tau->pz() - nutau->pz());
    mom_tauprod.setE(tau->e()   - nutau->e());
  }
  
  if ( atau != 0 && anutau != 0) {//atau
    mom_atauprod.setPx(atau->px() - anutau->px());
    mom_atauprod.setPy(atau->py() - anutau->py());
    mom_atauprod.setPz(atau->pz() - anutau->pz());
    mom_atauprod.setE(atau->e()   - anutau->e());
  }
  
  // now we can classify the event by looking up what decay products we found in the particle list
  
  // tau -> lepton + nu + anu, atau -> alepton + anu + nu
  if ( ( taulep != 0 ) && ( ataulep != 0 ) ) {//lep-lep
    
    CLHEP::HepLorentzVector mom_taulep(taulep->px(), taulep->py(), taulep->pz(), taulep->e());
    CLHEP::HepLorentzVector mom_ataulep(ataulep->px(), ataulep->py(), ataulep->pz(), ataulep->e());
    //cout << "TruthHandler ll event" << "tau lep " << (int)taulep->pdgId() << " Pt " << taulep->pt() << " Eta " << taulep->eta() << " ataulep " << (int)ataulep->pdgId() << " Pt " << ataulep->pt() << " Eta " << ataulep->eta() << " maxdphi " << fabs(mom_taulep.vect().deltaPhi(mom_ataulep.vect())) << "\n";
    
    m_eventsll_2tauf++;
    if ( ( ( ( taulep->pt() >= m_llPtmine_2tauf ) && ( taulep->pdgId() == PDG::e_minus ))
	   || ( ( taulep->pt() >= m_llPtminmu_2tauf ) && ( taulep->pdgId() == PDG::mu_minus )))
	 && ( ( ( ataulep->pt() >= m_llPtmine_2tauf ) && ( ataulep->pdgId() == PDG::e_plus ) ) 
	      || ( ( ataulep->pt() >= m_llPtminmu_2tauf ) && ( ataulep->pdgId() == PDG::mu_plus ) ) )
	 && ( ( fabs( taulep->eta() ) <= m_EtaRange_2tauf ) )
	 && ( fabs( ataulep->eta() ) <= m_EtaRange_2tauf ) ) {
      if ( fabs(mom_taulep.vect().deltaPhi(mom_ataulep.vect())) < m_maxdphi_2tauf ) {
        m_eventsllacc_2tauf++;
	m_eventsaccepted_2tauf++;
        return true;
      }
    }
  }//lep-lep
  
  // tau -> lepton + nu + anu, atau -> had + anu + nu || cc
  if ( ( taulep != 0 ) && ( ataulep == 0 ) ) {//lep-had
    CLHEP::HepLorentzVector mom_taulep(taulep->px(), taulep->py(), taulep->pz(), taulep->e());
    //cout << "TruthHandler lh event taulep " << (int)taulep->pdgId() << " Pt " << taulep->pt() << " atauprod.pt= " << mom_atauprod.perp() << " eta=" << mom_tauprod.eta() << " maxdphi " << fabs(mom_taulep.vect().deltaPhi(mom_atauprod.vect()))<< "\n";
    
    m_eventslh_2tauf++;
    if ( ( ( ( taulep->pt() >= m_lhPtmine_2tauf ) && ( taulep->pdgId() == PDG::e_minus ) )
	   || ( ( taulep->pt() >= m_lhPtminmu_2tauf ) && ( taulep->pdgId() == PDG::mu_minus)))
	 && ( mom_atauprod.perp() >= m_lhPtminh_2tauf )
	 && ( fabs( taulep->eta() ) <= m_EtaRange_2tauf )
	 && ( fabs( mom_atauprod.pseudoRapidity() ) <= m_EtaRange_2tauf ) ) {
      if ( fabs(mom_taulep.vect().deltaPhi(mom_atauprod.vect())) < m_maxdphi_2tauf ) {
        m_eventslhacc_2tauf++;
        m_eventsaccepted_2tauf++;
        return true;
      }
    }
  }//lep-had
  
  if ( ( ataulep != 0 ) && ( taulep == 0 ) ) {//had-lep
    CLHEP::HepLorentzVector mom_ataulep(ataulep->px(), ataulep->py(), ataulep->pz(), ataulep->e());
    // cout << "TruthHandler lh event ataulep " << (int)ataulep->pdgId() << " Pt " << ataulep->pt() << " tauprod.pt= " << mom_tauprod.perp() << " eta=" << mom_tauprod.eta() << " maxdphi " << fabs(mom_ataulep.vect().deltaPhi(mom_tauprod.vect()))  << "\n";
    
    m_eventslh_2tauf++;
    if ( ( ( ( ataulep->pt() >= m_lhPtmine_2tauf ) && ( ataulep->pdgId() == PDG::e_plus ) )
	   || ( ( ataulep->pt() >= m_lhPtminmu_2tauf ) && ( ataulep->pdgId() == PDG::mu_plus ) ) )
	 && ( mom_tauprod.perp() >= m_lhPtminh_2tauf )
	 && ( fabs( mom_tauprod.pseudoRapidity() ) <= m_EtaRange_2tauf )
	 && ( fabs( ataulep->eta() ) <= m_EtaRange_2tauf ) ) {
      if ( fabs(mom_ataulep.vect().deltaPhi(mom_tauprod.vect())) < m_maxdphi_2tauf ) {
        m_eventslhacc_2tauf++;
        m_eventsaccepted_2tauf++;
        return true;
      }
    }  
  }//had-lep
  
  // tau -> had + anu + nu, atau -> had + nu + anu
  if ( ( taulep == 0 ) && ( ataulep == 0 ) ) {//had-had
    //cout << "TruthHandler hh event tauprod.pt=" << mom_tauprod.perp() << ", atauprod.pt=" << mom_atauprod.perp() << " eta tauprod=" << mom_tauprod.eta() << " eta atauprod=" << mom_atauprod.eta()  << " maxdphi " << fabs(mom_atauprod.vect().deltaPhi(mom_tauprod.vect())) << "\n";
    m_eventshh_2tauf++;
    if ( ( mom_tauprod.perp() >= m_hhPtmin_2tauf )                                  // check pt
	 && ( mom_atauprod.perp() >= m_hhPtmin_2tauf )	 
	 && ( fabs( mom_tauprod.pseudoRapidity() ) <= m_EtaRange_2tauf )            // check eta-range
	 && ( fabs( mom_atauprod.pseudoRapidity() ) <= m_EtaRange_2tauf ) ) {
      if ( fabs(mom_atauprod.vect().deltaPhi(mom_tauprod.vect())) < m_maxdphi_2tauf ) {
        m_eventshhacc_2tauf++;
        m_eventsaccepted_2tauf++;
        return true;
      }
    }
  }//had-had
  
  // if we get here the event did not pass the filter
  //  cout << "TruthHandler event failed\n" << endreq;
  
  m_eventsrefused_2tauf++;
  
  return false;
}

/////////////////////////
/** Set ATauTau filter cut values on pT, eta of e/mu/had tau decay and dphi between tau daughters in lep-lep, lep-had, had-had tau decay modes*/
void TruthHandler::SetAtautauFilter(float Etacut, 
				    float llPtcute, float llPtcutmu, 
				    float lhPtcute, float lhPtcutmu, float lhPtcuth, 
				    float hhPtcut, 
				    float maxdphi) {
  
  //Default cut values can be found at:
  //http://alxr.usatlas.bnl.gov/lxr/source/atlas/Generators/EvgenJobOptions/share/CSC.005188.A3_Ztautau_filter.py
  m_EtaRange_2tauf = Etacut;      //2.7
  // pt cuts for a0 -> tau + atau -> l + l + X 
  m_llPtmine_2tauf = llPtcute;    //13GeV
  m_llPtminmu_2tauf = llPtcutmu;  //8GeV
  
  // pt cuts for a0 -> tau + atau -> l + h + X
  m_lhPtmine_2tauf = lhPtcute;    //13GeV
  m_lhPtminmu_2tauf = lhPtcutmu;  //8GeV
  m_lhPtminh_2tauf = lhPtcuth;    //12GeV
  
  // pt cuts for a0 -> tau + atau -> h + h + X 
  m_hhPtmin_2tauf = hhPtcut;      //12GeV
  
  // dphi cuts for a0 -> tau(x) + atau(y) -> x + y + X, dphi(x, y) < maxdphi
  m_maxdphi_2tauf = maxdphi;      //2.9
  
  /** initialize some simple counters */
  m_eventsll_2tauf = 0;
  m_eventsllacc_2tauf = 0;
  m_eventslh_2tauf = 0;
  m_eventslhacc_2tauf = 0; 
  m_eventshh_2tauf = 0;    
  m_eventshhacc_2tauf = 0; 
  m_eventsaccepted_2tauf = 0;
  m_eventsrefused_2tauf = 0;
  m_eventsnotau_2tauf = 0;
}

/////////////////////////
/** Tau filter: this method determines whether the generated tau passed the tau filter as implemented for dataset 5107 */ 
bool TruthHandler::TauFilter( const TruthParticleContainer *mcpartTES) {
  
  int leptonic = -1;
  int ntau = 0;
  const TruthParticle *tau; 
  CLHEP::HepLorentzVector nutau;
  CLHEP::HepLorentzVector tauvis;
  
  tau = 0;               // will contain the tau once found by id only 
  
  /** Iterate over list of MC particles looking for hadronic taus */
  TruthParticleContainer::const_iterator mcpartItr  = mcpartTES->begin();
  TruthParticleContainer::const_iterator mcpartItrE = mcpartTES->end();
  
  for (; mcpartItr != mcpartItrE; ++mcpartItr) {//loop over MC particles
    
    if( ! isGenerator(*mcpartItr) ) continue; // remove G4 particles
    
    // remove taus which decay to tau. We want the last tau in the decay chain
    if( ! isTauTruth(*mcpartItr)) continue;
    
    if ( (*mcpartItr)->nDecay() == 0 ) {
      cout << "TruthHandler  ERROR: This tau has no child! " << m_nochild_counter << endl;  
    }
    
    // we have a true tau, lets classify its decay
    tau = (*mcpartItr);
    
    // lets classify the type of tau
    for (unsigned int j=0; j< tau->nDecay(); j++) {
      int pdgid = abs((int)tau->pdgDecay(j));
      
      switch(pdgid) {
        case 12: leptonic = 1; break;
        case 14: leptonic = 2; break;
	case 15: leptonic = 3; break;
        default:
	  leptonic = 0; break; //non leptonic
      }
    }
    
    if (leptonic == 3) continue;
    
    // get the neutrino
    nutau = sumDaughterNeutrinos(tau);
    
    //cout << "pT\t\teta\tphi\tlh" << "\n";
    //cout.precision( 4 );
    //cout << nutau.perp() << "\t" << nutau.eta() << "\t" << nutau.phi() << "\t" << leptonic << "\n";
    
    tauvis.setPx(tau->px() - nutau.px());
    tauvis.setPy(tau->py() - nutau.py());
    tauvis.setPz(tau->pz() - nutau.pz());
    tauvis.setE(tau->e()   - nutau.e());
    
    //cout << tauvis.perp() << "\t" << tauvis.eta() << "\t" << tauvis.phi() << "\t" << leptonic << "\n";
    
    switch(leptonic) {
      
      case 1: m_eventse_tauf++;
	if ( tauvis.perp() < m_pTmine_tauf ) continue;
	if ( fabs( tauvis.eta() ) > m_etaMaxe_tauf ) continue;
	ntau++;
	m_eventseacc_tauf++;
	break;
      case 2: m_eventsmu_tauf++;
	if ( tauvis.perp() < m_pTminmu_tauf ) continue;
	if ( fabs( tauvis.eta() ) > m_etaMaxmu_tauf ) continue;
	ntau++;
	m_eventsmuacc_tauf++;
	break;
      case 0: m_eventshad_tauf++;
	if ( tauvis.perp() < m_pTminhad_tauf ) continue;
	if ( fabs( tauvis.eta() ) > m_etaMaxhad_tauf ) continue;
	ntau++;
	m_eventshadacc_tauf++;
    }
  }
  
  if (ntau >= m_NTau_tauf) { // we found the equal or greater than the number of taus required
    m_eventsaccepted_tauf++;
    return true;
  }
  
  m_eventsrefused_tauf++;
  
  return false;
}

/////////////////////////
/** This method adds the momentum of the daughter neutrinos of the tau, it is called recursively */
CLHEP::HepLorentzVector TruthHandler::sumDaughterNeutrinos( const TruthParticle *part ) {
  
  CLHEP::HepLorentzVector nu( 0, 0, 0, 0);
  
  // lets see if the particle
  if ((abs((int)part->pdgId()) == PDG::nu_e) || (abs((int)part->pdgId()) == PDG::nu_mu) || (abs((int)part->pdgId()) == PDG::nu_tau)) {
    nu.setPx(part->px());
    nu.setPy(part->py());
    nu.setPz(part->pz());
    nu.setE(part->e());
    //    cout << "Found a " << (int)part->pdgId() << "\n";
    return nu;
  }
  
  for (unsigned int j=0; j<part->nDecay(); j++) {
    nu += sumDaughterNeutrinos((TruthParticle*)part->child(j));
  }
  
  return nu;
}

/////////////////////////
/** Set Tau filter cut values on no. of taus and pT, eta of e/mu/had tau decay*/
void TruthHandler::SetTauFilter(int ntaumin, 
				float Etacute, float Ptcute, 
				float Etacutmu, float Ptcutmu, 
				float Etacuthad, float Ptcuthad) {
  
  //Default cut values can be found at:
  //http://alxr.usatlas.bnl.gov/lxr/source/atlas/Generators/EvgenJobOptions/share/CSC.005107.pythia_Wtauhad.py
  m_pTmine_tauf = Ptcute;          //1200GeV
  m_etaMaxe_tauf = Etacute;        //2.5
  m_pTminmu_tauf = Ptcutmu;        //1200GeV
  m_etaMaxmu_tauf = Etacutmu;      //2.5
  m_pTminhad_tauf = Ptcuthad;      //12GeV
  m_etaMaxhad_tauf = Etacuthad;    //2.7
  m_NTau_tauf = ntaumin;           //1
  
  m_eventse_tauf = 0;
  m_eventseacc_tauf = 0;
  m_eventsmu_tauf = 0;
  m_eventsmuacc_tauf = 0;
  m_eventshad_tauf = 0;
  m_eventshadacc_tauf = 0;
  m_eventsaccepted_tauf = 0;
  m_eventsrefused_tauf = 0;
  
}

/////////////////////////
/** Print the stats for the filter used, 1 for Atautau filter and 2 for Tau filter */
void TruthHandler::PrintFilterStats(int which_filter) {
  
  switch (which_filter) {
    
    case 1: // Atautau filter
      cout << "TruthHandler: The Atautau Filter stats\n";
      cout << "Total number of events " << m_eventsaccepted_2tauf + m_eventsrefused_2tauf + m_eventsnotau_2tauf << "\n";
      cout << "Passed " << m_eventsaccepted_2tauf << " Failed " << m_eventsrefused_2tauf << "\n";
      cout << "Events with no taus " <<  m_eventsnotau_2tauf << "\n";
      cout << "Lepton Lepton  Total " << m_eventsll_2tauf << " Passed " << m_eventsllacc_2tauf << " Failed " << m_eventsll_2tauf - m_eventsllacc_2tauf << "\n";
      cout << "Lepton Hadron  Total " << m_eventslh_2tauf << " Passed " << m_eventslhacc_2tauf << " Failed " << m_eventslh_2tauf - m_eventslhacc_2tauf << "\n";
      cout << "Hadron Hadron  Total " << m_eventshh_2tauf << " Passed " << m_eventshhacc_2tauf << " Failed " << m_eventshh_2tauf - m_eventshhacc_2tauf << endl;
      break;
      
    case 2: // Tau filter
      cout << "TruthHandler: The Tau Filter stats\n";
      cout << "Total number of events " << m_eventsaccepted_tauf+m_eventsrefused_tauf << "\n";
      cout << "Passed " << m_eventsaccepted_tauf << " Failed " << m_eventsrefused_tauf << "\n";
      cout << "Number of leptonic(e) taus " << m_eventse_tauf << "  Passed " << m_eventseacc_tauf << "\n";
      cout << "Number of leptonic(mu) taus " << m_eventsmu_tauf << "  Passed " << m_eventsmuacc_tauf << "\n";
      cout << "Number of hadronic taus " << m_eventshad_tauf << "  Passed " << m_eventshadacc_tauf << endl;
      break;
    default: ;
  }
}

/////////////////////////
