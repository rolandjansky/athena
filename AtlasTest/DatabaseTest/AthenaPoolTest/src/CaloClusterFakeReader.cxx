/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloClusterFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, reads CaloCluster objects
 * from the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeReader.cxx,v 1.8 2008-08-24 11:02:49 schaffer Exp $
 *
 */

#include "CaloClusterFakeReader.h"

// Event includes
#include "CaloEvent/CaloClusterContainer.h"

// DetDescr includes
#include "CaloIdentifier/CaloCell_ID.h"

// Gaudi includes
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// test includes
#include "LArCellContFakeCreator.h"


// Constructor with parameters:
CaloClusterFakeReader::CaloClusterFakeReader(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_calocellId(0)
{}

CaloClusterFakeReader::~CaloClusterFakeReader()
{}

// Initialize method:
StatusCode CaloClusterFakeReader::initialize()
{
    ATH_MSG_INFO( "CaloClusterFakeReader::initialize()"  );
    ATH_CHECK( detStore()->retrieve(m_calocellId, "CaloCell_ID") );
    ATH_CHECK(m_caloMgrKey.initialize());
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode CaloClusterFakeReader::execute() 
{
    ATH_MSG_DEBUG( "CaloClusterFakeReader::execute()"  );

    SG::ReadCondHandle<CaloDetDescrManager> caloMgrHandle{m_caloMgrKey};
    ATH_CHECK(caloMgrHandle.isValid());

    const CaloClusterContainer* clusCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(clusCont,"LArClusterCont") );

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();

    LArCellContFakeCreator creator;

    // Create container
    const CaloCellContainer* caloCont1 = creator.createCaloCellContainer(m_calocellId,
									 *caloMgrHandle,
									 msg());

    CaloCellContainer::const_iterator first1 = caloCont1->begin();
    CaloCellContainer::const_iterator last1  = caloCont1->end();

    bool error = false;
    int ncells = 0;
    int ncellErrs = 0;

    for(; first != last; ++first) {
	CaloCluster::cell_iterator first2 = (*first)->cell_begin();
	CaloCluster::cell_iterator last2  = (*first)->cell_end();
    
	for(; first2 != last2 && first1 != last1; ++first2, ++first1) {
	    if (((*first2)->ID()    !=  (*first1)->ID()) ||
		(fabs((*first2)->energy()  - (*first1)->energy()  ) > 0.01*fabs((*first1)->energy())) || 
		(fabs((*first2)->time()    - (*first1)->time()    ) > 0.0002*fabs((*first1)->time())) || 
		(std::abs((*first2)->quality() - (*first1)->quality() ) > 0.0000001) || 
		(fabs((*first2)->eta()     - (*first1)->eta()     ) > 0.0000001) || 
		(fabs((*first2)->phi()     - (*first1)->phi()     ) > 0.0000001)) {
              ATH_MSG_ERROR( "LArCell1,2 differ: id "
                             << m_calocellId->show_to_string((*first2)->ID())
                             << m_calocellId->show_to_string((*first1)->ID()) << "\n"
                             << " energy  " << (*first2)->energy() << " "
                             << (*first1)->energy() << "\n"
                             << " time    " << (*first2)->time() << " "
                             << (*first1)->time() << "\n"
                             << " quality " << (*first2)->quality() << " "
                             << (*first1)->quality() << "\n"
                             << " eta     " << (*first2)->eta() << " "
                             << (*first1)->eta() << "\n"
                             << " phi     " << (*first2)->phi() << " "
                             << (*first1)->phi() );
		error = true;
		ncellErrs++;
	    }
	    ncells++;
	}
    }

    printCells(clusCont);

    if (error) {
        ATH_MSG_ERROR( "Errors: " << ncellErrs  );
	return StatusCode::FAILURE;
    }
    else {
      ATH_MSG_INFO( "Successful check of CaloCellContainer I/O. Cells read: " << ncells  );
    }
	
    return StatusCode::SUCCESS;
}

// Print cells
void CaloClusterFakeReader::printCells(const CaloClusterContainer* clusCont) const
{
    ATH_MSG_INFO( "CaloClusterFakeReader::printCells from cluster"  );

    // loop on all cells

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();
    for(; first != last; ++first) {

	const CaloCluster* clus = *first;

	ATH_MSG_DEBUG( "CaloCluster: e, eta, phi EMB1 "
                       << " " << (*first)->eSample  (CaloSampling::EMB1)
                       << " " << (*first)->etaSample(CaloSampling::EMB1)
                       << " " << (*first)->phiSample(CaloSampling::EMB1) );
	ATH_MSG_DEBUG( "CaloCluster: e, eta, phi EMB2 "
                       << " " << (*first)->eSample  (CaloSampling::EMB2)
                       << " " << (*first)->etaSample(CaloSampling::EMB2)
                       << " " << (*first)->phiSample(CaloSampling::EMB2) );
	ATH_MSG_DEBUG( "CaloCluster: e, eta, phi EMB3 "
                       << " " << (*first)->eSample  (CaloSampling::EMB3)
                       << " " << (*first)->etaSample(CaloSampling::EMB3)
                       << " " << (*first)->phiSample(CaloSampling::EMB3) );


	CaloCluster::cell_iterator first1 = clus->cell_begin();
	CaloCluster::cell_iterator last1  = clus->cell_end();
    
	for(; first1 != last1; ++first1) {
	
          ATH_MSG_DEBUG( "  CaloCell: id "
                         << m_calocellId->show_to_string((*first1)->ID())
                         << " energy  " << (*first1)->energy()
                         << " time    " << (*first1)->time()
                         << " quality " << (*first1)->quality()
                         << std::setprecision(5)
                         << " eta     " << (*first1)->eta()
                         << " phi     " << (*first1)->phi() );
	}
    }
    
    ATH_MSG_DEBUG( "CaloClusterFakeReader::printCells from cluster end"  );
}


// Finalize method:
StatusCode CaloClusterFakeReader::finalize() 
{
  ATH_MSG_INFO( "CaloClusterFakeReader::finalize()"  );
  return StatusCode::SUCCESS;
}


