/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file CaloClusterFakeWriter.cxx
 *
 * @brief Test Algorithm for POOL I/O tests, writes CaloCluster
 * objects to the transient store
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: CaloClusterFakeWriter.cxx,v 1.12 2008-08-27 10:08:03 schaffer Exp $
 *
 */

#include "CaloClusterFakeWriter.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"

// DetDescr includes
#include "CaloIdentifier/LArEM_ID.h"

// TES include
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/errorcheck.h"



namespace {

class CaloClusterSetStores
  : public CaloCluster
{
public:
  using CaloCluster::setStores;
  using CaloCluster::setDataLink;
  using CaloCluster::setCellLink;
};

}


CaloClusterFakeWriter::CaloClusterFakeWriter(const std::string &name, 
					     ISvcLocator *pSvcLocator) :
    AthAlgorithm(name,pSvcLocator),
    m_laremId(0)
{}

CaloClusterFakeWriter::~CaloClusterFakeWriter()
{}

// Initialize method:
StatusCode CaloClusterFakeWriter::initialize()
{
    ATH_MSG_INFO( "CaloClusterFakeWriter::initialize()"  );
    ATH_CHECK( detStore()->retrieve(m_laremId, "LArEM_ID") );
    return StatusCode::SUCCESS;
}

// Execute method:
StatusCode CaloClusterFakeWriter::execute() 
{
    ATH_MSG_DEBUG( "CaloClusterFakeWriter::execute()"  );

    const CaloCellContainer* caloCont = nullptr;
    ATH_CHECK( evtStore()->retrieve(caloCont,"CaloCellCont1") );

    // print out cells
    printCells(caloCont);

    // create clusters
    CaloClusterContainer* lcc = new CaloClusterContainer();

    CHECK( evtStore()->record (lcc, "LArClusterCont") );
    CaloShowerContainer* pDataStore = new CaloShowerContainer;
    CHECK( evtStore()->record (pDataStore, "LArClusterCont_Data") );
    CaloCellLinkContainer* pCellLinkContainer = new CaloCellLinkContainer;
    CHECK( evtStore()->record (pCellLinkContainer, "LArClusterCont_Link") ); 

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    int i = 0;
    CaloCluster* clus = 0;
    
    for(; first != last; ++first, ++i) {
	
      ATH_MSG_DEBUG( "CaloCell: id "
                     << m_laremId->show_to_string((*first)->ID())
                     << " energy  " << (*first)->energy()
                     << " time    " << (*first)->time()
                     << " quality " << (*first)->quality()
                     << std::setprecision(5)
                     << " eta     " << (*first)->eta()
                     << " phi     " << (*first)->phi() );

	// Put two cells into a cluster
	if (i%2 == 0) {
	    if (clus) lcc->push_back(clus);  // Add previous one
            clus = new CaloCluster (0, 0, 0x80000000, CaloCluster::SW_55ele);
            CaloShower* pData = new CaloShower;
            CaloCellLink* pLink = new CaloCellLink;
            static_cast<CaloClusterSetStores*>(clus)->setStores (pData, pLink, true);

	}
	clus->addCell(caloCont, *first, 1.0);
	
    }
    if (clus) lcc->push_back(clus);  // Add last one
    clus = 0;

    // Create a dummy log to avoid debug printout for this alg
    for (CaloCluster* cl : *lcc) {
      static_cast<CaloClusterSetStores*>(cl)->setDataLink (pDataStore);
      static_cast<CaloClusterSetStores*>(cl)->setCellLink (pCellLinkContainer);
    }

    CHECK( evtStore()->setConst (lcc) );
    CHECK( evtStore()->setConst (pDataStore) );
    CHECK( evtStore()->setConst (pCellLinkContainer) );

    // print out cells from cluster container
    printCells(lcc);


    return StatusCode::SUCCESS;
}

// Print cells
void CaloClusterFakeWriter::printCells(const CaloCellContainer* caloCont) const
{

    ATH_MSG_INFO( "CaloClusterFakeWriter::printCells from Calo cont"  );

    // loop on all cells

    CaloCellContainer::const_iterator first = caloCont->begin();
    CaloCellContainer::const_iterator last  = caloCont->end();
    
    for(; first != last; ++first) {
	
      ATH_MSG_DEBUG( "CaloCell: id "
                     << m_laremId->show_to_string((*first)->ID())
                     << " energy  " << (*first)->energy()
                     << " time    " << (*first)->time()
                     << " quality " << (*first)->quality()
                     << std::setprecision(5)
                     << " eta     " << (*first)->eta()
                     << " phi     " << (*first)->phi() );
    }
    
    ATH_MSG_DEBUG( "CaloClusterFakeWriter::printCells from Calo cont end"  );

}


// Print cells
void CaloClusterFakeWriter::printCells(const CaloClusterContainer* clusCont) const
{
    // limit precision for output comparison
    ATH_MSG_INFO( "CaloClusterFakeWriter::printCells from cluster"  );

    // loop on all cells

    CaloClusterContainer::const_iterator first = clusCont->begin();
    CaloClusterContainer::const_iterator last  = clusCont->end();
    for(; first != last; ++first) {

	const CaloCluster* clus = *first;

	ATH_MSG_DEBUG( "CaloCluster: e, eta, phi EMB1 "
                       << " " << (*first)->eSample  (CaloSampling::EMB1)
                       << std::setprecision(4)
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
                         << m_laremId->show_to_string((*first1)->ID())
                         << " energy  " << (*first1)->energy()
                         << " time    " << (*first1)->time()
                         << " quality " << (*first1)->quality()
                         << std::setprecision(5)
                         << " eta     " << (*first1)->eta()
                         << " phi     " << (*first1)->phi() );
	}
    }
    
    ATH_MSG_DEBUG( "CaloClusterFakeWriter::printCells from cluster end"  );
}


// Finalize method:
StatusCode CaloClusterFakeWriter::finalize() 
{
  ATH_MSG_INFO( "CaloClusterFakeWriter::finalize()"  );
  return StatusCode::SUCCESS;
}



