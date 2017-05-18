/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CalcTopPartonHistory.cxx 800464 2017-03-13 18:06:24Z tpelzer $
#include "TopPartons/CalcTopPartonHistory.h"
#include "TopPartons/PartonHistory.h"
#include "TopConfiguration/TopConfig.h"
#include "TopPartons/CalcTtbarPartonHistory.h"
#include "xAODTruth/TruthVertex.h"

namespace top{
  
  CalcTopPartonHistory::CalcTopPartonHistory( const std::string& name ) :
    asg::AsgTool( name ),
    m_config(nullptr)
  {    
    declareProperty( "config" , m_config );
  }
  
  ///Store the four-momentum of the post-FSR top or anti-top found using statusCodes
  ///This would only work if there is at most one "true" top of each charge (i.e. won't work for SS tops or 4 tops)
  ///This code was adapted from the 7TeV parton-level differential ttbar routine: https://svnweb.cern.ch/trac/atlasphys-top/browser/Physics/Top/Software/MCvalidation/Rivet/Rivet2.X/trunk/routines/ATLAS_2014_I1304289/ATLAS_2014_I1304289.cc
  bool CalcTopPartonHistory::topAfterFSR_SC(const xAOD::TruthParticleContainer* truthParticles, int start, TLorentzVector& top_afterFSR_SC_p4){
    
    /// Step1: create vectors of particles of each status codes
    // Vectors to hold any status=3 (anti-)top quarks (Pythia 6)
    std::vector<const xAOD::TruthParticle*> v_status3_top;
    // Vectors to hold any status=155 (anti-)top quarks (Herwig 6)
    std::vector<const xAOD::TruthParticle*> v_status155_top;
    // Vectors to hold any status=11 (anti-)top quarks for Herwig++
    std::vector<const xAOD::TruthParticle*> v_status11_top;
    // Vectors to hold any status=22 (anti-)top quarks
    std::vector<const xAOD::TruthParticle*> v_statusOther_top;
    
    /// Step2: loop on the container of particles and fill the above created vectors
    for (const xAOD::TruthParticle* particle : *truthParticles) {
      if (particle->pdgId() != start) continue; // only keep particles of a given pdgID (e.g. 6 or -6)
      
      if (particle->status()==3) {
        v_status3_top.push_back(particle);
      }
      else if (particle->status()==155) {
        v_status155_top.push_back(particle);
      }
      else if (particle->status()==11) {// for Herwig++: take only the tops that decay into Wb!!!
        if (!particle->hasDecayVtx()) continue;
        const xAOD::TruthVertex* vertex = particle->decayVtx();
        if (vertex == nullptr) continue;
        if (vertex->nOutgoingParticles() == 2) v_status11_top.push_back(particle);
      }
      else {
        v_statusOther_top.push_back(particle);
      }
    }
    
    /// Step3: for some of the statuscodes, keep only the last of the vector
    // If there are more than 1 status 3 tops or anti-tops, only keep the last one put into the vector
    if(v_status3_top.size()>1) {
      v_status3_top = std::vector<const xAOD::TruthParticle*>(v_status3_top.end() - 1, v_status3_top.end());
    }
    // If there are more than 1 status 11 tops or anti-tops, only keep the last one put into the vector
    if(v_status11_top.size()>1) {
      v_status11_top = std::vector<const xAOD::TruthParticle*>(v_status11_top.end() - 1, v_status11_top.end());
    }
    // Rach: check for Pythia 8 as well
    // If there are more than 1 status 3 tops or anti-tops, only keep the last one put into the vector
    if(v_statusOther_top.size()>1) {
      v_statusOther_top = std::vector<const xAOD::TruthParticle*>(v_statusOther_top.end() - 1, v_statusOther_top.end());
    }
    
    /// Step4: chose which statuscode to take according to what is found in the event
    const xAOD::TruthParticle* top = nullptr;
    // If there are status 3 tops and no status 155 tops this is probably a Pythia event, so used the status 3s.
    if(v_status3_top.size() == 1 && v_status155_top.size() == 0) {
      top = v_status3_top[0];
    }
    // If there are status 155 tops this must be a Herwig event, so use the status 155s.
    if( v_status155_top.size() == 1 && v_status3_top.size() == 0) {
      top = v_status155_top[0];
    }
      // If there are tops with other status this must be a Pythia8 event, so use them.
    if( v_statusOther_top.size() == 1 && v_status155_top.size() == 0 && v_status3_top.size()==0) {
      top = v_statusOther_top[0];
    }
    // If there are status 155 tops this must be a Herwig event, so use the status 155s.
    if( v_status11_top.size() == 1 && v_status3_top.size() == 0) {
      top = v_status11_top[0];
    }
    
    /// Step5: if everything worked, set the 4-vector to its value and return true
    if (top != nullptr) {
      top_afterFSR_SC_p4 = top->p4();
      return true;
    }
    return false;
  }
  
  // for b coming from W'->tb  
  bool CalcTopPartonHistory::b( const xAOD::TruthParticleContainer* truthParticles, 
  				TLorentzVector& b_beforeFSR, TLorentzVector& b_afterFSR){
    for (const xAOD::TruthParticle* particle : *truthParticles) {
    
    	if (fabs(particle->pdgId()) != 5)	continue; 
	   
         bool skipit(false);		     
         for (size_t i=0; i< particle->nParents(); i++ ) {
     	     const xAOD::TruthParticle* parent = particle->parent(i);
     	     if (parent && (parent->isTop() || fabs(parent->pdgId())==5) ){
     		     skipit=true; 
     		     break;
     	     }//if
        }//for
	
        if( skipit) continue;
	
     	b_beforeFSR = particle->p4();
     	b_afterFSR = findAfterFSR(particle)->p4();
	
        return true;
     }
     

     return false;
  }
  
    
  bool CalcTopPartonHistory::topWb( const xAOD::TruthParticleContainer* truthParticles, 
  				    int start, TLorentzVector& t_beforeFSR_p4, TLorentzVector& t_afterFSR_p4, TLorentzVector& W_p4, 
				    TLorentzVector& b_p4, TLorentzVector& Wdecay1_p4, 
				    int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId){
    
    bool hasT		    = false;
    bool hasW		    = false;
    bool hasB		    = false;
    bool hasWdecayProd1     = false;
    bool hasWdecayProd2     = false;
    
    for (const xAOD::TruthParticle* particle : *truthParticles) {
    
    	if (particle->pdgId() != start)	continue;    
	
	if (hasParticleIdenticalParent(particle)) continue; // kepping only top before FSR			
	
	t_beforeFSR_p4 = particle->p4(); // top before FSR
	hasT = true;		
			
	// demanding the last tops after FSR
	particle = findAfterFSR(particle);
	t_afterFSR_p4 = particle->p4(); // top after FSR
    	
	for (size_t k=0; k < particle->nChildren(); k++) {
		const xAOD::TruthParticle* topChildren = particle->child(k);	
		
		if (fabs(topChildren->pdgId()) == 24){				
			
			W_p4 = topChildren->p4();  // W boson after FSR
			hasW = true;
		
			// demanding the last W after FSR 
			topChildren = findAfterFSR(topChildren);		
			
			for (size_t q = 0; q < topChildren->nChildren(); ++q) {
				const xAOD::TruthParticle* WChildren = topChildren->child(q);
				if (fabs(WChildren->pdgId())<17){
					if (WChildren->pdgId()>0){
						Wdecay1_p4 = WChildren->p4();
						Wdecay1_pdgId = WChildren->pdgId();
						hasWdecayProd1 = true;
					}else{
						Wdecay2_p4 = WChildren->p4();
						Wdecay2_pdgId = WChildren->pdgId();
						hasWdecayProd2 = true;
				 	}//else
				}//if						
			}//for

		} else if (fabs(topChildren->pdgId()) == 5) {					
			b_p4 = topChildren->p4();
			hasB = true;
		} //else if		
	
	} //for (size_t k=0; k < particle->nChildren(); k++)
	if (hasT && hasW && hasB && hasWdecayProd1 && hasWdecayProd2)	return true;
    } //for (const xAOD::TruthParticle* particle : *truthParticles)
    
    return false;		
				  
  }

  bool CalcTopPartonHistory::Wlv( const xAOD::TruthParticleContainer* truthParticles, 
  				    TLorentzVector& W_p4, 
				    TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, 
				    TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId){

    bool hasW		    = false;
    bool hasWdecayProd1     = false;
    bool hasWdecayProd2     = false;
    
    for (const xAOD::TruthParticle* particle : *truthParticles) {
    
	
	if (fabs(particle->pdgId()) != 24)	continue;    
    	//std::cout << "PDGID: " << particle->pdgId() << std::endl;
					
	// demanding the last W after FSR
	particle = findAfterFSR(particle);
	W_p4 = particle->p4();  // W boson after FSR
	hasW = true;
		
	for (size_t k=0; k < particle->nChildren(); k++) {
		const xAOD::TruthParticle* WChildren = particle->child(k);	
		if (fabs(WChildren->pdgId())<17){
			if (WChildren->pdgId()%2==1){ // charged lepton in the Wlv case
				Wdecay1_p4 = WChildren->p4();
				Wdecay1_pdgId = WChildren->pdgId();
				hasWdecayProd1 = true;
			}else{// neutral lepton in the Wlv case
				Wdecay2_p4 = WChildren->p4();
				Wdecay2_pdgId = WChildren->pdgId();
				hasWdecayProd2 = true;
		 	}//else
		}//if
				
	} //for (size_t k=0; k < particle->nChildren(); k++) 
	
    	if (hasW && hasWdecayProd1 && hasWdecayProd2)	return true;	
    } //for (const xAOD::TruthParticle* particle : *truthParticles)
						
    
    return false;		
				  
  }

  
  const xAOD::TruthParticle* CalcTopPartonHistory::findAfterFSR(const xAOD::TruthParticle* particle) {
     bool isAfterFSR(false);
     const int particle_ID = particle->pdgId();
     int forLoop  = 0;
     while(!isAfterFSR){

     	     forLoop  = 0;
     	     for (size_t j=0; j< particle->nChildren(); j++ ) {
     		     const xAOD::TruthParticle* tmp_children = particle->child(j);		     
     		     if (tmp_children && tmp_children->pdgId()==particle_ID){
     			     particle = particle->child(j);
     			     forLoop++;
     			     break;
     		     }//if
     	     }//for
     	     
     	     if (forLoop == 0)       isAfterFSR = true; 	     
     }//while
     return particle;
  }

  bool CalcTopPartonHistory::hasParticleIdenticalParent(const xAOD::TruthParticle* particle) {
     bool skipit(false);		     
     for (size_t i=0; i< particle->nParents(); i++ ) {
     	     const xAOD::TruthParticle* parent = particle->parent(i);
     	     if (parent && parent->pdgId()==particle->pdgId()){
     		     skipit=true; 
     		     break;
     	     }//if
     }//for
     return skipit;
  }
      
  StatusCode CalcTopPartonHistory::execute()
  {
     // Get the Truth Particles
     const xAOD::TruthParticleContainer* truthParticles(nullptr);
     ATH_CHECK( evtStore()->retrieve( truthParticles , m_config->sgKeyMCParticle() ) );
    
     // Create the partonHistory xAOD object
     xAOD::PartonHistoryAuxContainer* partonAuxCont = new xAOD::PartonHistoryAuxContainer{};    
     xAOD::PartonHistoryContainer* partonCont = new xAOD::PartonHistoryContainer{};
     partonCont->setStore( partonAuxCont );
  
     xAOD::PartonHistory* partonHistory = new xAOD::PartonHistory{};
     partonCont->push_back( partonHistory );
          
     // Save to StoreGate / TStore
     std::string outputSGKey = m_config->sgKeyTopPartonHistory();
     std::string outputSGKeyAux = outputSGKey + "Aux.";
      
     xAOD::TReturnCode save = evtStore()->tds()->record( partonCont , outputSGKey );
     xAOD::TReturnCode saveAux = evtStore()->tds()->record( partonAuxCont , outputSGKeyAux );
     if( !save || !saveAux ){
       return StatusCode::FAILURE;
     }      
    
     return StatusCode::SUCCESS;
  } 
  
  void CalcTopPartonHistory::fillEtaBranch( xAOD::PartonHistory* partonHistory, std::string branchName, TLorentzVector &tlv){
     if (tlv.CosTheta()==1.) partonHistory->auxdecor< float >( branchName ) = 1000.;
     else if (tlv.CosTheta()==-1.) partonHistory->auxdecor< float >( branchName ) = 1000.;
     else partonHistory->auxdecor< float >( branchName ) = tlv.Eta();
     return;
     
     
  }
}
