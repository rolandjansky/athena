/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_StorageMgr
#define TrigConf_StorageMgr 

#include "TrigConfStorage/StorageMgrBase.h"
#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfStorage/IStorageMgr.h"
#include "TrigConfStorage/SessionMgr.h"

#include <string>
#include <memory>

#include "TrigConfBase/MsgStream.h"

namespace TrigConf {


   /**@brief Database Storage Manager, controls the database session
      and the different loader classes for DB access*/
   class StorageMgr : public StorageMgrBase, public TrigConfMessaging, virtual public IStorageMgr {
   public:

      /**
       * @brief constructor
       *
       * @param cs connection string @see SessionMgr::SessionMgr()
       * @param user user name, if not specified the XML authentication will be used
       * @param password password string (only used in connection with a user name)
       * @param o output stream for all messages
       */
      StorageMgr( const std::string& cs, 
                  const std::string& user = "", 
                  const std::string& password = "",
                  std::ostream & o = std::cout);

      /**
       * @brief constructor
       *
       * @param type Server technology @see SessionMgr::SessionMgr()
       * @param server Server name @see SessionMgr::SessionMgr()
       * @param name Database name, or schema name, or alias @see SessionMgr::SessionMgr()
       * @param user user name, if not specified the XML authentication will be used
       * @param password password string (only used in connection with a user name)
       * @param o output stream for all messages
       */
      StorageMgr( const std::string& type, const std::string& server, 
                  const std::string& name, const std::string& user , 
                  const std::string& password , std::ostream & o = std::cout);
      
      /**destructor*/
      virtual ~StorageMgr();

      // setting log level of all loaders
      void setLevel(MSGTC::Level lvl);


      // provide CTP and L1 version to be accessible by all the L1 loaders
      void setCTPVersion(unsigned int v) { m_ctpVersion = v; }
      void setL1Version(unsigned int v) { m_l1Version = v; }
      unsigned int ctpVersion() const { return m_ctpVersion; }
      unsigned int l1Version() const { return m_l1Version; }


      /**creates and returns a new database session manager*/
      SessionMgr& sessionMgr();
    
      // Mandatory interface implementation
      virtual IL1TopoMenuLoader&               l1topoMenuLoader();
      virtual IMasterTableLoader&              masterTableLoader();
      virtual IMenuLoader&                     menuLoader();
      virtual IMuctpiLoader&                   muctpiLoader();
      virtual IDeadTimeLoader&                 deadTimeLoader();
      virtual IPrescaledClockLoader&           prescaledClockLoader();
      virtual IRandomLoader&                   randomLoader();
      virtual IThresholdConfigLoader&          thresholdConfigLoader();
      virtual ITriggerThresholdLoader&         triggerThresholdLoader();
      virtual ITriggerThresholdValueLoader&    triggerThresholdValueLoader();
      virtual IThresholdMonitorLoader&         thresholdMonitorLoader();
      virtual ICTPFilesLoader&                 ctpFilesLoader();
      virtual ICaloJetInputLoader&             caloJetInputLoader();
      virtual ICaloSinCosLoader&               caloSinCosLoader();
      virtual ICaloInfoLoader&                 caloInfoLoader();
      virtual ILutCamLoader&                   lutcamLoader();
      virtual IPrescaleSetLoader&              prescaleSetLoader();
      virtual IPrioritySetLoader&              prioritySetLoader();
      virtual IBunchGroupLoader&               bunchGroupLoader();
      virtual IBunchGroupSetLoader&            bunchGroupSetLoader();
      virtual ICTPConfigLoader&                ctpConfigLoader();
      virtual ICTPConfigLoader&                ctpConfigOnlineLoader();
      virtual IHLTFrameLoader&                 hltFrameLoader();
      virtual IJobOptionTableLoader&           jobOptionTableLoader();

      // These loaders are DB-only no XML implementation
      IHLTPrescaleSetLoader&                   hltPrescaleSetLoader();
      IHLTPrescaleSetCollectionLoader&         hltPrescaleSetCollectionLoader();
      IMuonThresholdSetLoader&                 muonThresholdSetLoader();

      void closeSession();

      void setUseFrontier(bool useFrontier) { m_useFrontier = useFrontier; } 
      bool useFrontier() const { return m_useFrontier; } 

      void setRetrialPeriod(int retrialPeriod) { m_retrialPeriod = retrialPeriod; }
      void setRetrialTimeout(int retrialTimeout) { m_retrialTimeout = retrialTimeout; }
      void setConnectionTimeout(int connectionTimeout) { m_connectionTimeout = connectionTimeout; }

      int retrialPeriod() const { return m_retrialPeriod; }
      int retrialTimeout() const { return m_retrialTimeout; }
      int connectionTimeout() const { return m_connectionTimeout; }

   private:

      template< class L >
      L * NewLoader() {
         return new L( *this, this->sessionMgr().createSession() );
      }

      IHLTPrescaleSetLoader *            m_hltPrescaleSetLoader { nullptr };
      IHLTPrescaleSetCollectionLoader *  m_hltPrescaleSetCollectionLoader { nullptr };

      SessionMgr*     m_sessionMgr { nullptr };
      std::string     m_cs {""};      ///< connection string
      std::string     m_dbtype {""};  ///< db type
      std::string     m_dbserver {""};///< db server
      std::string     m_dbname {""};  ///< db name
      std::string     m_user {""};    ///< user name        
      std::string     m_password {""};///< password         
      bool            m_useFrontier {false}; ///< using frontier to connect to oracle (if set, disables sqlite) 

      int             m_retrialPeriod {0};
      int             m_retrialTimeout {0};
      int             m_connectionTimeout {0};

   };
}

#endif
