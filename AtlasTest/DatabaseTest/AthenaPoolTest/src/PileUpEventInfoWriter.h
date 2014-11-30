/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_PILEUPEVENTINFOWRITER_H
# define SRC_PILEUPEVENTINFOWRITER_H

/**
 * @file PileUpEventInfoWriter.h
 *
 * @brief Test reading of PileUpEventInfo
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: PileUpEventInfoWriter.h,v 1.1 2005-03-09 15:44:39 schaffer Exp $
 *
 */

/**
 * @class PileUpEventInfoWriter
 *
 * @brief Test reading of PileUpEventInfo
 *
 */


//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IIncidentListener.h"

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class PileUpEventInfoWriter : public Algorithm,
			      virtual public IIncidentListener
{
public:

    /// Algorithm constructor
    PileUpEventInfoWriter(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    /// incident service handle for EndEvent
    void handle(const Incident& incident);


    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    PileUpEventInfoWriter();

    /// Avoid use of copy constructor
    PileUpEventInfoWriter(const PileUpEventInfoWriter&);

    /// Avoid use of copy operator
    PileUpEventInfoWriter &operator=(const PileUpEventInfoWriter&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Access to event store
    StoreGateSvc*   m_storeGate;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_PILEUPEVENTINFOWRITER_H
