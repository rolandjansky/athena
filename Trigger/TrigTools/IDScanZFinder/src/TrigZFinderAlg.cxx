/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <vector>
#include "TrigZFinderAlg.h"
#include "TrigZFinder.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrigInDetEvent/TrigSiSpacePointBase.h"
// To add identifier
#include "InDetPrepRawData/PixelCluster.h"

using namespace std;

TrigZFinderAlg::TrigZFinderAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  AthReentrantAlgorithm(name, pSvcLocator)
{
}

TrigZFinderAlg::~TrigZFinderAlg()
{
}

StatusCode TrigZFinderAlg::initialize()
{
  ATH_CHECK(m_zFinderTool.retrieve());
  ATH_CHECK(m_pixelSpKey.initialize());
  ATH_CHECK(m_pixelHelperKey.initialize());
  ATH_CHECK(m_zFinderKey.initialize());
  return StatusCode::SUCCESS;
}

StatusCode TrigZFinderAlg::finalize()
{
  return StatusCode::SUCCESS;
}

StatusCode TrigZFinderAlg::execute(const EventContext& context) const
{
  vector<TrigSiSpacePointBase> spVec;
  SG::ReadHandle<SpacePointContainer> pixelSP(m_pixelSpKey, context);
  
  //For Pixel info decoding
  SG::ReadHandle<PixelID> pixelHelper(m_pixelHelperKey, context);
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // This loop is to count sp
  // The z's are found from reconstructing these sp's.
  for (const auto pixSPointColl : *pixelSP)
  {
    if (pixSPointColl == nullptr)
      continue;
    
    const Identifier pixid = (pixSPointColl)->identify();
    int layerId = pixelHelper->layer_disk(pixid);
    // Set the resolution for r & z in barrel/end cap.
    double dr, dz;
    if(pixelHelper->is_barrel(pixid)){
      dr = 0.01;
      dz = 0.13;
    }
    else{
      dz=0.01;
      dr=0.13;
    }
      
    for (const auto pSP : *pixSPointColl)
    {
      // Store SP into vector
      spVec.push_back(TrigSiSpacePointBase(layerId, pSP->globalPosition().perp(), pSP->globalPosition().phi(), pSP->globalPosition().z(), 
              dr, dz, pSP));
    
    }
 
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TrigRoiDescriptor fullscan_roi( true );

  TrigVertexCollection* vertices = m_zFinderTool->findZ(spVec, TrigRoiDescriptor(true));
  
  ATH_MSG_DEBUG("Successfully retrieved size of number of vertices: " << vertices->size());
  ATH_MSG_DEBUG("Size of spVec: " << spVec.size());


  // Recording Data
  auto zFinderContainer = std::make_unique<xAOD::TrigCompositeContainer>();
  auto zFinderContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();
  zFinderContainer->setStore(zFinderContainerAux.get());

  for ( auto vertex: *vertices ) {
    ATH_MSG_DEBUG("z of vertex: "<< vertex->z() );
    ATH_MSG_DEBUG("weight vertex: "<< vertex->cov()[5] );

    xAOD::TrigComposite *zFinder = new xAOD::TrigComposite();
    zFinderContainer->push_back(zFinder);
    zFinder->setDetail<float>("zfinder_vtx_z", vertex->z() );
    zFinder->setDetail<float>("zfinder_vtx_weight", vertex->cov()[5] );
  }

  SG::WriteHandle<xAOD::TrigCompositeContainer> zFinderHandle(m_zFinderKey, context);
  ATH_CHECK(zFinderHandle.record(std::move(zFinderContainer), std::move(zFinderContainerAux)));
  
  delete vertices;

  return StatusCode::SUCCESS;
}

