/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file LArCellContFakeReader.cxx
 *
 * @brief Test Algorithm for POOL I/O uses CaloCellContainer for LAr as test
 * data
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: LArCellContFakeReader.cxx,v 1.10 2008-11-22 16:59:48 schaffer Exp $
 *
 */



#include "LArCellContFakeReader.h"

// DetDescr includes
#include "CaloIdentifier/CaloCell_ID.h"

// test includes
#include "LArCellContFakeCreator.h"


// Constructor with parameters:
LArCellContFakeReader::LArCellContFakeReader(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_calocellId(nullptr)
{}

// Initialize method:
StatusCode LArCellContFakeReader::initialize()
{
    ATH_MSG_INFO( "LArCellContFakeReader::initialize()"  );
    ATH_CHECK( detStore()->retrieve(m_calocellId, "CaloCell_ID") );
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode LArCellContFakeReader::execute() 
{
    ATH_MSG_DEBUG("LArCellContFakeReader::execute()" );

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
    ATH_CHECK(caloMgrHandle.isValid());

    // Retrieve container
    const CaloCellContainer* caloCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(caloCont,"CaloCellCont1") );
    ATH_MSG_DEBUG( "Container '" << "CaloCellCont"
                   << "' retrieved from StoreGate"  );

    LArCellContFakeCreator creator;

    // Create container
    MsgStream log(msgSvc(), name());
    const CaloCellContainer* caloCont1 = creator.createCaloCellContainer(m_calocellId,
									 *caloMgrHandle,
									 msg());

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    CaloCellContainer::const_iterator first1 = caloCont1->begin();
    CaloCellContainer::const_iterator last1  = caloCont1->end();
    
    bool error = false;
    int ncells = 0;
    int ncellErrs = 0;
    
    for(; first != last && first1 != last1; ++first, ++first1) {
	if (((*first)->ID()    !=  (*first1)->ID()) ||
	    (fabs((*first)->energy()  - (*first1)->energy()  ) > 0.01*fabs((*first)->energy())) || 
	    (fabs((*first)->time()    - (*first1)->time()    ) > 0.0002*fabs((*first)->time())) || 
	    (std::abs((*first)->quality() - (*first1)->quality() ) > 0) || 
	    (std::abs((*first)->provenance() - (*first1)->provenance() ) > 0) || 
	    (fabs((*first)->eta()     - (*first1)->eta()     ) > 0.0000001) || 
	    (fabs((*first)->phi()     - (*first1)->phi()     ) > 0.0000001)) {
	    ATH_MSG_ERROR("CaloCell1,2 differ: id "
		<< m_calocellId->show_to_string((*first)->ID())
		<< m_calocellId->show_to_string((*first1)->ID())
		<< " energy  " << (*first)->energy() << " "
		<< (*first1)->energy() 
		<< " time    " << (*first)->time() << " "
		<< (*first1)->time()
		<< " quality " << (*first)->quality() << " "
		<< (*first1)->quality() 
		<< " provenance " << (*first)->provenance() << " "
		<< (*first1)->provenance() 
		<< " eta     " << (*first)->eta() << " "
		<< (*first1)->eta() 
		<< " phi     " << (*first)->phi() << " "
		<< (*first1)->phi());
	    error = true;
	    ncellErrs++;
	}
	ncells++;
    }

    printCells(caloCont);

    if (error) {
	ATH_MSG_ERROR("Errors: " << ncellErrs );
	return StatusCode::FAILURE;
    }
    else {
	ATH_MSG_INFO("Successful check of CaloCellContainer I/O. Cells read: " << ncells );
    }
	
    return StatusCode::SUCCESS;
}

// Print cells
void LArCellContFakeReader::printCells(const CaloCellContainer* caloCont) const
{

    ATH_MSG_INFO("LArCellContFakeReader::printRDOs()" );

    // loop on all cells

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    
    for(; first != last; ++first) {
	
	ATH_MSG_DEBUG("CaloCell: id "
	    << m_calocellId->show_to_string((*first)->ID())
	    << std::setprecision(5)
	    << " energy  " << (*first)->energy()
	    << " time    " << (*first)->time()
	    << " quality " << (*first)->quality()
	    << " provenance " << (*first)->provenance()
	    << " eta     " << (*first)->eta()
	    << " phi     " << (*first)->phi()
	    );
    }
    
    ATH_MSG_DEBUG("LArCellContFakeReader::printCells() end" );

}


// Finalize method:
StatusCode LArCellContFakeReader::finalize() 
{
    ATH_MSG_INFO("LArCellContFakeReader::finalize()" );

    return StatusCode::SUCCESS;
}


