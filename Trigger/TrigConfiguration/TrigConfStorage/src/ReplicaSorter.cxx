/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ReplicaSorter.cxx
// Richard Hawkings, 26/11/07

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <climits>
#include <unistd.h>
#include "RelationalAccess/IDatabaseServiceDescription.h"
#include "ReplicaSorter.h"

using namespace std;

TrigConf::ReplicaSorter::ReplicaSorter() :
   m_frontiergen(false) 
{
   std::cout << "ReplicaSorter constructor" << std::endl;
   readConfig();
}

void
TrigConf::ReplicaSorter::sort(std::vector<const coral::IDatabaseServiceDescription*>& replicaSet) {
   // loop through all the offered replicas
   std::map<int,const coral::IDatabaseServiceDescription*> primap;
   for (std::vector<const coral::IDatabaseServiceDescription*>::const_iterator 
           itr=replicaSet.begin();itr!=replicaSet.end();++itr) {
      const std::string conn=(**itr).connectionString();
      // do not use SQLite files
      if (conn.find("sqlite_file")==std::string::npos) {
         // extract the server name (assuming URLs "techno://server/schema")
         // example of current conn naming scheme:  coral://127.0.0.1:3320/&oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_REPR
         std::string::size_type ipos0=conn.find('&');
         std::string::size_type ipos1=conn.find("://",ipos0+1);
         std::string::size_type ipos2=conn.find('/',ipos1+3);
         if (ipos1!=std::string::npos && ipos2!=std::string::npos) {
            const std::string server=conn.substr(ipos1+3,ipos2-ipos1-3);
            // check if this server is on list of replicas to use for domain
            // if so, add it with its associated priority
            for (ServerMap::const_iterator sitr=m_servermap.begin();
                 sitr!=m_servermap.end();++sitr) {
               if (sitr->first==server) 
                  primap[sitr->second]=*itr;
            }
         }
      }
   }
   // now create sorted list
   replicaSet.clear();
   for (std::map<int,const coral::IDatabaseServiceDescription*>::const_iterator 
           itr=primap.begin();itr!=primap.end();++itr) {
      replicaSet.push_back(itr->second);
      std::cout << "Allowed replica to try (priority " << itr->first 
                << ") : " << (itr->second)->connectionString() << std::endl;
   }
   if (replicaSet.empty())
      std::cout << "No matching replicas found" << std::endl;
}

bool
TrigConf::ReplicaSorter::readConfig() {
   // determine the hostname from ATLAS_CONDDB, HOSTNAME or hostname -fqdn
   m_hostname="";
   const char* chost=getenv("ATLAS_CONDDB");
   if (chost) m_hostname=chost;
   if (m_hostname.empty()) {
      const char* chost=getenv("HOSTNAME");
      if (chost) m_hostname=chost;
      // check if the returned host has a .
      if (m_hostname.find('.')==std::string::npos) {
         m_hostname="unknown";
         char cstr_host[HOST_NAME_MAX];
         if (gethostname(cstr_host, sizeof(cstr_host))==0) {
            m_hostname=std::string(cstr_host);
         }
      }
   }
   std::cout << "Using machine hostname " << m_hostname << " for DB replica resolution" << std::endl;
   // check if FRONTIER_SERVER is set, if so, allow generic replicas
   const char* cfrontier=getenv("FRONTIER_SERVER");
   if (cfrontier && strcmp(cfrontier,"")!=0) {
      std::cout << "Frontier server at " << cfrontier << " will be considered"
                << std::endl;
      m_frontiergen=true;
   }
 
   // try to locate configuration file using pathresolver
   FILE* p_inp=0;
   const char* datapath=getenv("DATAPATH");
   if (datapath!=0) p_inp = findFile("dbreplica.config",datapath);
   if (p_inp==0) {
      std::cout << "Cannot open/locate configuration file dbreplica.config" 
                << std::endl;
      return false;
   }
   // buffer for reading line
   unsigned int bufsize=999;
   char* p_buf=new char[bufsize];
   while (!feof(p_inp)) {
      char* p_line=fgets(p_buf,bufsize,p_inp);
      if (p_line!=NULL && p_line[0]!='#') {
         std::string buf=std::string(p_line);
         std::string::size_type iofs1=0;
         // analyse based on spaces as seperator
         bool sequal=false;
         std::vector<std::string> domains;
         std::vector<std::string> servers;
         bool atCERN = false;
         while (iofs1<buf.size()) {
            std::string::size_type iofs2=buf.find(' ',iofs1);
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
                  if(token=="cern.ch") atCERN = true;
               } else {
                  // token is a server name
                  // only add Frontier ATLF server if FRONTIER_CLIENT set
                  if(atCERN && token=="ATONR_CONF") atCERN = false;
                  if (token!="ATLF" || m_frontiergen) servers.push_back(token);
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
                  atCERN=true;
                  useit=true;
                  bestlen=len;
               }
            }
            // for 'default' domain name, add the servers as a last resort
            // if nothing has been found so far
            if ("default"==*itr && m_servermap.empty()) {
               std::cout <<
                  "No specific match for domain found - use default fallback"
                         << std::endl;
               useit=true;
               bestlen=0;
            }
         }
         if (useit) {
            if(atCERN) {
               servers.push_back("ATONR_COOL");
               servers.push_back("ATONR_CONF");
            }
            // assign these servers, priority based on position in list
            // and length of match of domain name
            for (unsigned int i=0;i<servers.size();++i) {
               int priority=i-100*bestlen;
               m_servermap.push_back(ServerPair(servers[i],priority));
            }
         }
      }
   }
   fclose(p_inp);
   delete [] p_buf;
   std::cout << "Total of " << m_servermap.size() << 
      " servers found for host " << m_hostname << std::endl;
   return true;
}



FILE*
TrigConf::ReplicaSorter::findFile(const std::string filename,
                                  const std::string pathvar) 
{
   // behave like pathresolver
   std::string::size_type iofs1,iofs2,len;
   FILE* fptr=0;
   iofs1=0;
   len=pathvar.size();
   std::string name;
   while (!fptr && iofs1<len) {
      iofs2=pathvar.find(':',iofs1);
      if (iofs2==std::string::npos) iofs2=len;
      name=pathvar.substr(iofs1,iofs2-iofs1)+"/"+filename;
      fptr=fopen(name.c_str(),"r");
      iofs1=iofs2+1;
   }
   std::cout << "Opening " << name << endl;
   return fptr;
}
