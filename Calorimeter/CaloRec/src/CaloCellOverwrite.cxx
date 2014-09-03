/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Calorimeter/CaloRec includes
#include "CaloRec/CaloCellOverwrite.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"
#include "CaloEvent/CaloCellContainer.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
CaloCellOverwrite::CaloCellOverwrite( const std::string& name, 
			  ISvcLocator* pSvcLocator ) : 
  ::AthAlgorithm( name, pSvcLocator ) {
  declareProperty("FactorsPerGroup", m_factorsPerGroupInput );
  declareProperty("ContainerKey",m_containerKey);
}

// Destructor
CaloCellOverwrite::~CaloCellOverwrite()
{}

// Athena Algorithm's Hooks
StatusCode CaloCellOverwrite::initialize() {

  const CaloCell_ID* caloCellId;
  StatusCode sc=detStore()->retrieve(caloCellId,"CaloCell_ID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve CaloCell_ID helper" << endreq;
    return sc;
  }


  if (!m_factorsPerGroup.setDefinition(caloCellId,m_factorsPerGroupInput,msg())) {
    msg(MSG::ERROR) << "Failed to initialize CaloCellGroup object from jobOptions:" << endreq;
    for (unsigned i=0;i<m_factorsPerGroupInput.size();++i) 
      msg(MSG::ERROR) << m_factorsPerGroupInput[i] << endreq;
    return StatusCode::FAILURE;
  }

  if (m_factorsPerGroup.getDefaults().size()!=1) {
    msg(MSG::ERROR) << "Expected a CaloCellGroup definition of size 1, got " << m_factorsPerGroup.getDefaults().size() << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CaloCellOverwrite::finalize() {

  return StatusCode::SUCCESS;
}

StatusCode CaloCellOverwrite::execute() {  
  
  const CaloCellContainer* input;

  StatusCode sc=evtStore()->retrieve(input,m_containerKey);
  if (sc. isFailure()) {
    msg(MSG::ERROR) << "Failed to retrieve CaloCellContainer with key " <<m_containerKey << endreq;
    return sc;
  }

  CaloCellContainer* cellCont=const_cast<CaloCellContainer*>(input);


  CaloCellContainer::iterator it=cellCont->begin();
  CaloCellContainer::iterator it_e=cellCont->end();
  for(;it!=it_e;++it) {
    CaloCell* cell=(*it);
    const float factor=m_factorsPerGroup.valuesForCell(cell->ID())[0];
    cell->setEnergy(cell->energy()*factor);
  }
  
  return StatusCode::SUCCESS;
}

