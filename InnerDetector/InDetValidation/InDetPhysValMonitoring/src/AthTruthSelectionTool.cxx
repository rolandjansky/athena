/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file AthTruthSelectionTool.cxx
 * implementation file for truth selection in this package
 * @author shaun roe
 * @date 10 October 2016
 **/

#include "AthTruthSelectionTool.h"
#include "xAODTruth/TruthVertex.h"

#include <vector>
#include <cmath>

namespace {
  constexpr int electronId(11);
  constexpr int gammaId(22);
}


AthTruthSelectionTool::AthTruthSelectionTool(const std::string& type, const std::string& name,
                                             const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_counters{} {
  // declare interface from base class
  declareInterface<IAthSelectionTool>(this);
  // declareProperty( "Property", m_nProperty ); set defaults
  declareProperty("maxEta", m_maxEta = 2.5);
  declareProperty("minPt", m_minPt = 400);
  declareProperty("maxPt", m_maxPt = -1);
  declareProperty("maxBarcode", m_maxBarcode = 200e3);
  declareProperty("requireCharged", m_requireCharged = true);
  declareProperty("requireStatus1", m_requireStatus1 = true);
  declareProperty("maxProdVertRadius", m_maxProdVertRadius = 110.);
  declareProperty("pdgId", m_pdgId = -1);
  declareProperty("hasNoGrandparent", m_grandparent = false);
  declareProperty("poselectronfromgamma", m_poselectronfromgamma = false);
}

StatusCode
AthTruthSelectionTool::initialize() {
  // can set cut properties now
  typedef xAOD::TruthParticle P_t;
  typedef Accept<P_t> Accept_t;
  typedef Accept_t::func_type F_t;
  //
  const std::vector<Accept_t> filters = {
    // if p.pt=0, TVector3 generates an error when querying p.eta(); a limit of 1e-7 was not found to be enough to
    // prevent this
    // the following also vetoes the case where the p.pt()=NaN, as any inequality with NaN evaluates to false
    Accept_t([this](const P_t& p) -> bool {
      return((p.pt() > 0.1) ? (std::abs(p.eta()) < m_maxEta) : false);
    }, std::string("eta")),
    Accept_t([this](const P_t& p) -> bool {
      return(p.pt() > m_minPt);
    }, std::string("min_pt")),
    Accept_t([this](const P_t& p) -> bool {
      return((not (p.hasProdVtx()))or(p.prodVtx()->perp() < m_maxProdVertRadius));
    }, "decay_before_" + std::to_string(m_maxProdVertRadius))
  };
  //
  m_cutFlow = CutFlow<P_t>(filters);
  if (m_maxPt > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.pt() < m_maxPt);
    }, "max_pt"));
  }
  if (m_maxBarcode > -1) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(p.barcode() < m_maxBarcode);
    }, "barcode < " + std::to_string(m_maxBarcode)));
  }
  if (m_requireCharged) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return(not (p.isNeutral()));
    }, "charged"));
  }
  if (m_requireStatus1) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return(p.status() == 1);
    }, "status1"));
  }
  if (m_pdgId > 0) {
    m_cutFlow.add(Accept_t([this](const P_t& p) {
      return(std::abs(p.pdgId()) == m_pdgId);
    }, "pdgId"));
  }
  if (m_grandparent) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return((p.nParents() == 0) || ((p.nParents() == 1)and((p.parent(0))->nParents() == 0)));
    }, "hasNoGrandparent"));
  }
  if (m_poselectronfromgamma) {
    m_cutFlow.add(Accept_t([](const P_t& p) {
      return((p.absPdgId() == electronId)and(p.nParents() >= 1) and(p.parent(0)) and(p.parent(0)->pdgId() == gammaId));
    }, "poselectronfromgamma"));
  }
  m_counters = std::vector<unsigned int>(m_cutFlow.size(), 0);
  std::string msg = std::to_string(m_cutFlow.size()) + " truth acceptance cuts are used:\n";
  for (const auto& i:m_cutFlow.names()) {
    msg += i + "\n";
  }
  ATH_MSG_INFO(msg);
  clearCounters();

  return StatusCode::SUCCESS;
}

StatusCode
AthTruthSelectionTool::finalize() {
  // nop
  return StatusCode::SUCCESS;
}

void
AthTruthSelectionTool::clearCounters() {
  m_cutFlow.clear();
  m_counters = m_cutFlow.counters();
}

std::vector<unsigned int>
AthTruthSelectionTool::counters() const {
  return m_cutFlow.counters();
}

std::vector<std::string>
AthTruthSelectionTool::names() const {
  return m_cutFlow.names();
}

bool
AthTruthSelectionTool::accept(const xAOD::IParticle* particle) {
  const xAOD::TruthParticle* pTruth = dynamic_cast<const xAOD::TruthParticle*>(particle);

  if (not pTruth) {
    return false;
  }
  return m_cutFlow.accept(*pTruth);
}

std::string
AthTruthSelectionTool::str() const {
  return m_cutFlow.report();
}
