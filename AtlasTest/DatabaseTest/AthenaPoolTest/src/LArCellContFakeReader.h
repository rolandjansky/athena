/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_LARCELLCONTFAKEREADER_H
# define SRC_LARCELLCONTFAKEREADER_H

/**
 * @file LArCellContFakeReader.h
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeReader.h,v 1.4 2006-12-20 16:48:06 schaffer Exp $
 *
 */

/**
 * @class LArCellContFakeReader
 *
 * @brief Test Algorithm for POOL I/O uses LArCellContainer as test
 * data
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"


class CaloDetDescrManager;
class CaloCell_ID;
class CaloCellContainer;


class LArCellContFakeReader : public AthAlgorithm {
public:

    /// Algorithm constructor
    LArCellContFakeReader(const std::string &name,ISvcLocator *pSvcLocator);
  
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
    LArCellContFakeReader();

    /// Avoid use of copy constructor
    LArCellContFakeReader(const LArCellContFakeReader&);

    /// Avoid use of copy operator
    LArCellContFakeReader &operator=(const LArCellContFakeReader&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Print out cell info
    void printCells(const CaloCellContainer* larCont) const;

    /// Need DD mgr to create cells
    const CaloDetDescrManager* 	m_caloMgr;

    /// Need id helper to create cells
    const CaloCell_ID*          m_calocellId;
};

#endif // SRC_LARCELLCONTFAKEREADER_H
