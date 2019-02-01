/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_EVENTTAGWRITER_H
#define ATHENAPOOLMULTITEST_EVENTTAGWRITER_H

/**
 * @file EventTagWriter.h
 * @brief Definition of class EventTagWriter
 * @author K. Karr <Kristo.Karr@cern.ch>
 * $Id: EventTagWriter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "xAODEventInfo/EventInfo.h"
#include <string>

namespace coral
{
  class AttributeListSpecification;
}

/**
 * @class EventTagWriter
 * @brief Algorithm to store dummy event tag metadata in an attribute list object.
 *
 * This algorithm creates dummy event tag metadata, stores this metadata in a 
 * POOL attribute list object, and records the attribute list object in the 
 * TDS so that it may be retrieved further downstream and added to an event 
 * tag collection along with a reference to the event's data header object.
 */

class EventTagWriter : public AthAlgorithm 
{
  public:

    /** 
     * @brief Standard constructor
     * @param name Name of this Algorithm class as recognized by Athena
     * @param pSvcLocator Athena services accessor
     */
    EventTagWriter(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Default destructor
    virtual ~EventTagWriter();

    /** 
     * @brief Defines event tag metadata schema
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
    SG::ReadHandleKey<xAOD::EventInfo> m_evt;

    /// Specification of event tag metadata schema
    coral::AttributeListSpecification* m_attribListSpec;

    /// Maximum number of particles expected for any type
    unsigned int m_maxNum;
};

#endif // ATHENAPOOLMULTITEST_EVENTTAGWRITER_H
