/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TgcPrepDataFillerTool.cxx 535262 2013-01-29 13:54:17Z salekd $
#include "TgcPrepDataFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


TgcPrepDataFillerTool::TgcPrepDataFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : BlockFillerTool<Muon::TgcPrepData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode TgcPrepDataFillerTool::book()
{
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("locX", m_locX) );
  //CHECK( addVariable ("bcBitMap", m_bcBitMap) );
 
  return StatusCode::SUCCESS;
}


StatusCode TgcPrepDataFillerTool::fill (const Muon::TgcPrepData& obj)
{
  *m_x   = obj.globalPosition().x();
  *m_y   = obj.globalPosition().y();
  *m_z   = obj.globalPosition().z();
  *m_locX = obj.localPosition().x();
  //*m_bcBitMap = obj.getBcBitMap();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
