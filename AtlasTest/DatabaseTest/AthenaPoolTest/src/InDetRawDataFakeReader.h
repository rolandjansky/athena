/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATAFAKEREADER_H
#define INDETRAWDATAFAKEREADER_H

/**
 * @file InDetRawDataFakeReader.h
 *
 * @brief Test Algorithm for POOL I/O, reads InDetRawData from
 * transient store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeReader.h,v 1.3 2005-01-10 18:05:52 schaffer Exp $
 *
 */

/**
 * @class InDetRawDataFakeReader
 *
 * @brief Test Algorithm for POOL I/O, reads InDetRawData from
 * transient store.
 *
 */

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

class PixelID;
class SCT_ID;
class TRT_ID;

class InDetRawDataFakeReader : public AthAlgorithm {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Algorithm constructor
    InDetRawDataFakeReader(const std::string &name,ISvcLocator *pSvcLocator);

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
    InDetRawDataFakeReader();

    /// Avoid use of copy constructor
    InDetRawDataFakeReader(const InDetRawDataFakeReader&);

    /// Avoid use of copy operator
    InDetRawDataFakeReader &operator=(const InDetRawDataFakeReader&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Compare objects read with newly created ones
    StatusCode checkPixel() const;

    /// Compare objects read with newly created ones
    StatusCode checkSCT() const;

    /// Compare objects read with newly created ones
    StatusCode checkTRT() const;

    /// IDhelper for creating objects
    const PixelID* m_pixelId;

    /// IDhelper for creating objects
    const SCT_ID* m_sctId;

    /// IDhelper for creating objects
    const TRT_ID* m_trtId;
};

#endif // INDETRAWDATAFAKEREADER_H
