// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCONDSVC_L1CALOCONDSVC_H
#define TRIGT1CALOCONDSVC_L1CALOCONDSVC_H

#include "AthenaBaseComps/AthService.h"

#include "StoreGate/DataHandle.h"
#include "AthenaKernel/IOVSvcDefs.h"

#include  "AthenaPoolUtilities/AthenaAttributeList.h"
#include  "AthenaPoolUtilities/CondAttrListCollection.h"

#include "AthenaKernel/IAthenaOutputStreamTool.h"
#include "RegistrationServices/IIOVRegistrationSvc.h"

#include "TrigT1CaloCalibConditions/IL1CaloPersistenceCapable.h"

#include <string>
#include <map>

/**
 *  L1CaloCondSvc class.
 *  Service that implement the methods required to store/retrieve
 *  inline payload to/from the condition DB
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class L1CaloCondSvc :  public AthService
{
public:
  L1CaloCondSvc(const std::string& name, ISvcLocator* svc);
  virtual ~L1CaloCondSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  static const InterfaceID& interfaceID();

  virtual StatusCode initialize();
  virtual StatusCode finalize();


  /**
     To store a condition object into the COOL DB

     @warning Only objects that implement the IL1CaloPersistenceCapable interface
     can be stored using this method
  */
  template <typename T>
  StatusCode store(const T*& pobj, const std::string& key, const std::string& folder, const std::string& tag) const;

  /**
     To retrieve a condition object from the COOL DB

     @warning Only objects that implement the IL1CaloPersistenceCapable interface
     can be retrieved using this method
  */
  template <typename T>
  StatusCode retrieve(T*& pobj, const std::string& key = "");

  /**
     To retrieve a condition object from the COOL DB with custom folder mapping.

     @warning Only objects that implement the IL1CaloPersistenceCapable interface
     can be retrieved using this method
  */
  template <typename T, typename FolderMapping>
  StatusCode retrieve(T*& pobj, const FolderMapping& folderKeyMap, const std::string& key = "");

  virtual StatusCode updateConditions(IOVSVC_CALLBACK_ARGS);


private:
  ServiceHandle<StoreGateSvc> m_detStore;
  ServiceHandle<IIOVRegistrationSvc> m_regSvc;

  // properties
  std::string m_streamName;

  bool m_bWriteNewChannels;

  // IOV range
  unsigned int m_iovBeginRun;
  unsigned int m_iovEndRun;
  unsigned int m_iovBeginLB;
  unsigned int m_iovEndLB;
  uint64_t m_iovBeginTime;
  uint64_t m_iovEndTime;
  bool m_bUseTimeStamp;

  // function for the non-differing parts of the two retrieve functions above
  template <typename T>
  StatusCode retrieveImpl(T*& pobj, const std::string& key);

  //
  //StatusCode registerCondition(std::string key, IL1CaloPersistenceCapable* pObj);
  //    template <typename T>
  //    StatusCode registerCondition(std::string key, T*& pobj); // to be replaced
  template <typename T>
  StatusCode registerCondition(T*& pobj, const std::string& mykey="");

  std::map<std::string, const DataHandle<AthenaAttributeList>* > m_mDataHandleAttrList;
  std::map<std::string, const DataHandle<CondAttrListCollection>* > m_mDataHandleAttrListColl;
  std::map<std::string, std::vector<IL1CaloPersistenceCapable*> > m_mConditions;
  std::map<IL1CaloPersistenceCapable*,std::vector<std::string> > m_map_conditions2key;

};

inline const InterfaceID& L1CaloCondSvc::interfaceID() {
  static const InterfaceID IID_L1CaloCondSvc("L1CaloCondSvc",1,0);
  return IID_L1CaloCondSvc;
}

#ifndef TRIGT1CALOCONDSVC_L1CALOCONDSVC_ICC
#include "TrigT1CaloCondSvc/L1CaloCondSvc.icc"
#endif

#endif
