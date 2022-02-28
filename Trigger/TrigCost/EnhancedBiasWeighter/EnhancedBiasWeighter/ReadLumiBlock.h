/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIG_ReadLumiBlock_H
#define TRIG_ReadLumiBlock_H

//
// Connect to COOL and get LB lengths for a run
//

// COOL
#include "CoolApplication/Application.h"
#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"

#include "GaudiKernel/MsgStream.h"

// C/C++
#include <map>
#include <string>

/**
 * @brief Helper class to read in lumi block length info from COOL for a run
 */
class ReadLumiBlock : public cool::Application {
public:

  /**
   * @brief Construct helper class for reading in lumi block length from COOL
   */
  ReadLumiBlock();
  ~ReadLumiBlock();
  
  bool updateLumiBlocks(uint32_t run, MsgStream& msg); //!< Load information for @param run
  float getLumiBlockLength(uint32_t lb, MsgStream& msg) const; //!< @return Length of lb 

  void setConnectionCOOL(const std::string &id) { m_cool_id = id; } //!< Set which COOL instance to connect to (run1, run2)
  bool getTriedSetup() { return m_triedSetup; } //!< True when a connection attempt has been made
  
  cool::DatabaseId&   cool_id()  { return m_cool_id; } //!< Get reference to COOL database ID
  cool::IDatabasePtr& cool_ptr() { return m_cool_ptr; } //!< Get reference to COOL database ptr
  
  const std::string& cool_source() const { return m_cool_source; } //!< Get name of COOL data source
  
  uint32_t getRun() const { return m_run; } //!< Get run number
  
private:
  
  bool dbIsOpen(); //!< Check if DB connection is open
  void closeDb(MsgStream& msg); //!< Close DB connection
  bool openDb(bool readOnly, MsgStream& msg); //!< Open a DB connection

private:

  std::map<uint32_t, uint64_t> m_lbLength;       // Hold the runs LB lengths

  cool::DatabaseId   m_cool_id;        // COOL connection id
  cool::DatabaseId   m_cool_id_run1;   // COOL connection id for run 1 data
  cool::DatabaseId   m_cool_id_run2;   // COOL connection id for run 2 data
  cool::IDatabasePtr m_cool_ptr;       // COOL database pointer
  
  std::string m_cool_source;    // COOL configuration source
  uint32_t    m_run;            // Run number
  bool        m_triedSetup;     // If we have tried to get a DB connection
};

#endif
