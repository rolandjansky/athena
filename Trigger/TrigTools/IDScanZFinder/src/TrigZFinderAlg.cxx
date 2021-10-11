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
  ATH_CHECK(m_zFinderTools.retrieve());
  ATH_CHECK(m_pixelSpKey.initialize());
  ATH_CHECK(m_pixelHelperKey.initialize());
  ATH_CHECK(m_vertexKey.initialize());
  if (!m_monTool.empty()) ATH_CHECK(m_monTool.retrieve());
  
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
    if(pixelHelper->is_barrel(pixid)){
      const double dr = 0.01;
      const double dz = 0.13;
      
      for (const auto pSP : *pixSPointColl)
      {
        // Store SP into vector
        spVec.push_back(TrigSiSpacePointBase(layerId, pSP->globalPosition().perp(), pSP->globalPosition().phi(), pSP->globalPosition().z(), 
                                              dr, dz, pSP));    
      }
    }
  }
  ATH_MSG_DEBUG("Size of spVec: " << spVec.size());
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  TrigRoiDescriptor fullscan_roi( true );



  // Recording Data
  auto zVertexContainer = std::make_unique<xAOD::TrigCompositeContainer>();
  auto zVertexContainerAux = std::make_unique<xAOD::TrigCompositeAuxContainer>();  
  zVertexContainer->setStore(zVertexContainerAux.get());

  int toolIndex=0;
  for ( auto& tool: m_zFinderTools ) {
    TrigVertexCollection* vertices = tool->findZ(spVec, TrigRoiDescriptor(true));
  
    ATH_MSG_DEBUG("Successfully retrieved size of number of vertices: " << vertices->size());

    for ( auto vertex: *vertices ) {
      ATH_MSG_DEBUG("z of vertex: "<< vertex->z() );
      ATH_MSG_DEBUG("weight vertex: "<< vertex->cov()[5] );

      xAOD::TrigComposite *outputVertex = new xAOD::TrigComposite();
      zVertexContainer->push_back(outputVertex);
      outputVertex->setDetail<int>("zfinder_tool", toolIndex );
      outputVertex->setDetail<float>("zfinder_vtx_z", vertex->z() );
      outputVertex->setDetail<float>("zfinder_vtx_weight", vertex->cov()[5] );
      // Adding monitoring histograms
      auto ZVertex = Monitored::Scalar("ZVertex",vertex->z());
      auto ZVertexWeight = Monitored::Scalar("ZVertexWeight",vertex->cov()[5]);
    
      auto mon = Monitored::Group(m_monTool, ZVertex, ZVertexWeight);
    }
    toolIndex++;
    delete vertices;
  }
  if ( zVertexContainer->empty() ) {
    xAOD::TrigComposite *emptyVertex = new xAOD::TrigComposite();
    zVertexContainer->push_back(emptyVertex);
    emptyVertex->setDetail<int>("zfinder_tool", -1 );
    emptyVertex->setDetail<float>("zfinder_vtx_z", -1000.0 );
    emptyVertex->setDetail<float>("zfinder_vtx_weight", -1.0 );
  }

  SG::WriteHandle<xAOD::TrigCompositeContainer> vertexHandle(m_vertexKey, context);
  ATH_CHECK(vertexHandle.record(std::move(zVertexContainer), std::move(zVertexContainerAux)));
  return StatusCode::SUCCESS;
}