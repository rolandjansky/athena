/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelRDOFillerTool.h"
#include "AthenaKernel/errorcheck.h"
 
 
namespace D3PD {

PixelRDOFillerTool::PixelRDOFillerTool (const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
  : PixelRDOFillerBase (type, name, parent),
    m_pixelId(0)
{
  book().ignore();  // Avoid coverity warnings.
}
 
StatusCode PixelRDOFillerTool::initialize()
{

CHECK( PixelRDOFillerBase::initialize() );

 if (detStore()->retrieve(m_pixelId,"PixelID").isFailure() ){
   REPORT_MESSAGE (MSG::FATAL) << "Unable to retrieve Pixel ID helper";
   return StatusCode::FAILURE;
 }

return StatusCode::SUCCESS;
}
 
StatusCode PixelRDOFillerTool::book()
{

  //CHECK( addVariable ("nRDO", m_nRDO) );
  //CHECK( addVariable ("time_ns", m_timeNanoSec) );
  //CHECK( addVariable ("isOnTrack", m_isOnTrack) );

  CHECK( addVariable ("bec", m_bec) );
  CHECK( addVariable ("layer" ,m_layer) );
  CHECK( addVariable ("moduleEta", m_moduleEta) );
  CHECK( addVariable ("modulePhi", m_modulePhi) );

  CHECK( addVariable ("ToT", m_ToT) );
  //CHECK( addVariable ("moduleSide", m_moduleSide) );
  //CHECK( addVariable ("firstStrip", m_firstStrip) );
  //CHECK( addVariable ("nStripsOnWafer", m_nStripsOnWafer) );
  //CHECK( addVariable ("timeBin", m_tbin) );


  return StatusCode::SUCCESS;
}
 
StatusCode PixelRDOFillerTool::fill (const PixelRDORawData& pixelRdo)
{

  this->clearData();

  //const PixelRDORawData *p_pixelRdo = &pixelRdo;

  //const Pixel1RawData* rdo1 = dynamic_cast<const Pixel1RawData*>(p_pixelRdo);
  //if (rdo1!=0)
  //  *m_tbin = rdo1->getTimeBin();
  //else *m_tbin = -1;
      
  Identifier Pixel_Identifier = pixelRdo.identify();
  *m_bec = m_pixelId->barrel_ec(Pixel_Identifier);
  *m_layer = m_pixelId->layer_disk(Pixel_Identifier);
  *m_modulePhi = m_pixelId->phi_module(Pixel_Identifier);
  *m_moduleEta = m_pixelId->eta_module(Pixel_Identifier);

  *m_ToT = pixelRdo.getToT();
  //*m_moduleSide = m_pixelId->side(Pixel_Identifier);
  //*m_firstStrip = pixelRdo.getStrip();
  //*m_nStripsOnWafer = pixelRdo.getGroupSize();
  //*m_isOnTrack = 0;

   return StatusCode::SUCCESS;
}
 
void PixelRDOFillerTool::clearData(){

  //*m_nRDO = -9999;
  //*m_timeNanoSec = -9999;
  //*m_isOnTrack = -9999;

  *m_bec = -9999;
  *m_layer = -9999;
  *m_moduleEta = -9999;
  *m_modulePhi = -9999;
  
  *m_ToT = -9999;
  //*m_moduleSide = -9999;
  //*m_firstStrip = -9999;
  //*m_nStripsOnWafer = -9999;
  //*m_tbin = -9999;

}

 
} // namespace D3PD
