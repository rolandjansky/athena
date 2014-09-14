/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl1FromFile
 *
 * @brief produce TriggerElements from a dummy level 1 result file and simply activate all HLT chains
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl1FromFile.h,v 1.6 2009-02-16 15:47:04 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_LVL1FROMFILE_H
#define TRIGSTEERING_LVL1FROMFILE_H


#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"

#include <fstream>
#include <iostream>


namespace HLT {

  class SteeringChain;
  class TriggerElement;
 /**
     @class Lvl1FromFile
     Produce TriggerElements from a dummy level 1 result file and simply activate all HLT chains

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
*/
  class Lvl1FromFile : public LvlConverter
  {
  public:

    Lvl1FromFile(const std::string& name, const std::string& type,
		 const IInterface* parent); //!< std Gaudi constructor

    ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& activeChains); //!< HLT execute -> see class desciption
    ErrorCode hltInitialize(); //!< HLT initialize -> open L1 ascii file
    ErrorCode hltFinalize();   //!< HLT finalize   -> close L1 file

  private:
    //properties:
    std::string m_fileName;  //!< string for dummy Lvl1 results file

    std::ifstream m_file;  //!< filestream, in case Level 1 result is read from file
    unsigned int m_lvl1ID; //!< Level 1 ID, in case Level 1 result is read from file

    ErrorCode activate(const std::string& str, std::vector<HLT::SteeringChain*>& chains);
    ErrorCode activate(const std::string& str, HLT::TriggerElement* initialTE);
    unsigned m_overallRoIsLimit;
  };
} // end namespace


#endif
