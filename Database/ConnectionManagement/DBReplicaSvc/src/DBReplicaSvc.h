/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DBREPLICASVC_DBREPLICASVC_H
#define DBREPLICASVC_DBREPLICASVC_H
// DBReplicaSvc.h - concrete implementation of service implementating
// CORAL IReplicaSortingAlgorithm
// Richard Hawkings, started 24/4/07

#include<string>
#include "AthenaBaseComps/AthService.h"
#include "DBReplicaSvc/IDBReplicaSvc.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"

class DBReplicaSvc : public virtual IDBReplicaSvc, public virtual AthService
{
  template <class TYPE> class SvcFactory;
 public:
  DBReplicaSvc(const std::string& name, ISvcLocator* svc);
  virtual ~DBReplicaSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);

  virtual const InterfaceID& type() const;

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  void sort(std::vector<const coral::IDatabaseServiceDescription*>& 
	    replicaSet);

 private:
  StatusCode readConfig();
  std::string m_configfile;
  std::string m_testhost;
  std::string m_coolsqlitepattern;
  bool m_usecoolsqlite;
  bool m_usecoolfrontier;
  bool m_usegeomsqlite;
  bool m_nofailover;

  bool m_frontiergen;
  std::string m_hostname;
  typedef std::pair<std::string,int> ServerPair;
  typedef std::vector< ServerPair > ServerMap;
  ServerMap m_servermap;
};

#endif // DBREPLICASVC_DBREPLICASVC_H
