/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagQGPartonTruthInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/QGPartonTruthInfo.h"


namespace D3PD {
  
  

JetTagQGPartonTruthInfoFillerTool::JetTagQGPartonTruthInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  declareProperty ("InfoType", m_infoType   = "QGPartonTruthInfo");

  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagQGPartonTruthInfoFillerTool::book()
{

  CHECK(addVariable ("label", m_label ));
  CHECK(addVariable ("dRToParton", m_drParton ));

  return StatusCode::SUCCESS;
}


StatusCode JetTagQGPartonTruthInfoFillerTool::fill(const Jet& p)
{


  *m_label=-999;
  *m_drParton=-999;

  const Analysis::QGPartonTruthInfo* mcinfo = p.getTagInfo<Analysis::QGPartonTruthInfo>(m_infoType);

  if(!mcinfo){
    ATH_MSG_DEBUG(" QGPartonTruthInfo object " << m_infoType << " not found ");
    return StatusCode::SUCCESS;
  }

  std::string label = mcinfo->jetTruthLabel();
 
  *m_label=0;
  if(label=="LQ") *m_label = 1;
  if(label=="C") *m_label = 4;
  if(label=="B") *m_label = 5;
  if(label=="G") *m_label = 21;
  if(label=="N/A") *m_label = 0;

  *m_drParton =  mcinfo->deltaRToParton();

  return StatusCode::SUCCESS;
}


}// namespace D3PD
