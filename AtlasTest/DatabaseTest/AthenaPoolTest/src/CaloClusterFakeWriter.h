/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_CALOCLUSTERFAKEWRITE_H
# define SRC_CALOCLUSTERFAKEWRITE_H

/**
 * @file CaloClusterFakeWriter.h
 *
 * @brief Test Algorithm for POOL I/O tests, writes CaloCluster
 * objects to the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeWriter.h,v 1.2 2005-01-10 18:05:51 schaffer Exp $
 *
 */

/**
 * @class CaloClusterFakeWriter
 *
 * @brief Test Algorithm POOL I/O tests, writes CaloCluster objects
 * to the transient store
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"

class LArEM_ID;
class CaloCellContainer;
class CaloClusterContainer;

class CaloClusterFakeWriter : public AthAlgorithm {
public:

    /// Algorithm constructor
    CaloClusterFakeWriter(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm destructor
    ~CaloClusterFakeWriter();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    CaloClusterFakeWriter();

    /// Avoid use of copy constructor
    CaloClusterFakeWriter(const CaloClusterFakeWriter&);

    /// Avoid use of copy operator
    CaloClusterFakeWriter &operator=(const CaloClusterFakeWriter&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Print method for CaloCellContainer
    void printCells(const CaloCellContainer* caloCont) const;

    /// Print method for CaloClusterContainer
    void printCells(const CaloClusterContainer* clusCont) const;

    /// IDhelper for LAr needed for identifiable container
    const LArEM_ID*             m_laremId;
};


#endif // SRC_CALOCLUSTERFAKEWRITE_H
