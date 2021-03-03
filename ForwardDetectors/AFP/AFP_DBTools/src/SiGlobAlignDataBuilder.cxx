/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file   SiGlobAlignDataBuilder.cxx
 * @author Petr Balek <petr.balek@cern.ch>
 * @date   2021-02-23
 * 
 * @brief  Source file for class SiGlobAlignDataBuilder.
 * 
 */


#include "AFP_DBTools/SiGlobAlignDataBuilder.h"

#include <sstream>

namespace AFP
{
  bool SiGlobAlignDataBuilder::addBuilderCommand (const std::string& name) 
  {
    if (name == "stationID") this->m_commands.push_back(new Command<int> (&SiGlobAlignData::m_stationID));

    else if (name == "shiftXTracker") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_xShiftTracker));
    else if (name == "shiftYTracker") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_yShiftTracker));
    else if (name == "shiftZTracker") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_zShiftTracker));
    else if (name == "alphaTracker")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_alphaTracker));
    else if (name == "betaTracker")   this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_betaTracker));
    else if (name == "gammaTracker")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_gammaTracker));

    else if (name == "shiftXBeam") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_xShiftBeam));
    else if (name == "shiftYBeam") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_yShiftBeam));
    else if (name == "shiftZBeam") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_zShiftBeam));
    else if (name == "alphaBeam")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_alphaBeam));
    else if (name == "betaBeam")   this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_betaBeam));
    else if (name == "gammaBeam")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_gammaBeam));

    else if (name == "shiftXRP") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_xShiftRP));
    else if (name == "shiftYRP") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_yShiftRP));
    else if (name == "shiftZRP") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_zShiftRP));
    else if (name == "alphaRP")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_alphaRP));
    else if (name == "betaRP")   this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_betaRP));
    else if (name == "gammaRP")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_gammaRP));

    else if (name == "shiftXCorr") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_xShiftCorr));
    else if (name == "shiftYCorr") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_yShiftCorr));
    else if (name == "shiftZCorr") this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_zShiftCorr));
    else if (name == "alphaCorr")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_alphaCorr));
    else if (name == "betaCorr")   this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_betaCorr));
    else if (name == "gammaCorr")  this->m_commands.push_back(new Command<double> (&SiGlobAlignData::m_gammaCorr));

    else {
      this->m_commands.push_back(new Command<double> (nullptr));
      return false;
    }

    return true;
  }

} // AFP namespace

