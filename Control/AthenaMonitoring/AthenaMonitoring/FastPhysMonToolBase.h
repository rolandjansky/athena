/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-
#ifndef FASTPHYSMONTOOLBASE_H
#define FASTPHYSMONTOOLBASE_H

#include "AthenaMonitoring/ManagedMonitorToolBase.h"
#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/IIncidentListener.h"

class TTree;


/**
 * @class FastPhysMonToolBase
 * @brief Base-class for MonTools used in the context of "Fast Physics Monitoring"
 * @author Walter Lampl, March 2011
 * 
 * Derives from ManagedMonitorToolBase 
 * TTrees registed with the regFPMTree method of this class will have 
 * RunNumber, EventNumber and LumiBlockN as additonal variables
 *
 * If other commonalities among the Fast Physics Monitoring tools are identified,
 * we could implement them here too. 
 */

// Legacy ManagedMonitorToolBase is not thread-safe.
class ATLAS_NOT_THREAD_SAFE FastPhysMonToolBase: public ManagedMonitorToolBase, 
                                                 public IIncidentListener {

 public:
  /**
   * @brief Regular AlgTool Constructor
   */
  FastPhysMonToolBase( const std::string & type, const std::string & name,
		       const IInterface* parent ) ;


  /**
   * @brief Destructor
   */
  virtual ~FastPhysMonToolBase();

  /** 
   * @brief Regular initialize method
   */
  virtual StatusCode initialize();


  /**
   * @brief Method to register a Fast Physics Monitoring TTree 
   * @param tree Pointer to the tree to be registered  
   * @param systemName Appears as folder name in the ROOT file
   * @param chain Forwarded to ManagedMonToolBase::regTree
   * @param merge Forwarded to ManagedMonToolBase::regTree
   *
   * Forwards to regTree method of ManagedMonitorToolBase
   */
  StatusCode regFPMTree(TTree* tree,const std::string& systemName,const std::string& chain="", const std::string& merge="");


  /**
   * @brief Method to create and register a Fast Physics Monitoring TTree 
   * @return Pointer to the new tree
   * @param treeName ROOT name of the new tree
   * @param treeTitle Titel of the new tree 
   * @param systemName Appears as folder name in the ROOT file
   * @param chain Forwarded to ManagedMonToolBase::regTree
   * @param merge Forwarded to ManagedMonToolBase::regTree
   *
   * Forwards to regTree method of ManagedMonitorToolBase
   */
  TTree* regFPMTree(const std::string& treeName, const std::string treeTitle, const std::string& systemName, 
		    const std::string& chain="", const std::string& merge="");


  /**
   * @brief book method called by the framework
   *
   * Calls the bookNtuple() method exactly once per job
   * Can be overloaded by concrete tool if more specialization is needed
   */
  virtual StatusCode bookHistograms( );

  /** 
   * @brief bookNtuple method called by the framework
   *
   * Needs to be implemented by the concrete FastPhysMonTool!
   */
  virtual StatusCode bookNtuple();

 private:

  /** 
   * @brief Incident handle method
   */
  void handle(const Incident&); 

  //Basic Ntuple variables
  uint32_t m_eventNumber;
  uint32_t m_runNumber;
  uint32_t m_LBNumber;
  double m_mcEventWeight;

  bool m_booked;

};


#endif
