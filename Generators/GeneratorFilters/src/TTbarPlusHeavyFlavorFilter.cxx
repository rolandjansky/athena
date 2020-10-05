/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeneratorFilters/TTbarPlusHeavyFlavorFilter.h"
#include "AtlasHepMC/Relatives.h"
#include "GaudiKernel/MsgStream.h"

//--------------------------------------------------------------------------
TTbarPlusHeavyFlavorFilter::TTbarPlusHeavyFlavorFilter(const std::string& fname, 
						 ISvcLocator* pSvcLocator)
  : GenFilter(fname,pSvcLocator)
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
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode TTbarPlusHeavyFlavorFilter::filterFinalize() {
  ATH_MSG_INFO( m_nPass  << " Events out of " << m_nPass+m_nFail << " passed the filter"  );
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
  for (itr = events()->begin(); itr!=events()->end(); ++itr) {

    const HepMC::GenEvent* genEvt = (*itr);

    // Loop over all truth particles in the event
    // ===========================================
    for(auto  part: *genEvt) {

      if(HepMC::barcode(part) > 200000) break;

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


bool TTbarPlusHeavyFlavorFilter::passBSelection(HepMC::ConstGenParticlePtr part) const{

  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = std::abs(p4.eta());

  if(pt<m_bPtMinCut) return false;
  if(eta>m_bEtaMaxCut) return false;

  return true;
  
}

bool TTbarPlusHeavyFlavorFilter::passCSelection(HepMC::ConstGenParticlePtr part) const{

  const HepMC::FourVector& p4 = part->momentum();
  double pt = p4.perp();
  double eta = std::abs(p4.eta());

  if(pt<m_cPtMinCut) return false;
  if(eta>m_cEtaMaxCut) return false;

  return true;
}


int TTbarPlusHeavyFlavorFilter::hadronType(int pdgid) const{

  int rest1(std::abs(pdgid%1000));
  int rest2(std::abs(pdgid%10000));

  if ( rest2 >= 5000 && rest2 < 6000 ) return 5;
  if( rest1 >= 500 && rest1 < 600 ) return 5;

  if ( rest2 >= 4000 && rest2 < 5000 ) return 4;
  if( rest1 >= 400 && rest1 < 500 ) return 4;

  return 0;

}


bool TTbarPlusHeavyFlavorFilter::isBHadron(HepMC::ConstGenParticlePtr part) const{

  if(HepMC::barcode(part) >= 200000) return false;
  int type = hadronType(part->pdg_id());
  if(type == 5)  return true;

  return false;

}


bool TTbarPlusHeavyFlavorFilter::isCHadron(HepMC::ConstGenParticlePtr part) const{

  if(HepMC::barcode(part) >= 200000) return false;
  int type = hadronType(part->pdg_id());
  if(type == 4)  return true;

  return false;

}



bool TTbarPlusHeavyFlavorFilter::isInitialHadron(HepMC::ConstGenParticlePtr part) const{

  auto prod = part->production_vertex();
  if(prod){
    int type = hadronType(part->pdg_id());
#ifdef HEPMC3
    for(auto firstParent: prod->particles_in()){
      if( HepMC::barcode(part) < HepMC::barcode(firstParent) ) continue; /// protection for sherpa
      int mothertype = hadronType( firstParent->pdg_id() );
      if( mothertype == type ){
	return false;
      }
    }
#else
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      int mothertype = hadronType( (*firstParent)->pdg_id() );
      if( mothertype == type ){
	return false;
      }
    }
#endif

  }

  return true;
}


bool TTbarPlusHeavyFlavorFilter::isFinalHadron(HepMC::ConstGenParticlePtr part) const{

  auto end = part->end_vertex();
  if(end){
    int type = hadronType(part->pdg_id());
#ifdef HEPMC3
    for(auto firstChild: end->particles_in()){
      if( HepMC::barcode(part) > HepMC::barcode(firstChild) ) continue; /// protection for sherpa
      int childtype = hadronType( firstChild->pdg_id() );
      if( childtype == type ){
	return false;
      }
    }
#else
    HepMC::GenVertex::particle_iterator firstChild = end->particles_begin(HepMC::children);
    HepMC::GenVertex::particle_iterator endChild = end->particles_end(HepMC::children);
    for(;firstChild!=endChild; ++firstChild){
      if( part->barcode() > (*firstChild)->barcode() ) continue; /// protection for sherpa
      int childtype = hadronType( (*firstChild)->pdg_id() );
      if( childtype == type ){
	return false;
      }
    }
#endif

  }

  return true;

}



bool TTbarPlusHeavyFlavorFilter::isQuarkFromHadron(HepMC::ConstGenParticlePtr part) const{

  auto prod = part->production_vertex();
  if(prod){
#ifdef HEPMC3
    for(auto firstParent: HepMC::ancestor_particles(prod)){
      if( HepMC::barcode(part) < HepMC::barcode(firstParent) ) continue; /// protection for sherpa
      int mothertype = hadronType( firstParent->pdg_id() );
      if( 4 == mothertype || 5 == mothertype ){
	return true;
      }
    }
#else	  
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      int mothertype = hadronType( (*firstParent)->pdg_id() );
      if( 4 == mothertype || 5 == mothertype ){
	return true;
      }
    }
#endif

  }

  return false;

}

bool TTbarPlusHeavyFlavorFilter::isCHadronFromB(HepMC::ConstGenParticlePtr part) const{

  if(!isCHadron(part)) return false;

  auto prod = part->production_vertex();
  if(prod){
#ifdef HEPMC3
    for(auto firstParent:HepMC::ancestor_particles(prod)){
      if( HepMC::barcode(part) < HepMC::barcode(firstParent) ) continue; /// protection for sherpa
      if( isBHadron(firstParent) ){
	return true;
      }
    }
#else
    HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::ancestors);
    HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::ancestors);
    for(;firstParent!=endParent; ++firstParent){
      if( part->barcode() < (*firstParent)->barcode() ) continue; /// protection for sherpa
      if( isBHadron(*firstParent) ){
	return true;
      }
    }
#endif

  }

  return false;
}


bool TTbarPlusHeavyFlavorFilter::isLooping(HepMC::ConstGenParticlePtr part, std::set<HepMC::ConstGenParticlePtr> init_part) const{

  auto prod = part->production_vertex();

  if(!prod) return false;

  init_part.insert(part);

#ifdef HEPMC3
  for(auto firstParent: prod->particles_in()){
    if( init_part.find(firstParent) != init_part.end() ) return true;
    if( isLooping(firstParent, init_part) ) return true;
  }
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( init_part.find(*firstParent) != init_part.end() ) return true;
    if( isLooping(*firstParent, init_part) ) return true;
  }
#endif

  return false;

}



HepMC::ConstGenParticlePtr  TTbarPlusHeavyFlavorFilter::findInitial(HepMC::ConstGenParticlePtr part, bool looping) const{

  auto prod = part->production_vertex();

  if(!prod) return part;

#ifdef HEPMC3
  for(auto firstParent: prod->particles_in()){
    if( HepMC::barcode(part) < HepMC::barcode(firstParent) &&  looping) continue; /// protection for sherpa
    if( part->pdg_id() == firstParent->pdg_id() ){
      return findInitial(firstParent, looping);
    }
  }
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() &&  looping) continue; /// protection for sherpa
    if( part->pdg_id() == (*firstParent)->pdg_id() ){
      return findInitial(*firstParent, looping);
    }
  }
#endif
   
  return part;

}

bool TTbarPlusHeavyFlavorFilter::isFromTop(HepMC::ConstGenParticlePtr part, bool looping) const{

  auto initpart = findInitial(part, looping);
  return isDirectlyFromTop(initpart, looping);
 
}

bool TTbarPlusHeavyFlavorFilter::isDirectlyFromTop(HepMC::ConstGenParticlePtr part, bool looping) const{

 auto prod = part->production_vertex();

  if(!prod) return false;

#ifdef HEPMC3
  for( auto firstParent: prod->particles_in()){
    if( HepMC::barcode(part) < HepMC::barcode(firstParent) &&  looping ) continue; /// protection for sherpa
    if( std::abs( firstParent->pdg_id() ) == 6 ) return true;
  }
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() &&  looping ) continue; /// protection for sherpa
    if( std::abs( (*firstParent)->pdg_id() ) == 6 ) return true;
  }
#endif 
   
  return false;
}



bool TTbarPlusHeavyFlavorFilter::isDirectlyFromWTop(HepMC::ConstGenParticlePtr part, bool looping) const{

  auto prod = part->production_vertex();

  if(!prod) return false;

#ifdef HEPMC3
  for(auto firstParent: prod->particles_in()){
    if( HepMC::barcode(part) < HepMC::barcode(firstParent) && looping  ) continue; /// protection for sherpa
    if( std::abs( firstParent->pdg_id() ) == 24 ){
      if( isFromTop(firstParent, looping) ) return true;
    }
  }
#else
  HepMC::GenVertex::particle_iterator firstParent = prod->particles_begin(HepMC::parents);
  HepMC::GenVertex::particle_iterator endParent = prod->particles_end(HepMC::parents);
  for(;firstParent!=endParent; ++firstParent){
    if( part->barcode() < (*firstParent)->barcode() && looping  ) continue; /// protection for sherpa
    if( std::abs( (*firstParent)->pdg_id() ) == 24 ){
      if( isFromTop(*firstParent, looping) ) return true;
    }
  }
#endif   
   
  return false;


}


