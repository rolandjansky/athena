/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagTruthInfoFillerTool.h"
#include "JetEvent/Jet.h"
#include "AthenaKernel/errorcheck.h"
#include "JetTagInfo/TruthInfo.h"


using CLHEP::Hep3Vector;


namespace D3PD {
  
  

JetTagTruthInfoFillerTool::JetTagTruthInfoFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<Jet> (type, name, parent)
{
  declareProperty ("InfoType", m_infoType   = "TruthInfo");

  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagTruthInfoFillerTool::book()
{

  CHECK(addVariable ("label", m_label ));
  CHECK(addVariable ("dRminToB", m_drB ));
  CHECK(addVariable ("dRminToC", m_drC ));
  CHECK(addVariable ("dRminToT", m_drT ));
  CHECK(addVariable ("BHadronpdg", m_truepdg ));
  CHECK(addVariable ("vx_x", m_vx_x ));
  CHECK(addVariable ("vx_y", m_vx_y ));
  CHECK(addVariable ("vx_z", m_vx_z ));

  return StatusCode::SUCCESS;
}


StatusCode JetTagTruthInfoFillerTool::fill(const Jet& p)
{

  *m_label=-999;
  *m_drB=-999;
  *m_drC=-999;
  *m_drT=-999;
  *m_truepdg=-999;
  *m_vx_x=-999;
  *m_vx_y=-999;
  *m_vx_z=-999;

  const Analysis::TruthInfo* mcinfo = p.getTagInfo<Analysis::TruthInfo>(m_infoType);

  if(!mcinfo){
    ATH_MSG_DEBUG(" TruthInfo object " << m_infoType << " not found ");
    return StatusCode::SUCCESS;
  }


  std::string label = mcinfo->jetTruthLabel();
 
  *m_label=0;
  if(label=="B") *m_label = 5;
  if(label=="C") *m_label = 4;
  if(label=="T") *m_label = 15;

  *m_drB =  mcinfo->deltaRMinTo("B");
  *m_drC =  mcinfo->deltaRMinTo("C");
  *m_drT =  mcinfo->deltaRMinTo("T");

  *m_truepdg = mcinfo->BHadronPdg();
  Eigen::Vector3d truebvtx = mcinfo->BDecayVertex();
 
  *m_vx_x = truebvtx.x();
  *m_vx_y = truebvtx.y();
  *m_vx_z = truebvtx.z();



  return StatusCode::SUCCESS;
}


}// namespace D3PD
