/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTEST_LARCELLCONTFAKEWRITER_H
#define ATHENAPOOLTEST_LARCELLCONTFAKEWRITER_H

/**
 * @file LArCellContFakeWriter.h
 *
 * @brief Test Algorithm for POOL I/O uses CaloCellContainer for LAr as test
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
 * @brief Test Algorithm for POOL I/O uses CaloCellContainer for LAr as test
 * data
 *
 */


#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "StoreGate/ReadCondHandleKey.h"

class CaloCell_ID;
class CaloCellContainer;


class LArCellContFakeWriter : public AthAlgorithm {
public:

    /// Algorithm constructor
    LArCellContFakeWriter(const std::string &name,ISvcLocator *pSvcLocator);
  
    /// Algorithm initialize at begin of job
    virtual StatusCode initialize() override;

    /// Algorithm execute once per event
    virtual StatusCode execute() override;

    /// Algorithm finalize at end of job
    virtual StatusCode finalize() override;

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
    SG::ReadCondHandleKey<CaloDetDescrManager> m_caloMgrKey { this
	, "CaloDetDescrManager"
	, "CaloDetDescrManager"
	, "SG Key for CaloDetDescrManager in the Condition Store" };

    /// Need id helper to create cells
    const CaloCell_ID*             m_calocellId;
};


#endif // SRC_LARCELLCONTFAKEWRITER_H
