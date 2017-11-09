/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PrimaryDPDMaker/CaloCellThinningTool.h"
#include "AthenaKernel/IThinningSvc.h"
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"

#include <vector>
#include <string>

DerivationFramework::CaloCellThinningTool::CaloCellThinningTool(const std::string& t,
                                                                const std::string& n,
                                                                const IInterface* p ) :
  AthAlgTool(t,n,p),
  m_thinningSvc("ThinningSvc",n),
  m_caloCellId(-1)
{
  declareInterface<DerivationFramework::IThinningTool>(this);
  declareProperty("ThinningService", m_thinningSvc);
  declareProperty("CaloCellId", m_caloCellId);       
}

DerivationFramework::CaloCellThinningTool::~CaloCellThinningTool() 
{
}

StatusCode DerivationFramework::CaloCellThinningTool::initialize()
{
  ATH_MSG_VERBOSE("initialize() ...");
  return StatusCode::SUCCESS;
}  

StatusCode DerivationFramework::CaloCellThinningTool::finalize()
{
  ATH_MSG_VERBOSE("finalize() ...");
  return StatusCode::SUCCESS;
}

StatusCode DerivationFramework::CaloCellThinningTool::doThinning() const
{
  const CaloCellContainer* container = 0;
  CHECK(evtStore()->retrieve(container, "AllCalo"));

  if (!container){
    ATH_MSG_DEBUG("Couldn't retrieve CaloCellContainer with key AllCalo");
    return StatusCode::SUCCESS;
  }

  std::vector<bool> mask;
  
  CaloCellContainer::const_iterator tcdc_it = container->begin();
  CaloCellContainer::const_iterator cit = container->end();
 
  for (; tcdc_it != cit; ++tcdc_it) {

    bool match = true;

    const CaloCell* cell = (*tcdc_it);
    if (cell->caloDDE()->getSubCalo() != m_caloCellId) match = false;

    mask.push_back(match);
  }
  
  if (m_thinningSvc->filter(*container, mask).isFailure()) {
    ATH_MSG_ERROR("Application of thinning service failed! ");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
