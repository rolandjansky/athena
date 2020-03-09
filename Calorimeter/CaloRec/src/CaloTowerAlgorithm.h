/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "CaloUtils/CaloTowerBuilderToolBase.h"

#include <string>
#include <vector>
#include <map>

class CaloTowerContainer;
class ICaloTowerBuilderToolBase;

class CaloTowerAlgorithm: public AthReentrantAlgorithm {
  public:

    typedef CaloTowerBuilderToolBase tool_type;
    typedef std::vector<tool_type*> tool_store;
    typedef tool_store::const_iterator tool_iterator;

    /// Algorithm constructor
    CaloTowerAlgorithm(const std::string& name, ISvcLocator* pService);

    virtual ~CaloTowerAlgorithm();

    /// inherited from Algorithm
    virtual StatusCode initialize() override;
    virtual StatusCode execute (const EventContext& ctx) const override;
    virtual StatusCode finalize() override;

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
    SG::WriteHandleKey<CaloTowerContainer> m_towerContainerKey;

    /////////////////////////
    // Stores and Services //
    /////////////////////////

    // list of tools
    tool_store m_tools;
};
#endif
