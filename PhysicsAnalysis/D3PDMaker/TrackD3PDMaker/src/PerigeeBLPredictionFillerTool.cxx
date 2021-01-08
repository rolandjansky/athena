/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrackD3PDMaker/src/PerigeeBLPredictionFillerTool.cxx
 * @author remi zaidan <remi.zaidan@cern.ch>
 * @date Apr, 2010
 * @brief Fill expectHitInBLayer from a Perigee.
 */

#include "PerigeeBLPredictionFillerTool.h"
#include "InDetTestBLayer/InDetTestBLayerTool.h"
#include "InDetTestBLayer/TrackStateOnBLayerInfo.h"
#include "AthenaKernel/errorcheck.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"

namespace D3PD {

/**
 * @brief Standard Gaudi tool constructor.
 * @param type The name of the tool type.
 * @param name The tool name.
 * @param parent The tool's Gaudi parent.
 */
PerigeeBLPredictionFillerTool::PerigeeBLPredictionFillerTool
    (const std::string& type,
     const std::string& name,
     const IInterface* parent)
  : BlockFillerTool<Trk::Perigee> (type, name, parent),
    m_inDetTestBLayerTool("InDet::InDetRecTestBLayerTool")
{
  declareProperty ("InDetTestBLayerTool", m_inDetTestBLayerTool,
                   "Tool to test if the track crosses a dead module "
                   "on the B-Layer.");

  book().ignore(); // Avoid coverity warnings.
}


/**
 * @brief Standard Gaudi initialize method.
 */
StatusCode PerigeeBLPredictionFillerTool::initialize()
{
  CHECK( BlockFillerTool<Trk::Perigee>::initialize() );
  CHECK( m_inDetTestBLayerTool.retrieve() );
  CHECK( detStore()->retrieve(m_pixId, "PixelID") );

  return StatusCode::SUCCESS;
}


/**
 * Book variables for this block.
 */
StatusCode PerigeeBLPredictionFillerTool::book()
{

  CHECK( addVariable ("x",  m_x, "predicted x global position in the B-layer" ) );
  CHECK( addVariable ("y",  m_y, "predicted y global position in the B-layer" ) );
  CHECK( addVariable ("z",  m_z, "predicted z global position in the B-layer" ) );
  CHECK( addVariable ("locX",  m_locX, "predicted x local position in the B-layer" ) );
  CHECK( addVariable ("locY",  m_locY, "predicted y local position in the B-layer" ) );
  CHECK( addVariable ("err_locX",  m_err_locX, "error on local x prediction in the B-layer" ) );
  CHECK( addVariable ("err_locY",  m_err_locY, "error on local y prediction in the B-layer" ) );
  CHECK( addVariable ("etaDistToEdge",  m_etaDistToEdge, "distance of B-layer prediction to module edge along eta") );
  CHECK( addVariable ("phiDistToEdge",  m_phiDistToEdge, "distance of B-layer prediction to module edge along phi") );
  CHECK( addVariable ("detElementId",  m_detElementId, "detector element identifier of B-layer prediction" ) );
  CHECK( addVariable ("row",  m_row, "" ) );
  CHECK( addVariable ("col",  m_col, "" ) );
  CHECK( addVariable ("type",  m_type, "" ) );

  return StatusCode::SUCCESS;
}


/**
 * @brief Fill one block --- type-safe version.
 * @param p The input object.
 *
 * This is called once per object.  The caller
 * is responsible for arranging that all the pointers for booked variables
 * are set appropriately upon entry.
 */
StatusCode
PerigeeBLPredictionFillerTool::fill (const Trk::Perigee& p) {

  this->clearData();

  std::vector<InDet::TrackStateOnBLayerInfo>  trackStateBlayer;
  if(!m_inDetTestBLayerTool->getTrackStateOnBlayerInfo(&p, trackStateBlayer)){
    return StatusCode::SUCCESS;
  }
  
  std::vector<InDet::TrackStateOnBLayerInfo>::const_iterator itr = trackStateBlayer.begin();
  std::vector<InDet::TrackStateOnBLayerInfo>::const_iterator end = trackStateBlayer.end();
  for (; itr!=end; ++itr){
    Identifier id = itr->moduleId();
    
    if(id.is_valid()){
      m_detElementId->push_back(id.get_identifier32().get_compact());
    }else{
      m_detElementId->push_back(0);
    }
    
    m_type->push_back((int)itr->type());
    
    m_etaDistToEdge->push_back(itr->distToModuleEdgeEta());
    m_phiDistToEdge->push_back(itr->distToModuleEdgePhi());
    
    Identifier holeId_c = itr->pixelId();
    if(holeId_c.is_valid()){
      m_col->push_back(m_pixId->eta_index(holeId_c));
      m_row->push_back(m_pixId->phi_index(holeId_c));
    }else{
      m_col->push_back(-1);
      m_row->push_back(-1);
    }
    
    m_x->push_back(itr->globalPosition().x());
    m_y->push_back(itr->globalPosition().y());
    m_z->push_back(itr->globalPosition().z());
    
    m_locX->push_back(itr->localX());
    m_locY->push_back(itr->localY());
    
    m_err_locX->push_back(itr->errLocalX());
    m_err_locY->push_back(itr->errLocalY());
  }

  return StatusCode::SUCCESS;
}

void PerigeeBLPredictionFillerTool::clearData(){

  m_x->clear();
  m_y->clear();
  m_z->clear();
  m_locX->clear();
  m_locY->clear();
  m_err_locX->clear();
  m_err_locY->clear();
  m_etaDistToEdge->clear();
  m_phiDistToEdge->clear();
  m_detElementId->clear();
  m_row->clear();
  m_col->clear();
  m_type->clear();
}

} // namespace D3PD
