/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: RpcPrepDataFillerTool.cxx 535262 2013-01-29 13:54:17Z salekd $
#include "RpcPrepDataFillerTool.h"
#include "AthenaKernel/errorcheck.h"


namespace D3PD {


RpcPrepDataFillerTool::RpcPrepDataFillerTool (const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent)
  : BlockFillerTool<Muon::RpcPrepData> (type, name, parent)
{
  book().ignore(); // Avoid coverity warnings.
}


StatusCode RpcPrepDataFillerTool::book()
{
  CHECK( addVariable ("x", m_x) );
  CHECK( addVariable ("y", m_y) );
  CHECK( addVariable ("z", m_z) );
  CHECK( addVariable ("time", m_time) );
  CHECK( addVariable ("locX", m_locX) );
 
  return StatusCode::SUCCESS;
}


StatusCode RpcPrepDataFillerTool::fill (const Muon::RpcPrepData& obj)
{
  *m_x    = obj.globalPosition().x();
  *m_y    = obj.globalPosition().y();
  *m_z    = obj.globalPosition().z();
  *m_time = obj.time();
  *m_locX = obj.localPosition().x();

  return StatusCode::SUCCESS;
}


} // namespace D3PD
