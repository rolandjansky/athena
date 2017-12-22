/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// AtlCoolCopy.cxx
// COOL copying utility, C++ based on PyCoolCopy from Sven Schmidt
// Richard Hawkings, started 30/5/06
// compiles in offline cmt framework to binary executable, needs offline env

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <time.h>
#include <math.h>
#include <algorithm>

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IRecordIterator.h"
#include "CoolApplication/Application.h"
#include "CoralBase/AttributeListException.h"

#include "FileCatalog/IFileCatalog.h"
#include "FileCatalog/IFCAction.h"
#include "FileCatalog/IFCContainer.h"
#include "PersistencySvc/IDatabase.h"
#include "PersistencySvc/ISession.h"
#include "PersistencySvc/ITransaction.h"
#include "PersistencySvc/PersistencySvcException.h"
#include "PersistencySvc/SimpleUtilityBase.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/ISchema.h"
#include "RelationalAccess/ITable.h"
#include "RelationalAccess/IQuery.h"
#include "RelationalAccess/ICursor.h"

#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"

#include "PoolMapElement.h"
#include "ReplicaSorter.h"
#include "CoolTagInfo.h"

#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TObjString.h"

class AtlCoolCopy {
 public:
  AtlCoolCopy(const std::string sourcedb, const std::string destdb,
	      bool allowcreate=false);
  bool isOpen() const;
  bool addFolder(const std::string& folder,const bool onlyTags);
  bool addExclude(const std::string& folder);
  int copyFolder(const std::string folder,const std::vector<std::string>& taglist);
  int doCopy();
  int setOpts(int argc, const char* argv[]);

 private:
  // methods
  bool openConnections(const std::string sourcedb, const std::string destdb,
		  bool allowcreate);
  bool openCoraCool();
  bool getLockedTags();
  bool procOptVector(const int argc, const char* argv[],
		     std::vector<std::string>& folders);
  std::string transConn(const std::string& inconn);
  void setChannelRange(cool::IFolderPtr sourcefl);
  cool::ChannelId channelID(cool::IFolderPtr folder,
			    const std::string& chanstring);
  int copyIOVs(const std::string& folder,const std::string& destfolder,
	       cool::IFolderPtr sourcefl,CoraCoolFolderPtr sourceflc,
	       cool::IFolderPtr destfl,CoraCoolFolderPtr destflc,
      const std::string sourcetag,const std::string desttag,
	       const cool::ValidityKey since,const cool::ValidityKey until,
	       bool timestamp,bool checkrefs,bool iscora,
	       const cool::PayloadMode::Mode paymode, bool created);
  void adjustIOVs(const cool::ValidityKey& since,
		  const cool::ValidityKey& until,
		  const cool::ValidityKey& qsince,
		  const cool::ValidityKey& quntil,
		  cool::ValidityKey& newsince,
		  cool::ValidityKey& newuntil,
		  const bool timestamp);
  int nocopyIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
	       const cool::ValidityKey since,const cool::ValidityKey until,
	       bool checkrefs);
  int verifyIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,CoraCoolFolderPtr sourceflc,
      cool::IFolderPtr destfl,CoraCoolFolderPtr destflc,
      const std::string sourcetag,
	       const cool::ValidityKey since,const cool::ValidityKey until,
	       const bool checkrefs,const bool iscora,
	       const cool::PayloadMode::Mode paymode);
  int rootIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
	       const cool::ValidityKey since,const cool::ValidityKey until,
	       const bool timestamp);
  std::string rootDirs(const std::string& folder, const std::string& toproot);
  bool rootAllocate(const cool::IFieldSpecification& spec,
		    void*& sptr,char& rootID);
  void rootWrite(void* sptr, const cool::IField& field);
  bool isNumeric(const char* input);
  bool equalRecord(const cool::IRecord& lhs,
		   const cool::IRecord& rhs);
  int analyseIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
 		 const cool::ValidityKey since,const cool::ValidityKey until,
		 const bool timestamp);
  TH1F* bookOrFindTH1F(const std::string& hID, const std::string& htitle,
		       const int chan, const float xlow, const float xhigh);
  cool::ValidityKey timeVal(const char* input);
  std::string timeString(const cool::ValidityKey iovtime);
  cool::ValidityKey runLBVal(const char* input1, const char* input2);
  bool getTimeFromRun();
  bool getOnlineRun();
  bool getBulkRun();
  bool getRunList();
  int getUpdateMode(const std::string& desc, const std::string& tag);

  bool checkChannels(const std::string& folder,
		     cool::IFolderPtr sourcefl,cool::IFolderPtr destfl,
		     bool newfolder);
  void checkRef(const cool::IRecord& payload,
		const std::string& folder,const std::string& tag);
  int tagParents();
  int writeTagInfo();
  int listPoolRefs();
  int resolvePoolRefs();
  std::string getCoolHistGUID(const std::string& file);
  void filePoolRefs();
  pool::IFileCatalog* setupCatalog(const std::vector<std::string>& catvec);

  // input parameters
  std::string m_sourcedb;
  std::string m_destdb;
  bool m_allowcreate;
  bool m_recreate;
  bool m_includehead;
  bool m_excludehead;
  bool m_usertags;
  bool m_userupdatehead;
  bool m_debug;
  bool m_alliov;
  bool m_verify;
  bool m_root;
  bool m_zeronull;
  bool m_analyse;
  bool m_checkrefs;
  bool m_listpfn;
  bool m_poolopen;
  bool m_poolall;
  bool m_nocopy;
  bool m_nodata;
  bool m_nochannel;
  bool m_chdesc;
  bool m_hitag;
  bool m_nohitag;
  bool m_forcesingle;
  bool m_forcemulti;
  bool m_forcerune;
  bool m_forcetime;
  bool m_forcepay;
  bool m_forcenopay;
  bool m_sourceread;
  bool m_truncate;
  bool m_skipout;
  bool m_skiprep;
  bool m_applock;
  bool m_applocksv;
  bool m_readoracle;
  bool m_gettime;
  bool m_getonline;
  bool m_onlinerun;
  bool m_getbulk;
  bool m_prunetags;
  bool m_lockedonly;
  bool m_copytaginfo;
  bool m_copytaglock;
  bool m_coracool;
  bool m_ignoremode;
  bool m_ignorespec;
  bool m_checkdesttag;
  bool m_noclobroot;
  cool::ValidityKey m_runemin;
  cool::ValidityKey m_runemax;
  cool::ValidityKey m_timemin;
  cool::ValidityKey m_timemax;
  cool::ValidityKey m_newrunemin;
  cool::ValidityKey m_newrunemax;
  cool::ValidityKey m_newtimemin;
  cool::ValidityKey m_newtimemax;
  cool::ValidityKey m_srunemin;
  cool::ValidityKey m_srunemax;
  std::vector<std::string> m_channelRange;
  std::string m_channel1;
  std::string m_channel2;
  int m_bufsize;
  int m_sealmsg;
  long long m_anadelt;
  std::string m_outfolder;
  std::string m_outtag;
  std::string m_newdataset;
  std::string m_checkoutputfile;
  std::string m_timedb;
  std::string m_taglabel;
  std::vector<std::string> m_addguid; // additional guids to be processsed
  std::vector<std::string> m_addlfn; // additional LFNs to be processsed
  std::vector<std::string> m_parfile; // list of additional files with params
  std::vector<cool::ChannelId> m_excludechans; // list of channels to exclude
  std::vector<std::string> m_runfile; // list of filenames with run numbers
  std::vector<unsigned int> m_runlist; // list of runs open for UPD2 tags

  // internal variables
  coral::ConnectionService m_coralsvc;
  cool::Application m_coolapp;
  cool::IDatabaseSvc* m_dbSvc;
  ReplicaSorter* m_repsort;
  cool::IDatabasePtr m_sourceDbPtr;
  cool::IDatabasePtr m_destDbPtr;
  CoraCoolDatabasePtr m_sourceCoraPtr;
  CoraCoolDatabasePtr m_destCoraPtr;
  std::vector<std::string> m_folderlist;  // list of leaf folders to process
  std::vector<std::string> m_folderexcl;  // list of leaf folders to exclude
  std::vector<std::string> m_tags; // list of tags
  std::vector<std::string> m_magic; // list magic tags fragments to match
  std::vector<std::string> m_poolcat; // list of POOL catalogues for input
  std::vector<std::string> m_mergecat; // list of POOL catalogues for mergechk
  bool m_open;
  cool::ChannelSelection m_chansel;
  typedef std::map<std::string,PoolMapElement> PoolMap;
  PoolMap::iterator m_poollast; // pointer to last POOL ref updated
  PoolMap m_poolrefs; // POOL refs and usage counts
  // hierarchical tags - tag name and folder
  typedef std::map<std::string,std::string> HiTagMap;
  HiTagMap m_hitagmap;
  // tags indirectly used in the hierarchy and have to be treated at end
  std::vector<std::string> m_hiparent;
  // map of CoolTagInfo objects - tags which have have their descriptions
  // and lock status set in the destination DB at the end
  typedef std::map<std::string,CoolTagInfo> CoolTagMap;
  CoolTagMap m_cooltagmap;
  // output root file for ROOT file export and IOV analysis
  TFile* p_rootfile;
};

inline bool AtlCoolCopy::isOpen() const { return m_open; }

void printHelp();

AtlCoolCopy::AtlCoolCopy(const std::string sourcedb, const std::string destdb,
	    bool allowcreate) :
  m_sourcedb(sourcedb),m_destdb(destdb),m_allowcreate(allowcreate),
  m_recreate(false),
  m_includehead(false),m_excludehead(false),
  m_usertags(true),m_userupdatehead(false),m_debug(false),m_alliov(false),
  m_verify(false),m_root(false),m_zeronull(false),m_analyse(false),
  m_checkrefs(false),m_listpfn(false),m_poolopen(false),m_poolall(false),
  m_nocopy(false),m_nodata(false),m_nochannel(false),m_chdesc(false),
  m_hitag(false),m_nohitag(false),m_forcesingle(false),m_forcemulti(false),
  m_forcerune(false),m_forcetime(false),m_forcepay(false),m_forcenopay(false),
  m_sourceread(true),m_truncate(false),m_skipout(false),m_skiprep(false),
  m_applock(false),m_applocksv(false),
  m_readoracle(false),m_gettime(false),m_getonline(false),m_onlinerun(false),
  m_getbulk(false),
  m_prunetags(false),m_lockedonly(false),m_copytaginfo(false),
  m_copytaglock(false),m_coracool(true),m_ignoremode(false),
  m_ignorespec(false),m_checkdesttag(false),m_noclobroot(false),
  m_runemin(cool::ValidityKeyMin),m_runemax(cool::ValidityKeyMax),
  m_timemin(cool::ValidityKeyMin),m_timemax(cool::ValidityKeyMax),
  m_newrunemin(cool::ValidityKeyMin),m_newrunemax(cool::ValidityKeyMax),
  m_newtimemin(cool::ValidityKeyMin),m_newtimemax(cool::ValidityKeyMax),
  m_srunemin(cool::ValidityKeyMin),m_srunemax(cool::ValidityKeyMax),
  m_channel1(""),m_channel2(""),m_bufsize(1000),m_sealmsg(5),
  m_anadelt(-1),m_outfolder(""),m_outtag(""),m_newdataset(""),
  m_checkoutputfile(""),m_timedb(""),m_taglabel(""),
  m_coolapp(&m_coralsvc),
  m_dbSvc(&(m_coolapp.databaseService())),m_repsort(0),
  m_open(false),m_chansel(cool::ChannelSelection::all()),p_rootfile(0)
{
  m_poolrefs.clear();
  m_poollast=m_poolrefs.end();
  // configure CORAL components
  coral::IConnectionServiceConfiguration& csconfig=m_coralsvc.configuration();
  csconfig.disablePoolAutomaticCleanUp();
  csconfig.setConnectionTimeOut(0);
}

bool AtlCoolCopy::openConnections(const std::string sourcedb,
			     const std::string destdb,bool allowcreate) {
  // check connections not already open
  if (m_open) return true;
  // initialise replica sorter if it was requested
  if (m_readoracle) {
    m_repsort=new ReplicaSorter();
    coral::IConnectionServiceConfiguration& csconfig=m_coralsvc.configuration();
    csconfig.setReplicaSortingAlgorithm(*m_repsort);
  }
  // cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
  // open source database
  std::cout << "Open source database: " << sourcedb << std::endl;
  if (!m_sourceread) std::cout << "... in UPDATE mode" << std::endl;
  try {
    m_sourceDbPtr=m_dbSvc->openDatabase(transConn(sourcedb),m_sourceread);
  }
  catch (std::exception& e) {
    std::cout << "Cool exception caught: " << e.what() << std::endl;
    return false;
  }
  // open destination database
  if (m_nocopy) {
    m_open=true;
    return true;
  } else if (m_root || m_analyse) {
    std::cout << "Open destination ROOT file: " << destdb << std::endl;
    p_rootfile=new TFile(destdb.c_str(),"RECREATE");
    if (p_rootfile==0) std::cout << "ERROR: Could not open ROOT file" << 
			 std::endl;
    m_open=(p_rootfile!=0);
    return m_open;
  }
  std::string tdestdb=transConn(destdb);
  std::cout << "Open destination database: " << tdestdb << std::endl;
  try {
    m_destDbPtr=m_dbSvc->openDatabase(tdestdb,m_verify);
    // if the open succeeds and we are using recreate mode, drop/delete 
    // the existing database first
    if (m_recreate) {
      std::cout << 
      "Forcing recreation of destination database - deleting existing data!" 
		<< std::endl;
      m_destDbPtr.reset();
      m_dbSvc->dropDatabase(tdestdb);
      // go into catch to recrete database
      throw cool::DatabaseDoesNotExist("old database deleted");
    }
  }
  catch (std::exception& e) {
    std::cout << "COOL exception caught: " << e.what() << std::endl;
    // try to recover by creating new DB if possible and requested
    if (allowcreate || m_recreate) {
      std::cout << "Try to create new conditions DB" << std::endl;
      try {
        m_destDbPtr=m_dbSvc->createDatabase(tdestdb);
	std::cout << "Creation succeeded" << std::endl;
      }
      catch (cool::Exception& e) {
	std::cout << "Creation failed" << std::endl;
	return false;
      }
    } else {
      return false;
    }
  }
  m_open=true;
  return true;
}

bool AtlCoolCopy::openCoraCool() {
  if (m_sourceCoraPtr.get()==0) {
    std::cout << "Attempt to open source CoraCool DB " << m_sourcedb << 
      std::endl;
    m_sourceCoraPtr=CoraCoolDatabasePtr(new CoraCoolDatabase(m_sourcedb,
       m_sourceDbPtr,m_coralsvc,m_sourceread));
    std::cout << "Opened CoraCool source DB" << std::endl;
  }
  // skip destination DB if not copying
  if (m_nocopy) return true;
  if (m_destCoraPtr.get()==0) {
    std::cout << "Attempt to open destination CoraCool DB " << m_destdb << 
      std::endl;
    m_destCoraPtr=CoraCoolDatabasePtr(new CoraCoolDatabase(m_destdb,
       m_destDbPtr,m_coralsvc,m_verify));
    std::cout << "Opened CoraCool dest DB" << std::endl;
  }
  return true;
}

bool AtlCoolCopy::getLockedTags() {
  // set m_tags to list of top level tags which are locked
  m_tags.clear();
  cool::IFolderSetPtr topfolder=m_sourceDbPtr->getFolderSet("/");
  const std::vector<std::string>& toptaglist=topfolder->listTags();
  for (std::vector<std::string>::const_iterator toptag=toptaglist.begin();
       toptag!=toptaglist.end();++toptag) {
    cool::HvsTagLock::Status tstat=topfolder->tagLockStatus(*toptag);
    if (tstat==cool::HvsTagLock::LOCKED || 
	tstat==cool::HvsTagLock::PARTIALLYLOCKED) {
      std::cout << "Top-level tag " << *toptag << " will be copied" <<
	std::endl;
      m_tags.push_back(*toptag);
    }
  }
  std::cout << "Total of " << m_tags.size() << " top-level tags to be copied"
	    << std::endl;
  return (!m_tags.empty());
}

std::string AtlCoolCopy::transConn(const std::string& inconn) {
  // translate simple connection string (no slash) to mycool.db with given
  // instance name, all others are left alone
  if (inconn.find("/")==std::string::npos) {
    return "sqlite://X;schema=mycool.db;dbname="+inconn;
  } else {
    return inconn;
  }
}


bool AtlCoolCopy::addFolder(const std::string& folder,const bool onlyTags) {
  std::cout << "Add folders in path:" << folder << " [ ";
  const std::vector<std::string> nodelist=m_sourceDbPtr->listAllNodes();
  // find all matching leaf folders
  for (std::vector<std::string>::const_iterator nodeitr=nodelist.begin();
       nodeitr!=nodelist.end();++nodeitr) {
    // match exact folder name or leading part of path
    // for leading part matches, next char in folder name must be '/'
    // so /CALO/SetA matches /CALO/SetA/X but not /CALO/SetAB
    if (*nodeitr==folder || folder=="/" ||
      (nodeitr->substr(0,folder.size())==folder && 
     nodeitr->size()>folder.size() && nodeitr->substr(folder.size(),1)=='/')) {
      // check if folder on exclude list
      bool exclude=false;
      for (std::vector<std::string>::const_iterator iexcl=m_folderexcl.begin();
	   iexcl!=m_folderexcl.end();++iexcl) {
	if (iexcl->substr(0,1)=="/") {
  	  // exclude pattern starting / matches folder path (/SCT or /SCT/DCS)
          exclude=(exclude || nodeitr->substr(0,iexcl->size())==*iexcl);
	} else {
	  // exclude pattern without leading / matches anywhere in folder
	  exclude=(exclude || (nodeitr->find(*iexcl)!=std::string::npos));
	}
      }
      // check folder exists (is a leaf folder), and not excluded
      if (m_sourceDbPtr->existsFolder(*nodeitr) && !exclude && !onlyTags) {
	// only add if folder not already on list
	if (find(m_folderlist.begin(),m_folderlist.end(),*nodeitr)==
	    m_folderlist.end()) {
	  std::cout << *nodeitr << " ";
	  m_folderlist.push_back(*nodeitr);
	}
      }
      // if its a folderset, check for any hierarchical tags
      if (!m_nohitag && m_sourceDbPtr->existsFolderSet(*nodeitr) && !exclude) {
	if (!m_prunetags) {
          const std::vector<std::string> foldersettags=
	    m_sourceDbPtr->getFolderSet(*nodeitr)->listTags();
  	  if (!foldersettags.empty()) {
	    for (std::vector<std::string>::const_iterator 
              itr=foldersettags.begin();itr!=foldersettags.end();++itr) {
	      m_hitagmap[*itr]=*nodeitr;
	    }
	  }
	} else {
	  // only take tags in this folder which are referenced from one of
	  // the input tags (assumed to be top-level tags)
	  cool::IFolderSetPtr sfolder=m_sourceDbPtr->getFolderSet(*nodeitr);
	  for (std::vector<std::string>::const_iterator toptag=m_tags.begin();
	  toptag!=m_tags.end();++toptag) {
	    try {
	      std::string rtag=sfolder->resolveTag(*toptag);
	      m_hitagmap[rtag]=*nodeitr;
	    }
	    // catch exceptions indicating tag defines nothing here
	    // note std::exception rather than COOL exception to cover case
	    // when trying to resolve a leaf tag in the '/' folder, which
	    // throws a coral AttributeException
	    catch (std::exception& e) {}
	  }
	}
      }
    }
  }
  std::cout << "]" << std::endl;
  return true;
}

bool AtlCoolCopy::addExclude(const std::string& folder) {
  std::cout << "Adding folder to exclude list: " << folder << std::endl;
  m_folderexcl.push_back(folder);
  return true;
}

int AtlCoolCopy::copyFolder(const std::string folder,const std::vector<std::string>& taglist) {
  // get source folder
  cool::IFolderPtr sourcefl,destfl;
  CoraCoolFolderPtr sourceflc,destflc;
  try {
    sourcefl=m_sourceDbPtr->getFolder(folder);
  }
  catch (cool::Exception& e) {
    std::cout << "Could not get source folder: " << folder << std::endl;
    return 20;
  }
  const std::string& sourcedesc=sourcefl->description();
  // check if folder should be skipped from replication
  if (m_skiprep && sourcedesc.find("<norep/>")!=std::string::npos) {
    std::cout << "Folder " << folder << 
      " skipped due to <norep/> metadata" << std::endl;
    return 0;
  }
  const bool iscora=(m_coracool && 
		     sourcedesc.find("<coracool")!=std::string::npos);
  if (iscora) {
    // activate CoraCool and get source folder pointer
    openCoraCool();
    sourceflc=m_sourceCoraPtr->getFolder(folder);
  }
  // setup the channel-range
  setChannelRange(sourcefl);

  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  cool::FolderVersioning::Mode dvermode=vermode;
  if (m_forcesingle) {
    dvermode=cool::FolderVersioning::SINGLE_VERSION;
    std::cout << "Forcing destination folder to singleversion" << std::endl;
  }
  if (m_forcemulti) {
    dvermode=cool::FolderVersioning::MULTI_VERSION;
    std::cout << "Forcing destination folder to multiversion" << std::endl;
  }
  // setup type of folder (payload mode, inline, separate, or vector)
  const cool::PayloadMode::Mode spaymode=
    sourcefl->folderSpecification().payloadMode();
  cool::PayloadMode::Mode dpaymode=spaymode;

  // set name for destination folder
  std::string destfolder=folder;
  if (m_outfolder!="") {
    destfolder=m_outfolder;
    std::cout << "Destination folder will be renamed to " << destfolder <<
      std::endl;
  }
  
// get destination folder, try to create if not there (and not verifying!)
  bool created=false;
  if (!m_nocopy && !m_root && !m_analyse) {
    if (!m_destDbPtr->existsFolder(destfolder) && !m_verify) {
      std::cout << "Creating folder " << destfolder << " payload-type " << 
	dpaymode << " on destination" << std::endl;
      created=true;
      std::string metadata=sourcedesc;
      if (m_forcerune || m_forcetime) {
	std::string newmeta=m_forcerune ? "run-lumi" : "time";
	std::string::size_type p1,p2;
	p1=metadata.find("<timeStamp>");
	p2=metadata.find("</timeStamp>");
	if (p1!=std::string::npos && p2!=std::string::npos) {
	  metadata=metadata.substr(0,p1)+"<timeStamp>"+newmeta+
	    metadata.substr(p2);
  	  std::cout << "Forced destination folder to " << newmeta << " : "
		  << metadata << std::endl;
	} else {
	  std::cout << 
	    "ERROR: Could not parse metadata string to force timestamp type" 
		    << std::endl;
	}
      }
      // force separate or inline payload, but not if input is vector
      if (m_forcepay && spaymode!=cool::PayloadMode::VECTORPAYLOAD) 
	dpaymode=cool::PayloadMode::SEPARATEPAYLOAD;
      if (m_forcenopay && spaymode!=cool::PayloadMode::VECTORPAYLOAD) 
	dpaymode=cool::PayloadMode::INLINEPAYLOAD;

      try {
	if (iscora) {
	  destflc=m_destCoraPtr->createFolder(destfolder,
				      sourceflc->coralTableName(),
				      sourceflc->fkSpecification(),
				      sourceflc->payloadSpecification(),
				      sourceflc->coralFKey(),
				      sourceflc->coralPKey(),
				      metadata,dvermode,true);
	  std::cout << "Created CoraCool folder" << std::endl;
	  destfl=m_destDbPtr->getFolder(destfolder);
	} else {
	  destfl=m_destDbPtr->createFolder(destfolder,
				  cool::FolderSpecification(dvermode,
				  sourcefl->payloadSpecification(),dpaymode),
					   metadata,true);
	}
      }
      catch (cool::Exception&e ) {
        std::cout << "Create folder failed - aborting" << std::endl;
        return 30;
      }
    }
    // now get the pointer to destination folder if needed
    // note both COOL and CoraCool pointers are set if needed
    try {
      if (iscora) destflc=m_destCoraPtr->getFolder(destfolder);
      destfl=m_destDbPtr->getFolder(destfolder);
    }
    catch (cool::Exception& e) {
      std::cout << "Could not get destination folder: " << destfolder 
		<< std::endl;
      return 21;
    }
    // check payload specifications of folders are the same
    const cool::IRecordSpecification& sourcespec=
      sourcefl->payloadSpecification();
    const cool::IRecordSpecification& destspec=
      destfl->payloadSpecification();
    if (!(sourcespec==destspec)) {
      bool badspec=false;
      if (m_ignorespec) {
	// specifications differ - check names are same
	std::cout << 
	  "WARNING Source and destination folder specifications differ" << 
	  std::endl;
	for (unsigned int i=0;i<sourcespec.size();++i) {
	  const std::string& sname=sourcespec[i].name();
	  if (!destspec.exists(sname)) {
	    std::cout << "ERROR: Field " << sname << " absent from destination"
	    << std::endl;
	    badspec=true;
	  }
	}
      } else {
	badspec=true;
      }
      if (badspec) {
        std::cout << 
	  "ERROR Source and destination folder specifications differ" 
		  << std::endl;
        return 22;
      }
    }
    // check folder descriptions are the same - just print WARNING if not
    const std::string& destdesc=destfl->description();
    if (sourcedesc!=destdesc) {
      std::cout << "WARNING: Source and destination folder descriptions (meta-data) differ" << std::endl;
      std::cout << "Source folder: " << sourcedesc << std::endl;
      std::cout << "Destn  folder: " << destdesc << std::endl;
    }
    // check payload modes are same - print warning if not
    const cool::PayloadMode::Mode dpaymode=
      destfl->folderSpecification().payloadMode();
    if (spaymode!=dpaymode) {
      std::cout << "WARNING: Source (" << spaymode << ") and destination (" <<
	dpaymode << " folder payload modes differ" << std::endl;
    }
    
    // check/set channels table if requested
    if (!m_nochannel) {
      if (!checkChannels(folder,sourcefl,destfl,created)) return 23;
    }
  }
  // if only copying structures, stop here
  if (m_nodata) return 0;
  // extract folder/range information
  std::cout << "Start to process folder: " << folder;
  // check for timestamp XML 
  bool timestamp=(sourcefl->description().find("<timeStamp>time")!=
		  std::string::npos);
  cool::ValidityKey since,until;
  if (timestamp) {
    since=m_timemin;
    until=m_timemax;
    std::cout << " (timestamp)" << std::endl;
  } else {
    since=m_runemin;
    until=m_runemax;
    std::cout << " (run/lumi)" << std::endl;
  }
  if (m_alliov || m_truncate) 
    std::cout << "Output IOVs will be modified" << std::endl;
  if (m_skipout) 
    std::cout << "IOVs extending outside selection range will be skipped"
	      << std::endl;

  bool checkrefs=false;
  const std::string name0=sourcefl->payloadSpecification()[0].name();
  if (m_checkrefs && (name0=="PoolRef" || name0=="fileGUID")) {
    checkrefs=true;
    std::cout << "Check POOL references in folder " << folder << std::endl;
  }
  // if nocopy, and checking POOL references, and folder is not a POOL ref one
  // can skip the data access 
  if (m_checkrefs && m_nocopy && !checkrefs) return 0;

  // check for <fullrep/> metadata indicating copy all tags if -skiprep option
  bool copyall=
    (m_skiprep && (sourcedesc.find("<fullrep/>")!=std::string::npos));
  if (copyall) std::cout << 
   "All tags in folder will be copied due to <fullrep/> metadata" << std::endl;

  // set up true list of tags to copy
  std::vector<std::string> tags;
  if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
    std::cout << "Single version folder" << std::endl;
    tags.push_back("HEAD");
  } else {
    std::cout << "Multi version folder: consider tags [ ";
    // get list of tags in this node which are requested
    const std::vector<std::string> foldertags=sourcefl->listTags();
    for (std::vector<std::string>::const_iterator itag=foldertags.begin();
	 itag!=foldertags.end();++itag) {
      bool copyit=copyall;
      // if input taglist is empty, take all tags in folder
      if (taglist.size()==0 ||
	  find(taglist.begin(),taglist.end(),*itag)!=taglist.end()) 
	copyit=true;
      for (std::vector<std::string>::const_iterator imtag=m_magic.begin();
	   imtag!=m_magic.end();++imtag) {
	if (itag->find(*imtag)!=std::string::npos) copyit=true;
      }
      if (copyit) {
	tags.push_back(*itag);
        std::cout << *itag << " ";
      }
    }
    // if no tags were found, or doing inclusive hierarchical tag copying
    //  try resolving input tags hierarchically
    if (tags.empty() || m_hitag) {
      for (std::vector<std::string>::const_iterator itag=taglist.begin();
	   itag!=taglist.end();++itag) {
	try {
  	  std::string htag=sourcefl->resolveTag(*itag);
	  if (find(tags.begin(),tags.end(),htag)==tags.end()) {
	    std::cout << *itag << "=" << htag << " ";
	    tags.push_back(htag);
	  }
	}
	// ignore exceptions indicating tag not defined here
	// note std::exception rather than COOL exception to cover case
	// when trying to resolve a leaf tag in the '/' folder, which
	// throws a coral AttributeException
	catch (std::exception& e) { }
      }
    }
    std::cout << "]" << std::endl;
    // if still no tags were found, or forced HEAD tag copying, or no
    // tags in the folder at all (=MV folder being used as SV), add HEAD
    if (((tags.size()==0 || m_includehead) && !m_excludehead) ||
	foldertags.size()==0)
      tags.push_back("HEAD");
  }

  sourcefl->setPrefetchAll(false);
  for (std::vector<std::string>::const_iterator itag=tags.begin();
       itag!=tags.end();++itag) {
    std::string outtag=*itag;
    if (m_outtag!="") outtag=m_outtag;
    int retcode;
    // verify or copy folder
    if (m_verify) {
      retcode=verifyIOVs(folder,sourcefl,sourceflc,destfl,destflc,
			 *itag,since,until,checkrefs,iscora,spaymode);
    } else if (m_nocopy) {
      retcode=nocopyIOVs(folder,sourcefl,*itag,since,until,checkrefs);
    } else if (m_root) {
      retcode=rootIOVs(folder,sourcefl,*itag,since,until,timestamp);
    } else if (m_analyse) {
      retcode=analyseIOVs(folder,sourcefl,*itag,since,until,timestamp);
    } else {
      retcode=copyIOVs(folder,destfolder,sourcefl,sourceflc,destfl,destflc,
		       *itag,outtag,since,until,timestamp,checkrefs,iscora,
		       spaymode,created);
    }
    if (retcode!=0) {
      std::cout << "ERROR operation failed for folder " << folder << " tag " <<
	*itag << std::endl;
      return retcode;
    }
  }
  return 0;
}

void AtlCoolCopy::setChannelRange(cool::IFolderPtr sourcefl) {
  // add range specified via -ch1 -ch2 if given
  if (m_channel1!="" && m_channel2!="") {
    m_channelRange.clear();
    m_channelRange.push_back(m_channel1+":"+m_channel2);
  }
  m_chansel=cool::ChannelSelection::all();
  const size_t nChanRange=m_channelRange.size();
  for (size_t i=0;i<nChanRange;i++) {
    size_t cpos=m_channelRange[i].find(":");
    if (cpos==std::string::npos || cpos > m_channelRange[i].size()-1) {
      // single channel
      std::cout << "Adding channel " << m_channelRange[i] << 
	" to channel selection" << std::endl;
      if (m_chansel.allChannels())
	m_chansel=cool::ChannelSelection(channelID(sourcefl,
						   m_channelRange[i]));
      else
	m_chansel.addChannel(channelID(sourcefl,m_channelRange[i]));
    }
    else { 
      // Channel Range
      std::string c1=m_channelRange[i].substr(0,cpos);
      std::string c2=m_channelRange[i].substr(1+cpos);
      std::cout << "Adding channel range " << c1 << " to " << c2 << 
	" to channel selection" << std::endl;
      if (m_chansel.allChannels())
	m_chansel=cool::ChannelSelection(channelID(sourcefl,c1),
					 channelID(sourcefl,c2));
      else
	m_chansel.addRange(channelID(sourcefl,c1),channelID(sourcefl,c2));
    }      
  }//end loop over channel ranges
}

cool::ChannelId AtlCoolCopy::channelID(cool::IFolderPtr folder,
				       const std::string& chanstring) {
  const char* cstr=chanstring.c_str();
  if (isNumeric(cstr)) {
    // channel is a number 
    return cool::ChannelId(strtoul(cstr,NULL,10));
  } else {
    cool::ChannelId chan=0;
    try {
      chan=folder->channelId(chanstring);
      std::cout << "Channel name " << chanstring << " maps to channelID " 
		<< chan << std::endl;
    }
    catch (cool::Exception& e) {
      std::cout << "ERROR: Channel name " << chanstring << 
	" not defined in folder " << std::endl;
    }
    return chan;
  }
}

int AtlCoolCopy::copyIOVs(const std::string& folder,
      const std::string& destfolder,
      cool::IFolderPtr sourcefl,CoraCoolFolderPtr sourceflc,
      cool::IFolderPtr destfl,CoraCoolFolderPtr destflc,
      const std::string sourcetag,const std::string desttag,
      const cool::ValidityKey since,const cool::ValidityKey until,
	    bool timestamp,bool checkrefs,bool iscora,
	    const cool::PayloadMode::Mode paymode ,bool created) {

  std::cout << "Copying tag " << sourcetag << " of folder " << folder << 
      " to destination tag " << desttag << std::endl;
  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  cool::FolderVersioning::Mode dvermode=destfl->versioningMode();
  // check for online mode
  int updatemode=getUpdateMode(destfl->description(),desttag);
  std::vector<std::string> dtaglist=destfl->listTags();
  // if online mode, must have --getonline and either -truncate or -alliov
  // only if destination DB is oracle
  if (updatemode==1 && m_destdb.find("oracle")!=std::string::npos &&
     (m_getonline==false || (m_truncate==false && m_alliov==false))) {
    if (m_ignoremode) {
      std::cout << "Folder is online (UPD1) mode but IGNORING PROTECTION"
		<< std::endl;
    } else {
         if(find(dtaglist.begin(),dtaglist.end(),desttag)!=dtaglist.end()) {
        	 std::cout << "Folder is online mode (UPD1) and tag already exist - -getonline and -truncate or -alliov options MUST be used" << std::endl;
        	 return 35;
         }
         else {
        	 std::cout << "Folder is online mode (UPD1), new tag will be created" << std::endl;
         }

    }
  }
  // if UPD4 mode, must have -getbulk and either -truncate or -alliov
  if (updatemode==4 && m_destdb.find("oracle")!=std::string::npos &&
     (m_getbulk==false || (m_truncate==false && m_alliov==false))) {
    if (m_ignoremode) {
      std::cout << "Folder is bulkreco (UPD4) mode but IGNORING PROTECTION"
		<< std::endl;
    } else {
    	if(find(dtaglist.begin(),dtaglist.end(),desttag)!=dtaglist.end()) {
    	    std::cout << "Folder is bulkreco mode (UPD4) - -getbulk and -truncate or -alliov options MUST be used" << std::endl;
    	    return 35;
    	} else {
    		std::cout << "Folder is bulkreco mode (UPD4), new tag will be created" << std::endl;
    	}

    }
  }

  // if destination tags are being checked, check if this tag already 
  // exists in destination folder, if so skip copy
  // also skip if tag is HEAD, and folder was not newly created
  if (m_checkdesttag) {
    if (find(dtaglist.begin(),dtaglist.end(),desttag)!=dtaglist.end()
        || (desttag=="HEAD" && created==false)) {
      std::cout << "Destination tag " << desttag << 
	" already exists in folder " << folder << " - skip copy" << std::endl;
      return 0;
    }
  }
  bool relock=false; // tag must be relocked afterwards
  bool prot=false; //update is protected, irrespective of if tag must be locked
  if ((m_applock || (updatemode==1 && m_getonline) || updatemode==2 ||
       (updatemode==4 && m_getbulk)) && 
      dvermode==cool::FolderVersioning::MULTI_VERSION) {
    // check if the destination tag exists and is locked
    std::vector<std::string> dtaglist=destfl->listTags();
    for (std::vector<std::string>::const_iterator itr=dtaglist.begin();
	 itr!=dtaglist.end();++itr) {
      if (desttag==*itr) {
	// tag exists - must protect it unless ignoremode
	prot=!m_ignoremode;
	// check lock status
	if (destfl->tagLockStatus(desttag)==cool::HvsTagLock::LOCKED) {
	  std::cout << "Unlocking destination tag " << desttag << 
	    " for append or UPDx access" << std::endl;
	  if (m_applocksv) {
	    std::cout << "Appending according to SV folder rules" << std::endl;
	    if (updatemode!=3) {
	      std::cout << 
	        "ERROR: Only allowed for UPD3 mode tags" << std::endl;
	      return 36;
	    }
	  }
	  destfl->setTagLockStatus(desttag,cool::HvsTagLock::UNLOCKED);
	  relock=true;
	}
      }
    }
  }
  int nobj=0;
  int nbuf=0;
  int nskip=0;
  int nbad=0;
  // set up tag to be used at point of insertion
  std::string localtag="";
  if (dvermode==cool::FolderVersioning::MULTI_VERSION &&
      desttag!="HEAD" && m_usertags==true) localtag=desttag;
  // now loop over IOVs - separate for CoraCool and COOL
  try {
    if (iscora) {
      // branch for CoraCool
      sourceflc->setPrefetchAll(false);
      CoraCoolObjectIterPtr sourceitr;
      if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
	sourceitr=sourceflc->browseObjects(since,until,m_chansel);
      } else {
	sourceitr=sourceflc->browseObjects(since,until,m_chansel,sourcetag);
      }
      // keep track of new FKs of inserted objects
      std::map<int,int> insertkeymap;
      unsigned int nref=0;
      while (sourceitr->hasNext()) {
	CoraCoolObjectPtr obj=sourceitr->next();
	// check object should not be skipped as spreading outside select-IOV
	if (m_skipout && 
	    (obj->since()<m_srunemin || obj->until()>m_srunemax)) {
	  ++nskip;
	  continue;
	}
	// check object should not be skipped as excluded channel
	if (!m_excludechans.empty()) {
	  if (find(m_excludechans.begin(),m_excludechans.end(),
		   obj->channelId())!=m_excludechans.end()) {
	    ++nskip;
	    continue;
	  }
	}
        if (nbuf==0) {
          if (m_debug) std::cout << 
	   "Setup new CoraCool storage buffer at object " << nobj << std::endl;
          destflc->setupStorageBuffer();
        }
	cool::ValidityKey newsince,newuntil;
	adjustIOVs(obj->since(),obj->until(),since,until,newsince,newuntil,
		   timestamp);
	// skip negative/zero IOV lengths
	if (newsince>=newuntil) {
	  std::cout << "WARNING: Skipping IOV with since " << newsince <<
	    ">= until" << newuntil << std::endl;
	  ++nbad;
	  continue;
	}
	// find the old FK, check if we have already inserted this
	// in which case a reference can be added
	int oldfk=0;
	std::map<int,int>::const_iterator ikey=insertkeymap.end();
	bool foundkey=false;
	// ensure returned data is not of zero size - if so cannot extract
	// and remember the FK
	if (obj->size()>0) {
	  oldfk=sourceflc->getAttrKey(
	   (*obj->begin())[sourceflc->coralFKey()]);
	  ikey=insertkeymap.find(oldfk);
	  foundkey=true;
	}
	if (ikey==insertkeymap.end()) {
  	  int newfk=
	    destflc->storeObject(newsince,newuntil,obj->begin(),obj->end(),
			       obj->channelId(),localtag,
			     (!m_userupdatehead && localtag!=""));
	  if (foundkey) insertkeymap[oldfk]=newfk;
	} else {
	  destflc->referenceObject(newsince,newuntil,ikey->second,
			       obj->channelId(),localtag,
			     (!m_userupdatehead && localtag!=""));
	  ++nref;
	}
        ++nbuf;
        // flush buffer every m_bufsize objects
        if (nbuf==m_bufsize) {
          if (m_debug) std::cout << "Flush buffer after " << nobj << "," << 
            nbuf << " objects " << std::endl;
	  // note CoraCool requires explicit re-setup of buffer after a flush
          destflc->flushStorageBuffer();
	  destflc->setupStorageBuffer();
          nbuf=0;
        }
	++nobj;
      }
      sourceitr->close();
      if (nref>0) std::cout << "Reference-to-existing used for " << nref
			    << " payload objects" << std::endl;
      if (nbuf>0) {
        if (m_debug) std::cout << "Final buffer flush at " << nobj << 
          "," << nbuf << std::endl;
        destflc->flushStorageBuffer();
      }
    } else {
      // branch for pure COOL objects
      cool::IObjectIteratorPtr sourceitr;
      if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
        sourceitr=sourcefl->browseObjects(since,until,m_chansel);
      } else {
        sourceitr=sourcefl->browseObjects(since,until,m_chansel,sourcetag);
      }
      while (sourceitr->goToNext()) {
        const cool::IObject& obj=sourceitr->currentRef();
	// check object should not be skipped as spreading outside select-IOV
	if (m_skipout && (obj.since()<m_srunemin || obj.until()>m_srunemax)) {
	  ++nskip;
	  continue;
	}
	// check object should not be skipped as excluded channel
	if (!m_excludechans.empty()) {
	  if (find(m_excludechans.begin(),m_excludechans.end(),
		   obj.channelId())!=m_excludechans.end()) {
	    ++nskip;
	    continue;
	  }
	}
        if (nbuf==0) {
          if (m_debug) std::cout << "Setup new storage buffer at object " <<
  	  nobj << std::endl;
          destfl->setupStorageBuffer();
        }
        cool::ValidityKey newsince;
        cool::ValidityKey newuntil;
	adjustIOVs(obj.since(),obj.until(),since,until,newsince,newuntil,
		   timestamp);
	// skip negative/zero IOV lengths
	if (newsince>=newuntil) {
	  std::cout << "WARNING: Skipping IOV with since " << newsince <<
	    ">= until" << newuntil << std::endl;
	  ++nbad;
	  continue;
	}
        if (checkrefs) checkRef(obj.payload(),folder,sourcetag);
        if (prot) {
        // check existing data in destination DB to ensure no clash
	  if (m_applocksv) {
	    // appendlockedsv mode - all IOV ends must be infinite 
	    // and new since must be greater than old since
	    if (newuntil!=cool::ValidityKeyMax) {
		std::cout << "New IOVs must have until=cool::ValidityKeyMax" << std::endl;
		throw cool::Exception("Illegal insert over locked IOV",
				      "AtlCoolCopy");
	    }
	    cool::IObjectIteratorPtr checkitr=destfl->browseObjects(newsince,
			   newuntil,obj.channelId(),desttag);
	    while (checkitr->goToNext()) {
	      const cool::IObject& checkobj=checkitr->currentRef();
	      if (checkobj.since()>=newsince) {
		std::cout << "ERROR:: Attempt to insert SV overlapping IOV whilst appending to locked tag" << std::endl;
		throw cool::Exception("Illegal insert over locked IOV",
				      "AtlCoolCopy");
	      }
	      if (checkobj.until()!=cool::ValidityKeyMax) {
		std::cout << "Existing IOVs must have until=cool::ValidityKeyMax" << std::endl;
		throw cool::Exception("Illegal insert over locked IOV",
				      "AtlCoolCopy");
	      }
	    }
	    checkitr->close();
	  } else if (m_applock) {
	    // appendlocked mode - just make sure no objects in this IOV
    	    const unsigned int nexist=
	      destfl->countObjects(newsince,newuntil,obj.channelId(),desttag);
	    if (nexist>0) {
	      std::cout << "ERROR: Attempt to insert IOV over " << nexist << 
	      " objects whilst appending to locked tag" << std::endl;
	      throw cool::Exception("Illegal insert over locked IOV",
				"AtlCoolCopy");
	    }
	  } else if (updatemode==2) {
	    // UPD2 tag - make sure run is on list of runs open for update
	    // only for run/lumi based folders
	    if (timestamp) 
	      throw cool::Exception(
	   "Attempt to insert into locked UPD2 tag with timestamp format",
				    "AtlCoolCopy");
	    unsigned int run1=newsince >> 32;
	    unsigned int run2=(newuntil-1) >> 32;
	    for (unsigned int irun=run1;irun<=run2;++irun) {
	      if (!std::binary_search(m_runlist.begin(),m_runlist.end(),irun))
	      { 
		std::cout << "Run " << irun << " from range [" << run1 << ","
			<< run2 << "] not found in runfile list" << std::endl;
		throw cool::Exception("Illegal insert over locked IOV",
				      "AtlCoolCopy");
	      }
	    }
	  }
	  // updatemode==1 will fall through, but no checks are done here
        }
	if (paymode==cool::PayloadMode::VECTORPAYLOAD) {
	  // cool vector payload object copy
	  cool::IRecordIterator& pitr=obj.payloadIterator();
	  const cool::IRecordVectorPtr vptr=pitr.fetchAllAsVector();
	  destfl->storeObject(newsince,newuntil,
			     *vptr,obj.channelId(),localtag,
			    (!m_userupdatehead && localtag!=""));
	  nbuf+=vptr->size();
	  pitr.close();
	} else {
	  // standard COOL object copy
	  destfl->storeObject(newsince,newuntil,
	      	  obj.payload(),obj.channelId(),localtag,
			    (!m_userupdatehead && localtag!=""));
	  ++nbuf;
	}
        // flush buffer every m_bufsize objects
        if (nbuf>=m_bufsize) {
          if (m_debug) std::cout << "Flush buffer after " << nobj << "," << 
            nbuf << " objects " << std::endl;
          destfl->flushStorageBuffer();
          nbuf=0;
        }
        ++nobj;
      }
      // finished with the iterator
      sourceitr->close();
      if (nbuf>0) {
        if (m_debug) std::cout << "Final buffer flush at " << nobj << 
          "," << nbuf << std::endl;
        destfl->flushStorageBuffer();
      } // end of COOL-specific part
    }
    std::cout << "Folder copied with " << nobj << " objects" << std::endl;
    if (nskip>0) std::cout << nskip << 
   " objects were skipped extending outside IOV selection or excluded channel"
			   << std::endl;
    if (nbad>0) std::cout << nbad << 
      " objects were skipped having zero or negative IOV lengths" << std::endl;
  }
  // exceptions thrown from insert loop (both COOL or CORACOOL)
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from copy loop: " << e.what() <<
      std::endl;
    if (relock) {
      std::cout << "Relocking destination tag " << desttag << std::endl;
      destfl->setTagLockStatus(desttag,cool::HvsTagLock::LOCKED);
    }
    return 31;
  }
  // check if tag needs to be relocked
  if (relock) {
    std::cout << "Relocking destination tag " << desttag << std::endl;
    destfl->setTagLockStatus(desttag,cool::HvsTagLock::LOCKED);
  }

  if (dvermode==cool::FolderVersioning::MULTI_VERSION && desttag!="HEAD") {
    if (m_usertags==false) {
      // apply HEAD style tagging
      std::cout << "Tag folder with HEAD-style tagging for tag: " << desttag
        	<< std::endl;
      try {
        destfl->tagCurrentHead(desttag,sourcefl->tagDescription(sourcetag));
      } 
      catch (cool::Exception& e) {
        std::cout << "Exception thrown in HEAD-style folder-tag: " <<
          e.what() << std::endl;
        return 32;
      }
    }
    // record information about this tag for later
    if (m_copytaginfo) m_cooltagmap.insert(
      CoolTagMap::value_type(desttag,CoolTagInfo(m_sourceDbPtr,folder,
			    destfolder,sourcetag,desttag,m_taglabel)));
    // check if any hierarchical tags reference the just-copied tag
    if (!m_hitagmap.empty()) {
      for (HiTagMap::const_iterator imap=m_hitagmap.begin();
	   imap!=m_hitagmap.end();++imap) {
	try {
	  if (sourcefl->findTagRelation(imap->first)==sourcetag) {
	    std::cout << "Create hierarchical tag between " << desttag <<
		" and " << imap->first << " in folder " << imap->second <<
		std::endl;
	    // first check this relation has not already been created
	    try {
	      std::string etag=destfl->resolveTag(imap->first);
	      if (etag==desttag) {
		std::cout << "This relation has already been created" << std::endl;
	      } else {
		std::cout << "ERROR: Tag in parent already related to " << 
		  desttag << std::endl;
	      }
	    }
	    catch (cool::Exception& e ) {
	      // only do creation if does not already exist - via exception
	      try {
	        destfl->createTagRelation(imap->first,desttag);
	        // check if this implicates a new parent folder
	        // to be checked for further tags up the tree
	        if (find(m_hiparent.begin(),m_hiparent.end(),imap->second)==
	  	  m_hiparent.end()) m_hiparent.push_back(imap->second);
		// store the original tag information to transfer
		// properties later
		if (m_copytaginfo && 
		    m_cooltagmap.find(imap->first)==m_cooltagmap.end())
		  m_cooltagmap.insert(CoolTagMap::value_type(imap->first,
			CoolTagInfo(m_sourceDbPtr,imap->second,imap->second,
				     imap->first,imap->first)));
	      }
	      catch (cool::Exception& e) {
	        std::cout << "Cool exception " << e.what() << 
	  	"thrown in hierarchical tag creation" << std::endl;
	        return 34;
	      }
	    }
	  }
	}
	catch (cool::Exception& e) {
	}
      }
    }
  }
  return 0;
}

void AtlCoolCopy::adjustIOVs(const cool::ValidityKey& since, 
			     const cool::ValidityKey& until,
			     const cool::ValidityKey& qsince,
			     const cool::ValidityKey& quntil,
			     cool::ValidityKey& newsince,
			     cool::ValidityKey& newuntil,
			     bool timestamp) {
  // set newsince/until to since/until
  // but doing any required truncation/adjustment
  newsince=since;
  newuntil=until;
  if (m_alliov) {
    // make IOV cover specified range
    if (timestamp) {
      newsince=m_newtimemin;
      newuntil=m_newtimemax;
    } else {
      newsince=m_newrunemin;
      newuntil=m_newrunemax;
    }
  }
  // check for IOV truncation to range of query
  if (m_truncate) {
    if (newsince<qsince) newsince=qsince;
    if (newuntil>quntil) newuntil=quntil;
  }
  // check not requesting an IOV with negative length
  if (newuntil<newsince) throw cool::Exception(
    "Attempt to insert IOV with -ve length","AtlCoolCopy::adjustIOVs");
}

int AtlCoolCopy::nocopyIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
      const cool::ValidityKey since,const cool::ValidityKey until,
			  bool checkrefs) {

  std::cout << "Reading tag " << sourcetag << " of folder " << folder << 
      " (no copy)" << std::endl;
  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  int nobj=0;
  cool::IObjectIteratorPtr sourceitr;
  if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
    sourceitr=sourcefl->browseObjects(since,until,m_chansel);
  } else {
    sourceitr=sourcefl->browseObjects(since,until,m_chansel,sourcetag);
  }
  try {
    while (sourceitr->goToNext()) {
      const cool::IObject& obj=sourceitr->currentRef();
      if (checkrefs) checkRef(obj.payload(),folder,sourcetag);
      ++nobj;
    }
    std::cout << "Folder scanned with " << nobj << " objects" << std::endl;
  }
  // exceptions thrown from read loop
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from read loop: " << e.what() <<
      std::endl;
    return 33;
  }
  // finished with the iterator
  sourceitr->close();
  return 0;
}

int AtlCoolCopy::verifyIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,CoraCoolFolderPtr sourceflc,
      cool::IFolderPtr destfl,CoraCoolFolderPtr destflc,
      const std::string sourcetag,
      const cool::ValidityKey since,const cool::ValidityKey until,
			    const bool checkrefs,const bool iscora, 
			    const cool::PayloadMode::Mode paymode) {

  std::cout << "Verifying tag " << sourcetag << " of folder " << folder << 
    std::endl;
  destfl->setPrefetchAll(false);
  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  int nobj=0;
  cool::IObjectIteratorPtr sourceitr,destitr;
  CoraCoolObjectIterPtr csourceitr,cdestitr;
  std::string tag=sourcetag;
  if (vermode==cool::FolderVersioning::SINGLE_VERSION || sourcetag=="HEAD") 
    tag="";
  try {
    if (iscora) {
      sourceflc->setPrefetchAll(false);
      csourceitr=sourceflc->browseObjects(since,until,m_chansel,tag);
      cdestitr=destflc->browseObjects(since,until,m_chansel,tag);
    } else {
      sourceitr=sourcefl->browseObjects(since,until,m_chansel,tag);
      destitr=destfl->browseObjects(since,until,m_chansel,tag);
    }
  }
  catch (std::exception& e) {
    std::cout << "Exception thrown from verify iterator setup: " << e.what() <<
      std::endl;
    return 105;
  }
  try {
    // the check algorithm assumes the two databases will give back the 
    // results of the query in the same order, which is currently the 
    // case in COOL 1.3. If this changes, the verification will be more complex
    if (iscora) {
      const std::string& cfkey=sourceflc->coralFKey();
      const std::string& cpkey=sourceflc->coralPKey();
      while (csourceitr->hasNext()) {
	CoraCoolObjectPtr sobj=csourceitr->next();
	// check object should not be skipped as excluded channel
	if (!m_excludechans.empty()) {
	  if (find(m_excludechans.begin(),m_excludechans.end(),
		   sobj->channelId())!=m_excludechans.end()) {
	    continue;
	  }
	}
	if (cdestitr->hasNext()) {
	  CoraCoolObjectPtr dobj=cdestitr->next();
  	  // check objects are the same
  	  int iret=0;
	  // check IOV equality
	  if (sobj->since()!=dobj->since() || sobj->until()!=dobj->until())
	    iret=100;
	  // check channel equality
	  if (sobj->channelId()!=dobj->channelId()) 
	    iret=101;
	  // check size of payloads
	  if (sobj->size()!=dobj->size()) {
	    std::cout << "ERROR CoraCool object " << nobj << 
	      " sizes do not match: " << sobj->size() << " " << dobj->size() 
		      << std::endl;
	    iret=102;
	  }
	  // check payload equality (does not check attribute names)
	  CoraCoolObject::const_iterator ditr=dobj->begin();
	  for (CoraCoolObject::const_iterator sitr=sobj->begin();
	       sitr!=sobj->end();++sitr,++ditr) {
	    // loop over all the attributes, in order to skip PK and FK
	    for (coral::AttributeList::const_iterator aitr=sitr->begin();
		 aitr!=sitr->end();++aitr) {
	      const std::string& aname=aitr->specification().name();
	      if (aname!=cfkey && aname!=cpkey) {
		try {
		  if (*aitr!=(*ditr)[aname]) {
		    std::cout << "ERROR Values of attriute " << aname << 
		      " differ" << std::endl;
	  	    iret=102;
		  }
		}
		catch (coral::AttributeListException& e) {
		  std::cout << "ERROR: CoraCool attribute " << aname << 
		    " not found in destination!" << std::endl;
		  iret=102;
		}
	      }
	    }
	  }
	  if (iret!=0) {
	    std::cout << "ERROR database entries do not match: since: " << 
	      sobj->since() << "," << dobj->since() << " until: " <<
	      sobj->until() << "," << dobj->until() << " channel: " <<
              sobj->channelId() << "," << dobj->channelId() << std::endl;
	    return iret;
	  }
	} else {
	  std::cout << 
           "ERROR destination folder no matching CoraCool iterator for object "
		  << nobj << std::endl;
	  return 103;
	}
 	++ nobj;
      }
    } else {
      while (sourceitr->goToNext()) {
        const cool::IObject& sobj=sourceitr->currentRef();
	// check object should not be skipped as excluded channel
	if (!m_excludechans.empty()) {
	  if (find(m_excludechans.begin(),m_excludechans.end(),
		   sobj.channelId())!=m_excludechans.end()) {
	    continue;
	  }
	}
        if (checkrefs) checkRef(sobj.payload(),folder,sourcetag);
        if (destitr->goToNext()) {
   	  const cool::IObject& dobj=destitr->currentRef();
  	  // check objects are the same
  	  int iret=0;
	  // check IOV equality
	  if (sobj.since()!=dobj.since() || sobj.until()!=dobj.until())
	    iret=100;
	  // check channel equality
	  if (sobj.channelId()!=dobj.channelId()) 
	    iret=101;
	  // check payload equality (does not check attribute names)
	  // do not use cool::IRecord equality operator as does not correctly 
	  // handle null values
	  if (paymode==cool::PayloadMode::VECTORPAYLOAD) {
	    cool::IRecordIterator& spitr=sobj.payloadIterator();
	    const cool::IRecordVectorPtr svptr=spitr.fetchAllAsVector();
	    cool::IRecordIterator& dpitr=dobj.payloadIterator();	
	    const cool::IRecordVectorPtr dvptr=dpitr.fetchAllAsVector();
	    if (svptr->size()!=dvptr->size()) {
	      iret=102;
	    } else {
	      // loop through the payloads, checking equality - assumes
	      // order is significant and same in source and destination DBs
	      cool::IRecordVector::const_iterator svitr=svptr->begin();
	      cool::IRecordVector::const_iterator svend=svptr->end();
	      cool::IRecordVector::const_iterator dvitr=dvptr->begin();
	      for (;svitr!=svend;++svitr,++dvitr) {
		if (!equalRecord(**svitr,**dvitr)) iret=102;
	      }
	    }
	    if (iret!=0) {
	      std::cout << "ERROR vector payloads do not match (size " << svptr->size() << "," << dvptr->size() << ")" << std::endl;
	    }
	    
	  } else {
	    // standard COOL folder - simple check of payloads
	    if (!equalRecord(sobj.payload(),dobj.payload())) iret=102;
	  }

	  if (iret!=0) {
	    std::cout << "ERROR database entries do not match: since: " << 
	      sobj.since() << "," << dobj.since() << " until: " <<
	      sobj.until() << "," << dobj.until() << " channel: " <<
              sobj.channelId() << "," << dobj.channelId() << std::endl;
	    std::cout << "Source payload:" << std::endl;
	    sobj.payload().attributeList().toOutputStream(std::cout);
	    std::cout << std::endl << "Destination payload:" << std::endl;
	    dobj.payload().attributeList().toOutputStream(std::cout);
	    std::cout << std::endl;
	    return iret;
	  }
        } else {  
	  std::cout << 
           "ERROR destination folder no matching iterator for object "
		  << nobj << std::endl;
	  return 103;
        }
        ++nobj;
      }
    }
  }
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from verify loop: " << e.what() <<
      std::endl;
    return 104;
  }
  // finished with the iterators
  if (iscora) {
  } else {
    sourceitr->close();
    destitr->close();
  }
  std::cout << "Verification of folder " << folder << " tag " << 
    sourcetag << " OK (" << nobj << " objects)" << std::endl;
  return 0;
}

// structures for copying data to ROOT ntuple
ULong64_t nt_since;
ULong64_t nt_until;
UInt_t nt_runsince;
UInt_t nt_rununtil;
UInt_t nt_lbsince;
UInt_t nt_lbuntil;
UInt_t nt_channel;
char nt_tagid[256];
std::string nt_treename;
std::vector<void*> nt_bufferptr;


int AtlCoolCopy::rootIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
      const cool::ValidityKey since,const cool::ValidityKey until,
			  const bool timestamp) {
  // copy this selection to ROOT
  std::cout << "Write tag " << sourcetag << " of folder " << folder << 
    " to ROOT file" << std::endl;
  // create the directory structure - top directory COOL
  std::string treename=rootDirs(folder,"COOL");
  bool timestamp2=timestamp;
  if (m_forcetime) timestamp2=true;
  if (m_forcerune) timestamp2=false;
  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  // create TTree with appropriate structure - only if it does not exist
  // could have been created from a previous tag in the same folder
  TTree* tree=static_cast<TTree*>(gDirectory->FindObject(treename.c_str()));
  if (tree==0) {
    std::cout << "Book TTree " << treename << std::endl;
    tree=new TTree(treename.c_str(),"COOL datadump");
    if (timestamp2) {
      tree->Branch("IOVSince",&nt_since,"IOVSince/l");
      tree->Branch("IOVUntil",&nt_until,"IOVUntil/l");
    } else {
      tree->Branch("RunSince",&nt_runsince,"RunSince/i");
      tree->Branch("RunUntil",&nt_rununtil,"RunUntil/i");
      tree->Branch("LBSince",&nt_lbsince,"LBSince/i");
      tree->Branch("LBUntil",&nt_lbuntil,"LBUntil/i");
    }
    tree->Branch("ChannelID",&nt_channel,"ChannelID/i");
    if (vermode==cool::FolderVersioning::MULTI_VERSION)
      tree->Branch("TagID",nt_tagid,"TagID/C");
    // now loop through specification, creating payload buffer and tree
    const cool::IRecordSpecification& spec=
    (sourcefl->folderSpecification()).payloadSpecification();
    unsigned int ncolumns=spec.size();
    // clear the buffer of pointers - note this leaks the memory of the
    // previous buffers, but to do this properly would have to remember
    // the type of each object and delete appropriately
    nt_treename=treename;
    nt_bufferptr.clear();
    for (unsigned int icol=0;icol<ncolumns;++icol) {
      const cool::IFieldSpecification& fieldspec=spec[icol];
      void* ptr=0;
      char rootID;
      if (rootAllocate(fieldspec,ptr,rootID)) {
        tree->Branch(fieldspec.name().c_str(),ptr,
		   (fieldspec.name()+"/"+rootID).c_str());
	if (m_debug) std::cout << "Defining column for " << spec[icol].name() 
	    << " of type " << spec[icol].storageType().name() << std::endl;
      } else {
        std::cout << "Attribute " << spec[icol].name() << " of type " << 
	  spec[icol].storageType().name() << " will be skipped" << std::endl;
      }
      nt_bufferptr.push_back(ptr);
    }
  } else {
    std::cout << "TTree " << treename << " already exists" << std::endl;
    // check we have seen and defined this tree
    unsigned int size=
      (sourcefl->folderSpecification()).payloadSpecification().size();
    if (treename!=nt_treename || size!=nt_bufferptr.size()) {
      std::cout << "ERROR in tree buffer definition: expect " << treename <<
	" size " << size << " but buffer has " << nt_treename << " size " 
		<< nt_bufferptr.size() << std::endl;
      return 123;
    }
  }

  int nobj=0;
  int nex=0;
  cool::IObjectIteratorPtr sourceitr;
  try {
    if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
      sourceitr=sourcefl->browseObjects(since,until,m_chansel);
    } else {
      sourceitr=sourcefl->browseObjects(since,until,m_chansel,sourcetag);
    }
  }
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from ROOT copy iterator setup: " << 
      e.what() << std::endl;
    return 125;
  }
  // now loop over all IOVs to copy them 
  try {
    while (sourceitr->goToNext()) {
      const cool::IObject& sobj=sourceitr->currentRef();
      if (timestamp2) {
        nt_since=sobj.since();
        nt_until=sobj.until();
      } else {
        nt_runsince=(sobj.since() >> 32);
        nt_rununtil=(sobj.until() >> 32);
        nt_lbsince=(sobj.since() & 0xFFFFFFFF);
        nt_lbuntil=(sobj.until() & 0xFFFFFFFF);
      }
      nt_channel=sobj.channelId();
      if (vermode==cool::FolderVersioning::MULTI_VERSION) {
	// truncate the string first to avoid coverity complaining about
	// potential buffer overruns
	std::string sourcetag2=sourcetag.substr(0,255);
	strncpy(nt_tagid,sourcetag2.c_str(),sizeof(nt_tagid)-1);
      }
      // loop over the payload elements and fill the ones for which buffers
      // are defined
      try {
        const cool::IRecord& record=sobj.payload();
        for (unsigned int icol=0;icol<record.size();++icol) {
	  if (nt_bufferptr[icol]!=0) rootWrite(nt_bufferptr[icol],record[icol]);
        }
        tree->Fill();
        ++nobj;
      }
      catch (cool::Exception& e) {
	// can get exceptions due to NULL values
	// catch them and continue...
	++nex;
      }
    }
    sourceitr->close();
    std::cout << "Written " << nobj << " objects to ROOT TTree with " << nex 
	      << " nulls/exceptions"  << std::endl;
  }
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from ROOT file writing loop: " << 
      e.what() << std::endl;
    return 124;
  }
  return 0;
}

std::string AtlCoolCopy::rootDirs(const std::string& folder, 
				  const std::string& toproot) {
  // create a ROOT directory structure start with toproot (=COOL or COOLANA)
  // and cd to the directory where a tree should be created
  // return the tree name (=leaf of the COOL foldername)
  p_rootfile->cd();
  if (p_rootfile->FindObject(toproot.c_str())==0) {
    p_rootfile->mkdir(toproot.c_str());
    std::cout << "Made top directory " << toproot << std::endl;
  }
  gDirectory->cd(toproot.c_str());
  // now parse the COOL folder name to create intermediate directories
  // assume folder names begin with '/', to be skipped
  std::string::size_type iofs1=1;
  std::string::size_type iofs2=1;
  std::string treename;
  while (iofs2!=std::string::npos) {
    iofs2=folder.find("/",iofs1);
    if (iofs2==std::string::npos) {
      // no more /, so current part of string is leaf tree name
      treename=folder.substr(iofs1);
    } else {
      std::string dirname=folder.substr(iofs1,iofs2-iofs1);
      iofs1=iofs2+1;
      if (gDirectory->FindObject(dirname.c_str())==0) {
	std::cout << "Make directory " << dirname << std::endl;
        gDirectory->mkdir(dirname.c_str());
      }
      gDirectory->cd(dirname.c_str());
    }
  }
  return treename;
}

bool AtlCoolCopy::rootAllocate(const cool::IFieldSpecification& spec,
			       void*& sptr,char& rootID) {
  const cool::StorageType& stype=spec.storageType();
  rootID=' ';
  sptr=0;
  if (stype==cool::StorageType::Bool) {
    // map bool to uint32 for now
    sptr=static_cast<void*>(new unsigned int(0));
    rootID='i';
  } else if (stype==cool::StorageType::UChar) {
    sptr=static_cast<void*>(new unsigned char(' '));
    rootID='C';
  } else if (stype==cool::StorageType::Int16) {
    sptr=static_cast<void*>(new short(0));
    rootID='S';
  } else if (stype==cool::StorageType::UInt16) {
    sptr=static_cast<void*>(new unsigned short(0));
    rootID='s';
  } else if (stype==cool::StorageType::Int32) {
    sptr=static_cast<void*>(new int(0));
    rootID='I';
  } else if (stype==cool::StorageType::UInt32) {
    sptr=static_cast<void*>(new unsigned int(0));
    rootID='i';
  } else if (stype==cool::StorageType::Int64) {
    sptr=static_cast<void*>(new long long int(0));
    rootID='L';
  } else if (stype==cool::StorageType::UInt63) {
    sptr=static_cast<void*>(new unsigned long long int(0));
    rootID='l';
  } else if (stype==cool::StorageType::Float) {
    sptr=static_cast<void*>(new float(0.));
    rootID='F';
  } else if (stype==cool::StorageType::Double) {
    sptr=static_cast<void*>(new double(0.));
    rootID='D';
  } else if (stype==cool::StorageType::String4k || 
	     stype==cool::StorageType::String255) {
    sptr=static_cast<void*>(new char[4100]);
    rootID='C';
  } else if (stype==cool::StorageType::String64k && !m_noclobroot) {
    sptr=static_cast<void*>(new char[65536]);
    rootID='C';
  } else if (stype==cool::StorageType::String16M && !m_noclobroot) {
    sptr=static_cast<void*>(new char[67108864]);
    rootID='C';
  } else {
    std::cout << "rootAllocate: Unsupported storage type for attribute: " <<
      spec.name() << std::endl;
  }
  return (sptr!=0);
}

void AtlCoolCopy::rootWrite(void* sptr,const cool::IField& field) {
  // check for NULL value
  bool recnull=(field.isNull() && m_zeronull);
  const cool::StorageType& stype=field.storageType();
  if (stype==cool::StorageType::Bool) {
    if (recnull) {
      *(static_cast<int*>(sptr))=0;
    } else {
      *(static_cast<int*>(sptr))=(field.data<bool>() ? 1 : 0);
    }
  } else if (stype==cool::StorageType::UChar) {
    if (recnull) {
      *(static_cast<unsigned char*>(sptr))=0;
    } else {
      *(static_cast<unsigned char*>(sptr))=field.data<unsigned char>();
    }
  } else if (stype==cool::StorageType::Int16) {
    if (recnull) {
      *(static_cast<short*>(sptr))=0;
    } else {
      *(static_cast<short*>(sptr))=field.data<short>();
    }
  } else if (stype==cool::StorageType::UInt16) {
    if (recnull) {
      *(static_cast<unsigned short*>(sptr))=0;
    } else {
      *(static_cast<unsigned short*>(sptr))=field.data<unsigned short>();
    }
  } else if (stype==cool::StorageType::Int32) {
    if (recnull) {
      *(static_cast<int*>(sptr))=0;
    } else {
      *(static_cast<int*>(sptr))=field.data<int>();
    }
  } else if (stype==cool::StorageType::UInt32) {
    if (recnull) {
      *(static_cast<unsigned int*>(sptr))=0;
    } else {
      *(static_cast<unsigned int*>(sptr))=field.data<unsigned int>();
    }
  } else if (stype==cool::StorageType::Int64) {
    if (recnull) {
      *(static_cast<long long*>(sptr))=0;
    } else {
      *(static_cast<long long*>(sptr))=field.data<long long>();
    }
  } else if (stype==cool::StorageType::UInt63) {
    if (recnull) {
      *(static_cast<unsigned long long*>(sptr))=0;
    } else {
      *(static_cast<unsigned long long*>(sptr))=field.data<unsigned long long>();
    }
  } else if (stype==cool::StorageType::Float) {
    if (recnull) {
      *(static_cast<float*>(sptr))=0.;
    } else {
      *(static_cast<float*>(sptr))=field.data<float>();
    }
  } else if (stype==cool::StorageType::Double) {
    if (recnull) {
      *(static_cast<double*>(sptr))=0.;
    } else {
      *(static_cast<double*>(sptr))=field.data<double>();
    }
  } else if (stype==cool::StorageType::String255 || 
	     stype==cool::StorageType::String4k ||
	     stype==cool::StorageType::String64k ||
	     stype==cool::StorageType::String16M) {
    if (recnull) {
      strcpy(static_cast<char*>(sptr),"NULL");
    } else {
      strcpy(static_cast<char*>(sptr),field.data<std::string>().c_str());
    }
  } else {
    std::cout << "ERROR: Unknown storage type in rootWrite!" << std::endl;
  }
}

int AtlCoolCopy::analyseIOVs(const std::string& folder,
      cool::IFolderPtr sourcefl,const std::string sourcetag,
       const cool::ValidityKey since,const cool::ValidityKey until,
			     const bool timestamp) {
  // analyse the IOV and channel structure of this folder/tag, make ROOT histos
  std::cout << "Analyse tag " << sourcetag << " of folder " << folder << 
    std::endl;
  bool anatime=(m_anadelt>=0);
  if (anatime) {
    std::cout << "Analyse time structures with tolerance of " << 
      m_anadelt/1.E9 << " seconds " << std::endl;
  }
  // create COOLANA/folder structure and create leaf directory for histograms
  std::string dirname=rootDirs(folder,"COOLANA");
  if (gDirectory->FindObject(dirname.c_str())==0) {
    std::cout << "Make directory " << dirname << std::endl;
    gDirectory->mkdir(dirname.c_str());
  }
  gDirectory->cd(dirname.c_str());
  // get channels
  std::vector<cool::ChannelId> channels=sourcefl->listChannels();
  unsigned int nchan=channels.size();
  // end of last IOV seen on this channel
  long long* lastiov=new long long[nchan];
  long long* iovtotlen=new long long[nchan];
  long long* iovtotgap=new long long[nchan];
  int* iovn=new int[nchan];
  for (unsigned int i=0;i<nchan;++i) {
    lastiov[i]=-1; // no IOV seen on this channel yet
    iovtotlen[i]=0;
    iovtotgap[i]=0;
    iovn[i]=0;
  }
  cool::ValidityKey globsince=cool::ValidityKeyMax;
  cool::ValidityKey globuntil=cool::ValidityKeyMin;

  // book histograms - only if the objects do not already exist (previous tag)
  TH1F* h_iovchan=bookOrFindTH1F("IOVSperChannel","IOVs per channel",
				 nchan,-0.5,nchan-0.5);
  TH1F* h_iovname=bookOrFindTH1F("IOVSperChannelName",
	 "IOVs per channel-name (copy of IOVSperChannel)",
			         nchan,-0.5,nchan-0.5);
  TH1F* h_iovlength=bookOrFindTH1F("IOVLogLength","IOV log10 length",
				   100,0.,20.);
  TH1F* h_iovgap=bookOrFindTH1F("IOVLogGapLength","IOV log10 gap length",
				100,0.,20.);
  // histograms for time analysis if needed
  TH1F* h_anadelt=0;
  TH1F* h_chandelt=0;
  if (anatime) {
    h_anadelt=bookOrFindTH1F("IOVAlignLogDelT","IOV alignment log deltaT",
			     100,0.,20.);
    // add factor 2 in number of channels, to catch those where a channel
    // changes many times within the m_anadelt window (i.e. noise/jitter)
    h_chandelt=bookOrFindTH1F("AlignedChanPerIOV","Aligned channels per IOV",
			      nchan*2,0.5,nchan*2-0.5);
  }
  std::cout << "Booked histograms for folder with " << nchan << " channels"
	    << std::endl;
  // setup storage for IOV time analysis 
  typedef std::map<cool::ValidityKey,int> IOVTimeMap;
  IOVTimeMap iov_time_map;

  // setup iterator to loop over objects
  int nobj=0;
  cool::FolderVersioning::Mode vermode=sourcefl->versioningMode();
  cool::IObjectIteratorPtr sourceitr;
  try {
    if (vermode==cool::FolderVersioning::SINGLE_VERSION) {
      sourceitr=sourcefl->browseObjects(since,until,m_chansel);
    } else {
      sourceitr=sourcefl->browseObjects(since,until,m_chansel,sourcetag);
    }
  }
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from analysis copy iterator setup: " << 
      e.what() << std::endl;
    return 135;
  }
  // look up names of all the channels in one go
  std::map<cool::ChannelId,std::string> 
    chanmap=sourcefl->listChannelsWithNames();
  // now loop over all IOVs and write them
  try {
    while (sourceitr->goToNext()) {
      const cool::IObject& sobj=sourceitr->currentRef();
      // get channel and find its index
      cool::ChannelId ichanid=sobj.channelId();
      const std::string& cname=chanmap[ichanid];
      std::pair<std::vector<cool::ChannelId>::iterator,
	std::vector<cool::ChannelId>::iterator > chanitr=
  	  std::equal_range(channels.begin(),channels.end(),ichanid);
      if (chanitr.first!=chanitr.second) {
	cool::ChannelId ichan=std::distance(channels.begin(),chanitr.first);
	Int_t bin=h_iovchan->Fill(ichan);
	if (ichan && !h_iovname->GetBinContent(bin)) {
	  h_iovname->GetXaxis()->SetBinLabel(bin,cname.c_str());	
	}
	h_iovname->Fill(ichan);
	// analyse IOV start/stop
	cool::ValidityKey since2=sobj.since();
	cool::ValidityKey until2=sobj.until();
	if (since2<globsince) globsince=since2;
	// dont count IOV is open-ended, so length statistics will make sense
        if (until2!=cool::ValidityKeyMax) {
  	  if (until2>globuntil) globuntil=until2;
   	  long long len=until2-since2;
	  h_iovlength->Fill(log10(len));
	  iovn[ichan]+=1;
	  iovtotlen[ichan]+=len;
	}
	if (lastiov[ichan]<static_cast<long long>(since2) && 
	    lastiov[ichan]>=0) {
	  // have a gap in the IOV structure for this channel
	  long long gap=since2-lastiov[ichan];
	  iovtotgap[ichan]+=gap;
	  h_iovgap->Fill(log10(gap));
	  std::cout << "Gap of " << gap << std::endl;
	}
	if (until2!=cool::ValidityKeyMax) {
	  lastiov[ichan]=until2;
	} else {
	  lastiov[ichan]=since2;
	}
	// analyse IOV alignment
	if (anatime) {
	  long long del1=-1;
	  long long del2=-1;
	  // find the nearest time to this one
	  // this gives the first value exceeding since2
	  IOVTimeMap::iterator hiitr=iov_time_map.lower_bound(since2);
	  IOVTimeMap::iterator lowitr=hiitr;
	  if (hiitr!=iov_time_map.end()) {
	    // del1 is +ve time interval to next one
	    del1=hiitr->first-since2;
	  }
	  if (lowitr!=iov_time_map.begin()) {
	    // del2 is +ve time interval to previous one
	    --lowitr;
	    del2=since2-lowitr->first;
	  }
	  long long del=-1;
	  IOVTimeMap::iterator moditr;
	  bool domod=false;
	  if (del1<=m_anadelt && del1>-1) {
	    moditr=hiitr;
	    del=del1;
	    domod=true;
	  }
	  if (del2<=m_anadelt && del2>-1 && (del2<del || del==-1)) {
	    moditr=lowitr;
	    del=del2;
	    domod=true;
	  }
	  if (domod) {
	    ++(moditr->second);
	    if (del>0) {
  	      h_anadelt->Fill(log10(del));
	    } else {
	      h_anadelt->Fill(0.);
	    }
	  } else {
	    iov_time_map[since2]=0;
	  }
	}
      } else {
  	// channel ID not found 
        std::cout << "ERROR :Channel " << ichanid << 
        " not found in channel list - ignored" << std::endl;
      }
      ++nobj;
    }
    std::cout << "Finished analysis with " << nobj << " objects" << std::endl;
    if (timestamp) {
      std::cout << "IOV timestamp range: " << globsince << " " << 
	timeString(globsince) << "to " << globuntil << " " << 
	timeString(globuntil) << std::endl;
    } else {
      std::cout << "IOV run/LB range [" << (globsince >> 32) << "," <<
      (globsince & 0xFFFFFFFF) << "] to [" << (globuntil >> 32) << "," <<
      (globuntil & 0xFFFFFFFF) << "]" << std::endl;
    }
    // calculate statistics for each channel
    TH1F* h_iovoccchan=bookOrFindTH1F("ChannelOcc","Channel Occupancy",
				      nchan,-0.5,nchan-0.5);
    TH1F* h_iovlenchan=bookOrFindTH1F("IOVLenChan",
	      "Mean IOV length per channel",nchan,-0.5,nchan-0.5);
    for (unsigned int i=0;i<nchan;++i) {
      float occ=0.;
      float avlen=0.;
      if (iovn[i]>0) {
        occ=1.-float(iovtotgap[i])/float(iovtotlen[i]+iovtotgap[i]);
	avlen=float(iovtotlen[i])/iovn[i];
      } 
      h_iovoccchan->Fill(i,occ);
      h_iovlenchan->Fill(i,avlen);
    }
    if (anatime) {
      std::cout << "Alignment analysis: " << iov_time_map.size()
		<< " seperate IOV starts (tolerance " << m_anadelt/1.E9 << 
	" seconds)" << std::endl;
      // fill histogram of number of channels incremented per IOV
      for (IOVTimeMap::const_iterator itr=iov_time_map.begin();
	   itr!=iov_time_map.end();++itr) {
	h_chandelt->Fill(itr->second);
      }
    }
  }
  catch (cool::Exception& e) {
    std::cout << "Exception thrown from folder analysis reading loop: " << 
      e.what() << std::endl;
    return 134;
  }
  sourceitr->close();
  delete [] lastiov;
  delete [] iovtotlen;
  delete [] iovtotgap;
  delete[] iovn; iovn=0; 
  return 0;
}

TH1F* AtlCoolCopy::bookOrFindTH1F(const std::string& hID,
				  const std::string& htitle,
		const int nchan, const float xlow, const float xhigh) {
  // check histogram does not exist in current directory
  // if yes, return pointer to it, else book and return pointer
  TObject* obj=gDirectory->FindObject(hID.c_str());
  if (obj==0) {
    return new TH1F(hID.c_str(),htitle.c_str(),nchan,xlow,xhigh);
  } else {
    return static_cast<TH1F*>(obj);
  }
}


int AtlCoolCopy::doCopy() {
  if (isOpen()) {
    int retcode=0;
    // execute copy for all defined folders
    for (std::vector<std::string>::const_iterator ifolder=
	 m_folderlist.begin();ifolder!=m_folderlist.end();++ifolder) {
      int code=copyFolder(*ifolder,m_tags);
      if (code>retcode) retcode=code;
    }
    if (!m_hiparent.empty()) {
      // fill in any hierarchical tags for parent folders
      int code=tagParents();
      if (code>retcode) retcode=code;
    }
    if (m_copytaginfo) {
      // write information (tag description, lock status) to dest tags
      int code=writeTagInfo();
      if (code>retcode) retcode=code;
    }
    if (m_checkrefs) {
      int code=0;
      if (m_poolcat.empty()) {
	code=listPoolRefs();
	if (m_checkoutputfile!="") filePoolRefs();
      } else {
	code=resolvePoolRefs();
      }
      if (code>retcode) retcode=code;
    }
    if (p_rootfile) {
      p_rootfile->Write();
      delete p_rootfile;
    }
    return retcode;
  } else {
    return 10;
  }
}

bool AtlCoolCopy::procOptVector(const int argc, const char* argv[], 
				std::vector<std::string>& folders) {
  // process an array of options, return True if OK, False if not
  int ic=0;
  bool error=false;
  while (ic<argc) {
    int ir=argc-ic;
    std::string par0=argv[ic];
    // strip double "--" to achieve compatability with python-style options
    if (par0.substr(0,2)=="--") par0=par0.substr(1);
    if ((par0=="-f" || par0=="-folder") && ir>1) {
      folders.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-e" || par0=="-exclude") && ir>1) {
      addExclude(argv[ic+1]);
      ++ic;
    } else if ((par0=="-t" || par0=="-tag") && ir>1) {
      m_tags.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-mt" || par0=="-magic") && ir>1) {
      m_magic.push_back("_"+std::string(argv[ic+1]));
      ++ic;
    } else if ((par0=="-of" || par0=="-outfolder") && ir>1) {
      m_outfolder=argv[ic+1];
      ++ic;
    } else if ((par0=="-ot" || par0=="-outtag") && ir>1) {
      m_outtag=argv[ic+1];
      ++ic;
    } else if ((par0=="-bs" || par0=="-buffersize") && ir>1) {
      m_bufsize=atoi(argv[ic+1]);
      ++ic;
    } else if ((par0=="-sl" || par0=="-seal") && ir>1) {
      m_sealmsg=atoi(argv[ic+1]);
      ++ic;
    } else if ((par0=="-rls" || par0=="-runlumisince") && ir>2) {
      m_runemin=runLBVal(argv[ic+1],argv[ic+2]);
      ic+=2;
    } else if ((par0=="-rlu" || par0=="-runlumiuntil") && ir>2) {
      m_runemax=runLBVal(argv[ic+1],argv[ic+2]);
      ic+=2;
    } else if ((par0=="-nrls" || par0=="-newrunlumisince") && ir>2) {
      m_newrunemin=runLBVal(argv[ic+1],argv[ic+2]);
      ic+=2;
    } else if ((par0=="-nrlu" || par0=="-newrunlumiuntil") && ir>2) {
      m_newrunemax=runLBVal(argv[ic+1],argv[ic+2]);
      ic+=2;
    } else if ((par0=="-srls" || par0=="-skiprunlumisince") && ir>2) {
      m_srunemin=runLBVal(argv[ic+1],argv[ic+2]);
      m_skipout=true;
      ic+=2;
    } else if ((par0=="-srlu" || par0=="-skiprunlumiuntil") && ir>2) {
      m_srunemax=runLBVal(argv[ic+1],argv[ic+2]);
      m_skipout=true;
      ic+=2;
    } else if (par0=="-ro" || par0=="-root") {
      m_root=true;
    } else if (par0=="-zn" || par0=="-zeronull") {
      m_zeronull=true;
    } else if (par0=="-ana" || par0=="-analyse") {
      m_analyse=true;
      float ttime=atof(argv[ic+1])*1.E9;
      m_anadelt=static_cast<long long>(ttime);
      ++ic;
    } else if ((par0=="-rs" || par0=="-runsince") && ir>1) {
      m_runemin=(static_cast<long long>(atol(argv[ic+1])) << 32);
      ++ic;
    } else if ((par0=="-ru" || par0=="-rununtil") && ir>1) {
      m_runemax=(static_cast<long long>(1+atol(argv[ic+1])) << 32)-1;
      ++ic;
    } else if ((par0=="-r" || par0=="-run") && ir>1) {
      m_runemin=(static_cast<long long>(atol(argv[ic+1])) << 32);
      m_runemax=(static_cast<long long>(1+atol(argv[ic+1])) << 32)-1;
      ++ic;
    } else if ((par0=="-ts" || par0=="-timesince") && ir>1) {
      m_timemin=timeVal(argv[ic+1]);
      ++ic;
    } else if ((par0=="-tu" || par0=="-timeuntil") && ir>1) {
      m_timemax=timeVal(argv[ic+1]);
      ++ic;
    } else if ((par0=="-nts" || par0=="-newtimesince") && ir>1) {
      m_newtimemin=timeVal(argv[ic+1]);
      ++ic;
    } else if ((par0=="-ntu" || par0=="-newtimeuntil") && ir>1) {
      m_newtimemax=timeVal(argv[ic+1]);
      ++ic;
    } else if (par0=="-c" || par0=="-create") {
      m_allowcreate=true;
    } else if ((par0=="-ch" || par0=="-channel") && ir>1) {
      m_channelRange.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-ch1" || par0=="-channel1") && ir>1) {
      m_channel1=argv[ic+1];
      ++ic;
    } else if ((par0=="-ch2" || par0=="-channel2") && ir>1) {
      m_channel2=argv[ic+1];
      ++ic;
    } else if (par0=="-chd" || par0=="-channeldesc") {
      m_chdesc=true;
      // no abbreviaton for this one - dangerous option
    } else if (par0=="-forcerecreate") {
      m_recreate=true;
    } else if (par0=="-d" || par0=="-debug") {
      m_debug=true;
    } else if (par0=="-a" || par0=="-alliov") {
      m_alliov=true;
    } else if (par0=="-ih" || par0=="-includehead") {
      m_includehead=true;
    } else if (par0=="-eh" || par0=="-excludehead") {
      m_excludehead=true;
    } else if (par0=="-ht" || par0=="-headtag") {
      m_usertags=false;
    } else if (par0=="-uht" || par0=="-userheadtag") {
      m_userupdatehead=true;
    } else if (par0=="-v" || par0=="-verify") {
      m_verify=true;
    } else if (par0=="-nc" || par0=="-nocopy") {
      m_nocopy=true;
    } else if (par0=="-noc" || par0=="-nocoracool") {
      m_coracool=false;
    } else if (par0=="-nch" || par0=="-nochannel") {
      m_nochannel=true;
    } else if (par0=="-ncr" || par0=="-noclobroot") {
      m_noclobroot=true;
    } else if (par0=="-nd" || par0=="-nodata") {
      m_nodata=true;
    } else if (par0=="-nh" || par0=="-nohitag") {
      m_nohitag=true;
    } else if (par0=="-hi" || par0=="-hitag") {
      m_hitag=true;
    } else if ((par0=="-ec" || par0=="-excludechannel") && ir>1) {
      m_excludechans.push_back(strtoul(argv[ic+1],NULL,10));
      ++ic;
    } else if (par0=="-fs" || par0=="-forcesingle") {
      m_forcesingle=true;
    } else if (par0=="-fm" || par0=="-forcemulti") {
      m_forcemulti=true;
    } else if (par0=="-frl" || par0=="-forcerunlumi") {
      m_forcerune=true;
    } else if (par0=="-ftm" || par0=="-forcetime") {
      m_forcetime=true;
    } else if (par0=="-fp" || par0=="-forcepayload") {
      m_forcepay=true;
    } else if (par0=="-fnp" || par0=="-forcenopayload") {
      m_forcenopay=true;
    } else if (par0=="-cr" || par0=="-checkrefs") {
      m_checkrefs=true;
    } else if (par0=="-lp" || par0=="-listpfn") {
      m_listpfn=true;
      m_checkrefs=true;
    } else if (par0=="-cf" || par0=="-checkfiles") {
      m_poolopen=true;
      m_checkrefs=true;
    } else if (par0=="-pa" || par0=="-poolall") {
      m_poolall=true;
    } else if ((par0=="-co" || par0=="-checkoutput") && ir>1) {
      m_checkoutputfile=argv[ic+1];
      ++ic;
    } else if ((par0=="-pc" || par0=="-poolcat") && ir>1) {
      m_poolcat.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-mc" || par0=="-mergecat") && ir>1) {
      m_mergecat.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-ds" || par0=="-dataset") && ir>1) {
      m_newdataset=argv[ic+1];
      ++ic;
    } else if (par0=="-us" || par0=="-updatesource") {
      m_sourceread=false;
    } else if (par0=="-cd" || par0=="-checkdest") {
      m_checkdesttag=true;
    } else if (par0=="-tr" || par0=="-truncate") {
      m_truncate=true;
    } else if (par0=="-al" || par0=="-appendlocked") {
      m_applock=true;
    } else if (par0=="-alsv" || par0=="-appendlockedsv") {
      m_applock=true;
      m_applocksv=true;
    } else if (par0=="-rdo" || par0=="-readoracle") {
      m_readoracle=true;
    } else if (par0=="-skiprep" || par0=="-sr") {
      m_skiprep=true;
    } else if (par0=="-go" || par0=="-getonline") {
      m_getonline=true;
    } else if (par0=="-onr" || par0=="-onlinerun") {
      m_onlinerun=true;
    } else if (par0=="-gb" || par0=="-getbulk") {
      m_getbulk=true;
    } else if (par0=="-gt" || par0=="-gettime") {
      m_gettime=true;
    } else if ((par0=="-tdb" || par0=="-timedb") && ir>1) {
      m_timedb=argv[ic+1];
      ++ic;
    } else if (par0=="-ignoremode" && ir>1) {
      std::cout << "Ignoremode password is " << argv[ic+1] << ":end" << std::endl;
      if (strcmp(argv[ic+1],"BackDoor")==0) {
	m_ignoremode=true;
      } else {
	std::cout << "ERROR: Incorrect password for -ignoremode" << std::endl;
	error=true;
      }
      ++ic;
    } else if (par0=="-is" || par0=="-ignorespec") {
      m_ignorespec=true;
    } else if (par0=="-pt" || par0=="-prunetags") {
      m_prunetags=true;
      m_excludehead=true;
    } else if (par0=="-lo" || par0=="-lockedonly") {
      m_lockedonly=true;
      m_prunetags=true;
      m_excludehead=true;
    } else if (par0=="-cti" || par0=="-copytaginfo") {
      m_copytaginfo=true;
    } else if (par0=="-ctl" || par0=="-copytaglock") {
      m_copytaginfo=true;
      m_copytaglock=true;
    } else if ((par0=="-tl" || par0=="-taglabel") && ir>1) {
      m_taglabel=argv[ic+1];
      ++ic;
      m_copytaginfo=true;
    } else if ((par0=="-ag" || par0=="-addguid") && ir>1) {
      m_addguid.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-alf" || par0=="-addlfn") && ir>1) {
      m_addlfn.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-pf" || par0=="-parfile") && ir>1) {
      m_parfile.push_back(argv[ic+1]);
      ++ic;
    } else if ((par0=="-rf" || par0=="-runfile") && ir>1) {
      m_runfile.push_back(argv[ic+1]);
      ++ic;
    } else if (par0=="-h" || par0=="-help") {
      // help printout triggered by -999 return code
      return 999;
    } else {
      std::cout << "Parameter error for argument: " << par0 << std::endl;
      error=true;
    }
    ++ic;
  }
  return !error;
}


int AtlCoolCopy::setOpts(int argc, const char* argv[]) {
  // accumulate raw folder list
  std::vector<std::string> folders;
  // process options given on command-line
  if (!procOptVector(argc,argv,folders)) return 2;

  // parse any parameters in files via parfile option
  for (std::vector<std::string>::const_iterator ifile=m_parfile.begin();
       ifile!=m_parfile.end();++ifile) {
    std::cout << "Reading parameters from file " << *ifile << std::endl;
    FILE* p_inp=fopen(ifile->c_str(),"r");
    if (p_inp==0) {
      std::cout << "File not found" << std::endl;
      return 3;
    }
    char* p_buf=new char[999];
    while (!feof(p_inp)) {
      char* p_line=fgets(p_buf,999,p_inp);
      if (p_line!=NULL) {
	int fargc=0;
	const char* fargv[99];
	// split this line into tokens
	char* p_start=0;
	while (*p_line!='\0') {
	  if (*p_line==' ' || *p_line=='\n') {
	    // pointing at a space/newline, marking the end of a parameter
	    if (p_start!=NULL) {
	      // if we have a parameter, mark the end and store it
	      *p_line='\0';
	      fargv[fargc]=p_start;
	      fargc++;
	      p_start=NULL;
	    }
	  } else {
	    // mark the start of a parameter
	    if (p_start==NULL) p_start=p_line;
	  }
	  ++p_line;
	}
	if (fargc>0) {
  	  if (!procOptVector(fargc,&fargv[0],folders)) {
	    fclose(p_inp);
	    return 3;
	  }
	}
      }
    }
    std::cout << "Close file" << std::endl;
    fclose(p_inp);
    delete[] p_buf;
  }

  // now open the database so folder lookup will work
  if (!openConnections(m_sourcedb,m_destdb,m_allowcreate)) {
    std::cout << "Problem opening connections" << std::endl;
    return 10;
  } else {
    // having assembled the raw list of folders and exclude list, construct
    // real list of folders (allows exclude to be specified after folders on
    // command line), if none given, add '/'
    if (m_lockedonly) {
      if (getLockedTags()==0) return 11;
    }
    if (folders.size()==0) {
      // no folders specified, take all with tags
      addFolder("/",false);
    } else {
      // explicit list of folders
      for (std::vector<std::string>::const_iterator ifold=folders.begin();
         ifold!=folders.end();++ifold) addFolder(*ifold,false);
      // need to process only tags in root folder, if inclusive hiearchicals
      if (m_hitag) addFolder("/",true);
    }
  }
  if (m_getonline) {
    if (!getOnlineRun()) return 6;
  }
  if (m_getbulk) {
    if (!getBulkRun()) return 6;
  }
  if (m_gettime) {
    if (!getTimeFromRun()) return 5;
  }
  if (!m_runfile.empty()) {
    if (!getRunList()) return 7;
  }
  // list out parameter changes
  if (m_runemin!=cool::ValidityKeyMin || m_runemax!=cool::ValidityKeyMax)
    std::cout << "Source run/LB range [" << (m_runemin >> 32) << "," <<
      (m_runemin & 0xFFFFFFFF) << "] to [" << (m_runemax >> 32) << "," <<
      (m_runemax & 0xFFFFFFFF) << "]" << std::endl;
  if (m_timemin!=cool::ValidityKeyMin || m_timemax!=cool::ValidityKeyMax)
    std::cout << "Source timestamp range " << m_timemin << " " <<
      timeString(m_timemin) << "to " << m_timemax << " " << 
      timeString(m_timemax) << std::endl;
  if (m_alliov)
    std::cout << "Change o/p run/LB range [" << (m_newrunemin >> 32) << "," <<
      (m_newrunemin & 0xFFFFFFFF) << "] to [" << (m_newrunemax >> 32) << "," <<
      (m_newrunemax & 0xFFFFFFFF) << "]" << std::endl;
  if (m_alliov) 
    std::cout << "Change o/p timestamp range " << m_newtimemin << " " <<
      timeString(m_newtimemin) << "to " << m_newtimemax << " " << 
      timeString(m_newtimemax) << std::endl;
  if (m_skipout) {
    std::cout << "Skip IOVs extending outside run/LB range [" << 
      (m_srunemin >> 32) << "," << (m_srunemin & 0xFFFFFFFF) << "] to [" <<
      (m_srunemax >> 32) << "," << (m_srunemax & 0xFFFFFFFF) << "]" << 
      std::endl;
  }
  if (!m_excludechans.empty()) {
    for (std::vector<cool::ChannelId>::const_iterator itr=
	   m_excludechans.begin();itr!=m_excludechans.end();++itr) 
      std::cout << "Channel " << *itr << " will be excluded" << std::endl;
  }
  if (m_hitag) std::cout << "All hierachical tags connecting to referenced tags will be copied" << std::endl;
  if (m_nohitag) std::cout << "Hierarchical tag relations will not be copied"
			   << std::endl;
  return 0;
}

bool AtlCoolCopy::isNumeric(const char* input) {
  // determine if input string is numeric or string
  bool isnum=true;
  const char* cptr=input;
  while (*cptr!='\0') {
    if (!isdigit(*cptr)) isnum=false;
    ++cptr;
  }
  return isnum;
}

bool AtlCoolCopy::equalRecord(const cool::IRecord& lhs,
			      const cool::IRecord& rhs) {
  // equality test for COOL IRecords, handling NULL string attributes correctly
  // tests values and types of Attributes, not names
  if (lhs.size()!=rhs.size()) return false;
  for (size_t i=0;i<lhs.size();++i) {
    // types must match - have to test explicitly first
    if (lhs[i].specification().storageType()!=
	rhs[i].specification().storageType())
      return false;
    // now use IField equality to for final test (spec again and data)
    if (lhs[i]!=rhs[i]) {
      // if not equal, check for special case of strings and compare directly
      // types are known to be equal so only test LHS
      const cool::StorageType& stype=lhs[i].specification().storageType();
      if (stype==cool::StorageType::String255 || 
	  stype==cool::StorageType::String4k ||
	  stype==cool::StorageType::String64k ||
	  stype==cool::StorageType::String16M) {
	// check if string data payloads are really equal or not
	if (lhs[i].data<std::string>()!=rhs[i].data<std::string>()) 
	  return false;
      } else {
	// if not string, trust the result of IField !=operator
	return false;
      }
    }
  }
  return true;
}


cool::ValidityKey AtlCoolCopy::timeVal(const char* input) {
  // convert input char* string to 64bit COOL validityKey
  // input either represents a string in seconds, or a date in the form
  // yyyy/mm/dd:hh:mm:ss
  // first determine if input is a number
  if (isNumeric(input)) {
    return static_cast<long long>(atol(input))*
      static_cast<long long>(1.E9);
  } else {
    struct tm mytm,mytm2;
    char* result=strptime(input,"%Y-%m-%d:%T",&mytm);
    if (result!=0) {
      // make the DST field zero, so the time is interpreted without daylight
      // savings time
      mytm.tm_isdst=0;
      // now have to correct for the local time zone - do this by also 
      // calculating the time since epoch for 2/1/1970 midnight (no DST)
      cool::ValidityKey itime=static_cast<cool::ValidityKey>(mktime(&mytm));
      strptime("1970-01-02:00:00:00","%Y-%m-%d:%T",&mytm2);
      cool::ValidityKey caltime=static_cast<cool::ValidityKey>(mktime(&mytm2));
      itime+=24*60*60-caltime;
      return itime*static_cast<cool::ValidityKey>(1.E9);
    } else {
      std::cout << 
	"ERROR in format of time value, use e.g. 2007-05-25:14:01:00" <<
	std::endl;
      return 0;
    }
  }
}

std::string AtlCoolCopy::timeString(const cool::ValidityKey iovtime) {
  if (iovtime==cool::ValidityKeyMin) {
    return "ValidityKeyMin ";
  } else if (iovtime==cool::ValidityKeyMax) {
    return "ValidityKeyMax ";
  } else {
    time_t time=static_cast<time_t>(iovtime/1E9);
    return "UTC "+std::string(asctime(gmtime(&time)));
  }
}

cool::ValidityKey AtlCoolCopy::runLBVal(const char* input1,const char* input2) {
  // parse the inputs as run/LB numbers to generate a validitykey
  // used in processing -rls-type options
  cool::ValidityKey val;
  if (input1[0]=='M' || input1[0]=='m') {
    val=((1LL << 31)-1) << 32;
  } else {
    val=static_cast<long long>(atol(input1)) << 32;
  }
  if (input2[0]=='M' || input2[0]=='m') {
    val+=(1LL << 32);
  } else {
    val+=atoll(input2);
  }
  if (val>cool::ValidityKeyMax) val=cool::ValidityKeyMax;
  return val;
}


bool AtlCoolCopy::getTimeFromRun() {
  // extract time limits for a run range using the expected information
  // in the /TDAQ/RunCtrl/SOR_Params and EOR_Params on the given DB connection

  // choose database instance based on run number switchover if none is given
  if (m_timedb=="") {
    if ((m_runemin >> 32)>=236107) {
      m_timedb="COOLONL_TDAQ/CONDBR2";
    } else {
      m_timedb="COOLONL_TDAQ/COMP200";
    }
  }
  // select foldername based on database instance
  std::string sorfolder="/TDAQ/RunCtrl/SOR_Params";
  std::string eorfolder="/TDAQ/RunCtrl/EOR_Params";
  if (m_timedb.find ("CONDBR2")!=std::string::npos) {
    sorfolder="/TDAQ/RunCtrl/SOR";
    eorfolder="/TDAQ/RunCtrl/EOR";
  }
  std::cout << "Extracting times for run range [" << (m_runemin >> 32) <<
    "," << (m_runemax >> 32) << "] from database " << m_timedb << 
    " folder " << sorfolder << std::endl;
  if (m_runemin==cool::ValidityKeyMin || m_runemax==cool::ValidityKeyMax) {
    std::cout << "ERROR: Run range not correctly specified" << std::endl;
    return false;
  }
  // open database connection
  cool::IDatabasePtr tdaqdb;
  try {
    tdaqdb=m_dbSvc->openDatabase(transConn(m_timedb),m_sourceread);
    std::cout << "Opened database connection" << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "COOL exception caught: " << e.what() << std::endl;
    return false;
  }
  // start of run information
  try {
    cool::IFolderPtr folder=tdaqdb->getFolder(sorfolder);
    // channel number is arbitrary - have to loop
    cool::IObjectIteratorPtr itr=folder->browseObjects(m_runemin,m_runemin,
			cool::ChannelSelection::all());
    int nobj=0;
    while (itr->goToNext()) {
      const cool::IRecord& payload=itr->currentRef().payload();
      m_timemin=payload["SORTime"].data<unsigned long long>();
      ++nobj;
    }
    itr->close();
    if (nobj!=1) {
      std::cout << "ERROR: Found " << nobj << " SOR records" << std::endl;
      return false;
    }
  }
  catch (std::exception& e) {
    std::cout << "Exception accessing SOR information: " << e.what() << 
      std::endl;
    return false;
  }
  // end of run information
  try {
    cool::IFolderPtr folder=tdaqdb->getFolder(eorfolder);
    // channel number is arbitrary - have to loop
    cool::IObjectIteratorPtr itr=folder->browseObjects(((m_runemax >> 32) << 32),((m_runemax >> 32) << 32),
			cool::ChannelSelection::all());
    int nobj=0;
    while (itr->goToNext()) {
      const cool::IRecord& payload=itr->currentRef().payload();
      m_timemax=payload["EORTime"].data<unsigned long long>();
      ++nobj;
    }
    itr->close();
    if (nobj!=1) {
      std::cout << "ERROR: Found " << nobj << " EOR records" << std::endl;
      return false;
    }
  }
  catch (std::exception& e) {
    std::cout << "Exception accessing SOR information: " << e.what() << 
      std::endl;
    return false;
  }
  tdaqdb->closeDatabase();
  std::cout << "Timestamp range set to " << m_timemin << " " <<
      timeString(m_timemin) << "to " << m_timemax << " " << 
      timeString(m_timemax) << std::endl;
  return true;
}

bool AtlCoolCopy::getOnlineRun() {
  // open using the ATLAS_COOLONL_GLOBAL schema, since we have authentication
  // information for this one by default
  std::string connstr;
  if (m_onlinerun) {
    connstr="oracle://ATONR_COOL/ATLAS_COOLONL_GLOBAL";
    std::cout << "Extracting current run-number from online ATONR ... " << 
      std::endl;
  } else {
    connstr="oracle://ATLAS_COOLPROD/ATLAS_COOLONL_GLOBAL";
    std::cout << "Extracting current run-number from replica ATLR ... " << 
      std::endl;
  }
  coral::ISessionProxy* proxy=m_coralsvc.connect(connstr,coral::ReadOnly);
  if (proxy==0) {
    std::cout << "Could not connect to " << connstr << std::endl;
    return false;
  }
  try {
    proxy->transaction().start(true);
    // access table in the ATLAS_RUN_NUMBER schema (GRANTs have been done)
    coral::ITable& table=
      proxy->schema("ATLAS_RUN_NUMBER").tableHandle("RUNNUMBER");
    coral::IQuery* query=table.newQuery();
    query->setRowCacheSize(1);
    query->addToOutputList("MAX(RUNNUMBER)","res");
    query->defineOutputType("res","int");
    coral::ICursor& cursor=query->execute();
    if (cursor.next()) {
      const coral::AttributeList& res=cursor.currentRow();
      int nextrun=res["res"].data<int>()+1;
      std::cout << "Next run started will be " << nextrun << std::endl;
      const long long rtime=time(0);
      std::cout << "Epoch time extracted " << rtime << std::endl;
      if (m_alliov) {
	// if overwriting IOVs, set the new IOV lower limit
	m_newrunemin=(static_cast<long long>(nextrun)) << 32;
	m_newtimemin=rtime*static_cast<long long>(1E9);
      } else {
	// set the query lower limit - for use with truncate option
        m_runemin=(static_cast<long long>(nextrun)) << 32;
	m_timemin=rtime*static_cast<long long>(1E9);
	m_truncate=true;
      }
    } else {
      std::cout << "Query returned no data" << std::endl;
      return false;
    }
  }
  catch (coral::Exception& e) {
    std::cout << "Exception when querying for run number: " << e.what()
	      << std::endl;
    return false;
  }
  return true;
}

bool AtlCoolCopy::getBulkRun() {
  // get minimum run-number/timestamp for bulk reco update
  if (m_getonline) {
    std::cout << "ERROR: -getonline and -getbulk cannot be used simultaneously"
	      << std::endl;
    return false;
  }
  const std::string connstr="oracle://ATLAS_COOLPROD/ATLAS_COOLONL_GLOBAL";
  std::cout << "Extracting bulk-reco run limit from ATLR ... " << std::endl;
  coral::ISessionProxy* proxy=m_coralsvc.connect(connstr,coral::ReadOnly);
  if (proxy==0) {
    std::cout << "Could not connect to " << connstr << std::endl;
    return false;
  }
  try {
    proxy->transaction().start(true);
    // access table in the ATLAS_COOL_GLOBAL schema (GRANTs have been done)
    coral::ITable& table=
      proxy->schema("ATLAS_COOL_GLOBAL").tableHandle("NEMOP_SYNC");
    coral::IQuery* query=table.newQuery();
    query->setRowCacheSize(1);
    query->addToOutputList("RUN","run");
    query->defineOutputType("run","int");
    query->addToOutputList("TIMESTAMP","timestamp");
    query->defineOutputType("timestamp","int");
    coral::AttributeList bindvar;
    bindvar.extend<int>("ID");
    bindvar[0].data<int>()=1;
    query->setCondition("ID=:ID",bindvar);
    coral::ICursor& cursor=query->execute();
    if (cursor.next()) {
      const coral::AttributeList& res=cursor.currentRow();
      const int nextrun=res["run"].data<int>();
      std::cout << "Next run started will be " << nextrun << std::endl;
      long long rtime=res["timestamp"].data<int>();
      std::cout << "Epoch time extracted " << rtime << std::endl;
      if (m_alliov) {
	// if overwriting IOVs, set the new IOV lower limit
	m_newrunemin=(static_cast<long long>(nextrun)) << 32;
	m_newtimemin=rtime*static_cast<long long>(1E9);
      } else {
	// set the query lower limit - for use with truncate option
        m_runemin=(static_cast<long long>(nextrun)) << 32;
	m_timemin=rtime*static_cast<long long>(1E9);
	m_truncate=true;
      }
    } else {
      std::cout << "Query returned no data" << std::endl;
      return false;
    }
  }
  catch (coral::Exception& e) {
    std::cout << "Exception when querying for run number: " << e.what()
	      << std::endl;
    return false;
  }
  return true;
}

bool AtlCoolCopy::getRunList() {
  // loop over all given filenames
  for (std::vector<std::string>::const_iterator itr=m_runfile.begin();
       itr!=m_runfile.end();++itr) {
    std::cout << "Reading allowed run numbers from file " << *itr << std::endl;
    FILE* p_inp=fopen(itr->c_str(),"r");
    if (p_inp==0) {
      std::cout << "File not found" << std::endl;
      return false;
    }
    char* p_buf=new char[999];
    while (!feof(p_inp)) {
      char* p_line=fgets(p_buf,999,p_inp);
      if (p_line!=NULL) {
	unsigned int run=atoi(p_line);
	m_runlist.push_back(run);
      }
    }
    fclose(p_inp);
    delete[] p_buf;
  }
  std::sort(m_runlist.begin(),m_runlist.end());
  std::cout << "Read list of " << m_runlist.size() << " runs from " <<
    m_runfile.size() << " input runlist files" << std::endl;
  for (std::vector<unsigned int>::const_iterator itr=m_runlist.begin();
       itr!=m_runlist.end();++itr) std::cout << 
	 "Update allowed for run " << *itr << std::endl;
  return true;
}

int AtlCoolCopy::getUpdateMode(const std::string& desc, 
			       const std::string& tag) {
  // analyse the folder description and tag name for updateMode flags
  // return 1 for online mode, 0 otherwise
  std::string modestr="";
  int mode=0;
  std::string::size_type iofs1=desc.find("<updateMode>");
  std::string::size_type iofs2=desc.find("</updateMode>");
  if (iofs1!=std::string::npos && iofs2!=std::string::npos && iofs2>iofs1)
    modestr=desc.substr(iofs1+12,iofs2-iofs1-12);
  if (modestr=="UPD1" || tag.find("UPD1")!=std::string::npos) mode=1;
  if (modestr=="UPD2" || tag.find("UPD2")!=std::string::npos) mode=2;
  if (modestr=="UPD3" || tag.find("UPD3")!=std::string::npos) mode=3;
  if (modestr=="UPD4" || tag.find("UPD4")!=std::string::npos) mode=4;
  return mode;
}


bool AtlCoolCopy::checkChannels(const std::string& folder,
	     cool::IFolderPtr sourcefl,cool::IFolderPtr destfl,
				bool newfolder) {
  // Channel manipulation is slow due to lack of bulk API for creating them
  // this routine therefore does the minimum work, not checking destination
  // if folder is newly created, and not copying description unless requested
  // get map of all channels
  const std::map<cool::ChannelId,std::string> chanmap=
    sourcefl->listChannelsWithNames();
  if (m_debug) std::cout << "Checking channels table for folder " << folder 
	<< " with " << chanmap.size() << " channels" << std::endl;
  int ncreate=0;
  int nmodify=0;
  // iterate over all channels
  for (std::map<cool::ChannelId,std::string>::const_iterator 
	 itr=chanmap.begin();itr!=chanmap.end();++itr) {
    // assume now ALL channels are in channels table (COOL 2.4 and above)
    const cool::ChannelId chan=itr->first;
    // check if channel should be skipped
    if (!m_excludechans.empty()) {
      if (find(m_excludechans.begin(),m_excludechans.end(),
	       chan)!=m_excludechans.end()) continue;
    }
    if (!m_chansel.inSelection(chan)) continue;

    const std::string& sourcename=itr->second;
    // only look up channel description if we are copying it
    std::string sourcedesc="";
    if (m_chdesc) sourcedesc=sourcefl->channelDescription(chan);
    // only check destination if folder alreay existed
    if (!newfolder && destfl->existsChannel(chan)) {
      const std::string & destname=destfl->channelName(chan);
      std::string destdesc="";
      if (m_chdesc) destdesc=destfl->channelDescription(chan);
      // if sourcename is not set, and destination is, dont worry
      if (sourcename!=destname && sourcename!="") {
        // channel names differ - error if verifying, set if copying
        if (m_verify) {
          std::cout << "ERROR: Channel " << chan << " names differ: " << 
            sourcename << " " << destname << std::endl;
          return false;
        }
	destfl->setChannelName(chan,sourcename);
	std::cout << "Modify channel " << chan << " name from "
		    << destname << " to " << sourcename << std::endl;
 	++nmodify;
      }
      if (sourcedesc!=destdesc && sourcedesc!="") {
        // channel descriptions differ - error if verifying, set if copying
        if (m_verify) {
          std::cout << "ERROR: Channel " << chan << " descriptions differ: " 
	            << sourcedesc << " " << destdesc << std::endl;
	  return false;
	}
        std::cout << "Modify channel " << chan << " description from "
      	    << sourcedesc << " to " << destdesc << std::endl;
	destfl->setChannelDescription(chan,sourcedesc);
	++nmodify;
      }
    } else {
      // channel does not exist on destination - if verifying, this is error
      if (m_verify) return false;
      // otherwise create it
      if (m_debug) std::cout << "Create new channel " << chan << " name " 
            << sourcename << " description" << sourcedesc << std::endl;
      destfl->createChannel(chan,sourcename,sourcedesc);
      ++ncreate;
    }
  }
  if (ncreate>0) std::cout << "Created " << ncreate << " new channels for "
			   << folder << std::endl;
  if (nmodify>0) std::cout << "Modified " << nmodify << " channel info for "
			   << folder << std::endl;
  return true;
}


void AtlCoolCopy::checkRef(const cool::IRecord& payload,
			const std::string& folder, const std::string& tag) {
  const std::string name=payload[0].name();
  if (name=="PoolRef" || name=="fileGUID") {
    std::string poolref;
    std::string addr=payload[0].data<std::string>();
    if (name=="PoolRef") {
      std::string::size_type iofs1=addr.find("[DB=");
      std::string::size_type iofs2=addr.find("]",iofs1);
      if (iofs1!=std::string::npos && iofs2!=std::string::npos && iofs2>iofs1) {
        poolref=addr.substr(iofs1+4,iofs2-iofs1-4);
      } else {
      std::cout << "ERROR: Badly formed POOL object reference " <<
	addr << std::endl;
      }
    } else {
      // for fileGUID, POOL GUID is just the string
      poolref=addr;
    }
    std::string foldertag=folder+":"+tag;
    // insert into list, first check if same as before
    if (m_poollast!=m_poolrefs.end() && m_poollast->first==poolref) {
      m_poollast->second.inccount();
      m_poollast->second.addfoldertag(foldertag);
    } else {
      m_poollast=m_poolrefs.find(poolref);
      if (m_poollast!=m_poolrefs.end()) {
        m_poollast->second.inccount();
        m_poollast->second.addfoldertag(foldertag);
      } else {
        m_poolrefs[poolref]=PoolMapElement(1,foldertag);
        m_poollast=m_poolrefs.find(poolref);
      }
    }
  }
}

int AtlCoolCopy::tagParents() {
  std::cout << "Copying additional hierarchical tags of parent folders"
	    << std::endl;
  while (!m_hiparent.empty()) {
    // take first element and erase it
    std::string folderset=*m_hiparent.begin();
    m_hiparent.erase(m_hiparent.begin());
    // process any tags defined for this folderset
    if (m_sourceDbPtr->existsFolderSet(folderset)) {
      cool::IFolderSetPtr sourcefl=m_sourceDbPtr->getFolderSet(folderset);
      cool::IFolderSetPtr destfl=m_destDbPtr->getFolderSet(folderset);
      std::cout << "Processing folderset " << folderset << std::endl;
      for (HiTagMap::const_iterator imap=m_hitagmap.begin();
	   imap!=m_hitagmap.end();++imap) {
	try {
	  // will throw exception if this tag is not pointing to the folder
	  std::string sourcetag=sourcefl->findTagRelation(imap->first);
	  std::cout << "Create hierarchical tag between " << sourcetag <<
		" and " << imap->first << " in folder " << imap->second <<
		std::endl;
	  try {
	    // first check this relation has not already been created
	    std::string etag=destfl->resolveTag(imap->first);
	    if (etag==sourcetag) {
	      std::cout << "This relation has already been created" << std::endl;
	    } else {
	      std::cout << "ERROR: Tag in parent already related to " << 
		  etag << std::endl;
	      return 34;
	    }
	  }
	  catch (cool::Exception& e ) {
            // only do creation if does not already exist - via exception
	    try {
	      destfl->createTagRelation(imap->first,sourcetag);
	      // check if this implicates yet another parent folder to be checked
	      if (find(m_hiparent.begin(),m_hiparent.end(),imap->second)==
		      m_hiparent.end()) m_hiparent.push_back(imap->second);
	      if (m_copytaginfo && 
		m_cooltagmap.find(imap->first)==m_cooltagmap.end())
		  m_cooltagmap.insert(CoolTagMap::value_type(imap->first,
			 CoolTagInfo(m_sourceDbPtr,imap->second,imap->second,
				     imap->first,imap->first)));
	    }
	    catch (cool::Exception& e) {
	      std::cout << "Cool exception " << e.what() << 
	        "thrown in hierarchical tag creation" << std::endl;
	      return 34;
	    }
	  }
	}
	// exceptions from findTag - do nothing
	catch (cool::Exception& e) { }
      }
    } else {
      std::cout << "ERROR Folderset " << folderset << " not found" << 
	std::endl;
    }
  }
  return 0;
}

int AtlCoolCopy::writeTagInfo() {
  std::cout << "Write tag description ";
  if (m_copytaglock) std::cout << "and lock info ";
  std::cout << "for " << m_cooltagmap.size() << " tags" << std::endl;
  for (CoolTagMap::const_iterator itr=m_cooltagmap.begin();
       itr!=m_cooltagmap.end();++itr) {
    itr->second.write(m_destDbPtr,m_copytaglock);
  }
  return 0;
}

int AtlCoolCopy::listPoolRefs() {
  std::cout << "Total of " << m_poolrefs.size() << " POOL files referenced"
	    << std::endl;
  for (PoolMap::const_iterator ipool=m_poolrefs.begin();
       ipool!=m_poolrefs.end();++ipool) {
    std::cout << "Ref " << ipool->first << " (" << ipool->second.count() 
	      << ")" << std::endl;
  }
  return 0;
}

int AtlCoolCopy::resolvePoolRefs() {
  std::cout << "Total of " << m_poolrefs.size() << " POOL Files referenced"
	    << std::endl;
  pool::IFileCatalog* catalog=setupCatalog(m_poolcat);
  if (catalog==0) return 110;
  pool::SimpleUtilityBase       pool_utility;
  if (m_poolopen) {
     // prepare POOL session
     pool_utility.startSession();
  }
  pool::FClookup lookup;
  catalog->setAction(lookup);

  // inject additional GUIDs/LFNs if needed
  if (m_addguid.size()>0) {
    for (std::vector<std::string>::const_iterator itr=m_addguid.begin();
	 itr!=m_addguid.end();++itr) {
      m_poolrefs[*itr]=PoolMapElement(1,"ADDGUID");
      std::cout << "Added POOL file GUID: " << *itr << std::endl;
    }
  }
  if (m_addlfn.size()>0) {
    for (std::vector<std::string>::const_iterator itr=m_addlfn.begin();
	 itr!=m_addlfn.end();++itr) {
      std::string guid;
      lookup.lookupFileByLFN(*itr,guid);
      std::cout << "Add POOL file GUID: " << guid << " from LFN " << *itr
		<< std::endl;
      m_poolrefs[guid]=PoolMapElement(1,"ADDLFN");
    }
  }

  // set up for making new dataset if needed
  typedef std::vector<std::pair<std::string,std::string> > LFNGVec;
  LFNGVec dsfound;
  bool dscopy=!m_newdataset.empty();

  // loop through GUIDs and attempt to resolve
  int nbad=0;
  for (PoolMap::iterator ipool=m_poolrefs.begin();
       ipool!=m_poolrefs.end();++ipool) {
    pool::FileCatalog::FileID guid=ipool->first;
    pool::LFNContainer mylfn(catalog,100);
    lookup.lookupLFN(guid,mylfn);
    if (mylfn.hasNext()) {
      // file found in cataloge - print LFN and usage count
      const std::string lfn=mylfn.Next().lfname();
      ipool->second.setlfn(lfn);
      std::cout << "LFN: " << lfn << " (" << ipool->second.count() 
		<< ")" << std::endl;
      if (dscopy) 
	dsfound.push_back(std::pair<std::string,std::string>(lfn,guid));
    } else {
      // error bit 0 - no LFN
      if (!m_listpfn) ipool->second.setErrorBit(0);
    }
    if (m_listpfn || m_poolopen) {
      pool::PFNContainer mypfn(catalog,100);
      lookup.lookupPFN(guid,mypfn);
      if (mypfn.hasNext()) {
        const std::string pfn=mypfn.Next().pfname();
	ipool->second.setpfn(pfn);
        std::cout << "PFN: " << pfn << " (" << ipool->second.count() 
		<< ")" << std::endl;
	if (m_poolopen) {
	  // first try the file as a CoolHist file
	  std::string hguid=getCoolHistGUID(pfn);
	  if (hguid!="") {
	    // successful get of CoolHist GUID
	    if (hguid!=ipool->first) {
	      std::cout << "ERROR File CoolHist GUID " << hguid << 
		" inconsistent with catalogue " << ipool->first << std::endl;
	      ipool->second.setErrorBit(3);
	    }
	  } else {
             // try the file as a genuine POOL file
             try {
                const std::string fid = pool_utility.readFileGUID( pfn );
                if( fid != ipool->first ) {
                   std::cout << "ERROR File GUID " << fid << 
                      " inconsistent with catalogue " << ipool->first << std::endl;
                   // file GUID inconsistent with catalogue - error bit 3
                   ipool->second.setErrorBit(3);
                }
             }
             catch( std::runtime_error& e ) {
                std::cout << "Cannot open file for reading!" << std::endl;
                std::cout << e.what() << std::endl;
                // File cannot be opened: set error bit 2
                ipool->second.setErrorBit(2);
             }
	  }
	} // end of actions opening POOL file
      } else {
	// PFN not found - set bit 1
	ipool->second.setErrorBit(1);
      }
    }
    // check file error code
    if (ipool->second.errcode()>0) ++nbad;
  }
  catalog->commit();
  catalog->disconnect();
  delete catalog;

  // produce definition of new dataset if needed
  if (dscopy) {
    // start up a new catalogue instance with the merge catalogues
    // which indicate which files are already available and don't need to 
    // put in the output dataset definition
    catalog=setupCatalog(m_mergecat);
    if (catalog==0) return 110;
    pool::FClookup lookup;
    catalog->setAction(lookup);
    const std::string dssname="register.sh";
    std::cout << "Write DQ2 registerFileInDataset commands to " << dssname
	      << " for registration in dataset " << m_newdataset << std::endl;
    std::ofstream dsstream(dssname.c_str());
    for (LFNGVec::const_iterator itr=dsfound.begin();
     itr!=dsfound.end();++itr) {
      const std::string& lfn=itr->first;
      const std::string& guid=itr->second;
      pool::FileCatalog::FileID pguid=guid;
      pool::LFNContainer mylfn(catalog,100);
      lookup.lookupLFN(pguid,mylfn);
      if (mylfn.hasNext()) {
	// file is already registered - check logical names are consistent
        const std::string lfn2=mylfn.Next().lfname();
	if (lfn2!=lfn) std::cout << "WARNING: LFNs for GUID " << guid << 
			 " differ in input/merge datasets: " << lfn << " vs " 
				 << lfn2 << std::endl;
      } else {
	// file is not registered - add to output
	dsstream << "dq2-register-files " << m_newdataset << " " 
		    << lfn << " " << guid << std::endl;
      }
    }
    catalog->commit();
    catalog->disconnect();
    delete catalog;
  }

  int retcode=0;
  if (nbad==0) {
    std::cout << "All POOL references were resolved by catalogues" 
	      << std::endl;
  } else {
    std::cout <<  "ERROR Could not resolve " << nbad << " files" << std::endl;
    retcode=111;
  }
  if (nbad>0 || m_poolall) filePoolRefs();
  return retcode;
}

std::string AtlCoolCopy::getCoolHistGUID(const std::string& file) {
  // attempt to extract COOL Hist GUID from file
  std::string hguid="";
  TFile* myfile=TFile::Open(file.c_str(),"READ");
  if (myfile!=0) {
    TObjString* oguid;
    myfile->GetObject("fileGUID",oguid);
    if (oguid!=0) {
      hguid=oguid->GetString();
      std::cout << "CoolHist GUID found to be " << hguid << std::endl;
    }
    myfile->Close();
  }
  return hguid;
}

void AtlCoolCopy::filePoolRefs() {
  // list POOL refs and error codes, optionally to file
  if (m_checkoutputfile!="") {
    std::cout << "Write POOL file checkoutput on " << m_checkoutputfile << 
      std::endl;
  } else {
    m_checkoutputfile="/dev/null";
  }
  std::ofstream chkostream(m_checkoutputfile.c_str());
  std::cout << "ErrCode GUID Count LFN PFN Folders ..." << std::endl;
  for (PoolMap::const_iterator ipool=m_poolrefs.begin();
       ipool!=m_poolrefs.end();++ipool) {
    if (m_poolall || ipool->second.errcode()>0) {
      std::ostringstream line;
      // write error code, GUID, usage count 
      line << ipool->second.errcode() << " " << ipool->first << " " << 
        ipool->second.count() << " ";
      if (ipool->second.lfn()!="") {
        line << ipool->second.lfn() << " ";
      } else {
        line << "noLFN ";
      }
      if (ipool->second.pfn()!="") {
	line << ipool->second.pfn() << " ";
      } else {
        line << "noPFN ";
      }
      for (std::vector<std::string>::const_iterator 
           itr=ipool->second.foldertag().begin();
         itr!=ipool->second.foldertag().end();++itr)
        line << *itr << " ";
      std::cout << line.str() << std::endl;
      chkostream << line.str()  << std::endl;
    }
  }
}

pool::IFileCatalog* AtlCoolCopy::setupCatalog(
		    const std::vector<std::string>& catvec) {
  pool::IFileCatalog* catalog=new pool::IFileCatalog;
  try {
    catalog->setWriteCatalog("file:PoolFileCatalog.xml");
    for (std::vector<std::string>::const_iterator icat=catvec.begin();
	 icat!=catvec.end();++icat) {
      std::cout << "Add catalogue: " << *icat << std::endl;
      // if catalogue contains no ":" specifier, assume plain file
      if (icat->find(":")==std::string::npos) {
        catalog->addReadCatalog("file:"+(*icat));
      } else {
        catalog->addReadCatalog(*icat);
      }
    }
    catalog->connect();
    catalog->start();
    return catalog;
  }
  catch (std::exception& e) {
    std::cout << "Could not setup POOL catalogues, exception:" << e.what() 
	      << std::endl;
    return 0;
  }
}

void printHelp() {
  std::cout << "usage: AtlCoolCopy.exe <sourceCoolDB> <destinationCoolDB> { <options> }" << std::endl;
  std::cout << "Options are (see doc/mainpage.h for more):" << std::endl <<
    "-a, -alliov : set IOVs on destination to [ValidityKeyMin, ValidityKeyMax]"   << std::endl <<" or following settings of -nrls,-nrlu,-nts,-ntu" << 
    std::endl <<
    "-ag, -addguid <guid> : Add GUID to list accessed for POOL checks" << 
    std::endl <<
    "-alf, -addlfn <LFN> : Add LFN to list accessed for POOL checks" << 
    std::endl <<
    "-al, -appendlocked : Allow locked tags to be updated if no overlap" << 
    std::endl << 
    "-alsv, -appendlockedsv : Allow locked tag update for openended IOVs" << 
    std::endl << 
    "-ana, -analyse <delta_t>: produce analysis ROOT file (filename = dest DB argument)" << std::endl << 
"-bs, -buffersize <size> : set size of bulkstorage output buf to <size> objs"
	    << std::endl << 
    "-c, -create : create destination DB if not already existing" 
	    << std::endl <<
    "-cd, -checkdest : Check destination DB and skip already existing tags"
	    << std::endl <<
    "-ch, -channel : restrict selection to given channel or range specified as c1:c2" << std::endl <<
    "-ch1, -channel1 : specify start of a range of channels" << std::endl <<
    "-ch2, -channel2 : specify end of a range of channels" << std::endl <<
    "-cf, -checkfiles : check POOL files can be opened and have correct GUID" 
	    << std::endl << 
    "-co, -checkoutput <file> : write POOL file check output on file"
	    << std::endl << "-cti, -copytaginfo : Copy tag descriptions"
	    << std::endl << "-ctl, -copytaglock : Copy tag locked status and descriptions"
	    << std::endl << "-cr, -checkrefs : check POOL references"
	    << std::endl << "-d, -debug : produce debug output" << std::endl;
  std::cout <<
    "-ds, -dataset : output register.sh for creating DQ2 datasets " 
	    << std::endl <<
    "-ec, -excludechannel : exclude given channel from copy" << std::endl <<
    "-eh, -excludehead : exclude HEAD tag even if no tags found in MV folders" 
	    <<  std::endl <<
    "-ih, -includehead : include HEAD as well as any tags in MV folders" << 
    std::endl <<
    "-e, -exclude <pattern> : exclude folders" << std::endl <<
    "-f, -folder <pattern> : include folders" << std::endl <<
    "-fs, -forcesingle : force destination folder to be singleversion" 
	    << std::endl <<
    "-fm, -forcemulti : force destination folder to be multiversion" 
	    << std::endl <<
    "-frl, -forcerunlumi : force destination folder to be run/LB indexed" 
	    << std::endl <<
    "-ftm, -forcetime : force destination folder to be timestamp indexed" 
	    << std::endl <<
    "-fp, -forcepayload : Force destn folders to be created with payload tbl" << std::endl <<
    "-fnp, -forcenopayload : Force destn folders to be created without payload tbl" << std::endl <<
    "-forcerecreate : delete and recreate destination database" << std::endl;
  std::cout << "-ht, -headtag : Use HEAD-style tagging"
	    << std::endl <<
   "-go, -getonline : Set minimum run number (-rls setting) to next ONLINE run"
	    << std::endl <<
 "-gb, -getbulk :  Set minimum run number (-rls setting) to next bulk reco run"
	    << std::endl <<
    "-gt, -gettime : Extract timestamp information for given run number range" 
	    << std::endl <<
    "-h, -help : print help and exit" << std::endl << 
    "-hi, -hitag : Copy hierrchical tags inclusively" << std::endl << 
    "-ignoremode <pwd> : Ignore UPDx mode protection (experts only)" 
	    << std::endl <<
    "-is, -ignorespec : Ignore differences in folder spec if names are all equal" << std::endl <<
    "-lo, -lockedonly : Only copy locked/partially-locked top-level tags"
	    << std::endl <<
    "-nc, -nocopy : Do not actually copy, just read source DB" << std::endl <<
    "-nch, -nochannel : Do not check or copy channel information" << std::endl 
<<  "-ncr, -noclobroot: Do not copy CLOB data into ROOT files" << std::endl
<< "-noc, -nocoracool : Do not copy Coracool structure payloads " << std::endl <<
    "-nd, -nodata : Copy only folder structures, not data" << std::endl <<
    "-nh, -nohitag : Do not follow hierarchical tag relations" << std::endl <<
"-mc, -mergecat <catfile> : specify POOL file catalogue for new dataset making"
	    << std::endl;
  std::cout << "-of, -outfolder <folder> : rename folder on output" 
	    << std::endl <<
    "-onr, -onlinerun : Retrieve run number from online server (not replica)" <<std::endl <<
    "-ot, -outtag : " << "Rename tag on output" << std::endl;
  std::cout <<
    "-pa, -poolall : Output all POOL files (incl good) when checking files"
	    << std::endl <<
"-pc, -poolcat <catfile> : specify POOL file catalogue for ref checking" 
	    << std::endl <<
    "-pf, -parfile <file> : Read additional options/parameters from file" << 
    std::endl << 
"-pt, -prunetags : Copy only hierarchical tags descending from specified toptags"
	    << std::endl <<
"-rdo, -readoracle : force data to be read from Oracle, using dbreplica.config"
	    << std::endl <<
    "-sl, -seal <val>: Set SEAL (hence COOL/POOL) output level to <val>"
	    << std::endl << 
    "-sr, -skiprep : Skip folders having <norep/> folder metadata tag" <<
    std::endl << 
    "-t, -tag <tag> : Copy multiversion data with tag <tag>" << std::endl <<
    "-mt, -magic <tag> : Include magic tags involving the given pattern " << std::endl << 
"-tr, -truncate : Set destination IOVs outside query interval to query interval"
	    << std::endl <<
"-tl, -taglabel : Specify tag description to enter in destination DB" 
	    << std::endl << 
    "-uht, -userheadtag : Also copy user tag data to the HEAD" << std::endl <<
    "-v, -verify : Verify data present on destination rather than copying" <<
    std::endl << 
    "-ro, -root : Produce ROOT output file instead of copying to COOL"
	    << std::endl <<
    "-zn, -zeronull : Zero NULLs in ROOT file instead of skipping them" << 
    std::endl << 
    "-rls, -runlumisince <run> <LB> : Set minimum IOV interval" << std::endl <<
    "-rlu, -runlumiuntil <run> <LB> : Set maximum IOV interval" << std::endl <<
    "-rs, -runsince <run> : Set minimum IOV interval" << std::endl << 
    "-ru, -rununtil <run> : Set maximum IOV interval" << std::endl <<
    "-r, -run <run> : Copy only run <run>" << std::endl <<
"-srls, -skiprunlumisince <run> <LB> : Set minimum IOV for skipping IOV in copy" 
	    << std::endl <<
"-srlu, -skiprunlumiuntil <run> <LB> : Set maximum IOV for skipping IOV in copy" 
	    << std::endl <<
    "-tdb, -timedb <dbconn> : Set database connection for time information" <<
    "-ts, -timesince <time> : Set minimum IOV interval (UTC SECONDs or UTC YYYY-MM-DD:hh:mm:ss)" << 
    std::endl <<
    "-tu, -timeuntil <time> : Set maximum IOV interval (UTC SECONDs or UTC YYYY-MM-DD:hh:mm:ss)" << 
    std::endl;
  std::cout << 
    "-nrls, -newrunlumisince <run> <LB> : Set minimum of output IOV interval (use with -alliov)" << std::endl <<
    "-nrlu, -newrunlumiuntil <run> <LB> : Set maximum of output IOV interval (use with --aliov)" << std::endl <<
    "-nts, -newtimesince <time> : Set minimum of outputIOV interval (UTC SECONDs or UTC YYYY-MM-DD:hh:mm:ss) (use with --alliov)" << 
    std::endl <<
    "-ntu, -newtimeuntil <time> : Set maximum of output IOV interval (UTC SECONDs or UTC YYYY-MM-DD:hh:mm:ss) (use with --alliov)" << 
    std::endl;
  std::cout << "See http://twiki.cern.ch/twiki/bin/view/Atlas/AtlCoolCopy for more details" << std::endl;
}

int main(int argc, const char* argv[]) {
  int retcode=0;
  if (argc<3) {
    printHelp();
    retcode=1;
  } else {
    AtlCoolCopy mycopy(argv[1],argv[2]);
    retcode=mycopy.setOpts(argc-3,&argv[3]);
    if (retcode==999) {
      printHelp();
      return 0;
    }
    if (retcode==0) retcode=mycopy.doCopy();
  }
  if (retcode>0) std::cout << "ERROR AtlCoolCopy.exe fails with exit code " << 
		   retcode << std::endl;
  return retcode;
}
