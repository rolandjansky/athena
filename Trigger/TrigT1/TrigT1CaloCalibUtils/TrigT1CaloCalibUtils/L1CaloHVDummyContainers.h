/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1CALOHVDUMMYCONTAINERS_H
#define L1CALOHVDUMMYCONTAINERS_H

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>

// forward declrations
class CaloIdManager;
class CaloCell_ID;
class CaloDetDescriptor;
class CaloDetDescrElement;
class CaloDetDescrManager;

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

    StatusCode initialize();
    StatusCode execute();
    StatusCode finalize();

 private:
    std::string m_triggerTowerCollectionName;
    std::string m_caloCellContainerName;

    const CaloIdManager* m_caloMgr;
    const CaloCell_ID* m_caloCellHelper;
    const CaloDetDescrManager* m_caloDetDescrMgr;

    bool m_firstEvent;
};

#endif // L1CALOHVDUMMYCONTAINERS_H
