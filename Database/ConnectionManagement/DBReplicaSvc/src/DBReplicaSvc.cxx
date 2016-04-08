/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DBReplicaSvc.cxx - service implementing CORAL IReplicaSortingAlgorithm
// Richard Hawkings, started 24/4/07

#include <fstream>
#include <cstring>
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "PathResolver/PathResolver.h"
#include "DBReplicaSvc.h"

DBReplicaSvc::DBReplicaSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  m_configfile("dbreplica.config"),
  m_testhost(""),
  m_coolsqlitepattern(""),
  m_usecoolsqlite(true),
  m_usecoolfrontier(true),
  m_usegeomsqlite(true),
  m_nofailover(false),
  m_frontiergen(false)
{
  declareProperty("ConfigFile",m_configfile);
  declareProperty("TestHost",m_testhost);
  declareProperty("COOLSQLiteVetoPattern",m_coolsqlitepattern);
  declareProperty("UseCOOLSQLite",m_usecoolsqlite);
  declareProperty("UseCOOLFrontier",m_usecoolfrontier);
  declareProperty("UseGeomSQLite",m_usegeomsqlite);
  declareProperty("DisableFailover",m_nofailover);
}

DBReplicaSvc::~DBReplicaSvc() {}

const InterfaceID& DBReplicaSvc::type() const
{ 
return IDBReplicaSvc::interfaceID();
}

StatusCode DBReplicaSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (IDBReplicaSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(IDBReplicaSvc*)this;
  } else {
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode DBReplicaSvc::initialize() {
  // service initialisation
  if (StatusCode::SUCCESS!=AthService::initialize()) return StatusCode::FAILURE;
  // determine the hostname (or override if from joboption)
  m_hostname=m_testhost;
  // if nothing set on job-options, try environment variable ATLAS_CONDDB
  if (m_hostname.empty()) {
    const char* chost=getenv("ATLAS_CONDDB");
    if (chost) m_hostname=chost;
  }
  // if ATLAS_CONDDB not set, try environment variable HOSTNAME
  if (m_hostname.empty()) {
    const char* chost=getenv("HOSTNAME");
    if (chost) m_hostname=chost;
    // check if the returned host has a . 
    if (m_hostname.find(".")==std::string::npos) {
      ATH_MSG_DEBUG("HOSTNAME " << m_hostname
                    << " has no domain - try hostname --fqdn");
      m_hostname="unknown";
#ifndef __APPLE__
      system("hostname --fqdn > hostnamelookup.tmp");
#else
      // Unfortunately Leopard doesn't understand the -f option to hostname, only Snow Leopard does
      system("hostname > hostnamelookup.tmp");
#endif
      std::ifstream infile;
      infile.open("hostnamelookup.tmp");
      if (infile) { 
        infile >> m_hostname; 
        ATH_MSG_DEBUG ("HOSTNAME from fqdn: " << m_hostname);
      } else {
	m_hostname="unknown";
      }
    }
  }
  // check if FRONTIER_SERVER is set, if so, allow generic replicas
  const char* cfrontier=getenv("FRONTIER_SERVER");
  if (m_usecoolfrontier && cfrontier && strcmp(cfrontier,"")!=0) {
    ATH_MSG_INFO ("Frontier server at " << cfrontier
                  << " will be considered for COOL data");
    m_frontiergen=true;
  }
  StatusCode sc=readConfig();
  if (!m_usecoolsqlite) {
    ATH_MSG_INFO ("COOL SQLite replicas will be excluded");
  } else if (m_coolsqlitepattern!="") {
    ATH_MSG_INFO ("COOL SQLite replicas will be excluded if matching pattern "
                  << m_coolsqlitepattern);
  }
  if (!m_usecoolfrontier)
    ATH_MSG_INFO ("COOL Frontier replicas will be excluded");
  if (!m_usegeomsqlite)
    ATH_MSG_INFO ("Geometry SQLite replicas will be excluded");
  if (m_nofailover) 
    ATH_MSG_INFO ("Failover to secondary replicas disabled");
  return sc;
}

StatusCode DBReplicaSvc::finalize() {
  return StatusCode::SUCCESS;
}

StatusCode DBReplicaSvc::readConfig() {


  // try to locate the file using pathresolver
  std::string file=PathResolver::find_file(m_configfile,"DATAPATH");
  if (file.empty()) {
    ATH_MSG_ERROR ("Cannot locate configuration file " << m_configfile);
    return StatusCode::FAILURE;
  }
  // open and read the file
  ATH_MSG_INFO ("Read replica configuration from " << file);
  FILE* p_inp=fopen(file.c_str(),"r");
  if (p_inp==0) {
    ATH_MSG_ERROR ("Cannot open configuration file");
    return StatusCode::FAILURE;
  }
  // buffer for reading line
  const unsigned int bufsize=999;
  char p_buf[bufsize];
  while (!feof(p_inp)) {
    char* p_line=fgets(p_buf,bufsize,p_inp);
    if (p_line!=NULL && p_line[0]!='#') {
      std::string buf=std::string(p_line);
      std::string::size_type iofs1=0;
      // analyse based on spaces as seperator
      bool sequal=false;
      std::vector<std::string> domains;
      std::vector<std::string> servers;
      while (iofs1<buf.size()) {
        std::string::size_type iofs2=buf.find(" ",iofs1);
        // allow for trailing linefeed
        if (iofs2==std::string::npos) iofs2=buf.size()-1;
        std::string token=buf.substr(iofs1,iofs2-iofs1);
	// skip empty or space tokens
	if (token!="" && token!=" ") {
          if (token=="=") {
  	    sequal=true;
          } else if (!sequal) {
 	    // token is a domain name
	    domains.push_back(token);
          } else {
  	    // token is a server name
    	    if (!m_nofailover || servers.size()==0 || token=="atlas_dd") 
	      servers.push_back(token);
          }
	}
        iofs1=iofs2+1;
      }
      // check the list of domains against the hostname to see if this
      // set of servers is appropriate
      bool useit=false;
      unsigned int bestlen=0;
      for (std::vector<std::string>::const_iterator itr=domains.begin();
	   itr!=domains.end();++itr) {
        std::string::size_type len=(itr->size());
	std::string::size_type hlen=m_hostname.size();
        if (hlen>=len && *itr==m_hostname.substr(hlen-len,len)) {
	  if (len>bestlen) {
	    useit=true;
 	    bestlen=len;
	  }
	}
	// for 'default' domain name, add the servers as a last resort
	// if nothing has been found so far
	if ("default"==*itr && m_servermap.empty()) {
	  ATH_MSG_INFO ("No specific match for domain found - use default fallback");
	  useit=true;
	  bestlen=0;
	}
      }
      if (useit) {
	// assign these servers, priority based on position in list
	// and length of match of domain name
	for (unsigned int i=0;i<servers.size();++i) {
	  int priority=i*5-100*bestlen;
	  // only add ATLF Frontier generic servers if allowed
	  if (servers[i]!="ATLF" || m_frontiergen) {
	    // give generic Frontier server higher (more negative) priority
	    // so it will be preferred over  DBRelaese SQLite file
	    if (servers[i]=="ATLF") priority-=2000;
	    m_servermap.push_back(ServerPair(servers[i],priority));
	    ATH_MSG_DEBUG ("Candidate server " << servers[i] <<
                           " (priority " << priority << ")");
	  }
	}
      }
    }
  }
  fclose(p_inp);
  msg() << MSG::INFO << "Total of " << m_servermap.size() << 
    " servers found for host " << m_hostname << " [";
  for (ServerMap::const_iterator itr=m_servermap.begin();
       itr!=m_servermap.end();++itr) msg() <<  itr->first << " ";
  msg() << "]" << endreq;
  return StatusCode::SUCCESS;
}

void DBReplicaSvc::sort(std::vector<const 
			 coral::IDatabaseServiceDescription*>& replicaSet) {
  // if only one replica offered, return immediately
  // this helps for online, where explicit configuration file is given
  // that does not match any of the standard dbreplica.config entries
  if (replicaSet.size()<=1) return;

  // loop through all the offered replicas
  std::map<int,const coral::IDatabaseServiceDescription*> primap;
  for (std::vector<const coral::IDatabaseServiceDescription*>::const_iterator 
	 itr=replicaSet.begin();itr!=replicaSet.end();++itr) {
    const std::string conn=(**itr).connectionString();
    ATH_MSG_DEBUG ("Replica connection string: " << conn);
    if (conn.find("sqlite_file")!=std::string::npos) {
      // include SQLite files unless they are vetoed
      // COOL SQLIte files recognised by ALLP in connection string
      // vetoed if use-SQlite flag not set, or pattern is found in 
      // SQLite filename
      // Geometry SQLite files recognised by geomDB in connection string
      if (!( ((m_usecoolsqlite==false || 
	     (m_coolsqlitepattern!="" && 
	      conn.find(m_coolsqlitepattern)!=std::string::npos))
	      && conn.find("ALLP")!=std::string::npos)
          || ((m_usegeomsqlite==false || 
             (m_coolsqlitepattern!="" &&
              conn.find(m_coolsqlitepattern)!=std::string::npos))
               && conn.find("geomDB")!=std::string::npos))) {
	// local sqlite files get -9999, DB release ones 
	// (identified with path starting / or containing DBRelease)
	// get -999, so local one will be tried first if present
	if (conn.find("sqlite_file:/")!=std::string::npos ||
	    conn.find("DBRelease")!=std::string::npos) {
          primap[-999]=*itr;
	} else {
          primap[-9999]=*itr;
	}
      }
    } else {
      // define priority for technologies with this server (lower = better)
      bool veto=false;
      int spri=5; // default for Oracle
      if (conn.find("frontier:")!=std::string::npos) {
	spri=3; // use frontier before oracle
	// dont use frontier servers if disabled, or generic Frontier server
	// is specified (via '()' in server definition) and FRONTIER_SERVER
	// env variable is not set
	if (!m_usecoolfrontier || 
	    (conn.find("()")!=std::string::npos && m_frontiergen==false))
	  veto=true;
      }
      // extract the server name (assuming URLs "techno://server/schema")
      std::string::size_type ipos1=conn.find("://");
      std::string::size_type ipos2=conn.find("/",ipos1+3);
      // for Frontier, have to remove the (..) part after the server name
      // e.g. frontier://ATLAS_COOLPROD/(serverurl=http://xyzfrontier.cern.ch:8000/atlr)/schema
      std::string::size_type ipos3=conn.find("(",ipos1+3);
      if (ipos3!=std::string::npos && ipos3<ipos2) ipos2=ipos3;
      if (ipos1!=std::string::npos && ipos2!=std::string::npos && !veto) {
        const std::string server=conn.substr(ipos1+3,ipos2-ipos1-3);
        // check if this server is on list of replicas to use for domain
        // if so, add it with its associated priority
        for (ServerMap::const_iterator sitr=m_servermap.begin();
	     sitr!=m_servermap.end();++sitr) {
          if (sitr->first==server) 
            primap[sitr->second+spri]=*itr;
	}
      }
    }
  }
  // now create sorted list
  replicaSet.clear();
  for (std::map<int,const coral::IDatabaseServiceDescription*>::const_iterator 
	   itr=primap.begin();itr!=primap.end();++itr) {
    replicaSet.push_back(itr->second);
    ATH_MSG_DEBUG ("Allowed replica to try (priority " << itr->first <<
                   ") : " << (itr->second)->connectionString());
  }
  if (replicaSet.empty())
    ATH_MSG_ERROR ("No matching replicas found");
  ATH_MSG_DEBUG ("Retained total of " << replicaSet.size() << " replicas");
}
