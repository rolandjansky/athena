/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTestDataWriter_h
#define AthenaPoolTestDataWriter_h

/**
 * @file AthenaPoolTestDataWriter.h
 *
 * @brief Test Algorithm for POOL I/O tests, writes AthenaPoolData
 * objects to the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestDataWriter.h,v 1.4 2005-01-10 18:05:51 schaffer Exp $
 *
 */

/**
 * @class AthenaPoolTestDataWriter
 *
 * @brief Test Algorithm POOL I/O tests, writes AthenaPoolData objects
 * to the transient store
 *
 */

class StoreGateSvc;

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"

class AthenaPoolTestDataWriter : public AthAlgorithm
{

public:

    /// Algorithm constructor
    AthenaPoolTestDataWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Algorithm destructor
    ~AthenaPoolTestDataWriter();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();

private:

    /// Create only part of the collections
    BooleanProperty m_partialCreate;

    /// For partial create read second half of collections
    BooleanProperty m_readOtherHalf;

    /// For partial create read first half of collections
    BooleanProperty m_readFirstHalf; 

};
#endif
