/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDETRAWDATAFAKEWRITER_H
#define INDETRAWDATAFAKEWRITER_H

/**
 * @file InDetRawDataFakeWriter.h
 *
 * @brief Test Algorithm for POOL I/O, writes InDetRawData to
 * transient store.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: InDetRawDataFakeWriter.h,v 1.5 2007-03-03 18:31:31 schaffer Exp $
 *
 */

/**
 * @class InDetRawDataFakeWriter
 *
 * @brief Test Algorithm for POOL I/O, writes InDetRawData to
 * transient store.
 *
 */

#include "AthenaBaseComps/AthAlgorithm.h"
#include "InDetRawData/PixelRDO_Container.h"
#include "InDetRawData/SCT_RDO_Container.h"
#include "InDetRawData/TRT_RDO_Container.h"

class PixelID;
class SCT_ID;
class TRT_ID;

class InDetRawDataFakeWriter : public AthAlgorithm {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Algorithm constructor
    InDetRawDataFakeWriter(const std::string &name,ISvcLocator *pSvcLocator);

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
    InDetRawDataFakeWriter();

    /// Avoid use of copy constructor
    InDetRawDataFakeWriter(const InDetRawDataFakeWriter&);

    /// Avoid use of copy operator
    InDetRawDataFakeWriter &operator=(const InDetRawDataFakeWriter&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Create pixel collections
    StatusCode createPixels() const;

    /// Create sct collections
    StatusCode createSCTs() const;

    /// Create trt collections
    StatusCode createTRTs() const;

    /// Print out RDOs
    StatusCode printRDOs() const;

    /// Identifiable containers are created only once
    PixelRDO_Container* m_pixCont ;

    /// Identifiable containers are created only once
    SCT_RDO_Container*  m_sctCont ;

    /// Identifiable containers are created only once
    TRT_RDO_Container*  m_trtCont ;

    /// IDhelper needed for identifiable container
    const PixelID* m_pixelId;

    /// IDhelper needed for identifiable container
    const SCT_ID* m_sctId;

    /// IDhelper needed for identifiable container
    const TRT_ID* m_trtId;
};

#endif // INDETRAWDATAFAKEWRITER_H
