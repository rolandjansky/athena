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

#include "GaudiKernel/MsgStream.h"

class ReadLumiBlock {
public:

  ReadLumiBlock();
  ~ReadLumiBlock();
  
  bool updateLumiBlocks(uint32_t run, MsgStream& msg);
  float getLumiBlockLength(uint32_t _lb, MsgStream& msg) const;

  void setConnectionCOOL(const std::string &id) { m_cool_id = id; }
  bool getTriedSetup() { return m_triedSetup; }
  
  cool::DatabaseId&   cool_id()  { return m_cool_id; }
  cool::IDatabasePtr& cool_ptr() { return m_cool_ptr; }
  
  const std::string& cool_source() const { return m_cool_source; }
  
  uint32_t getRun() const { return m_run; }
  
private:
  
  cool::IDatabaseSvc& databaseService();

  bool dbIsOpen();
  void closeDb(MsgStream& msg);

  bool openDb(bool readOnly, MsgStream& msg);

private:

  std::map<uint32_t, uint64_t> m_lbLength;       // Hold the runs LB lengths

  cool::DatabaseId            m_cool_id;        // COOL connection id
  cool::DatabaseId            m_cool_id_run1;   // COOL connection id for run 1 data
  cool::DatabaseId            m_cool_id_run2;   // COOL connection id for run 2 data
  cool::IDatabasePtr          m_cool_ptr;       // COOL database pointer
  
  std::string                 m_cool_source;    // COOL configuration source
  uint32_t                    m_run;            // Run number

  bool                        m_triedSetup;     // If we have tried to get a DB connection
};

#endif
