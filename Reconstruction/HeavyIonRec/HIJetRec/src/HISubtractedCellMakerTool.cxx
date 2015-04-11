/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
  declareProperty("FlowHarmonics",m_flow_harmonics=std::vector<unsigned int>());
}

//**********************************************************************

StatusCode HISubtractedCellMakerTool::initialize()
{
  int nh=HI::setHarmonics(m_flow_harmonics);
  if(nh >= 0)
  {
    ATH_MSG_ERROR("Invalid harmonic " << m_flow_harmonics[nh] );
    return StatusCode::FAILURE;
  }
  std::sort(m_flow_harmonics.begin(),m_flow_harmonics.end());
  return StatusCode::SUCCESS;
}

StatusCode HISubtractedCellMakerTool::process(CaloCellContainer* theCells)
{
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

  const xAOD::HIEventShape* es_temp=shape->front();
  unsigned int nh_max=es_temp->Et_cos().size();

  if(m_flow_harmonics.size()!=0)
  {
    if(m_flow_harmonics.back() >= nh_max)
    {
      ATH_MSG_ERROR("HIEventShapeContainer " << m_event_shape_key << " only has up to v" << nh_max << ". Requested UE modulation w/ v" << m_flow_harmonics.back()+1);
      return StatusCode::FAILURE;
    }
  }

  for(auto pCell : *theCells)
  {
    unsigned int sample = (CaloSampling::CaloSample) pCell->caloDDE()->getSampling();
    double eta=pCell->eta();
    //double phi=pCell->phi();

    unsigned int bin=index->getIndex(eta,sample);
    if( bin >= shape->size() )
    {
      if( std::abs(eta) - HICaloRange::getRange().getRangeMax(sample) )
      {
	double fp_round=(eta > 0.) ? -1e-3 : 1e-3;
	bin=index->getIndex(eta+fp_round,sample);
      }
    }

    const xAOD::HIEventShape* s=shape->at(bin);    
    float nCells=s->nCells();
    float rho=0;
    if(nCells!=0.) rho=s->rho()/nCells;
    //float ue=rho*HICaloCellHelper::GetAreaEtaPhi(pCell)*HI::GetModulation(s,m_flow_harmonics,phi)*std::cosh(eta);
    float ue=rho*HICaloCellHelper::GetAreaEtaPhi(pCell)*std::cosh(eta);
    pCell->setEnergy(pCell->energy()-ue);
  }
  return StatusCode::SUCCESS;
}

