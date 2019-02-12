/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "JetTagPixelModCondFillerTool.h"
#include "AthenaKernel/errorcheck.h"
#include "InDetReadoutGeometry/SiDetectorElementCollection.h"
#include "InDetReadoutGeometry/SiDetectorElement.h"
#include "InDetIdentifier/PixelID.h"

namespace D3PD {
  
  

JetTagPixelModCondFillerTool::JetTagPixelModCondFillerTool
  (const std::string& type,
   const std::string& name,
   const IInterface* parent)
    : BlockFillerTool<InDetDD::SiDetectorElementCollection> (type, name, parent),
      m_pixId(0),
      m_pixelCondSummarySvc("",name)
{
  declareProperty("PixelSummarySvc", m_pixelCondSummarySvc);
  declareProperty("MaxDeadModules", m_maxDeadModules=500);

  book().ignore(); // Avoid coverity warnings.
}
  

/**
 * @brief Book variables for this block.
 */
StatusCode JetTagPixelModCondFillerTool::initialize()
{
  ATH_CHECK( detStore()->retrieve( m_pixId,"PixelID") );

  if( m_pixelCondSummarySvc.empty() ){
    ATH_MSG_FATAL( "PixelConditionsSummarySvc not configured "  );
    ATH_MSG_FATAL( "you need to configure PixelConditionsSummarySvc to be able to dump pixel condition "  );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( m_pixelCondSummarySvc.retrieve() );
  ATH_MSG_INFO( "Retrieved service " << m_pixelCondSummarySvc  );
  return StatusCode::SUCCESS;
}

StatusCode JetTagPixelModCondFillerTool::book()
{

  CHECK(addVariable ("idHash", m_idHash ));
  CHECK(addVariable ("nDead", m_ndead ));

  return StatusCode::SUCCESS;
}


StatusCode JetTagPixelModCondFillerTool::fill(const InDetDD::SiDetectorElementCollection& p)
{

  std::vector<short> badmodules;
  badmodules.reserve(100);

  InDetDD::SiDetectorElementCollection::const_iterator iter=p.begin();

  for( ; iter != p.end(); ++iter){ 

    const InDetDD::SiDetectorElement* element = *iter; 
    if(selectModule(element)) continue;
       
    Identifier ident = element->identify(); 
    IdentifierHash id_hash = m_pixId->wafer_hash(ident);

    badmodules.push_back(id_hash);
  }

  *m_ndead=badmodules.size();

  /// do not right all modules if more the m_maxDeadModules are dead
  if(*m_ndead>=m_maxDeadModules){
    badmodules.clear();
  }

  *m_idHash = badmodules;

  return StatusCode::SUCCESS;
}

bool JetTagPixelModCondFillerTool::selectModule(const InDetDD::SiDetectorElement* element){

  if(!element) return false;

  Identifier ident = element->identify(); 
  if(!m_pixId->is_pixel(ident)) return false;

  IdentifierHash id_hash = m_pixId->wafer_hash(ident);

  bool active = m_pixelCondSummarySvc->isActive(id_hash);
  bool good = m_pixelCondSummarySvc->isGood(id_hash);

  return active && good;

}



}// namespace D3PD
