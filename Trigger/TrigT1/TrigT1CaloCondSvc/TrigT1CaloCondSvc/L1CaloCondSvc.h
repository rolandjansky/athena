/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCONDSVC_L1CALOCONDSVC_H
#define TRIGT1CALOCONDSVC_L1CALOCONDSVC_H

#include "GaudiKernel/Service.h"
#include "StoreGate/StoreGate.h"

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

class L1CaloCondSvc :  virtual public IInterface, virtual public Service
{
    template <class TYPE> class SvcFactory;

public:
    L1CaloCondSvc(const std::string& name, ISvcLocator* svc);
    virtual ~L1CaloCondSvc();

    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

    static const InterfaceID& interfaceID();

    virtual const InterfaceID& type() const;

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
       The second argument is a map of TrigT1CaloCalibConditions/L1CaloXXXContainer::eCoolFolders to
       a string given the correct folder path.

        @warning Only objects that implement the IL1CaloPersistenceCapable interface
        can be retrieved using this method
    */
    template <typename T, typename FolderType>
    StatusCode retrieve(T*& pobj, const std::map<FolderType, std::string>& folderKeyMap, const std::string& key = "");

	virtual StatusCode updateConditions(IOVSVC_CALLBACK_ARGS);


private:

    // services
    StoreGateSvc* m_detStore;

    IIOVRegistrationSvc*  m_regSvc;
    IAthenaOutputStreamTool* m_streamer;

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
    StatusCode registerCondition(T*& pobj);

    std::map<std::string, const DataHandle<AthenaAttributeList>* > m_mDataHandleAttrList;
    std::map<std::string, const DataHandle<CondAttrListCollection>* > m_mDataHandleAttrListColl;
    //std::map<std::string, IL1CaloPersistenceCapable*> m_mConditions;
    std::map<std::string, std::vector<IL1CaloPersistenceCapable*> > m_mConditions;

};

inline const InterfaceID& L1CaloCondSvc::interfaceID() {
    static const InterfaceID IID_L1CaloCondSvc("L1CaloCondSvc",1,0);
    return IID_L1CaloCondSvc;
}

#ifndef TRIGT1CALOCONDSVC_L1CALOCONDSVC_ICC
 #include "TrigT1CaloCondSvc/L1CaloCondSvc.icc"
#endif

#endif
