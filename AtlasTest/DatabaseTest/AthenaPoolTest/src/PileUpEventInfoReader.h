/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_PILEUPEVENTINFOREADER_H
# define SRC_PILEUPEVENTINFOREADER_H

/**
 * @file PileUpEventInfoReader.h
 *
 * @brief Test reading of PileUpEventInfo
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: PileUpEventInfoReader.h,v 1.1 2005-03-09 15:44:39 schaffer Exp $
 *
 */

/**
 * @class PileUpEventInfoReader
 *
 * @brief Test reading of PileUpEventInfo
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"


class IGeoModelSvc;


class PileUpEventInfoReader : public AthAlgorithm {
public:

    /// Algorithm constructor
    PileUpEventInfoReader(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    PileUpEventInfoReader();

    /// Avoid use of copy constructor
    PileUpEventInfoReader(const PileUpEventInfoReader&);

    /// Avoid use of copy operator
    PileUpEventInfoReader &operator=(const PileUpEventInfoReader&);
};


#endif // SRC_PILEUPEVENTINFOREADER_H
