/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelClusterFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"
#include "InDetPrepRawData/PixelCluster.h"
//#include "TrkEventPrimitives/ErrorMatrix.h"

namespace D3PD {


PixelClusterFillerTool::PixelClusterFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<InDet::PixelCluster> (type, name, parent),
    m_pixId(0)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode PixelClusterFillerTool::initialize(){

  CHECK( Base::initialize() );

  if (detStore()->retrieve(m_pixId,"PixelID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve pixel ID helper";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode PixelClusterFillerTool::book(){

  CHECK( addVariable ("id", m_id) );
  CHECK( addVariable ("bec", m_bec) );
  CHECK( addVariable ("layer", m_layer) );
  CHECK( addVariable ("detElementId", m_detElementId) );
  CHECK( addVariable ("phi_module", m_phi_module) );
  CHECK( addVariable ("eta_module", m_eta_module) );
  CHECK( addVariable ("col", m_col) );
  CHECK( addVariable ("row", m_row) );
  CHECK( addVariable ("charge", m_charge) );
  CHECK( addVariable ("ToT", m_ToT) );
  CHECK( addVariable ("LVL1A", m_LVL1A) );
  CHECK( addVariable ("sizePhi", m_sizePhi) );
  CHECK( addVariable ("sizeZ", m_sizeZ) );
  CHECK( addVariable ("size", m_size) );
  CHECK( addVariable ("locX", m_locX) );
  CHECK( addVariable ("locY", m_locY) );
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("isFake", m_isFake) );
  CHECK( addVariable ("isGanged", m_isGanged) );
  CHECK( addVariable ("isSplit", m_isSplit) );
  CHECK( addVariable ("splitProb1", m_splitProb1) );
  CHECK( addVariable ("splitProb2", m_splitProb2) );

  return StatusCode::SUCCESS;
}

StatusCode PixelClusterFillerTool::fill (const InDet::PixelCluster& clus)
{

  this->clearData();

  Identifier id = clus.identify();

  if ( id.is_valid() ) {
    *m_id = id.get_compact();
    *m_bec = (int)m_pixId->barrel_ec(id);
    *m_layer = (int)m_pixId->layer_disk(id);
    *m_phi_module = (char)m_pixId->phi_module(id);
    *m_eta_module = (char)m_pixId->eta_module(id);
    *m_col = (short)m_pixId->eta_index(id);
    *m_row = (short)m_pixId->phi_index(id);
  }

  const InDetDD::SiDetectorElement* de = clus.detectorElement();
  if(de){
    Identifier detId = de->identify();
    if ( detId.is_valid() ) {
      *m_detElementId = detId.get_compact();
    }
  }

  *m_LVL1A = (short)clus.LVL1A();
  *m_charge = clus.totalCharge();
  *m_ToT = clus.totalToT();

  const InDet::SiWidth cw = clus.width();
  *m_sizePhi = (int)cw.colRow()[0];
  *m_sizeZ = (int)cw.colRow()[1];
  *m_size = (int)clus.rdoList().size();

  const Amg::Vector2D& locpos = clus.localPosition();
  *m_locX = locpos[Trk::locX];
  *m_locY = locpos[Trk::locY];

  const Amg::Vector3D& glopos = clus.globalPosition();
  *m_x = glopos[Trk::x];
  *m_y = glopos[Trk::y];
  *m_z = glopos[Trk::z];

  *m_isFake = (char)clus.isFake();
  *m_isGanged = (char)clus.gangedPixel();
  *m_isSplit = (int)clus.isSplit();
  *m_splitProb1= clus.splitProbability1();
  *m_splitProb2 = clus.splitProbability2();

  return StatusCode::SUCCESS;
}

void PixelClusterFillerTool::clearData(){

  *m_id = 0;
  *m_detElementId = 0;
  *m_bec = -1;
  *m_layer = -1;
  *m_phi_module = 0;
  *m_eta_module = 0;
  *m_col = 0;
  *m_row = 0;
  *m_charge = 0;
  *m_ToT = -99999.;
  *m_LVL1A = -1;
  *m_sizePhi = 0;
  *m_sizeZ = 0;
  *m_size = 0;
  *m_locX = -99999.;
  *m_locY = -99999.;
  *m_x = -99999.;
  *m_y = -99999.;
  *m_z = -99999.;
  *m_isFake = 0;
  *m_isGanged = 0;
  *m_isSplit = 0;
  *m_splitProb1 = 0.;
  *m_splitProb2 = 0.;
}

} // namespace D3PD
