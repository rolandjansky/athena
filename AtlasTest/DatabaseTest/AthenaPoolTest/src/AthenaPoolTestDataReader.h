/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTestDataReader_h
#define AthenaPoolTestDataReader_h

/**
 * @file AthenaPoolTestAttrReader.h
 *
 * @brief Test Algorithm for POOL I/O tests, reads AthenaPoolData
 * objects from the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestDataReader.h,v 1.2 2005-01-10 18:05:51 schaffer Exp $
 *
 */

/**
 * @class AthenaPoolTestAttrReader
 *
 * @brief Test Algorithm POOL I/O tests, reads AthenaPoolData objects
 * from the transient store
 *
 */

class StoreGateSvc;

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"

class AthenaPoolTestDataReader : public AthAlgorithm
{

 public:

    /// Algorithm constructor
    AthenaPoolTestDataReader(const std::string& name, ISvcLocator* pSvcLocator);

    /// Algorithm destructor
    ~AthenaPoolTestDataReader();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();

};
#endif
