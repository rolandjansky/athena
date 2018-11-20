/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "SCTRDOFillerTool.h"
#include "AthenaKernel/errorcheck.h"
 
 
namespace D3PD {

SCTRDOFillerTool::SCTRDOFillerTool (const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent)
  : SCTRDOFillerBase (type, name, parent),
    m_sctId(0)
{
  book().ignore();  // Avoid coverity warnings.
}
 
StatusCode SCTRDOFillerTool::initialize()
{

CHECK( SCTRDOFillerBase::initialize() );

 if (detStore()->retrieve(m_sctId,"SCT_ID").isFailure() ){
   REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve SCT ID helper";
   return StatusCode::FAILURE;
 }

return StatusCode::SUCCESS;
}
 
StatusCode SCTRDOFillerTool::book()
{

  //CHECK( addVariable ("nRDO", m_nRDO) );
  //CHECK( addVariable ("time_ns", m_timeNanoSec) );
  //CHECK( addVariable ("isOnTrack", m_isOnTrack) );

  CHECK( addVariable ("bec", m_bec) );
  CHECK( addVariable ("layer" ,m_layer) );
  CHECK( addVariable ("moduleEta", m_moduleEta) );
  CHECK( addVariable ("modulePhi", m_modulePhi) );
  CHECK( addVariable ("moduleSide", m_moduleSide) );
  CHECK( addVariable ("firstStrip", m_firstStrip) );
  CHECK( addVariable ("nStripsOnWafer", m_nStripsOnWafer) );
  CHECK( addVariable ("timeBin", m_tbin) );


  return StatusCode::SUCCESS;
}
 
StatusCode SCTRDOFillerTool::fill (const SCT_RDORawData& sctRdo)
{

  this->clearData();

  const SCT_RDORawData *p_sctRdo = &sctRdo;

  const SCT3_RawData* rdo3 = dynamic_cast<const SCT3_RawData*>(p_sctRdo);
  if (rdo3!=0)
    *m_tbin = rdo3->getTimeBin();
  else *m_tbin = -1;
      
  Identifier SCT_Identifier = sctRdo.identify();
  *m_bec = m_sctId->barrel_ec(SCT_Identifier);
  *m_layer = m_sctId->layer_disk(SCT_Identifier);
  *m_modulePhi = m_sctId->phi_module(SCT_Identifier);
  *m_moduleEta = m_sctId->eta_module(SCT_Identifier);
  *m_moduleSide = m_sctId->side(SCT_Identifier);
  *m_firstStrip = m_sctId->strip(SCT_Identifier);
  *m_nStripsOnWafer = sctRdo.getGroupSize();
  //*m_isOnTrack = 0;

   return StatusCode::SUCCESS;
}
 
void SCTRDOFillerTool::clearData(){

  //*m_nRDO = -9999;
  //*m_timeNanoSec = -9999;
  //*m_isOnTrack = -9999;

  *m_bec = -9999;
  *m_layer = -9999;
  *m_moduleEta = -9999;
  *m_modulePhi = -9999;
  *m_moduleSide = -9999;
  *m_firstStrip = -9999;
  *m_nStripsOnWafer = -9999;
  *m_tbin = -9999;

}

 
} // namespace D3PD
