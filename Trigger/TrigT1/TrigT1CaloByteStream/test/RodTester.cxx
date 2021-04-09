/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include <stdint.h>

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

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

StatusCode RodTester::initialize()
{
  msg(MSG::INFO) << "Initializing " << name() << endmsg;

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
          << endmsg;
    const std::string location(m_rodHeaderLocation + *flagIter);
    const RodHeaderCollection* rods = 0;
    StatusCode sc = evtStore()->retrieve(rods, location);
    if (sc.isFailure() || !rods || rods->empty()) {
      msg() << "No ROD headers found for " << location << endmsg;
    } else {

      // Print the ROD headers

      msg() << "Number of ROD headers in collection "
            << location << " is " << rods->size() << MSG::hex << endmsg;
      RodHeaderCollection::const_iterator rodIter = rods->begin();
      RodHeaderCollection::const_iterator rodEnd  = rods->end();
      for (; rodIter != rodEnd; ++rodIter) {
        const LVL1::RODHeader* const header = *rodIter;
	msg() << "-------------------------------------------------"
	      << endmsg
	      << "version/majorVersion/minorVersion: "
	      << header->version() << "/" << header->majorVersion() << "/"
	      << header->minorVersion() << endmsg
              << "sourceID/subDetectorID/moduleID/crate/sLink/dataType: "
	      << header->sourceID() << "/" << header->subDetectorID() << "/"
	      << header->moduleID() << "/" << header->crate() << "/"
	      << header->sLink() << "/" << header->dataType() << endmsg
              << "run/runType/runNumber: "
	      << header->run() << "/" << header->runType() << "/"
	      << header->runNumber() << endmsg
              << "extendedL1ID/ecrID/l1ID: "
	      << header->extendedL1ID() << "/" << header->ecrID() << "/"
	      << header->l1ID() << endmsg
              << "bunchCrossing/l1TriggerType: "
	      << header->bunchCrossing() << "/" << header->l1TriggerType() << "/"
	      << endmsg
              << "detEventType/orbitCount/stepNumber/stepType: "
	      << header->detEventType() << "/" << header->orbitCount() << "/"
	      << header->stepNumber() << "/" << header->stepType() << endmsg
	      << "payloadSize: " << header->payloadSize() << endmsg;
	const std::vector<uint32_t>& words(header->statusWords());
        msg() << "statusWords(" << words.size() << "): ";
	std::vector<uint32_t>::const_iterator wordIter = words.begin();
	std::vector<uint32_t>::const_iterator wordEnd  = words.end();
	for (; wordIter != wordEnd; ++wordIter) {
	  msg() << *wordIter << " ";
        }
	msg() << endmsg
	      << "bcnMismatch/gLinkTimeout/dataTransportError/rodFifoOverflow: "
	      << header->bcnMismatch() << "/" << header->gLinkTimeout() << "/"
	      << header->dataTransportError() << "/" << header->rodFifoOverflow()
	      << endmsg
              << "lvdsLinkError/cmmParityError/gLinkError: "
	      << header->lvdsLinkError() << "/" << header->cmmParityError() << "/"
	      << header->gLinkError() << endmsg
              << "limitedRoISet/triggerTypeTimeout: "
	      << header->limitedRoISet() << "/" << header->triggerTypeTimeout()
	      << endmsg;
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
