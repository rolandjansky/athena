/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_ASCIICONDDBSVC_H
#define ATHEXHIVE_ASCIICONDDBSVC_H 1

#include "GaudiKernel/Service.h"
#include "GaudiKernel/EventIDBase.h"
#include "GaudiKernel/EventIDRange.h"
#include "AthenaBaseComps/AthService.h"
#include "AthenaBaseComps/AthService.h"

#include "AthExHive/IASCIICondDbSvc.h"

#include <map>
#include <set>
#include <vector>
#include <mutex>

class ASCIICondDbSvc: public extends1<AthService, IASCIICondDbSvc> {
public:

  ASCIICondDbSvc(const std::string& name, ISvcLocator* svc);
  ~ASCIICondDbSvc();

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  // from IASCIICondDbSvc
public:

  virtual StatusCode getRange(const std::string&, const EventContext&, EventIDRange&, 
                              IASCIICondDbSvc::dbData_t&) const;

  virtual void dump() const;
  virtual void dump(std::ostringstream&) const;

private:

  bool parse(EventIDRange& t, const std::string& s);
  bool parse(IOVEntryT<IASCIICondDbSvc::dbData_t>& t, const std::string& s);

  StatusCode readDbFile(const std::string&);

  Gaudi::Property<std::string> m_file{this, "CondFile", "", "cond db file name"};
  Gaudi::Property<unsigned int> m_lbn{this, "LBN", 1, "explicit lumi block number"};

  typedef std::map<std::string, std::vector<IOVEntryT<IASCIICondDbSvc::dbData_t>>> registry_t;
  registry_t m_registry;

  mutable std::mutex m_lock;

};

#endif
