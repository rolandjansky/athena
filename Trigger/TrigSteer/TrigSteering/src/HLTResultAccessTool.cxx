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
 * @author Till Eifert     <Till.Eifert@cern.ch>     - U. of Geneva, Switzerland
 * @author Nicolas Berger  <Nicolas.Berger@cern.ch>  - CERN
 *
 * File and Version Information:
 * $Id: HLTResultAccessTool.cxx,v 1.11 2009-02-10 17:36:45 fwinkl Exp $
 **********************************************************************************/

#include "TrigSteering/HLTResultAccessTool.h"

#include "TrigSteeringEvent/Chain.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigNavigation/Navigation.h"
#include <iostream>


using namespace HLT;


HLTResultAccessTool::HLTResultAccessTool(const std::string& name, const std::string& type,
                                         const IInterface* parent)
  : AthAlgTool(name, type, parent)
{
  declareInterface<HLT::IHLTResultAccessTool>( this );
}

StatusCode HLTResultAccessTool::initialize()
{
   ATH_MSG_DEBUG("initialized " << name());
   return StatusCode::SUCCESS;
}

StatusCode HLTResultAccessTool::finalize()
{
   return StatusCode::SUCCESS;
}


StatusCode HLT::HLTResultAccessTool::updateResult(const HLTResult& result, AlgoConfig* /*config*/)
{
   // some debug output
   ATH_MSG_DEBUG("Reading HLTResult for Lvl1ID=" << result.getLvl1Id()
                 << " Event is Accepted=" << result.isAccepted()
                 << " passedThrough=" << result.isPassThrough()
                 << " HLTStatus=" << (unsigned int)result.getHLTStatus()
                 << " size of rawResult=" << result.size()
                 << " words (=" << result.size()*4 << " bytes)");
  
   const std::vector<uint32_t>& chainsData = result.getChainResult();
   if (chainsData.size() == 0) {
      ATH_MSG_DEBUG("chains data in HLTResult has size 0!");
      return StatusCode::FAILURE;
   }

   if (chainsData.size() == 1) {
      ATH_MSG_WARNING("chains data in HLTResult has size 1 => no accepted chain, event should have been rejected!");
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("Found " << chainsData[0] << " chains in HLTResult");

   // deserialize the chains:
   std::vector<uint32_t>::const_iterator pos = chainsData.begin();

   m_chains.clear();

   pos++; // first entry is number of chains!

   m_chains.reserve(chainsData[0]+1); // speed up processing by already reserving enough space

   for (unsigned int i = 0; i < chainsData[0]; ++i) {
      Chain myChain(*pos);
      m_chains.push_back(myChain); // this is calling the Chain class's copy constructor !
      pos ++;
      ATH_MSG_DEBUG(myChain);
   }
   return StatusCode::SUCCESS;
}


StatusCode HLTResultAccessTool::deserializeNavigation(Navigation& nav, const HLTResult& result)
{
   const std::vector<uint32_t>& navData = result.getNavigationResult();
   if (navData.size() == 0) {
      ATH_MSG_ERROR("navigation data in HLTResult has size 0!");
      return StatusCode::FAILURE;
   }

   bool status = nav.deserialize( navData );

   if ( !status ) {
      ATH_MSG_WARNING("problems while deserializing navigation, will stop now");
      return StatusCode::FAILURE;
   }

   ATH_MSG_DEBUG("\n" << nav);

   return StatusCode::SUCCESS;
}


StatusCode
HLTResultAccessTool::queryInterface( const InterfaceID& riid,
                                     void** ppvIf )
{
   if ( riid == IHLTResultAccessTool::interfaceID() )  {
      *ppvIf = (IHLTResultAccessTool*)this;
      addRef();
      return StatusCode::SUCCESS;
   }
   
   return AlgTool::queryInterface( riid, ppvIf );
}
