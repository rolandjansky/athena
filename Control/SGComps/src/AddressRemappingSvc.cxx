/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** @file AddressRemappingSvc.cxx
 *  @brief This file contains the implementation for the AddressRemappingSvc class.
 *  @author Peter van Gemmeren <gemmeren@anl.gov>
 *  $Id: AddressRemappingSvc.cxx,v 1.7 2009-05-04 15:47:18 calaf Exp $
 **/

#include "AddressRemappingSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"

#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IProxyDict.h"
#include "AthenaKernel/IStringPool.h"

#include "SGTools/DataProxy.h"
#include "SGTools/TransientAddress.h"

#include <algorithm>

#include "boost/foreach.hpp"

//________________________________________________________________________________
AddressRemappingSvc::AddressRemappingSvc(const std::string& name, ISvcLocator* pSvcLocator)
	: ::AthService(name, pSvcLocator),
        m_clidSvc("ClassIDSvc", name),
	m_proxyDict("StoreGateSvc", name),
	m_oldTads(),
	m_newTads() {
   declareProperty("ProxyDict", m_proxyDict,
     "the IProxyDict we want to apply the remapping to (by default the event store)");
   declareProperty("TypeKeyOverwriteMaps", m_overwriteMaps);
   declareProperty("SkipBadRemappings", m_skipBadRemappings=false,"If true, will delay the remapping setup until the first load, and will check against the given file");
}
//__________________________________________________________________________
AddressRemappingSvc::~AddressRemappingSvc() {
}
//__________________________________________________________________________
StatusCode AddressRemappingSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version "
                << PACKAGE_VERSION);

   StatusCode status = ::AthService::initialize();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Can not initialize Service base class.");
      return(status);
   }

   status = m_clidSvc.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Unable to get the ClassIDSvc");
      return(status);
   }
   m_oldTads.clear();
   m_newTads.clear();
   for (std::vector<std::string>::const_iterator iter = m_overwriteMaps.value().begin(),
                   iterEnd = m_overwriteMaps.value().end(); iter != iterEnd; iter++) {
      const std::string::size_type p_sep = iter->find("->");
      if (p_sep == std::string::npos) {
         ATH_MSG_ERROR("Unexpected format in TypeKeyOverwriteMaps: " << *iter);
         return(StatusCode::FAILURE);
      }
      //const std::pair<std::string, std::string> entry(iter->substr(0, p_sep), iter->substr(p_sep + 2));
      const std::pair<std::string, std::string> entry(iter->substr(p_sep + 2), iter->substr(0, p_sep));
      ATH_MSG_INFO("TypeKeyOverwriteMaps for: " << entry.second
                   << " -> " << entry.first);
      const std::string::size_type p_oldSep = entry.first.find("#");
      if (p_oldSep == std::string::npos) {
         ATH_MSG_ERROR("Unexpected format in TypeKeyOverwriteMaps: " << *iter);
         return(StatusCode::FAILURE);
      }

      std::string clidStr = entry.first.substr(0, p_oldSep);
      std::set<CLID> symClids;
      for (std::string::size_type p_clidSep = clidStr.rfind(","); p_clidSep != std::string::npos; clidStr = clidStr.substr(0, p_clidSep), p_clidSep = clidStr.rfind(",")) {
         symClids.insert(getClid(clidStr.substr(p_clidSep + 1)));
      }
      std::string keyStr = entry.first.substr(p_oldSep + 1);
      std::set<std::string> aliases;
      for (std::string::size_type p_keySep = keyStr.rfind(","); p_keySep != std::string::npos; keyStr = keyStr.substr(0, p_keySep), p_keySep = keyStr.rfind(",")) {
         aliases.insert(keyStr.substr(p_keySep + 1));
      }
      SG::TransientAddress oldTad(getClid(clidStr), keyStr);
      for (std::set<CLID>::const_iterator clidIter = symClids.begin(), clidEnd = symClids.end();
		      clidIter != clidEnd; clidIter++) {
         oldTad.setTransientID(*clidIter);
      }
      oldTad.setAlias(aliases);

      const std::string::size_type p_newSep = entry.second.find("#");
      if (p_newSep == std::string::npos) {
         ATH_MSG_ERROR("Unexpected format in TypeKeyOverwriteMaps: " << *iter);
         return(StatusCode::FAILURE);
      }
      SG::TransientAddress newTad(getClid(entry.second.substr(0, p_newSep)), entry.second.substr(p_newSep + 1));
      ATH_MSG_INFO("TypeKeyOverwriteMaps for: " << newTad.clID()
                   << "#" << newTad.name()
                   << " -> " << oldTad.clID()
                   << "#" << oldTad.name());
      m_oldTads.push_back(oldTad);
      m_newTads.push_back(newTad);
   }
   return(StatusCode::SUCCESS);
}
//__________________________________________________________________________
StatusCode AddressRemappingSvc::finalize() {
   StatusCode status = m_clidSvc.release();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Cannot release ClassIDSvc");
      return(status);
   }
   status = m_proxyDict.release();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Cannot release IProxyDict");
      return(status);
   }
   status = ::AthService::finalize();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Can not finalize Service base class.");
      return(status);
   }
   return(StatusCode::SUCCESS);
}
//_______________________________________________________________________
StatusCode AddressRemappingSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) {
   // Interface is not directly available: try out a base class
   return(::AthService::queryInterface(riid, ppvInterface));
}
//________________________________________________________________________________
StatusCode AddressRemappingSvc::preLoadAddresses(StoreID::type /*storeID*/,
	IAddressProvider::tadList& tads) {
if(m_skipBadRemappings) return StatusCode::SUCCESS;
   StatusCode status = m_proxyDict.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Unable to get the IProxyDict");
      return(status);
   }
   IStringPool* strPool = dynamic_cast<IStringPool*>(&*m_proxyDict);
   for (std::vector<SG::TransientAddress>::const_iterator oldIter = m_oldTads.begin(),
		   newIter = m_newTads.begin(), oldIterEnd = m_oldTads.end();
		   oldIter != oldIterEnd; oldIter++, newIter++) {
      SG::TransientAddress* tadd = new SG::TransientAddress(*oldIter);
      tads.push_back(tadd);
      if (strPool != 0) {
         strPool->stringToKey(oldIter->name(), oldIter->clID());
         strPool->stringToKey(newIter->name(), newIter->clID());
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode AddressRemappingSvc::loadAddresses(StoreID::type /*storeID*/,
	IAddressProvider::tadList& tads) {
if(!m_skipBadRemappings) return(StatusCode::SUCCESS);

   //do same as in preLoadAddresses, except check each tad will have a valid proxy to remap to 
   StatusCode status = m_proxyDict.retrieve();
   if (!status.isSuccess()) {
      ATH_MSG_ERROR("Unable to get the IProxyDict");
      return(status);
   }
   IStringPool* strPool = dynamic_cast<IStringPool*>(&*m_proxyDict);
   for (std::vector<SG::TransientAddress>::iterator oldIter = m_oldTads.begin(),
		   newIter = m_newTads.begin(), oldIterEnd = m_oldTads.end();
		   oldIter != oldIterEnd; oldIter++, newIter++) {
      CLID goodCLID = newIter->clID(); //newIter are the things we are remapping to 
      SG::TransientAddress::TransientClidSet clidToKeep(oldIter->transientID());
      //try dataproxy, if it fails, try data proxy of next type 
      SG::DataProxy* dataProxy(m_proxyDict->proxy(goodCLID,newIter->name()/*the name of the address in the input file*/));
      if(dataProxy==0) {
         //remove goodCLID from the clidToKeep 
         clidToKeep.erase(goodCLID);
         //try base types 
         const SG::BaseInfoBase* bi = SG::BaseInfoBase::find (newIter->clID());
         if (bi) {
            BOOST_FOREACH(goodCLID, bi->get_bases()) {
               dataProxy = m_proxyDict->proxy(goodCLID, newIter->name());
               if(dataProxy) break;
               clidToKeep.erase(goodCLID); //was a bad CLID, so get rid of it
            }
         }
         if(dataProxy) {
            ATH_MSG_DEBUG("Remapping " << oldIter->clID() << "#" << oldIter->name() << " to base-class CLID: " << goodCLID << "#" << newIter->name());
            //modify newIter to have these restricted clid instead 
            *newIter = SG::TransientAddress(goodCLID, newIter->name());
         }
      }
      if(dataProxy==0) {
         ATH_MSG_INFO("Skipping remapping of " << oldIter->clID() << "#" << oldIter->name() << " because no suitable remapping found");
         continue;
      }
      SG::TransientAddress* tadd = new SG::TransientAddress(goodCLID,oldIter->name(),oldIter->address());
      tadd->setAlias(oldIter->alias());//match the aliases  
      //also match the transientIDs, but only for the ok CLID 
      for(auto tIter = clidToKeep.begin(); tIter != clidToKeep.end(); ++tIter) {
         if(goodCLID == *tIter) continue; //already added in constructor 
         tadd->setTransientID(goodCLID);
      }
      //replace oldIter tad with this new tadd if we have changed the primary CLID
      if(oldIter->clID()!=tadd->clID()) {
         *oldIter = *tadd; //relies on assignment operator
      }
      tads.push_back(tadd);
      if (strPool != 0) {
         strPool->stringToKey(oldIter->name(), oldIter->clID());
         strPool->stringToKey(newIter->name(), newIter->clID());
      }
   }


   m_skipBadRemappings=false; //only do this once, not every event .. FIXME: Should I be rechecking every new file? For now, we assume will not chain together different sample types
   return StatusCode::SUCCESS;

}
//________________________________________________________________________________
StatusCode AddressRemappingSvc::updateAddress(StoreID::type /*storeID*/,
					      SG::TransientAddress* tad) {
   for (std::vector<SG::TransientAddress>::const_iterator oldIter = m_oldTads.begin(),
		   newIter = m_newTads.begin(), oldIterEnd = m_oldTads.end();
		   oldIter != oldIterEnd; oldIter++, newIter++) {
      if (oldIter->transientID(tad->clID())
	      && (oldIter->name() == tad->name() || oldIter->alias().find(tad->name()) != oldIter->alias().end())) {
         ATH_MSG_DEBUG("Overwrite for: " << tad->clID() << "#" << tad->name() << " -> " << newIter->clID() << "#" << newIter->name());
         SG::DataProxy* dataProxy(m_proxyDict->proxy(newIter->clID(), newIter->name()));
         if (dataProxy == 0) {
            ATH_MSG_WARNING("Cannot get proxy for: " << newIter->clID() << "#" << newIter->name());
            return(StatusCode::FAILURE);
         }
         GenericAddress* newGadd = 0;
	 if (dataProxy->isValidAddress()) newGadd = dynamic_cast<GenericAddress*>(dataProxy->address());
         if (newGadd == 0) {
            ATH_MSG_WARNING("Cannot get GenericAddress for: " << newIter->clID() << "#" << newIter->name());
            return(StatusCode::FAILURE);
         }
         GenericAddress* oldGadd = new GenericAddress(newGadd->svcType(), tad->clID(), *newGadd->par(), tad->name(), *newGadd->ipar());
         tad->setAddress(oldGadd);
         return(StatusCode::SUCCESS);
      }
   }
   return(StatusCode::FAILURE);
}
//________________________________________________________________________________
CLID AddressRemappingSvc::getClid(std::string type) const {
   CLID clid(atoi(type.c_str())); // Is type a CLID? 
   if (clid == 0) { // or not
      if (!m_clidSvc->getIDOfTypeName(type, clid).isSuccess()) {
         return(0);
      }
   }
   return(clid);
}
