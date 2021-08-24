/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include <vector>
#include <string>
#include <array>
#include <iostream>

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolApplication/Application.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/FieldSpecification.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IRecordIterator.h"
#include "CoolKernel/StorageType.h"

#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include "RelationalAccess/ConnectionService.h"

struct MyKey{
	cool::ValidityKey first;
	cool::ValidityKey second;
	cool::ChannelId channelId;
	
	MyKey():first(0),second(0),channelId(0){/** **/}
	MyKey(const cool::ValidityKey & f,const cool::ValidityKey & s, const cool::ChannelId & c):first(f),second(s),channelId(c){/** **/}
	bool operator<(const MyKey & rhs) const{
		if (channelId > rhs.channelId) return true;
		if ((channelId == rhs.channelId) and (first<rhs.first)) return true;
		if ((channelId == rhs.channelId) and (first==rhs.first) and (second<rhs.second)) return true;
		return false;
	}
};

//typedef std::pair<cool::ValidityKey, cool::ValidityKey> IovKey_t;
typedef MyKey IovKey_t;

typedef std::map<IovKey_t, CoraCoolObject::AttrListVec> CCIovVectorMap_t;

/**
	USAGE: openCoraCool.exe "COOLONL_SCT/COMP200"
**/
//conversion between types is going to be very manual
namespace Run1{
  namespace Chip{
	  typedef cool::Int16 chip_t;
	  typedef cool::Int16 active_t;
	  typedef cool::Int16 address_t;
	  typedef cool::Int16 config_t;
	  typedef cool::Int32 mask_t;
	  typedef cool::Int16 settings_t;
	  typedef cool::Int16 rc_t;
	  typedef cool::String255 rcArgs_t;
	  typedef cool::Float c_t;
	  typedef cool::Float target_t;
	  typedef cool::String255 trim_t;
	}
	// NOTE: Order of variables changed Run1/Run2 for Geog
	namespace Geog{
		typedef cool::Int32 super_t;
		typedef cool::Int32 medium_t;
		typedef cool::Int32 position_t;
		typedef cool::Int32 MUR_t; //<! last in run 1

	}
	namespace MUR{
		typedef cool::Int32 MUR_t;
		typedef cool::Int32 module_t;
		typedef cool::Int64 moduleId_t;
		typedef cool::Int32 xxFibre_t;
	}
	namespace Module{
		typedef cool::Int64 id_t;
		typedef cool::Int16 group_t;
		typedef cool::Int16 active_t;
		typedef cool::Int16 select_t;
	}
	namespace PowerMUR{
		typedef cool::Int32 MUR_t;
		typedef cool::Int16 module_t;
		typedef cool::Int16 crate_t;
		typedef cool::Int16 channel_t;
	}
	namespace Channel{
	  typedef cool::Int32 crate_t;
	  typedef cool::Int32 rod_t;
	  typedef cool::Int32 channel_t;
	  typedef cool::Int16 opCurrent_t;
	  typedef cool::Int16 opDelay_t;
    typedef cool::Int16 opMarkSpace_t;
    typedef cool::Int16 threshold_t;
    typedef cool::Int16 delay_t;
    typedef cool::Int32 errMask_t;
	}
	namespace ROD{
	  typedef cool::Int32 crate_t;
	  typedef cool::Int16 slot_t;
	  typedef cool::Int32 ROB_t;
	  typedef cool::Int32 SRC_t;
	  typedef cool::Int16 bcid_t;
	  typedef cool::Int32 slave_t;
	}
	namespace RODMUR{
	  typedef cool::Int32 crate_t;
		typedef cool::Int32 rod_t;
		typedef cool::Int32 position_t;
		typedef cool::Int32 MUR_t;
	}
	namespace Slave{
	  typedef cool::Int32 id_t;
		typedef cool::String255 file_t;
	}
	namespace TIM {
	  typedef cool::Int32 crate_t;
	  typedef cool::Float freq_t;
	  typedef cool::Int32 trigType_t;
	  typedef cool::Int32 bcidOffset_t;
	  typedef cool::Int32 trigEnable_t;
	  typedef cool::Int16 runMode_t;
	  typedef cool::Int16 delay_t;
	}
}

namespace Run2{
  namespace Chip{
	  typedef cool::UChar chip_t;
	  typedef cool::Bool active_t;
	  typedef cool::UChar address_t;
	  typedef cool::UInt16 config_t;
	  typedef cool::UInt32 mask_t;
	  typedef cool::UChar settings_t;
	  typedef cool::UChar rc_t;
	  typedef cool::String255 rcArgs_t;
	  typedef cool::Float c_t;
	  typedef cool::Float target_t;
	  typedef cool::String255 trim_t;
	}
	// NOTE: Order of variables changed Run1/Run2 for Geog
	namespace Geog{
		typedef cool::UInt32 MUR_t; //<! first in run 2
		typedef cool::Int16 super_t;
		typedef cool::UChar medium_t;
		typedef cool::Int16 position_t;
	}
	
	namespace MUR{
		typedef cool::UInt32 MUR_t;
		typedef cool::UChar module_t;
		typedef cool::Int64 moduleId_t;
		typedef cool::UChar xxFibre_t;
	}
	namespace Module{
		typedef cool::Int64 id_t;
		typedef cool::Int16 group_t;
		typedef cool::Bool active_t;
		typedef cool::Bool select_t;
		typedef cool::UInt16 chipStatus_t;
	}
	namespace PowerMUR{
		typedef cool::UInt32 MUR_t;
		typedef cool::UChar module_t;
		typedef cool::UChar crate_t;
		typedef cool::UChar channel_t;
	}
	namespace Channel{
	  typedef cool::UChar crate_t;
	  typedef cool::UChar rod_t;
	  typedef cool::UChar channel_t;
	  typedef cool::UChar opCurrent_t;
	  typedef cool::UInt16 opDelay_t;
    typedef cool::UChar opMarkSpace_t;
    typedef cool::UChar threshold_t;
    typedef cool::UChar delay_t;
    typedef cool::UInt32 errMask_t;
	}
	namespace ROD{
	  typedef cool::UChar crate_t;
	  typedef cool::UChar slot_t;
	  typedef cool::Int32 ROB_t;
	  typedef cool::Int32 SRC_t;
	  typedef cool::Int16 bcid_t;
	  typedef cool::Int32 slave_t;
	}
	namespace RODMUR{
	  typedef cool::UChar crate_t;
		typedef cool::UChar rod_t;
		typedef cool::UChar position_t;
		typedef cool::UInt32 MUR_t;
	}
	namespace Slave{
	  typedef cool::Int32 id_t;
		typedef cool::String255 file_t;
	}
	namespace TIM {
	  typedef cool::UChar crate_t;
	  typedef cool::Float freq_t;
	  typedef cool::UInt32 trigType_t;
	  typedef cool::UInt16 bcidOffset_t;
	  typedef cool::UInt32 trigEnable_t;
	  typedef cool::UChar runMode_t;
	  typedef cool::UInt16 delay_t;
	}
}
//http://acode-browser.usatlas.bnl.gov/lxr/source/atlas/Simulation/RunDependentSim/RunDependentSimComps/python/RunDMCFlags.py
/**  
https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers 
152777:1270866809, ##2010 7 TeV p-p for period A (very low mu)
154471:1272831478, ##2010 900 GeV p-p
155697:1274722685, ##2010 7 TeV p-p for period C (mu~0.2)
162623:1282673387, ##2010 7 TeV p-p for period F (36 single bunches, mu~2)
167576:1287909620, ##2010 7 TeV p-p (150ns bunch trains)
170002:1291063245, ##2010 Heavy Ion run
178211:1301070988, ##2011 2.76 TeV
180164:1303554669, ##2011 7 TeV p-p (B-D)
183003:1306973316, ##2011 7 TeV p-p (E-H)
186169:1311379706, ##2011 7 TeV p-p (I-K)
189751:1316868299, ##2011 7 TeV p-p (L-M)
193662:1322075751, ##2011 Heavy Ion run
195847:1328040250, ##Upgrade samples from MC12
210184:1347470747, ##2012 Heavy Ion - proton run
212272:1349603811, ##2012 8 TeV p-p
219114:1360457560, ##2013 Heavy Ion - proton run
219305:1360689056, ##2013 2.76 TeV p-p
222222:1376009561, ##DC14 (mu=30) 25ns, old R1 geo tag
222250:1376075781, ##DC14 (mu=30) 50ns, old R1 geo tag
222500:1376646281, ##MC15 50ns, new R2 geo tag (mu=0)
222510:1376669101, ##MC15 50ns, new R2 geo tag (nominal mu)
222525:1376703331  ##MC15 25ns, new R2 geo tag (nominal mu)

From this, looking at the tags in -MC-06:
0-1230768000000000000 : 0-142308; ->Thu, 01 Jan 2009 00:00:00 GMT
1230768000000000000-1262304000000000000 : ->Fri, 01 Jan 2010 00:00:00 GMT 142308 - 152166	
1262304000000000000-1288569600000000000 : ->Mon, 01 Nov 2010 00:00:00 GMT 152777,154471,155697,162623,167576; 152166 - 153599
1288569600000000000-1290686400000000000 : ->Thu, 25 Nov 2010 12:00:00 GMT ? not attributed 170000
1290686400000000000-1293840000000000000 : ->Sat, 01 Jan 2011 00:00:00 GMT 170002
1293840000000000000-1333588404000000000 : ->Thu, 05 Apr 2012 01:13:24 GMT 178211,180164,183003,186169,189751,193662,195847-200805;178163 - 178264
1333588404000000000-1333612447000000000 : ->Thu, 05 Apr 2012 07:54:07 GMT 200805-216432
1333612447000000000-1356998400000000000 : ->Tue, 01 Jan 2013 00:00:00 GMT 210184,212272
1356998400000000000-9223372036854775807 : 219114,219305 

**/



typedef std::pair<cool::ValidityKey, cool::ValidityKey> IovPeriod_t;
IovKey_t
makeRunInterval(const cool::ValidityKey runstart, const cool::ValidityKey runend,const cool::ChannelId channelId ){
  cool::ValidityKey s =runstart<<32LL;
  if (runend==cool::ValidityKeyMax) return IovKey_t(s, cool::ValidityKeyMax,channelId);
  cool::ValidityKey e =(runend+1LL)<<32LL;
  return IovKey_t(s, e,channelId);
}

bool inInterval(const cool::ValidityKey n, const cool::ValidityKey from, const cool::ValidityKey to){
	return ((n>from) and (n<=to));
}
cool::ValidityKey
convertUntil(const cool::ValidityKey & until){
  if (until ==0) return 140000;
	if (until==1230768000000000000) return 139999;
	if (until==1262304000000000000) return 149999;
	if (until==1288569600000000000) return 167999;
  if (until==1290686400000000000) return 169799;
  if (until==1293840000000000000) return 170999;
  
  if (until==1333588404000000000) return 200804;
  if (until==1333612447000000000) return 200805;
  
  if (until==1356998400000000000) return 216999;
   return cool::ValidityKeyMax;
}

cool::ValidityKey
convertSince(const cool::ValidityKey & since){
  if (since==0) return 0;
	if (since==1230768000000000000) return 140000;
	if (since==1262304000000000000) return 150000;
  if (since==1288569600000000000) return 168000;
  if (since==1290686400000000000) return 169800;
  if (since==1293840000000000000) return 171000;
  
  if (since==1333588404000000000) return 200805;
  
  if (since==1333612447000000000) return 200806;
  return 217000;
}


IovKey_t
time2RunLumi(const IovKey_t & iov){
  const auto since=iov.first;
  const auto until=iov.second;
  const auto channelId = iov.channelId;

  return makeRunInterval(convertSince(since),convertUntil(until),channelId);
}
  
//function to check the input arguments to main()
bool
argumentsOK(const int numArgs, const char **/**argv[]**/){
	bool result(true);
	result = (numArgs>1) and (numArgs<4);
	return result;
}


// translate simple connection string (no slash) to mycool.db with given
// instance name, all others are left alone
std::string 
transConn(const std::string& inconn) {
  std::string result(inconn);
  if (inconn.find('/')==std::string::npos) {
    result= "sqlite://X;schema=/tmp/sroe/mycool.db;dbname="+inconn;
  } 
  return result;
}

//RAII context carrying the database connection
class DbConnection{
public:
  DbConnection(const std::string & sourceDb,const bool isCoraCool=false);
  ~DbConnection();
  bool isOpen() const;
  cool::IDatabasePtr dbPtr() const;
  CoraCoolDatabasePtr coraCoolPtr() const;
  
private:
  coral::ConnectionService m_coralsvc;
  cool::Application m_coolapp;
  const cool::IDatabaseSvc & m_dbSvc;
  cool::IDatabasePtr m_sourceDbPtr;
  CoraCoolDatabasePtr m_sourceCoraPtr;
  bool m_isOpen;
};

//is the descriptor that of a CoraCool-style folder?
bool 
isCoraCoolDescriptor(const std::string & descriptor){
	return (descriptor.find("<coracool") != std::string::npos);
}

//make db connection upon construction
DbConnection::DbConnection(const std::string & sourceDb, const bool isCoraCool):
    m_coralsvc{},
    m_coolapp(&m_coralsvc),
    m_dbSvc(m_coolapp.databaseService()),
    m_sourceDbPtr(nullptr),
    m_sourceCoraPtr(nullptr),
    m_isOpen(false){
  bool readOnly(true);
  const std::string dbConnectionString=transConn(sourceDb);
  readOnly= (dbConnectionString.find ("sqlite")==std::string::npos);
  try {
    m_sourceDbPtr=m_dbSvc.openDatabase(dbConnectionString,readOnly);
    if (isCoraCool) m_sourceCoraPtr=CoraCoolDatabasePtr(new CoraCoolDatabase(sourceDb,
       m_sourceDbPtr,m_coralsvc,readOnly));
  }
  catch (std::exception& e) {
    std::cout << "Cool exception caught: " << e.what() << std::endl;
    if (not readOnly){
      try {
        std::cout<<"creating "<<dbConnectionString<<std::endl;
        m_sourceDbPtr=m_dbSvc.createDatabase(dbConnectionString);
      } catch (std::exception& e){
        std::cout << "Cool exception caught: " << e.what() << std::endl;
      }
    }
  }
  m_isOpen=(m_sourceDbPtr!=nullptr);
}

//break db connection on destruction
DbConnection::~DbConnection(){
  if (isOpen()){
	  m_sourceDbPtr->closeDatabase();
	}
}

//return bare db pointer
cool::IDatabasePtr
DbConnection::dbPtr() const{
  return m_sourceDbPtr;
}

//return coracool style db pointer
CoraCoolDatabasePtr
DbConnection::coraCoolPtr() const{
  return m_sourceCoraPtr;
}

bool
DbConnection::isOpen() const{
  return m_isOpen;
}


std::string
convertTagToRun2(const std::string & run1Tag){
  std::string tag=run1Tag;
  tag.erase(12,7);
  return tag;
}



//specification to identify a given folder from full name and tag
struct FolderSpec{
  const std::string name;
	const std::string tag;
  FolderSpec(const std::string & thename, const std::string & thetag):name(thename),tag(thetag){
  /**nop**/};
};


//Folder interface class
class Folder{
public:
  Folder(const DbConnection & theConnection, const FolderSpec & theFolder);
  bool isValid() const;
  std::string description() const;
  FolderSpec folderSpec() const;
  cool::RecordSpecification foreignKeySpec();
  cool::RecordSpecification payloadSpec() const; //!< specification of the payload entries
  bool isSingleVersion() const;
  std::string tag() const;
  const CoraCoolFolderPtr & ccPtr() const;
  const cool::IFolderPtr & ptr() const;
private:
  const DbConnection & m_connection;
  const FolderSpec m_folderSpec;
  cool::IFolderPtr m_folderPtr;
  CoraCoolFolderPtr m_sourceflc;
  bool m_isCoraCool;
  bool checkWhetherCoraCool() const;
};

Folder::Folder(const DbConnection & theConnection, const FolderSpec & theFolder)
  :m_connection(theConnection),m_folderSpec(theFolder),m_folderPtr{},
  m_sourceflc{},m_isCoraCool(false){
  try{
    m_folderPtr=m_connection.dbPtr()->getFolder(theFolder.name);//!< get the cool folder
    m_isCoraCool=checkWhetherCoraCool();
    if (m_isCoraCool) m_sourceflc=m_connection.coraCoolPtr()->getFolder(theFolder.name);//!< get the data vector folder
  } catch (cool::Exception & e){
    std::cout<<" Could not get folder "<<theFolder.name<<std::endl;
  }
}

FolderSpec
Folder::folderSpec() const{
	return m_folderSpec;
}

const cool::IFolderPtr &
Folder::ptr() const{
  return m_folderPtr;
}

//assume the coracool folder is valid if the pointer to it is valid
bool
Folder::isValid() const{
  bool result = (m_folderPtr != nullptr);
	if (m_isCoraCool) result &=(m_sourceflc != nullptr);
	return result;
}

bool
Folder::isSingleVersion() const{
  return (cool::FolderVersioning::SINGLE_VERSION == m_folderPtr->versioningMode());
}

std::string
Folder::description() const{
	return (m_folderPtr->description());
}

bool
Folder::checkWhetherCoraCool() const{
	return isCoraCoolDescriptor(description());
}

cool::RecordSpecification 
Folder::foreignKeySpec(){
	if (m_sourceflc==nullptr) return cool::RecordSpecification();
  return m_sourceflc->fkSpecification();
}

cool::RecordSpecification 
Folder::payloadSpec() const{
  if (m_isCoraCool){
    return m_sourceflc->payloadSpecification();
  }
  return m_folderPtr->payloadSpecification();
}

std::string
Folder::tag() const{
  return m_folderSpec.tag;
}

const CoraCoolFolderPtr & 
Folder::ccPtr() const{
  return m_sourceflc;
}

//Criterion for selecting a particular set of data vectors in the folder
struct SelectionCriterion{
  SelectionCriterion(const cool::ChannelSelection & c, const cool::ValidityKey & s, const cool::ValidityKey & u):
  channels(c), since(s),until(u){
  /**nop**/
  }
	const cool::ChannelSelection channels;
	const cool::ValidityKey since;
	const cool::ValidityKey until;
};

//Read one vector of attribute lists
CoraCoolObject::AttrListVec
readOneDatum(const CoraCoolObjectPtr & ccPtr){
	if (ccPtr->size()>0){
	  CoraCoolObject::const_iterator i(ccPtr->begin());
	  CoraCoolObject::const_iterator end(ccPtr->end());
	  CoraCoolObject::AttrListVec result(i,end);
	  return result;
	}
	CoraCoolObject::AttrListVec emptyArray;
	return emptyArray;
}



//read a selection of vectors of attribute lists in the given folder
CCIovVectorMap_t
readCC(const Folder & theFolder, const SelectionCriterion & choice, const unsigned int limit=10){
  theFolder.ccPtr()->setPrefetchAll(true);
  CoraCoolObjectIterPtr sourceitr;
  if (theFolder.isSingleVersion()) {
	  sourceitr=theFolder.ccPtr()->browseObjects(choice.since, choice.until, choice.channels);
  } else {
	  sourceitr=theFolder.ccPtr()->browseObjects(choice.since, choice.until, choice.channels, theFolder.tag());
  }
  unsigned int counter(0);
  std::map<IovKey_t, CoraCoolObject::AttrListVec> mapOfVectorsByIov;
  while (sourceitr->hasNext() and (counter<limit)) {
    CoraCoolObjectPtr obj=sourceitr->next();
    IovKey_t iov(obj->since(),obj->until(),obj->channelId());
    mapOfVectorsByIov[iov]=readOneDatum(obj);
    ++counter;
  }
  return mapOfVectorsByIov;
}

typedef std::map<IovKey_t, cool::IRecordVector> IovVectorMap_t;

IovVectorMap_t
read(const Folder & theFolder, const SelectionCriterion & choice, const unsigned int limit=10){
  theFolder.ptr()->setPrefetchAll(false);
  cool::IObjectIteratorPtr sourceitr;
  if (theFolder.isSingleVersion()) {
	  sourceitr=theFolder.ptr()->browseObjects(choice.since, choice.until, choice.channels);
  } else {
	  sourceitr=theFolder.ptr()->browseObjects(choice.since, choice.until, choice.channels, theFolder.tag());
  }
  unsigned int counter(0);
  std::map<IovKey_t, cool::IRecordVector> mapOfVectorsByIov;
  while (sourceitr->goToNext() and counter<limit) {
    const cool::IObject& sobj=sourceitr->currentRef();
    cool::IRecordIterator& spitr=sobj.payloadIterator();
    const cool::IRecordVectorPtr svptr=spitr.fetchAllAsVector();
    cool::IRecordVector::const_iterator svitr=svptr->begin();
		cool::IRecordVector::const_iterator svend=svptr->end();
    IovKey_t iov(sobj.since(),sobj.until(),sobj.channelId());
    mapOfVectorsByIov[iov]=*svptr;
    ++counter;
  }
  return mapOfVectorsByIov;
}
/**
IovKey_t
runLumiFromTimeStamp(const IovKey_t & timeStamp){
	return timeStamp;
}
**/



template<class R2, class R1>
void
setRecord(cool::Record & r, const coral::AttributeList & a, const std::string & itemName, const std::string &targetName){
   static bool warning(true);
   try{
      if (not a[itemName].isNull()){
        const R1 attribute =a[itemName].data<R1>();
        const R2 field = static_cast<R2>(attribute);
        if (attribute != static_cast<R1>(field)) std::cout<<"Cast value mismatch, attr.="<<attribute<<", field="<<field<<std::endl;
  	 	  r[targetName].setValue<R2>(attribute);
  	  } else {
  	    r[targetName].setNull();
  	  }
  	} catch (coral::AttributeException & e) {
  	  if (warning) std::cout<<itemName<<" gives exception"<<std::endl;
  	  warning=true;
  	}
}

template<class R2, class R1>
void
setRecord(cool::Record & r, const coral::AttributeList & a, const std::string & itemName){
   setRecord<R2,R1>(r,a,itemName,itemName);
}


void 
writeChip(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
	 	setRecord<Run2::Chip::chip_t, Run1::Chip::chip_t>(r,attrList, "chip");
	 	setRecord<Run2::Chip::active_t, Run1::Chip::active_t>(r,attrList, "active");
	 	setRecord<Run2::Chip::address_t, Run1::Chip::address_t>(r,attrList, "address");
	 	setRecord<Run2::Chip::config_t, Run1::Chip::config_t>(r,attrList, "config");
		setRecord<Run2::Chip::mask_t, Run1::Chip::mask_t>(r,attrList, "mask0");
	 	setRecord<Run2::Chip::mask_t, Run1::Chip::mask_t>(r,attrList, "mask1");
	 	setRecord<Run2::Chip::mask_t, Run1::Chip::mask_t>(r,attrList, "mask2");
	 	setRecord<Run2::Chip::mask_t, Run1::Chip::mask_t>(r,attrList, "mask3");
	 	setRecord<Run2::Chip::settings_t, Run1::Chip::settings_t>(r,attrList, "vthr");
	  setRecord<Run2::Chip::settings_t, Run1::Chip::settings_t>(r,attrList, "vcal");
	 	setRecord<Run2::Chip::settings_t, Run1::Chip::settings_t>(r,attrList, "delay");
	 	setRecord<Run2::Chip::settings_t, Run1::Chip::settings_t>(r,attrList, "preamp");
	 	setRecord<Run2::Chip::settings_t, Run1::Chip::settings_t>(r,attrList, "shaper");
	 	setRecord<Run2::Chip::rc_t, Run1::Chip::rc_t>(r,attrList, "rc_function");
	 	setRecord<Run2::Chip::rcArgs_t, Run1::Chip::rcArgs_t>(r, attrList,"rc_args");
	 	setRecord<Run2::Chip::c_t, Run1::Chip::c_t>(r, attrList,"c_factor");
    setRecord<Run2::Chip::target_t, Run1::Chip::target_t>(r,attrList,"target");
    setRecord<Run2::Chip::trim_t, Run1::Chip::trim_t>(r,attrList,"trim");
    
	 	const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 //std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 	// std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,(runLumi.channelId-1),tag );
	}
}
void 
writeGeog(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::Geog::MUR_t, Run1::Geog::MUR_t>(r,attrList,"MUR");
    setRecord<Run2::Geog::super_t, Run1::Geog::super_t>(r,attrList,"super");
    setRecord<Run2::Geog::medium_t, Run1::Geog::medium_t>(r,attrList,"medium");
    setRecord<Run2::Geog::position_t, Run1::Geog::position_t>(r,attrList,"position");

	 	const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;
	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,runLumi.channelId,tag );
	}
}
void 
writeMUR(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::MUR::MUR_t, Run1::MUR::MUR_t>(r,attrList,"MUR");
    setRecord<Run2::MUR::module_t, Run1::MUR::module_t>(r,attrList,"module");
    setRecord<Run2::MUR::moduleId_t, Run1::MUR::moduleId_t>(r,attrList,"moduleID");
    setRecord<Run2::MUR::moduleId_t, Run1::MUR::moduleId_t>(r,attrList,"rmoduleID");
    setRecord<Run2::MUR::xxFibre_t, Run1::MUR::xxFibre_t>(r,attrList,"rx0Fibre");
    setRecord<Run2::MUR::xxFibre_t, Run1::MUR::xxFibre_t>(r,attrList,"rx1Fibre");
    setRecord<Run2::MUR::xxFibre_t, Run1::MUR::xxFibre_t>(r,attrList,"txFibre");
	 	const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,runLumi.channelId,tag );
	}
}
void 
writeModule(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::Module::id_t, Run1::Module::id_t>(r,attrList,"id");
    setRecord<Run2::Module::group_t, Run1::Module::group_t>(r,attrList,"group","slaveGroup");
    setRecord<Run2::Module::active_t, Run1::Module::active_t>(r,attrList,"active");
    setRecord<Run2::Module::select_t, Run1::Module::select_t>(r,attrList,"select","clockSelect");
    //the chipStatus is only in run 2
    r["chipStatus"].setValue<Run2::Module::chipStatus_t>(static_cast<Run2::Module::chipStatus_t>(0x0FFF));
	 	const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 //std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 //	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,(runLumi.channelId-1),tag );
	}
}

void 
writeChannel(const CCIovVectorMap_t & data, const Folder & destination, const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::Channel::crate_t, Run1::Channel::crate_t>(r,attrList,"crate");
    setRecord<Run2::Channel::rod_t, Run1::Channel::rod_t>(r,attrList,"rod");
    setRecord<Run2::Channel::channel_t, Run1::Channel::channel_t>(r,attrList,"channel");
    setRecord<Run2::Channel::opCurrent_t, Run1::Channel::opCurrent_t>(r,attrList,"outputCurrent");
    setRecord<Run2::Channel::opDelay_t, Run1::Channel::opDelay_t>(r,attrList,"outputDelay");
    setRecord<Run2::Channel::opMarkSpace_t, Run1::Channel::opMarkSpace_t>(r,attrList,"outputMarkSpace");
    setRecord<Run2::Channel::threshold_t, Run1::Channel::threshold_t>(r,attrList,"stream0Threshold");
    setRecord<Run2::Channel::delay_t, Run1::Channel::delay_t>(r,attrList,"stream0Delay");
    setRecord<Run2::Channel::errMask_t, Run1::Channel::errMask_t>(r,attrList,"stream0ErrMask");
    setRecord<Run2::Channel::threshold_t, Run1::Channel::threshold_t>(r,attrList,"stream1Threshold");
    setRecord<Run2::Channel::delay_t, Run1::Channel::delay_t>(r,attrList,"stream1Delay");
    setRecord<Run2::Channel::errMask_t, Run1::Channel::errMask_t>(r,attrList,"stream1ErrMask");	 
    const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 //std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 //	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,runLumi.channelId,tag );
	}
}
void 
writeRod(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::ROD::crate_t, Run1::ROD::crate_t>(r,attrList,"crate");
    setRecord<Run2::ROD::slot_t, Run1::ROD::slot_t>(r,attrList,"slot");
    setRecord<Run2::ROD::ROB_t, Run1::ROD::ROB_t>(r,attrList,"ROB");
    setRecord<Run2::ROD::SRC_t, Run1::ROD::SRC_t>(r,attrList,"SRCid");
    setRecord<Run2::ROD::bcid_t, Run1::ROD::bcid_t>(r,attrList,"BCIDOffset");
    setRecord<Run2::ROD::slave_t, Run1::ROD::slave_t>(r,attrList,"slave0");
    setRecord<Run2::ROD::slave_t, Run1::ROD::slave_t>(r,attrList,"slave1");
    setRecord<Run2::ROD::slave_t, Run1::ROD::slave_t>(r,attrList,"slave2");
    setRecord<Run2::ROD::slave_t, Run1::ROD::slave_t>(r,attrList,"slave3");
    const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,runLumi.channelId,tag );
	}
}    

void 
writeRodmur(const CCIovVectorMap_t & data, const Folder & destination,const std::string &tag){
	//assumed monte carlo, translate IOVs 
	std::cout<<"data size "<<data.size()<<std::endl;
	for (const auto &iov: data){
	 const IovKey_t & runLumi = time2RunLumi(iov.first);
	 const CoraCoolObject::AttrListVec & thisVectorPayload = iov.second;
	 //its going to be a Cool Vector Payload
	 cool::RecordSpecification rSpec = destination.payloadSpec();
	 cool::IRecordVector cvp;
	 for (auto const & attrList:thisVectorPayload){
	  cool::Record r(rSpec);
    setRecord<Run2::RODMUR::crate_t, Run1::RODMUR::crate_t>(r,attrList,"crate");
    setRecord<Run2::RODMUR::rod_t, Run1::RODMUR::rod_t>(r,attrList,"rod");
    setRecord<Run2::RODMUR::position_t, Run1::RODMUR::position_t>(r,attrList,"position");
    setRecord<Run2::RODMUR::MUR_t, Run1::RODMUR::MUR_t>(r,attrList,"MUR");
    const auto pRec=std::make_shared<cool::Record>(r);
	 	cvp.push_back(pRec);
	 }
	 std::cout<<iov.first.first<<"-"<<iov.first.second<<" : "<<iov.first.channelId<<std::endl;
	 std::cout<<">"<<runLumi.first<<"-"<<runLumi.second<<" : "<<runLumi.channelId<<std::endl;

	 destination.ptr()->storeObject(runLumi.first,runLumi.second,cvp,runLumi.channelId,tag );
	}
}    
//print specification for a cool record as TWiki table markup
void 
printRecordSpec(const cool::RecordSpecification & spec){
  const std::string s("|");//TWiki table separator
  std::cout<<"|*Payload Name*|*COOL Type*|"<<std::endl;
  for (unsigned int i(0);i!=spec.size();++i){
    const cool::IFieldSpecification& f=spec[i];
    std::cout<<s<<f.name()<<s<<f.storageType().name()<<s<<std::endl;
  }
}


const coral::AttributeList &
getAttributes(const coral::AttributeList & vectorEntry){
  return vectorEntry;
}
const coral::AttributeList &
getAttributes(const cool::IRecordPtr & vectorEntry){
  return vectorEntry->attributeList();
}


template <class T>
void
showResult(const T & theResult, const unsigned int maxShown=5){
	for (const auto & i:theResult){
	  const IovKey_t &iov=i.first;
	  typename T::mapped_type dataVector=i.second;
		std::cout<<"IOV: "<<iov.first<<","<<iov.second<<std::endl;
		std::cout<<"Data: "<<std::endl;
		const unsigned int numEntries=dataVector.size();
		const unsigned int numberToShow=std::min(numEntries,maxShown);
		for(unsigned int j(0);j!=numberToShow;++j){
			const coral::AttributeList & thisEntry=getAttributes(dataVector[j]);
			thisEntry.toOutputStream(std::cout);
			std::cout<<std::endl;
		}
	}
}


/**
cool::StorageType::TypeId
coral2CoolType(const std::string & coralTypeName){
 std::vector<std::string> vec ={
 "Bool",
"UChar",
"Int16",
"UInt16",
"Int32",
"UInt32",
"UInt63",
"Int64",
"Float",
"Double",
"String255",
"String4k",
"String64k",
"String16M",
"Blob64k",
"Blob16M"
 };
 std::vector<std::string>::iterator it=std::find(vec.begin(),vec.end(),coralTypeName);
 return static_cast<cool::StorageType::TypeId>(std::distance(vec.begin(), it));
}
**/

std::string
defaultFolderPath(const bool itsCoraCool){
  const std::string result = itsCoraCool?("/SCT/DAQ/Configuration/"):("/SCT/DAQ/Config/");
  return result;
}

bool
 replaceTimestamp(std::string & metadata){
	std::string::size_type p1,p2;
	p1=metadata.find("<timeStamp>")+11;
	p2=metadata.find("</timeStamp>");
	const bool valid= ((p1<p2) and (p2!=std::string::npos));
  if (valid) metadata.replace(p1,p2-11,"time");
  return valid;
}

cool::IFolderPtr
createLocalCopyStructure(const Folder & sourceFolder, const DbConnection & writeDb, const bool forceTimestamp=false){
	std::string description=sourceFolder.description();
	
	bool replacementMade(false);
	if (forceTimestamp){
	  replacementMade=replaceTimestamp(description);
	}
	if (replacementMade) std::cout<<"timestamp replacement made\n"<<description<<std::endl;
	
	const std::string destFolder=sourceFolder.folderSpec().name;
	cool::IFolderPtr pSourceFolder=sourceFolder.ptr();
	const cool::FolderVersioning::Mode dvermode=cool::FolderVersioning::MULTI_VERSION;
	const cool::PayloadMode::Mode payMode=pSourceFolder->folderSpecification().payloadMode();
	const cool::FolderSpecification coolFolderSpec(dvermode,pSourceFolder->payloadSpecification(),payMode);
  cool::IFolderPtr destfl=writeDb.dbPtr()->createFolder(destFolder,coolFolderSpec,description,true);
  std::cout<<"Folder "<<destFolder<<" created."<<std::endl;
  //copy channels
  const std::map<cool::ChannelId,std::string> & chanmap=pSourceFolder->listChannelsWithNames();
  for (const auto & chan:chanmap){
    std::cout<<".";
    const std::string & sourcedesc=pSourceFolder->channelDescription(chan.first);
    const std::string& sourcename=chan.second;
    destfl->createChannel(chan.first,sourcename,sourcedesc);
  }
  std::cout<<std::endl;
  return destfl;
}

std::vector<std::string> &
split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> 
split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int main(int argc, const char * argv[]) {
  int retcode=0;
  //http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/Chip/tags
  //http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Config/Chip/tags
  /**
  xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/Chip/tags'
<?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/Chip" version="multi">
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:48:26 GMT">SctDaqConfigurationChip-001-01</tag>
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:54:50 GMT">SctDaqConfigurationChip-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:23:33 GMT">SctDaqConfigurationChip-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:49:49 GMT">SctDaqConfigurationChip-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 15:08:24 GMT">SctDaqConfigurationChip-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:31:49 GMT">SctDaqConfigurationChip-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:27:49 GMT">SctDaqConfigurationChip-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:56:21 GMT">SctDaqConfigurationChip-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:35:01 GMT">SctDaqConfigurationChip-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:55:47 GMT">SctDaqConfigurationChip-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:13:55 GMT">SctDaqConfigurationChip-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:54:21 GMT">SctDaqConfigurationChip-MC-06</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:46:51 GMT">SctDaqConfigurationChip-NOMINAL</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:43:34 GMT">SctDaqConfigurationChip-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:41:56 GMT">SctDaqConfigurationChip-NOMINAL-Jan09</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 12:44:54 GMT">SctDaqConfigurationChip-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:47:13 GMT">SctDaqConfigurationChip-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:45:10 GMT">SctDaqConfigurationChip-Nov10-02</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:45:11 GMT">SctDaqConfigurationChip-PERFECT</tag>
  <tag lock="locked" insertionTime="Mon, 30 Aug 2010 13:03:13 GMT">SctDaqConfigurationChip-PERFECT-Aug10-01</tag>
</tagList>
  **/
  std::array<std::string,2> chipTagNames={"SctDaqConfigChip-Run2-10July2015","SctDaqConfigurationChip-MC-06"};
  //xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/Geog/tags'
/**
  <?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/Geog" version="multi">
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:29:27 GMT">SctDaqConfigurationGeog-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:18:58 GMT">SctDaqConfigurationGeog-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:42:07 GMT">SctDaqConfigurationGeog-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 10:57:58 GMT">SctDaqConfigurationGeog-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:24:46 GMT">SctDaqConfigurationGeog-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:23:01 GMT">SctDaqConfigurationGeog-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:52:06 GMT">SctDaqConfigurationGeog-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:30:24 GMT">SctDaqConfigurationGeog-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:45:04 GMT">SctDaqConfigurationGeog-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:08:40 GMT">SctDaqConfigurationGeog-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:41:15 GMT">SctDaqConfigurationGeog-MC-06</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:58:20 GMT">SctDaqConfigurationGeog-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:58:20 GMT">SctDaqConfigurationGeog-NOMINAL-Jan09</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:58:20 GMT">SctDaqConfigurationGeog-NOMINAL-Sept08</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 10:12:06 GMT">SctDaqConfigurationGeog-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:41:46 GMT">SctDaqConfigurationGeog-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:38:02 GMT">SctDaqConfigurationGeog-Nov10-02</tag>
  <tag lock="locked" insertionTime="Mon, 30 Aug 2010 13:04:12 GMT">SctDaqConfigurationGeog-PERFECT-Aug10-01</tag>
</tagList>
**/
  std::array<std::string,2> geogTagNames={"SctDaqConfigGeog-Run2-10July2015","SctDaqConfigurationGeog-MC-06"};
  //xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/MUR/tags'
/**
<?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/MUR" version="multi">
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:32:24 GMT">SctDaqConfigurationMur-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:20:34 GMT">SctDaqConfigurationMur-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:44:03 GMT">SctDaqConfigurationMur-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 10:59:06 GMT">SctDaqConfigurationMur-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:27:33 GMT">SctDaqConfigurationMur-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:24:33 GMT">SctDaqConfigurationMur-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:53:46 GMT">SctDaqConfigurationMur-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:31:43 GMT">SctDaqConfigurationMur-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:50:58 GMT">SctDaqConfigurationMur-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:10:28 GMT">SctDaqConfigurationMur-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:43:17 GMT">SctDaqConfigurationMur-MC-06</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:58:27 GMT">SctDaqConfigurationMur-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:58:26 GMT">SctDaqConfigurationMur-NOMINAL-Jan09</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 09:58:26 GMT">SctDaqConfigurationMur-NOMINAL-Sept08</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 10:19:32 GMT">SctDaqConfigurationMur-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:43:36 GMT">SctDaqConfigurationMur-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:38:55 GMT">SctDaqConfigurationMur-Nov10-02</tag>
  <tag lock="locked" insertionTime="Mon, 30 Aug 2010 13:04:12 GMT">SctDaqConfigurationMur-PERFECT-Aug10-01</tag>
</tagList>
**/
  std::array<std::string,2> murTagNames={"SctDaqConfigMur-Run2-10July2015","SctDaqConfigurationMur-MC-06"};
    //xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/Module/tags'
/**
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/Module" version="multi">
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 10:04:53 GMT">SctDaqConfigurationModule-001-01</tag>
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:33:35 GMT">SctDaqConfigurationModule-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:21:34 GMT">SctDaqConfigurationModule-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:45:17 GMT">SctDaqConfigurationModule-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 10:59:56 GMT">SctDaqConfigurationModule-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:28:45 GMT">SctDaqConfigurationModule-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:25:25 GMT">SctDaqConfigurationModule-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:54:35 GMT">SctDaqConfigurationModule-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:32:38 GMT">SctDaqConfigurationModule-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:52:52 GMT">SctDaqConfigurationModule-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:11:30 GMT">SctDaqConfigurationModule-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:45:49 GMT">SctDaqConfigurationModule-MC-06</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 10:03:30 GMT">SctDaqConfigurationModule-NOMINAL</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:00:41 GMT">SctDaqConfigurationModule-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:59:18 GMT">SctDaqConfigurationModule-NOMINAL-Jan09</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 10:23:22 GMT">SctDaqConfigurationModule-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:44:30 GMT">SctDaqConfigurationModule-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:40:57 GMT">SctDaqConfigurationModule-Nov10-02</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:02:07 GMT">SctDaqConfigurationModule-PERFECT</tag>
  <tag lock="locked" insertionTime="Tue, 31 Aug 2010 13:57:26 GMT">SctDaqConfigurationModule-PERFECT-Aug10-01</tag>
</tagList>
**/
  std::array<std::string,2> moduleTagNames={"SctDaqConfigModule-Run2-10July2015","SctDaqConfigurationModule-MC-06"};
  //xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/PowerMUR/tags'
  //std::array<std::string,2> powerMurTagNames={"SctDaqConfigPowermMur-Run2-10July2015","SctDaqConfigurationPowerMur-MC-06"};
	/** 
	xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/Channel/tags' 
<?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/Channel" version="multi">
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:40:49 GMT">SctDaqConfigurationChannel-001-01</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 09:40:50 GMT">SctDaqConfigurationChannel-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 30 Aug 2010 13:02:39 GMT">SctDaqConfigurationChannel-PERFECT-Aug10-01</tag>
</tagList>
**/
	std::array<std::string,2> channelTagNames={"SctDaqConfigChannel-Run2-10July2015","SctDaqConfigurationChannel-MC-06"};
/**
xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/ROD/tags'    
<?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/ROD" version="multi">
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:30:59 GMT">SctDaqConfigurationRod-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:19:51 GMT">SctDaqConfigurationRod-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:43:26 GMT">SctDaqConfigurationRod-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 10:58:30 GMT">SctDaqConfigurationRod-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:26:05 GMT">SctDaqConfigurationRod-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:23:40 GMT">SctDaqConfigurationRod-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:52:54 GMT">SctDaqConfigurationRod-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:30:56 GMT">SctDaqConfigurationRod-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:48:21 GMT">SctDaqConfigurationRod-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:09:14 GMT">SctDaqConfigurationRod-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:42:07 GMT">SctDaqConfigurationRod-MC-06</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:06:26 GMT">SctDaqConfigurationRod-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 10:06:25 GMT">SctDaqConfigurationRod-NOMINAL-Jan09</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:06:25 GMT">SctDaqConfigurationRod-NOMINAL-Sept08</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 10:15:21 GMT">SctDaqConfigurationRod-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:42:51 GMT">SctDaqConfigurationRod-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:38:40 GMT">SctDaqConfigurationRod-Nov10-02</tag>
  <tag lock="locked" insertionTime="Tue, 31 Aug 2010 13:03:27 GMT">SctDaqConfigurationRod-PERFECT-Aug10-01</tag>
</tagList>
**/
  	std::array<std::string,2> rodTagNames={"SctDaqConfigRod-Run2-10July2015","SctDaqConfigurationRod-MC-06"};
/**
 xmllint --format 'http://coolcherrypy.cern.ch:8080/cooldb/ATLAS_COOLPROD/ATLAS_COOLOFL_SCT/OFLP200/SCT/DAQ/Configuration/RODMUR/tags'
<?xml version="1.0" encoding="utf-8"?>
<tagList server="ATLAS_COOLPROD" schema="ATLAS_COOLOFL_SCT" dbname="OFLP200" folder="SCT/DAQ/Configuration/RODMUR" version="multi">
  <tag lock="locked" insertionTime="Fri, 09 Apr 2010 14:31:47 GMT">SctDaqConfigurationRodmur-Apr10-01</tag>
  <tag lock="locked" insertionTime="Thu, 05 Apr 2012 11:20:16 GMT">SctDaqConfigurationRodmur-Apr12-01</tag>
  <tag lock="locked" insertionTime="Mon, 20 Sep 2010 14:43:45 GMT">SctDaqConfigurationRodmur-Aug10-01</tag>
  <tag lock="locked" insertionTime="Tue, 15 Dec 2009 10:58:50 GMT">SctDaqConfigurationRodmur-Dec09</tag>
  <tag lock="locked" insertionTime="Tue, 06 Dec 2011 10:27:01 GMT">SctDaqConfigurationRodmur-Dec11-01</tag>
  <tag lock="unlocked" insertionTime="Thu, 09 Feb 2012 14:24:09 GMT">SctDaqConfigurationRodmur-MC-01</tag>
  <tag lock="locked" insertionTime="Thu, 09 Feb 2012 15:53:21 GMT">SctDaqConfigurationRodmur-MC-02</tag>
  <tag lock="locked" insertionTime="Thu, 28 Feb 2013 10:31:19 GMT">SctDaqConfigurationRodmur-MC-03</tag>
  <tag lock="locked" insertionTime="Mon, 01 Jul 2013 14:49:50 GMT">SctDaqConfigurationRodmur-MC-04</tag>
  <tag lock="locked" insertionTime="Tue, 30 Jul 2013 10:09:32 GMT">SctDaqConfigurationRodmur-MC-05</tag>
  <tag lock="locked" insertionTime="Tue, 05 Aug 2014 10:42:44 GMT">SctDaqConfigurationRodmur-MC-06</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:06:31 GMT">SctDaqConfigurationRodmur-NOMINAL-Feb09</tag>
  <tag lock="unlocked" insertionTime="Mon, 13 Jul 2009 10:06:31 GMT">SctDaqConfigurationRodmur-NOMINAL-Jan09</tag>
  <tag lock="locked" insertionTime="Mon, 13 Jul 2009 10:06:31 GMT">SctDaqConfigurationRodmur-NOMINAL-Sept08</tag>
  <tag lock="unlocked" insertionTime="Mon, 09 Nov 2009 10:16:48 GMT">SctDaqConfigurationRodmur-Nov09</tag>
  <tag lock="locked" insertionTime="Mon, 15 Nov 2010 11:43:17 GMT">SctDaqConfigurationRodmur-Nov10-01</tag>
  <tag lock="locked" insertionTime="Wed, 01 Dec 2010 15:39:09 GMT">SctDaqConfigurationRodmur-Nov10-02</tag>
  <tag lock="locked" insertionTime="Mon, 30 Aug 2010 13:04:13 GMT">SctDaqConfigurationRodmur-PERFECT-Aug10-01</tag>
</tagList>
**/
  std::array<std::string,2> rodmurTagNames={"SctDaqConfigRodmur-Run2-10July2015","SctDaqConfigurationRodmur-MC-06"};

  //
  std::array<std::string,6> folderNames={"Chip","Geog","MUR","Module","ROD","RODMUR"};
  std::string dbName("");
  const std::string defaultDbName="COOLOFL_SCT/OFLP200";
  std::string iovString=argv[1];
  if (argc!=2){
   std::cout<<"No arguments? "<<std::endl;
   std::cout<<argc<<std::endl;
   iovString="1333588404000000000-1333588404000000001";
   std::cout<<"running with default argument "<<iovString<<std::endl;
  }
  
  std::vector<std::string> splitString=split(iovString,'-');
  if (splitString.size()!=2) exit(-1);
  std::cout<<"From "<<splitString[0]<<" to "<<splitString[1]<<std::endl;
  const cool::ValidityKey since = std::stoll(splitString[0]);
  cool::ValidityKey until = std::stoll(splitString[1]);
	dbName=defaultDbName;
	
	//bool itsCoraCool =(dbName.find("COMP200") != std::string::npos);
	//DbConnection connection(dbName, itsCoraCool);
	//const std::string folderPath = defaultFolderPath(itsCoraCool);
	std::array<std::string,2> folderPaths={"/SCT/DAQ/Config/","/SCT/DAQ/Configuration/"};
	const unsigned int nFolders=folderNames.size();
	DbConnection writeDb("LOCAL");
	if (!writeDb.isOpen()) {
	  std::cout<<"problem"<<std::endl;
	  return -1;
	}
	unsigned int index(0);
	for (const auto & folderPath:folderPaths){
		bool itsCoraCool =(folderPath.find("Configuration") != std::string::npos);
		const std::string chipTagName=chipTagNames[index];
		const std::string geogTagName=geogTagNames[index];
		const std::string murTagName=murTagNames[index];
    const std::string moduleTagName=moduleTagNames[index];
    const std::string channelTagName=channelTagNames[index];
    const std::string rodTagName=rodTagNames[index];
    const std::string rodmurTagName=rodmurTagNames[index++];

		DbConnection connection(dbName, itsCoraCool);
		const unsigned int nData(1000000);
		for (unsigned int i(0);i!=nFolders;++i){
		  std::string tagName;
		  if (folderNames[i]=="Chip") tagName=chipTagName;
		  if (folderNames[i]=="Geog") tagName=geogTagName;
		  if (folderNames[i]=="MUR") tagName=murTagName;
			if (folderNames[i]=="Module") tagName=moduleTagName;
			if (folderNames[i]=="Channel") tagName=channelTagName;
			if (folderNames[i]=="ROD") tagName=rodTagName;
			if (folderNames[i]=="RODMUR") tagName=rodmurTagName;

			std::string folderName=folderPath+folderNames[i];
			const FolderSpec theFolder(folderName,tagName);
			Folder folder(connection, theFolder);
			//std::cout<<folder.description()<<std::endl;
			std::cout<<"---++++"<<folderName<<std::endl;
			printRecordSpec(folder.payloadSpec());
			/**
			0-1230768000000000000 : 0-142308; ->Thu, 01 Jan 2009 00:00:00 GMT
1230768000000000000-1262304000000000000 : ->Fri, 01 Jan 2010 00:00:00 GMT 142308 - 152166	
1262304000000000000-1288569600000000000 : ->Mon, 01 Nov 2010 00:00:00 GMT 15277,154471,155697,162623,167576; 152166 - 153599
1288569600000000000-1290686400000000000 : ->Thu, 25 Nov 2010 12:00:00 GMT ? not attributed 170000
1290686400000000000-1293840000000000000 : ->Sat, 01 Jan 2011 00:00:00 GMT 170002
1293840000000000000-1333588404000000000 : ->Thu, 05 Apr 2012 01:13:24 GMT 178211,180164,183003,186169,189751,193662,195847-200805;178163 - 178264
1333588404000000000-1333612447000000000 : ->Thu, 05 Apr 2012 07:54:07 GMT 200805-216432
1333612447000000000-1356998400000000000 : ->Tue, 01 Jan 2013 00:00:00 GMT 210184,212272
1356998400000000000-9223372036854775807 : 219114,219305 
**/
		
			SelectionCriterion selection(cool::ChannelSelection::all(),since,until);
			
			if (!itsCoraCool){
			  createLocalCopyStructure(folder, writeDb);
				//showResult(result);
			} else {
			  std::string run1Tag = folder.folderSpec().tag;
			  const std::string destTag = convertTagToRun2(run1Tag);
			  std::cout<<"Destination tag "<<destTag<<std::endl;
			  
				if (folderNames[i]=="Chip"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/Chip",tagName);
				  Folder target(writeDb,destFolder);
				  writeChip(result,target,destTag);
				}
				if (folderNames[i]=="Geog"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/Geog",tagName);
				  Folder target(writeDb,destFolder);
				  writeGeog(result,target,destTag);
				}
				if (folderNames[i]=="MUR"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/MUR",tagName);
				  Folder target(writeDb,destFolder);
				  writeMUR(result,target,destTag);
				}
				if (folderNames[i]=="Module"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/Module",tagName);
				  Folder target(writeDb,destFolder);
				  writeModule(result,target,destTag);
				}
				if (folderNames[i]=="Channel"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/Channel",tagName);
				  Folder target(writeDb,destFolder);
				  writeChannel(result,target,destTag);
				}
				if (folderNames[i]=="ROD"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/ROD",tagName);
				  Folder target(writeDb,destFolder);
				  writeRod(result,target,destTag);
				}
				if (folderNames[i]=="RODMUR"){
				  const CCIovVectorMap_t &result = readCC(folder,selection,nData);
				  const FolderSpec destFolder("/SCT/DAQ/Config/RODMUR",tagName);
				  Folder target(writeDb,destFolder);
				  writeRodmur(result,target,destTag);
				}
				//const IovVectorMap_t &result = read(folder,selection,5);
				//showResult(result);
			}
		}
	}
	  
	
	
  return retcode;
}