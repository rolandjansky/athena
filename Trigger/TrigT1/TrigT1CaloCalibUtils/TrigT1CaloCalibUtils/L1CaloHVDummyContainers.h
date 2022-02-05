/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOHVDUMMYCONTAINERS_H
#define L1CALOHVDUMMYCONTAINERS_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

#include <string>

// forward declrations
class CaloIdManager;
class CaloCell_ID;
class CaloDetDescriptor;
class CaloDetDescrElement;

/**
 * This algorithms creates dummy containers so HV job will run without data.
 *
 * @author Peter Faulkner <pjwf@hep.ph.bham.ac.uk>
 */

class L1CaloHVDummyContainers : public AthAlgorithm
{
 public:
    L1CaloHVDummyContainers(const std::string& name, ISvcLocator *pSvcLocator);
    virtual ~L1CaloHVDummyContainers();

    virtual StatusCode initialize() override;
    virtual StatusCode execute() override;
    virtual StatusCode finalize() override;

 private:
    std::string m_triggerTowerCollectionName;
    std::string m_caloCellContainerName;

    const CaloIdManager* m_caloMgr;
    const CaloCell_ID* m_caloCellHelper;
    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };


    bool m_firstEvent;
};

#endif // L1CALOHVDUMMYCONTAINERS_H
