/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTestAttrReader_h
#define AthenaPoolTestAttrReader_h

/**
 * @file AthenaPoolTestAttrReader.h
 *
 * @brief Test Algorithm for reading an AttributeList from the
 * transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestAttrReader.h,v 1.2 2005-01-10 18:05:51 schaffer Exp $
 *
 */

/**
 * @class AthenaPoolTestAttrReader
 *
 * @brief Test Algorithm for reading an AttributeList from the
 * transient store
 *
 */

class StoreGateSvc;

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"

class EventSelectorAthenaPool;

class AthenaPoolTestAttrReader : public AthAlgorithm
{

public:

    /// Algorithm constructor
    AthenaPoolTestAttrReader(const std::string& name, ISvcLocator* pSvcLocator);

    /// Algorithm destructor
    ~AthenaPoolTestAttrReader();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();

};
#endif
     
