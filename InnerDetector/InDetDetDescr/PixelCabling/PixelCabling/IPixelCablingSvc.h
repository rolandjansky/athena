/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// Interface for cabling service
///////////////////////////////////////////////////////////////////
//  Florian Hirsch
///////////////////////////////////////////////////////////////////

#ifndef IPIXELCABLINGSVC_H
#define IPIXELCABLINGSVC_H

#include "GaudiKernel/IInterface.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include <deque>
#include <vector>

// forward declarations
class IdentifierHash;
class Identifier;


/** Interface class for PixelCablingSvc
 *  See PixelCablingSvc.h for further documentation
 */
class IPixelCablingSvc: virtual public IInterface {

 public:
  
   // destructor	
  virtual ~IPixelCablingSvc(){};
  
  // InterfaceID
  static const InterfaceID& interfaceID( ) ;

  // callback function
  virtual StatusCode IOVCallBack(IOVSVC_CALLBACK_ARGS) = 0;

  // Get a list of offlineIds from a RobId.
  virtual void getOfflineList(std::vector<IdentifierHash>& offlineIdHashList, int robid) = 0;
  // Get offlineId from a certain onlineId
  virtual Identifier getOfflineId(uint64_t onlineId) = 0;
  virtual IdentifierHash getOfflineIdHash(uint64_t onlineId) = 0;
  virtual Identifier getOfflineIdFromDCS(std::string) = 0;
  
  // Get RobId from a certain offlineId
  virtual uint32_t getRobId(Identifier offlineId) = 0; 

  // For backward compatibility
  virtual uint32_t getRobID(Identifier offlineId) = 0; 

  // Get RobId from a RodId
  virtual int getRobId(const int rodId) = 0;

  // Get RODId from a ROBId
  virtual int getRodId(const int robId) = 0;

  // get onlineId from a offlineId
  virtual uint64_t getOnlineId(Identifier offlineId) = 0;

  virtual Identifier getPixelIdfromHash(IdentifierHash offlineIdHash, uint32_t FE, uint32_t row, uint32_t column) = 0;
  virtual Identifier getPixelId(Identifier offlineId, uint32_t FE, uint32_t row, uint32_t column) = 0;

  virtual uint32_t getFE(Identifier * pixelId, Identifier offlineId) = 0;
  virtual uint32_t getFEwrtSlink(Identifier *pixelId) = 0; 
  virtual uint32_t getColumn(Identifier * pixelId, Identifier offlineId) = 0;
  virtual uint32_t getRow(Identifier * pixelId, Identifier offlineId) = 0;

  virtual uint64_t getOnlineIdFromRobId(const uint32_t robId, const uint32_t link_module) = 0;
  virtual unsigned int getLocalFEI4(const uint32_t fe, const uint64_t onlineId) = 0;

  virtual bool isIBL(const uint32_t robId) = 0;
  virtual bool isIBL(const Identifier& pixelId) = 0;
  virtual bool isDBM(const uint32_t robId) = 0;
  virtual bool isDBM(const Identifier& pixelId) = 0;
  
  virtual std::vector<uint32_t>& getAllRods() = 0;
  virtual std::vector<uint32_t>& getAllRobs() = 0;

  enum moduletype {DBM, IBL, PIX_BARREL, PIX_ENDCAP, NONE};
  virtual moduletype getModuleType(const Identifier& id) = 0;

  virtual int getHitDiscCnfg(const uint32_t robId, const int link) = 0;
  virtual int getHitDiscCnfg(Identifier* pixelId) = 0;


   // Wrappers to PixelCablingData -- get maps
   virtual std::map< uint64_t, Identifier > get_idMap_onoff() = 0;
   virtual std::map< Identifier, uint64_t> get_idMap_offon() = 0;
   virtual std::map< Identifier, uint32_t> get_idMap_offrob() = 0;
   virtual std::map< int,int> get_idMap_rodrob() = 0;
   virtual std::map< int,int> get_idMap_robrod() = 0;
   virtual std::map< std::string, Identifier> get_idMapDCSoff() = 0;


};

inline const InterfaceID& IPixelCablingSvc::interfaceID(){
  static const InterfaceID IID("IPixelCablingSvc",1,0);
  return IID;
}


#endif   // IPIXELCABLINGSVC_H
