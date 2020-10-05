/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConfigSvc_JobOptionsSvc
#define TrigConfigSvc_JobOptionsSvc

#include <string>
#include <vector>

#include "GaudiKernel/IJobOptionsSvc.h"
#include "Gaudi/Interfaces/IOptionsSvc.h"
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
  class JobOptionsSvc : public extends<AthService, TrigConf::IJobOptionsSvc, ::IJobOptionsSvc, Gaudi::Interfaces::IOptionsSvc> {
    using AthService::getProperties;

  public:
    JobOptionsSvc(const std::string& name, ISvcLocator* pSvcLocator);

    virtual StatusCode initialize() override;
    virtual StatusCode start() override;

    /// @name IOptionsSvc interface
    ///@{
    virtual void set( const std::string& key, const std::string& value ) override
    {
      return m_optsvc->set(key,value);
    }

    virtual std::string get( const std::string& key, const std::string& default_ = {} ) const override
    {
      return m_optsvc->get(key,default_);
    }

    virtual std::string pop( const std::string& key, const std::string& default_ = {} ) override
    {
      return m_optsvc->pop(key,default_);
    }
    
    virtual bool has( const std::string& key ) const override
    {
      return m_optsvc->has(key);
    }
    
    virtual bool isSet( const std::string& key ) const override
    {
      return m_optsvc->isSet(key);
    }

    virtual std::vector<std::tuple<std::string, std::string>> items() const override
    {
      return m_optsvc->items();
    }

    virtual void bind( const std::string& prefix, Gaudi::Details::PropertyBase* property ) override
    {
      return m_optsvc->bind(prefix,property);
    }

    using OnlyDefaults = Gaudi::Interfaces::IOptionsSvc::OnlyDefaults;
    virtual void broadcast( const std::regex& filter, const std::string& value,
                            OnlyDefaults defaults = OnlyDefaults{true} ) override
    {
      return m_optsvc->broadcast(filter, value, defaults);
    }
    ///@}

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

    virtual const Gaudi::Details::PropertyBase* getClientProperty(const std::string& client,
                                              const std::string& name) const override
    {
      return m_josvc->getClientProperty(client, name);
    }

    virtual std::vector<std::string> getClients() const override { return m_josvc->getClients(); }
    virtual StatusCode readOptions(const std::string&, const std::string&) override
    {
      throw std::runtime_error("TrigConf::JobOptionsSvc::readOptions() is deprecated");
    }
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
    StatusCode readOptionsJson(const std::string& file);
    StatusCode readOptionsDB(const std::string& db_server, int smk);
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

    /// handle to the "real" IOptionsSvc
    ServiceHandle<::IJobOptionsSvc> m_josvc;
    ServiceHandle<Gaudi::Interfaces::IOptionsSvc> m_optsvc;
  };

} // namespace TrigConf

#endif
