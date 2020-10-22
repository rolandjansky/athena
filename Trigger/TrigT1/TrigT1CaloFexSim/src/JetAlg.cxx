/*
 *   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloFexSim/JetAlg.h"
#include "TrigT1CaloFexSim/JGTower.h"
#include "TrigT1CaloFexSim/Objects.h"
#include "AthenaBaseComps/AthCheckMacros.h"

std::map<TString, std::vector<std::shared_ptr<JetAlg::L1Jet>>> JetAlg::m_JetMap;
std::map<TString, std::shared_ptr<JetAlg::Seed>>  JetAlg::m_SeedMap;


//function to build up the location of all seeds
StatusCode JetAlg::SeedGrid(const xAOD::JGTowerContainer*towers, TString seedName, bool &m_dumpSeedsEtaPhi){
  
  std::shared_ptr<JetAlg::Seed> seeds = std::make_shared<JetAlg::Seed>();

  std::vector<float> seed_candi_eta;
  unsigned t_size = towers->size();
  
  //find t_maxi=(max eta of all towers)
  float t_maxi=-999;
  for(unsigned i=0; i<t_size;i++){
     const xAOD::JGTower*tower = towers->at(i);
     float t_eta = tower->eta();
     if(t_eta>t_maxi) t_maxi=t_eta;
  }

  //arrange seeds in the order from eta=-4.9 to eta=4.9
  //fill seed_candi_eta with all distinct eta values of seeds, from lowest to highest
  for(float t_eta=t_maxi-0.01; t_eta<t_maxi;){

     float tmp=999;

     for(unsigned i=0;i<t_size;i++){
        const xAOD::JGTower*tower = towers->at(i);
        std::vector<int> SC_indices = tower->SCIndex();
        bool isTile = (fabs(tower->eta())<1.5 && tower->sampling()==1); 
        if(SC_indices.size()==0 && !isTile)  continue;

        // only want EM towers as centres of barrel seeds (overlap in position with hadronic ones). 0 = barrel EM. 1 = barrel had
        if(tower->sampling()==1) continue;

        // only want to seed from FCAL0 towers in forward region. sampling 2 = FCAL0, also have 3, 4
        if(tower->sampling() > 2) continue;
        float candi_eta = tower->eta();           

        if( candi_eta<tmp && seed_candi_eta.size()==0) { tmp=candi_eta; t_eta = candi_eta;}
        else if( candi_eta > t_eta && candi_eta<tmp )  {  tmp = candi_eta; }
     }
     t_eta = tmp;
     seed_candi_eta.push_back(t_eta);
  }

  //arrange seeds in the order from phi=-pi to phi=pi for each possible eta value in seed_candi_eta
  for(unsigned i=0; i<seed_candi_eta.size(); i++){

     (seeds->eta).push_back(seed_candi_eta.at(i));
     std::vector<float> tmp_phi;
     for(float t_phi=-TMath::Pi(); ;){

        float dphi=0;
        //make phi at the edge of towers (the edge that has the lower phi)
        for(unsigned t=0;t<t_size;){
           const xAOD::JGTower*tower = towers->at(t);
           t++;
           std::vector<int> SC_indices = tower->SCIndex();
	   bool isTile = (fabs(tower->eta())<1.5 && tower->sampling()==1);
           if(SC_indices.size()==0 && !isTile) continue;
           float eta = tower->eta();
           if(eta!=seed_candi_eta.at(i)) continue;
           // only want EM towers for central seeds. 0 = barrel EM. 1 = barrel had
           if(tower->sampling()==1) continue;
           // only want to seed from FCAL0 towers in forward region. sampling 2 = FCAL0, also have 3, 4
           if(tower->sampling() > 2) continue;

           dphi = tower->dphi();
           break;
        }
        tmp_phi.push_back(t_phi);
        t_phi+=dphi;

        if(t_phi>TMath::Pi()) break;
     }
     (seeds->phi).push_back(tmp_phi);
  }

  for(unsigned i=0; i<seeds->eta.size(); i++){
    std::vector<float> tmp_et(seeds->phi.at(i).size(), 0.);
    std::vector<float> tmp_noise(seeds->phi.at(i).size(), 0.);
    (seeds->et).push_back(tmp_et);
    (seeds->noise).push_back(tmp_noise);
  }

  if(m_dumpSeedsEtaPhi) {
    ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::INFO,"JetAlg") << "Dumping seed eta phi";
    std::cout << "seed eta phi" << std::endl;
    std::cout << "i_eta" << "\t" << "i_phi" << "\t" << "eta" << "\t" << "phi" << std::endl;
    for(unsigned i=0; i<seeds->eta.size(); i++){
      for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
        float eta=seeds->eta.at(i);
        float phi=seeds->phi.at(i).at(ii);
        std::cout << i << "\t" << ii << "\t" << eta << "\t" << phi << std::endl;
      }
    }
    m_dumpSeedsEtaPhi = false; // only want to do this once
  }

  if(m_SeedMap.find(seedName)==m_SeedMap.end()) m_SeedMap[seedName] = seeds;  
  return StatusCode::SUCCESS;
}

//To find the seeds as local maxima
StatusCode JetAlg::SeedFinding(const xAOD::JGTowerContainer*towers, TString seedname,  float seed_size, float range, std::vector<float> noise, float seed_tower_noise_multiplier,  float seed_min_ET_MeV, bool seed_electrons){
  // get the energy of each seeds which is defined as 2x2 towers in barrel and endcap, and single tower in fcal
  // static MsgStream staticMsg("MyStaticMsgStream"0);
  // static MsgStream staticMsg();
  // staticMsg << "My message" << endmsg
  // msg() << MSG::INFO << "Retrieved tool " << endmsg;
  // ATH_MSG_DEBUG("How can I get ATH_MSG_DEBUG to compile???") 

  std::shared_ptr<JetAlg::Seed> seeds = m_SeedMap[seedname];

  ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "getting " << seedname << " with size " << seed_size << " and local max range " << range;
  int nSeedsAboveThreshold = 0;
  int nSeeds = 0;

  //reset seed et and noise to zero
  for(unsigned i=0; i<seeds->eta.size(); i++){
    for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
      (seeds->et).at(i).at(ii) = 0.;
      (seeds->noise).at(i).at(ii) = 0.;
    }
  }

  // reversed order of tower & seed loops for significant speedup
  for(unsigned t=0; t<towers->size(); t++){
    const xAOD::JGTower*tower = towers->at(t);
    
    float tower_phi   = tower->phi();
    float tower_eta   = tower->eta();
    float tower_et    = tower->et();
    float tower_noise = 0;
    bool isTile = (fabs(tower_eta) <1.5 && tower->sampling()==1);
    bool isHad = tower->sampling()==1;
    // New logic: do not seed on HAD towers
    if(seed_electrons && isHad) continue;

    if(noise.size() < t) {
      ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::ERROR,"JetAlg::SeedFinding") << "the noise vector is smaller (at " << noise.size() << " entries) than the tower number " << t << " that you are attempting to use";
      return StatusCode::FAILURE;
    }
    else {
      tower_noise = noise.at(t);
    }

    bool seedOnTower = false;

    for(unsigned i=0; i<seeds->eta.size(); i++){
      float seed_eta = seeds->eta.at(i);
      if ( (seed_eta < tower_eta-0.5*seed_size) || (seed_eta > tower_eta+0.5*seed_size) )
  	continue;

      for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
  	float seed_phi = seeds->phi.at(i).at(ii);

  	// this doesn't work at counting seeds
  	// nSeeds is actually 4256 (taking every forward tower, rather than just FCAL0)
  	if(inBox(tower_eta,seed_eta,0.1,tower_phi,seed_phi,0.1))
  	  seedOnTower = true;
  	if(!inBox(tower_eta,seed_eta,0.5*seed_size,tower_phi,seed_phi,0.5*seed_size)) 
  	  continue;

  	seeds->noise.at(i).at(ii) += tower_noise;
	bool applyNoise=true;
	if(isTile)applyNoise=false;
	if(applyNoise){
	  if( tower_et > tower_noise * seed_tower_noise_multiplier )
	    seeds->et.at(i).at(ii) += tower_et;
	  //If it's not tile, or fwd then apply noise cut
	}
	else seeds->et.at(i).at(ii) += tower_et; //otherwise for tile seed anyway
      }
    }
    if (seedOnTower) {
      nSeeds++;
    }
  }	
  /*
  for(unsigned i=0; i<seeds->eta.size(); i++){
    for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
      if(seeds->et.at(i).at(ii) < seeds->noise.at(i).at(ii)*seed_total_noise_multiplier){
  	seeds->et.at(i).at(ii) = 0;
      }
      else  {
        ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "found above-noise seed (" << i << ", " << ii << ") at " << seeds->eta.at(i) << ", " << seeds->phi.at(i).at(ii) << ", et = " << seeds->et.at(i).at(ii) << ", noise = " << seeds->noise.at(i).at(ii)*seed_total_noise_multiplier;
  	nSeedsAboveThreshold += 1;
      }
    }
  }*/

  ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "in total, found " << nSeedsAboveThreshold << " seeds above noise thresholds out of " << nSeeds << " seeds";

  // determine whether the seed is with locally maximal energy
  for(unsigned iseed_eta=0; iseed_eta<seeds->eta.size(); iseed_eta++){
    std::vector<bool> tmp_max;
    for(unsigned iseed_phi=0; iseed_phi<seeds->phi.at(iseed_eta).size(); iseed_phi++){
      float et = seeds->et.at(iseed_eta).at(iseed_phi);
      // only seeds with Et > some GeV is available
      if(et < seed_min_ET_MeV) {
        tmp_max.push_back(0);
        continue;
      }
      // eta_n: et higher than all seeds with smaller eta
      // eta_p: et higher than all seeds with larger eta
      // eta_0: et higher than the other seeds along the same eta ring

      // re-define matching range based on eta for electrons
      float match_range = range;
      if(seed_electrons && fabs(seeds->eta.at(iseed_eta)) > 2.5)
	match_range += 0.1;

      bool eta_n=1, eta_p=1, eta_0=1;

      for(unsigned i=iseed_eta+1; ;i++){
        if(i>=seeds->eta.size()) break;
        if(fabs(seeds->eta.at(i)-seeds->eta.at(iseed_eta))>match_range) break;
        for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){

          float dphi = fabs(deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii)));
          if(dphi>match_range) continue;
          if(seeds->et.at(iseed_eta).at(iseed_phi) < seeds->et.at(i).at(ii)){
            eta_p = false;
            break;
          }
        }
      }

      for(int i=iseed_eta-1; ;i--){
        if(i<0) break;
        if(fabs(seeds->eta.at(iseed_eta)-seeds->eta.at(i))>match_range) break;
        for(unsigned ii=0; ii<seeds->phi.at(i).size(); ii++){
          float dphi = fabs(deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(i).at(ii)));
          if(dphi>match_range) continue;
          if(seeds->et.at(iseed_eta).at(iseed_phi) < seeds->et.at(i).at(ii)){
            eta_n = false;
            break;
          }
        }
      }

      for(unsigned ii=0; ii<seeds->phi.at(iseed_eta).size(); ii++){
        if(ii==iseed_phi) continue;
        float dphi = fabs(deltaPhi(seeds->phi.at(iseed_eta).at(iseed_phi),seeds->phi.at(iseed_eta).at(ii)));
        if(dphi>match_range) continue;
        if(seeds->et.at(iseed_eta).at(iseed_phi) < seeds->et.at(iseed_eta).at(ii)){
          eta_0 = false;
          break;
        }
      }
      tmp_max.push_back(eta_n&&eta_p&&eta_0);
    }
    seeds->local_max.push_back(tmp_max);
  }

  // I can now have adjacent seeds with identical energy and temp_max = true;
  // follow firmware implementation: if Deta+Dphi>0: require >, if <0 then >=, if==0 then if Deta>0 then >, else >=
  // NB: Deta = eta(competitor seed) - eta(potential local max seed)

  for(unsigned iseed_eta=0; iseed_eta<seeds->eta.size(); iseed_eta++){
    for(unsigned iseed_phi=0; iseed_phi<seeds->phi.at(iseed_eta).size(); iseed_phi++){

      // re-define matching range based on eta for electrons                                                                                                                                     
      float match_range = range;
      if(seed_electrons && fabs(seeds->eta.at(iseed_eta)) > 2.5)
	match_range += 0.1;

      // only look at local max (or equal max) seeds
      if(!seeds->local_max.at(iseed_eta).at(iseed_phi))
        continue;

      // now loop over all seeds
      bool is_localMax = true;
      for(unsigned iiseed_eta=0; iiseed_eta<seeds->eta.size(); iiseed_eta++){
        for(unsigned iiseed_phi=0; iiseed_phi<seeds->phi.at(iiseed_eta).size(); iiseed_phi++){

          // don't compare with self!
          if(iiseed_eta == iseed_eta && iiseed_phi == iseed_phi)
            continue;

          // care about equal E pairs, the rest have been excluded above
          if( seeds->et.at(iseed_eta).at(iseed_phi) != seeds->et.at(iiseed_eta).at(iiseed_phi) )
            continue;

          // I now have another seed with the same energy. Check it's within range in case of weird coincidence
          float deta = seeds->eta.at(iiseed_eta) - seeds->eta.at(iseed_eta);
          if(fabs(deta) > match_range)
            continue;
          float dphi = deltaPhi(seeds->phi.at(iiseed_eta).at(iiseed_phi), seeds->phi.at(iseed_eta).at(iseed_phi));
	  //This dphi should not be absolute value, as we need the sign below! 
	  //The correct usage in deltaPhi is the following function from root: TVector2::Phi_mpi_pi(phi1,phi2)
          if(fabs(dphi) > match_range)
            continue;
          
          // I now have another seed that this one has to compete with
          ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta, dphi = " << deta << ", " << dphi;
          // follow firmware implementation: if Deta+Dphi>0: require >, if <0 then >=, if==0 then if Deta>0 then >, else >=
          if(deta + dphi > 0) {
            is_localMax = false;
            ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta+dphi>0: need >, not local max";
          }
          else if(deta + dphi < 0) {
            is_localMax = true;
            ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta+dphi<0: only need >=, is local max";
          }
          else {
            ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta+dphi==0: must go further";
            if(deta > 0) {
              is_localMax = false;
              ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta>0: need >, not local max";
            }
            else {
              is_localMax = true;
              ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::SeedFinding") << "deta<0: only need >=, is local max";
            }
          }
          
        }
      }
      seeds->local_max.at(iseed_eta).at(iseed_phi) = is_localMax;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode JetAlg::BuildFatJet(const xAOD::JGTowerContainer towers, TString jetname, float jet_r, std::vector<float> noise, float jet_tower_noise_multiplier,float seed_block_threshold, float jet_min_ET_MeV, float rhoA, float rhoB,float rhoC, bool useNegTowers){


  std::vector<TowerObject::Block> blocks;
  std::vector<std::shared_ptr<JetAlg::L1Jet>> js;
  if(m_JetMap.find(jetname)!=m_JetMap.end())  js =  m_JetMap[jetname];
  else m_JetMap[jetname] = js;
  TowerObject::TowerGrid grid = TowerObject::TowerGrid(towers);

  for(const xAOD::JGTower* seed: towers){
    //if(fabs(seed->eta())>2.4) continue;
    if(!useNegTowers && seed->et() <0) continue; 
    int seedIndex = std::find(towers.begin(), towers.end(), seed) - towers.begin();

    std::vector<int> neighbors = grid.neighbors(*seed, 3, 3);
    float seed_Et = seed->et();

    double block_area(0.0);
    double block_pt(seed_Et);
    double neighbor_pt = 0;

    for(const int& neighborIndex: neighbors){
      const xAOD::JGTower* neighbor = towers.at(neighborIndex);
      block_area += neighbor->deta()*neighbor->dphi();
      neighbor_pt = neighbor->et();

      block_pt += neighbor_pt;
    }

    TowerObject::Block block(block_pt, seed->eta(), seed->phi(), 0.0);
    block.seedIndex(seedIndex);
    block.numEta(3);
    block.numPhi(3);
    block.area(block_area);
    block.numConstituents(neighbors.size());

    blocks.push_back(block);
  }
  std::sort(blocks.rbegin(), blocks.rend());

  for(unsigned b = 0; b < blocks.size(); b++){
    // const xAOD::JGTower* seed = towers.at(blocks[b].seedIndex());
    float block_phi = blocks[b].Phi();
    float block_eta = blocks[b].Eta();
    float pt_block = blocks[b].Pt();

    float rho = -999; 
    std::string FPGA = GFEX_pFPGA(block_eta);
    if(FPGA=="A") rho = rhoA;
    else if (FPGA=="B") rho = rhoB;
    else if (FPGA=="C") rho = rhoC;
    else ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::WARNING,"JetAlg::BuildFatJet") << "NO FPGA FOUND, ETA" << block_eta ; 
    if(pt_block <= seed_block_threshold) continue; 

    int NtowersInJet = 0; 
    float j_Et = 0;

    for(unsigned int t = 0; t < towers.size(); t++){
      const xAOD::JGTower* tower = towers.at(t);
      if(!useNegTowers && tower->et()<0)continue; 
      if(jet_tower_noise_multiplier>0){
	//only apply noise cuts if the noise multiplier is non-zero 
	if(fabs(tower->et()) < noise.at(t) * jet_tower_noise_multiplier)  continue;
      }
      if(!withinRadius(block_eta, tower->eta(), block_phi, tower->phi(), jet_r, /*acceptEqual*/ false) ) continue;
      j_Et += tower->et();
      NtowersInJet++;
    }//looping over all towers
    j_Et -= 69*rho; 
    if(j_Et < jet_min_ET_MeV) continue;
    std::shared_ptr<JetAlg::L1Jet> j = std::make_shared<JetAlg::L1Jet>(block_eta, block_phi, j_Et);
    js.push_back(j);
      
  }//blocks
  
  m_JetMap[jetname] = js;
  
  return StatusCode::SUCCESS;
}

StatusCode JetAlg::BuildJet(const xAOD::JGTowerContainer*towers, TString seedname, TString jetname, float jet_r, std::vector<float> noise, float jet_tower_noise_multiplier, float jet_min_ET_MeV, bool m_saveSeeds){

  std::shared_ptr<JetAlg::Seed> seeds = m_SeedMap[seedname];
  std::vector<std::shared_ptr<JetAlg::L1Jet>> js;
  if(m_JetMap.find(jetname)!=m_JetMap.end())  js = m_JetMap[jetname];
  else m_JetMap[jetname] = js;

  // save seeds
  std::vector<std::shared_ptr<JetAlg::L1Jet>> ss, ss_localMax;
  if(m_saveSeeds) {
    if(m_JetMap.find(seedname)!=m_JetMap.end()) ss = m_JetMap[seedname];
    else m_JetMap[seedname] = ss;
    if(m_JetMap.find(seedname+"_localMax")!=m_JetMap.end()) ss_localMax = m_JetMap[seedname+"_localMax"];
    else m_JetMap[seedname+"_localMax"] = ss_localMax;
  }

  for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){
     for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){
        float eta = seeds->eta.at(eta_ind);
        float phi = seeds->phi.at(eta_ind).at(phi_ind);
        float et = seeds->et.at(eta_ind).at(phi_ind);
        if(m_saveSeeds && et > 0) {
          std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(eta, phi, et);
          ss.push_back(s);
        }
        
        if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
        ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildJet") << "found a local max seed at (eta,phi)=("<<eta<<","<<phi<<")";

        if(m_saveSeeds && et > 0) {
          std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(eta, phi, et);
          ss_localMax.push_back(s);
        }

        float j_et = 0;
	TLorentzVector jvector;
        for(unsigned t=0; t<towers->size(); t++){
           const xAOD::JGTower* tower = towers->at(t);
           if(fabs(tower->et()) < noise.at(t)*jet_tower_noise_multiplier) continue;
           if(!inBox(eta,tower->eta(),jet_r, phi, tower->phi(),jet_r)) continue;
           j_et += tower->et();
	   TLorentzVector towerVector;
	   towerVector.SetPtEtaPhiM(tower->et(),tower->eta(),tower->phi(),0);
	   jvector+=towerVector;
           ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildJet") << "   adding tower at (eta,phi)=("<<tower->eta()<<","<<tower->phi()<<")";
        }
        ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildJet") << " final jet has et = " << j_et << " and mass " << jvector.M();
        if(j_et<jet_min_ET_MeV) continue;
        std::shared_ptr<JetAlg::L1Jet> j = std::make_shared<JetAlg::L1Jet>(eta, phi, j_et);
        js.push_back(j);
     }
  }
  m_JetMap[jetname] = js;
  if(m_saveSeeds) {
    m_JetMap[seedname] = ss;
    m_JetMap[seedname+"_localMax"] = ss_localMax;
  }
  ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildJet") << "built " << js.size() << " jets:";
  for(int i=0; i < int(js.size()); i++) {
    ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildJet") << "  " << i << ": eta = " << (*js[i]).eta << ", phi = " << (*js[i]).phi << ", et = " << (*js[i]).et;
  }
  return StatusCode::SUCCESS;
}


StatusCode JetAlg::BuildRoundJet(const xAOD::JGTowerContainer*towers, TString seedname, TString jetname, float jet_r, std::vector<float> noise, float jet_tower_noise_multiplier,float jet_min_ET_MeV, bool m_saveSeeds){

  std::vector<std::shared_ptr<JetAlg::L1Jet>> js;
  std::shared_ptr<JetAlg::Seed> seeds = m_SeedMap[seedname];
  if(m_JetMap.find(jetname)!=m_JetMap.end())  js = m_JetMap[jetname];
  else m_JetMap[jetname] = js;

  // save seeds
  std::vector<std::shared_ptr<JetAlg::L1Jet>> ss, ss_localMax;
  if(m_saveSeeds) {
    if(m_JetMap.find(seedname)!=m_JetMap.end()) ss = m_JetMap[seedname];
    else m_JetMap[seedname] = ss;
    if(m_JetMap.find(seedname+"_localMax")!=m_JetMap.end()) ss_localMax = m_JetMap[seedname+"_localMax"];
    else m_JetMap[seedname+"_localMax"] = ss_localMax;
  }

  for(unsigned eta_ind=0; eta_ind<seeds->eta.size(); eta_ind++){
     for(unsigned phi_ind=0; phi_ind<seeds->phi.at(eta_ind).size(); phi_ind++){

        float eta = seeds->eta.at(eta_ind);
        float phi = seeds->phi.at(eta_ind).at(phi_ind);
        float et = seeds->et.at(eta_ind).at(phi_ind);

        if(m_saveSeeds && et > 0) {
          std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(eta, phi, et);
          ss.push_back(s);
        }

        if(!seeds->local_max.at(eta_ind).at(phi_ind)) continue;
        float j_et = 0;
	ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildRoundJet") << "BuildRoundJet: found a local max seed at (eta,phi)=("<<eta<<","<<phi<<")";

        if(m_saveSeeds && et > 0) {
          std::shared_ptr<JetAlg::L1Jet> s = std::make_shared<JetAlg::L1Jet>(eta, phi, et);
          ss_localMax.push_back(s);
        }

        for(unsigned t=0; t<towers->size(); t++){
           const xAOD::JGTower* tower = towers->at(t);
	   bool isTile = (fabs(tower->eta()) <1.5 && tower->sampling()==1);
	   bool applyNoise = true;
	   if(isTile) applyNoise=false; 
           if(applyNoise && fabs(tower->et()) < noise.at(t)*jet_tower_noise_multiplier) continue;
           if(!withinRadius(eta, tower->eta(), phi, tower->phi(), jet_r,1)) continue;
           j_et += tower->et();
           ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildRoundJet") << "   adding tower at (eta,phi)=("<<tower->eta()<<","<<tower->phi()<<")";
        }
        ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildRoundJet") << " final jet has et = " << j_et;
        if(j_et<jet_min_ET_MeV) continue;
        std::shared_ptr<JetAlg::L1Jet> j = std::make_shared<JetAlg::L1Jet>(eta, phi, j_et);

        js.push_back(j);
     }
  }
  m_JetMap[jetname] = js;
  if(m_saveSeeds) {
    m_JetMap[seedname] = ss;
    m_JetMap[seedname+"_localMax"] = ss_localMax;
  }
  ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildRoundJet") << "built " << js.size() << " jets:";
  for(int i=0; i < int(js.size()); i++) {
    ATH_REPORT_MESSAGE_WITH_CONTEXT(MSG::DEBUG,"JetAlg::BuildRoundJet") << "  " << i << ": eta = " << (*js[i]).eta << ", phi = " << (*js[i]).phi << ", et = " << (*js[i]).et;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode JetAlg::BuildgBlocksJets(const xAOD::JGTowerContainer* gBs, TString jetname, float rhoA, float rhoB, float rhoC){
  
  std::vector<std::shared_ptr<JetAlg::L1Jet>> gbJets;
  if(m_JetMap.find(jetname)!=m_JetMap.end())  gbJets =  m_JetMap[jetname];
  else m_JetMap[jetname] = gbJets;
  std::vector<float> max_pt(6,0.0); // One entry for each eta slice
  std::vector<float> sec_pt(6,0.0);
  std::vector<int> max_index(6,0);
  std::vector<int> sec_index(6,0);
    
  for(unsigned int b = 0; b < gBs->size(); b++){
    const xAOD::JGTower* block = gBs->at(b);

    const float eta = block->eta();
    const float pt = block->et();

    // fpga_a
    if(eta > -2.5 && eta < -1.25){
      if(pt>max_pt.at(0)){
	max_pt.at(0) = pt;
	max_index.at(0) = b;
      }
      else if(pt>sec_pt.at(0)){
	sec_pt.at(0) = pt;
	sec_index.at(0) = b;
      }
    }
    if(eta > -1.25 && eta < 0){
      if(pt>max_pt.at(1)){
	max_pt.at(1) = pt;
	max_index.at(1) = b;
      }
      else if(pt>sec_pt.at(1)){
	sec_pt.at(1) = pt;
	sec_index.at(1) = b;
      } 
    }
    // fpga_b
    if(eta > 0 && eta < 1.25){
      if(pt>max_pt.at(2)){
	max_pt.at(2) = pt;
	max_index.at(2) = b;
      }
      else if(pt>sec_pt.at(2)){
	sec_pt.at(2) = pt;
	sec_index.at(2) = b;
      } 
    }
    if(eta > 1.25 && eta < 2.5){
      if(pt>max_pt.at(3)){
	max_pt.at(3) = pt;
	max_index.at(3) = b;
      }
      else if(pt>sec_pt.at(3)){
	sec_pt.at(3) = pt;
	sec_index.at(3) = b;
      } 
    }
    // fpga_c
    if(eta >= 2.5){
      if(pt>max_pt.at(4)){
	max_pt.at(4) = pt;
	max_index.at(4) = b;
      }
      else if(pt>sec_pt.at(4)){
	sec_pt.at(4) = pt;
	sec_index.at(4) = b;
      } 
    }
    if(eta <= -2.5){
      if(pt>max_pt.at(5)){
	max_pt.at(5) = pt;
	max_index.at(5) = b;
      }
      else if(pt>sec_pt.at(5)){
	sec_pt.at(5) = pt;
	sec_index.at(5) = b;
      } 
    }
  }

  for(unsigned int i=0; i<max_pt.size(); i++){
    int i_max = max_index.at(i);
    int i_sec = sec_index.at(i);

    float eta_max = gBs->at(i_max)->eta();
    float phi_max = gBs->at(i_max)->phi();
    float rho = -999; 
    std::string FPGA = GFEX_pFPGA(eta_max);
    if(FPGA=="A") rho = rhoA;
    else if (FPGA=="B") rho = rhoB;
    else if (FPGA=="C") rho = rhoC;
    rho *=9; 
    float et_max = gBs->at(i_max)->et() - rho;

    float eta_sec = gBs->at(i_sec)->eta();
    float phi_sec = gBs->at(i_sec)->phi();
    FPGA = GFEX_pFPGA(eta_sec);
    if(FPGA=="A") rho = rhoA;
    else if (FPGA=="B") rho = rhoB;
    else if (FPGA=="C") rho = rhoC;
    rho*=9;
    float et_sec = gBs->at(i_sec)->et() - rho;

    std::shared_ptr<JetAlg::L1Jet> gbJ_max = std::make_shared<JetAlg::L1Jet>(eta_max, phi_max, et_max);
    std::shared_ptr<JetAlg::L1Jet> gbJ_sec = std::make_shared<JetAlg::L1Jet>(eta_sec, phi_sec, et_sec);

    gbJets.push_back(gbJ_max);
    gbJets.push_back(gbJ_sec);
  }
  m_JetMap[jetname] = gbJets;
  return StatusCode::SUCCESS;
}
