/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LArBadChanLegacyTool_H
#define LArBadChanLegacyTool_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "LArRecConditions/ILArBadChanTool.h"

#include "LArRecConditions/LArBadChannel.h"
#include "LArRecConditions/BadChanContainer.h"
#include "CaloConditions/ICaloBadChanTool.h"

#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingLegacyService.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "StoreGate/DataHandle.h"
#include "LArBadChannelTool/LArBadChannelState.h"
#include "LArBadChannelTool/LArBadChanContainerHashed.h"
#include "LArBadChannelTool/LArBadChanFEBHasher.h"

class StoreGateSvc;

class LArBadChanLegacyTool : public AthAlgTool,
		       public ILArBadChanTool,
		       public ICaloBadChanTool 
{
public:

  typedef LArBadChannelState                                   State;
  typedef LArBadChanContainerHashed::BadFebEntry               BadFebEntry;
  typedef std::vector<BadFebEntry>                             BadFebVec;

  LArBadChanLegacyTool(const std::string& type, const std::string& name, 
		    const IInterface* parent);
					  
// to allow access to the  interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );


  StatusCode initialize();
  // StatusCode finalize();

  LArBadChannel status( HWIdentifier id) const;

  LArBadChannel status( const HWIdentifier& FEBid, int chan) const;

  LArBadChannel offlineStatus( Identifier id) const;

  virtual bool febMissing(  HWIdentifier febId) const {
    return m_HwBadChan.febBad( febId);
  }
  virtual bool febAllGood(  HWIdentifier febId) const {
    return m_HwBadChan.febAllGood( febId);
  }

  virtual LArBadFeb febStatus( HWIdentifier febId) const {
    return  m_HwBadChan.febStatus( febId);
  }

/// Converts a LArBadChannel status to CaloBadChannel, thus implementing the 
/// ICaloBadChannelTool interface
  virtual CaloBadChannel caloStatus( Identifier id) const;

  static const InterfaceID& interfaceID();

  const State& fullState() const {return m_State;}

  std::vector<HWIdentifier> missingFEBs() const;

  const BadFebVec& fullBadFebsState() const {return m_BadFebs;}

  // Callback functions
  virtual StatusCode updateFromDB(IOVSVC_CALLBACK_ARGS);
  virtual StatusCode updateBadFebsFromDB(IOVSVC_CALLBACK_ARGS);

private:

  typedef LArBadChanContainerHashed                      BadChanInfo;
  typedef BadChanInfo::BadChanVec                        BadChanVec;
  typedef BadChanContainer<Identifier, LArBadChannel>    OfflineInfo;
  typedef OfflineInfo::BadChanVec                        OfflineVec;

  const DataHandle<LArOnlineID> m_onlineID;
  const DataHandle<CondAttrListCollection>  m_DBColl;
  const DataHandle<CondAttrListCollection>  m_TmpDBColl;
  const DataHandle<AthenaAttributeList>          m_DBBadFebColl;
  ToolHandle<LArCablingLegacyService> m_cablingService;

  mutable BadChanInfo      m_HwBadChan; //should be non-mutable if possible
  mutable OfflineInfo      m_OfflineBadChan;
  State                    m_State;
  State                    m_Updates;
  BadFebVec                m_BadFebs;
  BadFebVec                m_BadFebUpdates;

  bool                 m_updatedFromDB;
  bool                 m_updatedFebsFromDB;
  mutable bool         m_ready;

  bool                 m_readFromASCII;
  int                  m_ASCII_action;
  bool                 m_writeEmptyFolders;
  std::vector<std::string> m_fileNames;
  std::string              m_febFileName;
  std::string              m_DBFolder;
  std::string              m_TmpDBFolder;
  std::string              m_DBBadFebFolder;
  bool                     m_dumpCache;
  mutable bool             m_OfflineCacheInvalid;

  bool prepareFebHash();
  void check() const {if (!m_ready) complain();} // keep it inline for speed

  void complain() const;
  bool readASCII();

  void fillOfflineInfo() const;
  void updateCache();
  void updateFebCache();

  bool readFromDB();
  bool readFromDB( const DataHandle<CondAttrListCollection> collection);
  bool readBadFebsFromDB();
  void dumpHWCache() const;
  void applyUpdates();
  void applyUpdate( State::CoolChannelData& coolChan, const State::BadChanEntry& entry);
  void applyFebUpdates();

  void dumpAscii( const std::string& fileName) const;
  void dumpFEBsAscii( const std::string& fileName) const;
  void warnInvalidFEB( const HWIdentifier& id) const;

};

#endif
