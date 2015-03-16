/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: HLT Steering
 * @Package: TrigSteering
 * @Class  : HLTResultAccessTool
 *
 * @brief Tool to extract HLT chain information from HLTResult
 *
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: HLTResultAccessTool.h,v 1.5 2007-07-04 18:38:52 eifert Exp $
 **********************************************************************************/

#ifndef TRIGSTEERING_HLT_HLTResultAccessTool_H
#define TRIGSTEERING_HLT_HLTResultAccessTool_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TrigSteeringEvent/Enums.h"
#include "TrigSteeringEvent/Chain.h"

#include <vector>

class StatusCode;
class MsgStream;

namespace HLT {

   class AlgoConfig;
   class Navigation;
   class HLTResult;


   static const InterfaceID IID_HLTResultAccessTool("HLT::IHLTResultAccessTool", 1, 0);


   /**
      @class IHLTResultAccessTool
      interface
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class IHLTResultAccessTool : public virtual IAlgTool
   {
   public:

      virtual StatusCode updateResult(const HLTResult& result, AlgoConfig* config) = 0; //!< abstract: get new result from given HLTResult
      virtual const std::vector<Chain>& getChains() const = 0; //!< abstract: return vector of Chains

      virtual StatusCode deserializeNavigation(Navigation& navigation, const HLTResult& result) = 0; //!< abstract: deserialize HLT Navigation

      static const InterfaceID& interfaceID() { return IID_HLTResultAccessTool; } //!< std interface declaration
   };


   /**
      @class HLTResultAccessTool Tool to extract HLT chain information from HLTResult
      @author Nicolas Berger  <Nicolas.Berger@cern.ch>
   */
   class HLTResultAccessTool : virtual public AthAlgTool, public virtual IHLTResultAccessTool
   {
   public:

      HLTResultAccessTool(const std::string& name, const std::string& type,
                          const IInterface* parent); //!< std Gaudi constructor

      virtual ~HLTResultAccessTool() {} //!< virtual desctructor

      StatusCode initialize(); //!< Gaudi initialize
      StatusCode finalize();   //!< Gaudi finalize

      StatusCode updateResult(const HLTResult& result, AlgoConfig* config); //!< load (deserialize) HLT::Chain objects from HLTResult (this should be called once per event before getChains !!!)
      const std::vector<Chain>& getChains() const { return m_chains; }      //!< Accessor for deserialized HLT::Chain objects

      StatusCode deserializeNavigation(Navigation& navigation, const HLTResult& result); //!< deserialize HLT::Navigation from HLTResult (no need to execute updateResult just for the Navigation!)


      StatusCode queryInterface( const InterfaceID& riid, void** ppvIf ); //!< Gaudi interface method
      static const InterfaceID& interfaceID() { return IHLTResultAccessTool::interfaceID(); }  //!< Gaudi interface method

   protected:

      std::vector<Chain> m_chains;  //!< deserialized HLT::Chain objects
   };

} // end namespace




#endif
