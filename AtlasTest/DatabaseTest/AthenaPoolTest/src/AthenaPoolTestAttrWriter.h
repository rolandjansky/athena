/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AthenaPoolTestAttrWriter_h
#define AthenaPoolTestAttrWriter_h

/**
 * @file AthenaPoolTestAttrWriter.h
 *
 * @brief Test Algorithm for testing creation of explicit collections,
 * writes an AttributeList to the transient store each event.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: AthenaPoolTestAttrWriter.h,v 1.4 2006-03-26 18:16:09 schaffer Exp $
 *
 */

/**
 * @class AthenaPoolTestAttrWriter
 *
 * @brief Test Algorithm for testing creation of explicit collections,
 * writes an AttributeList to the transient store each event.
 *
 */

// INCLUDE HEADER FILES:

#include "AthenaBaseComps/AthAlgorithm.h"

class AthenaAttributeList;

class AthenaPoolTestAttrWriter : public AthAlgorithm
{

public:

    /// Algorithm constructor
    AthenaPoolTestAttrWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Algorithm destructor
    ~AthenaPoolTestAttrWriter();
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();

private:

    /// Create attr list at intitialize and use each event
    AthenaAttributeList*               m_attributes;

};
#endif
