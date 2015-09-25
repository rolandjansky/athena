/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "InDetAlignDBTools/InDetTrkAlignDBTool.h"

#include "TFile.h"

#include <vector>

namespace InDet
{

//________________________________________________________________________
InDetTrkAlignDBTool::InDetTrkAlignDBTool(const std::string & type, const std::string & name, const IInterface * parent)
   : AthAlgTool(type,name,parent)
   , m_siDBtool("InDet::SiTrkAlignDBTool/SiTrkAlignDBTool")
   , m_trtDBtool("InDet::TRTTrkAlignDBTool/TRTTrkAlignDBTool")
{
  declareInterface<ITrkAlignDBTool>(this);

  declareProperty("SiTrkAlignDBTool",     m_siDBtool,     "Tool for handling the Silicon DB I/O");
  declareProperty("TRTTrkAlignDBTool",    m_trtDBtool,    "Tool for handling the TRT DB I/O");
}

//________________________________________________________________________
InDetTrkAlignDBTool::~InDetTrkAlignDBTool()
{}

//________________________________________________________________________
StatusCode InDetTrkAlignDBTool::initialize() {

  ATH_MSG_DEBUG("initialize() of InDetTrkAlignDBTool");

  // get SiTrkAlignDBTool
  if( m_siDBtool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << " Could not retrieve " << m_siDBtool << endreq;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("retrieved " << m_siDBtool);

  // get TRTTrkAlignDBTool
  if( m_trtDBtool.retrieve().isFailure() ) {
    msg(MSG::FATAL) << " Could not retrieve " << m_trtDBtool << endreq;
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("retrieved " << m_trtDBtool);

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
StatusCode InDetTrkAlignDBTool::finalize()
{
  ATH_MSG_DEBUG("finalize() of InDetTrkAlignDBTool");

  return StatusCode::SUCCESS;
}

//________________________________________________________________________
void InDetTrkAlignDBTool::writeAlignPar()
{
  ATH_MSG_DEBUG("in InDetTrkAlignDBTool::writeAlignPar()");

  m_siDBtool->writeAlignPar();
  m_trtDBtool->writeAlignPar();

  return;
}
//_________________________________________________________________________________________

} // end namespace
