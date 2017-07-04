/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetRecTools/VoronoiWeightTool.h"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/Selector.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/tools/JetMedianBackgroundEstimator.hh"
#include <fastjet/tools/Subtractor.hh>

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

  declareProperty("doSpread", m_doSpread);
  declareProperty("nSigma", m_nSigma);

  // Option to disregard cPFOs in the weight calculation
  declareProperty("IgnoreChargedPFO", m_ignoreChargedPFOs);
}

VoronoiWeightTool::~VoronoiWeightTool() {}

StatusCode VoronoiWeightTool::initialize() {

  if(m_inputType==xAOD::Type::ParticleFlow) {
    if(m_ignoreChargedPFOs && m_applyToChargedPFO) {
      ATH_MSG_ERROR("Incompatible configuration: setting both IgnoreChargedPFO and ApplyToChargedPFO to true"
		    <<  "will set all cPFOs to zero");
      return StatusCode::FAILURE;
    }
    if(!m_applyToNeutralPFO) {
      ATH_MSG_ERROR("Incompatible configuration: ApplyToNeutralPFO=False -- what kind of pileup do you wish to suppress?");
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

//Have to define custom comparator for PseudoJets in order to have a map from PJs to anything
//Comparison is fuzzy to account for rounding errors

StatusCode VoronoiWeightTool::process_impl(xAOD::IParticleContainer* particlesin) const
{
  std::vector<fastjet::PseudoJet> particles; particles.reserve(particlesin->size());

  for(const auto& part: *particlesin){
    // Only use positive E
    bool accept = part->e() > 1e-9;
    // For PFlow we would only want to apply the correction to neutral PFOs,
    // because charged hadron subtraction handles the charged PFOs.
    // However, we might still want to use the cPFOs for the min pt calculation
    if(m_inputType==xAOD::Type::ParticleFlow && m_ignoreChargedPFOs) {
      // The auto loop returns an ElementProxy, so we need to dereference/reference
      const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>(&*part);
      accept = fabs(pfo->charge())>1e-9;
    }
    if(accept) {
      particles.push_back( fastjet::PseudoJet(part->p4()) );
      ATH_MSG_VERBOSE( "Accepted particle with pt " << part->pt() );
    }
  }

  std::vector< std::pair< fastjet::PseudoJet, std::vector<float> > > ptvec; //vector of pairs of PJs and their corrected pTs
  if(makeVoronoiParticles(particles, ptvec) != StatusCode::SUCCESS) ATH_MSG_ERROR("Error in makeVoronoiParticles");
  std::sort(ptvec.begin(), ptvec.end(), SortHelper::PJcomp());

  if(m_doSpread && m_nSigma > 0) ATH_MSG_ERROR("Can't combine spreading with nSigma yet");
  int alg;
  if(m_doSpread && m_nSigma == 0) alg = 3;
  if(!m_doSpread && m_nSigma == 0) alg = 1;
  if(!m_doSpread && m_nSigma > 0) alg = 2;

  size_t i=0;
  SG::AuxElement::Accessor<float> weightAcc("VoronoiWeight"); // Handle for PU weighting here
  for(const auto& part : SortHelper::sort_container_pt(particlesin)){
    // Skip the check on charged PFOs if needed
    bool accept(true);
    if(m_inputType==xAOD::Type::ParticleFlow && m_ignoreChargedPFOs) {
      // The auto loop returns an ElementProxy, so we need to dereference/reference
      const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>(&*part);
      accept = fabs(pfo->charge())>1e-9;
    }

    if(accept) {
      float newE(0.);
      //There should be the same number of positive E Particles in the container as particles in the ptvec
      bool endContainer = part->e()<=0;
      bool endVec = i>=ptvec.size();
      if(endVec && endContainer){
	newE = 0;  //remove negative energy particles
      }
      else if(endContainer || endVec){
	ATH_MSG_ERROR("Filtered particle list doesn't have same number of elements as the list returned by FastJet.");
	return StatusCode::FAILURE;
      }
      else{
	//And the particles should match
	float Containerpt = part->pt();
	float PJpt = ptvec[i].first.pt();
	ATH_MSG_VERBOSE( "Container: " << Containerpt );
	ATH_MSG_VERBOSE( "Ptvec: " << PJpt );
	if (fabs(Containerpt-PJpt) > 0.1){
	  ATH_MSG_VERBOSE( fabs(Containerpt-PJpt) );
	  ATH_MSG_ERROR("Particle pt's don't match.");
	  return StatusCode::FAILURE;
	}
	newE = ptvec[i].second[alg]*cosh(part->eta());
      }
      float w = newE/part->e();
      weightAcc(*part) = w;
      ATH_CHECK(setEnergyPt(part,newE,part->pt()*w));
      i++;
    }
  }
  return StatusCode::SUCCESS;
 }

StatusCode VoronoiWeightTool::makeVoronoiParticles(std::vector<fastjet::PseudoJet>& particles,std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec) const{
  std::vector<fastjet::PseudoJet> & inputConst = particles;
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
    for(const auto& cons : constituents){
      float pt = cons.pt();
      float area = cons.area();
      float subPt = pt-rho*area;
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

  if(m_doSpread) spreadPt(correctedptvec);

  return StatusCode::SUCCESS;
}

void VoronoiWeightTool::spreadPt(std::vector< std::pair< fastjet::PseudoJet,std::vector<float> > >& correctedptvec, float spreadr, float alpha) const{
  //default alpha = 2
  //Set up neighbors within spreadr:
  int Nparticles = correctedptvec.size();
  std::vector<float> spreadPT(Nparticles);
  std::vector<bool> isPositive(Nparticles);
  for(int iPart = 0; iPart < Nparticles; iPart++){
    spreadPT[iPart] = correctedptvec[iPart].second[0];
    isPositive[iPart] = spreadPT[iPart]>0;
  }

  std::vector<std::vector<std::pair<int,float> > > particle_drs; //for each particle, list of nearby positive pT particles and their distances
  for(int iPart = 0; iPart < Nparticles; iPart++){
    fastjet::PseudoJet iparticle = correctedptvec[iPart].first;
    std::vector<std::pair<int,float> > this_particle_drs;
    for(int jCl = 0; jCl < Nparticles; jCl++){
      if(iPart == jCl) continue;
      if(!isPositive[jCl]) continue;
      fastjet::PseudoJet jparticle = correctedptvec[jCl].first;
      float dphi = iparticle.delta_phi_to(jparticle);
      float deta = iparticle.eta() - jparticle.eta(); //fastjet::pseudojet::delta_R(const PseudoJet& other) gives rap-phi distance
      float dr2 = pow(dphi,2) + pow(deta,2);
      if(dr2 > pow(spreadr,2)) continue;
      std::pair<int,float> jdrpair (jCl,dr2);
      this_particle_drs.push_back(jdrpair);
    }
    particle_drs.push_back(this_particle_drs);
  }

  for(int i = 0; i < Nparticles; i++){
    if(!(spreadPT[i]<0)) continue; //only spread from negative pT particles
    //find closest positive PT particle:
    float sumdR2 = 0;
    //iterate over nearby positive pT particles
    for(size_t j=0; j<particle_drs[i].size(); j++){
      float dr = particle_drs[i][j].second;
      if(dr>0) sumdR2 += 1./(pow(dr,alpha/2));
    }
    //if at least one neighbor
    if(sumdR2 > 0){
      float spreadPT_orig = spreadPT[i];
      for(size_t j=0; j<particle_drs[i].size(); j++){
        float dr = particle_drs[i][j].second;
        float realid = particle_drs[i][j].first;
        if(dr>0){
          float weight = (1./pow(dr,alpha/2))/sumdR2;
          if(fabs(weight*spreadPT_orig)>spreadPT[realid]){
            spreadPT[i]+=spreadPT[realid];
            spreadPT[realid]=0;
          }
          else{
            spreadPT[realid]+=weight*spreadPT_orig;
            spreadPT[i]-=weight*spreadPT_orig;
          }
        }
      }
    }
  }

  for(int iPart = 0; iPart < Nparticles; iPart++){
    correctedptvec[iPart].second[3] = spreadPT[iPart] * (spreadPT[iPart] > 0);
  }
}
