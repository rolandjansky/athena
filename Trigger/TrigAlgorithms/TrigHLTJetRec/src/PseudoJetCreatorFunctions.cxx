/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#include "TrigHLTJetRec/PseudoJetCreatorFunctions.h"
#include "TrigHLTJetRec/IIParticleRejectionTool.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODPFlow/PFOContainer.h"
#include "GaudiKernel/ToolHandle.h"


namespace PseudoJetCreatorFunctions{


  // Rejecter objects are used to select which incoming IParticles
  // are converted to PseudoJets.

  class IRejecter{
  public:
    virtual ~IRejecter() {}
    virtual bool operator()(const xAOD::IParticle*) = 0;
  };


  ////////////////////////////////
  // Concrete Rejecter

  struct IParticleRejecter: public IRejecter{
    // maintain state to allow the reasons for rejection to be examined.
    bool null{false};
    bool negativeE{false};
    bool skipNegativeEnergy{false};
    
    IParticleRejecter(bool skip): skipNegativeEnergy(skip){
    }
    
    virtual bool operator()(const xAOD::IParticle* ip) override {
      null = (ip == 0);
      negativeE = skipNegativeEnergy && ip->e() <= 0.0;
      return (null || negativeE);
    }
  };
  
  ////////////////////////////////
  // Concrete Rejecter
  
  struct PFlowRejecter: public IRejecter {

    bool skipNegativeEnergy{false};
    
    PFlowRejecter(bool skip): skipNegativeEnergy(skip){
    }
    
    virtual bool operator()(const xAOD::IParticle* ip) override {
      
      const xAOD::PFO* pfo = dynamic_cast<const xAOD::PFO*>(ip);
      
      // keep charged PFOs with energy==0 because for MET TST with PFlow, 
      // there may be high pt 
      // charged PFOs that receive a weight of 0 due to being in dense 
      // showers, but need to be present for overlap removal, because they 
      // don't retain these weights when added to the TST      
      
      if( fabs(pfo->charge())>FLT_MIN) {
        const static SG::AuxElement::ConstAccessor<char> 
          PVMatchedAcc("matchedToPV");
      return  !PVMatchedAcc(*pfo);
      }
      
      // This also skips 0 energy, which has no effect other than
      // on memory size, but is used in some workflows for pileup rejection
      return skipNegativeEnergy && pfo->e()<FLT_MIN;
    }
  };

  ////////////////////////////////
  // Concrete Rejecter

  struct EMTopoRejecter: public IRejecter{
    const xAOD::CaloCluster* cluster{0};
    std::unique_ptr<IParticleRejecter> ipRejecter;

    EMTopoRejecter(bool skip){
      ipRejecter = std::make_unique<IParticleRejecter>(skip);
    }
      
    virtual bool operator()(const xAOD::IParticle* ip) override {
      if ((*ipRejecter)(ip)){return true;}
      cluster = dynamic_cast<const xAOD::CaloCluster*>(ip);
      return cluster == 0;  // reject if not a cluster
    }
  };

  ////////////////////////////////
  // Concrete Rejecter

  struct NullRejecter: public IRejecter{
    virtual bool operator()(const xAOD::IParticle*) override {return false;}
  };

  ////////////////////////////////
  // Rejecter Factory

  std::unique_ptr<IRejecter> make_rejector(xAOD::JetInput::Type iptype, 
                                           bool skipNegativeEnergy, 
                                           bool noRejection){

    if(noRejection) {return std::make_unique<NullRejecter>();}

    if (iptype == xAOD::JetInput::PFlow) {
      return std::make_unique<PFlowRejecter>(skipNegativeEnergy);
    }

    if (iptype == xAOD::JetInput::EMTopo) {
      return std::make_unique<EMTopoRejecter>(skipNegativeEnergy);
    }

    return std::make_unique<IParticleRejecter>(skipNegativeEnergy);
  }

  ////////////////////////////////
  // Momentum Getters: EMTopo clusters: use a different momentum from p4()

  class  IMomentumGetter {
  public:
    virtual ~IMomentumGetter() {}
    virtual xAOD::IParticle::FourMom_t 
    operator()(const xAOD::IParticle*) const = 0;
  };

  ////////////////////////////////
  // Concrete Momentum Getter
  
  class IParticleMomentumGetter: public IMomentumGetter {
    virtual xAOD::IParticle::FourMom_t  
    operator()(const xAOD::IParticle* ip) const override {
      return ip->p4();
    }
  };

  ////////////////////////////////
  // Concrete Momentum Getter
  
  class EMTopoMomentumGetter: public IMomentumGetter {
    virtual xAOD::IParticle::FourMom_t  

    // unprotected dynamic cast....
    operator()(const xAOD::IParticle* ip) const override {
      auto cluster = dynamic_cast<const xAOD::CaloCluster*>(ip);
      if (cluster == nullptr){
        throw(std::runtime_error("Error casting IParticle to xAOD::CaloCluster"));
      }
      return cluster->p4(xAOD::CaloCluster::UNCALIBRATED);
    }
  };

  ////////////Ghost version////////////////////
  class GhostParticleMomentumGetter: public IMomentumGetter {
    virtual xAOD::IParticle::FourMom_t  
    operator()(const xAOD::IParticle* ip) const override {
      return ip->p4()*1e-40; // momentum scaled to near zero.
    }
  };

  ////////////////////////////////
  // MomentumGetter Factory

  std::unique_ptr<IMomentumGetter> 
  make_momentumGetter(xAOD::JetInput::Type iptype){

    if (iptype == xAOD::JetInput::EMTopo) {
      return std::make_unique<EMTopoMomentumGetter>();
    }

    return std::make_unique<IParticleMomentumGetter>();
  }

  ////////////Ghost version////////////////////
  std::unique_ptr<IMomentumGetter> 
  make_ghostmomentumGetter(){

    return std::make_unique<GhostParticleMomentumGetter>();
  }
  
  ////////////////////////////////
  // IParticle to PseudoJet converter

  template<typename Rejecter>
  std::vector<PseudoJet> 
  IParticlesToPseudoJets(const xAOD::IParticleContainer* ips,
                         // std::unique_ptr<IRejecter> rejecter,
                         Rejecter&& rejecter,
                         std::unique_ptr<IMomentumGetter> momentumGetter
                         ){ 
    std::vector<PseudoJet> vpj;
    int index = -1;

      
    // loop over the input iparticles, select and  convert to pseudojets
    for(const xAOD::IParticle* ip: *ips) {
      if((*rejecter)(ip)){continue;}
      
      // Create a Pseudojet with the momentum of the selected IParticles.
      fastjet::PseudoJet psj((*momentumGetter)(ip));

      // user index is used to identify the xAOD object used for the PseudoJet
      psj.set_user_index(++index); 
      vpj.push_back(psj);
      
    }
    return vpj;
  }

#if 0
  template<typename Rejecter>
  std::vector<PseudoJet> 
  IParticlesToPseudoJets(const xAOD::IParticleContainer* ips,
                         // std::unique_ptr<IRejecter> rejecter,
                         const Rejecter& rejecter,
                         std::unique_ptr<IMomentumGetter> momentumGetter
                         ){ 
    std::vector<PseudoJet> vpj;
    int index = -1;

      
    // loop over the input iparticles, select and  convert to pseudojets
    for(const xAOD::IParticle* ip: *ips) {
      if((*rejecter)(ip)){continue;}
      
      // Create a Pseudojet with the momentum of the selected IParticles.
      fastjet::PseudoJet psj((*momentumGetter)(ip));

      // user index is used to identify the xAOD object used for the PseudoJet
      psj.set_user_index(++index); 
      vpj.push_back(psj);
      
    }
    return vpj;
  }
#endif
  
  ////////////////////////////////////////////

  std::vector<PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer* ips,
                   xAOD::JetInput::Type iptype,
                   bool noNegE,
                   bool noRejection){

    
    auto rejecter = make_rejector(iptype, noNegE, noRejection);
    auto momentumGetter = make_momentumGetter(iptype);

    return IParticlesToPseudoJets(ips, 
                                  std::move(rejecter),
                                  std::move(momentumGetter));
  }
  ////////////////////////////////////////////
  
  std::vector<PseudoJet> 
  createPseudoJets(const xAOD::IParticleContainer* ips,
                   xAOD::JetInput::Type iptype,
                   ToolHandle<IIParticleRejectionTool>& rejecter){

    
    auto momentumGetter = make_momentumGetter(iptype);

    return IParticlesToPseudoJets(ips, 
                                  rejecter,
                                  std::move(momentumGetter));
  }

  ////////////////Ghost version////////////////////////////
  std::vector<PseudoJet> 
  createGhostPseudoJets(const xAOD::IParticleContainer* ips,
                   ToolHandle<IIParticleRejectionTool>& rejecter){

    
    auto momentumGetter = make_ghostmomentumGetter();

    return IParticlesToPseudoJets(ips, 
                                  rejecter,
                                  std::move(momentumGetter));
  }
  
}
