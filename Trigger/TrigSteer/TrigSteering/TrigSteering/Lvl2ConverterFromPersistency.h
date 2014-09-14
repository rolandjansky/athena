/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl2ConverterFromPersistency
 *
 * @brief
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl2ConverterFromPersistency.h,v 1.6 2009-01-16 11:09:49 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_Lvl2ConverterFromPersistency_H
#define TRIGSTEERING_Lvl2ConverterFromPersistency_H


#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"
#include "GaudiKernel/ToolHandle.h"

#include <string>

namespace HLT {

  class Chain;
  class Navigation;
  /**
     @class Lvl2ConverterFromPersistency

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
  */

  class Lvl2ConverterFromPersistency : public LvlConverter
  {
  public:

    Lvl2ConverterFromPersistency(const std::string& name, const std::string& type,
				 const IInterface* parent);

    ErrorCode hltInitialize();
    ErrorCode hltFinalize();

    virtual ErrorCode hltExecute(std::vector<SteeringChain*>& chains);

  private:

    //properties:
    std::string m_l2Name;
    std::string m_efName;
    ToolHandle<Navigation> m_tmpNavigation; 


  };
} // end namespace


#endif
