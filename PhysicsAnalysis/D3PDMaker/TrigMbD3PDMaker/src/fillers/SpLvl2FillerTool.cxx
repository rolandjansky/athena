/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMbD3PDMaker/fillers/SpLvl2FillerTool.h"
#include "AthenaKernel/errorcheck.h"

#include "TrigInDetEvent/TrigSpacePointCountsCollection.h"

namespace TrigMbD3PD {

SpLvl2FillerTool::SpLvl2FillerTool (const std::string& type,
				    const std::string& name,
				    const IInterface* parent)
  : SpLvl2FillerBase (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode SpLvl2FillerTool::initialize(){

  CHECK( SpLvl2FillerBase::initialize() );

  return StatusCode::SUCCESS;
}

StatusCode SpLvl2FillerTool::book()
{

  CHECK( addVariable ("nElements", m_nElements) );
  CHECK( addVariable ("hPixelClusTotBins", m_hPixelClusTotBins) );
  CHECK( addVariable ("hPixelClusTotMin", m_hPixelClusTotMin) );
  CHECK( addVariable ("hPixelClusTotMax", m_hPixelClusTotMax) );
  CHECK( addVariable ("hPixelClusSizeBins", m_hPixelClusSizeBins) );

  CHECK( addVariable ("hPixelClusSizeMin", m_hPixelClusSizeMin) );
  CHECK( addVariable ("hPixelClusSizeMax", m_hPixelClusSizeMax) );
  CHECK( addVariable ("pixelClusEndcapC_contents", m_pixelClusEndcapC_contents) );
  CHECK( addVariable ("pixelClusBarrel_contents", m_pixelClusBarrel_contents) );
  CHECK( addVariable ("pixelClusEndcapA_contents", m_pixelClusEndcapA_contents) );

  CHECK( addVariable ("droppedPixelModuleIds", m_droppedPixelModuleIds) );
  CHECK( addVariable ("sctSpEndcapC", m_sctSpEndcapC) );
  CHECK( addVariable ("sctSpBarrel", m_sctSpBarrel) );
  CHECK( addVariable ("sctSpEndcapA", m_sctSpEndcapA) );
  CHECK( addVariable ("droppedSctModuleIds", m_droppedSctModuleIds) );

  return StatusCode::SUCCESS;
}

StatusCode SpLvl2FillerTool::fill (const SpLvl2FillerObjectType& spCountsCollection)
{

  this->clearData();

  std::vector<Identifier> droppedIdentifiers;
  std::vector<Identifier>::iterator id_itr;
  std::vector<Identifier>::iterator id_itr_end;

  TrigHisto2D pixelClusEndcapC;
  TrigHisto2D pixelClusBarrel;
  TrigHisto2D pixelClusEndcapA;
 
  *m_nElements = spCountsCollection.size();
 
  TrigSpacePointCountsCollection::const_iterator itr = spCountsCollection.begin();
  TrigSpacePointCountsCollection::const_iterator itr_end = spCountsCollection.end();

  if(itr != itr_end) {

    pixelClusEndcapC = (*itr)->pixelClusEndcapC();
    pixelClusBarrel = (*itr)->pixelClusBarrel();
    pixelClusEndcapA = (*itr)->pixelClusEndcapA();
   
    *m_hPixelClusTotBins = pixelClusEndcapC.nbins_x();
    *m_hPixelClusTotMin = pixelClusEndcapC.min_x();
    *m_hPixelClusTotMax = pixelClusEndcapC.max_x();
    *m_hPixelClusSizeBins = pixelClusEndcapC.nbins_y();
    *m_hPixelClusSizeMin = pixelClusEndcapC.min_y();
    *m_hPixelClusSizeMax = pixelClusEndcapC.max_y();
    *m_pixelClusEndcapC_contents = pixelClusEndcapC.contents();
    *m_pixelClusBarrel_contents = pixelClusBarrel.contents();
    *m_pixelClusEndcapA_contents = pixelClusEndcapA.contents();
   
    droppedIdentifiers.clear();
    droppedIdentifiers = (*itr)->droppedPixelModules();
    id_itr = droppedIdentifiers.begin();
    id_itr_end = droppedIdentifiers.end();
    for(;id_itr != id_itr_end; ++id_itr) {
      m_droppedPixelModuleIds->push_back((*id_itr).get_identifier32().get_compact());
    }

    *m_sctSpEndcapC = (*itr)->sctSpEndcapC();
    *m_sctSpBarrel = (*itr)->sctSpBarrel();
    *m_sctSpEndcapA  = (*itr)->sctSpEndcapA();
   
    droppedIdentifiers.clear();
    droppedIdentifiers = (*itr)->droppedSctModules();
    id_itr = droppedIdentifiers.begin();
    id_itr_end = droppedIdentifiers.end();
    for(;id_itr != id_itr_end; ++id_itr) {
      m_droppedSctModuleIds->push_back((*id_itr).get_identifier32().get_compact());
    }
  }

  return StatusCode::SUCCESS;
}

void SpLvl2FillerTool::clearData(){

  *m_nElements = 0;
  *m_hPixelClusTotBins = 0;
  *m_hPixelClusTotMin = 0.;
  *m_hPixelClusTotMax = 0.;
  *m_hPixelClusSizeBins = 0;
  *m_hPixelClusSizeMin = 0.;
  *m_hPixelClusSizeMax = 0.;
  m_pixelClusEndcapC_contents->clear();
  m_pixelClusBarrel_contents->clear();
  m_pixelClusEndcapA_contents->clear();
  m_droppedPixelModuleIds->clear();
  *m_sctSpEndcapC = 0;
  *m_sctSpBarrel = 0;
  *m_sctSpEndcapA = 0;
  m_droppedSctModuleIds->clear();
}

} // namespace TrigMbD3PD
