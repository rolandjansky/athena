/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include <stdint.h>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloEvent/RODHeader.h"

#include "RodTester.h"

namespace LVL1BS {

RodTester::RodTester(const std::string& name, ISvcLocator* pSvcLocator)
                     : AthAlgorithm(name, pSvcLocator)
{
  declareProperty("RodHeaderLocation", m_rodHeaderLocation =
                   LVL1::TrigT1CaloDefs::RODHeaderLocation);

  m_flags.push_back("");
  m_flags.push_back("PP");
  m_flags.push_back("CP");
  m_flags.push_back("CPRoI");
  m_flags.push_back("JEP");
  m_flags.push_back("JEPRoI");
  m_flags.push_back("CPRoIB");
  m_flags.push_back("JEPRoIB");
}

RodTester::~RodTester()
{
}

// Initialize

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

StatusCode RodTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << /* version() */ PACKAGE_VERSION << endreq;

  return StatusCode::SUCCESS;
}

// Execute

StatusCode RodTester::execute()
{
  if ( !msgLvl(MSG::INFO) ) return StatusCode::SUCCESS;
  msg(MSG::INFO);

  // Loop over all possible collections

  std::vector<std::string>::const_iterator flagIter = m_flags.begin();
  std::vector<std::string>::const_iterator flagEnd  = m_flags.end();
  for (; flagIter != flagEnd; ++flagIter) {
    msg() << "================================================="
          << endreq;
    const std::string location(m_rodHeaderLocation + *flagIter);
    const RodHeaderCollection* rods = 0;
    StatusCode sc = evtStore()->retrieve(rods, location);
    if (sc.isFailure() || !rods || rods->empty()) {
      msg() << "No ROD headers found for " << location << endreq;
    } else {

      // Print the ROD headers

      msg() << "Number of ROD headers in collection "
            << location << " is " << rods->size() << MSG::hex << endreq;
      RodHeaderCollection::const_iterator rodIter = rods->begin();
      RodHeaderCollection::const_iterator rodEnd  = rods->end();
      for (; rodIter != rodEnd; ++rodIter) {
        const LVL1::RODHeader* const header = *rodIter;
	msg() << "-------------------------------------------------"
	      << endreq
	      << "version/majorVersion/minorVersion: "
	      << header->version() << "/" << header->majorVersion() << "/"
	      << header->minorVersion() << endreq
              << "sourceID/subDetectorID/moduleID/crate/sLink/dataType: "
	      << header->sourceID() << "/" << header->subDetectorID() << "/"
	      << header->moduleID() << "/" << header->crate() << "/"
	      << header->sLink() << "/" << header->dataType() << endreq
              << "run/runType/runNumber: "
	      << header->run() << "/" << header->runType() << "/"
	      << header->runNumber() << endreq
              << "extendedL1ID/ecrID/l1ID: "
	      << header->extendedL1ID() << "/" << header->ecrID() << "/"
	      << header->l1ID() << endreq
              << "bunchCrossing/l1TriggerType: "
	      << header->bunchCrossing() << "/" << header->l1TriggerType() << "/"
	      << endreq
              << "detEventType/orbitCount/stepNumber/stepType: "
	      << header->detEventType() << "/" << header->orbitCount() << "/"
	      << header->stepNumber() << "/" << header->stepType() << endreq
	      << "payloadSize: " << header->payloadSize() << endreq;
	const std::vector<uint32_t>& words(header->statusWords());
        msg() << "statusWords(" << words.size() << "): ";
	std::vector<uint32_t>::const_iterator wordIter = words.begin();
	std::vector<uint32_t>::const_iterator wordEnd  = words.end();
	for (; wordIter != wordEnd; ++wordIter) {
	  msg() << *wordIter << " ";
        }
	msg() << endreq
	      << "bcnMismatch/gLinkTimeout/dataTransportError/rodFifoOverflow: "
	      << header->bcnMismatch() << "/" << header->gLinkTimeout() << "/"
	      << header->dataTransportError() << "/" << header->rodFifoOverflow()
	      << endreq
              << "lvdsLinkError/cmmParityError/gLinkError: "
	      << header->lvdsLinkError() << "/" << header->cmmParityError() << "/"
	      << header->gLinkError() << endreq
              << "limitedRoISet/triggerTypeTimeout: "
	      << header->limitedRoISet() << "/" << header->triggerTypeTimeout()
	      << endreq;
      }
      msg() << MSG::dec;
    }
  }

  return StatusCode::SUCCESS;
}

// Finalize

StatusCode RodTester::finalize()
{

  return StatusCode::SUCCESS;
}

} // end namespace
