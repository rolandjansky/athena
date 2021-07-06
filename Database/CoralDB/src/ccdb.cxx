/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ccdb.cxx,v 1.47 2009-03-05 20:03:03 beringer Exp $

#ifndef _GNU_SOURCE
#define _GNU_SOURCE /* for getopt_long() */
#endif

#include "CoralDB/CoralDB.h"
#include "CoralDB/Connection.h"
#include "CoralDB/Alias.h"
#include "CoralDB/CompoundTag.h"
#include "CoralDB/Encodable.h"

//#include "PluginManager/PluginManager.h"
//#include "SealKernel/Context.h"
//#include "SealKernel/ComponentLoader.h"
//#include "SealKernel/IMessageService.h"
#include "CoralBase/MessageStream.h"
#include "CoralBase/Exception.h"
#include "CoralBase/AttributeSpecification.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/ISessionProxy.h"
#include "RelationalAccess/ITransaction.h"
#include "RelationalAccess/TableDescription.h"
#include "RelationalAccess/ISchema.h"

#include <iostream>
#include <iomanip>
#include <iterator>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

#include <getopt.h>

using namespace std;

using CoralDB::Alias;
using CoralDB::Connection;
using CoralDB::TagList;
using CoralDB::CompoundTag;
using CoralDB::ClobNameContainer;
using CoralDB::IdTagList;
using CoralDB::TagStatus;
using CoralDB::HistoryTable;
using CoralDB::Encodable;
    

namespace {
  std::string gDoc =
  "\n" "ccdb - A test utility for CoralDB"
  "\n" "================================="
  "\n" "Usage: ccdb [OPTS] CMD [ARGS]"
  "\n" ""
  "\n" "Options can come in any order.  Supported options are:"
  "\n"
  "\n" "        --db    CONNECTION  # a DB connection string. This is required."
  "\n"	 
  "\n" "        --idTag IDTAG # sets a current object dictionary tag"
  "\n" "              Most commands require that an existing idTag is defined"
  "\n" "              through this option. (\"Current\" idTag)"
  "\n" "              Exceptions are schema creation/dropping, making new idTag,"
  "\n" "              locking idTag, and working with historic tags."
  "\n" "              Specifying idTag when not needed is OK, provided it exists."
  "\n"	 
  "\n" "        --ctag  TAG   # sets a current connectivity tag"
  "\n" "        --dtag  TAG   # sets a current data tag"
  "\n" "        --atag  TAG   # sets a current alias tag"
  "\n" "        --tags  TAG   # a shorthand for ctag=dtag=atag"
  "\n" 	 
  "\n" "              A proper tag must be defined for most commands to work."
  "\n" "              Exceptions are schema creation/dropping, making or locking tags,"
  "\n" "              and working with historic tags."
  "\n" "              Specifying a tag when not needed is OK, provided it exists."
  "\n"	 
  "\n" "        --sloppyAliases  # Allows the loadAliases command to ignore aliases"
  "\n" "              for objects that are not known to the object dictionary."
  "\n"	 
  "\n" "        --help        # print out this message and exit"
  "\n" "        --debug       # print out debug info when executing a command"
  "\n"
  "\n" "The commands:"
  "\n"
  ;
}

#define CCDB_COMMAND(allow_exec, cmd, name, tagtype, minarg, maxarg, narg, accessmode, docstr) (gDoc += "\n\t" name, gDoc += docstr, allow_exec && (name==cmd) && ( assertNumArgs(name, minarg, maxarg, narg), db = new CoralDB::CoralDB(opt.dbConnectionString(), accessmode, outputLevel, CoralDB::CoralDB::AUTOCOMMIT), setTags(db, optCompoundTag, tagtype), true ) )

bool CCDB_ADDTEXT(const string& str) {
  gDoc += str;
  return false;
}

//================================================================
// What tag(s) need to be set 
enum CCDB_TagBits { IDTAG_BIT=0x1, CTAG_BIT=0x2, DTAG_BIT=0x4, ATAG_BIT=0x8 };

enum CCDB_TagType { NOTAG=0, IDTAG=IDTAG_BIT, 
		    CTAG=CTAG_BIT|IDTAG_BIT, DTAG=DTAG_BIT|IDTAG_BIT, ATAG=ATAG_BIT|IDTAG_BIT,
		    ALLTAGS=IDTAG_BIT|CTAG_BIT|DTAG_BIT|ATAG_BIT
};

void setTags(CoralDB::CoralDB* db, const CompoundTag& ct, CCDB_TagType tt) {
  if(tt & IDTAG_BIT) {
    db->setObjectDictionaryTag(ct.objectDictionaryTag());
  }
  if(tt & CTAG_BIT) {
    db->setConnectivityTag(ct.connectivityTag());
  }
  if(tt & DTAG_BIT) {
    db->setDataTag(ct.dataTag());
  }
  if(tt & ATAG_BIT) {
    db->setAliasTag(ct.aliasTag());
  }
}

//================================================================
time_t parseTimeSpec(const string& spec) {
  std::istringstream is(spec);
  time_t seconds;
  if(! (is>>seconds) ) {
    throw runtime_error("parseTimeSpec(): invalid number of seconds \""+spec+"\"");
  }
  return seconds;
}

//================================================================
void printTags(const TagList& tagList) {
  typedef TagList::const_iterator Iter;
  for(Iter q=tagList.begin(); q!=tagList.end(); ++q) {
    cout<<*q<<" ";
  }
}

//================================================================
const std::string& short_usage() { 
  static const std::string str = 
    "Run \"ccdb --help\" for more information.";
  return str;
}

//================================================================
std::string  long_usage() {
  return gDoc 
    +
    "\n"
    "\n" "Examples:"
    "\n"
    "\n" "       ccdb --db sqlite_file:test.db create"
    "\n" "       ccdb --db sqlite_file:test.db makeNewObjectDictionaryTag IDTAG1"
    "\n" "       ccdb --db sqlite_file:test.db --idTag IT makeNewConnectivityTag C1"
    "\n"
    "\n" "       ccdb --db sqlite_file:test.db --idTag IT --ctag C1 \\"
    "\n" "                  loadConnections < data/connectivity.dat"
    "\n"
    "\n" "       ccdb --db sqlite_file:test.db --idTag IT --ctag C1 \\"
    "\n" "                  findOutConn ROD_CRATE_0"
    "\n"
    "\n" "       ccdb --db oracle://ATLAS_COOLPROD/ATLAS_COOLONL_PIXEL getExistingTags"
    "\n"
    "\n" "Note: to connect to Oracle, the working dir should contain"
    "\n" "an authentication.xml file with valid username and password."
    "\n" ;
}

//================================================================
void assertNumArgs(const string& cmd, int nRequired, int nAccepted, int nProvided) {
  if(nProvided < nRequired) {
    cerr<<"Error: command  "<<cmd<<"  requires "<<nRequired
	<<" arguments, but only "<<nProvided<<" are given.\n"
	<<short_usage()
	<<endl
      ;
    throw runtime_error("Bad command line arguments");
  }
  if(nAccepted < nProvided) {
    cerr<<"Error: command  "<<cmd<<"  accepts up to "<<nAccepted
	<<" arguments, but  "<<nProvided<<" arguments are given.\n"
	<<short_usage()
	<<endl
      ;
    throw runtime_error("Bad command line arguments");
  }
}

//================================================================
class CCDB_Config {
public:
  CCDB_Config() :
    m_help (0),
    m_debug (0),
    m_sloppyAliases (0)
  {}

  CCDB_Config(int argc, char *const argv[]);

  const string& dbConnectionString() const { return m_dbConnectionString; }

  const string& objectDictionaryTag() const { return m_objectDictionaryTag; }
  const string& connectivityTag() const { return m_connectivityTag; }
  const string& dataTag() const { return m_dataTag; }
  const string& aliasTag() const { return m_aliasTag; }

  bool help() const { return m_help; }
  bool debug() const { return m_debug; }
  bool sloppyAliases() const { return m_sloppyAliases; }

private:
  string m_dbConnectionString;

  string m_objectDictionaryTag;
  string m_connectivityTag;
  string m_dataTag;
  string m_aliasTag;

  // int instead of bool for getopt_long() compatibility
  int m_help;
  int m_debug;
  int m_sloppyAliases;

  static void checkAndSet(std::string *str, const std::string& newvalue, const std::string& opt_name);
};

CCDB_Config gOpt;

//================================================================
std::ostream& operator<<(std::ostream& os, const CCDB_Config& opt) {
  return os<<"config: dbConnectionString="<<opt.dbConnectionString()<<endl    
	   <<"config: objectDictionaryTag="<<opt.objectDictionaryTag()<<endl    
	   <<"config: connectivityTag="<<opt.connectivityTag()<<endl    
	   <<"config: dataTag="<<opt.dataTag()<<endl    
	   <<"config: aliasTag="<<opt.aliasTag()<<endl    
	   <<"config: help="<<opt.help()<<endl    
	   <<"config: debug="<<opt.debug()<<endl    
	   <<"config: sloppyAliases="<<opt.sloppyAliases()<<endl    
    ;
}

//================================================================
void CCDB_Config::checkAndSet(std::string *str, const std::string& /*newvalue*/, const std::string& opt_name) {
  if(! str->empty() ) {
    throw runtime_error("Error: option "+opt_name+" attempts to redefine a variable, which is already set to "+*str);
  }
  *str = optarg;
}

//================================================================
CCDB_Config::CCDB_Config(int argc, char *const argv[]) 
  : m_help(0)
  , m_debug(0)
  , m_sloppyAliases(0)
{
  // Process options
  while(1) {
    int option_index = 0;
    enum {TRUE = 1, OPT_DB=900, OPT_IDTAG, OPT_CTAG, OPT_DTAG, OPT_ATAG, OPT_TAGS };
    static struct option long_options[] = {
      {"help", 0, &m_help, TRUE},
      {"debug", 0, &m_debug, TRUE},
      {"sloppyAliases", 0, &m_sloppyAliases, TRUE},
      {"db", 1, 0, OPT_DB},
      {"idTag", 1, 0, OPT_IDTAG},
      {"ctag", 1, 0, OPT_CTAG},
      {"dtag", 1, 0, OPT_DTAG},
      {"atag", 1, 0, OPT_ATAG},
      {"tags", 1, 0, OPT_TAGS},
      {0, 0, 0, 0}
    };

    //int status = getopt_long(argc, argv, "+"/*No short opts, don't permute argv*/, long_options, &option_index);

    // Default behaviour: permute argv (allowing to mix opts and args).  Implement the "-h" option.
    int status = getopt_long(argc, argv, "h",long_options, &option_index);
    if(status == -1) {
      break;
    }
    switch(status) {
    case 0: case TRUE: // long option
      // cerr<<"Got option: "<<long_options[option_index].name;
      // if(optarg) {
      // 	cerr<<" with arg "<< optarg;
      // }
      // cerr<<endl;

      break;
      
    case 'h':
      m_help = true;
      break;

    case OPT_DB:
      checkAndSet(&m_dbConnectionString, optarg, long_options[option_index].name);
      break;
      
    case OPT_IDTAG:
      checkAndSet(&m_objectDictionaryTag, optarg, long_options[option_index].name);
      break;
      
    case OPT_CTAG:
      checkAndSet(&m_connectivityTag, optarg, long_options[option_index].name);
      break;

    case OPT_DTAG:
      checkAndSet(&m_dataTag, optarg, long_options[option_index].name);
      break;

    case OPT_ATAG:
      checkAndSet(&m_aliasTag, optarg, long_options[option_index].name);
      break;

    case OPT_TAGS:
      checkAndSet(&m_connectivityTag, optarg, long_options[option_index].name);
      checkAndSet(&m_dataTag, optarg, long_options[option_index].name);
      checkAndSet(&m_aliasTag, optarg, long_options[option_index].name);
      break;
      
    case ':': // missing arg
    case '?': // unrecognized or ambiguous option
      cerr<<short_usage()<<endl;
      throw runtime_error("Bad command line parameters");
      break;
    default:
      throw runtime_error("CCDB_Config: internal error: getopt_long() returned unrecognized code");
      break;
    }
  }
}

//================================================================
int main(int argc, char* argv[]) {
  try {
    CCDB_Config opt(argc, argv);
    // cout<<"Got config:\n"<<opt<<endl;

    const int nargs = argc-optind-1;

    bool allow_exec = opt.help() ? false : true;

    if(allow_exec && (nargs < 0)) {
      cerr<<"Error: missing a command.\n"<<short_usage()<<endl;
      return 1;
    }

    if(allow_exec && opt.dbConnectionString().empty()) {
      cerr<<"Error: database not specified.\n"<<short_usage()<<endl;
      return 1;
    }

    CompoundTag optCompoundTag(opt.objectDictionaryTag(), opt.connectivityTag(), opt.dataTag(), opt.aliasTag() );

    coral::MsgLevel outputLevel = opt.debug() ? coral::Debug : coral::Error;

    CoralDB::CoralDB *db(0);

    const string cmd(allow_exec ? argv[optind] :"");

    //----------------------------------------------------------------
    if (CCDB_COMMAND(allow_exec, cmd, "create", NOTAG, 0, 0, nargs, coral::Update, "")) {
      db->createSchema();
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "dropSchemaAndDeleteAllData", NOTAG, 0, 0, nargs, coral::Update, "")) {
      db->dropSchema();
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "loadConnections", CTAG, 0, 0, nargs, coral::Update, 
			  " < connectivitydata.dat\t# reads from stdin") ) {
      cout << "*** " << db->insertBulkConnections(cin, CoralDB::CoralDB::SIX_COL, opt.debug() ) << " entries loaded into connection table" << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "loadAliases", ATAG, 0, 0, nargs, coral::Update,
                          " < aliasesdata.dat\t\t# reads from stdin") ) {
      cout << "*** " << db->insertBulkAliases(cin, true, opt.debug(), opt.sloppyAliases() ) << " aliases loaded" << endl;
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "insertMaster", CTAG, 1, 1, nargs, coral::Update, " id")) {
      db->insertMaster(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteMaster", CTAG, 1, 1, nargs, coral::Update, " id")) {
      cout << db->deleteMaster(argv[optind+1]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "masterList", CTAG, 0, 0, nargs, coral::ReadOnly, "")) {
      vector<string> ml = db->masterList();
      // Sort so that dumps are reproducable
      std::sort(ml.begin(), ml.end());
      for (const std::string& s : ml) {
	cout <<s<< endl;
      }
    }    
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "insertObject", IDTAG, 2, 2, nargs, coral::Update, " id type")) {
      cout << db->checkAndInsertObject(argv[optind+1],argv[optind+2]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteObject", IDTAG, 1, 1, nargs, coral::Update, " id")) {
      cout << db->deleteObject(argv[optind+1]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "updateObject", IDTAG, 2, 2, nargs, coral::Update, " id type")) {
      cout << db->updateObject(argv[optind+1],argv[optind+2]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "objectType", IDTAG, 1, 1, nargs, coral::ReadOnly, "   id")) {
      cout << db->objectType(argv[optind+1]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "renameID", IDTAG, 2, 2, nargs, coral::Update, "     oldID newID")) {
      // Note: cmdline args are "old new" like for shell's "mv", while API method args are "new, old"
      // like in assignment, memcpy() and elsewhere.
      cout << db->renameID(argv[optind+2],argv[optind+1]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "renameType", IDTAG, 2, 2, nargs, coral::Update, "   oldType newType")) {
      // Note: cmdline args are "old new" like for shell's "mv", while API method args are "new, old"
      // like in assignment, memcpy() and elsewhere.
      cout << db->renameType(argv[optind+2],argv[optind+1]) << endl;
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "ids", IDTAG, 0, 0, nargs, coral::ReadOnly,
			  "\t# If only idTag is set, shows all its objects.\n"
			  "\t\tIf in addition to --idTag any of --ctag, --atag, --dtag\n"
			  "\t\tare set, show the union of sets of IDs mentioned in the\n"
			  "\t\tgiven connectivity, alias and data tags.\n"
			  )) {
      CoralDB::ObjectDictionaryMap od;
      db->getObjectDictionary(od, optCompoundTag);

      // A map is guaranteed to be sorted
      for(CoralDB::ObjectDictionaryMap::const_iterator i = od.begin(); i != od.end(); ++i) {
	cout<<i->first<<"\t"<<i->second<<endl;
      }
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "insertConnection", CTAG, 4, 4, nargs, coral::Update, "\tid slot toId toSlot")) {
      db->insertConnection(argv[optind+1],argv[optind+2],argv[optind+3],argv[optind+4]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteOutConnection", CTAG, 2, 2, nargs, coral::Update, "\tid slot")) {
      cout << db->deleteConnection(argv[optind+1],argv[optind+2],true) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteInConnection", CTAG, 2, 2, nargs, coral::Update, "\tid slot")) {
      cout << db->deleteConnection(argv[optind+1],argv[optind+2],false) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "updateOutConnection", CTAG, 4, 4, nargs, coral::Update, "\tid slot toId toSlot")) {
      cout << db->updateConnection(argv[optind+1],argv[optind+2],argv[optind+3],argv[optind+4],true) << endl;
      
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "updateInConnection", CTAG, 4, 4, nargs, coral::Update, "\tid slot toId toSlot")) {
      cout << db->updateConnection(argv[optind+1],argv[optind+2],argv[optind+3],argv[optind+4],false) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findOutSlot", CTAG, 2, 2, nargs, coral::ReadOnly, "\t\tid slot")) {
      vector<Connection> connections = db->findConnections(argv[optind+1],argv[optind+2],true);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
        connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findInSlot", CTAG, 2, 2, nargs, coral::ReadOnly, "\t\tid slot")) {
      vector<Connection> connections = db->findConnections(argv[optind+1],argv[optind+2],false);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
        connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findOutConn", CTAG, 1, 1, nargs, coral::ReadOnly, "\t\tid")) {
      vector<Connection> connections = db->findAllConnections(argv[optind+1],true);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findInConn", CTAG, 1, 1, nargs, coral::ReadOnly, "\t\tid")) {
      vector<Connection> connections = db->findAllConnections(argv[optind+1],false);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findOutConnByType", CTAG, 2, 2, nargs, coral::ReadOnly, "\tid type")) {
      vector<Connection> connections = db->findConnectionsByType(argv[optind+1],argv[optind+2],true);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findInConnByType", CTAG, 2, 2, nargs, coral::ReadOnly, "\tid type")) {
      vector<Connection> connections = db->findConnectionsByType(argv[optind+1],argv[optind+2],false);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findConnFromSlot", CTAG, 2, 2, nargs, coral::ReadOnly, "\tslot toid")) {
      vector<Connection> connections = db->findConnectionsFromSlot(argv[optind+1],argv[optind+2]);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findConnToSlot", CTAG, 2, 2, nargs, coral::ReadOnly, "\t\tid toslot")) {
      vector<Connection> connections = db->findConnectionsToSlot(argv[optind+1],argv[optind+2]);
      for (vector<Connection>::size_type i=0; i<connections.size(); i++) {
	connections[i].print(cout);
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "connections", CTAG, 0, 0, nargs, coral::ReadOnly, "\t\t# prints out all connections for the tag")) {
      CoralDB::ConnectionTableMap ct;
      db->getConnectionTable(ct);
      // Sort so that dumps are reproducable
      std::sort(ct.begin(), ct.end());
      for(CoralDB::ConnectionTableMap::const_iterator i=ct.begin(); i!=ct.end(); ++i) {
	//cout<<*i<<endl;
	i->print(cout);
      }
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "insertAlias", ATAG, 3, 3, nargs, coral::Update, "\talias convention id")) {
      db->insertAlias(argv[optind+1],argv[optind+2],argv[optind+3]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteAlias", ATAG, 2, 2, nargs, coral::Update, "\tid    convention")) {
      db->deleteAlias(argv[optind+1],argv[optind+2]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findAliases", ATAG, 1, 1, nargs, coral::ReadOnly, "\tid")) {
      vector<Alias> aliases = db->findAliases(argv[optind+1]);
      for (vector<Alias>::size_type i=0; i<aliases.size(); i++) {
	cout << left << setw(20) << aliases[i].alias() << "convention = " << aliases[i].convention() << endl;
      }
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findAlias", ATAG, 2, 2, nargs, coral::ReadOnly, "\tid    convention")) {
      cout << db->findAlias(argv[optind+1],argv[optind+2]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findId", ATAG, 2, 2, nargs, coral::ReadOnly, "\t\talias convention")) {
      cout << db->findId(argv[optind+1],argv[optind+2]) << endl;
    }
    else if(CCDB_COMMAND(allow_exec, cmd, "aliases", ATAG, 0, 0, nargs, coral::ReadOnly, "\t\t\t# prints all aliases")) {
      CoralDB::AliasesTable data;
      db->getAliasesTable(data);
      // aliases are already sorted
      for(CoralDB::AliasesTable::const_iterator i=data.begin(); i!=data.end(); ++i) {
	//cout<<i->alias()<<"\t"<<i->convention()<<"\t"<<i->id()<<"\n";
	cout<<*i<<endl;
      }
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "insertCLOB", DTAG, 3, 3, nargs, coral::Update, "\tid field clob")) {
      db->insertCLOB(argv[optind+1],argv[optind+2],argv[optind+3]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteCLOB", DTAG, 2, 2, nargs, coral::Update, "\tid field")) {
      cout << db->deleteCLOB(argv[optind+1],argv[optind+2]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "updateCLOB", DTAG, 3, 3, nargs, coral::Update, "\tid field clob")) {
      cout << db->updateCLOB(argv[optind+1],argv[optind+2],argv[optind+3]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "findCLOB", DTAG, 2, 2, nargs, coral::ReadOnly, "\tid field")) {
      cout << db->findCLOB(argv[optind+1],argv[optind+2]) << endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "getClobNames", DTAG, 0, 0, nargs, coral::ReadOnly, "\t# prints all clob names in the tag")) {
      ClobNameContainer res;
      db->getClobNames(res);
      for(ClobNameContainer::const_iterator i=res.begin(); i!=res.end(); ++i) {
	cout << i->first << ", "<<i->second << endl;
      }
    }      
    else if(CCDB_COMMAND(allow_exec, cmd, "clobs", DTAG, 0, 0, nargs, coral::ReadOnly, "\t\t# prints names and contents for all clobs in the tag")) {
      CoralDB::ClobNameContainer cn;
      db->getClobNames(cn);
      
      // std::set is guaranteed to be sorted
      for(CoralDB::ClobNameContainer::const_iterator i=cn.begin(); i!=cn.end(); ++i) {
	Encodable clob = db->findCLOB(i->first, i->second);
	cout<<i->first<<"\t"<<i->second<<"\t"<<clob<<"\n";
      }
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "makeNewObjectDictionaryTag", NOTAG, 1, 1, nargs, coral::Update, "\tnewIdTag")) {
      db->makeNewObjectDictionaryTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "makeNewConnectivityTag", IDTAG, 1, 1, nargs, coral::Update, "\tnewTag")) {
      db->makeNewConnectivityTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "makeNewDataTag", IDTAG, 1, 1, nargs, coral::Update, "\t\tnewTag")) {
      db->makeNewDataTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "makeNewAliasTag", IDTAG, 1, 1, nargs, coral::Update, "\t\tnewTag")) {
      db->makeNewAliasTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "makeNewTags", IDTAG, 1, 1, nargs, coral::Update, "\t\tnewTag  # is a shorthand for the above 3 calls")) {
      db->makeNewConnectivityTag(argv[optind+1]);
      db->makeNewDataTag(argv[optind+1]);
      db->makeNewAliasTag(argv[optind+1]);
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if(CCDB_ADDTEXT("\n\t\t\tThe above commands create new tags")) {}
    else if(CCDB_ADDTEXT("\n\t\t\tfor the current objectDictionaryTag")) {}
    else if(CCDB_ADDTEXT("\n")) {} //................................................................

    else if (CCDB_COMMAND(allow_exec, cmd, "copyConnectivityTag", CTAG, 1, 2, nargs, coral::Update, "\tnewConnectivityTag\t[dstIdTag]")) {
      db->copyConnectivityTag(argv[optind+1], (nargs>1 ? argv[optind+2] : "" ) );
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "copyDataTag", DTAG, 1, 2, nargs, coral::Update, "\t\tnewDataTag\t[dstIdTag]")) {
      db->copyDataTag(argv[optind+1], (nargs>1 ? argv[optind+2] : "") );
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "copyAliasTag", ATAG, 1, 2, nargs, coral::Update, "\t\tnewAliasTag\t[dstIdTag]")) {
      db->copyAliasTag(argv[optind+1], (nargs>1 ? argv[optind+2] : "") );
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "copyTags", ALLTAGS, 1, 2, nargs, coral::Update, "\t\tnewTags\t[dstIdTag]  # a shorthand"
			  "\n\t\tfor the other 3 calls.")) {
      const string dstIdTag = (nargs>1 ? argv[optind+2] : "");
      db->copyConnectivityTag(argv[optind+1], dstIdTag);
      db->copyDataTag(argv[optind+1], dstIdTag);
      db->copyAliasTag(argv[optind+1], dstIdTag);
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if(CCDB_ADDTEXT("\n\t\tThe above commands create new tags for the specified dstIdTag,"
			 "\n\t\twhich must already exist, and copy the content of the current"
			 "\n\t\ttag(s) into the new one(s).  If dstIdTag is not given,"
			 "\n\t\tthe current IDTAG is used."
			 )
	    ) {}
    else if(CCDB_ADDTEXT("\n")) {} //................................................................

    else if (CCDB_COMMAND(allow_exec, cmd, "copyObjectDictionaryTag", IDTAG, 1, 1, nargs, coral::Update, 
			  "\tnewIdTag  # copies\n\t\tjust the object dictionary"
			  )
	     ) {
      db->copyObjectDictionaryTag(argv[optind+1]);
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "copyMissingObjects", IDTAG, 1, 1, nargs, coral::Update, 
			  "\tnewIdTag  # copies"
			  "\n\t\tfrom --idTag objects mentioned in any combination"
			  "\n\t\tof --ctag, --atag, --dtag skipping those (ID,TYPE) pairs that"
			  "\n\t\talready exist in newIdTag.  If only --idTag is set but no other"
			  "\n\t\ttags, adds all missing objects from --idTag to newIdTag."
			  )
	     ) {
      db->copyMissingObjects(argv[optind+1], optCompoundTag);
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "copyAllForObjectDictionaryTag", IDTAG, 1, 1, nargs, coral::Update, 
			  "\tnewIdTag  # copies\n\t\t\tall tags from current idTag to newIdTag"
			  )
	     ) {
      db->copyAllForObjectDictionaryTag(argv[optind+1]);
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteConnectivityTag", IDTAG, 1, 1, nargs, coral::Update, "\ttag")) {
      db->deleteConnectivityTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteDataTag", IDTAG, 1, 1, nargs, coral::Update, "\t\ttag")) {
      db->deleteDataTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteAliasTag", IDTAG, 1, 1, nargs, coral::Update, "\t\ttag")) {
      db->deleteAliasTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteAllForObjectDictionaryTag", NOTAG, 1, 1, nargs, coral::Update,
			  "\tidTag\t#  deletes\n\t\t\tall subordinate connectivity, data and alias tags."
			  )
	     ) {
      db->deleteAllForObjectDictionaryTag(argv[optind+1]);
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................

    // FIXME: optional existingIdTag. Add a note that the idTag should be already locked.
    else if (CCDB_COMMAND(allow_exec, cmd, "lockConnectivityTag", IDTAG, 1, 1, nargs, coral::Update, "\ttag")) {
      db->lockConnectivityTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "lockDataTag", IDTAG, 1, 1, nargs, coral::Update, "\t\ttag")) {
      db->lockDataTag(argv[optind+1]);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "lockAliasTag", IDTAG, 1, 1, nargs, coral::Update, "\t\ttag")) {
      db->lockAliasTag(argv[optind+1]);
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "lockCurrentTags", NOTAG, 0, 0, nargs, coral::Update, 
			  "  # locks tags specified via cmdline options")
	     ) {
      db->lockCompoundTag(optCompoundTag);
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "lockObjectDictionaryTag", NOTAG, 1, 1, nargs, coral::Update, "\t\tidTag # locks just object dictionary")) {
      db->lockObjectDictionaryTag(argv[optind+1]);
    }

    else if (CCDB_COMMAND(allow_exec, cmd, "lockAllForObjectDictionaryTag", NOTAG, 1, 1, nargs, coral::Update, 
			  "\tidTag # locks the idTag\n\t\t\tand all subordinate tags"
			  )
	     ) {
      db->lockAllForObjectDictionaryTag(argv[optind+1]);
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "getExistingTags", NOTAG, 0, 0, nargs, coral::ReadOnly, "")) {
      IdTagList idTags;
      db->getExistingObjectDictionaryTags(idTags);
      cout<<"Object Dictionary tags: ";
      std::copy(idTags.begin(), idTags.end(), std::ostream_iterator<TagStatus>(cout, " "));
      cout<<endl<<endl;
      
      cout<<setw(80)<<setfill('=')<<left<<"Connectivity tags "<<endl;
      for(TagList::const_iterator it=idTags.begin(); it!=idTags.end(); ++it) {
	TagList connectivityTags;
	db->getExistingConnectivityTags(connectivityTags, it->tag() );
	cout<<"ObjDictTag ["<<it->tag()<<"] ==> ";
	printTags(connectivityTags);
	cout<<endl;
      }

      cout<<setw(80)<<setfill('=')<<left<<"Data tags "<<endl;
      for(TagList::const_iterator it=idTags.begin(); it!=idTags.end(); ++it) {
	TagList dataTags;
	db->getExistingDataTags(dataTags, it->tag());
	cout<<"ObjDictTag ["<<it->tag()<<"] ==> ";
	printTags(dataTags);
	cout<<endl;
      }

      cout<<setw(80)<<setfill('=')<<left<<"Alias tags "<<endl;
      for(TagList::const_iterator it=idTags.begin(); it!=idTags.end(); ++it) {
	TagList aliasTags;
	db->getExistingAliasTags(aliasTags, it->tag());
	cout<<"ObjDictTag ["<<it->tag()<<"] ==> ";
	printTags(aliasTags);
	cout<<endl;
      }

      cout<<"\n'*' denotes locked tags"<<endl;

    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "setHistoricTag", NOTAG, 0, 1, nargs, coral::Update, "\t[timeInSecondsSinceEpoch=now]"
			  "\n\t\t# All tags must be given via cmdline options.")) {
      time_t when = (argc-optind-1 > 0)? parseTimeSpec(argv[optind+1]) : time(0);
      db->setHistoricTag(optCompoundTag, when);
    }
    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if (CCDB_COMMAND(allow_exec, cmd, "getHistoricTag", NOTAG, 0, 1, nargs, coral::ReadOnly, "\t[timeInSecondsSinceEpoch=now] ")) {
      time_t when = (nargs > 0)? parseTimeSpec(argv[optind+1]) : time(0);
      cout<<db->getHistoricTag(when)<<endl;
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "deleteHistoryEntry", NOTAG, 1, 1, nargs, coral::Update, "\ttimeStampToBeDeleted")) {
      time_t when = parseTimeSpec(argv[optind+1]);
      db->deleteHistoryEntry(when);
    }
    else if (CCDB_COMMAND(allow_exec, cmd, "getHistoryTable", NOTAG, 0, 0, nargs, coral::ReadOnly, "\t# prints out all entries")) {
      HistoryTable h;
      db->getHistoryTable(h);
      for(HistoryTable::const_iterator i=h.begin(); i!=h.end(); ++i) {
	cout<<"Unix time = "<<i->first<<" => "<<i->second<<endl;
      }
    }

    else if(CCDB_ADDTEXT("\n")) {} //................................................................
    else if(CCDB_ADDTEXT("\n\t\t\tTo get time in seconds one can use"
			 "\n\t\t\tthe Linux \"date\" command, like"
			 "\n\t\t\tdate +%s --date 'June 27, 2007  17:22 CET'"
			 )
	    ) {}
    else if(CCDB_ADDTEXT("\n")) {} //................................................................

    else if(allow_exec) {
      cerr<<"Error: unknown command \""<<cmd<<"\"."<<endl
	  <<short_usage()<<endl;
      return 1;
    }
    else {
      // help
      cout<<long_usage()<<endl;
    }
  }
  //----------------------------------------------------------------
  catch ( coral::Exception& e ) {
    cerr << "*** FATAL: CORAL exception: " << e.what() << endl;
    return 1;
  }
  catch ( exception& e ) {
    cerr << "*** FATAL: Standard C++ exception: " << e.what() << endl;
    return 1;
  }
  catch( ... ) {
    cerr << "*** FATAL: Unknown exception ..." << endl;
    return 1;
  }

  return 0;
}
