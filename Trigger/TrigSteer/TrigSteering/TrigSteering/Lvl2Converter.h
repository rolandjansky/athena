/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : Lvl2Converter
 *
 * @brief
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: Lvl2Converter.h,v 1.10 2009-03-25 08:17:11 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_LVL2CONVERTER_H
#define TRIGSTEERING_LVL2CONVERTER_H


#include "TrigSteeringEvent/Enums.h"
#include "TrigSteering/LvlConverter.h"

#include <string>

namespace HLT {

  class SteeringChain;

 /**
     @class Lvl2Converter
     Read in the HLTResult from LVL2, extract the chains and the Navigation structure
     and use this to initialize the EF Chains and have a starting Navigation.

     @author Till Eifert     <Till.Eifert@cern.ch>
     @author Nicolas Berger  <Nicolas.Berger@cern.ch>
 */

  class Lvl2Converter : public LvlConverter
  {
  public:

    Lvl2Converter(const std::string& name, const std::string& type,
		  const IInterface* parent); //!< std Gaudi constructor
    void setConfigurationKeys(uint32_t supermaster, uint32_t prescales) 
      { m_smk = supermaster; m_psk = prescales; }
    ErrorCode hltInitialize();
    ErrorCode hltFinalize()   { return HLT::OK; } //!< HLT finalize -> do nothing

    /** @brief convert L2 results, i.e. unpack HLTResult from L2 (chains & navigation);
	       initiate matching EF chains
     */
    ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& chains);


  private:

    std::string m_l2Name;     //!< StoreGate key for L2 HLTResult
    bool m_stickyPassThrough; //!< keep L2 Chain passThrough state ? (otherwise the passThrough state will be re-evaluated using the scaler engine)
    uint32_t m_smk;
    uint32_t m_psk;
    bool m_checkConfig;                  //!< enable/disable checking of the SMKeys in configuration and 
    std::string m_errorLevel;  //!< property to configure which level of error is acceptable 
    HLT::ErrorCode m_maxEC;    //!< decoded version of above
  };
} // end namespace


#endif
