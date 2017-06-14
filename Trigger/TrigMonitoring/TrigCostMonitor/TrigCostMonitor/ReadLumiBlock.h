/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ReadLumiBlock_H
#define TRIG_ReadLumiBlock_H

//
// Connect to COOL and get LB lengths for a run
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

  class ReadLumiBlock {
  public:

    ReadLumiBlock();
    ~ReadLumiBlock();
    
    void updateLumiBlocks(unsigned run);
    float getLumiBlockLength(unsigned lb);

    void setConnectionCOOL(const std::string &id) { m_cool_id = id; }
    bool getTriedSetup() { return m_triedSetup; }
    
    //const std::map<unsigned, ConfigKeys>& getAllKeys() const { return m_keys; }
    //const std::map<unsigned, ConfigKeys>  getNewKeys() const;
    
    std::string debug() const { return m_debug.str(); } 
    std::string infos() const { return m_infos.str(); } 
    void clearMsg() { m_debug.str(""); m_debug.clear(); m_infos.str(""); m_infos.clear(); }
    
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

    //typedef std::map<unsigned, ConfigKeys> KeyMap;   

  private:


  	std::map<unsigned, uint64_t> m_lbLength;       // Hold the runs LB lengths

    cool::DatabaseId            m_cool_id;        // COOL connection id
    cool::DatabaseId            m_cool_id_run1;   // COOL connection id for run 1 data
    cool::DatabaseId            m_cool_id_run2;   // COOL connection id for run 2 data
    cool::IDatabasePtr          m_cool_ptr;       // COOL database pointer
    
    std::string                 m_cool_source;    // COOL configuration source
    unsigned                    m_run;            // Run number

    std::stringstream           m_debug;          // debug messages
    std::stringstream           m_infos;          // info messages

    bool                        m_triedSetup;     // If we have tried to get a DB connection
  };
}

#endif
