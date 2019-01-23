/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbFolder.cxx - helper class for IOVDbSvc to manage folder & data cache
// Richard Hawkings, started 24/11/08

#include <sstream>
#include <stdexcept>


#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/ISvcLocator.h"


#include "StoreGate/StoreGateSvc.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IRecord.h"
#include "CoolKernel/IRecordIterator.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeSpecification.h"
#include "CoralBase/Blob.h"
#include "TStopwatch.h"

#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObject.h"
#include "CoraCool/CoraCoolObjectIter.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/AthenaAttrListAddress.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/CondAttrListCollAddress.h"
#include "AthenaPoolUtilities/CondAttrListVec.h"
#include "AthenaPoolUtilities/CondAttrListVecAddress.h"

#include "GeoModelInterfaces/IGeoModelSvc.h"
#include "EventInfo/TagInfo.h"

#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"

#include "IOVDbConn.h"

#include "ReadFromFileMetaData.h"
#include "IOVDbFolder.h"
#include "IOVDbStringFunctions.h"
#include "IOVDbCoolFunctions.h"
#include "CxxUtils/make_unique.h"

using namespace IOVDbNamespace;



IOVDbFolder::IOVDbFolder(IOVDbConn* conn,
                         const IOVDbParser& folderprop, MsgStream & /*msg*/,
                         IClassIDSvc* clidsvc, const bool checklock):
  
  p_detStore(0),
  p_clidSvc(clidsvc),
  p_metaDataTool(0),
  m_conn(conn),
  m_foldername(""),
  m_key(""),
  m_multiversion(false),
  m_timestamp(false),
  m_tagoverride(false),
  m_notagoverride(false),
  m_writemeta(false),
  m_fromMetaDataOnly(false),
  m_extensible(false),
  m_named(false),
  m_iovoverridden(false),
  m_jokey(false),
  m_dropped(false),
  m_autocache(true),
  m_checklock(checklock),
  m_iovoverride(0),
  m_foldertype(AttrList),
  m_metacon(0),
  m_cachelength(0),
  m_cachestart(0),
  m_cachestop(0),
  m_cachehint(0),
  m_cacheinc(0),
  m_chansel(cool::ChannelSelection::all()),
  m_jotag(""),
  m_tag(""),
  m_typename(""),
  m_eventstore(""),
  m_cachepar(""),
  m_addrheader(""),
  m_clid(0),
  m_ndbread(0),
  m_ncacheread(0),
  m_nobjread(0),
  m_nbytesread(0),
  m_readtime(0.),
  m_nchan(0),
  m_retrieved(false),
  m_cachespec(0),
  m_boundmin(),
  m_boundmax(),
  m_nboundmin(0),
  m_nboundmax(0),
  m_msg("IOVDbFolder")
{
  // extract settings from the properties
  // foldername from the 'unnamed' property
  folderprop.getKey("","",m_foldername);
  // SG key from 'key' property, otherwise same as foldername
  // m_jokey is true if the 'key' property was set - need to remember this
  // to avoid using folder descripton <key> if present later
  m_jokey=folderprop.getKey("key",m_foldername,m_key);
  // tag from 'tag' property
  folderprop.getKey("tag","",m_jotag);
  // event store from 'eventStoreName' property, default 'StoreGateSvc'
  folderprop.getKey("eventStoreName","StoreGateSvc",m_eventstore);
  // cachelength (seconds or LB)
  folderprop.getKey("cache","",m_cachepar);
  std::string buf;
  // check for <noover> - disables using tag override read from input file
  m_notagoverride=folderprop.getKey("noover","",buf);
  if (m_notagoverride) ATH_MSG_INFO( "Inputfile tag override disabled for " << m_foldername );

  // channel selection from 'channelSelection' property
  // syntax is A:B,C:D,E:F
  // :B implies zero lower limit, A: implies zero upper limit
  std::string chanspec,rangespec;
  if (folderprop.getKey("channelSelection","",chanspec) && chanspec!="") {
    m_chanrange=IOVDbNamespace::parseChannelSpec<cool::ChannelId>(chanspec);
    // explicit setting of channel selection
    bool first(true);
    //push to the channel selection
    try{
      for(const auto & i:m_chanrange){
        if (first){
          first = false;
          m_chansel = cool::ChannelSelection(i.first,i.second);
        } else {
          m_chansel.addRange(i.first,i.second);
        }
      }
    } catch (cool::Exception& e) {
        ATH_MSG_ERROR("defining channel range (must be given in ascending order)");
        throw;
    }
  }
  // check for folder-specific run/LB/time overrides
  if (folderprop.getKey("forceRunNumber","",buf)) {
    const auto run = IOVDbNamespace::iovFromRunString(buf);
    unsigned int lb=0;
    if (folderprop.getKey("forceLumiblockNumber","",buf)) lb=IOVDbNamespace::iovFromLumiBlockString(buf);
    overrideWithRunLumi(run, lb);
  }
  if (folderprop.getKey("forceTimestamp","",buf)) {
    const auto ts=IOVDbNamespace::iovFromTimeString(buf);
    overrideWithTimestamp(ts);
  }
  m_fromMetaDataOnly=folderprop.getKey("metaOnly","",buf);
  if (m_fromMetaDataOnly) {
    ATH_MSG_INFO( "Read from meta data only for folder " << m_foldername );
  }

  m_extensible=folderprop.getKey("extensible","",buf);
  if (m_extensible) {
    ATH_MSG_INFO( "Extensible folder " << m_foldername );
  }
}

void
IOVDbFolder::overrideWithRunLumi(const unsigned long long run, const unsigned long long lb){
  m_iovoverridden=true;
  m_iovoverride=run+lb;
  ATH_MSG_INFO( "Override run/LB number to [" << run << ":" << lb << "] for folder " << m_foldername );
}

void
IOVDbFolder::overrideWithTimestamp(const unsigned long long timestamp){
  m_iovoverridden=true;
  m_iovoverride = timestamp;
  ATH_MSG_INFO( "Override timestamp to " << m_iovoverride << " for folder " << m_foldername );
}

IOVDbFolder::~IOVDbFolder() {
  if (m_cachespec!=0) m_cachespec->release();
}

void IOVDbFolder::setMetaCon(const IOVMetaDataContainer* metacon) {
  // enable folder from FLMD at given connection
  m_metacon=metacon;
  // if previously connected to a real DB connection, remove association
  if (m_conn!=0) {
    m_conn->decUsage();
    m_conn=0;
  }
}

void IOVDbFolder::setTagOverride(const std::string& tag,const bool setFlag) {
  if (m_tagoverride) {
    ATH_MSG_WARNING( "Request to override tag for folder " <<
      m_foldername << " to " << tag << " supercedes earlier override to "  << m_jotag );
  } else {
    if (setFlag) m_tagoverride=true;
  }
  m_jotag=IOVDbNamespace::spaceStrip(tag);
}

void IOVDbFolder::setWriteMeta(IIOVDbMetaDataTool* metadatatool) {
  p_metaDataTool=metadatatool;
  m_writemeta=true;
}

void IOVDbFolder::setIOVOverride(const unsigned int run,
                                 const unsigned int lumiblock,
                                 const unsigned int time) {
  // set IOV override for this folder if run!=0 or time!=0
  // folder-specific override takes precedence if set
  if (m_iovoverridden) return;
  if (m_timestamp) {
    if (time!=0) {
      m_iovoverride=(static_cast<unsigned long long>(time))*1000000000LL;
      ATH_MSG_INFO( "Override timestamp to " << m_iovoverride << " for folder "<< m_foldername );
      m_iovoverridden=true;
    }
  } else {
    if (run!=0 || lumiblock!=0) {
      m_iovoverride=((static_cast<unsigned long long>(run)) << 32LL)+lumiblock;
      ATH_MSG_INFO( 
        "Override run/LB number to [" << run << ":" << lumiblock << 
        "] for folder " << m_foldername );
      m_iovoverridden=true;
    }
  }
}


// return validitykey for folder, given input reftime
// take into account an overridden IOV, if present for folder
cool::ValidityKey IOVDbFolder::iovTime(const IOVTime& reftime) const {
  if (m_iovoverridden) {
    return m_iovoverride;
  } else {
    return (m_timestamp ? reftime.timestamp() : reftime.re_time());
  }
}

bool IOVDbFolder::loadCache(const cool::ValidityKey vkey,
                            const unsigned int cacheDiv,
                            const std::string& globalTag,
                            const bool ignoreMissChan) {
  // load the cache for the given IOVTime, making a range around this time
  // according to the cacheing policy
  // if cacheDiv > 0, specifies number of slices of cache for query alignment
  // if ignoreMissChan set, don't worry about missing channels outside the cache range
  // return false if any problem

  // timer to track amount of time in loadCache
  TStopwatch cachetimer;

  ATH_MSG_DEBUG( "Load cache for folder " << m_foldername << " validitykey " << vkey);
  // if not first time through, and limit not reached,and cache was not reset, 
  // and we are going forwards in time, double cachesize
  if (m_ndbread>0 && m_cacheinc<3 && (m_cachestop!=m_cachestart) && 
      vkey>m_cachestart &&m_autocache) {
    m_cachelength*=2;
    ++m_cacheinc;
    ATH_MSG_INFO( "Increase cache length (step " << m_cacheinc << 
      ") for folder " << m_foldername
           << " to " << m_cachelength << " at validityKey " << vkey );
  }
  ++m_ndbread;
  if (cacheDiv>0) {
    // quantise queries on boundaries that are sub-multiples of cache length
    unsigned long long cacheq=m_cachelength/cacheDiv;
    m_cachestart=vkey-vkey % cacheq;
    m_cachestop=m_cachestart+m_cachelength;
  } else {
    // for run/LB indexed folders and cache of at least one run
    // align the query to the run start
    if (m_timestamp==false && m_cachelength>=(1LL << 32)-1) {
      m_cachestart=vkey & (0x7FFFFFFFLL << 32);
    } else {
      m_cachestart=vkey;
    }
    m_cachestop=vkey+m_cachelength;
  }
  if (m_cachestop>cool::ValidityKeyMax) m_cachestop=cool::ValidityKeyMax;
  ATH_MSG_DEBUG( "IOVDbFolder:loadCache limits set to [" 
           << m_cachestart << "," << m_cachestop << "]" );

  if (m_cachespec==0) {
    // on first init, guess size based on channel count
    unsigned int estsize=m_nchan;
    if (m_cachehint > 0) {
      estsize=estsize*m_cachehint;
    } else if (m_timestamp) {
      // for timestamp indexed folder (likely to be DCS), increase this
      estsize=estsize*3;
    }
    // note this is only reserved size of the cache vectors
    // actual datastorage is mainly allocated by pointer elsewhere
    m_cachesince.reserve(estsize);
    m_cacheuntil.reserve(estsize);
    m_cachechan.reserve(estsize);
    m_cacheattr.reserve(estsize);
    if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
      m_cacheccstart.reserve(estsize);
      m_cacheccend.reserve(estsize);
    }
  } else {
    // reset cache if it already contained data
    // TBIO - could keep the attributelists and only change the data on reload
    // avoiding some attributelist construction/destruction
    clearCache();
  }

  // query to fill cache - request for database activates connection
  cool::IDatabasePtr dbPtr=m_conn->getCoolDb();
  if (dbPtr.get()==0) {
    ATH_MSG_FATAL( "Conditions database connection " <<
      m_conn->name() << " cannot be opened - STOP" );
    return false;
  }
  // access COOL inside try/catch in case of using stale connection
  unsigned int attempts=0;
  bool retrievedone=false;
  
  while (attempts<2 && !retrievedone) {
    ++attempts;
    try {
      unsigned int iadd=0;
      m_boundmin=0;
      m_boundmax=cool::ValidityKeyMax;
      // check how many channels cross the cache boundaries
      // if a selection is in use, count them explicitly
      if (not m_chanrange.empty()) {
        m_nboundmin=IOVDbNamespace::countSelectedChannels(m_channums, m_chansel);
      } else {
        m_nboundmin=m_nchan;
      }
      ATH_MSG_DEBUG( "Expecting to see " << m_nboundmin << " channels" );
      m_nboundmax=m_nboundmin;
      // check pointer is still valid - can go stale in AthenaMT environment
      // according to CORAL server tests done by Andrea Valassi (23/6/09)
      if (not dbPtr.get()) throw std::runtime_error("COOL database pointer invalidated");
      // access COOL folder in case needed to resolve tag (even for CoraCool)
      cool::IFolderPtr folder=dbPtr->getFolder(m_foldername);
      // resolve the tag for MV folders if not already done so
      if (m_multiversion && m_tag=="") {
        if (!resolveTag(folder,globalTag)) return false;
        ATH_MSG_DEBUG( "resolveTag returns " << m_tag);
      }
      if (m_foldertype==CoraCool) {
        // CoraCool retrieve
        // initialise CoraCool connection
        CoraCoolDatabasePtr ccDbPtr=m_conn->getCoraCoolDb();
        CoraCoolFolderPtr ccfolder=ccDbPtr->getFolder(m_foldername);
        CoraCoolObjectIterPtr itr=ccfolder->browseObjects(m_cachestart, 
                                                          m_cachestop,m_chansel,m_tag);
        while (itr->hasNext()) {
          CoraCoolObjectPtr obj=itr->next();
          addIOVtoCache(obj->since(),obj->until());
          m_cachechan.push_back(obj->channelId());
          // store all the attributeLists in the buffer
          // save pointer to start
          const unsigned int istart=m_cacheattr.size();
          for (CoraCoolObject::const_iterator pitr=obj->begin();
               pitr!=obj->end(); ++pitr) {
            // setup shared specification on first store
            if (m_cachespec==0) setSharedSpec(*pitr);
            // use the shared specification in storing the payload
            m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));
            m_cacheattr[m_cacheattr.size()-1].fastCopyData(*pitr);
            m_nbytesread+=IOVDbNamespace::attributeListSize(*pitr);
          }
          // save pointers to start and end
          m_cacheccstart.push_back(istart);
          m_cacheccend.push_back(m_cacheattr.size());
          ++iadd;
        }
        itr->close();
        retrievedone=true;
      } else {
        cool::IObjectIteratorPtr itr=folder->browseObjects(m_cachestart,m_cachestop,m_chansel,m_tag);
        while (itr->goToNext()) {
          const cool::IObject& ref=itr->currentRef();
          addIOVtoCache(ref.since(),ref.until());
          m_cachechan.push_back(ref.channelId());
          if (m_foldertype==CoolVector) {
            // store all the attributeLists in the buffer
            // save pointer to start
            const unsigned int istart=m_cacheattr.size();
            // get payload iterator and vector of payload records
            cool::IRecordIterator& pitr=ref.payloadIterator();
            const cool::IRecordVectorPtr& pvec=pitr.fetchAllAsVector();
            for (cool::IRecordVector::const_iterator vitr=pvec->begin();
                 vitr!=pvec->end();++vitr) {
              const coral::AttributeList& atrlist=(*vitr)->attributeList();
              // setup shared specification on first store
              if (m_cachespec==0) setSharedSpec(atrlist);
              // use the shared specification in storing the payload
              m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));
              m_cacheattr[m_cacheattr.size()-1].fastCopyData(atrlist);
              m_nbytesread+=IOVDbNamespace::attributeListSize(atrlist);
            }
            // save pointers to start and end
            m_cacheccstart.push_back(istart);
            m_cacheccend.push_back(m_cacheattr.size());
            ++iadd;
            pitr.close();
          } else {
            // standard COOL retrieve
            const coral::AttributeList& atrlist=ref.payload().attributeList();
            // setup shared specification on first store
            if (m_cachespec==0) setSharedSpec(atrlist);
            // use the shared specification in storing the payload
            m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));
            m_cacheattr[iadd].fastCopyData(atrlist);
            ++iadd;
            m_nbytesread+=IOVDbNamespace::attributeListSize(atrlist);
          }
        }
        itr->close();
        retrievedone=true;
      }
      ATH_MSG_DEBUG( "Retrieved " << iadd << " objects for "<< m_nchan << " channels into cache" );
      m_nobjread+=iadd;
    } catch (std::exception& e) {
      ATH_MSG_WARNING( "COOL retrieve attempt " << attempts << 
        " failed: " << e.what() );
      // disconnect and reconnect
      try {
        m_conn->setInactive();
        dbPtr=m_conn->getCoolDb();
      } catch (std::exception& e) {
        ATH_MSG_WARNING( "Exception from disconnect/reconnect: " <<e.what() );
        // try once more to connect
        try {
          dbPtr=m_conn->getCoolDb();
        } catch (std::exception& e) {
          ATH_MSG_ERROR( "Cannot reconnect to database:" << e.what());
        }
      }
    }
  }

  if (!retrievedone) {
    ATH_MSG_ERROR( "Could not retrieve COOL data for folder " <<
      m_foldername << " tag " << m_tag << " validityKeys [" << m_cachesince <<
      "," << m_cacheuntil << "]" );
    return false;
  }
  // check if cache can be stretched according to extent of IOVs crossing
  // boundaries - this requires all channels to have been seen
  ATH_MSG_DEBUG( "Cache retrieve missing " << m_nboundmin  << " lower and " << m_nboundmax << " upper channels" );
  if ((m_nboundmin==0 || ignoreMissChan) && m_boundmin < m_cachestart) {
    ATH_MSG_DEBUG( "Lower cache limit extended from " << m_cachestart << " to " << m_boundmin );
    m_cachestart=m_boundmin;
  }
  if ((m_nboundmax==0 || ignoreMissChan) && m_boundmax > m_cachestop) {
    ATH_MSG_DEBUG( "Upper cache limit extended from " <<m_cachestop << " tp " << m_boundmax );
    m_cachestop=m_boundmax;
  }
  // keep track of time spent
  const float timeinc=cachetimer.RealTime();
  m_readtime+=timeinc;
  ATH_MSG_DEBUG( "Cache retrieve done for " << m_foldername << " with " << 
      m_cachesince.size() << " objects stored in" << std::fixed <<
      std::setw(8) << std::setprecision(2) << timeinc << " s" );
  return true;
}

bool IOVDbFolder::loadCacheIfDbChanged(const cool::ValidityKey vkey,
                                       const std::string& globalTag, 
                                       cool::IDatabasePtr dbPtr,
                                       const ServiceHandle<IIOVSvc>& iovSvc) {
  ATH_MSG_DEBUG( "IOVDbFolder::recheck with DB for folder " << m_foldername<< " validitykey: " << vkey );
  if (!m_cachesince.size()) {
    ATH_MSG_DEBUG( "Cache empty ! returning ..." );
    return true;
  }
  ++m_ndbread;
  // access COOL inside try/catch in case of using stale connection
  unsigned int attempts     = 0;
  bool         retrievedone = false;
  while (attempts<2 && !retrievedone) {
    ++attempts;
    try {
      m_boundmin=0;
      m_boundmax=cool::ValidityKeyMax;
      // check how many channels cross the cache boundaries
      // if a selection is in use, count them explicitly
      if (not m_chanrange.empty()) {
        m_nboundmin = IOVDbNamespace::countSelectedChannels(m_channums, m_chansel);
      } else {       
        m_nboundmin=m_nchan;
      }
      ATH_MSG_DEBUG( "Expecting to see " << m_nboundmin << " channels" );
      m_nboundmax=m_nboundmin;
      // access COOL folder in case needed to resolve tag (even for CoraCool)
      cool::IFolderPtr folder=dbPtr->getFolder(m_foldername);
      // resolve the tag for MV folders if not already done so
      if (m_multiversion && m_tag.empty()) { // NEEDED OR NOT?
        if (!resolveTag(folder,globalTag)) return false;
        ATH_MSG_DEBUG( "resolveTag returns " << m_tag );
      }   
      int counter=0;
      if (m_foldertype==CoraCool) {
        ATH_MSG_DEBUG("CoraCool folder cachestart:\t"<<m_cachestart<<" \t cachestop:"<< m_cachestop);
        ATH_MSG_DEBUG("checking range:  "<<vkey+1<<" - "<<vkey+2);
        // CoraCool retrieve initialise CoraCool connection
        CoraCoolDatabasePtr ccDbPtr   = m_conn->getCoraCoolDb();
        CoraCoolFolderPtr   ccfolder  = ccDbPtr->getFolder(m_foldername);
        // this returns all the objects whose IOVRanges crosses this range .
        CoraCoolObjectIterPtr itr = ccfolder->browseObjects(vkey+1, vkey+2,m_chansel,m_tag);
        while (itr->hasNext()) {
          CoraCoolObjectPtr obj = itr->next();
          ATH_MSG_DEBUG("from DB \t chID: "<<obj->channelId()
                   <<"\tobjstart:\t"<<obj->since()<<"\t objstop: \t"
                   << obj->until() );
          std::vector<cool::ValidityKey>::iterator st = m_cachesince.begin();
          std::vector<cool::ValidityKey>::iterator ut = m_cacheuntil.begin();
          std::vector<cool::ChannelId>::iterator   sc = m_cachechan.begin();
          // The covered flag is used to check whether the
          // requested IOV time is inside the range covered
          // by the current cache. If not, a cache reset
          // will be done.
          bool covered = 0; 
          for(; st != m_cachesince.end(); ++st,++ut,++sc){ 
            if (obj->channelId()!=(*sc)) continue;  
            if ((*st) < obj->since() && obj->since() < (*ut)){ 
              // obj time is larger than cache start (and less than cache stop)
              //   ==> update cache
              ++counter;
              ATH_MSG_DEBUG("special reload needed on THIS ONE !!!!!!!!!!!");
                // just change existing IOVRange
              ATH_MSG_DEBUG("changing "<<(*ut)<<"  to "<<obj->since()-1);
              (*ut)=obj->since()-1;
              specialCacheUpdate(obj, iovSvc); //  reset proxy, add to cache, addIOV 
              covered = 1;
              break; // can not be more than one
            }
            // For obj time with start == cache start, or stop == cache stop,
            //   we consider that the range is covered and set flag to prevent cache update
            if ( (obj->since()>=(*st) && obj->since()<(*ut)) || (obj->until()>(*st) && obj->until()<=(*ut)) ) covered=1;
          }
          if (!covered) {
            // cache range has not been covered, so update the cache
            ++counter;
            specialCacheUpdate(obj, iovSvc);
          }
        }
        ATH_MSG_DEBUG( "Need a special update for " << counter << " objects " );
        itr->close();
        retrievedone=true;
      } else {
        ATH_MSG_DEBUG("not CoraCool type.   cachestart:"<<m_cachestart<<"  cachestop:"<< m_cachestop );
        ATH_MSG_DEBUG("checking range:  "<<vkey+1<<" - "<<vkey+2);
        // this returns all the objects whose IOVRanges crosses this range . 
        cool::IObjectIteratorPtr itr=folder->browseObjects(vkey+1, vkey+2, m_chansel,m_tag);
        while (itr->goToNext()) {
          const cool::IObject& ref=itr->currentRef();
          ATH_MSG_DEBUG("from DB -----> objstart:"<<ref.since()<<"  objstop:"<< ref.until() <<" chID: "<<ref.channelId());
          std::vector<cool::ValidityKey>::iterator st = m_cachesince.begin();
          std::vector<cool::ValidityKey>::iterator ut = m_cacheuntil.begin();
          std::vector<cool::ChannelId>::iterator sc   = m_cachechan.begin();
          // The covered flag is used to check whether the
          // requested IOV time is inside the range covered
          // by the current cache. If not, a cache reset
          // will be done.
          bool covered=0;
          for(; st!=m_cachesince.end();++st,++ut,++sc){
            if (ref.channelId()!=(*sc)) continue;   
            if ((*st) < ref.since() && ref.since() < (*ut)){
              // ref time is larger than cache start (and less than cache stop)
              //   ==> update cache
              ATH_MSG_DEBUG("special reload needed on this one !!!!!!!!!!!");
                // just change existing IOVRange
              ATH_MSG_DEBUG("changing "<<(*ut)<<"  to "<<ref.since()-1);
              (*ut)=ref.since()-1;               
              ++counter;
              specialCacheUpdate(ref, iovSvc);
              covered=1;
              // The object is found, limit changed and
              // a new one has added. no need to search
              // for more.
              break; 
            }
            // For ref time with start == cache start, or stop == cache stop,
            //   we consider that the range is covered and set flag to prevent cache update
            if ( (ref.since()>=(*st) && ref.since()<(*ut)) || (ref.until()>(*st) && ref.until()<=(*ut)) ) covered=1;
          }
          if (!covered) {
            // cache range has not been covered, so update the cache
            ++counter;
            specialCacheUpdate(ref, iovSvc);
          }
        }
        ATH_MSG_DEBUG( "Need a special update for " << counter << " objects " ); 
        itr->close();
        retrievedone=true;
      }      
      m_nobjread+=counter;
    }catch (std::exception& e) {
      ATH_MSG_WARNING( "COOL retrieve attempt " << attempts <<  " failed: " << e.what() );
      try { // disconnect and reconnect
        m_conn->setInactive();
        dbPtr=m_conn->getCoolDb();
      }
      catch (std::exception& e) {
        ATH_MSG_WARNING( "Exception from disconnect/reconnect: " << e.what() );
      }
    }
  }
  ATH_MSG_INFO( "Special cache check finished for folder " << m_foldername );
  return true;
}

void 
IOVDbFolder::specialCacheUpdate(CoraCoolObjectPtr obj,
    const ServiceHandle<IIOVSvc>& iovSvc) {

  // reset IOVRange in IOVSvc to trigger reset of object. Set to a
  // time earlier than since.
  IOVRange range = makeRange(obj->since()-2, obj->since()-1);
  if (StatusCode::SUCCESS != iovSvc->setRange(clid(), key(), range, eventStore())) {
    ATH_MSG_ERROR( "IOVDbFolder::specialCacheUpdate - setRange failed for folder " 
           << folderName() );
    return;
  }
    
  addIOVtoCache(obj->since(),obj->until());
  m_cachechan.push_back(obj->channelId());
        
  // store all the attributeLists in the buffer save pointer to start
  const unsigned int istart=m_cacheattr.size();
  for (CoraCoolObject::const_iterator pitr=obj->begin(); pitr!=obj->end();++pitr) {
    // use the shared specification in storing the payload
    m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));
    m_cacheattr.back().fastCopyData(*pitr);
    m_nbytesread+=IOVDbNamespace::attributeListSize(*pitr);
  }
  // save pointers to start and end
  m_cacheccstart.push_back(istart);
  m_cacheccend.push_back(m_cacheattr.size());
}

void IOVDbFolder::specialCacheUpdate(const cool::IObject& ref,
                                     const ServiceHandle<IIOVSvc>& iovSvc) {
  // reset IOVRange in IOVSvc to trigger reset of object. Set to a
  // time earlier than since.
  IOVRange range = makeRange(ref.since()-2, ref.since()-1);
  if (StatusCode::SUCCESS != iovSvc->setRange(clid(), key(), range, eventStore())) {
    ATH_MSG_ERROR( "IOVDbFolder::specialCacheUpdate - setRange failed for folder " 
           << folderName() );
    return;
  }

  // add new object.
  addIOVtoCache(ref.since(),ref.until());
  m_cachechan.push_back(ref.channelId());
  const coral::AttributeList& atrlist = ref.payload().attributeList();
 
  // use the shared specification in storing the payload
  m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));// maybe needs to be cleared before
  /**if (m_msg.level()<=MSG::DEBUG) {
    std::ostringstream os;
    atrlist.toOutputStream(os);
    m_msg << MSG::DEBUG << "AttributeList: " << os.str() );
  }**/
  m_cacheattr.back().fastCopyData(atrlist);
  m_nbytesread+=IOVDbNamespace::attributeListSize(atrlist);
}

void IOVDbFolder::resetCache() {
  // reset the cache to unfilled state, used if no more data will be required
  // from this folder
  m_cachestart=0;
  m_cachestop=0;
  clearCache();
}


bool IOVDbFolder::getAddress(const cool::ValidityKey reftime,
                             IAddressCreator* persSvc,
                             const unsigned int poolSvcContext,
                             IOpaqueAddress*& address,
                             IOVRange& range, bool& poolPayloadReq) {
  ++m_ncacheread;
  // will produce strAddress and one pointer type depending on folder data
  std::string strAddress;
  AthenaAttributeList* attrList=0;
  CondAttrListCollection* attrListColl=0;
  CondAttrListVec* attrListVec=0;
  cool::ValidityKey naystart=0;
  cool::ValidityKey naystop=cool::ValidityKeyMax;
  
  
  if (m_metacon) {    
    IOVDbNamespace::ReadFromFileMetaData readFromMetaData(m_metacon, reftime, m_timestamp);
    if (not readFromMetaData.isValid()){
      ATH_MSG_ERROR( "read:Could not find IOVPayloadContainer for folder "<< m_foldername );
      return false;
    }
    // read from file metadata
    m_foldertype=readFromMetaData.folderType();
    m_nobjread+=readFromMetaData.numberOfObjects();
    poolPayloadReq=readFromMetaData.poolPayloadRequested();
    strAddress = readFromMetaData.stringAddress();
    range = readFromMetaData.range();
    attrList = readFromMetaData.attributeList();
    attrListColl = readFromMetaData.attrListCollection();
    ATH_MSG_DEBUG( "Read file metadata for folder " << m_foldername << " foldertype is " << m_foldertype );
  } else {
    // COOL/CoraCool data to be read from cache
    // for AttrListColl or PoolRefColl, need a CondAttrListCollection ready
    // to receive the data
    if (m_foldertype==AttrListColl || m_foldertype==PoolRefColl) {
      attrListColl=new CondAttrListCollection(!m_timestamp);
    } else if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
      // for CoraCool/CoolVector, assume we will get everything in the cache
      attrListVec=new CondAttrListVec(!m_timestamp, m_cacheattr.size());
    }

    // loop over cached data
    unsigned int nobj=0;
    // keep track of closest neighbouring IOVs
    naystart=m_cachestart;
    naystop=m_cachestop;
    for (unsigned int ic=0; ic<m_cachesince.size();++ic) {
      if (m_cachesince[ic]<=reftime && reftime<m_cacheuntil[ic]) {
        ++nobj;
        if (m_foldertype==AttrList || m_foldertype==PoolRef) {
          // retrieve of AthenaAttributeList or single PoolRef
          if (m_foldertype==AttrList) {
            attrList=new AthenaAttributeList(m_cacheattr[ic]);
            strAddress="POOLContainer_AthenaAttributeList][CLID=x";
          } else {
            strAddress=(m_cacheattr[ic])["PoolRef"].data<std::string>();
          }
          range=makeRange(m_cachesince[ic],m_cacheuntil[ic]);
          // write meta-data if required
          if (m_writemeta) 
            if (!addMetaAttrList(m_cacheattr[ic],range)) return false;
        } else if (m_foldertype==AttrListColl || m_foldertype==PoolRefColl) {
          // retrieve of CondAttrListCollection
          attrListColl->addShared(m_cachechan[ic],m_cacheattr[ic]);
          attrListColl->add(m_cachechan[ic],makeRange(m_cachesince[ic],m_cacheuntil[ic]));
        } else if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
          // retrieval of CoraCool data
          attrListVec->addSlice(makeRange(m_cachesince[ic],m_cacheuntil[ic]),
                                m_cachechan[ic],m_cacheattr,
                                m_cacheccstart[ic],m_cacheccend[ic]);
          if (m_writemeta) {
            ATH_MSG_ERROR( "Writing of CoraCool folders to file metadata not implemented");
            return false;
          }
        } else {
          ATH_MSG_ERROR( "Unhandled folder type " << m_foldertype);
          return false;
        }
      } else if (m_cacheuntil[ic]<=reftime && m_cacheuntil[ic]>naystart) {
        naystart=m_cacheuntil[ic];
      } else if (m_cachesince[ic]>reftime && m_cachesince[ic]<naystop) {
        naystop=m_cachesince[ic];
      }
    }
    // post-loop actions
    if (m_foldertype==AttrListColl || m_foldertype==PoolRefColl) {
      // set up channel names if required
      if (m_named) {
        std::vector<std::string>::const_iterator nitr=m_channames.begin();
        for (std::vector<cool::ChannelId>::const_iterator chitr=m_channums.begin();
             chitr!=m_channums.end(); ++chitr,++nitr) {
          attrListColl->add(*chitr,*nitr);
        }
      }
      // set range
      range=attrListColl->minRange();
      strAddress="POOLContainer_CondAttrListCollection][CLID=x";
    } else if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
      range=attrListVec->minRange();
      strAddress="POOLContainer_CondAttrListVec][CLID=x";
    } else if (m_foldertype==AttrList || m_foldertype==PoolRef) {
      // single object retrieve - should have exactly one object
      if (nobj==0) {
        ATH_MSG_ERROR("COOL object not found in single-channel retrieve, folder " 
               << m_foldername << " currentTime " << reftime );
        return false;
      } else if (nobj>1) {
        ATH_MSG_ERROR( nobj << 
          " valid objects found for single-channel retrieve, folder " << 
          m_foldername << " currentTime " << reftime );
        return false;
      }
    }
    //was debug
    ATH_MSG_DEBUG( "Retrieved object: folder " << m_foldername 
             <<  " at IOV " << reftime << " channels " << nobj << " has range " 
             << range );
    // shrink range so it does not extend into 'gap' channels or outside cache
    IOVTime tnaystart=makeEpochOrRunLumi(naystart, m_timestamp);
    IOVTime tnaystop=makeEpochOrRunLumi(naystop, m_timestamp);
    IOVTime rstart=range.start();
    IOVTime rstop=range.stop();
    if (tnaystart > rstart || rstop > tnaystop) {
      ATH_MSG_DEBUG( "Shrink IOV range from [" << rstart << ":" <<
          rstop << "] to [" << tnaystart << ":" << tnaystop << "]" );
      if (tnaystart > rstart) rstart=tnaystart;
      if (tnaystop  < rstop)  rstop=tnaystop;
      range=IOVRange(rstart,rstop);
      if (m_foldertype==AttrListColl || m_foldertype==PoolRefColl) {
        attrListColl->addNewStart(rstart);
        attrListColl->addNewStop(rstop);
      }
    }
  }
  // save the range for possible later lookup in IOVDbSvc::getKeyInfo
  m_currange=range;
  m_retrieved=true;
  // write metadata for attrListColl if required (after range shrinking)
  if (m_writemeta && 
      (m_foldertype==AttrListColl || m_foldertype==PoolRefColl)) {
    if (!addMetaAttrListColl(attrListColl)) return false;
  }

  // turn the data into an IOpaqueAddress
  strAddress=m_addrheader+strAddress;
  if (StatusCode::SUCCESS!=persSvc->createAddress(0,0,strAddress,address)) {
    ATH_MSG_ERROR( "Could not get IOpaqueAddress from string address "
           << strAddress );
    return false;
  }
  GenericAddress* gAddr=dynamic_cast<GenericAddress*>(address);
  if (!gAddr) {
    ATH_MSG_ERROR( "Could not cast IOpaqueAddress to GenericAddress");
    return false;
  }
  // create a new GenericAddress to set pool context
  if (m_foldertype==AttrListColl) {
    CondAttrListCollAddress* addr=new CondAttrListCollAddress(*gAddr);
    addr->setAttrListColl(attrListColl);
    delete address;
    address=addr;
  } else if (m_foldertype==PoolRefColl || m_foldertype==PoolRef) {
    CondAttrListCollAddress* addr=new CondAttrListCollAddress(gAddr->svcType(),
                                                              gAddr->clID(),gAddr->par()[0],gAddr->par()[1],
                                                              poolSvcContext,gAddr->ipar()[1]);
    delete address;
    if (m_foldertype==PoolRefColl) {
      addr->setAttrListColl(attrListColl);
    }
    address=addr;
    poolPayloadReq=true;
  } else if (m_foldertype==AttrList) {
    AthenaAttrListAddress* addr=new AthenaAttrListAddress(*gAddr);
    addr->setAttrList(attrList);
    delete address;
    address=addr;
  } else if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
    CondAttrListVecAddress* addr=new CondAttrListVecAddress(*gAddr);
    addr->setAttrListVec(attrListVec);
    delete address;
    address=addr;
  }
  return true;
}


void IOVDbFolder::summary() {
  const std::string & folderTypeName=IOVDbNamespace::folderTypeName(m_foldertype);
  // summarise the read statistics for this folder
  ATH_MSG_INFO(  "Folder " << m_foldername << " ("<<folderTypeName
	    << ") db-read " << m_ndbread << "/" <<
	    m_ncacheread << " objs/chan/bytes " << m_nobjread << "/" <<
	    m_nchan << "/" << m_nbytesread << " (( " << std::fixed << std::setw(8)
	         << std::setprecision(2) << m_readtime << " ))s" );
	
  // print WARNING if data for this fodler was never read from Storegate
  if (m_ncacheread==0 && m_ndbread>0) {
    ATH_MSG_WARNING( "Folder " << m_foldername << " is requested but no data retrieved" );
  }
}

std::unique_ptr<SG::TransientAddress>
IOVDbFolder::preLoadFolder(StoreGateSvc* detStore,const unsigned int cacheRun,
    const unsigned int cacheTime){
  // preload Address from SG - does folder setup including COOL access
  // also set detector store location - cannot be done in constructor
  // as detector store does not exist yet in IOVDbSvc initialisation
  // and sets up cache length, taking into account optional overrides
  // returns null pointer in case of problem
  ATH_MSG_DEBUG( "preLoadFolder for folder " << m_foldername);
  p_detStore=detStore;
  std::string folderdesc;
  cool::IDatabasePtr dbPtr;
  cool::IFolderPtr fldPtr;
  if (m_metacon==0) {
    // folder being read from COOL
    // get COOL database - will wake up connection on first access
    dbPtr=m_conn->getCoolDb();
    if (dbPtr.get()==0) {
      ATH_MSG_FATAL( "Conditions database connection " << m_conn->name() << " cannot be opened - STOP" );
      return 0;
    }
    // get folder and read information
    if (!dbPtr->existsFolder(m_foldername)) {
      ATH_MSG_FATAL( "Folder " << m_foldername << " does not exist" );
      return 0;
    }
    fldPtr=dbPtr->getFolder(m_foldername);
    // get versiontype of folder
    m_multiversion=(fldPtr->versioningMode()==cool::FolderVersioning::MULTI_VERSION);
    // read and process description string
    folderdesc=fldPtr->description();
  } else {
    // folder from meta-data
    folderdesc=m_metacon->folderDescription();
  }
  ATH_MSG_DEBUG( "Folder description " << folderdesc);

  // register folder with meta-data tool if writing metadata
  if (m_writemeta) {
    if (StatusCode::SUCCESS!=p_metaDataTool->registerFolder(m_foldername,folderdesc)) {
      ATH_MSG_ERROR( "Failed to register folder " << m_foldername<< " for meta-data write" );
      return 0;
    }
  }
  // parse the description string
  IOVDbParser folderpar(folderdesc,m_msg.get());
  // check for timeStamp indicating folder is timestamp indexed
  std::string buf;
  m_timestamp=(folderpar.getKey("timeStamp","",buf) && buf=="time");
  // check for key, giving a different key to the foldername
  if (folderpar.getKey("key","",buf) && !m_jokey) {
    ATH_MSG_DEBUG( "Key for folder " << m_foldername << " set to "
             << buf << " from description string" );
    m_key=buf;
  }
  // check for 'cache' but only if not already found in joboptions
  if (m_cachepar=="")
    folderpar.getKey("cache","",m_cachepar);
  // check for cachehint
  if (folderpar.getKey("cachehint","",buf)) 
    m_cachehint=atoi(buf.c_str());
  // check for <named/>   
  m_named=folderpar.getKey("named","",buf);

  // decode the address header to find the CLID (optional)
  bool gotCLID=false;
  // get addressHeader
  if (folderpar.getKey("addrHeader","",m_addrheader)) {
    IOVDbNamespace::replaceServiceType71(m_addrheader);
    ATH_MSG_DEBUG( "Decode addrHeader " << m_addrheader);
    IOVDbParser addrH(m_addrheader,m_msg.get());
    std::string addr2;
    // parse addrHeader XML of form
    // <addrHeader><address_header service_type="256" clid="12345" /></addrHeader>
    if (addrH.getKey("address_header","",addr2)) {
      // look for 'clid', then number between quotes afterwards
      m_clid = IOVDbNamespace::parseClid(addr2);
      ATH_MSG_DEBUG( "Got CLID " << m_clid << " from " << addr2 );
      gotCLID=(m_clid>0);
    }
  }
  // decode the typeName
  if (!folderpar.getKey("typeName","",m_typename)) {
    ATH_MSG_ERROR( "Primary type name is empty" );
    return 0;
  }
  ATH_MSG_DEBUG( "Got folder typename " << m_typename );
  if (!gotCLID)
    if (StatusCode::SUCCESS==p_clidSvc->getIDOfTypeName(m_typename,m_clid)) 
      gotCLID=true;
  if (!gotCLID) {
    ATH_MSG_ERROR("Could not get clid for typeName: " << m_typename);
    return 0;
  }
  ATH_MSG_DEBUG( "Got folder typename " << m_typename <<  " with CLID " << m_clid );

  // setup channel list and folder type
  if (m_metacon==0) {
    // data being read from COOL
    // get the list of channels
    if (m_named) {
      const std::map<cool::ChannelId,std::string> chanmap=fldPtr->listChannelsWithNames();
      m_channums.reserve(chanmap.size());
      m_channames.reserve(chanmap.size());
      for (auto & thisChannel:chanmap){
        m_channums.push_back(thisChannel.first);
        m_channames.push_back(thisChannel.second);
      }
      m_nchan=m_channums.size();
      ATH_MSG_DEBUG( "Retrieving list of channel numbers/names: got "<< m_nchan << " channels " );
    } else {
      m_channums=fldPtr->listChannels();
      m_nchan=m_channums.size();
      ATH_MSG_DEBUG( "Retrieving list of channel numbers only: got "<< m_nchan << " channels " );
    }

    // set folder type 
    m_foldertype = IOVDbNamespace::determineFolderType(fldPtr);
    ATH_MSG_DEBUG( "Folder identified as type " << m_foldertype );
  }
  // note that for folders read from metadata, folder type identification
  // is deferred until getAddress when first data is read
  // and channel number/name information is not read

  // change channel selection for single-object read
  if (m_foldertype==AttrList || m_foldertype==PoolRef)
    m_chansel=cool::ChannelSelection(0);

  // now create TAD
  auto tad = CxxUtils::make_unique<SG::TransientAddress>(m_clid,m_key);

  // process symlinks, if any
  if (folderpar.getKey("symlinks","",buf)) {
    // symlinks specified as <symlink>A:B:C</symlink> i.e. colon-sep list
    const auto linknameVector=IOVDbNamespace::parseLinkNames(buf);
    for (const auto & linkname:linknameVector){
      if (not linkname.empty()) {
        CLID sclid;
        if (StatusCode::SUCCESS==p_clidSvc->getIDOfTypeName(linkname,sclid)) {
          tad->setTransientID(sclid);
          ATH_MSG_DEBUG( "Setup symlink " << linkname << " CLID " <<sclid << " for folder " << m_foldername );
        } else {
          ATH_MSG_ERROR( "Could not get clid for symlink: "<< linkname );
          return 0;
        }
      }
   }
  }
  // setup cache length
  if (m_timestamp) {
    long long int clen=600; // default value of 10 minutes
    if (cacheTime!=0) {
      clen=cacheTime;
      m_autocache=false;
    } else {
      // for timestamp, cache parameter (if set) sets length in seconds
      if (m_cachepar!="") clen=atoi(m_cachepar.c_str());
    }
    m_cachelength=clen*1000000000LL;
    ATH_MSG_DEBUG( "Cache length set to " << clen << " seconds" );
  } else {
    // for run/event, cache parameter sets length in LB
    // default value is 1 whole run (-1 to avoid getting the next run)
    m_cachelength=(1LL << 32) - 1;
    if (cacheRun!=0) {
      m_cachelength=(static_cast<unsigned long long>(cacheRun) << 32) - 1;
      m_autocache=false;
    } else {
      if (m_cachepar!="") m_cachelength=atoi(m_cachepar.c_str());
    }
    ATH_MSG_DEBUG( "Cache length set to " << (m_cachelength >> 32) <<
        " runs " << (m_cachelength & 0xFFFFFFFF) << " lumiblocks" );
  }
  return tad;
}

// private implementation methods
/**
IOVTime 
IOVDbFolder::makeTime(const cool::ValidityKey key) {
  IOVTime time;
  if (m_timestamp) 
    time.setTimestamp(key);
  else 
    time.setRETime(key);
  return time;
}

**/
IOVRange 
IOVDbFolder::makeRange(const cool::ValidityKey since,const cool::ValidityKey until) {
  // make an IOVRange object corresponding to given interval
  // dealing with timestamp/RunLB differences
  IOVTime itsince,ituntil;
  if (m_timestamp) {
    itsince.setTimestamp(since);
    ituntil.setTimestamp(until);
  } else {
    itsince.setRETime(since);
    ituntil.setRETime(until);
  }
  return IOVRange(itsince,ituntil);
}


cool::ChannelId IOVDbFolder::makeChannel(const std::string& strval,
                                         const cool::ChannelId defchan) {
  // construct a cool channelId from the string value (numeric)
  // if empty, use the default value
  if (strval!="") return atol(strval.c_str());
  return defchan;
}


void IOVDbFolder::clearCache() {
  // clear all the cache vectors of information
  m_cachesince.clear();
  m_cacheuntil.clear();
  m_cachechan.clear();
  m_cacheattr.clear();
  m_cacheccstart.clear();
  m_cacheccend.clear();
}


bool 
IOVDbFolder::resolveTag(cool::IFolderPtr fptr,
                             const std::string& globalTag) {
  // resolve the tag 
  // if specified in job options or already-processed override use that,
  // else use global tag
  // return false for failure
  std::string tag=m_jotag;
  if (tag=="HEAD") return true;
  if (tag=="") tag=globalTag;
  if (tag=="") {
    ATH_MSG_ERROR( "No IOVDbSvc.GlobalTag specified on job options or input file" );
    return false;
  }
  // check for magic tags
  if (tag.substr(0,7)=="TagInfo" && tag.find('/')!=std::string::npos) {
    if (!magicTag(tag)) return false;
  }
  // check tag exists - if not, lookup hierarchically
  const std::vector<std::string>& taglist=fptr->listTags();
  if (find(taglist.begin(),taglist.end(),tag)!=taglist.end()) {
    // tag exists directly in folder
    ATH_MSG_DEBUG( "Using tag "<< tag << " for folder " << m_foldername );
  } else {
    // tag maybe an HVS tag 
    try {
      std::string restag=fptr->resolveTag(tag);
      ATH_MSG_INFO( "HVS tag " << tag << " resolved to "<< restag << " for folder " << m_foldername );
      // HVS tag may itself be magic
      if (restag.substr(0,7)=="TagInfo" && 
          restag.find('/')!=std::string::npos) {
        if (!magicTag(restag)) return false;
      }
      tag=restag;
    }catch (cool::Exception& e) {
      ATH_MSG_ERROR( "Tag " << tag <<" cannot be resolved for folder " << m_foldername );
      return false;
    }
  }
  m_tag=tag;
  // optionally check if tag is locked
  if (m_checklock) {
    try {
      if (fptr->tagLockStatus(tag)!=cool::HvsTagLock::LOCKED) {
        ATH_MSG_ERROR("Tag " << tag <<" is not locked and IOVDbSvc.CheckLock is set" );
        return false;
      }
    } catch (cool::Exception& e) {
      ATH_MSG_ERROR( "Could not check tag lock status for " << tag );
      return false;
    }
  }
  return true;
}


bool 
IOVDbFolder::magicTag(std::string& tag) {
  // tag an inputag of form TagInfo{Major|Minor}/<tag> or 
  // TagInfo(Major|Minor}/<prefix>/<tag>
  // and resolve to value of TagInfo object tag <tag>
  // with optional prefix
  // <prefix>DEFAULT tags are no longer returned
  // return true for successful resolution
  const std::string inputtag=tag;
  tag="";
  std::string::size_type rstrip=0;
  enum ResultIndices{WHOLETAG, MAJMIN, PATH1, PATH2, SIZEWITHPREFIX};
  auto results=IOVDbNamespace::parseMagicTag(inputtag);
  if (results.empty())   return false;
  //number of characters to strip from tag end
  rstrip = (results[MAJMIN] == "Major")?6:3;
  //revisit with C++17 in mind
  const auto & resultPair = IOVDbNamespace::tag2PrefixTarget(results);
  const std::string & prefix=resultPair.first;
  const std::string & target=resultPair.second;
  ATH_MSG_DEBUG("In magicTag for tag name: " << target << " prefix " << prefix );

  // try to get TagInfo object
  const TagInfo* tagInfo=0;
  if (StatusCode::SUCCESS==p_detStore->retrieve(tagInfo)) {
    std::string tagval;
    tagInfo->findTag(target,tagval);
    if (tagval!="") tag=prefix+tagval;
  }
  // if nothing found, try to get GeoAtlas directly from GeoModelSvc
  if (tag=="" && target=="GeoAtlas") {
    ATH_MSG_DEBUG( "Attempt to get GeoAtlas tag directly from GeoModelSvc" );
    IGeoModelSvc* geomodel=0;
    ISvcLocator* svcLocator=Gaudi::svcLocator();
    if (svcLocator==0 || 
        StatusCode::SUCCESS!=svcLocator->service("GeoModelSvc",geomodel)) {
      ATH_MSG_ERROR( "Cannot get GeoModelSvc" );
    } else { 
      tag=prefix+geomodel->atlasVersion();
      ATH_MSG_DEBUG( "Resolved tag " << target << " to " << tag << " directly from GeoModel" );
    }
  }
  if (tag!="") {
    // check if characters need to be stripped from end of tag
    if (rstrip>0 && tag.size()>rstrip) tag=tag.substr(0,tag.size()-rstrip);
    ATH_MSG_DEBUG( "Resolved TagInfo tag " << target  << " to value " << tag );
  } else {
    ATH_MSG_ERROR( "Could not resolve TagInfo tag " << target );
  }
  return (not tag.empty());
}


bool IOVDbFolder::addMetaAttrList(const coral::AttributeList& atrlist,
                                  const IOVRange& range) {
  // make a temporary CondAttrListCollection with channel 0xFFFF
  // This channel number is used to flag on readback that an 
  // AthenaAttributeList and not a CondAttrListCollection must be created
  CondAttrListCollection tmpColl(!m_timestamp);
  tmpColl.add(0xFFFF,atrlist);
  tmpColl.add(0xFFFF,range);
  return addMetaAttrListColl(&tmpColl);
}

bool IOVDbFolder::addMetaAttrListColl(const CondAttrListCollection* coll) {
  // send given payload to folder metadata
  // make a new CondAttrListCollection for the payload
  CondAttrListCollection* flmdColl=new CondAttrListCollection(*coll);
  if (StatusCode::SUCCESS!=p_metaDataTool->addPayload(m_foldername,flmdColl)) {
    ATH_MSG_ERROR( "addMetaAttrList: Failed to write metadata for folder " << m_foldername);
    return false;
  } else {
    ATH_MSG_DEBUG( "addMetaAttrList: write metadata for folder " << m_foldername );
    return true;
  }
}


void 
IOVDbFolder::setSharedSpec(const coral::AttributeList& atrlist) {
  m_cachespec=new coral::AttributeListSpecification;
  for (const auto & attribute:atrlist){
    const coral::AttributeSpecification& aspec=attribute.specification();
    m_cachespec->extend(aspec.name(),aspec.type());
    if (not typeSizeIsKnown(attribute)) {
      ATH_MSG_WARNING( "addType: unknown type " << aspec.typeName()<< 
      " in folder " << m_foldername <<  " will not be counted for bytes-read statistics" );
    }
  }
  m_nbytesread+=IOVDbNamespace::attributeListSize(atrlist);
  ATH_MSG_DEBUG( "Setup shared AttributeListSpecification with " <<  m_cachespec->size() << " elements" );
}


void IOVDbFolder::addIOVtoCache(cool::ValidityKey since,cool::ValidityKey until) {
  // add IOV to the cache, and check for IOVs crossing cache boundaries
  // indicating missing channels
  m_cachesince.push_back(since);
  m_cacheuntil.push_back(until);
  // does IOV cross lower cache boundary?
  if (since<=m_cachestart && until>m_cachestart) {
    --m_nboundmin;
    if (since>m_boundmin) m_boundmin=since;
  }
  if (since<m_cachestop && until>m_cachestop) {
    --m_nboundmax;
    if (until<m_boundmax) m_boundmax=until;
  }
}

void IOVDbFolder::printCache(){
    ATH_MSG_DEBUG("folder cache printout -------------------");
    ATH_MSG_DEBUG("length: "<<m_cachelength<<"\tstart: "<<m_cachestart<<"\tstop: "<<m_cachestop);
    ATH_MSG_DEBUG("current range: "<<m_currange);
    std::vector<cool::ValidityKey>::iterator i= m_cachesince.begin();
    std::vector<cool::ValidityKey>::iterator o= m_cacheuntil.begin();
    std::vector<cool::ChannelId>::iterator      ci= m_cachechan.begin();
    for (;i!=m_cachesince.end();++i,++o,++ci){
      ATH_MSG_DEBUG("channelID:\t"<<(*ci)<<"\t since: "<<(*i)<<"\t until: "<<(*o));
    }
    ATH_MSG_DEBUG("folder cache printout -------------------");
  
}
