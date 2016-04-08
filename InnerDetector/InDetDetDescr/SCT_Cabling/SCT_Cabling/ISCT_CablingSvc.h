/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISCT_CablingSvc_h
#define ISCT_CablingSvc_h
/**
 * @file ISCT_CablingSvc.h
 * Header file for abstract interface to SCT cabling service
 * @author Shaun Roe
 * @date 20 October, 2008
 **/
 
//standard includes
//#include "boost/cstdint.hpp"
#include <cstdint> //C++11
#include <vector>
//Gaudi includes
#include "GaudiKernel/IInterface.h"

//InnerDetector
//#include "Identifier/IdentifierHash.h"

//to ensure clients can actually use the conversion constructors, include these here (otherwise could be forward declared)
#include "SCT_Cabling/SCT_SerialNumber.h"
#include "SCT_Cabling/SCT_OnlineId.h"


//fwd declarations
/**
 * this works but is not allowed by the standard:
 * http://www.kuzbass.ru/docs/isocpp/lib-intro.html
 * 17.4.3.1
 * namespace std { 
 *  template <typename T> class allocator;
 *  template <typename T, typename A> class vector; 
 * }
 **/


class Identifier;
class IdentifierHash;



/**
 * @class ISCT_CablingSvc
 * Client interface to the cabling, providing conversions between online and offline identifiers
 **/
class ISCT_CablingSvc: virtual public IInterface{
  public:
    /// no-op destructor
    virtual ~ISCT_CablingSvc(){}
   
    /// interfaceID re-implemented from IInterface
    static const InterfaceID & interfaceID();
   
    /// return offline hash, given the online Id (used by decoders)
    virtual IdentifierHash getHashFromOnlineId(const SCT_OnlineId & onlineId, const bool withWarnings = true)  = 0;
   
    /// return the online Id, given a hash (used by simulation encoders)
    virtual SCT_OnlineId getOnlineIdFromHash(const IdentifierHash & hash)  = 0;
  
    /// return the rob/rod Id, given a hash (used by simulation encoders)
  virtual std::uint32_t getRobIdFromHash(const IdentifierHash & hash)  = 0;

    /// return the online Id, given an offlineId
    virtual SCT_OnlineId getOnlineIdFromOfflineId(const Identifier & offlineId)  = 0;
  
    /// return the rob/rod Id, given an offlineId (used by simulation encoders)
    virtual std::uint32_t getRobIdFromOfflineId(const Identifier & offlineId)  = 0;
    
    /// size of the data structure (for the SCT should be 8176, one for each module side)
    virtual unsigned int size() const = 0;
    
    /// is the data structure empty?
    virtual bool empty() const = 0;
    
    /// get hash from a module serial number, needed in the conditions service because configurations are stored by module s/n
    virtual IdentifierHash getHashFromSerialNumber(const SCT_SerialNumber & sn)  = 0;
  
    /// get module serial number from hash, needed during filling of data structure
    virtual SCT_SerialNumber getSerialNumberFromHash(const IdentifierHash & hash)  = 0;
  
    /// fill a users vector with all the RodIds
    virtual void getAllRods(std::vector<std::uint32_t> & usersVector)  = 0;
    
    /// fill a user's vector with all the hash ids which belong to a given rod
    virtual void getHashesForRod(std::vector<IdentifierHash> & usersVector, const std::uint32_t rodId) = 0;
};
 
inline const InterfaceID & ISCT_CablingSvc::interfaceID(){
  static const InterfaceID IID("ISCT_CablingSvc",1,0);
  return IID;
}


#endif

