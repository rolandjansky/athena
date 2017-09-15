/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbFolder.cxx - helper class for IOVDbSvc to manage folder & data cache
// Richard Hawkings, started 24/11/08

#include <sstream>
#include <stdexcept>

#include "GaudiKernel/MsgStream.h"
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

#include "IOVDbFolder.h"
#include "CxxUtils/make_unique.h"



IOVDbFolder::IOVDbFolder(IOVDbConn* conn,
                         const IOVDbParser& folderprop, MsgStream* msg,
                         IClassIDSvc* clidsvc, const bool checklock):
  m_log(msg),
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
  m_fixedfields(0),
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
  m_nboundmax(0)
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
  if (m_notagoverride) *m_log << MSG::INFO << 
                         "Inputfile tag override disabled for " << m_foldername << endmsg;

  // channel selection from 'channelSelection' property
  // syntax is A:B,C:D,E:F
  // :B implies zero lower limit, A: implies zero upper limit
  std::string chanspec,rangespec;
  if (folderprop.getKey("channelSelection","",chanspec) && chanspec!="") {
    // explicit setting of channel selection
    bool first=true;
    std::string::size_type index;
    do {
      index=chanspec.find(',');
      if (index!=std::string::npos) {
        rangespec=chanspec.substr(0,index);
        chanspec=chanspec.substr(index+1,chanspec.size());
      } else {
        rangespec=chanspec;
      }
      if (rangespec!="") {
        // parse the range specification, look for : separator between limits
        std::string::size_type icol=rangespec.find(':');
        cool::ChannelId lower,upper;
        if (icol!=std::string::npos) {
          lower=makeChannel(spaceStrip(rangespec.substr(0,icol)),0);
          upper=makeChannel(spaceStrip(rangespec.substr(
                                                        icol+1,rangespec.size())),std::numeric_limits<cool::ChannelId>::max());
        } else {
          lower=atol(spaceStrip(rangespec).c_str());
          upper=lower;
        }
        *m_log << MSG::INFO << "Add channel range " << lower << ":" << upper 
               << " for folder " << m_foldername << endmsg;
        if (first) {
          m_chansel=cool::ChannelSelection(lower,upper);
          first=false;
        } else {
          // COOL gives error if range is not given in pure ascending
          // or descending order
          try {
            m_chansel.addRange(lower,upper);
          }
          catch (cool::Exception& e) {
            *m_log << MSG::ERROR  <<  
              "defining channel range (must be given in ascending order)" 
                   << endmsg;
            throw;
          }
        }
        m_chanrange.push_back(ChanRange(lower,upper));
      }
    } while (index!=std::string::npos);
    *m_log << MSG::INFO << "Channel selection for folder " << m_foldername << 
      " with " << m_chanrange.size() << " ranges" << endmsg;
  }
  // check for folder-specific run/LB/time overrides
  if (folderprop.getKey("forceRunNumber","",buf)) {
    unsigned long long run=atoi(buf.c_str());
    unsigned int lb=0;
    if (folderprop.getKey("forceLumiblockNumber","",buf)) lb=atoi(buf.c_str());
    m_iovoverridden=true;
    m_iovoverride=(run << 32LL)+lb;
    *m_log << MSG::INFO << 
      "Override run/LB number to [" << run << ":" << lb << 
      "] for folder " << m_foldername << endmsg;
  }
  if (folderprop.getKey("forceTimestamp","",buf)) {
    unsigned int time=atoi(buf.c_str());
    m_iovoverridden=true;
    m_iovoverride=time*1000000000LL;
    *m_log << MSG::INFO << "Override timestamp to " << m_iovoverride << 
      " for folder " << m_foldername << endmsg;
  }
  m_fromMetaDataOnly=folderprop.getKey("metaOnly","",buf);
  if (m_fromMetaDataOnly) {
    *m_log << MSG::INFO << "Read from meta data only for folder " << m_foldername << endmsg;
  }
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
    *m_log << MSG::WARNING << "Request to override tag for folder " <<
      m_foldername << " to " << tag << " supercedes earlier override to " 
           << m_jotag << endmsg;
  } else {
    if (setFlag) m_tagoverride=true;
  }
  m_jotag=spaceStrip(tag);
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
      *m_log << MSG::INFO << 
        "Override timestamp to " << m_iovoverride << " for folder "
             << m_foldername << endmsg;
      m_iovoverridden=true;
    }
  } else {
    if (run!=0 || lumiblock!=0) {
      m_iovoverride=((static_cast<unsigned long long>(run)) << 32LL)+lumiblock;
      *m_log << MSG::INFO << 
        "Override run/LB number to [" << run << ":" << lumiblock << 
        "] for folder " << m_foldername << endmsg;
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
    if (m_timestamp)
      return reftime.timestamp();
    else 
      return reftime.re_time();
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

  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "IOVDbFolder::loadCache for folder " << m_foldername 
           << " validitykey " << vkey << endmsg;
  // if not first time through, and limit not reached,and cache was not reset, 
  // and we are going forwards in time, double cachesize
  if (m_ndbread>0 && m_cacheinc<3 && (m_cachestop!=m_cachestart) && 
      vkey>m_cachestart &&m_autocache) {
    m_cachelength*=2;
    ++m_cacheinc;
    *m_log << MSG::INFO << "Increase cache length (step " << m_cacheinc << 
      ") for folder " << m_foldername
           << " to " << m_cachelength << " at validityKey " << vkey << endmsg;
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
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "IOVDbFolder:loadCache limits set to [" 
           << m_cachestart << "," << m_cachestop << "]" << endmsg;

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
    *m_log << MSG::FATAL << "Conditions database connection " <<
      m_conn->name() << " cannot be opened - STOP" << endmsg;
    return false;
  }
  // access COOL inside try/catch in case of using stale connection
  unsigned int attempts=0;
  bool retrievedone=false;
  // check if explicit checking of channels is required to work around COOL
  // bug 42708
  // bool checkChan=(m_chanrange.size()>1);
  // now relying on COOL channel selection alone
  bool checkChan=false;
  while (attempts<2 && !retrievedone) {
    ++attempts;
    try {
      unsigned int iadd=0;
      m_boundmin=0;
      m_boundmax=cool::ValidityKeyMax;
      // check how many channels cross the cache boundaries
      // if a selection is in use, count them explicitly
      if (m_chanrange.size()>0) {
        m_nboundmin=0;
        for (std::vector<cool::ChannelId>::const_iterator citr=
               m_channums.begin();citr!=m_channums.end();++citr) {
          // if (isChanSelected(*citr)) ++m_nboundmin;
          // use internal COOL channel selection
          if (m_chansel.inSelection(*citr)) ++m_nboundmin;
        }
      } else {
        m_nboundmin=m_nchan;
      }
      if (m_log->level()<=MSG::DEBUG)
        *m_log << MSG::DEBUG << "Expecting to see " << m_nboundmin << 
          " channels" << endmsg;
      m_nboundmax=m_nboundmin;
      // check pointer is still valid - can go stale in AthenaMT environment
      // according to CORAL server tests done by Andrea Valassi (23/6/09)
      if (dbPtr.get()==0) throw std::runtime_error(
                                                   "COOL database pointer invalidated");
      // access COOL folder in case needed to resolve tag (even for CoraCool)
      cool::IFolderPtr folder=dbPtr->getFolder(m_foldername);
      // resolve the tag for MV folders if not already done so
      if (m_multiversion && m_tag=="") {
        if (!resolveTag(folder,globalTag)) return false;
        if (m_log->level()<=MSG::DEBUG) 
          *m_log << MSG::DEBUG << "resolveTag returns " << m_tag << endmsg;
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
          // skip channels which are explicitly vetoed by range
          if (checkChan && !isChanSelected(obj->channelId()))  continue;
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
            countSize(*pitr);
          }
          // save pointers to start and end
          m_cacheccstart.push_back(istart);
          m_cacheccend.push_back(m_cacheattr.size());
          ++iadd;
        }
        itr->close();
        retrievedone=true;
      } else {
        cool::IObjectIteratorPtr itr=folder->browseObjects(m_cachestart, 
                                                           m_cachestop,m_chansel,m_tag);
        while (itr->goToNext()) {
          const cool::IObject& ref=itr->currentRef();
          // skip channels which are explicitly vetoed by range
          if (checkChan && !isChanSelected(ref.channelId())) continue;
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
              countSize(atrlist);
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
            countSize(atrlist);
          }
        }
        itr->close();
        retrievedone=true;
      }
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "Retrieved " << iadd << " objects for " 
               << m_nchan << " channels into cache" << endmsg;
      m_nobjread+=iadd;
    }
    catch (std::exception& e) {
      *m_log << MSG::WARNING << "COOL retrieve attempt " << attempts << 
        " failed: " << e.what() << endmsg;
      // disconnect and reconnect
      try {
        m_conn->setInactive();
        dbPtr=m_conn->getCoolDb();
      }
      catch (std::exception& e) {
        *m_log << MSG::WARNING << "Exception from disconnect/reconnect: " << 
          e.what() << endmsg;
        // try once more to connect
        try {
          dbPtr=m_conn->getCoolDb();
        }
        catch (std::exception& e) {
          *m_log << MSG::ERROR << "Cannot reconnect to database:" << e.what()
                 << endmsg;
        }
      }
    }
  }

  if (!retrievedone) {
    *m_log << MSG::ERROR << "Could not retrieve COOL data for folder " <<
      m_foldername << " tag " << m_tag << " validityKeys [" << m_cachesince <<
      "," << m_cacheuntil << "]" << endmsg;
    return false;
  }
  // check if cache can be stretched according to extent of IOVs crossing
  // boundaries - this requires all channels to have been seen
  if (m_log->level()<=MSG::DEBUG)
    *m_log << MSG::DEBUG << "Cache retrieve missing " << m_nboundmin 
           << " lower and " << m_nboundmax << " upper channels" << endmsg;
  if ((m_nboundmin==0 || ignoreMissChan) && m_boundmin < m_cachestart) {
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "Lower cache limit extended from " 
             << m_cachestart << " to " << m_boundmin << endmsg;
    m_cachestart=m_boundmin;
  }
  if ((m_nboundmax==0 || ignoreMissChan) && m_boundmax > m_cachestop) {
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "Upper cache limit extended from " <<
        m_cachestop << " tp " << m_boundmax << endmsg;
    m_cachestop=m_boundmax;
  }
  // keep track of time spent
  const float timeinc=cachetimer.RealTime();
  m_readtime+=timeinc;
  if (m_log->level()<=MSG::DEBUG)
    *m_log << "Cache retrieve done for " << m_foldername << " with " << 
      m_cachesince.size() << " objects stored in" << std::fixed <<
      std::setw(8) << std::setprecision(2) << timeinc << " s" << endmsg;
  return true;
}

bool IOVDbFolder::loadCacheIfDbChanged(const cool::ValidityKey vkey,
                                       const std::string& globalTag, 
                                       cool::IDatabasePtr dbPtr,
                                       const ServiceHandle<IIOVSvc>& iovSvc) {
    
  if (m_log->level()<=MSG::DEBUG)     
    *m_log << MSG::DEBUG << "IOVDbFolder::recheck with DB for folder " << m_foldername
           << " validitykey: " << vkey << endmsg;
  if (!m_cachesince.size()) {
    if (m_log->level()<=MSG::DEBUG)         
      *m_log << MSG::DEBUG << "Cache empty ! returning ..." << endmsg;
    return true;
  }
        
  ++m_ndbread;
  // access COOL inside try/catch in case of using stale connection
  unsigned int attempts     = 0;
  bool         retrievedone = false;
  bool         checkChan    = false;

  while (attempts<2 && !retrievedone) {
    ++attempts;
    try {
      //      unsigned int iadd=0;
      m_boundmin=0;
      m_boundmax=cool::ValidityKeyMax;
      // check how many channels cross the cache boundaries
      // if a selection is in use, count them explicitly
      if (m_chanrange.size()>0) {
        m_nboundmin=0;
        for (std::vector<cool::ChannelId>::const_iterator citr=m_channums.begin();citr!=m_channums.end();++citr) 
          if (m_chansel.inSelection(*citr)) ++m_nboundmin;
      } 
      else        m_nboundmin=m_nchan;
      
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "Expecting to see " << m_nboundmin << " channels" << endmsg;
      m_nboundmax=m_nboundmin;
      // access COOL folder in case needed to resolve tag (even for CoraCool)
      cool::IFolderPtr folder=dbPtr->getFolder(m_foldername);
      // resolve the tag for MV folders if not already done so
      if (m_multiversion && m_tag=="") { // NEEDED OR NOT?
        if (!resolveTag(folder,globalTag)) return false;
        if (m_log->level()<=MSG::DEBUG)
          *m_log << MSG::DEBUG << "resolveTag returns " << m_tag << endmsg;
      }
      
                
                
      int counter=0;
      if (m_foldertype==CoraCool) {
        if (m_log->level()<=MSG::DEBUG) {
          *m_log<< MSG::DEBUG<<"CoraCool folder cachestart:\t"<<m_cachestart<<" \t cachestop:"<< m_cachestop<<endmsg;
          *m_log<< MSG::DEBUG<<"checking range:  "<<vkey+1<<" - "<<vkey+2<<endmsg;
        }
        // CoraCool retrieve initialise CoraCool connection
        CoraCoolDatabasePtr ccDbPtr   = m_conn->getCoraCoolDb();
        CoraCoolFolderPtr   ccfolder  = ccDbPtr->getFolder(m_foldername);

        // this returns all the objects whose IOVRanges crosses this range .
        CoraCoolObjectIterPtr itr = ccfolder->browseObjects(vkey+1, vkey+2,m_chansel,m_tag);
                                
        while (itr->hasNext()) {
          CoraCoolObjectPtr obj = itr->next();
                                        
          // skip channels which are explicitly vetoed by range
          if (checkChan && !isChanSelected(obj->channelId()))  continue;
                                        
          if (m_log->level()<=MSG::DEBUG) 
            *m_log << MSG::DEBUG<<"from DB \t chID: "<<obj->channelId()
                   <<"\tobjstart:\t"<<obj->since()<<"\t objstop: \t"
                   << obj->until() <<endmsg;
                                        
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
              if (m_log->level()<=MSG::DEBUG) {
                *m_log << MSG::DEBUG<<"special reload needed on THIS ONE !!!!!!!!!!!"<<endmsg;

                // just change existing IOVRange
                *m_log << MSG::DEBUG<<"changing "<<(*ut)<<"  to "<<obj->since()-1<<endmsg;
              }
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
        if (m_log->level()<=MSG::DEBUG) 
          *m_log << MSG::DEBUG << "Need a special update for " << counter << " objects " << endmsg;
        itr->close();
        retrievedone=true;
    

      } else {
        
        if (m_log->level()<=MSG::DEBUG) {
          *m_log << MSG::DEBUG<<"not CoraCool type.   cachestart:"<<m_cachestart<<"  cachestop:"<< m_cachestop <<endmsg;
          *m_log<< MSG::DEBUG<<"checking range:  "<<vkey+1<<" - "<<vkey+2<<endmsg;
        }
                
        // this returns all the objects whose IOVRanges crosses this range . 
        cool::IObjectIteratorPtr itr=folder->browseObjects(vkey+1, vkey+2, m_chansel,m_tag);
                
        while (itr->goToNext()) {
          const cool::IObject& ref=itr->currentRef();
                        
          // skip channels which are explicitly vetoed by range
          if (checkChan && !isChanSelected(ref.channelId())) continue;
        
          if (m_log->level()<=MSG::DEBUG) 
            *m_log << MSG::DEBUG<<"from DB -----> objstart:"<<ref.since()<<"  objstop:"
                   << ref.until() <<" chID: "<<ref.channelId()<<endmsg;
                        
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
              if (m_log->level()<=MSG::DEBUG) {
                *m_log << MSG::DEBUG<<"special reload needed on this one !!!!!!!!!!!"<<endmsg;
                // just change existing IOVRange
                *m_log << MSG::DEBUG<<"changing "<<(*ut)<<"  to "<<ref.since()-1<<endmsg;
              }
                            
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
        if (m_log->level()<=MSG::DEBUG) 
          *m_log << MSG::DEBUG << "Need a special update for " << counter << " objects " << endmsg; 
        itr->close();
        retrievedone=true;
      }      

      m_nobjread+=counter;

    }
    catch (std::exception& e) {
      *m_log << MSG::WARNING << "COOL retrieve attempt " << attempts <<  " failed: " << e.what() << endmsg;
      try { // disconnect and reconnect
        m_conn->setInactive();
        dbPtr=m_conn->getCoolDb();
      }
      catch (std::exception& e) {
        *m_log << MSG::WARNING << "Exception from disconnect/reconnect: " << e.what() << endmsg;
      }
    }
  }

  *m_log << "Special cache check finished for folder " << m_foldername << endmsg;
  return true;
}

void IOVDbFolder::specialCacheUpdate(CoraCoolObjectPtr obj,
                                     const ServiceHandle<IIOVSvc>& iovSvc) {

  // reset IOVRange in IOVSvc to trigger reset of object. Set to a
  // time earlier than since.
  IOVRange range = makeRange(obj->since()-2, obj->since()-1);
  if (StatusCode::SUCCESS != iovSvc->setRange(clid(), key(), range, eventStore())) {
    *m_log << MSG::ERROR << "IOVDbFolder::specialCacheUpdate - setRange failed for folder " 
           << folderName() << endmsg;
    return;
  }
    
  addIOVtoCache(obj->since(),obj->until());
  m_cachechan.push_back(obj->channelId());
        
  // store all the attributeLists in the buffer save pointer to start
  const unsigned int istart=m_cacheattr.size();
  for (CoraCoolObject::const_iterator pitr=obj->begin(); pitr!=obj->end();++pitr) {
    // setup shared specification on first store
    // if (m_cachespec==0) {  //never happens
    //      m_cachespec=new coral::AttributeListSpecification;
    //      m_varfields.clear();
    //      m_fixedfields=0;
    //      unsigned int ielm=0;
    //      for (coral::AttributeList::const_iterator aitr=pitr->begin(); aitr!=pitr->end();++aitr,++ielm) {
    //              const coral::AttributeSpecification& aspec=aitr->specification();
    //              m_cachespec->extend(aspec.name(),aspec.type());
    //              addType(aspec.typeName(),ielm);
    //      }
    //      *m_log << MSG::DEBUG << "Setup shared CoraCool AttributeListSpecifcation with " << m_cachespec->size() << " elements" << endmsg;
    // }
    // use the shared specification in storing the payload
    m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));
    m_cacheattr.back().fastCopyData(*pitr);
    countSize(*pitr);
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
    *m_log << MSG::ERROR << "IOVDbFolder::specialCacheUpdate - setRange failed for folder " 
           << folderName() << endmsg;
    return;
  }

  // add new object.
  addIOVtoCache(ref.since(),ref.until());
  m_cachechan.push_back(ref.channelId());
  const coral::AttributeList& atrlist = ref.payload().attributeList();
  // setup shared specification on first store
  //     if (m_cachespec==0) { // never happens
  // *m_log << MSG::DEBUG << "Setting up shared specs... " << m_key << endmsg;
  //          m_cachespec=new coral::AttributeListSpecification;
  //          m_varfields.clear();
  //          m_fixedfields=0;
  //          unsigned int ielm=0;
  //          for (coral::AttributeList::const_iterator aitr=atrlist.begin();aitr!=atrlist.end();++aitr,++ielm) {
  //                  const coral::AttributeSpecification& aspec=aitr->specification();
  //                  m_cachespec->extend(aspec.name(),aspec.type());
  //                  addType(aspec.typeName(),ielm);
  //          }
  //          *m_log << MSG::DEBUG << "Setup shared AttributeListSpecifcation with " << m_cachespec->size() << " elements" << endmsg;
  //          }
  // use the shared specification in storing the payload
  m_cacheattr.push_back(coral::AttributeList(*m_cachespec,true));// maybe needs to be cleared before
  if (m_log->level()<=MSG::DEBUG) {
    std::ostringstream os;
    atrlist.toOutputStream(os);
    *m_log << MSG::DEBUG << "AttributeList: " << os.str() << endmsg;
  }
  m_cacheattr.back().fastCopyData(atrlist);
  countSize(atrlist);
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
    // read from file metadata
    // this also sets the foldertype, as it is the first time it is known
    // get payload container, assuming it has the correct tag
    const IOVPayloadContainer* payload=m_metacon->payloadContainer();
    if (payload==0) {
      *m_log << MSG::ERROR << "Could not find IOVPayloadContainer for folder "
             << m_foldername << endmsg;
      return false;
    }
    // get payload for current time, including case where nothing returned
    IOVTime ireftime=makeTime(reftime);
    IOVPayloadContainer::const_iterator pitr=payload->find(ireftime);
    // keep track of whether we create a new CondAttrListCollection, so we 
    // can release the pointer later
    bool newpptr=false; 
    const CondAttrListCollection* pptr;
    if (pitr==payload->end()) {
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "No IOVPayloadContainer for time " 
               << ireftime << " so make empty CondAttrListCollection" << endmsg;
      pptr=new CondAttrListCollection(!m_timestamp);
      newpptr=true;
    } else {
      pptr=*pitr;
    }
    m_nobjread+=pptr->size();
    // check for single magic channel - then need an AttributeList or PoolRef
    if (pptr->size()==1 && pptr->begin()->first==0xFFFF) {
      // check if this is a poolref
      const coral::AttributeList& payload1=pptr->begin()->second;
      const coral::AttributeSpecification& spec=payload1[0].specification();
      if (spec.name()=="PoolRef" && spec.typeName()=="string") {
        // single channel with PoolRef
        strAddress=payload1[0].data<std::string>();
        poolPayloadReq=true;
        m_foldertype=PoolRef;
      } else {
        // create an AthenaAttributeList for this data
        attrList=new AthenaAttributeList(payload1);
        strAddress="POOLContainer_AthenaAttributeList][CLID=x";
        m_foldertype=AttrList;
      }
      range=(pptr->iov_begin())->second;
    } else {
      // multiple channels - check if poolref
      // this can only be done if the collection is non-empty
      // otherwise we assume it is a CondAttrListCollection
      m_foldertype=AttrListColl;
      if (pptr->size()>0) {
        const coral::AttributeList& payload1=pptr->begin()->second;
        const coral::AttributeSpecification& spec=payload1[0].specification();
        if (spec.name()=="PoolRef" && spec.typeName()=="string") {
          m_foldertype=PoolRefColl;
          poolPayloadReq=true;
        }
      }
      range=pptr->minRange();
      // have to copy the CondAttrListCollection as this will be put in 
      // detector store and eventually deleted
      attrListColl=new CondAttrListCollection(*pptr);
      // release the pointer if we created a new CondAttrListCollection object
      if (newpptr) delete pptr;
    }
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Read file metadata for folder " << m_foldername 
             << " foldertype is " << m_foldertype << endmsg;
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
          attrListColl->add(m_cachechan[ic],
                            makeRange(m_cachesince[ic],m_cacheuntil[ic]));
        } else if (m_foldertype==CoraCool || m_foldertype==CoolVector) {
          // retrieval of CoraCool data
          attrListVec->addSlice(makeRange(m_cachesince[ic],m_cacheuntil[ic]),
                                m_cachechan[ic],m_cacheattr,
                                m_cacheccstart[ic],m_cacheccend[ic]);
          if (m_writemeta) {
            *m_log << MSG::ERROR << 
              "Writing of CoraCool folders to file metadata not implemented" 
                   << endmsg;
            return false;
          }
        } else {
          *m_log << MSG::ERROR << "Unhandled folder type " << m_foldertype
                 << endmsg;
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
        *m_log << MSG::ERROR << 
          "COOL object not found in single-channel retrieve, folder " 
               << m_foldername << " currentTime " << reftime << endmsg;
        return false;
      } else if (nobj>1) {
        *m_log << MSG::ERROR << nobj << 
          " valid objects found for single-channel retrieve, folder " << 
          m_foldername << " currentTime " << reftime << endmsg;
        return false;
      }
    }
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Retrieved object: folder " << m_foldername 
             <<  " at IOV " << reftime << " channels " << nobj << " has range " 
             << range << endmsg;
    // shrink range so it does not extend into 'gap' channels or outside cache
    IOVTime tnaystart=makeTime(naystart);
    IOVTime tnaystop=makeTime(naystop);
    IOVTime rstart=range.start();
    IOVTime rstop=range.stop();
    if (tnaystart > rstart || rstop > tnaystop) {
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "Shrink IOV range from [" << rstart << ":" <<
          rstop << "] to [" << tnaystart << ":" << tnaystop << "]" << endmsg;
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
    *m_log << MSG::ERROR << "Could not get IOpaqueAddress from string address "
           << strAddress << endmsg;
    return false;
  }
  GenericAddress* gAddr=dynamic_cast<GenericAddress*>(address);
  if (!gAddr) {
    *m_log << MSG::ERROR << "Could not cast IOpaqueAddress to GenericAddress" 
           << endmsg;
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
  // summarise the read statistics for this folder
  *m_log << MSG::INFO <<  "Folder " << m_foldername << " (";
  if (m_foldertype==AttrList) {
    *m_log << "AttrList";
  } else if (m_foldertype==AttrListColl) {
    *m_log << "AttrListColl";
  } else if (m_foldertype==PoolRef) {
    *m_log << "PoolRef";
  } else if (m_foldertype==PoolRefColl) {
    *m_log << "PoolColl";
  } else if (m_foldertype==CoraCool) {
    *m_log << "CoraCool";
  } else if (m_foldertype==CoolVector) {
    *m_log << "CoolVector";
  } else {
    *m_log << "Unknown";
  }
  *m_log << ") db-read " << m_ndbread << "/" << 
    m_ncacheread << " objs/chan/bytes " << m_nobjread << "/" << 
    m_nchan << "/" << m_nbytesread << " (( " << std::fixed << std::setw(8)
         << std::setprecision(2) << m_readtime << " ))s" << endmsg;
  // print WARNING if data for this fodler was never read from Storegate
  if (m_ncacheread==0 && m_ndbread>0) {
    *m_log << MSG::WARNING << "Folder " << m_foldername << 
      " is requested but no data retrieved" << endmsg;
  }
}

std::unique_ptr<SG::TransientAddress>
IOVDbFolder::preLoadFolder(StoreGateSvc* detStore,
                           const unsigned int cacheRun,
                           const unsigned int cacheTime)
{
  // preload Address from SG - does folder setup including COOL access
  // also set detector store location - cannot be done in constructor
  // as detector store does not exist yet in IOVDbSvc initialisation
  // and sets up cache length, taking into account optional overrides
  // returns null pointer in case of problem
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "preLoadFolder for folder " << m_foldername 
           << endmsg;
  p_detStore=detStore;
  std::string folderdesc;
  cool::IDatabasePtr dbPtr;
  cool::IFolderPtr fldPtr;
  if (m_metacon==0) {
    // folder being read from COOL
    // get COOL database - will wake up connection on first access
    dbPtr=m_conn->getCoolDb();
    if (dbPtr.get()==0) {
      *m_log << MSG::FATAL << "Conditions database connection " 
             << m_conn->name() << " cannot be opened - STOP" << endmsg;
      return 0;
    }
    // get folder and read information
    if (!dbPtr->existsFolder(m_foldername)) {
      *m_log << MSG::FATAL << "Folder " << m_foldername << " does not exist" 
             << endmsg;
      return 0;
    }
    fldPtr=dbPtr->getFolder(m_foldername);
    // get versiontype of folder
    m_multiversion=(fldPtr->versioningMode()==
                    cool::FolderVersioning::MULTI_VERSION);
    // read and process description string
    folderdesc=fldPtr->description();
  } else {
    // folder from meta-data
    folderdesc=m_metacon->folderDescription();
  }
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "Folder description " << folderdesc << endmsg;

  // register folder with meta-data tool if writing metadata
  if (m_writemeta) {
    if (StatusCode::SUCCESS!=p_metaDataTool->registerFolder(m_foldername,
                                                            folderdesc)) {
      *m_log << MSG::ERROR << "Failed to register folder " << m_foldername
             << " for meta-data write" << endmsg;
      return 0;
    }
  }

  // parse the description string
  IOVDbParser folderpar(folderdesc,m_log);
  // check for timeStamp indicating folder is timestamp indexed
  std::string buf;
  m_timestamp=(folderpar.getKey("timeStamp","",buf) && buf=="time");
  // check for key, giving a different key to the foldername
  if (folderpar.getKey("key","",buf) && !m_jokey) {
    if (m_log->level()<=MSG::DEBUG)
      *m_log << MSG::DEBUG << "Key for folder " << m_foldername << " set to "
             << buf << " from description string" << endmsg;
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
    const std::size_t svcType = m_addrheader.find("service_type=\"71\"");
    if (svcType != std::string::npos) {
      m_addrheader.replace(svcType, 17, "service_type=\"256\"");
    }
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Decode addrHeader " << m_addrheader << endmsg;
    IOVDbParser addrH(m_addrheader,m_log);
    std::string addr2;
    // parse addrHeader XML of form
    // <addrHeader><address_header service_type="256" clid="12345" /></addrHeader>
    if (addrH.getKey("address_header","",addr2)) {
      // look for 'clid', then number between quotes afterwards
      std::string::size_type iofs=addr2.find("clid");
      if (iofs!=std::string::npos) {
        std::string::size_type iofs1=addr2.find("\"",iofs+4);
        if (iofs1!=std::string::npos) {
          std::string::size_type iofs2=addr2.find("\"",iofs1+1);
          if (iofs2!=std::string::npos) {
            std::istringstream is(addr2.substr(iofs1+1,iofs2-iofs1-1));
            is >> m_clid;
            if (m_log->level()<=MSG::DEBUG) 
              *m_log << MSG::DEBUG << "Got CLID " << m_clid << " from " 
                     << addr2 << endmsg;
            gotCLID=true;
          }
        }
      }
    }
  }
  // decode the typeName
  if (!folderpar.getKey("typeName","",m_typename)) {
    *m_log << MSG::ERROR << "Primary type name is empty" << endmsg;
    return 0;
  }
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "Got folder typename " << m_typename << endmsg;
  if (!gotCLID)
    if (StatusCode::SUCCESS==p_clidSvc->getIDOfTypeName(m_typename,m_clid)) 
      gotCLID=true;
  if (!gotCLID) {
    *m_log << MSG::ERROR << "Could not get clid for typeName: " << 
      m_typename << endmsg;
    return 0;
  }
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG << "Got folder typename " << m_typename << 
      " with CLID " << m_clid << endmsg;

  // setup channel list and folder type
  if (m_metacon==0) {
    // data being read from COOL
    // get the list of channels
    if (m_named) {
      const std::map<cool::ChannelId,std::string> chanmap=fldPtr->listChannelsWithNames();
      m_channums.reserve(chanmap.size());
      m_channames.reserve(chanmap.size());
      for (std::map<cool::ChannelId,std::string>::const_iterator chitr=chanmap.begin();
           chitr!=chanmap.end();++chitr) {
        m_channums.push_back(chitr->first);
        m_channames.push_back(chitr->second);
      }
      m_nchan=m_channums.size();
      if (m_log->level()<=MSG::DEBUG)
        *m_log << MSG::DEBUG << "Retrieving list of channel numbers/names: got "
               << m_nchan << " channels " << endmsg;
    } else {
      m_channums=fldPtr->listChannels();
      m_nchan=m_channums.size();
      if (m_log->level()<=MSG::DEBUG) 
        *m_log << MSG::DEBUG << "Retrieving list of channel numbers only: got "
               << m_nchan << " channels " << endmsg;
    }

    // set folder type 
    if (folderpar.getKey("coracool","",buf)) {
      m_foldertype=CoraCool;
    } else if (m_typename=="CondAttrListVec") {
      m_foldertype=CoolVector;
    } else {
      //  check for PoolRef in recordspec
      const cool::IRecordSpecification& rspec=fldPtr->payloadSpecification();
      if (rspec.exists("PoolRef") && rspec[0].name()=="PoolRef" &&
          rspec[0].storageType()==cool::StorageType::String4k) {
        if (m_nchan==1 && m_channums[0]==0) {
          m_foldertype=PoolRef;
        } else {
          m_foldertype=PoolRefColl;
        }
      } else {
        //if (m_nchan==1 && m_channums[0]==0) {
        if (m_typename.compare("CondAttrListCollection")==0) {
          m_foldertype=AttrListColl;
        } else { //typename.compare("AthenaAttributeList")==0
          m_foldertype=AttrList;
        }
      }
    }
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Folder identified as type " << m_foldertype << 
        endmsg;
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
    std::string linkname;
    std::string::size_type idx;
    do {
      // Type names may be qualified C++ names including a scope: SCOPE::NAME.
      // So ignore double colons when splitting the symlink list.
      idx = 0;
      while (true) {
        idx=buf.find(":", idx);
        if (idx == std::string::npos || buf[idx+1] != ':') break;
        idx += 2;
      }

      if (idx!=std::string::npos) {
        linkname=buf.substr(0,idx);
        buf=buf.substr(1+idx,buf.size());
      } else {
        linkname=buf;
      }
      linkname=spaceStrip(linkname);
      if (linkname!="") {
        CLID sclid;
        if (StatusCode::SUCCESS==p_clidSvc->getIDOfTypeName(linkname,sclid)) {
          tad->setTransientID(sclid);
          if (m_log->level()<=MSG::DEBUG) 
            *m_log << MSG::DEBUG << "Setup symlink " << linkname << " CLID " <<
              sclid << " for folder " << m_foldername << endmsg;
        } else {
          *m_log << MSG::ERROR << "Could not get clid for symlink: "
                 << linkname << endmsg;
          return 0;
        }
      }
    } while (idx!=std::string::npos);
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
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Cache length set to " << clen << " seconds" << 
        endmsg;
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
    if (m_log->level()<=MSG::DEBUG) 
      *m_log << MSG::DEBUG << "Cache length set to " << (m_cachelength >> 32) <<
        " runs " << (m_cachelength & 0xFFFFFFFF) << " lumiblocks" << endmsg;
  }
  return tad;
}

// private implementation methods

IOVTime IOVDbFolder::makeTime(const cool::ValidityKey key) {
  IOVTime time;
  if (m_timestamp) 
    time.setTimestamp(key);
  else 
    time.setRETime(key);
  
  return time;
}

IOVRange IOVDbFolder::makeRange(const cool::ValidityKey since,
                                const cool::ValidityKey until) {
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

bool IOVDbFolder::resolveTag(cool::IFolderPtr fptr,
                             const std::string& globalTag) {
  // resolve the tag 
  // if specified in job options or already-processed override use that,
  // else use global tag
  // return false for failure
  std::string tag=m_jotag;
  if (tag=="HEAD") return true;
  if (tag=="") tag=globalTag;
  if (tag=="") {
    *m_log << MSG::ERROR << 
      "No IOVDbSvc.GlobalTag specified on job options or input file" << endmsg;
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
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << "Using tag "
                                           << tag << " for folder " << m_foldername << endmsg;
  } else {
    // tag maybe an HVS tag 
    try {
      std::string restag=fptr->resolveTag(tag);
      *m_log << MSG::INFO << "HVS tag " << tag << " resolved to "
             << restag << " for folder " << m_foldername << endmsg;
      // HVS tag may itself be magic
      if (restag.substr(0,7)=="TagInfo" && 
          restag.find('/')!=std::string::npos) {
        if (!magicTag(restag)) return false;
      }
      tag=restag;
    }
    catch (cool::Exception& e) {
      *m_log << MSG::ERROR << "Tag " << tag << 
        " cannot be resolved for folder " << m_foldername << endmsg;
      return false;
    }
  }
  m_tag=tag;
  // optionally check if tag is locked
  if (m_checklock) {
    try {
      if (fptr->tagLockStatus(tag)!=cool::HvsTagLock::LOCKED) {
        *m_log << MSG::ERROR << "Tag " << tag << 
          " is not locked and IOVDbSvc.CheckLock is set" << endmsg;
        return false;
      }
    }
    catch (cool::Exception& e) {
      *m_log << MSG::ERROR << "Could not check tag lock status for " << tag 
             << endmsg;
      return false;
    }
  }
  return true;
}

bool IOVDbFolder::magicTag(std::string& tag) {
  // tag an inputag of form TagInfo{Major|Minor}/<tag> or 
  // TagInfo(Major|Minor}/<prefix>/<tag>
  // and resolve to value of TagInfo object tag <tag>
  // with optional prefix
  // <prefix>DEFAULT tags are no longer returned
  // return true for successful resolution
  const std::string inputtag=tag;
  tag="";
  std::string prefix="";
  // find the / after the TagInfo{Major|Minor}
  std::string::size_type ptr1=inputtag.find('/');
  if (ptr1==std::string::npos) return false;
  std::string target=inputtag.substr(ptr1+1);
  // check for TagInfoMajor or TagInfoMinor - if so, will need to strip
  // characters from the end of the looked up tag
  std::string::size_type rstrip=0;
  if (inputtag.substr(7,5)=="Major") {
    rstrip=6;
  } else if (inputtag.substr(7,5)=="Minor") {
    rstrip=3;
  }
  std::string::size_type ptr=target.find("/");
  if (ptr!=std::string::npos) {
    prefix=target.substr(0,ptr);
    target=target.substr(ptr+1);
  }
  if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
                                    "In magicTag for tag name: " << target << " prefix " << prefix << endmsg;

  // try to get TagInfo object
  const TagInfo* tagInfo=0;
  if (StatusCode::SUCCESS==p_detStore->retrieve(tagInfo)) {
    std::string tagval;
    tagInfo->findTag(target,tagval);
    if (tagval!="") tag=prefix+tagval;
  }
  // if nothing found, try to get GeoAtlas directly from GeoModelSvc
  if (tag=="" && target=="GeoAtlas") {
    if (m_log->level()<=MSG::DEBUG)  *m_log << MSG::DEBUG << 
                                       "Attempt to get GeoAtlas tag directly from GeoModelSvc" << endmsg;
    IGeoModelSvc* geomodel=0;
    ISvcLocator* svcLocator=Gaudi::svcLocator();
    if (svcLocator==0 || 
        StatusCode::SUCCESS!=svcLocator->service("GeoModelSvc",geomodel)) {
      *m_log << MSG::ERROR << "Cannot get GeoModelSvc" << endmsg;
    } else { 
      tag=prefix+geomodel->atlasVersion();
      *m_log << MSG::INFO << "Resolved tag " << target << " to " << 
        tag << " directly from GeoModel" << endmsg;
    }
  }
  if (tag!="") {
    // check if characters need to be stripped from end of tag
    if (rstrip>0 && tag.size()>rstrip) tag=tag.substr(0,tag.size()-rstrip);
    *m_log << MSG::INFO << "Resolved TagInfo tag " << target 
           << " to value " << tag << endmsg;
  } else {
    *m_log << MSG::ERROR << "Could not resolve TagInfo tag " << target 
           << endmsg;
  }
  return (tag!="");
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
    *m_log << MSG::ERROR << 
      "addMetaAttrList: Failed to write metadata for folder " << m_foldername
           << endmsg;
    return false;
  } else {
    if (m_log->level()<=MSG::DEBUG) *m_log << MSG::DEBUG << 
                                      "addMetaAttrList: write metadata for folder " << m_foldername << endmsg;
    return true;
  }
}

void IOVDbFolder::addType(const std::string& type,const unsigned int ielm) 
{
  if (type=="int" || type=="unsigned int" || type=="float") {
    m_fixedfields+=4;
  } else if (type=="long long" || type=="unsigned long long" || type=="double") {
    m_fixedfields+=8;
  } else if (type=="bool" || type=="unsigned char" || type=="char") {
    m_fixedfields+=1;
  } else if (type=="short" || type=="unsigned short") {
    m_fixedfields+=2;
  } else if (type=="string") {
    m_varfields.push_back(VarSizePair(1,ielm));
  } else if (type=="blob") {
    m_varfields.push_back(VarSizePair(2,ielm));
  } else {
    *m_log << MSG::WARNING << "addType: unknown type " << type << 
      " in folder " << m_foldername << 
      " will not be counted for bytes-read statistics" << endmsg;
  }
}

void IOVDbFolder::setSharedSpec(const coral::AttributeList& atrlist) {
  m_cachespec=new coral::AttributeListSpecification;
  m_varfields.clear();
  m_fixedfields=0;
  unsigned int ielm=0;
  for (coral::AttributeList::const_iterator aitr=atrlist.begin();
       aitr!=atrlist.end();++aitr,++ielm) {
    const coral::AttributeSpecification& aspec=aitr->specification();
    m_cachespec->extend(aspec.name(),aspec.type());
    addType(aspec.typeName(),ielm);
  }
  if (m_log->level()<=MSG::DEBUG) 
    *m_log << MSG::DEBUG 
           << "Setup shared AttributeListSpecifcation with " << 
      m_cachespec->size() << " elements" << endmsg;
}

void IOVDbFolder::countSize(const coral::AttributeList& atrlist) {
  m_nbytesread+=m_fixedfields;
  if (m_varfields.size()>0) {
    // add size of string 
    for (std::vector<VarSizePair>::const_iterator itr=m_varfields.begin();
         itr!=m_varfields.end();++itr) {
      if (itr->first==1) {
        // string object
        m_nbytesread+=(atrlist[itr->second].data<std::string>()).size();
      } else {
        // blob object
        m_nbytesread+=(atrlist[itr->second].data<coral::Blob>()).size();
      }
    }
  }
}

bool IOVDbFolder::isChanSelected(const cool::ChannelId chan) {
  // simple check of channel ranges
  bool sel=false;
  for (std::vector<ChanRange>::const_iterator itr=m_chanrange.begin();
       itr!=m_chanrange.end(); ++itr) {
    if (chan>=itr->first && chan<=itr->second) {
      sel=true;
      break;
    }
  }
  return sel;
}

std::string IOVDbFolder::spaceStrip(const std::string& input) const {
  // return the input string stripped of leading/trailing spaces
  std::string::size_type idx1=input.find_first_not_of(" ");
  std::string::size_type idx2=input.find_last_not_of(" ");
  if (idx1==std::string::npos || idx2==std::string::npos) {
    return "";
  } else {
    return input.substr(idx1,1+idx2-idx1);
  }
}

void IOVDbFolder::addIOVtoCache(cool::ValidityKey since,
                                cool::ValidityKey until) {
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
  if (m_log->level()<=MSG::DEBUG) {
    *m_log<<MSG::DEBUG<<"folder cache printout -------------------"<<endmsg;
    *m_log<<MSG::DEBUG<<"length: "<<m_cachelength<<"\tstart: "<<m_cachestart<<"\tstop: "<<m_cachestop<<endmsg;
    *m_log<<MSG::DEBUG<<"current range: "<<m_currange<<endmsg;
    std::vector<cool::ValidityKey>::iterator i= m_cachesince.begin();
    std::vector<cool::ValidityKey>::iterator o= m_cacheuntil.begin();
    std::vector<cool::ChannelId>::iterator      ci= m_cachechan.begin();
    for (;i!=m_cachesince.end();++i,++o,++ci){
      *m_log<<MSG::DEBUG<<"channelID:\t"<<(*ci)<<"\t since: "<<(*i)<<"\t until: "<<(*o)<<endmsg;
    }
    *m_log<<MSG::DEBUG<<"folder cache printout -------------------"<<endmsg;
  }
}
