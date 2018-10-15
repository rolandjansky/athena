/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GaudiKernel/Service.h"

#include "CaloIdentifier/CaloCell_ID.h"

#include "CaloUtils/CaloTowerBuilderTool.h"

#include "LArTowerBuilderTool.h"

#include <string>

LArTowerBuilderTool::LArTowerBuilderTool(const std::string& name,
					 const std::string& type,
					 const IInterface* parent)
  : CaloTowerBuilderTool(name,type,parent)
{ }

LArTowerBuilderTool::~LArTowerBuilderTool()
{ }


/**
 * @brief Convert calorimeter strings to enums.
 * @param includedCalos Property with calorimeter strings.
 */
std::vector<CaloCell_ID::SUBCALO>
LArTowerBuilderTool::parseCalos 
  (const std::vector<std::string>& includedCalos) const
{
  // convert to enumerators
  std::vector<CaloCell_ID::SUBCALO> indices;

  for (const std::string& s : includedCalos) {
    if ( s == "LAREM" ) {
      indices.push_back(CaloCell_ID::LAREM);
    }
    else if ( s == "LARHEC" ) {
      indices.push_back(CaloCell_ID::LARHEC);
    }
    else if ( s == "LARFCAL" ) {
      ATH_MSG_INFO( "use LArFCalTowerBuilderTool for the FCal - request ignored" );
    }
  }

  return indices;
}

