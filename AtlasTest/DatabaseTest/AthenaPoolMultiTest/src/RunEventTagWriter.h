/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_RUNEVENTTAGWRITER_H
#define ATHENAPOOLMULTITEST_RUNEVENTTAGWRITER_H

/**
 * @file RunEventTagWriter.h
 * @brief Definition of class RunEventTagWriter
 * @author K. Karr (Kristo.Karr@cern.ch)
 * @author J. Cranshaw (Jack.Cranshaw@cern.ch)
 * $Id: RunEventTagWriter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $ 
 */

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "xAODEventInfo/EventInfo.h"
#include "PersistentDataModel/AthenaAttributeList.h"

#include <string>

namespace coral
{
  class AttributeListSpecification;
}
class AthenaAttributeListSpecification; 
class AthenaAttributeList; 

/**
 * @class RunEventTagWriter
 *
 * This algorithm acts as a utility to create a POOL collection of event 
 * references and their associated run and event number metadata from an input 
 * ROOT file or set of such files containing persistent event data. The 
 * resulting collection may be used to query the events via the run and/or 
 * event metadata.
 */

class RunEventTagWriter : public AthAlgorithm 
{
  public:

    /**
     * @brief Standard constructor
     * @param name Name of this class as recognized by Athena
     * @param pSvcLocator Athena services accessor
     */
    RunEventTagWriter(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Default destructor
    virtual ~RunEventTagWriter();

    /**
     * @brief Defines attribute list specification object
     * @return Status of method execution success
     */
    virtual StatusCode initialize();

    /** 
     * @brief Stores dummy event tag metadata in attribute list object
     * @return Status of method execution success
     */
    virtual StatusCode execute();

    /**
     * @brief Prints confirmation message
     * @return Status of method execution success
     */
    virtual StatusCode finalize();

  private:

    StatusCode fillTag(const xAOD::EventInfo*, AthenaAttributeList*);

    SG::ReadHandleKey<xAOD::EventInfo> m_evt;
    SG::WriteHandleKey<AthenaAttributeList> m_attribList;

    /// Specification of the event tag metadata schema
    coral::AttributeListSpecification* m_attribListSpec;
};

#endif // ATHENAPOOLMULTITEST_RUNEVENTTAGWRITER_H
