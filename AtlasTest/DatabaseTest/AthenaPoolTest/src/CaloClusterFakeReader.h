/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTEST_CALOCLUSTERFAKEREADER_H
#define ATHENAPOOLTEST_CALOCLUSTERFAKEREADER_H

/**
 * @file CaloClusterFakeReader.h
 *
 * @brief Test Algorithm for POOL I/O tests, reads CaloCluster objects
 * from the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeReader.h,v 1.3 2006-12-20 16:48:05 schaffer Exp $
 *
 */

/**
 * @class CaloClusterFakeReader
 *
 * @brief Test Algorithm POOL I/O tests, reads CaloCluster objects
 * from the transient store
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell_ID;
class CaloCellContainer;
class CaloClusterContainer;


class CaloClusterFakeReader : public AthAlgorithm {
public:

    /// Algorithm constructor
    CaloClusterFakeReader(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm destructor
    ~CaloClusterFakeReader();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize() override;

    /// Algorithm execute once per event
    virtual StatusCode execute() override;

    /// Algorithm finalize at end of job
    virtual StatusCode finalize() override;
  
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    CaloClusterFakeReader();

    /// Avoid use of copy constructor
    CaloClusterFakeReader(const CaloClusterFakeReader&);

    /// Avoid use of copy operator
    CaloClusterFakeReader &operator=(const CaloClusterFakeReader&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Print method
    void printCells(const CaloClusterContainer* clusCont) const;

    /// DetDescr mgr for access to the id helper
    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };

    /// IDhelper for LAr needed for identifiable container
    const CaloCell_ID*             m_calocellId;
};


#endif // SRC_CALOCLUSTERFAKEREADE_H
