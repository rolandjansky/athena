/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTEST_LARCELLCONTFAKECREATOR_H
#define ATHENAPOOLTEST_LARCELLCONTFAKECREATOR_H

/**
 * @file LArCellContFakeCreator.h
 *
 * @brief Create a CaloCellContainer with LArCells for POOL I/O tests
 * algs
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeCreator.h,v 1.11 2008-11-22 17:35:59 schaffer Exp $
 *
 */

/**
 * @class LArCellContFakeCreator
 *
 * @brief Create a CaloCellContainer with LArCells for POOL I/O tests
 * algs
 *
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/MsgStream.h"
#include "CaloEvent/CaloCellContainer.h"
#include "LArRecEvent/LArCell.h"

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class LArCellContFakeCreator
{
public:
    
    /// Default constructor
    LArCellContFakeCreator();

    /// Create a CaloCellContainer with LAr cells
    CaloCellContainer* createCaloCellContainer(const CaloCell_ID* calocellId, 
					       const CaloDetDescrManager* caloMgr,
					       MsgStream& log) const;
    

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline LArCellContFakeCreator::LArCellContFakeCreator()
{
}


inline CaloCellContainer* 
LArCellContFakeCreator::createCaloCellContainer(const CaloCell_ID* calocellId, 
						const CaloDetDescrManager* caloMgr,
						MsgStream& log) const
{


    // Create container
    CaloCellContainer* caloCont = new CaloCellContainer();


    log << MSG::DEBUG << "Created CaloCellContainer" << endmsg;

    // add  10 cells to container
    for (int eta = 1; eta < 5; ++eta) {
	for (int phi = 3; phi < 5; ++phi) {

	    Identifier     id         = calocellId->cell_id (0, 1, 1, 0, eta, phi);
	    IdentifierHash idHash     = calocellId->calo_cell_hash (id);
	    const CaloDetDescrElement* elem = caloMgr->get_element (idHash);
	    
	    //log << MSG::DEBUG << "Found elem" << elem << endmsg;

	    double energy    = 5.0*eta;
	    double time      = 0.0;
	    uint16_t quality = 0;
	    uint16_t prov    = 0;
	    // time and quality/prov use the same bits - only write one out
	    if (4 == phi) {
		time    = 10.0*phi;
		quality = 2*eta;
		prov    = 2*phi | 0x2000;
	    }
	    CaloGain::CaloGain gain=CaloGain::LARHIGHGAIN ;

	    LArCell* cell = new LArCell(elem, energy, time, quality, prov, gain);
	    caloCont->push_back(cell);

	    //log << MSG::DEBUG << "Created CaloCellContainer" << endmsg;

	}
    }

    return (caloCont);
}

#endif // SRC_LARCELLCONTFAKECREATOR_H



