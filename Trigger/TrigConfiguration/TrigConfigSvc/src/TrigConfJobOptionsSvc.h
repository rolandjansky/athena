/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_JobOptionsSvc
#define TrigConfigSvc_JobOptionsSvc

#include <string>
#include <vector>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthService.h"
#include "TrigConfInterfaces/IJobOptionsSvc.h"

namespace TrigConf {

  /**
   * JobOptionsSvc for the HLT
   *
   * This implementation of IJobOptionSvc wraps the regular JobOptionSvc
   * and forwards most of the calls directly to it. Beyond this it provides
   * the ability to read the job configuration from JSON (file or DB) and
   * create a JSON dump of all configured properties when running from Python.
   *
   * The job property source is specified by the `TYPE` property:
   *  - `NONE`: Default mode running from Python
   *  - `FILE`: Run from JSON file (specified in `PATH`)
   *  - `DB`:   Read properties from DB, connection string in `PATH`, see \ref parseDBString
   *
   */
  class JobOptionsSvc : public extends<AthService, TrigConf::IJobOptionsSvc, ::IJobOptionsSvc> {
    using AthService::getProperties;

  public:
    JobOptionsSvc(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode start() override;

    /// @name IJobOptionsSvc interface
    /// Most interfaces are just forwards to the "real" JobOptionsSvc.
    ///@{
    virtual StatusCode setMyProperties(const std::string& client, IProperty* me) override
    {
      return m_josvc->setMyProperties(client, me);
    }

    virtual StatusCode addPropertyToCatalogue(const std::string& client,
                                              const Gaudi::Details::PropertyBase& property) override
    {
      return m_josvc->addPropertyToCatalogue(client, property);
    }

    virtual StatusCode removePropertyFromCatalogue(const std::string& client,
                                                   const std::string& name) override
    {
      return m_josvc->removePropertyFromCatalogue(client, name);
    }

    virtual const std::vector<const Gaudi::Details::PropertyBase*>*
    getProperties(const std::string& client) const override
    {
      return m_josvc->getProperties(client);
    }

    virtual const Property* getClientProperty(const std::string& client,
                                              const std::string& name) const override
    {
      return m_josvc->getClientProperty(client, name);
    }

    virtual std::vector<std::string> getClients() const override { return m_josvc->getClients(); }
    virtual StatusCode readOptions(const std::string& file, const std::string& path = "") override;

    StatusCode readOptionsDB(const std::string& db_server, int smk);
    ///@}

    /// @name TrigConf::IJobOptionsSvc interface
    /// Configuration keys as provided during the job initialization
    virtual const std::string& server() const override { return m_server; } ///< DB connection alias
    virtual int superMasterKey() const override { return m_smk; }           ///< Return SMK (-1 if not set)
    virtual int l1PrescaleKey() const override { return m_l1psk; }          ///< Return L1PSK (-1 if not set)
    virtual int hltPrescaleKey() const override { return m_hltpsk; }        ///< Return HLTPSK (-1 if not set)
    ///@}

  private:
    StatusCode dumpOptions(const std::string& file);
    void parseDBString(const std::string& s);

    int m_smk{-1};        ///< SuperMasterKey
    int m_l1psk{-1};      ///< L1 prescale key
    int m_hltpsk{-1};     ///< HLT prescale key
    std::string m_server; ///< DB connection alias

    // Same properties as in Gaudi's JobOptionsSvc
    Gaudi::Property<std::string> m_sourceType{this, "TYPE", "NONE", "Configuration type (NONE, FILE, DB)"};
    Gaudi::Property<std::string> m_sourcePath{this, "PATH", {}, "Path for NONE/FILE, connection string for DB"};
    Gaudi::Property<std::string> m_searchPath{this, "SEARCHPATH", {}, "NOT SUPPORTED"};
    Gaudi::Property<std::string> m_dump{this, "DUMPFILE", {}, "Dump job properties into JSON file"};

    /// handle to the "real" IJobOptionsSvc
    ServiceHandle<::IJobOptionsSvc> m_josvc;
  };

} // namespace TrigConf

#endif
