/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagMultiSVInfoMSVVtxInfoAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/MultiSVInfoPlus.h"
#include "JetTagInfo/MSVVtxInfo.h"

namespace D3PD {


JetTagMultiSVInfoMSVVtxInfoAssociationTool::JetTagMultiSVInfoMSVVtxInfoAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,Analysis::MSVVtxInfo>(type, name, parent),
      m_finder (*this, "MultiSVInfoPlus", true),
      m_mSVInfoPlus(nullptr),
      m_vtxItr(0),
      m_vtxEnd(0)
{
}


StatusCode JetTagMultiSVInfoMSVVtxInfoAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,Analysis::MSVVtxInfo> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagMultiSVInfoMSVVtxInfoAssociationTool::book()
{

  /// book variable here
  /// none for now

  return StatusCode::SUCCESS;
}


StatusCode JetTagMultiSVInfoMSVVtxInfoAssociationTool::reset (const  Jet& p)
{
  m_mSVInfoPlus = m_finder.getTagInfo<Analysis::MultiSVInfoPlus> (p);
 
  m_vtxEnd = 0;
  m_vtxItr = 0;

  if(m_mSVInfoPlus){

    m_vtxEnd=m_mSVInfoPlus->numVtxInfo();

  }  

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const Analysis::MSVVtxInfo* JetTagMultiSVInfoMSVVtxInfoAssociationTool::next()
{

  // reached the end
  if(m_vtxItr == m_vtxEnd) return 0;

  const Analysis::MSVVtxInfo* vtxInfo = m_mSVInfoPlus->getVtxInfo(m_vtxItr);

  ++m_vtxItr;

  if(!vtxInfo){
    ATH_MSG_WARNING("NULL MSVVtxInfo pointer in MultiSVInfoPlus");
    //// no need to keep it synchronized.
    return next();
  }

  return vtxInfo;


}

} // namespace D3PD
 
