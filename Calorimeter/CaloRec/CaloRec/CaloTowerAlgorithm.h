/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOREC_CALOTOWERALGORITHM_H
#define CALOREC_CALOTOWERALGORITHM_H
///////////////////////////////////////////////////////////////////////////////
/// \brief CaloTower builder algorithm
///
/// The CaloTowerAlgorithm executes a sequence of CaloTowerBuilderTools 
/// defined in the jobOptions. It creates, records and locks a 
/// CaloTowerContainer into StoreGate. Even though this class is probably
/// sufficient for all tower builder strategies, it has been designed as
/// a concrete base class, thus allowing to overwrite the default behavior with
/// very specific client implementations.
///
/// \author Peter Loch <loch@physics.arizona.edu>
/// \date May 3, 2004 - first implementation
///
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ToolHandle.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include <string>
#include <vector>
#include <map>

class StoreGateSvc;
class CaloTowerContainer;
class ICaloTowerBuilderToolBase;

class CaloTowerAlgorithm: public AthAlgorithm {
  public:

    typedef std::string tool_key;
    typedef CaloTowerBuilderToolBase tool_type;
    typedef std::vector<tool_type*> tool_store;
    typedef tool_store::const_iterator tool_iterator;
    typedef std::map<tool_key, unsigned int> tool_stats;
    typedef tool_stats::const_iterator tool_stats_iterator;

    /// Algorithm constructor
    CaloTowerAlgorithm(const std::string& name, ISvcLocator* pService);

    virtual ~CaloTowerAlgorithm();

    /// inherited from Algorithm
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  protected:

    ////////////////
    // Properties //
    ////////////////

    // tower grid specs
    unsigned int m_nEtaTowers;
    unsigned int m_nPhiTowers;
    double m_minEta, m_maxEta;

    // link output container
    bool m_genericLink;

    // tool collection
    std::vector<std::string> m_toolNames;
    ToolHandleArray<ICaloTowerBuilderToolBase> m_ptools;
    // output tower container name
    std::string m_towerContainerName;

    /////////////////////////
    // Stores and Services //
    /////////////////////////

    // list of tools
    tool_store m_tools;

    // tool invocation stats
    tool_stats m_toolInvoke;
    tool_stats m_toolAccept;
    tool_stats m_toolReject;

};
#endif
