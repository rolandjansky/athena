/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// PseudoJetGetter.cxx

#include "JetRec/PseudoJetGetter.h"
#include "fastjet/PseudoJet.hh"
#include "JetRec/PseudoJetContainer.h"
#include "JetEDM/PseudoJetVector.h"

#include "xAODBase/IParticleContainer.h"
#include "JetRec/IParticleExtractor.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#ifndef GENERATIONBASE
#include "xAODPFlow/PFOContainer.h"
#endif

using std::string;
using jet::PseudoJetVector;
using fastjet::PseudoJet;

//**********************************************************************

PseudoJetGetter::PseudoJetGetter(const std::string& name)
  : AsgTool(name), m_emtopo(false), m_pflow(false) {
  declareProperty("InputContainer", m_incoll);
  declareProperty("OutputContainer", m_outcoll);
  declareProperty("Label", m_label);
  declareProperty("SkipNegativeEnergy", m_skipNegativeEnergy =false);
  declareProperty("GhostScale", m_ghostscale =0.0);
  declareProperty("TreatNegativeEnergyAsGhost", m_negEnergyAsGhosts =false);

}

//**********************************************************************

StatusCode PseudoJetGetter::initialize() {
  ATH_MSG_DEBUG("Initializing...");
  // PJG needs to know if this is the basic EMTopo cluster collection
  // in order to change the cluster signal state.
  if ( m_label == "EMTopo") m_emtopo = true;
  if ( m_label == "EMPFlow") m_pflow = true;
  if ( m_label == "EMNPFlow") m_pflow = true;
  print();

  ATH_CHECK( m_incoll.initialize() );
  ATH_CHECK( m_outcoll.initialize() );

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode PseudoJetGetter::createAndRecord() const {
  // Use const pointer for now, but can change to unique pointer when
  // we move to DataVector and when MR 2431 is complete:
  // https://gitlab.cern.ch/atlas/athena/merge_requests/2431

  // should rename getC createAndRecord once get() is removed.
  const PseudoJetContainer* pjc = this->getC();
  if (!pjc) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}
//**********************************************************************
const PseudoJetVector* PseudoJetGetter::get() const {
  // Kept for backward compatibity
  ATH_MSG_DEBUG("Getting PseudoJetContainer as PseudoJetVector ...");
  const PseudoJetContainer* cont = this->getC();
  const PseudoJetVector* vpj = cont->casVectorPseudoJet(); 
  return vpj;
}

//**********************************************************************

const PseudoJetContainer* PseudoJetGetter::getC() const {
  ATH_MSG_DEBUG("Getting PseudoJetContainer...");
  // build and record the container
  const xAOD::IParticleContainer* cont;
  auto handle_in = SG::makeHandle(m_incoll);
  if ( handle_in.isValid() ) {
    cont = handle_in.cptr();
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_incoll.key() << "of size "<<cont->size()
                  << ", ghost scale=" << m_ghostscale  
                  <<  ", isGhost=" << bool(m_ghostscale));


  } else {
    ATH_MSG_ERROR("Unable to find input collection: " << m_incoll.key());
    ATH_MSG_ERROR("Error creating pseudojets.");
    return nullptr;
  }

  // create PseudoJets
  std::vector<PseudoJet> vpj = createPseudoJets(cont);

  // create an extractor

  // "Ghost" in outout collection name? If so is a ghost collection.
  bool isGhost = (m_outcoll.key()).find("Ghost") != std::string::npos;

  IParticleExtractor* extractor = new IParticleExtractor(cont,
                                                         m_label,
                                                         isGhost);
  ATH_MSG_DEBUG("New  extractor: "  << extractor->toString(0));
  
  // ghostify the pseudojets if necessary
  if(isGhost){
    for(PseudoJet& pj : vpj) {pj *= 1e-40;}
  }
  
  // Put the PseudoJetContainer together :
  std::unique_ptr<const PseudoJetContainer> uppjcont(new PseudoJetContainer(extractor, vpj));

  // record
  
  SG::WriteHandle<PseudoJetContainer> handle_out(m_outcoll);


  ATH_MSG_DEBUG("New PseudoJetContainer size " << uppjcont->size());
  // notify
  const PseudoJetContainer* ppjcont = handle_out.put(std::move(uppjcont));
  if (!ppjcont) {
    ATH_MSG_ERROR("Unable to write new PseudoJetContainer to event store: " 
                  << m_outcoll.key());
  } else {
    ATH_MSG_DEBUG("Created new PseudoJetContainer in event store: " 
                  << m_outcoll.key());
  }

  return ppjcont;  // used by legacy code, looks wrong....
}


std::vector<PseudoJet> 
PseudoJetGetter::createPseudoJets(const xAOD::IParticleContainer* ips) const{
#ifndef GENERATIONBASE
  if (m_pflow) {return PFlowsToPJs(ips);}
#endif
  if (m_emtopo) {return EMToposToPJs(ips);}
  return IParticlesToPJs(ips);
}

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


std::vector<PseudoJet> 
PseudoJetGetter::IParticlesToPJs(const xAOD::IParticleContainer* ips) const {

  IParticleRejecter rejecter(m_skipNegativeEnergy);

  std::vector<PseudoJet> vpj;
  int index = -1;

  // loop over the input iparticles, select and  convert to pseudojets
  for(const xAOD::IParticle* ip: *ips) {
    ++index;
    if(rejecter(ip)){continue;}
    
    // Create a PSeudojet with the momentum of the selected IParticles.
    fastjet::PseudoJet psj(ip->p4());

    // user index is used to identify the xAOD object used for the PseudoJet
    psj.set_user_index(index); 
    vpj.push_back(psj);

  }
  return vpj;
}

struct EMTopoRejecter{
  const xAOD::CaloCluster* cluster{0};

  bool operator()(const xAOD::IParticle* ip){
    cluster = dynamic_cast<const xAOD::CaloCluster*>(ip);
    return cluster == 0;  // reject if not a cluster
  }
};

std::vector<PseudoJet> 
PseudoJetGetter::EMToposToPJs(const xAOD::IParticleContainer* ips) const {

  // helper objects for selecting iparticles to be converted to pseudojets
  IParticleRejecter ipRejecter(m_skipNegativeEnergy);
  EMTopoRejecter emRejecter;
  
  std::vector<PseudoJet> vpj;
  int index = -1;

  // loop over iparticles, select and  convert to pseudojets

  for(const xAOD::IParticle* ip: *ips) {
    ++index;
    if(ipRejecter(ip) or emRejecter(ip)){continue;}
    
    // Create a Pseudojet with the momentum of the cluster.
    fastjet::PseudoJet 
      psj(emRejecter.cluster->p4(xAOD::CaloCluster::UNCALIBRATED));
    
    // user index is used to identify the xAOD object used for the PSeudoJet
    psj.set_user_index(index); 
    vpj.push_back(psj);
  }
  return vpj;
}

#ifndef GENERATIONBASE
struct PFlowRejecter{

  bool skipNegativeEnergy{false};
  
  PFlowRejecter(bool skip): skipNegativeEnergy(skip){
  }

  bool operator()(const xAOD::IParticle* ip){
    
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

std::vector<PseudoJet> 
PseudoJetGetter::PFlowsToPJs(const xAOD::IParticleContainer* ips) const {

  PFlowRejecter rejecter(m_skipNegativeEnergy);
  std::vector<PseudoJet> vpj;
  int index = -1;

  // loop over the input iparticles, select and  convert to pseudojets

  for(const xAOD::IParticle* ip: *ips) {
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

//**********************************************************************


std::string PseudoJetGetter::label() const{
  return m_label;
}

//**********************************************************************

void PseudoJetGetter::print() const {
  string sskip = m_skipNegativeEnergy ? "true" : "false";
  ATH_MSG_INFO("Properties for PseudoJetGetter " << name());
  ATH_MSG_INFO("             Label: " << m_label);
  ATH_MSG_INFO("   Input container: " << m_incoll.key());
  ATH_MSG_INFO("  Output container: " << m_outcoll.key());
  ATH_MSG_INFO("   Skip negative E: " << sskip);
  ATH_MSG_INFO("         Is EMTopo: " << m_emtopo);
  ATH_MSG_INFO("          Is PFlow: " << m_pflow);
  ATH_MSG_INFO(" Treat negative E as ghost: " << m_negEnergyAsGhosts);
}

//**********************************************************************

int  PseudoJetGetter::inputContainerNames(std::vector<std::string>& connames) {
  if ( m_incoll.key().size() ) connames.push_back(m_incoll.key());
  return 0;
}

//**********************************************************************

int PseudoJetGetter::outputContainerNames(std::vector<std::string>& connames) {
  if ( m_outcoll.key().size() ) connames.push_back(m_outcoll.key());
  return 0;
}

//**********************************************************************
