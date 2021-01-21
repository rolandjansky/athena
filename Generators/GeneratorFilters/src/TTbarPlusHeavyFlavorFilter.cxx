/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarPlusHeavyFlavorFilter.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"

//--------------------------------------------------------------------------
TTbarPlusHeavyFlavorFilter::TTbarPlusHeavyFlavorFilter(const std::string& fname, 
						 ISvcLocator* pSvcLocator)
  : GenFilter(fname,pSvcLocator),
    m_log(MsgStream(messageService(), name()))
 {

  declareProperty("UseFinalStateHadrons",m_useFinalStateHadrons=true);
  declareProperty("SelectB",m_selectB=false);
  declareProperty("SelectC",m_selectC=false);
  declareProperty("SelectL",m_selectL=false);
  declareProperty("BpTMinCut",m_bPtMinCut=5000.); /// MeV
  declareProperty("BetaMaxCut",m_bEtaMaxCut=3.);
  declareProperty("CpTMinCut",m_cPtMinCut=5000.); /// MeV
  declareProperty("CetaMaxCut",m_cEtaMaxCut=3.);
  declareProperty("BMultiplicityCut",m_bMultiCut=1); /// >=
  declareProperty("CMultiplicityCut",m_cMultiCut=1); /// >=
  declareProperty("ExcludeBFromTTbar",m_excludeBFromTop=true);  /// use IS quark
  declareProperty("ExcludeCFromTTbar",m_excludeCFromTop=true); /// use IS quark

}

//--------------------------------------------------------------------------
 TTbarPlusHeavyFlavorFilter::~TTbarPlusHeavyFlavorFilter(){
   /////
}

//---------------------------------------------------------------------------
StatusCode TTbarPlusHeavyFlavorFilter::filterInitialize() {
  m_log << MSG::INFO << "Initialized" << endreq;
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarPlusHeavyFlavorFilter::filterFinalize() {
  m_log << MSG::INFO << m_nPass  << " Events out of " << m_nPass+m_nFail << " passed the filter" << endreq;
  return StatusCode::SUCCESS;
}


//---------------------------------------------------------------------------
StatusCode TTbarPlusHeavyFlavorFilter::filterEvent() {
//---------------------------------------------------------------------------

  bool pass = false;

  int nB=0;
  int nC=0;

  int nBtop=0;
  int nCtop=0;

  McEventCollection::const_iterator itr;
  for (itr = events_const()->begin(); itr!=events_const()->end(); ++itr) {

    const HepMC::GenEvent* genEvt = (*itr);

    // Loop over all truth particles in the event
    // ===========================================
    for(HepMC::GenEvent::particle_const_iterator pitr=genEvt->particles_begin();pitr!=genEvt->particles_end(); ++pitr ) {

      const HepMC::GenParticle* part = (*pitr);

      if(part->barcode() > 200000) break;

      bool isbquark=false;
      bool iscquark=false;

      bool isbhadron=false;
      bool ischadron=false;

      int pdgid = abs(part->pdg_id());

      //// don't loose time checking all if one found
      if(pdgid == 5 ){
	isbquark=true;
      }
      else if(pdgid == 4 ){
	iscquark=true;
      }
      else if ( isBHadron(part) ){
	isbhadron=true;
      }
      else if ( isCHadron(part) ){
	ischadron=true;
      }
      else{
	continue;
      }

 
      if( (isbquark || isbhadron) && !passBSelection(part) ) continue;
      if( (iscquark || ischadron) && !passCSelection(part) ) continue;

      if(isbhadron || ischadron){
	if(!isInitialHadron(part) && !m_useFinalStateHadrons) continue;
	if(!isFinalHadron(part) && m_useFinalStateHadrons) continue;
      }

      if(m_excludeBFromTop && isbquark){
	bool islooping = isLooping(part);
	if(isDirectlyFromTop(part, islooping))++nBtop;
	if(isDirectlyFromWTop(part, islooping))++nBtop;
      }
      if(m_excludeCFromTop && iscquark){
	bool islooping = isLooping(part);
	if(isDirectlyFromTop(part, islooping))++nCtop;
	if(isDirectlyFromWTop(part, islooping))++nCtop;
      }

      bool ischadronfromb = isCHadronFromB(part);
      
      if(isbhadron) ++nB;
      if(ischadron && !ischadronfromb) ++nC;
      

    } /// loop on particles

    // Coverity dislikes this break as it means the loop is poitless, commentted out in line with other filters
    //break;

  } /// loop on events (only one at evgen - no PU)
  

  int nAddB=nB;
  if(m_excludeBFromTop){
    nAddB-=nBtop;
  }

  int nAddC=nC;
  if(m_excludeCFromTop){
    nAddC-=nCtop;
  }

  int flavortype=0;

  if(nAddC>=m_cMultiCut){
    flavortype=4;
  }

  if(nAddB>=m_bMultiCut){
    flavortype=5;
  }

  if(m_selectB && 5 == flavortype) pass=true;
  if(m_selectC && 4 == flavortype) pass=true;
  if(m_selectL && 0 == flavortype) pass=true;

  setFilterPassed(pass);
  return StatusCode::SUCCESS;

}


bool TTbarPlusHeavyFlavorFilter::passBSelection(const HepMC::GenParticle* part) const{

  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = fabs(p4.eta());

  if(pt<m_bPtMinCut) return false;
  if(eta>m_bEtaMaxCut) return false;

  return true;
  
}

bool TTbarPlusHeavyFlavorFilter::passCSelection(const HepMC::GenParticle* part) const{

  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = fabs(p4.eta());

  if(pt<m_cPtMinCut) return false;
  if(eta>m_cEtaMaxCut) return false;

  return true;
}


int TTbarPlusHeavyFlavorFilter::hadronType(int pdgid) const{

  int rest1(abs(pdgid%1000));
  int rest2(abs(pdgid%10000));

  if ( rest2 >= 5000 && rest2 < 6000 ) return 5;
  if( rest1 >= 500 && rest1 < 600 ) return 5;

  if ( rest2 >= 4000 && rest2 < 5000 ) return 4;
  if( rest1 >= 400 && rest1 < 500 ) return 4;

  return 0;

}


bool TTbarPlusHeavyFlavorFilter::isBHadron(const HepMC::GenParticle* part) const{

  if(part->barcode() >= 200000) return false;
  int type = hadronType(part->pdg_id());
  if(type == 5)  return true;

  return false;

}


bool TTbarPlusHeavyFlavorFilter::isCHadron(const HepMC::GenParticle* part) const{

  if(part->barcode() >= 200000) return false;
  int type = hadronType(part->pdg_id());
  if(type == 4)  return true;

  return false;

}



bool TTbarPlusHeavyFlavorFilter::isInitialHadron(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* prod = part->production_vertex();
  if(prod){
    int type = hadronType(part->pdg_id());
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      int mothertype = hadronType( (*firstParent)->pdg_id() );
      if( mothertype == type ){
	return false;
      }
    }

  }

  return true;
}


bool TTbarPlusHeavyFlavorFilter::isFinalHadron(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* end = part->end_vertex();
  if(end){
    int type = hadronType(part->pdg_id());
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(;firstChild!=endChild; ++firstChild){
      if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
      int childtype = hadronType( (*firstChild)->pdg_id() );
      if( childtype == type ){
	return false;
      }
    }

  }

  return true;

}



bool TTbarPlusHeavyFlavorFilter::isQuarkFromHadron(const HepMC::GenParticle* part) const{

  HepMC::GenVertex* prod = part->production_vertex();
  if(prod){
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      int mothertype = hadronType( (*firstParent)->pdg_id() );
      if( 4 == mothertype || 5 == mothertype ){
	return true;
      }
    }

  }

  return false;

}

bool TTbarPlusHeavyFlavorFilter::isCHadronFromB(const HepMC::GenParticle* part) const{

  if(!isCHadron(part)) return false;

  HepMC::GenVertex* prod = part->production_vertex();
  if(prod){
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      if( isBHadron(*firstParent) ){
	return true;
      }
    }

  }

  return false;
}


bool TTbarPlusHeavyFlavorFilter::isLooping(const HepMC::GenParticle* part, std::set<const HepMC::GenParticle*> init_part) const{

  HepMC::GenVertex* prod = part->production_vertex();

  if(!prod) return false;

  init_part.insert(part);

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( init_part.find(*firstParent) != init_part.end() ) return true;
    if( isLooping(*firstParent, init_part) ) return true;
  }

  return false;

}



const HepMC::GenParticle*  TTbarPlusHeavyFlavorFilter::findInitial(const HepMC::GenParticle* part, bool looping) const{

  HepMC::GenVertex* prod = part->production_vertex();

  if(!prod) return part;

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() &&  looping) continue; /// protection for sherpa
    if( part->pdg_id() == (*firstParent)->pdg_id() ){
      return findInitial(*firstParent, looping);
    }
  }
   
  return part;

}

bool TTbarPlusHeavyFlavorFilter::isFromTop(const HepMC::GenParticle* part, bool looping) const{

  const HepMC::GenParticle* initpart = findInitial(part, looping);
  return isDirectlyFromTop(initpart, looping);
 
}

bool TTbarPlusHeavyFlavorFilter::isDirectlyFromTop(const HepMC::GenParticle* part, bool looping) const{

 HepMC::GenVertex* prod = part->production_vertex();

  if(!prod) return false;

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() &&  looping ) continue; /// protection for sherpa
    if( abs( (*firstParent)->pdg_id() ) == 6 ) return true;
  }
   
  return false;
}



bool TTbarPlusHeavyFlavorFilter::isDirectlyFromWTop(const HepMC::GenParticle* part, bool looping) const{

  HepMC::GenVertex* prod = part->production_vertex();

  if(!prod) return false;

  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() && looping  ) continue; /// protection for sherpa
    if( abs( (*firstParent)->pdg_id() ) == 24 ){
      if( isFromTop(*firstParent, looping) ) return true;
    }
  }
   
  return false;


}


