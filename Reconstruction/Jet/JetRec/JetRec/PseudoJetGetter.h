/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/// Implementations of concrete input-to-PseudoJet conversions
/// Separated from PseudoJetAlgorithm for readability
///
/// IParticle is the generic type that is normally assumed.
/// Special treatment is needed for two cases:
///   * EMTopo clusters will be converted at the uncalibrated
///     cluster scale -- this is mostly obsolete in offline reco
///     but used by trigger for the moment
///   * ParticleFlowObjects that are charged will be filtered
///     out if they are not matched to the primary vertex

#ifndef PSEUDOJETGETTER_H
#define PSEUDOJETGETTER_H

#include "fastjet/PseudoJet.hh"
#include "xAODBase/IParticle.h"

#ifndef GENERATIONBASE
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODPFlow/PFO.h"
#include "xAODPFlow/FlowElement.h"
#endif

namespace PseudoJetGetter {

  struct IParticleRejecter{
    bool null{false};
    bool negativeE{false};
    bool skipNegativeEnergy{false};
  
    IParticleRejecter(bool skip): skipNegativeEnergy(skip){
    }

    bool operator()(const xAOD::IParticle* ip) {
      null = (ip == 0);
      negativeE = skipNegativeEnergy && ip->e() <= 0.0;
      return (null || negativeE);
    }
  };


  std::vector<fastjet::PseudoJet> 
  IParticlesToPJs(const xAOD::IParticleContainer& ips, bool skipNegativeEnergy) {

    IParticleRejecter rejecter(skipNegativeEnergy);

    std::vector<fastjet::PseudoJet> vpj;
    int index = -1;

    // loop over the input iparticles, select and  convert to pseudojets
    for(const xAOD::IParticle* ip: ips) {
      ++index;
      if(rejecter(ip)){continue;}
    
      // Create a Pseudojet with the momentum of the selected IParticles.
      fastjet::PseudoJet psj(ip->p4());

      // user index is used to identify the xAOD object used for the PseudoJet
      psj.set_user_index(index); 
      vpj.push_back(psj);

    }
    return vpj;
  }

  //**********************************************************************

#ifndef GENERATIONBASE

  struct EMTopoRejecter{
    const xAOD::CaloCluster* cluster{0};

    bool operator()(const xAOD::IParticle* ip){
      cluster = dynamic_cast<const xAOD::CaloCluster*>(ip);
      return cluster == 0;  // reject if not a cluster
    }
  };

  std::vector<fastjet::PseudoJet> 
  EMToposToPJs(const xAOD::IParticleContainer& ips, bool skipNegativeEnergy) {

    // helper objects for selecting iparticles to be converted to pseudojets
    IParticleRejecter ipRejecter(skipNegativeEnergy);
    EMTopoRejecter emRejecter;
  
    std::vector<fastjet::PseudoJet> vpj;
    int index = -1;

    // loop over iparticles, select and  convert to pseudojets

    for(const xAOD::IParticle* ip: ips) {
      ++index;
      if(ipRejecter(ip) or emRejecter(ip)){continue;}
    
      // Create a Pseudojet with the momentum of the cluster.
      fastjet::PseudoJet 
	psj(emRejecter.cluster->p4(xAOD::CaloCluster::UNCALIBRATED));
    
      // user index is used to identify the xAOD object used for the PseudoJet
      psj.set_user_index(index); 
      vpj.push_back(psj);
    }
    return vpj;
  }

  //**********************************************************************

  struct PFlowRejecter{

    bool skipNegativeEnergy{false};
    bool useChargedPFOs{true};
    bool useNeutralPFOs{true};
    bool useChargedPV{true};
    bool useChargedPUsideband{false};

    PFlowRejecter(bool skip, bool useCharged, bool useNeutral, bool chargedPV, bool chargedPUsideband): 
      skipNegativeEnergy(skip),
      useChargedPFOs(useCharged),
      useNeutralPFOs(useNeutral),
      useChargedPV(chargedPV),
      useChargedPUsideband(chargedPUsideband){
    }

    bool operator()(const xAOD::IParticle* ip){
      
      bool reject = false;

      if(ip->type() == xAOD::Type::FlowElement){
        const xAOD::FlowElement* pfo = dynamic_cast<const xAOD::FlowElement*>(ip);

	reject = (skipNegativeEnergy && pfo->e()<FLT_MIN);

        if( pfo->isCharged() ){
	  if(!useChargedPFOs) reject = true;
          const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
	  if(useChargedPV && !PVMatchedAcc(*pfo)) reject = true;
          const static SG::AuxElement::ConstAccessor<char> PUsidebandMatchedAcc("matchedToPUsideband");
          if (useChargedPUsideband && !PUsidebandMatchedAcc(*pfo)) reject = true;
        }
	else{
          if(!useNeutralPFOs) reject = true;
        }
        return reject;
      }
    
      const xAOD::PFO* pfo = dynamic_cast<const xAOD::PFO*>(ip);
    
      // keep charged PFOs with energy==0 because for MET TST with PFlow, 
      // there may be high pt 
      // charged PFOs that receive a weight of 0 due to being in dense 
      // showers, but need to be present for overlap removal, because they 
      // don't retain these weights when added to the TST      

      reject = (skipNegativeEnergy && pfo->e()<FLT_MIN);

      if( pfo->isCharged() ) {
	if(!useChargedPFOs) reject = true;
	const static SG::AuxElement::ConstAccessor<char> PVMatchedAcc("matchedToPV");
	if(useChargedPV && !PVMatchedAcc(*pfo)) reject = true;
	const static SG::AuxElement::ConstAccessor<char> PUsidebandMatchedAcc("matchedToPUsideband");
	if (useChargedPUsideband && !PUsidebandMatchedAcc(*pfo)) reject = true;
      }
      else{
	if(!useNeutralPFOs) reject = true;
      }
      return reject;
    }
  };


  std::vector<fastjet::PseudoJet> 
  PFlowsToPJs(const xAOD::IParticleContainer& ips, bool skipNegativeEnergy, bool useChargedPFOs, bool useNeutralPFOs, bool useChargedPV, bool useChargedPUsideband) {

    PFlowRejecter rejecter(skipNegativeEnergy, useChargedPFOs, useNeutralPFOs, useChargedPV, useChargedPUsideband);
    std::vector<fastjet::PseudoJet> vpj;
    int index = -1;

    // loop over the input iparticles, select and  convert to pseudojets

    for(const xAOD::IParticle* ip: ips) {
      ++index;
      if(rejecter(ip)){continue;}
    
      // Create a PSeudojet with the momentum of the selected IParticles.
      fastjet::PseudoJet psj(ip->p4());

      // user index is used to identify the xAOD object used for the PSeudoJet
      psj.set_user_index(index);

      vpj.push_back(psj);
    }
    return vpj;
  }
#endif

}

#endif
