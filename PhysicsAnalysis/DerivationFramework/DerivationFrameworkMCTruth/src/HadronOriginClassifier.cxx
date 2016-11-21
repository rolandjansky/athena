/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DerivationFrameworkMCTruth/HadronOriginClassifier.h"


namespace DerivationFramework{



  HadronOriginClassifier::HadronOriginClassifier(const std::string& t, const std::string& n, const IInterface* p): 
    AthAlgTool(t,n,p),
    m_mcName("TruthEvents"),
    m_HadronPtMinCut(0),
    m_HadronEtaMaxCut(0),
    m_DSID(0)
  {

    declareInterface<DerivationFramework::HadronOriginClassifier>(this);

    declareProperty("MCCollectionName",m_mcName="TruthEvents");    
    declareProperty("HadronpTMinCut",m_HadronPtMinCut=5000.); /// MeV
    declareProperty("HadronetaMaxCut",m_HadronEtaMaxCut=2.5);
    declareProperty("DSID",m_DSID=410000);
    
   
  }




  //--------------------------------------------------------------------------
  HadronOriginClassifier::~HadronOriginClassifier(){
    /////
  }
  
  //---------------------------------------------------------------------------
  StatusCode HadronOriginClassifier::initialize() {
    ATH_MSG_INFO("Initialize " );
    if(m_DSID==410003 || m_DSID == 410004 || m_DSID == 410008 || m_DSID == 410232 || m_DSID == 410233){
      m_GenUsed=HerwigPP;
    }
    else if(m_DSID==410006 || m_DSID==410500 || m_DSID==410159 || m_DSID==410160){
      m_GenUsed=Pythia8;
    }
    else if(m_DSID==410186 || m_DSID==410187 || m_DSID==410188 || m_DSID==410189 || m_DSID==410051){
      m_GenUsed=Sherpa;
      
      if(m_DSID==410051){
	m_ttbb=true;
      }
      
    }
    else{
      m_GenUsed=Pythia6;
    }
    
    return StatusCode::SUCCESS;
  }
  
  //---------------------------------------------------------------------------
  StatusCode HadronOriginClassifier::finalize() {
    return StatusCode::SUCCESS;
  }




  std::map<const xAOD::TruthParticle*, DerivationFramework::HadronOriginClassifier::HF_id> HadronOriginClassifier::GetOriginMap(){
        

    


    initMaps();
    
    //--- map partons to hadrons
    std::vector<const xAOD::TruthParticle*> matched_partons;
    std::vector<const xAOD::TruthParticle*> matched_hadrons;    

    matched_partons.clear();
    matched_hadrons.clear();

    while (matched_partons.size()<partonsOrigin.size() && matched_hadrons.size()<mainHadronMap.size()){


      float dR=999.;
      const xAOD::TruthParticle* hadron=0;
      const xAOD::TruthParticle* parton=0;
      for(std::map<const xAOD::TruthParticle*, HF_id>::iterator itr = partonsOrigin.begin(); itr!=partonsOrigin.end(); itr++){


	if(std::find(matched_partons.begin(), matched_partons.end(), (*itr).first) != matched_partons.end()) continue;

	TVector3 v, vtmp;
	v.SetPtEtaPhi((*itr).first->pt(),(*itr).first->eta(),(*itr).first->phi());	
	
	
	for(std::map<const xAOD::TruthParticle*, int>::iterator it = mainHadronMap.begin(); it!=mainHadronMap.end(); it++){

	  if(std::find(matched_hadrons.begin(), matched_hadrons.end(), (*it).first) != matched_hadrons.end()) continue;

	  
	  if((*it).second != abs((*itr).first->pdgId()) ) continue; 
	  vtmp.SetPtEtaPhi((*it).first->pt(),(*it).first->eta(),(*it).first->phi());
      
	  if(vtmp.DeltaR(v) < dR){
	    dR = vtmp.DeltaR(v);
	    hadron = (*it).first;
	    parton = (*itr).first;
	  }
      

	}//loop hadrons
       	
      }//loop partons
     
      
      matched_partons.push_back(parton);
      matched_hadrons.push_back(hadron);
      
      hadronsPartons[ hadron ] = parton;

    }

      
  
    for(std::map<const xAOD::TruthParticle*, int>::iterator it = mainHadronMap.begin(); it!=mainHadronMap.end(); it++){

      const xAOD::TruthParticle* hadron = (*it).first;
      
	if(hadronsPartons.find(hadron)!=hadronsPartons.end()){
	  
	  hadronsOrigin[hadron] = partonsOrigin[ hadronsPartons[hadron] ];
	  
	}

	else{
	  hadronsOrigin[hadron] = extrajet;

	}
    
    }


    return hadronsOrigin;


   
  }

  void HadronOriginClassifier::initMaps(){


    partonsOrigin.clear();
    hadronsOrigin.clear();

    usedHadron.clear();
    mainHadronMap.clear();

    hadronsPartons.clear();

    buildPartonsHadronsMaps();
    
  }


  
  void HadronOriginClassifier::buildPartonsHadronsMaps(){
  
    
    const xAOD::TruthEventContainer* xTruthEventContainer = 0;
    if (evtStore()->retrieve(xTruthEventContainer,m_mcName).isFailure()) {
      ATH_MSG_WARNING("could not retrieve TruthEventContainer " <<m_mcName);
     
    }
    
    for ( const auto* truthevent : *xTruthEventContainer ) {
      
      for(unsigned int i = 0; i < truthevent->nTruthParticles(); i++){
	
	const xAOD::TruthParticle* part = truthevent->truthParticle(i);	
	
	if(part->barcode() >= 200000) break;
	
		
	bool isbquark=false;
	bool iscquark=false;
	bool isHFhadron=false;

	int pdgid = abs(part->pdgId());
	
	//// don't loose time checking all if one found
	if(pdgid == 5 ){
	  isbquark=true;
	}
	else if(pdgid == 4 ){
	  iscquark=true;
	}
	else if(isBHadron(part) || isCHadron(part)){
	  isHFhadron=true;
	}
	else{
	  continue;
	}
	
	
	if(isbquark){
	  bool islooping = isLooping(part);
	  if(isDirectlyFromWTop(part, islooping)){
	    partonsOrigin[ part ] = b_from_W;
	  }
	  else if(isDirectlyFromTop(part, islooping)){
	    partonsOrigin[ part ] = b_from_top;
	  }	  
	  else if(!IsTtBb()&&(IsHerwigPP()||IsSherpa())&&isDirectlyFSR(part,islooping)){
	    partonsOrigin[ part ] = b_FSR;
	  }
	  else if(!IsTtBb()&&IsPythia8()&&isDirectlyFSRPythia8(part,islooping)){
	    partonsOrigin[ part ] = b_FSR;
	  }
	  else if(!IsTtBb()&&IsPythia6()&&isDirectlyFSRPythia6(part,islooping)){
	    partonsOrigin[ part ] = b_FSR;
	  }
	  else if(!IsTtBb()&&IsPythia6()&&isDirectlyMPIPythia6(part, islooping)){
	    partonsOrigin[ part ] = b_MPI;
	  }
	  else if(!IsTtBb()&&IsPythia8()&&isDirectlyMPIPythia8(part, islooping)){
	    partonsOrigin[ part ] = b_MPI;
	  }
	  else if(!IsTtBb()&&IsSherpa()&&isDirectlyMPISherpa(part)){
	    partonsOrigin[ part ] = b_MPI;
	  }
	  
	  
	  
	  
	}
	if(iscquark){
	  bool islooping = isLooping(part);
	  
	  if(isDirectlyFromWTop(part, islooping)){
	    partonsOrigin[ part ] = c_from_W;
	  }
	  else if(isDirectlyFromTop(part, islooping)){
	    partonsOrigin[ part ] = c_from_top;
	  }
	  else if(!IsTtBb()&&(IsHerwigPP()&&IsSherpa())&&isDirectlyFSR(part,islooping)){
	    partonsOrigin[ part ] = c_FSR;
	  }
	  else if(!IsTtBb()&&IsPythia8()&&isDirectlyFSRPythia8(part,islooping)){
	    partonsOrigin[ part ] = c_FSR;
	  }
	  else if(!IsTtBb()&&IsPythia6()&&isDirectlyFSRPythia6(part,islooping)){
	    partonsOrigin[ part ] = c_FSR;
	  }

	  else if(!IsTtBb()&&IsPythia6()&&isDirectlyMPIPythia6(part, islooping)){
	    partonsOrigin[ part ] = c_MPI;
	  }
	  else if(!IsTtBb()&&IsPythia8()&&isDirectlyMPIPythia8(part, islooping)){
	    partonsOrigin[ part ] = c_MPI;
	  }
	  else if(!IsTtBb()&&IsSherpa()&&isDirectlyMPISherpa(part)){
	    partonsOrigin[ part ] = c_MPI;
	  }


	  
	}




	if(isHFhadron){
	  if(!isCHadronFromB(part)){
	    if(usedHadron.count(part)) continue;
	    
	    fillHadronMap(part,part);
	  }
	}
	
	
	
	
      }//loop on particles
    }//loop on truthevent container

   

  }
  

  int HadronOriginClassifier::hadronType(int pdgid) const{
    
    int rest1(abs(pdgid%1000));
    int rest2(abs(pdgid%10000));
    
    if ( rest2 >= 5000 && rest2 < 6000 ) return 5;
    if( rest1 >= 500 && rest1 < 600 ) return 5;
    
    if ( rest2 >= 4000 && rest2 < 5000 ) return 4;
    if( rest1 >= 400 && rest1 < 500 ) return 4;
    
    return 0;
    
  }
  
  
  bool HadronOriginClassifier::isBHadron(const xAOD::TruthParticle* part) const{
    
    if(part->barcode() >= 200000) return false;
    int type = hadronType(part->pdgId());
    if(type == 5)  return true;
    
    return false;
    
  }
  
  
  bool HadronOriginClassifier::isCHadron(const xAOD::TruthParticle* part) const{
    
    if(part->barcode() >= 200000) return false;
    int type = hadronType(part->pdgId());
    if(type == 4)  return true;
    
    return false;
    
  }
  

  bool HadronOriginClassifier::isQuarkFromHadron(const xAOD::TruthParticle* part) const{
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() ) continue; /// protection for sherpa
      int mothertype = hadronType( parent->pdgId() );
      if( 4 == mothertype || 5 == mothertype ){
	return true;
      }
      if(isQuarkFromHadron(parent))return true;
    }
    
    return false;
    
  }
  
  bool HadronOriginClassifier::isCHadronFromB(const xAOD::TruthParticle* part) const{
    
    if(!isCHadron(part)) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() ) continue; /// protection for sherpa
      if( isBHadron(parent) ){
	return true;
      }
      if(isCHadron(parent)){
	if(isCHadronFromB(parent))return true;
      }
    }
    
    return false;
  }
  

  
  void HadronOriginClassifier::fillHadronMap(const xAOD::TruthParticle* mainhad, const xAOD::TruthParticle* ihad, bool decayed){

    usedHadron.insert(ihad); 
    int parent_flav,child_flav;
    bool isFinal = true;


    if(!ihad->nChildren()) return;

    for(unsigned int j=0; j<ihad->nChildren(); ++j){
      const xAOD::TruthParticle* child = ihad->child(j);

      if(decayed){
	fillHadronMap(mainhad,child,true);
	isFinal=false;
      }
      else{
	child_flav = hadronType(child->pdgId());
	if(child_flav!=4 && child_flav!=5) continue;
	parent_flav = hadronType(mainhad->pdgId());
	if(child_flav!=parent_flav) continue;
	fillHadronMap(mainhad,child);
	isFinal=false;
      }
      
    }
    
    if(isFinal && !decayed){

      mainHadronMap[mainhad]=hadronType(mainhad->pdgId());
      
      
      for(unsigned int j=0; j<ihad->nChildren(); ++j){
	const xAOD::TruthParticle* child = ihad->child(j);
	fillHadronMap(mainhad,child,true);
      }
    }
    
    
    
  }
  
  
  
  
  
  
  bool HadronOriginClassifier::passHadronSelection(const xAOD::TruthParticle* part) const{
    
    double pt = part->pt();
    double eta = fabs(part->eta());
    
    if(pt<m_HadronPtMinCut) return false;
    if(eta>m_HadronEtaMaxCut) return false;
    
    return true;
    
  }

  
    
  
  bool HadronOriginClassifier::isFromTop(const xAOD::TruthParticle* part, bool looping) const{
    
    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    return isDirectlyFromTop(initpart, looping);
    
  }
  
  bool HadronOriginClassifier::isDirectlyFromTop(const xAOD::TruthParticle* part, bool looping) const{
    
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs( parent->pdgId() ) == 6 ) return true;
    }
    
    return false;
  }
  

  bool HadronOriginClassifier::isFromWTop(const xAOD::TruthParticle* part, bool looping) const{
    
    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    return isDirectlyFromWTop(initpart, looping);
    
  }
  
  
  bool HadronOriginClassifier::isDirectlyFromWTop(const xAOD::TruthParticle * part, bool looping) const{
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs( parent->pdgId() ) == 24 ){
	if( isFromTop(parent, looping) ) return true;
      }
    }
    
    return false;
    
    
  }




  bool HadronOriginClassifier::isDirectlyFromGluonQuark(const xAOD::TruthParticle* part, bool looping) const{
    
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs( parent->pdgId() ) == 21 || abs(parent->pdgId())<5 ) return true;
    }
    
    return false;
  }
  
  bool HadronOriginClassifier::isFromGluonQuark(const xAOD::TruthParticle* part, bool looping) const{
    
    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    return isDirectlyFromGluonQuark(initpart, looping);
    
  }
  

  
  bool HadronOriginClassifier::isDirectlyFSRPythia6(const xAOD::TruthParticle * part, bool looping) const{
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if(abs( parent->pdgId() ) == 24){
	if(abs(part->pdgId())==4){
	  //trick to get at least 50% of PowhegPythia c from FSR
	  if(part->pdgId()==-(parent->pdgId())/6){
	    if( isFromGluonQuark(parent, looping) ) return true;
	  }
	}
	else{
	  if( isFromGluonQuark(parent, looping) ) return true;
	}
      }
    }
    
    return false;
    
    
  }




  bool HadronOriginClassifier::isDirectlyFSR(const xAOD::TruthParticle * part, bool looping) const{
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs(parent->pdgId())== 21 || abs(parent->pdgId())==22 ){
	if( isFromQuarkTop( parent,looping ) ) return true;
      }

    }
    
    return false;
    
    
  }

  bool HadronOriginClassifier::isDirectlyFromQuarkTop(const xAOD::TruthParticle* part, bool looping) const{
    
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs(parent->pdgId())<6 ) {

	if(isFromTop(parent,looping)){
	  return true;
	}
	else if(isFromWTop(parent,looping)){
	  return true;
	}
	
      }
    }
    
    return false;
  }
  
  bool HadronOriginClassifier::isFromQuarkTop(const xAOD::TruthParticle* part, bool looping) const{
    
    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    return isDirectlyFromQuarkTop(initpart, looping);
    
  }



  
  bool HadronOriginClassifier::isDirectlyFSRPythia8(const xAOD::TruthParticle * part, bool looping) const{
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs(parent->pdgId())== 21 || abs(parent->pdgId())==22 ){
	if( isFromQuarkTopPythia8( parent,looping ) ) return true;
      }

    }
    
    return false;
    
    
  }

  bool HadronOriginClassifier::isDirectlyFromQuarkTopPythia8(const xAOD::TruthParticle* part, bool looping) const{
    
    
    
    if(!part->nParents()) return false;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs(parent->pdgId())<6 ) {
	
	if(isFromWTop(parent,looping)){
	  return true;
	}
	
      }
    }
    
    return false;
  }
  
  bool HadronOriginClassifier::isFromQuarkTopPythia8(const xAOD::TruthParticle* part, bool looping) const{
    
    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    return isDirectlyFromQuarkTopPythia8(initpart, looping);
    
  }






  bool HadronOriginClassifier::isDirectlyMPIPythia6(const xAOD::TruthParticle * part, bool looping) const{
    
    if(!part->nParents()) return false;
      
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping ) continue; /// protection for sherpa
      if( abs(parent->pdgId())== 2212 && part->status()!=3) return true;

    }
    
    return false;
    
  }
  
  
  
  bool HadronOriginClassifier::isDirectlyMPIPythia8(const xAOD::TruthParticle * part, bool looping) const{
    

    const xAOD::TruthParticle* initpart = findInitial(part, looping);
    
    if( initpart->status()>30 && initpart->status()<40) return true;

    
    return false;
    
  }

  bool HadronOriginClassifier::isDirectlyMPISherpa(const xAOD::TruthParticle * part) const{
    
    if(!part->hasProdVtx()) return false;
    
    const xAOD::TruthVertex* vertex = part->prodVtx();
    if(vertex->id()==2) return true;

    
    return false;
    
  }


  
  bool HadronOriginClassifier::isLooping(const xAOD::TruthParticle* part, std::set<const xAOD::TruthParticle*> init_part) const{
    
    if(!part->nParents()) return false;
    
    init_part.insert(part);
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( init_part.find(parent) != init_part.end() ) return true;
      if( isLooping(parent, init_part) ) return true;
    }
    
    return false;
    
  }
  
  
  
  const xAOD::TruthParticle*  HadronOriginClassifier::findInitial(const xAOD::TruthParticle* part, bool looping) const{
    
    
    if(!part->nParents()) return part;
    
    for(unsigned int i=0; i<part->nParents(); ++i){
      const xAOD::TruthParticle* parent = part->parent(i);
      if( part->barcode() < parent->barcode() &&  looping) continue; /// protection for sherpa
      if( part->pdgId() == parent->pdgId() ){
	return findInitial(parent, looping);
      }
    }
    
    return part;
    
  }
  
  



  const xAOD::TruthParticle* HadronOriginClassifier::partonToHadron(const xAOD::TruthParticle* parton){

    const xAOD::TruthParticle* hadron(NULL);
    
    TVector3 v, vtmp;
    v.SetPtEtaPhi(parton->pt(),parton->eta(),parton->phi());
    float dR=999.;
    

    
    for(std::map<const xAOD::TruthParticle*,int>::iterator it = mainHadronMap.begin(); it != mainHadronMap.end(); it++){
      
      
      //      const xAOD::TruthParticle* fhadron=(*it).first;
      
      if((*it).second != abs(parton->pdgId()) ) continue; // || ((fhadron->pt()/parton->pt())>1)
      vtmp.SetPtEtaPhi((*it).first->pt(),(*it).first->eta(),(*it).first->phi());
      
      if(vtmp.DeltaR(v) < dR){
	dR = vtmp.DeltaR(v);
	hadron = (*it).first;
	
      }
      
    }
    
    return hadron;
  }
  
  
}//namespace







