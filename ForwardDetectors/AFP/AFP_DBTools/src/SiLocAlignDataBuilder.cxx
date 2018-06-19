/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiLocAlignDataBuilder.cxx
 * @author Grzegorz Gach <grzegorz.gach@cern.ch>
 * @date   2018-01-17
 * 
 * @brief  Source file for class SiLocAlignDataBuilder.
 * 
 */


#include "../AFP_DBTools/SiLocAlignDataBuilder.h"

#include <sstream>

namespace AFP
{
  bool SiLocAlignDataBuilder::addBuilderCommand (const std::string& name) 
  {
    if (name == "stationID")
      this->m_commands.push_back(new Command<int> (&SiLocAlignData::m_stationID));

    else if (name == "layerID")
      this->m_commands.push_back(new Command<int> (&SiLocAlignData::m_layerID));

    else if (name == "shiftX")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_xShift));

    else if (name == "shiftY")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_yShift));

    else if (name == "shiftZ")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_zShift));

    else if (name == "alpha")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_alpha));

    else if (name == "beta")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_beta));

    else if (name == "gamma")
      this->m_commands.push_back(new Command<double> (&SiLocAlignData::m_gamma));

    else {
      this->m_commands.push_back(new Command<double> (nullptr));
      return false;
    }

    return true;
  }

} // AFP namespace
