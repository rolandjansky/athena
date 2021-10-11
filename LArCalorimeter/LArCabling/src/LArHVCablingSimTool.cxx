/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCabling/LArHVCablingSimTool.h"
#include "LArCabling/LArHVIdMappingReader.h"
#include "LArIdentifier/LArElectrodeID.h"
#include "LArIdentifier/LArHVLineID.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"


//==================================================================================
LArHVCablingSimTool::LArHVCablingSimTool( const std::string& type, 
                                          const std::string& name, 
                                          const IInterface* parent )
  : AthAlgTool(type,name,parent),
    m_hvlineHelper(nullptr)
{
  declareProperty("MappingFolder",m_mappingFolder="/LAR/Identifier/HVLineToElectrodeMap");
}



//===========================================
StatusCode LArHVCablingSimTool::initialize ()
//============================================
{
  ATH_MSG_DEBUG(" ====> Initializing LArHVCablingSimTool... ");
  
  ATH_CHECK( detStore()->retrieve(m_hvlineHelper, "LArHVLineID") );

  const LArElectrodeID*  electrodeHelper = nullptr;
  ATH_CHECK( detStore()->retrieve(electrodeHelper, "LArElectrodeID") );

  const AthenaAttributeList* attrList = nullptr;
  ATH_CHECK ( detStore()->retrieve(attrList, m_mappingFolder) );

  LArHVIdMappingReader reader;
  std::map<HWIdentifier,HWIdentifier> electrode_hvline_map;
  ATH_CHECK( reader.read (attrList,
                          *m_hvlineHelper,
                          *electrodeHelper,
                          electrode_hvline_map,
                          m_hvline_electrode_map) );

  ATH_MSG_DEBUG(" ====> ...initialized LArHVCablingSimTool ");
  return StatusCode::SUCCESS;
}


//========================================================================================
const std::vector<HWIdentifier>&
LArHVCablingSimTool::getLArElectrodeIDvec(const HWIdentifier& hvlineId) const
//========================================================================================
{
  ATH_MSG_VERBOSE("Entering getLArElectrodeIDvec( hvlineId )" );

  std::map<HWIdentifier,std::vector<HWIdentifier> >::const_iterator elecIt=m_hvline_electrode_map.find(hvlineId);
  if( elecIt != m_hvline_electrode_map.end()){
    return (elecIt)->second;
  }
  else{
    int part = m_hvlineHelper->partition(hvlineId);
    int canl = m_hvlineHelper->can_line(hvlineId);
    int cann = m_hvlineHelper->can_node(hvlineId);
    int hvli = m_hvlineHelper->hv_line(hvlineId);
    ATH_MSG_WARNING( "LArHVCablingSimTool: getLArElectrodeIDvec did not find electrode for hvline [" 
                     << part << "." << canl << "." << cann << "." << hvli << "]" );
    static const std::vector<HWIdentifier> invalid;
    return(invalid);
  }
}
