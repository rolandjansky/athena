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
     m_par_print(false),m_par_veto(false),
     m_count(0),m_nveto(0)
{
  // declare properties
  declareProperty("Print",m_par_print);
  declareProperty("Veto",m_par_veto);
}

DetStatusAlg::~DetStatusAlg()
{}

StatusCode DetStatusAlg::initialize() {
  ATH_MSG_INFO( "Initialize called"  );
  // get DetStatusSvc
  if (StatusCode::SUCCESS!=service("DetStatusSvc",m_detstatussvc)) {
    ATH_MSG_ERROR( "Cannot get DetStatusSvc"  );
    return StatusCode::FAILURE;
  }
  if (m_par_veto) ATH_MSG_INFO( "Events with bad status will be vetoed" );
  return StatusCode::SUCCESS;
}

StatusCode DetStatusAlg::execute() {
  // execute method - dispatch calls to DetStatusSvc
  if (m_par_print) m_detstatussvc->print();

  // veto functionality
  if (m_par_veto) doVeto();

  // if nothing done, retrieve the map anyway, to ensure conditions data
  // gets read in to StoreGate from CondDB
  // this is needed to ensure it can be written out to file-level meta-data
  if (!(m_par_print || m_par_veto)) {
    DetStatusMap::const_iterator begin,end;
    m_detstatussvc->getIter(begin,end);
    ATH_MSG_DEBUG( "Dummy retrieve of DetStatusMap done"  );
  }

  return StatusCode::SUCCESS;
}

void DetStatusAlg::doVeto() {
  ++m_count;
  if (m_detstatussvc->vetoed()) {
    ATH_MSG_DEBUG( "Event vetoed due to bad status"  );
    ++m_nveto;
    // set filter to false so this can be used in an algorithm Sequence
    setFilterPassed(false);
  }
}

StatusCode DetStatusAlg::finalize() {
  if (m_par_veto) {
    ATH_MSG_INFO( "Saw " << m_count << " events of which " <<
                  m_nveto << " vetoed due to bad status"  );
  }
  return StatusCode::SUCCESS;
}
