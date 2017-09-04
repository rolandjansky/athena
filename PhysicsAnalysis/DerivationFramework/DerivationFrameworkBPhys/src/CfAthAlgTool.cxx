/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//============================================================================
// CfAthAlgTool.h
//============================================================================
// 
// Author : Wolfgang Walkowiak <Wolfgang.Walkowiak@cern.ch.>
// Changes:
//
// Wrapper around AthAlgTool to provide easy access to CutFlowSvc
// and some utility methods for it.
// Methods for accessing the CutFlowSvc are modelled after
// AthFilterAlgorithm's implementation.
//
// This class inherits from AthAlgTool.  It should be inherited from.
//
//-----------------------------------------------------------------------------
//
// Usage information
//
// Example:
//
//   // Bmumu_reco_mumu.h:
//  class Bmumu_reco_mumu : public CfAthAlgTool, public IAugmentationTool {
//    public: 
//      Bmumu_reco_mumu(const std::string& t, const std::string& n,
//                      const IInterface* p);
//    ...
//
//   // Bmumu_reco_mumu.cxx:
//  Bmumu_reco_mumu::Bmumu_reco_mumu(const std::string& t,
//      const std::string& n,
//      const IInterface* p) : 
//    CfAthAlgTool(t,n,p),
//    ...
//
//   // inside a method like Bmumu_reco_mumu::addBranches():
//   ...
//    // add counter for number of events seen
//    addEvent("dimuEvents");
//    // add counter for the number of events with >= 1 reco'd vertices
//    if ( vtxContainer->size() > 0 ) {
//      addEvent("dimuWithVertexCand");
//    }
//    // add counter for the number of vertices
//    addToCounter("dimuNumVertices", vtxContainer->size());
//    ...
//
// Please note that a line for
//    addEvent(nameString, weight=1.);
// or
//    addToCounter(nameString, counts=1, weight=1.);
// is sufficient.
// In a case a counter with that name does not exist yet, it will be
// initialized automatically.
//
//============================================================================
//
#include "DerivationFrameworkBPhys/CfAthAlgTool.h"

namespace DerivationFramework {

  //--------------------------------------------------------------------------
  // Constructor
  CfAthAlgTool::CfAthAlgTool(const std::string& t,
			     const std::string& n,
			     const IInterface*  p) : 
    AthAlgTool(t,n,p),
    m_cutFlowSvc("CutFlowSvc/CutFlowSvc", n),
    m_ctbasename(n),
    m_bid(0), m_bidisset(false) {

    ATH_MSG_DEBUG("Calling constructor with parameters");

    // Declare counters base name
    declareProperty("CountersBaseName", m_ctbasename);

    // clean-up counter base name
    std::string fstr("ToolSvc.");
    std::string::size_type ind = m_ctbasename.find(fstr);
    if (ind != std::string::npos) m_ctbasename.erase(ind, fstr.length());

  }
  //--------------------------------------------------------------------------
  // Destructor
  CfAthAlgTool::~CfAthAlgTool() {
    
    ATH_MSG_DEBUG("Calling destructor");
  }
  //--------------------------------------------------------------------------
  // return a handle to an ICutFlowSvc instance
  ServiceHandle<ICutFlowSvc>& CfAthAlgTool::cutFlowSvc() const {

    return m_cutFlowSvc;
  }
  //--------------------------------------------------------------------------
  // Initialization method invoked by the framework.
  StatusCode CfAthAlgTool::sysInitialize() {

    // retrieve CutFlowSvc instance
    CHECK( cutFlowSvc().retrieve() );
    
    // re-direct to base class...
    return AthAlgTool::sysInitialize();
  }
  //--------------------------------------------------------------------------
  // add one event to a named counter -- returns true on success
  bool CfAthAlgTool::addEvent(std::string name, double weight) const {

    CutIdentifier id = getCounter(name);
    if ( id > 0 ) {
      cutFlowSvc()->addEvent(id, weight);
    }
    return (id > 0);
  }
  //--------------------------------------------------------------------------
  // add to a named counter -- returns true on success
  // if counts > 1 : same weight is added multiple times
  bool CfAthAlgTool::addToCounter(std::string name, uint64_t counts,
				  double weight) const {

    CutIdentifier id = getCounter(name);
    if ( id > 0 ) {
      for (uint64_t i=0; i<counts; ++i) {
	cutFlowSvc()->addEvent(id, weight);
      }
    }
    return (id > 0);
  }
  //--------------------------------------------------------------------------
  // add a counter by name -- simply returns id if counter already exists
  CutIdentifier CfAthAlgTool::getCounter(std::string name) const {

    CutIdentifier id = getCounterIdByName(name);
    if ( id < 1 ) {
      std::string fullname = m_ctbasename + "_" + name;
      if ( ! m_bidisset ) {
	id = cutFlowSvc()->registerFilter(fullname, "N/A");
	m_bid = id;
      } else {
	id = cutFlowSvc()->registerCut(fullname, "N/A", m_bid);
      }
      m_mctn[name] = id;
    }
    return id;
  }
  //--------------------------------------------------------------------------
  // returns counter name by id
  std::string CfAthAlgTool::getCounterNameById(CutIdentifier id) const {

    std::string res = "__NOT_FOUND__";

    for (NameIdMap_t::iterator it = m_mctn.begin(); it != m_mctn.end(); ++it) {
      if ( it->second == id ) {
	res = it->first;
	break;
      }
    }
    return res;
  }
  //--------------------------------------------------------------------------
  // returns counter id by name
  CutIdentifier CfAthAlgTool::getCounterIdByName(std::string name) const {

    CutIdentifier id = 0;
    
    NameIdMap_t::iterator it = m_mctn.find(name);
    if ( it != m_mctn.end() ) {
      id = m_mctn[name];
    }
    return id;
  }
  //--------------------------------------------------------------------------
} // namespace
