/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLMULTITEST_EVENTTAGREADER_H
#define ATHENAPOOLMULTITEST_EVENTTAGREADER_H

/**
 * @file EventTagReader.h
 * @brief Definition of class EventTagReader
 * @author K. Karr (Kristo.Karr@cern.ch)
 * $Id: EventTagReader.h,v 1.1 2009-02-19 22:04:49 cranshaw Exp $
 */

//#include "GaudiKernel/Algorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"

#include <map>

class StoreGateSvc;

/**
 * @class EventTagReader
 * @brief Algorithm to read back event tag metadata of events satisfying a query.
 *
 * This algorithm opens an existing collection of dummy event tag metadata
 * created by the EventTagWriter algorithm, gets an iterator over the events
 * in the collection that satisfy a query, iterates over this subset of events
 * and prints out the list of event tag metadata values for each of these 
 * events.
 */

class EventTagReader : public AthAlgorithm
{
  public:

    /**
     * @brief Standard constructor
     * @param name Name of this class as recognized by Athena
     * @param pSvcLocator Athena services accessor
     */
    EventTagReader( const std::string& name, ISvcLocator* pSvcLocator );

    /// Default destructor
    virtual ~EventTagReader();

    /** 
     * @brief Opens existing collection for read
     * @return Status of method execution success
     */
    virtual StatusCode initialize();

    /**
     * @brief Retrieves metadata of events in collection that satisfy a query
     * @return Status of method execution success
     */
    virtual StatusCode execute();

    /**
     * @brief Prints confirmation message
     * @return Status of method execution success
     */
    virtual StatusCode finalize();

  private:

    /// Store Gate service accessor
    StoreGateSvc* m_storeGateSvc;

};
#endif
     
