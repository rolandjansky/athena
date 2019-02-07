/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "HISubtractedCellMakerTool.h"
#include "HICaloCellHelper.h"
#include "xAODHIEvent/HIEventShapeContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "HIEventUtils/HIEventDefs.h"
#include "HIEventUtils/HIEventShapeMap.h"
#include "HIEventUtils/HICaloRange.h"

#include <algorithm>

//**********************************************************************

HISubtractedCellMakerTool::HISubtractedCellMakerTool(const std::string& type, const std::string& name, const IInterface* parent)  : AthAlgTool(type, name, parent)
{
  declareInterface<ICaloCellMakerTool>(this);
  declareProperty("EventShapeKey",m_event_shape_key);
  declareProperty("Modulator",m_modulator_tool);
}

//**********************************************************************

StatusCode HISubtractedCellMakerTool::initialize()
{
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
  CHECK(evtStore()->retrieve(shape,m_event_shape_key));
  const HIEventShapeIndex* index=HIEventShapeMap::getIndex(m_event_shape_key);
  if(index==nullptr) 
  {
    ATH_MSG_ERROR("Could not retrieve HIEventShapeIndex for key " << m_event_shape_key);
    return StatusCode::FAILURE;    
  }

  if(shape->size()==0)
  {
    ATH_MSG_WARNING("HIEventShapeContainer " << m_event_shape_key << " exists but has zero size");
    return StatusCode::SUCCESS;
  }

  CHECK(m_modulator_tool->retrieveShape());

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

    // if( bin >= shape->size() ) 
    // {
    //   ATH_MSG_ERROR("Requested bin for cell " << bin << " is out of range " shape->size());
    //   return StatusCode::ERROR;
    // }

    const xAOD::HIEventShape* s=shape->at(bin);    
    float nCells=s->nCells();
    float rho=0;
    if(nCells!=0.) rho=s->rho()/nCells;
    rho*=m_modulator_tool->getModulation(phi);
    float ue=rho*HICaloCellHelper::GetAreaEtaPhi(pCell)*std::cosh(eta);
    pCell->setEnergy(pCell->energy()-ue);
  }
  return StatusCode::SUCCESS;
}

