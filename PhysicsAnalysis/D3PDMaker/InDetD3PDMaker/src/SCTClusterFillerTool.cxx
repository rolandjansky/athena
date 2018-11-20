/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTClusterFillerTool.h"

#include "AthenaKernel/errorcheck.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/SCT_ID.h"
#include "InDetPrepRawData/SCT_Cluster.h"


namespace D3PD {


SCTClusterFillerTool::SCTClusterFillerTool (const std::string& type,
				      const std::string& name,
				      const IInterface* parent)
  : D3PD::BlockFillerTool<InDet::SCT_Cluster> (type, name, parent),
    m_sctId(0)
{
  book().ignore(); // Avoid coverity warnings.
}

StatusCode SCTClusterFillerTool::initialize(){

  CHECK( Base::initialize() );

  if (detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
    REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve SCT ID helper";
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode SCTClusterFillerTool::book(){

  CHECK( addVariable ("id", m_id) );
  CHECK( addVariable ("bec", m_bec) );
  CHECK( addVariable ("layer", m_layer) );
  CHECK( addVariable ("detElementId", m_detElementId) );
  CHECK( addVariable ("phi_module", m_phi_module) );
  CHECK( addVariable ("eta_module", m_eta_module) );
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("locX", m_locX) );
  CHECK( addVariable ("locY", m_locY) );
  CHECK( addVariable ("side", m_side) );
  CHECK( addVariable ("size", m_size) );

  return StatusCode::SUCCESS;
}

StatusCode SCTClusterFillerTool::fill (const InDet::SCT_Cluster& clus)
{

  this->clearData();

  Identifier id = clus.identify();
  
  if ( id.is_valid() ) {

    *m_size =  int(clus.rdoList().size());
    *m_id = id.get_compact();
    *m_bec = (int)m_sctId->barrel_ec(id);
    *m_layer = (int)m_sctId->layer_disk(id);
    *m_phi_module = (int)m_sctId->phi_module(id);
    *m_eta_module = (int)m_sctId->eta_module(id);
    *m_side = (int)m_sctId->side(id);

  }

  const InDetDD::SiDetectorElement* de = clus.detectorElement();
  if(de){
    Identifier detId = de->identify();
    if ( detId.is_valid() ) {
      *m_detElementId = detId.get_compact();
    }
  }

  const Amg::Vector2D& locpos = clus.localPosition();
  *m_locX = locpos.x(); //[Trk::locX];
  if ( !(std::isinf(locpos.y()) || std::isnan(locpos.y())) ){
    if (locpos.y()<1e-07)
      *m_locY= 0.;
    else 
      *m_locY=locpos.y();
  }
  else 
    *m_locY = -9999.;//= locpos[Trk::locY];

  
  
  const Amg::Vector3D& glopos = clus.globalPosition();
  *m_x = glopos.x(); //[Trk::x];
  *m_y = glopos.y(); //[Trk::y];
  *m_z = glopos.z(); //[Trk::z];
  return StatusCode::SUCCESS;
}

void SCTClusterFillerTool::clearData(){

  *m_id = 0;
  *m_detElementId = 0;
  *m_bec = -1;
  *m_layer = -1;
  *m_phi_module = 0;
  *m_eta_module = 0;
  *m_x = -99999.;
  *m_y = -99999.;
  *m_z = -99999.;
  *m_locX = -99999.;
  *m_locY = -99999.;
  *m_side = -10;
  *m_size = -99999;
 
}

} // namespace D3PD
