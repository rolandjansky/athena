/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetTagSoftElecInfoegammaAssociationTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/SoftElectronInfo.h"
#include "egammaEvent/egamma.h"

namespace D3PD {


JetTagSoftElecInfoegammaAssociationTool::JetTagSoftElecInfoegammaAssociationTool 
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : MultiAssociationTool<Jet,egamma>(type, name, parent),
      m_finder (*this, "SoftElectronTag", true),
      m_egItr(0),
      m_egEnd(0)
{
  declareProperty("FillVariables", m_fillVariables=true); 
  book().ignore(); // Avoid coverity warnings.
}


StatusCode JetTagSoftElecInfoegammaAssociationTool::initialize()
{

  typedef MultiAssociationTool<Jet,egamma> base;

  CHECK ( base::initialize() );

  /// add additional initialization here

  return StatusCode::SUCCESS;
}


StatusCode JetTagSoftElecInfoegammaAssociationTool::book()
{

  if(m_fillVariables){
    CHECK( addVariable ("isElectron", m_isElectron) );
    CHECK( addVariable ("d0", m_d0) ); 
    CHECK( addVariable ("pTRel", m_pTRel) );  
  }

  return StatusCode::SUCCESS;
}


StatusCode JetTagSoftElecInfoegammaAssociationTool::reset (const  Jet& p)
{
  m_softElectronInfo=m_finder.getTagInfo<Analysis::SoftElectronInfo> (p);
 
  m_egEnd = 0;
  m_egItr = 0;

  if(m_softElectronInfo){

    m_egEnd=m_softElectronInfo->numTrackInfo();

  }  

  return StatusCode::SUCCESS;
 
}


/**
 * @brief Return a pointer to the next element in the association.
 *
 * Return 0 when the association has been exhausted.
 */
const egamma* JetTagSoftElecInfoegammaAssociationTool::next()
{

  // reached the end
  if(m_egItr == m_egEnd) return 0;

  const Analysis::SETrackInfo& seinfo = m_softElectronInfo->getTrackInfo(m_egItr);
  const egamma* eg = seinfo.electron();

  if(m_fillVariables){
    *m_isElectron = !seinfo.isPhoton();
    *m_pTRel = seinfo.pTrel();
    *m_d0 = seinfo.d0Value();
  }

  if(!eg){
    ATH_MSG_WARNING("NULL egamma pointer in SETrackInfo, association terminated");
    //// if this happens we can hack it by creating a dummy track 
    ////  so that it does not terminate the association
  }

  ++m_egItr;

  return eg;


}

} // namespace D3PD
 
