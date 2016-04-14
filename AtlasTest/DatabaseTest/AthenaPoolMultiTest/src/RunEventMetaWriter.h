/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_RUNEVENTMETAWRITER_H
#define ATHENAPOOLMULTITEST_RUNEVENTMETAWRITER_H

/**
 * @file RunEventMetaWriter.h
 * @brief Definition of class RunEventMetaWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * @author J. Cranshaw (Jack.Cranshaw@cern.ch)
 * $Id: RunEventMetaWriter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $ 
 */

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"

#include <string>

namespace coral
{
  class AttributeListSpecification;
}
//class AthenaAttributeListSpecification; 

/**
 * @class RunEventMetaWriter
 *
 * This algorithm acts as a utility to create a POOL collection of event 
 * references and their associated run and event number metadata from an input 
 * ROOT file or set of such files containing persistent event data. The 
 * resulting collection may be used to query the events via the run and/or 
 * event metadata.
 */

class RunEventMetaWriter : public AthAlgorithm 
{
  public:

    /**
     * @brief Standard constructor
     * @param name Name of this class as recognized by Athena
     * @param pSvcLocator Athena services accessor
     */
    RunEventMetaWriter(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Default destructor
    virtual ~RunEventMetaWriter();

    /**
     * @brief Defines attribute list specification object
     * @return Status of method execution success
     */
    virtual StatusCode initialize();

    /** 
     * @brief Stores dummy event tag metadata in attribute list object
     * @return Status of method execution success
     */
    virtual StatusCode start();
    virtual StatusCode execute();

    /**
     * @brief Prints confirmation message
     * @return Status of method execution success
     */
    virtual StatusCode finalize();

  private:
    /// Specification of the event tag metadata schema
    coral::AttributeListSpecification* m_attribListSpec;
};

#endif // ATHENAPOOLMULTITEST_RUNEVENTMETAWRITER_H
