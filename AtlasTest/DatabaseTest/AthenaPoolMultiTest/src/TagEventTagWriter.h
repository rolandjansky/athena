/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_TAGEVENTTAGWRITER_H
#define ATHENAPOOLMULTITEST_TAGEVENTTAGWRITER_H

/**
 * @file TagEventTagWriter.h
 * @brief Definition of class TagEventTagWriter
 * @author K. Karr <Kristo.Karr@cern.ch>
 * $Id: TagEventTagWriter.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/StoreGateSvc.h"
#include "xAODEventInfo/EventInfo.h"
#include <string>

class AthenaAttributeListSpecification;

/**
 * @class TagEventTagWriter
 * @brief Algorithm to store dummy event tag metadata in an attribute list object.
 *
 * This algorithm creates dummy event tag metadata, stores this metadata in a 
 * POOL attribute list object, and records the attribute list object in the 
 * TDS so that it may be retrieved further downstream and added to an event 
 * tag collection along with a reference to the event's data header object.
 */

class TagEventTagWriter : public AthAlgorithm 
{
  public:

    /** 
     * @brief Standard constructor
     * @param name Name of this Algorithm class as recognized by Athena
     * @param pSvcLocator Athena services accessor
     */
    TagEventTagWriter(const std::string& name, ISvcLocator* pSvcLocator);
    
    /// Default destructor
    virtual ~TagEventTagWriter();

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
    AthenaAttributeListSpecification* m_attribListSpec;

    /// Maximum number of particles expected for any type
    unsigned int m_maxNum;
};

#endif // ATHENAPOOLMULTITEST_TAGEVENTTAGWRITER_H
