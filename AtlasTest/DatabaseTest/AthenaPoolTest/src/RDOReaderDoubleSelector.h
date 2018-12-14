/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RDOREADERDOUBLESELECTOR_H
#define RDOREADERDOUBLESELECTOR_H

/**
 * @file RDOReaderDoubleSelector.h
 *
 * @brief Test Algorithm for POOL I/O, reads InDetRawData and CaloCells from
 * transient store.
 *
 * @author Miha Muskinja <miha.muskinja@cern.ch>
 *
 */

// Base class
#include "AthenaBaseComps/AthAlgorithm.h"

class PixelID;
class SCT_ID;
class TRT_ID;
class CaloDetDescrManager;
class CaloCell_ID;
class CaloCellContainer;

class RDOReaderDoubleSelector : public AthAlgorithm {

    ///////////////////////////////////////////////////////////////////
    // Public methods:
    ///////////////////////////////////////////////////////////////////
public:

    /// Algorithm constructor
    RDOReaderDoubleSelector(const std::string &name,ISvcLocator *pSvcLocator);

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
    RDOReaderDoubleSelector();

    /// Avoid use of copy constructor
    RDOReaderDoubleSelector(const RDOReaderDoubleSelector&);

    /// Avoid use of copy operator
    RDOReaderDoubleSelector &operator=(const RDOReaderDoubleSelector&);
  
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

    /// Compare objects read with newly created ones
    StatusCode checkCells() const;

    /// Print out cell info
    void printCells(const CaloCellContainer* larCont) const;

    /// IDhelper for creating objects
    const PixelID* m_pixelId;

    /// IDhelper for creating objects
    const SCT_ID* m_sctId;

    /// IDhelper for creating objects
    const TRT_ID* m_trtId;

    /// Need DD mgr to create cells
    const CaloDetDescrManager*  m_caloMgr;

    /// Need id helper to create cells
    const CaloCell_ID*          m_calocellId;
};

#endif // RDOREADERDOUBLESELECTOR_H
