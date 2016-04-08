/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DetStatusAlg.cxx
// Richard Hawkings, started 18/12/04
// implementation for DetStatusAlg algorithm

#include "DetectorStatus/IDetStatusSvc.h"
#include "DetDescrConditions/DetStatus.h"
#include "DetectorStatus/DetStatusAlg.h"


DetStatusAlg::DetStatusAlg(const std::string& name, ISvcLocator* pSvcLocator)
    :AthAlgorithm   (name, pSvcLocator),
     m_log       (msgSvc(), name),
     par_print(false),par_veto(false),
     m_count(0),m_nveto(0)
{
  // declare properties
  declareProperty("Print",par_print);
  declareProperty("Veto",par_veto);
}

DetStatusAlg::~DetStatusAlg()
{}

StatusCode DetStatusAlg::initialize() {
  m_log << MSG::INFO << "Initialize called" << endreq;
  // get DetStatusSvc
  if (StatusCode::SUCCESS!=service("DetStatusSvc",p_detstatussvc)) {
    m_log << MSG::ERROR << "Cannot get DetStatusSvc" << endreq;
    return StatusCode::FAILURE;
  }
  if (par_veto) m_log << MSG::INFO << "Events with bad status will be vetoed"
		      << endreq;
  return StatusCode::SUCCESS;
}

StatusCode DetStatusAlg::execute() {
  // execute method - dispatch calls to DetStatusSvc
  if (par_print) p_detstatussvc->print();

  // veto functionality
  if (par_veto) doVeto();

  // if nothing done, retrieve the map anyway, to ensure conditions data
  // gets read in to StoreGate from CondDB
  // this is needed to ensure it can be written out to file-level meta-data
  if (!(par_print || par_veto)) {
    DetStatusMap::const_iterator begin,end;
    p_detstatussvc->getIter(begin,end);
    m_log << MSG::DEBUG << "Dummy retrieve of DetStatusMap done" << endreq;
  }

  return StatusCode::SUCCESS;
}

void DetStatusAlg::doVeto() {
  ++m_count;
  if (p_detstatussvc->vetoed()) {
    m_log << MSG::DEBUG << "Event vetoed due to bad status" << endreq;
    ++m_nveto;
    // set filter to false so this can be used in an algorithm Sequence
    setFilterPassed(false);
  }
}

StatusCode DetStatusAlg::finalize() {
  if (par_veto) {
    m_log << MSG::INFO << "Saw " << m_count << " events of which " <<
      m_nveto << " vetoed due to bad status" << endreq;
  }
  return StatusCode::SUCCESS;
}
