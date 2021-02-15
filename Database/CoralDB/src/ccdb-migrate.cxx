/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <limits>
#include <exception>
#include <stdexcept>

#include <errno.h>

#include <sys/stat.h> // mkdir()

// opendir(), readdir(), ...
#include <sys/types.h>
#include <dirent.h>

#include <ctime>

// Suppress a warning from boost.
// (Binaries apparently include boost with -I rather than -isystem.)
#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc11-extensions"
#endif
#include "boost/program_options.hpp"
namespace po = boost::program_options;
#ifdef __clang__
# pragma clang diagnostic pop
#endif

#include "CoralDB/CoralDB.h"
#include "CoralDB/Connection.h"
#include "CoralDB/Alias.h"
#include "CoralDB/CompoundTag.h"
#include "CoralDB/Encodable.h"

using namespace std;
using CoralDB::TagStatus;
using CoralDB::Encodable;
using CoralDB::CompoundTag;

//================================================================
// File names, for use by dump and restore

//const string defaultIdTag("IDTAG-DEFAULT");

const string versionFileName("/CORALDB_DUMP_VERSION");
const string historyFileName("/TAG_HISTORY");

const string objectDictFileName("/ObjectDictionary");

const string idTagDirPrefix("IDTAG-");
const string masterListFilePrefix("MasterList-");
const string connectivityFilePrefix("Connections-");
const string aliasFilePrefix("Aliases-");
const string clobFilePrefix("CLOBs-");


//================================================================
class CCDB_Migrate_Config {
public:
  class HelpMessage: public std::runtime_error {
  public:
    HelpMessage(const std::string& msg) : std::runtime_error(msg) {}
  };

  enum WorkMode { NOT_SET, DUMP, RESTORE };

  CCDB_Migrate_Config()
    : m_mode (NOT_SET),
      m_debug (false),
      m_noSchema (false),
      m_lockAllTags (false),
      m_sloppyAliases (false),
      m_noHistory (false),
      m_noIdTag (false)
  {}
  CCDB_Migrate_Config(int argc, char *argv[]);

  const string& dbConnectionString() const { return m_dbConnectionString; }
  const string& topDir() const { return m_topDir; }
  WorkMode mode() const { return m_mode; }

  const string& onlyIdTag() const { return m_onlyIdTag; }
  const string& onlyCTag() const { return m_onlyCTag; }
  const string& onlyATag() const { return m_onlyATag; }
  const string& onlyDTag() const { return m_onlyDTag; }

  const string& onlyTag() const { return m_onlyTag; }
  bool noIdTag() const { return m_noIdTag; }

  bool  fullIdTag() const {
    return m_onlyIdTag.empty() ||
      (m_onlyCTag.empty() && m_onlyATag.empty() && m_onlyDTag.empty()); 
  }
  
  bool debug() const { return m_debug; }
  bool noSchema() const { return m_noSchema; }
  bool lockAllTags() const { return m_lockAllTags; }
  bool sloppyAliases() const { return m_sloppyAliases; }
  bool noHistory() const { return m_noHistory; }

  static const std::string& short_usage();
  static const std::string long_usage();

private:
  string m_dbConnectionString;
  string m_topDir;
  WorkMode m_mode;

  string m_onlyIdTag;
  string m_onlyCTag;
  string m_onlyATag;
  string m_onlyDTag;

  string m_onlyTag;

  bool m_debug;
  bool m_noSchema;
  bool m_lockAllTags;
  bool m_sloppyAliases;
  bool m_noHistory;

  bool m_noIdTag;

  //----------------
  po::options_description buildDumpOptions();
  po::options_description buildRestoreOptions();
  po::options_description buildOptions();

  void forbidOptions(const po::variables_map& vm,
					  const po::options_description& od,
					  const std::string& allowedModeName);
};


CCDB_Migrate_Config gOpt;

//================================================================
std::ostream& operator<<(std::ostream& os, const CCDB_Migrate_Config& opt) {
  return os<<"config: dbConnectionString="<<opt.dbConnectionString()<<endl
	   <<"config: topDir="<<opt.topDir()<<endl    
	   <<"config: mode="<<opt.mode()<<endl    
	   <<"config: onlyIdTag="<<opt.onlyIdTag()<<endl    
	   <<"config: onlyCTag="<<opt.onlyCTag()<<endl    
	   <<"config: onlyATag="<<opt.onlyATag()<<endl    
	   <<"config: onlyDTag="<<opt.onlyDTag()<<endl    
	   <<"config: debug="<<opt.debug()<<endl    
	   <<"config: noSchema="<<opt.noSchema()<<endl    
	   <<"config: lockAllTags="<<opt.lockAllTags()<<endl    
	   <<"config: sloppyAliases="<<opt.sloppyAliases()<<endl    
	   <<"config: noHistory="<<opt.noHistory()<<endl    
           <<"config: noIdTag="<<opt.noIdTag()<<endl
    ;
}

//================================================================
po::options_description CCDB_Migrate_Config::buildOptions() {

  po::options_description required("Required configuration");
  required.add_options()
    ("db", po::value(&m_dbConnectionString), "db connection string.\n")

    ("dumpTo", po::value(&m_topDir),
     "Specifies the dump mode, mutually exclusive with the --restoreFrom"
     " option.  The argument is the name of a directory to hold a directory"
     " tree structure with the dump data. The directory must not exist, it"
     " will be created by the call.\n")

    ("restoreFrom", po::value(&m_topDir),
     "Mutually exclusive with the --dumpTo option."
     " Restore dump data from the given directory tree produced by"
     " an earlier --dumpTo call.\n"
     )
    ;
  
  po::options_description general("General options");
  general.add_options()
    ("help,h", "Print this message and exit.")    
    ("debug", po::value(&m_debug)->zero_tokens(),  "Be more verbose.")    
    ;
  
  po::options_description cmdline_options;
  cmdline_options
    .add(required)
    .add(general)
    .add(buildDumpOptions())
    .add(buildRestoreOptions())
    ;

  return cmdline_options;
}

//================================================================
po::options_description CCDB_Migrate_Config::buildDumpOptions() {

  po::options_description opts("Dump options");
  opts.add_options()
    ("onlyIdTag", po::value(&m_onlyIdTag),
     "Limit dumping to the given idTag.  If no other --only* options are given,"
     " all connectivity, alias, and data tags for the requested idTag will be dumped.\n")
    
    ("onlyCTag", po::value(&m_onlyCTag),"")
    
    ("onlyATag", po::value(&m_onlyATag), "")
    
    ("onlyDTag", po::value(&m_onlyDTag), ""
     "Limit dumping to the given connectivity, alias, or data tag."
     " An idTag must be provided via the --onlyIdTag option."
     " Only the explicitly requested tag of each kind will be dumped."
     " One does not have to specify all three kinds of tags.")

    ("onlyTag", po::value(&m_onlyTag), ""
     "Limit dumping to the given triplet connectivity, alias, or data tag."
     " An idTag must be provided via the --onlyIdTag option."
     " Mutually exclusive with only*Tag options")
    ;
  
  return opts;
}

//================================================================
po::options_description CCDB_Migrate_Config::buildRestoreOptions() {

  po::options_description restore("Restore options");
  restore.add_options()
    ("noSchema", po::value(&m_noSchema)->zero_tokens(),  "For adding data to an existing DB.\n")    
    ("lockAllTags", po::value(&m_lockAllTags)->zero_tokens(),  "Each restored tag will be locked.\n")    
    ("sloppyAliases", po::value(&m_sloppyAliases)->zero_tokens(),
     "Make unknown object IDs warnings instead of errors when loading aliases.\n")
    ("noHistory", po::value(&m_noHistory)->zero_tokens(),
     "Don't add history table entries.  Useful for restoring from V1 dumps"
     " that were edited to split data into several IDTAGs.\n")
    ("noIdTag", po::value(&m_noIdTag)->zero_tokens(),
     "Don't restore the IDTAG; Useful for adding several dumps to one DB (to use with noSchema option)\n")
    ;
  
  return restore;
}

//================================================================
void CCDB_Migrate_Config::forbidOptions(const po::variables_map& vm,
					const po::options_description& od,
					const std::string& errorMessageBody)
{
  for(unsigned i=0; i<od.options().size(); ++i) {
    if(vm.count(od.options()[i]->long_name())) {
      throw std::runtime_error("Error: option --"+od.options()[i]->long_name()
			       +" "+errorMessageBody);
    }
  }
}
					

//================================================================
CCDB_Migrate_Config::CCDB_Migrate_Config(int argc, char *argv[]) 
  : m_mode(NOT_SET)
  , m_debug(false)
  , m_noSchema(false)
  , m_lockAllTags(false)
  , m_sloppyAliases(false)
  , m_noHistory(false)
  , m_noIdTag(false)
{
  po::variables_map vm;

  try {

    // If there is a help request, we shouldn't try to make sense of of the rest of cmdline
    for(int i=1; i<argc; ++i) {
      if( (std::string("--help") == argv[i]) || (std::string("-h") == argv[i])) {
	throw HelpMessage(long_usage());
      }
    }

    //----------------
    // Go ahead with the real cmdline parsing.

    po::options_description cmdline_options = buildOptions();

    // No non-option arguments: empty description.
    po::positional_options_description nopos;

    po::store(po::command_line_parser(argc, argv)
	      .options(cmdline_options).positional(nopos).run(),
	      vm);

    po::notify(vm);
  }
  catch(po::error& e) {
    throw po::error("Command line error: "+std::string(e.what())+"\n"+short_usage());
  }

  //----------------------------------------------------------------
  // Check that the basic job configuration we got is consistent and complete.

  if( vm.count("dumpTo") && vm.count("restoreFrom") ) {
    throw std::runtime_error("Command line error: options --dumpTo and --restoreFrom"
			     " are mutually exclusive.\n" + short_usage());
  }
  
  m_mode = vm.count("dumpTo") ? DUMP : (vm.count("restoreFrom") ? RESTORE : NOT_SET);
  if(m_mode == NOT_SET) {
    throw runtime_error("Command line error: Do you want to dump or restore?"
			" Mode is not set.\n" + short_usage());
  }

  if(m_dbConnectionString.empty()) {
    throw runtime_error("Command line error: a database must be specified via the --db option.\n"
			+short_usage());
  }


  //----------------------------------------------------------------
  // Check that options are consistent with the major mode (DUMP or RESTORE)
  
  if(m_mode != RESTORE) { // should not have any restore options
    forbidOptions(vm, buildRestoreOptions(), "can only be used in the RESTORE mode.");
  }

  if(m_mode != DUMP) { // should not have any restore options
    forbidOptions(vm, buildDumpOptions(), "can only be used in the DUMP mode.");
  }

  //----------------------------------------------------------------
  // Any of --only*Tag options requires that --onlyIdTag is set
  if(m_onlyIdTag.empty() ) {
    forbidOptions(vm, buildDumpOptions(), "requires that an IDTAG is provided via the --onlyIdTag option.");
  }

}

//================================================================
const std::string& CCDB_Migrate_Config::short_usage() { 
  static const std::string str = 
    "Run \"ccdb-migrate  --help\" for more information.";
  return str;
}

//================================================================
const std::string CCDB_Migrate_Config::long_usage() {
  std::ostringstream os;
  os<<
    "\n" "Usage:"
    "\n"
    "\n" "       ccdb-migrate [options] --dumpTo dirname --db dbconnection"
    "\n" "       ccdb-migrate [options] --restoreFrom dirname --db dbconnection"
    "\n"
    <<CCDB_Migrate_Config().buildOptions()    
    <<
    "\n" "Examples:"
    "\n"
    "\n" "       ./ccdb-migrate  --dumpTo testdump --db sqlite_file:test.db"
    "\n" "       ./ccdb-migrate --restoreFrom testdump  --db sqlite_file:new.db"
    "\n" ;

  return os.str();
}
  
//================================================================
class DumpStructure {
  static const unsigned int m_currentVersion = 2; // the one this program dumps
  unsigned int m_version;
public:
  // Creates a "version" file with the given name containing info
  // about dumps that this program makes
  static void writeVersion(const string& filename);

  // Reads back a "version" file created by the current or an earlier
  // version of this program, an initializes the DumpStructure object accordingly.
  DumpStructure(const string& fileName);

  unsigned int version() const { return m_version; }
  bool tagLockInFileName() const { return (m_version > 1); }
  bool idTagInHistory() const { return (m_version > 1); }
};

std::ostream& operator<<(std::ostream& os, const DumpStructure& d) {
  return os<<"DumpStructure(version = "<<d.version()<<")";
}

DumpStructure::DumpStructure(const string& fileName) 
  : m_version(-1U) 
{
  ifstream file(fileName.c_str());
  if(! (file>>m_version) ) {
    throw runtime_error("Can't get version number of the dump from file \""+fileName+"\"");
  }
  if(m_version > m_currentVersion) {
    ostringstream os;
    os<<"Dump version "<<m_version<<" not supported: this program supports dump versions up to "<<m_currentVersion;
    throw runtime_error(os.str());
  }
}

void DumpStructure::writeVersion(const string& filename) {
  ofstream of(filename.c_str());
  of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
  of<<m_currentVersion<<endl;
  of<<"# ccdb-migrate dump of database " + gOpt.dbConnectionString()<<endl;
  time_t tt;
  time(&tt);
  of<<"# on "<<ctime(&tt)<<endl;
}


//================================================================
void makeDir(const string& dirname) {
  if(mkdir(dirname.c_str(), 0777)) {
    throw runtime_error("Can't create output dir \""+dirname+"\": "+strerror(errno) );
  }
}

//================================================================
vector<string> readDir(const string& dirname) {
  vector<string> result;
  DIR* itd = opendir(dirname.c_str());
  if(!itd) {
    throw runtime_error("readDir(): Can't opendir("+dirname+"): "+strerror(errno) );
  }

  errno = 0; // must set set to be able to check for readdir()'s errors.
  struct dirent *entry(0);
  while( (entry = readdir(itd)) ) {
    result.push_back(entry->d_name);
  }

  // readdir() can also return NULL on an error
  if(errno) {
    throw runtime_error("readDir(\""+dirname+"\"): error from readdir(): "+strerror(errno) );
  }
  
  closedir(itd);

  std::sort(result.begin(), result.end());

  return result;
}

//================================================================
string makeFileName(const string& prefix, const TagStatus& ts) {
  return prefix + (ts.locked() ? "L" : "U" ) + "-" + ts.tag();
}

//================================================================
TagStatus getTagStatusFromFileName(const DumpStructure& ver, const string& prefix, const string& filename) {

  string stripped =
    (prefix.length() < filename.length()) && !filename.compare(0, prefix.length(), prefix) ? 
    filename.substr(prefix.length()) : string() ;
  
  if(stripped.empty()) {
    return TagStatus(string(), false);
  }
  else {
    // After stripping the prefix, the string may still contain 
    // tag lock status in addition to the tag name.
    if(ver.tagLockInFileName()) {
      // New dump structure: PREFIX+LockStatus+"-"+TAG
      
      if(stripped.length() < 3) {
	throw runtime_error("getTagStatusFromFileName(): prefix=\""+prefix+"\", filename=\""+filename+"\":"
			    +" the remained of the filename is too short to get tag lock status");
      }

      bool locked(false);
      switch(stripped[0]) {
      case 'L': locked = true; break;
      case 'U': locked = false; break;
      default: throw runtime_error("getTagStatusFromFileName(): prefix=\""+prefix+"\", filename=\""+filename+"\":"
				   +" the character '"+string(1,stripped[0])+"' does not represent a valid tag lock status. Expect 'L' or 'U'");
      }

      return TagStatus(string(stripped, 2), gOpt.lockAllTags() ? true : locked );
      
    }
    else { 
      // Old dump structure: PREFIX+TAG
      return TagStatus(string(stripped), gOpt.lockAllTags() ? true : false );
    }
  }
}

//================================================================
bool is_comment(const string& line) {
  string::size_type length = line.length();
  string::size_type i=0;

  // find first non-whitespace char
  while( (i<length) && std::isspace(line[i])) {
    ++i;
  }

  return (i==length)/*empty line*/ || (line[i]=='#')/*comment*/;
}

//================================================================
void dump_idtag(const string& idTagDir, CoralDB::CoralDB& db) {
  using CoralDB::TagList;

  //----------------------------------------------------------------
  // Object dictionary dump

  if(1) {
    cout<<"Dumping object dictionary..."<<flush;

    const string dumpFile(idTagDir + objectDictFileName);
    ofstream of(dumpFile.c_str());
    of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
    of<<"# ID\tTYPE\n";

    CompoundTag tags = gOpt.onlyIdTag().empty() ? 
      CompoundTag::OnlyIdTag(db.compoundTag().objectDictionaryTag())
      : CompoundTag(gOpt.onlyIdTag(), gOpt.onlyCTag(), gOpt.onlyATag(), gOpt.onlyDTag())
      ;

    CoralDB::ObjectDictionaryMap od;
    db.getObjectDictionary(od, tags);

    // A map is guaranteed to be sorted, so this part of a dump is reproducable.
    for(CoralDB::ObjectDictionaryMap::const_iterator i = od.begin(); i != od.end(); i++) {
      of<<i->first<<"\t"<<i->second<<"\n";
    }

    cout<<"\t"<<od.size()<<" objects dumped"<<endl;
  }

  //----------------------------------------------------------------
  // Iterate over connectivity tags
  
  if(1) {
    cout<<"Dumping connectivity data. Tags: "<<flush;

    TagList tags;
    db.getExistingConnectivityTags(tags);

    for(TagList::const_iterator itag = tags.begin(); itag != tags.end(); itag++) {
      if(gOpt.fullIdTag() || (gOpt.onlyCTag() == itag->tag())) {
	cout<<*itag<<" "<<flush;
	db.setConnectivityTag(itag->tag());

	//----------------------------------------------------------------
	// MasterList
	if(1) {
	  const string dumpFile(idTagDir + "/" + masterListFilePrefix + itag->tag());
	
	  ofstream of(dumpFile.c_str());
	  of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
	
	  vector<string> ml = db.masterList();
	
	  // Sort so that dumps are reproducable
	  std::sort(ml.begin(), ml.end());
	  for(vector<string>::const_iterator i = ml.begin(); i != ml.end(); i++) {
	    of<<*i<<"\n";
	  }
	}

	//----------------------------------------------------------------
	// Connections
	if(1) {
	  const string dumpFile(makeFileName(idTagDir + "/" + connectivityFilePrefix, *itag));
	  ofstream of(dumpFile.c_str());
	  of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
	  of<<"# ID\tSLOT\tTOID\tTOSLOT\n";

	  CoralDB::ConnectionTableMap ct;
	  db.getConnectionTable(ct);

	  // Sort so that dumps are reproducable
	  std::sort(ct.begin(), ct.end());
	
	  for(CoralDB::ConnectionTableMap::const_iterator i=ct.begin(); i!=ct.end(); i++) {
	    of<<i->fromId()<<"\t"<<i->fromSlot()<<"\t"<<i->toId()<<"\t"<<i->toSlot()<<"\n";
	  }
	}

      } // if(dump this tag)
    } // for()

    cout<<endl;
  }

  //----------------------------------------------------------------
  // Iterate over alias tags
  
  if(1) {
    cout<<"Dumping aliases. Tags: "<<flush;

    TagList tags;
    db.getExistingAliasTags(tags);

    for(TagList::const_iterator itag = tags.begin(); itag != tags.end(); itag++) {
      if(gOpt.fullIdTag() || (gOpt.onlyATag() == itag->tag())) {

	cout<<*itag<<" "<<flush;
	db.setAliasTag(itag->tag());

	//----------------------------------------------------------------
	// Aliases
	if(1) {
	  const string dumpFile(makeFileName(idTagDir + "/" + aliasFilePrefix,*itag));
	  ofstream of(dumpFile.c_str());
	  of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
	  // the same format as generic file for insertBulkAliases()
	  of<<"# ALIAS\tCONVENTION\tID\n";

	  CoralDB::AliasesTable data;
	  db.getAliasesTable(data);

	  // aliases are already sorted
	  for(CoralDB::AliasesTable::const_iterator i=data.begin(); i!=data.end(); i++) {
	    of<<i->alias()<<"\t"<<i->convention()<<"\t"<<i->id()<<"\n";
	  }
	}

      }// if(dump this tag)
    } // for()

    cout<<endl;
  }

  //----------------------------------------------------------------
  // Iterate over data tags
  
  if(1) {
    cout<<"Dumping payloads. Tags: "<<flush;

    TagList tags;
    db.getExistingDataTags(tags);

    for(TagList::const_iterator itag = tags.begin(); itag != tags.end(); itag++) {
      if(gOpt.fullIdTag() || (gOpt.onlyDTag() == itag->tag())) {

	cout<<*itag<<" "<<flush;
	db.setDataTag(itag->tag());

	//----------------------------------------------------------------
	// CLOBs
	if(1) {
	  const string dumpFile(makeFileName(idTagDir + "/" + clobFilePrefix, *itag));
	  ofstream of(dumpFile.c_str());
	  of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
	  of<<"# ID\tFIELD\tCLOB\n";

	  CoralDB::ClobNameContainer cn;
	  db.getClobNames(cn);
	
	  // std::set is guaranteed to be sorted
	  for(CoralDB::ClobNameContainer::const_iterator i=cn.begin(); i!=cn.end(); i++) {
	    Encodable clob = db.findCLOB(i->first, i->second);
	    of<<i->first<<"\t"<<i->second<<"\t"<<clob<<"\n";
	  }
	}

      }// if(dump this tag)
    }// for()

    cout<<endl;
  }
}

//================================================================
void ccdb_migrate_dump(const string& dirname, CoralDB::CoralDB& db) {
  //throw std::runtime_error("dump: not implemented for schema V2");
  
  using CoralDB::IdTagList;

  makeDir(dirname);

  IdTagList itl;
  db.getExistingObjectDictionaryTags(itl);

  for(IdTagList::const_iterator its = itl.begin(); its != itl.end(); ++its) {
    
    if( gOpt.onlyIdTag().empty() || (gOpt.onlyIdTag() == its->tag()) ) {

      const string idTagDir(dirname + "/" + makeFileName(idTagDirPrefix, *its) );
      makeDir(idTagDir);
      
      db.setObjectDictionaryTag(its->tag());
      dump_idtag(idTagDir, db);
    }

  }

  //----------------------------------------------------------------
  // Tag history dump

  if(gOpt.onlyIdTag().empty()) {
    cout<<"Dumping tag history..."<<flush;

    const string dumpFile(dirname + historyFileName);
    ofstream of(dumpFile.c_str());
    of.exceptions(ios_base::failbit | ios_base::badbit | ios_base::eofbit);
    of<<"# unixTime\tIDTAG\tCTAG\tATAG\tDTAG\n";

    CoralDB::HistoryTable hh;
    db.getHistoryTable(hh);

    // Alredy sorted
    for(CoralDB::HistoryTable::const_iterator i = hh.begin(); i != hh.end(); i++) {
      of<<i->first
	<<"\t"<<i->second.objectDictionaryTag()
	<<"\t"<<i->second.connectivityTag()
	<<"\t"<<i->second.aliasTag()
	<<"\t"<<i->second.dataTag()
	<<"\n";
    }

    cout<<"\t"<<hh.size()<<" entries dumped"<<endl;
  }
  
  //----------------------------------------------------------------
  // Dump structure version last.  If the version file is written, there were
  // no errors earlier in the dumping process.
  
  DumpStructure::writeVersion(dirname + versionFileName);
  
  //----------------
}

//================================================================
void restore_idtag(const DumpStructure& ver, const string& idTagDir, CoralDB::CoralDB& db) {

  // Object dictionary
  //  if(1) {
  if(!gOpt.noIdTag()){
    cout<<"Restoring object dictionary..."<<flush;
    const string dumpFile(idTagDir + objectDictFileName);
    ifstream ff(dumpFile.c_str());
    ff.exceptions(ios_base::badbit);
    unsigned count = db.insertBulkObjects(ff, gOpt.debug());
    cout<<"\t"<<count<<" objects inserted"<<endl;
  }

  // List of all directory entries
  vector<string> dirList = readDir(idTagDir);
  
  //----------------------------------------------------------------
  cout<<"Restoring connectivity data:"<<endl;
  for(vector<string>::const_iterator i=dirList.begin(); i!=dirList.end(); i++) {
    TagStatus ts = getTagStatusFromFileName(ver, connectivityFilePrefix, *i);
    const string tag = ts.tag();

    //cerr<<"file = "<<*i<<", tag = "<<tag<<endl;
    if(!tag.empty()) {
      cout<<"\tWorking on connectivity tag "<<tag<<"..."<<flush;
      db.makeNewConnectivityTag(tag);
      db.setConnectivityTag(tag);

      const string pathname(idTagDir+"/"+*i);
      ifstream ff(pathname.c_str());
      ff.exceptions(ios_base::badbit);
      if(!ff) {
        throw runtime_error("Can't open file "+pathname);
      }

      unsigned count = db.insertBulkConnections(ff, CoralDB::CoralDB::FOUR_COL, gOpt.debug() );

      // master list goes under the same tag.
      const string masterListPathName(idTagDir+"/"+masterListFilePrefix+tag);
      ifstream mlfile(masterListPathName.c_str());
      mlfile.exceptions(ios_base::badbit);
      if(!mlfile) {
	throw runtime_error("Can't open file "+masterListPathName);
      }

      string line;
      while(getline(mlfile, line)) {
	if(!is_comment(line)) {
	  //cout<<"Got non-comment line: "<<line<<": after the line"<<endl;
	  db.insertMaster(line);
	}	
      }

      cout<<"\t"<<count<<" connections inserted"<<endl;
    }
  }

  //----------------------------------------------------------------
  // Iterate over alias tags

  cout<<"Restoring aliases:"<<endl;
  for(vector<string>::const_iterator i=dirList.begin(); i!=dirList.end(); i++) {
    TagStatus ts = getTagStatusFromFileName(ver, aliasFilePrefix, *i);
    const string tag = ts.tag();
    //cerr<<"Alias file = "<<*i<<", tag = "<<tag<<endl;
    if(!tag.empty()) {
      cout<<"\tWorking on alias tag "<<tag<<"..."<<flush;
      db.makeNewAliasTag(tag);
      db.setAliasTag(tag);

      const string pathname(idTagDir+"/"+*i);
      ifstream ff(pathname.c_str());
      ff.exceptions(ios_base::badbit);
      if(!ff) {
        throw runtime_error("Can't open file "+pathname);
      }

      unsigned count = db.insertBulkAliases(ff, true, gOpt.debug(), gOpt.sloppyAliases() );
      cout<<"\t"<<count<<" aliases inserted"<<endl;
    }
  }

  //----------------------------------------------------------------
  // Iterate over data tags

  cout<<"Restoring payloads:"<<endl;
  for(vector<string>::const_iterator i=dirList.begin(); i!=dirList.end(); i++) {
    TagStatus ts = getTagStatusFromFileName(ver, clobFilePrefix, *i);
    const string tag = ts.tag();
    //cerr<<"Data file = "<<*i<<", tag = "<<tag<<endl;
    if(!tag.empty()) {
      cout<<"\tWorking on data tag "<<tag<<"... "<<flush;
      db.makeNewDataTag(tag);
      db.setDataTag(tag);

      const string pathname(idTagDir+"/"+*i);
      ifstream ff(pathname.c_str());
      ff.exceptions(ios_base::badbit);
      if(!ff) {
        throw runtime_error("Can't open file "+pathname);
      }

      unsigned clobCount(0), lineCount(0);

      string id;
      while(1) {
	++lineCount;

	// Eat empty lines
	char c;
	while(ff.get(c) && (c == '\n') ) {
	  ++lineCount;
	}

	if(!ff) {
	  break;
	}

	if(c=='#') {
	  // Comment line, skip
	  ff.ignore(std::numeric_limits<streamsize>::max() , '\n');
	  continue;
	}

	if(c == '\t') {
	  ostringstream err;
	  err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : empty ID";
	  throw runtime_error(err.str());
	}

	string id;
	do {
	  if(c == '\n') {
	    ostringstream err;
	    err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : newline while reading ID";
	    throw runtime_error(err.str());
	  }

	  id.push_back(c);
	} while( ff.get(c) && (c != '\t' ) );


	if(!ff) {
	  ostringstream err;
	  err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : no tab after ID=\""<<id<<"\"";
	  throw runtime_error(err.str());
	}
	

	// now c is the '\t' between ID and FIELD
	string field;
	while( ff.get(c) && (c != '\t' ) ) {
	  if(c == '\n') {
            ostringstream err;
            err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : newline while reading FIELD for ID=\""<<id<<"\"";
            throw runtime_error(err.str());
          }
	  
	  field.push_back(c);
	}

	if(field.empty()) {
	  ostringstream err;
          err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : empty FIELD for ID=\""<<id<<"\"";
          throw runtime_error(err.str());
	}

	if(!ff) {
          ostringstream err;
          err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : no tab after FIELD=\""<<field<<"\" for ID=\""<<id<<"\"";
          throw runtime_error(err.str());
        }
	
	// now c is the '\t' between FIELD and CLOB
	Encodable clob;
	if(! (ff>>clob) ) { // this eats the newline
	  ostringstream err;
	  err<<"Ill formed line "<<lineCount<<" in file \""<<pathname<<"\" : can't decode CLOB."
	     <<"\nID=\""<<id<<"\", FIELD=\""<<field<<"\", for CLOB so far read: "<<clob;
	  throw runtime_error(err.str());
	}

	// CLOB is allowed to be empty

	//cout<<"ID = "<<id<<", field = "<<field<<", CLOB = "<<clob<<endl;
	
	db.insertCLOB(id, field, clob);
	++clobCount;
      }

      if(ff.bad()) {
	throw runtime_error("Error reading from file "+pathname);
      }

      cout<<"\t"<<clobCount<<" CLOBs inserted"<<endl;
    }
  }

}

//================================================================
void ccdb_migrate_restore(const string& dirname, CoralDB::CoralDB& db) {
  //----------------------------------------------------------------
  // Check the dump structure version

  DumpStructure ver(dirname + versionFileName);
  cout<<ver<<endl;
    
  //----------------------------------------------------------------
  if(!gOpt.noSchema()) {
    cout<<"Creating database schema..."<<endl;
    db.createSchema();
  }

  //----------------------------------------------------------------
  // Top level dir

  string idTagForHistory;
  vector<string> topDir = readDir(dirname);
  for(vector<string>::const_iterator entry=topDir.begin(); entry!=topDir.end(); entry++) {
    TagStatus idTagStatus = getTagStatusFromFileName(ver, idTagDirPrefix, *entry);
    if(!idTagStatus.tag().empty()) {

      cout<<"Working on IDTAG = "<<idTagStatus.tag()<<endl;

      if(!gOpt.noHistory()) {
	// old dumps do not have IDTAG information in the history dump
	// If there is only one IDTAG to be restored, it is unambiguous to use it for history.
	// Refuse to restore more than one IDTAG from an old format dump.
	if(!ver.idTagInHistory()) { 
	  if(idTagForHistory.empty()) {
	    idTagForHistory = idTagStatus.tag();
	  }
	  else {
	    throw runtime_error("ccdb_migrate_restore(): got more than one IDTAG from an old format dump: "
				+idTagForHistory+" and "+idTagStatus.tag());
	  }
	}
      }
      
      if(!gOpt.noIdTag()){
	db.makeNewObjectDictionaryTag(idTagStatus.tag());
      }
      db.setObjectDictionaryTag(idTagStatus.tag());

      restore_idtag(ver, dirname + "/" + *entry, db);
      
      if(idTagStatus.locked()) {
	db.lockObjectDictionaryTag(idTagStatus.tag());
      }
      
    } // !idtag.empty()
  } // for(entry in topDir)
  

  //----------------------------------------------------------------
  // Tag history
  if(!gOpt.noHistory()) {
    cout<<"Restoring tag history table..."<<flush;
    const string pathname(dirname + historyFileName);
    ifstream ff(pathname.c_str());
    ff.exceptions(ios_base::badbit);
    if(!ff) {
      throw runtime_error("Can't open file "+pathname);
    }
    
    unsigned histCount(0);
    string line;
    while(getline(ff, line)) {
      if(!is_comment(line)) {
	istringstream is(line);

	time_t tt(0);
	if(!(is>>tt)) {
	  throw runtime_error("Error reading time: file \""+pathname+"\", line:\n"+line);
	}

	string idtag;
	if(ver.idTagInHistory()) {
	  if(!(is>>idtag)) {
	    throw runtime_error("Error reading idtag: file \""+pathname+"\", line:\n"+line);
	  }
	}
	else {
	  idtag = idTagForHistory;
	}

	string ctag;
	if(!(is>>ctag)) {
	  throw runtime_error("Error reading ctag: file \""+pathname+"\", line:\n"+line);
	}

	string atag;
	if(!(is>>atag)) {
	  throw runtime_error("Error reading atag: file \""+pathname+"\", line:\n"+line);
	}

	string dtag;
	if(!(is>>dtag)) {
	  throw runtime_error("Error reading dtag: file \""+pathname+"\", line:\n"+line);
	}

	db.setHistoricTag(CoralDB::CompoundTag(idtag, ctag, atag, dtag), tt);

	++histCount;
      }
    }

    cout<<"\t"<<histCount<<" history entries inserted"<<endl;
  } // if(!gOpt.noHistory())

}

//================================================================

//int main(int argc, char *const argv[]) {
int main(int argc, char  *argv[]) {
  int exitcode = 1;
  try {

    gOpt = CCDB_Migrate_Config(argc, argv);

    if(gOpt.debug()) {
      cout<<"\n================================================================\n"
	  <<gOpt
	  <<"\n================================================================\n"
      ;
    }

    switch(gOpt.mode()) {
    case CCDB_Migrate_Config::DUMP: {
      CoralDB::CoralDB db(gOpt.dbConnectionString(),
			  coral::ReadOnly,
			  gOpt.debug()? coral::Debug : coral::Error, 
			  CoralDB::CoralDB::MANUAL_COMMIT);
      
      cout<<"Dumping database \""<<gOpt.dbConnectionString()<<"\" to directory \""<<gOpt.topDir()<<"\" ..."<<endl;
      ccdb_migrate_dump(gOpt.topDir(), db);
    }
      break;
    case CCDB_Migrate_Config::RESTORE: {

      CoralDB::CoralDB db(gOpt.dbConnectionString(),
			  coral::Update,
			  gOpt.debug()? coral::Debug : coral::Error, 
			  CoralDB::CoralDB::MANUAL_COMMIT);
      

      cout<<"Restoring dump in directory  \""<<gOpt.topDir()<<"\" to database \""<<gOpt.dbConnectionString()<<"\""<<endl;
      db.transactionStart();
      ccdb_migrate_restore(gOpt.topDir(), db);
      db.transactionCommit();
    }
      break;
    default:
      throw runtime_error("Internal error: unknown return value from CCDB_Migrate_Config::mode()");
    }

    cout<<"OK!"<<endl;
    exitcode = 0;
  }
  //----------------------------------------------------------------
  catch(CCDB_Migrate_Config::HelpMessage& e) {
    std::cout<<e.what()<<std::endl;
  }
  catch(std::exception& e) {
    std::cerr<<"\nGot std::exception: "<<e.what()<<std::endl;
  }
  catch(...) {
    std::cerr<<"\nGot a non-std exception"<<std::endl;
  }

  return exitcode;
}

//EOF
