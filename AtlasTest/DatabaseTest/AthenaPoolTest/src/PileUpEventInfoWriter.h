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


#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/IIncidentListener.h"


class StoreGateSvc;

class PileUpEventInfoWriter : public AthAlgorithm,
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
};


#endif // SRC_PILEUPEVENTINFOWRITER_H
