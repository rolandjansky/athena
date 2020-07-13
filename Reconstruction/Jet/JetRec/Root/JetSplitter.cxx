/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetSplitter.cxx

#include "JetRec/JetSplitter.h"
#include <iomanip>
#include "fastjet/PseudoJet.hh"
#include "fastjet/tools/MassDropTagger.hh"
#include "fastjet/tools/Filter.hh"
#include "JetEDM/PseudoJetVector.h"
#include "JetEDM/IConstituentUserInfo.h"
#include "JetEDM/JetConstituentFiller.h"
#include "JetRec/JetDumper.h"

using std::setw;
using fastjet::PseudoJet;
using fastjet::MassDropTagger;
using fastjet::Filter;
using fastjet::cambridge_algorithm;
using fastjet::JetDefinition;
using fastjet::SelectorNHardest;
using xAOD::JetContainer;
using jet::IConstituentUserInfo;
using jet::JetConstituentFiller;

//**********************************************************************

JetSplitter::JetSplitter(std::string name)
  : AsgTool(name), m_bld("",this),
  m_pdmp(new JetDumper(name+"Dumper")) {
  declareProperty("MuMax", m_mumax =1.0);
  declareProperty("YMin", m_ymin =0.0);
  declareProperty("RClus", m_rclus =0.3);
  declareProperty("BDRS", m_bdrs =false);
  declareProperty("NSubjetMax", m_nsubjetmax =3);
  declareProperty("JetBuilder", m_bld);
}

//**********************************************************************

JetSplitter::~JetSplitter() {
  delete m_pdmp;
}

//**********************************************************************

StatusCode JetSplitter::initialize() {
  if ( m_mumax < 0.0 || m_mumax > 1.0 ) {
    ATH_MSG_ERROR("Invalid value for MuMax: " << m_mumax);
    return StatusCode::FAILURE;
  }
  if ( m_mumax < 0.0 || m_mumax > 1.0 ) {
    ATH_MSG_ERROR("Invalid value for YMin: " << m_ymin);
    return StatusCode::FAILURE;
  }
  if ( m_bld.empty() ) {
    ATH_MSG_ERROR("Unable to retrieve jet builder.");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

int JetSplitter::groom(const xAOD::Jet& jin,
                       const PseudoJetContainer& pjContainer,
                       xAOD::JetContainer& jets) const {
  MassDropTagger mdtagger(m_mumax, m_ymin);
  if ( pseudojetRetriever() == nullptr ) {
    ATH_MSG_WARNING("Pseudojet retriever is null.");
    return 1;
  }
  const PseudoJet* ppjin = pseudojetRetriever()->pseudojet(jin);
  if ( ppjin == 0 ) {
    ATH_MSG_WARNING("Jet does not have a pseudojet.");
    return 1;
  }
  ATH_MSG_VERBOSE("Input pseudojet pT: " << ppjin->pt());
  ATH_MSG_VERBOSE("Input pseudojet has area: " << ppjin->has_area());
  int nconin = ppjin->constituents().size();
  PseudoJet pjfilt = mdtagger(*ppjin);
  if ( pjfilt == 0 ) {
    ATH_MSG_DEBUG("Jet rejected by splitter.");
    return 1;
  }
  ATH_MSG_DEBUG("Jet accepted by splitter.");
  PseudoJet parent1 = pjfilt.pieces()[0];
  PseudoJet parent2 = pjfilt.pieces()[1];
  int npfilt = pjfilt.pieces().size();
  double drfilt = parent1.delta_R(parent2);
  double mufilt = pjfilt.structure_of<MassDropTagger>().mu();
  double yfilt = pjfilt.structure_of<MassDropTagger>().y();
  ATH_MSG_DEBUG("Properties after filtering:");
  ATH_MSG_DEBUG("     pT: " << pjfilt.pt());
  ATH_MSG_DEBUG("  has A: " << pjfilt.has_area());
  ATH_MSG_DEBUG("   ncon: " << pjfilt.constituents().size() << "/" << nconin);
  ATH_MSG_DEBUG("   nsub: " << npfilt);
  ATH_MSG_DEBUG("   DR12: " << drfilt);
  ATH_MSG_DEBUG("     mu: " << mufilt);
  ATH_MSG_DEBUG("      y: " << yfilt);
  if ( msgLvl(MSG::VERBOSE) ) {
    std::vector<PseudoJet> cons = pjfilt.constituents();
    m_pdmp->dump_collection(&cons);
  }

  // Recluster.
  double rclus = m_rclus;
  if ( m_bdrs ) {
    double rbdrs = 0.5*drfilt;
    if ( rbdrs < rclus ) rclus = rbdrs;
  }
  Filter filter(JetDefinition(cambridge_algorithm, rclus),
                SelectorNHardest(m_nsubjetmax));
  PseudoJet pjclus = filter(pjfilt);
  ATH_MSG_DEBUG("Properties after reclustering:");
  ATH_MSG_DEBUG("     pT: " << pjclus.pt());
  ATH_MSG_DEBUG("  has A: " << pjclus.has_area());
  ATH_MSG_VERBOSE("       Input cluster sequence: " << ppjin->associated_cluster_sequence());
  ATH_MSG_VERBOSE("    Filtered cluster sequence: " << pjfilt.associated_cluster_sequence());
  ATH_MSG_VERBOSE(" Reclustered cluster sequence: " << pjclus.associated_cluster_sequence());
  int npclus = pjclus.pieces().size();
  double drclus =  0.0;
  double muclus = 0.0;
  double yclus = 0.0;
  if ( npclus > 1 ) {
    PseudoJet parent1 = pjclus.pieces()[0];
    PseudoJet parent2 = pjclus.pieces()[1];
    drclus= parent1.delta_R(parent2);
  }
  ATH_MSG_DEBUG("Properties after reclustering:");
  ATH_MSG_DEBUG("   ncon: " << pjclus.constituents().size() << "/"
                            << pjfilt.constituents().size() << "/" << nconin);
  ATH_MSG_DEBUG("   nsub: " << npclus);
  ATH_MSG_DEBUG("   DR12: " << drclus);
  ATH_MSG_DEBUG("     mu: " << muclus);
  ATH_MSG_DEBUG("      y: " << yclus);

  // Add jet to collection.
  xAOD::Jet* pjet = m_bld->add(pjclus, pjContainer, jets, &jin);
  if ( pjet == 0 ) {
    ATH_MSG_ERROR("Unable to add jet to container");
  } else {
    ATH_MSG_DEBUG("Added jet to container.");
    pjet->setAttribute<int>("TransformType", xAOD::JetTransform::MassDrop);
    pjet->setAttribute<float>("MuMax", m_mumax);
    pjet->setAttribute<float>("YMin", m_ymin);
    pjet->setAttribute<float>("RClus", m_rclus);
    //this has to be a char, because by default bools are converted to chars in perstification. Thus if we run this tool on an exiting collection in a POOL file the type is char.                                                                                               
    pjet->setAttribute<char>("BDRS", m_bdrs);
    pjet->setAttribute<int>("NSubjetMax", m_nsubjetmax); 
    pjet->setAttribute<float>("DRFilt", drfilt);
    pjet->setAttribute<float>("MuFilt", mufilt);
    pjet->setAttribute<float>("YFilt",  yfilt);
    pjet->setAttribute<int>("NSubjet", npclus);
  }
  return 0;
}

//**********************************************************************

void JetSplitter::print() const {
  ATH_MSG_INFO("       mu max: " << m_mumax);
  ATH_MSG_INFO("        y min: " << m_ymin);
  ATH_MSG_INFO("  Recluster R: " << m_rclus);
  ATH_MSG_INFO("     Use BDRS: " << m_bdrs);
  ATH_MSG_INFO("    # subjets: " << m_nsubjetmax);
  ATH_MSG_INFO("  Jet builder: " << m_bld.name());
}

//**********************************************************************
