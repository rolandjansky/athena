/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/VoronoiWeightTool.h"

namespace SortHelper {
  //
  // For Sorting
  //
  struct pt_sort
  {

    inline bool operator() (const TLorentzVector& lhs, const TLorentzVector& rhs)
    {
      return (lhs.Pt() > rhs.Pt());
    }

    inline bool operator() (const TLorentzVector* lhs, const TLorentzVector* rhs)
    {
      return (lhs->Pt() > rhs->Pt());
    }

    inline bool operator() (const xAOD::IParticle& lhs, const xAOD::IParticle& rhs)
    {
      return (lhs.pt() > rhs.pt());
    }

    inline bool operator() (const xAOD::IParticle* lhs, const xAOD::IParticle* rhs)
    {
      return (lhs->pt() > rhs->pt());
    }
  };


  template<typename T>
    T sort_container_pt(T* inCont) {
    T sortedCont(SG::VIEW_ELEMENTS);
    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return sortedCont;
  }

  template<typename T>
    const T sort_container_pt(const T* inCont) {
    ConstDataVector<T> sortedCont(SG::VIEW_ELEMENTS);

    for(auto el : *inCont) sortedCont.push_back( el );
    std::sort(sortedCont.begin(), sortedCont.end(), pt_sort());
    return *sortedCont.asDataVector();
  }

  struct PJcomp {
    bool operator() (const std::pair<fastjet::PseudoJet, std::vector<float> >& lhsp, const std::pair<fastjet::PseudoJet, std::vector<float> >& rhsp)
    {
      fastjet::PseudoJet lhs = lhsp.first;
      fastjet::PseudoJet rhs = rhsp.first;
      return lhs.pt()>rhs.pt();
      //The comparator must be a strict weak ordering. 
    }
  };
  
  
} // end SortHelper


VoronoiWeightTool :: VoronoiWeightTool(const std::string& name) :
  JetConstituentModifierBase(name)
{
  #ifdef ASG_TOOL_ATHENA
    declareInterface<IJetConstituentModifier>(this);
  #endif

  declareProperty("doLCWeights", m_doLC);
  declareProperty("doSpread", m_doSpread);
  declareProperty("nSigma", m_nSigma);
}

VoronoiWeightTool::~VoronoiWeightTool() {}

//Have to define custom comparator for PseudoJets in order to have a map from PJs to anything
//Comparison is fuzzy to account for rounding errors

StatusCode VoronoiWeightTool::process(xAOD::CaloClusterContainer* in_clusters) const
{
  const char* APP_NAME = "VoronoiWeightTool::process()";

  //clusters.clear();
  std::vector<fastjet::PseudoJet> clusters; clusters.reserve(in_clusters->size());

  CaloClusterChangeSignalStateList stateHelperList;
  for(auto clust: *in_clusters){
    //read in clusters as PseudoJets
    //if(m_doLC) stateHelperList.add(clust,xAOD::CaloCluster::State(1)); //default is calibrated but we can make it explicit anyway
    //else stateHelperList.add(clust,xAOD::CaloCluster::State(0));
    if(!m_doLC) ATH_MSG_ERROR(APP_NAME << ": Unclear how we're going to implement EM scaling as an input to JetConstituentModiferBase tools.");
    fastjet::PseudoJet test;
    test = fastjet::PseudoJet(clust->p4());
    if(clust->e() >= 0){ //definition of clusters (\label{clust_def})
	clusters.push_back(test);
	if(m_debug) std::cout << clust->pt() << std::endl;
    }
  }

  std::vector< std::pair< fastjet::PseudoJet, std::vector<float> > > ptvec; //vector of pairs of PJs and their corrected pTs
  if(makeVoronoiClusters(clusters, ptvec) != StatusCode::SUCCESS) ATH_MSG_ERROR(APP_NAME << ": Error in makeVoronoiClusters");
  std::sort(ptvec.begin(), ptvec.end(), SortHelper::PJcomp());

  if(m_doSpread && m_nSigma > 0) ATH_MSG_ERROR(APP_NAME << ": Can't combine spreading with nSigma yet");
  int alg;
  if(m_doSpread && m_nSigma == 0) alg = 3;
  if(!m_doSpread && m_nSigma == 0) alg = 1;
  if(!m_doSpread && m_nSigma > 0) alg = 2;

  size_t i=0;
  SG::AuxElement::Accessor<float> weightAcc("VoronoiWeight"); // Handle for PU weighting here
  for(auto clust : SortHelper::sort_container_pt(in_clusters)){
    float newE;
   //There should be the same number of positive E Clusters in the container as clusters in the ptvec
    bool endContainer = clust->e()<0; //consistent with definition of clusters on L.~\ref{clust_def}
    bool endVec = i>=ptvec.size();
    if(endVec && endContainer){
      newE = 0;  //remove negative energy clusters
    }
    else if(endContainer || endVec){
      ATH_MSG_ERROR(APP_NAME << ": Clusters don't have same number of elements.");
      return StatusCode::FAILURE;
    }
    else{
      //And the clusters should match
      float Containerpt = clust->pt();
      float PJpt = ptvec[i].first.pt();
      if(m_debug){
        std::cout << "Container: " << Containerpt << std::endl;
        std::cout << "Ptvec: " << PJpt << std::endl;
      }
      if (fabs(Containerpt-PJpt) > 0.1){
        if(m_debug) std::cout << fabs(Containerpt-PJpt) << std::endl;
        ATH_MSG_ERROR(APP_NAME << ": Clusters don't match.");
        return StatusCode::FAILURE;
      }
      newE = ptvec[i].second[alg]*cosh(clust->eta());
    }
    float w = newE/clust->e();
    weightAcc(*clust) = w;
    clust->setE(newE);
    i++;
  }

  return StatusCode::SUCCESS;
}

StatusCode VoronoiWeightTool::process(xAOD::IParticleContainer* cont) const {
    xAOD::CaloClusterContainer* clusters = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
    if(clusters) return process(clusters);
    return StatusCode::FAILURE;
}

StatusCode VoronoiWeightTool::makeVoronoiClusters(std::vector<fastjet::PseudoJet>& clusters,std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec) const{
  std::vector<fastjet::PseudoJet> & inputConst = clusters;
  fastjet::Selector jselector = fastjet::SelectorAbsRapRange(0.0,2.1);
  fastjet::JetAlgorithm algo = fastjet::kt_algorithm;
  float jetR = 0.4;
  fastjet::JetDefinition jetDef(algo, jetR,fastjet::E_scheme, fastjet::Best);
  fastjet::AreaDefinition area_def(fastjet::voronoi_area, fastjet::VoronoiAreaSpec(0.9));

  fastjet::ClusterSequenceArea clustSeq(inputConst, jetDef, area_def);
  fastjet::JetMedianBackgroundEstimator bge(jselector,jetDef,area_def);

  bge.set_particles(inputConst);
  std::vector<fastjet::PseudoJet> inclusiveJets = sorted_by_pt(clustSeq.inclusive_jets(0));

  int nsigma = m_nSigma;
  float rho = bge.rho();
  float sigma = bge.sigma();
  for(unsigned int iJet = 0 ; iJet < inclusiveJets.size() ; iJet++){
    fastjet::PseudoJet jet = inclusiveJets[iJet];
    std::vector<fastjet::PseudoJet> constituents = jet.constituents();
    for(auto cons : constituents){
      float pt = cons.pt();
      float area = cons.area();
      float subPt = pt-rho*area;
      //std::cout << "Area: " << area << "; Rho: " << bge.rho() << "; pt: " << constituents[iCons].pt() << "; corrected: " << correctedPt << std::endl;
      //std::cout << "Pt: " << cons.pt() << "; Eta: " << cons.eta() <<"; Phi: " << cons.phi() << std::endl;
      //fastjet::PseudoJet constituentP;
      /*if(correctedPt<0.) continue;
      constituentP.reset_PtYPhiM(correctedPt, constituents[iCons].rap(), constituents[iCons].phi(), constituents[iCons].m());
      clusters_voronoi.push_back(constituentP);*/
      //correctedptmap[cons] = correctedPt;
      float voro0pt = subPt * (subPt > 0);
      float voro1pt = subPt * (subPt > sqrt(area)*sigma*(float)nsigma);
      std::vector<float> algopts;
      algopts.push_back(subPt);
      algopts.push_back(voro0pt);
      algopts.push_back(voro1pt);
      algopts.push_back(0);
      std::pair <fastjet::PseudoJet,std::vector<float> > pjcptpair (cons,algopts);
      correctedptvec.push_back(pjcptpair);
    } // end loop over cons
  } // end loop over jets
  //std::cout << "Size: " << correctedptmap.size() << std::endl;

  if(m_doSpread) spreadPt(correctedptvec);

return StatusCode::SUCCESS;
}

void VoronoiWeightTool::spreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr, float alpha) const{
  //default alpha = 2
  //Set up neighbors within spreadr:
  int clusters = correctedptvec.size();
  std::vector<float> spreadPT(clusters);
  std::vector<bool> isPositive(clusters);
  for(int iCl = 0; iCl < clusters; iCl++){
    spreadPT[iCl] = correctedptvec[iCl].second[0];
    isPositive[iCl] = spreadPT[iCl]>0;
  }

  std::vector<std::vector<std::pair<int,float> > > cluster_drs; //for each cluster, list of nearby positive pT clusters and their distances
  for(int iCl = 0; iCl < clusters; iCl++){
    fastjet::PseudoJet icluster = correctedptvec[iCl].first;
    //float ieta = icluster.eta();
    //float iphi = icluster.phi();
    std::vector<std::pair<int,float> > this_cluster_drs;
    for(int jCl = 0; jCl < clusters; jCl++){
      if(iCl == jCl) continue;
      if(!isPositive[jCl]) continue;
      fastjet::PseudoJet jcluster = correctedptvec[jCl].first;
      //float jeta = jcluster.eta();
      //float jphi = jcluster.phi();
      float dphi = icluster.delta_phi_to(jcluster);
      float deta = icluster.eta() - jcluster.eta(); //fastjet::pseudojet::delta_R(const PseudoJet& other) gives rap-phi distance
      float dr2 = pow(dphi,2) + pow(deta,2);
      if(dr2 > pow(spreadr,2)) continue;
      std::pair<int,float> jdrpair (jCl,dr2);
      this_cluster_drs.push_back(jdrpair);
    }
    cluster_drs.push_back(this_cluster_drs);
  }

  for(int i = 0; i < clusters; i++){
    if(!(spreadPT[i]<0)) continue; //only spread from negative pT clusters
    //find closest positive PT cluster:
    float sumdR2 = 0;
    //iterate over nearby positive pT clusters
    for(size_t j=0; j<cluster_drs[i].size(); j++){
      //cout << "j: " << j << " realid: " << realid << " Eta: " << points[realid][0]<< " Phi: " << points[realid][1] << " Pt:" << spreadPT[realid] << " Dist: " << dists[j] << endl;  // dists[j] = dR^2
      float dr = cluster_drs[i][j].second;
      if(dr>0) sumdR2 += 1./(pow(dr,alpha/2));
    }
    //if at least one neighbor
    if(sumdR2 > 0){
      float spreadPT_orig = spreadPT[i];
      //std::cout << "orig: " << spreadPT_orig << std::endl;
      for(size_t j=0; j<cluster_drs[i].size(); j++){
        float dr = cluster_drs[i][j].second;
        float realid = cluster_drs[i][j].first;
        if(dr>0){
          float weight = (1./pow(dr,alpha/2))/sumdR2;
          //std::cout << dr << "; " << weight << std::endl;
          //std::cout << "Before spreading: " << weight << ";" << weight*spreadPT_orig << ";" << spreadPT[realid] << std::endl;
          if(fabs(weight*spreadPT_orig)>spreadPT[realid]){
            spreadPT[i]+=spreadPT[realid];
            spreadPT[realid]=0;
          }
          else{
            spreadPT[realid]+=weight*spreadPT_orig;
            spreadPT[i]-=weight*spreadPT_orig;
          }
          //std::cout << "After spreading: " << weight << ";" << weight*spreadPT_orig << ";" << spreadPT[realid] << std::endl;
        }
      }
      //std::cout << "final: "  << spreadPT[i] << std::endl;
    }
    //cout << i << ";" << cluster(i,key).Float("correctedPT") << ";" << spreadPT[i]<< endl;
  }

  /*float totalcorrpt=0, totalspreadpt=0;
    for(int i=0; i<clusters; i++){ totalcorrpt+=cluster(i,key).Float("correctedPT"); totalspreadpt+=spreadPT[i];}
    cout << totalcorrpt << ";" << totalspreadpt << endl; //should be the same*/

  for(int iCl = 0; iCl < clusters; iCl++){
    correctedptvec[iCl].second[3] = spreadPT[iCl] * (spreadPT[iCl] > 0);
  }
}
