/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_READCONF_H
#define TRIG_READCONF_H

//
// Algorithm for predicting rates
//  - requires two instances of MakeRates algorithms
//  - one instance for rates with initial  PS values
//  - one instance for rates with modified PS values
//

// C/C++
#include <map>
#include <string>

// COOL
#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"

namespace Trig
{
  class ConfigKeys {
  public:

    ConfigKeys();
    ~ConfigKeys() {}
    
    unsigned getSMK() const { return m_smk; }
    unsigned getBGK() const { return m_bgk; }

    unsigned getHLT_PS() const { return m_hlt_ps; }
    unsigned getLV1_PS() const { return m_lv1_ps; }
    
    bool equalKeys(const ConfigKeys &rhs) const;
    
    bool isValid() const;

    void setSMK(unsigned smk) { m_smk = smk; m_smk_set = true; }
    void setBGK(unsigned bgk) { m_bgk = bgk; m_bgk_set = true; }

    void setHLT_PS(unsigned psk) { m_hlt_ps = psk; m_hlt_ps_set = true; }
    void setLV1_PS(unsigned psk) { m_lv1_ps = psk; m_lv1_ps_set = true; }
    
    void print(std::ostream &os) const;    

  private:

    unsigned         m_smk;
    unsigned         m_bgk;
    unsigned         m_hlt_ps;
    unsigned         m_lv1_ps;

    bool             m_smk_set;
    bool             m_bgk_set;
    bool             m_hlt_ps_set;
    bool             m_lv1_ps_set;
  };

  //
  // Comparison operators
  //
  bool operator <(const ConfigKeys &lhs, const ConfigKeys &rhs);
  bool operator==(const ConfigKeys &lhs, const ConfigKeys &rhs);

  class ReadConf {
  public:

    ReadConf();
    ~ReadConf();
    
    void updateConfigKeys(unsigned run);

    void setConnectionCOOL(const std::string &id) { m_cool_id = id; }
    
    const std::map<unsigned, ConfigKeys>& getAllKeys() const { return m_keys; }
    const std::map<unsigned, ConfigKeys>  getNewKeys() const;
    
    std::string debug() const { return m_debug.str(); } 
    std::string infos() const { return m_infos.str(); } 
    
    cool::DatabaseId&   cool_id()  { return m_cool_id; }
    cool::IDatabasePtr& cool_ptr() { return m_cool_ptr; }
    
    const std::string& cool_source() const { return m_cool_source; }
    
    unsigned getRun() const { return m_run; }
    
    void print(std::ostream &os) const;

  private:
    
    cool::IDatabaseSvc& databaseService();

    bool dbIsOpen();
    void closeDb();

    bool openDb(bool readOnly = true);

  private:

    typedef std::map<unsigned, ConfigKeys> KeyMap;

  private:

    KeyMap                      m_keys;           // Configuration keys for this run
    cool::DatabaseId            m_cool_id;        // COOL connection id
    cool::IDatabasePtr          m_cool_ptr;       // COOL database pointer
    
    std::string                 m_cool_source;    // COOL configuration source
    unsigned                    m_run;            // Run number

    std::stringstream           m_debug;          // debug messages
    std::stringstream           m_infos;          // info messages
  };
}

#endif
