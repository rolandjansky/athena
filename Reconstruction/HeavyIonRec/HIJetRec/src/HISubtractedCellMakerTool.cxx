/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "HISubtractedCellMakerTool.h"
#include "HICaloCellHelper.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "HIEventUtils/HIEventDefs.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HICaloRange.h"

#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"

#include <algorithm>

//**********************************************************************

HISubtractedCellMakerTool::HISubtractedCellMakerTool(const std::string& type, const std::string& name, const IInterface* parent)  : AthAlgTool(type, name, parent)
{
  declareInterface<ICaloCellMakerTool>(this);
}

//**********************************************************************

StatusCode HISubtractedCellMakerTool::initialize()
{
  ATH_CHECK( m_eventShapeKey.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode HISubtractedCellMakerTool::process (CaloCellContainer* theCells,
                                               const EventContext& ctx) const
{
  if (ctx.slot() > 1) {
    ATH_MSG_ERROR("This tool hasn't been converted for MT.");
    return StatusCode::FAILURE;
  }

  const xAOD::HIEventShapeContainer* shape=0;

  SG::ReadHandle<xAOD::HIEventShapeContainer>  readHandleEvtShape ( m_eventShapeKey , ctx);
  shape = readHandleEvtShape.cptr();

  const HIEventShapeIndex* index=m_eventShapeMapTool->getIndexFromShape( shape );
  if(index==nullptr)
  {
    ATH_MSG_ERROR("Could not retrieve HIEventShapeIndex for key " << m_eventShapeKey.key());
    return StatusCode::FAILURE;
  }

  if(shape->size()==0)
  {
    ATH_MSG_WARNING("HIEventShapeContainer " << m_eventShapeKey.key() << " exists but has zero size");
    return StatusCode::SUCCESS;
  }

  auto modtool_nc = m_modulatorTool.get();
  CHECK(modtool_nc->retrieveShape());

  for(auto pCell : *theCells)
  {
    unsigned int sample = (CaloSampling::CaloSample) pCell->caloDDE()->getSampling();
    double eta=pCell->eta();
    double phi=pCell->phi();

    unsigned int bin=index->getIndex(eta,sample);
    if( bin >= shape->size() )
    {
      if( std::abs(eta) - HICaloRange::getRange().getRangeMax(sample) )
      {
      	double fp_round=(eta > 0.) ? -5e-3 : 5e-3;
      	bin=index->getIndex(eta+fp_round,sample);
      }
    }

    const xAOD::HIEventShape* s=shape->at(bin);
    float nCells=s->nCells();
    float rho=0;
    if(nCells!=0.) rho=s->rho()/nCells;
    rho*=m_modulatorTool->getModulation(phi);
    float ue=rho*HICaloCellHelper::getAreaEtaPhi(pCell)*std::cosh(eta);
    pCell->setEnergy(pCell->energy()-ue);
  }
  return StatusCode::SUCCESS;
}
