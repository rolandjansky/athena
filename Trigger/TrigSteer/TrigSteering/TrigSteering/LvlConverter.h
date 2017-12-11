/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @class  : LvlConverter
 *
 * @brief  Base class for HLT Lvl1Converter, Lvl2Converter, ...
 *
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: LvlConverter.h,v 1.20 2009-03-25 08:17:11 tbold Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_LVLCONVERTER_H
#define TRIGSTEERING_LVLCONVERTER_H

#include "AthenaBaseComps/AthMsgStreamMacros.h"
#include "AthenaBaseComps/AthCheckMacros.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "TrigSteeringEvent/Enums.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrigSteering/HLTResultAccessTool.h"

#include "L1TopoEvent/TopoInputEvent.h"

#include <vector>
#include <map>

// forward declarations
class StatusCode;


namespace HLT {

   // forward declarations
   class AlgoConfig;
   class SteeringChain;
   class Navigation;
   class HLTResult;
   class TriggerElement;


   static const InterfaceID IID_LvlConverter("HLT::ILvlConverter", 1, 0); //!< std Gaudi interface

   /**
      @class ILvlConverter
      Interface to the base class for all the HLT level converter tools. This is what is used
      in e.g. the TrigSteer algorithm.

      Note: All methods are abstract, some implementations are in the LvlConverter base class.

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */

   class ILvlConverter : public virtual IAlgTool
   {
   public:

      virtual void setConfig(HLT::AlgoConfig* conf) = 0; //!< Set pointer to common config object
      virtual void setConfigurationKeys(uint32_t supermaster, uint32_t prescales) = 0;

      virtual ErrorCode setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains) = 0; //!< Setup a search map of all configured chains
      virtual void setIncludePrescaledChains(bool yn) = 0;
      /** @brief execute LvlConverter for one event

      @param activeChains reference to vector holding active chains; to be filled by the method !
      */
      virtual ErrorCode hltExecute(std::vector<HLT::SteeringChain*>& activeChains) = 0;

      virtual ErrorCode deserializeNavigation(HLT::Navigation& nav, const HLTResult& result) = 0; //!< deserialize Navigation from given HLTResult into nav

      /** @brief activate chains that match a previous chain

      @param chains vector holding the chains to be activated
      @param previousChains vector holding chains from previous level
      @param stickyPassThrough should Chains keep passThrough from the previous Chain ? (false: passThrough will be re-evaluated by scaler engine)
      */
      virtual ErrorCode activateChains(std::vector<SteeringChain*>& chains,
                                       const std::vector<Chain>& previousChains, bool stickyPassThrough=true ) = 0;

      static const InterfaceID& interfaceID() { return IID_LvlConverter; } //!< Gaudi interface method

      virtual const std::map<uint32_t, HLT::TriggerElement*> * roIWordToTE() const { return nullptr; }

   };


   /**
      @class LvlConverter
      abstract base class for all HLT level converter tools

      This class implements the methods from the interface that are common to all
      HLT level converter tools: setConfig(..), setConfiguredChains(..),
      deserializeNavigation(..), activateChains(..).

      To be implemented by the concrete tool: hltExecute(..) !

      @author Till Eifert     <Till.Eifert@cern.ch>
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class LvlConverter : public AthAlgTool, public virtual ILvlConverter
   {
   public:

      LvlConverter(const std::string& name, const std::string& type,
                   const IInterface* parent); //!< std Gaudi constructor

      virtual ~LvlConverter() {} //!< virtual desctructor

      virtual void setConfig(HLT::AlgoConfig* conf) { m_config = conf; } //!< Set pointer to common config object
      virtual void setConfigurationKeys(uint32_t, uint32_t) { }
      virtual ErrorCode setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains); //!< Setup a search map of all configured chains

      virtual StatusCode initialize(); //!< Gaudi initialize
      virtual StatusCode finalize();   //!< Gaudi finalize

      virtual ErrorCode deserializeNavigation(HLT::Navigation& nav, const HLTResult& result); //!< deserialize Navigation from given HLTResult into nav

      /** @brief activate chains that match a previous chain
          @param chains vector holding the chains to be activated
          @param previousChains vector holding chains from previous level
          @param stickyPassThrough should Chains keep passThrough from the previous Chain ? (false: passThrough will be re-evaluated by scaler engine)
      */
      virtual ErrorCode activateChains(std::vector<HLT::SteeringChain*>& chains,
                                       const std::vector<HLT::Chain>& previousChains, bool stickyPassThrough=true ); //!< activate chains that match a previous chain
      virtual void setIncludePrescaledChains(bool yn) { m_includePrescaledChains = yn; }

   protected:
      void setEvent();

      bool addChain(std::vector<HLT::SteeringChain*>& activeChains, HLT::SteeringChain* ch,
                    bool isAfterPrescale,
                    bool isBeforePrescale, bool stickyPassThrough=false);

      virtual ErrorCode hltInitialize() = 0; //!< Initialize derived classes
      virtual ErrorCode hltFinalize() = 0;   //!< Finalize derived classes

      AlgoConfig* m_config;  //!< config object, holding common variables
      bool m_includePrescaledChains;   //!< include chains which originate from prescaled lower-level chains
      bool m_ignorePrescales;
    
      ToolHandle< IHLTResultAccessTool > m_hltTool; //!< helper access tool for HLTResult

      std::map< unsigned int, std::vector<HLT::SteeringChain*> > m_chainIdMap;      //!< search map of all chains, with lower_chain_counter as Key
      std::map< unsigned int, std::vector<HLT::SteeringChain*> > m_chainCounterMap; //!< search map of all chains, with lower_chain_hash_id as Key
      const std::vector<HLT::SteeringChain*>* m_configuredChains;                   //!< local reference to all configured chains of this level

      std::vector< HLT::SteeringChain* > m_chainsAlwaysActive; //!< Remeber chains w/o a lower_chain_name => will always be activated !

   };

} // end namespace




#endif
