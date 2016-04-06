/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_LARCELLCONTFAKEWRITER_H
# define SRC_LARCELLCONTFAKEWRITER_H

/**
 * @file LArCellContFakeWriter.h
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeWriter.h,v 1.4 2006-12-20 16:48:06 schaffer Exp $
 *
 */

/**
 * @class LArCellContFakeWriter
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"

class CaloDetDescrManager;
class CaloCell_ID;
class CaloCellContainer;


class LArCellContFakeWriter : public AthAlgorithm {
public:

    /// Algorithm constructor
    LArCellContFakeWriter(const std::string &name,ISvcLocator *pSvcLocator);
  
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
    LArCellContFakeWriter();

    /// Avoid use of copy constructor
    LArCellContFakeWriter(const LArCellContFakeWriter&);

    /// Avoid use of copy operator
    LArCellContFakeWriter &operator=(const LArCellContFakeWriter&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Print out cell info
    void printCells(const CaloCellContainer* larCont) const;

    /// Need DD mgr to create cells
    const CaloDetDescrManager* 	m_caloMgr;

    /// Need id helper to create cells
    const CaloCell_ID*             m_calocellId;
};


#endif // SRC_LARCELLCONTFAKEWRITER_H
