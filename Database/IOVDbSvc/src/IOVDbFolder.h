/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbFolder.h
// helper class for IOVDbSvc managing folder access
// Richard Hawkings, started 24/11/08
#ifndef IOVDbSvc_IOVDbFolder_h
#define IOVDbSvc_IOVDbFolder_h

#include <string>
#include "AthenaBaseComps/AthMessaging.h"
#include "AthenaKernel/IClassIDSvc.h"
#include "AthenaKernel/IOVTime.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IIOVSvc.h"

#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "SGTools/TransientAddress.h"
#include "IOVDbParser.h"

#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"
#include <memory>
#include <algorithm>
#include "FolderTypes.h"
#include "IovStore.h"

class MsgStream;
class IOVDbConn;
class IOVMetaDataContainer;
class IOpaqueAddress;
class IAddressCreator;
class StoreGateSvc;
class IIOVDbMetaDataTool;
class CondAttrListCollection;

class IOVDbFolder : public AthMessaging {
public:
  IOVDbFolder(IOVDbConn* conn, const IOVDbParser& folderprop, MsgStream& msg,
              IClassIDSvc* clidsvc,const bool checklock, const bool outputToFile=false,
              const std::string & source="COOL_DATABASE");
  ~IOVDbFolder();
  

  // access methods to various internal information
  const std::string& folderName() const;
  const std::string& key() const;
  IOVDbConn* conn() const;
  bool multiVersion() const;
  bool timeStamp() const;
  bool tagOverride() const;
  bool retrieved() const;
  bool noOverride() const;
  IOVDbNamespace::FolderType folderType() const;
  bool readMeta() const;
  bool writeMeta() const;
  // read from meta data only, otherwise ignore folder
  bool fromMetaDataOnly() const;
  // If true, then the end time for an open-ended range will be set to just
  // past the current event.  The end time will be automatically updated on accesses
  // in subsequent events.
  bool extensible() const;
  bool dropped() const;
  const std::string& joTag() const;
  const std::string& resolvedTag() const;
  const std::string& eventStore() const;
  CLID clid() const;
  unsigned long long bytesRead() const;
  float readTime() const;
  IOVRange currentRange() const;

  // set methods - used after folder creation to set properties externally
  void setMetaCon(const IOVMetaDataContainer* con);
  // set tag override, set override flag as well if setFlag is true
  // override flag prevents reading of FLMD for this folder if present
  void setTagOverride(const std::string& tag,const bool setFlag);
  // set writeMeta flag
  void setWriteMeta(IIOVDbMetaDataTool* metadatatool);
  // set IOV overrides
  void setIOVOverride(const unsigned int run,const unsigned int lumiblock,
                      const unsigned int time);
  // mark object as dropped from Storegate
  void setDropped(const bool dropped);

  // get validityKey for folder, given current time (accounting for overrides)
  cool::ValidityKey iovTime(const IOVTime& reftime) const;

  // check cache is valid for current time
  bool cacheValid(const cool::ValidityKey reftime) const;

  // load cache for given validitykey and globalTag
  bool loadCache(const cool::ValidityKey vkey, const unsigned int cacheDiv,
                 const std::string& globalTag, const bool ignoreMissChan);

  // reset cache to empty
  void resetCache();

  // fill in object details from cache
  // set poolPayloadRequested flag if a POOL file was referenced
  bool getAddress(const cool::ValidityKey reftime,IAddressCreator* persSvc,
                  const unsigned int poolSvcContext,
                  std::unique_ptr<IOpaqueAddress>& address,
                  IOVRange& range,bool& poolPayloadRequested);

  // make summary of usage
  void summary();
  // preload address to Storegate (does folder initialisation from COOL)
  std::unique_ptr<SG::TransientAddress>
  preLoadFolder(StoreGateSvc* detStore,
                const unsigned int cacheRun,
                const unsigned int cacheTime);

  // print out cache
  void printCache();
  // reload cache in online mode if ValidityKey returns a new object
  // with start > previously used start
  bool loadCacheIfDbChanged(const cool::ValidityKey vkey,
                            const std::string& globalTag,
                            cool::IDatabasePtr dbPtr,
                            const ServiceHandle<IIOVSvc>& iovSvc);
        
private:
  // clear cache vectors
  void clearCache();
  // resolve tag in given folder, using global tag if needed
  bool resolveTag(cool::IFolderPtr fptr, const std::string& globalTag);
  // interpret given tag as a magic tag
  bool magicTag(std::string& tag);
  // call metadata writing tool for given list and range
  // - version for single channel AttributeList
  bool addMetaAttrList(const coral::AttributeList& atrlist,
                       const IOVRange& range);
  // - version for multichannel collection
  bool addMetaAttrListColl(const CondAttrListCollection* coll);
 
  // setup shared AttributeListSpecification cache
  void setSharedSpec(const coral::AttributeList& atrlist);
  
  // add this IOV to cache, including channel counting if over edge of cache
  void addIOVtoCache(cool::ValidityKey since, cool::ValidityKey until);
  
  //override intrinsic (member variable) options from the from a parsed folder description
  bool overrideOptionsFromParsedDescription(const IOVDbParser & parsedDescription);
  
  //create transient address, processing symlinks if given
  std::unique_ptr<SG::TransientAddress>
  createTransientAddress(const std::vector<std::string> & symlinks);
  
  //setup cache length according to whether timestamp==ns of epoch
  void setCacheLength(const bool timeIs_nsOfEpoch, const unsigned int cacheRun, const unsigned int cacheTime);
  
  //update the cache using either a Cool or CoraCool object (templated)
  template<class T>
  unsigned int 
  cacheUpdateImplementation(T & obj, const ServiceHandle<IIOVSvc>& iovSvc){
    const auto & objSince = obj.since();
    const auto & objUntil = obj.until();
    const auto & objChannel = obj.channelId();
    ATH_MSG_DEBUG("from DB \t chID: "<<objChannel<<"\tobjstart:\t"<<objSince<<"\t objstop: \t"
                   << objUntil );
    // The covered flag is used to check whether the
    // requested IOV time is inside the range covered
    // by the current cache. If not, a cache reset
    // will be done.
    unsigned int counter{};
    bool covered{false};
    //find the iterator distance into the channel array which gives the sought ChannelId
    const auto pCacheChannel = std::find(m_cachechan.begin(), m_cachechan.end(), objChannel);
    if (pCacheChannel != m_cachechan.end()){
      //find corresponding iov, which we shall modify
      const auto iovIdx = std::distance(m_cachechan.begin(), pCacheChannel);
      const auto & iov = m_iovs.at(iovIdx);
      if ((iov.first < objSince) and (objSince < iov.second)){ 
        // obj time is larger than cache start (and less than cache stop)
        //   ==> update cache
        ++counter;
        ATH_MSG_DEBUG("special reload needed on THIS ONE !!!!!!!!!!!");
        // just change existing IOVRange
        ATH_MSG_DEBUG("changing "<<iov.second<<"  to "<<objSince-1);
        m_iovs.extendIov(iovIdx, objSince-1);
        specialCacheUpdate(obj, iovSvc); //  reset proxy, add to cache, addIOV 
        covered = true;
      }
      if ( (objSince>=iov.first and objSince<iov.second) or (objUntil>iov.first and objUntil<=iov.second) ) covered=true;
    }
    if (!covered) {
      // cache range has not been covered, so update the cache
      ++counter;
      specialCacheUpdate(obj, iovSvc);
    }
    return counter;    
  }
  
  
  bool
  objectIteratorIsValid( cool::IObjectIteratorPtr & objItr){
    return objItr->goToNext();
  }
 
  bool
  objectIteratorIsValid(CoraCoolObjectIterPtr & objItr){
    return objItr->hasNext();
  }
  

  // cache update for online mode
  void 
  specialCacheUpdate(CoraCoolObject & obj,const ServiceHandle<IIOVSvc>& iovSvc);

  void 
  specialCacheUpdate(const cool::IObject& obj,const ServiceHandle<IIOVSvc>& iovSvc);
 
  
  StoreGateSvc*        p_detStore{nullptr};     // pointer to detector store
  IClassIDSvc*         p_clidSvc{nullptr};      // pointer to CLID service
  IIOVDbMetaDataTool*  p_metaDataTool{nullptr}; // pointer to metadata tool (writing)
  IOVDbConn*           m_conn{nullptr};         // pointer to corresponding IOVDbConn object (=0 FLMD)
  std::string m_foldername;       // COOL foldername
  std::string m_key;              // SG key where data is loaded (unique)
  bool m_multiversion{false};     // is folder multiversion
  bool m_timestamp{false};        // is folder indexed by timestamp (else runLB)
  bool m_tagoverride{false};      // is tag reset from override (needed for FLMD)
  bool m_notagoverride{false};    // tag must not be overridden from input file
  bool m_writemeta{false};        // is writing to metadata
  bool m_fromMetaDataOnly{false}; // to be read from metadata only
  bool m_extensible{false};       // is this an extensible folder?
  bool m_named{false};            // folder has named channels
  bool m_iovoverridden{false};    // folder has IOV override
  bool m_jokey{false};            // folder has non-default key from joboptions
  bool m_dropped{false};          // data object was dropped from SG
  bool m_autocache{true};         // indicates if cache length was automatically set
  bool m_checklock{true};         // indicates if global tags should be checked locked
  cool::ValidityKey m_iovoverride{0};             // validity key to use
  IOVDbNamespace::FolderType m_foldertype;        // type of data in folder (enum)
  const IOVMetaDataContainer* m_metacon{nullptr}; // metadata container (=0 if not FLMD)

  cool::ValidityKey m_cachelength{0}; // length of cache
  int m_cachehint{0};                 // cachehint value (set initial size to Nxchan)
  int m_cacheinc{0};                  // number of cache increments performed

  cool::ChannelSelection m_chansel; // COOL channel selection
  typedef std::pair<cool::ChannelId,cool::ChannelId> ChanRange;
  std::vector<ChanRange> m_chanrange; // explicit list of channel ranges
  // channel range list needed to work around COOL bug 42708 which does not
  // properly select channels in non-contiguous ranges
  std::string m_jotag;      // raw tag found on job options
  std::string m_tag;        // resolved tag actually used for lookup
  std::string m_typename;   // typename, read from folder description
  std::string m_eventstore; // associated event store name
  std::string m_cachepar;   // folder cache parameter read from jobopt/desc
  std::string m_addrheader; // address header string from folder description
  CLID m_clid{0};           // CLID, read from folder description or ClassIDSvc

  unsigned int m_ndbread{0};          // number of times data read from DB
  unsigned int m_ncacheread{0};       // number of times data read from cache
  unsigned int m_nobjread{0};         // number of objects read from DB
  unsigned long long m_nbytesread{0}; // number of bytes read from DB
  float m_readtime{0};                // time spent reading data from COOL (in loadcache)

  // channel number and names (latter only filled for 'named' folders)
  unsigned int m_nchan{0};
  std::vector<cool::ChannelId> m_channums;
  std::vector<std::string> m_channames;

  // current range and validity flag
  bool m_retrieved{false};
  IOVRange m_currange;

  // COOL data cache and limits
  coral::AttributeListSpecification* m_cachespec{nullptr};
  std::vector<cool::ChannelId> m_cachechan;
  std::vector<coral::AttributeList> m_cacheattr;
  std::vector<unsigned int> m_cacheccstart;
  std::vector<unsigned int> m_cacheccend;
  IOVDbNamespace::IovStore m_iovs;
  const bool m_outputToFile{false};
  const std::string m_source;
};

inline const std::string& IOVDbFolder::folderName() const {return m_foldername;}
inline const std::string& IOVDbFolder::key() const { return m_key;}

inline IOVDbConn* IOVDbFolder::conn() const { return m_conn;}

inline bool IOVDbFolder::multiVersion() const { return m_multiversion; }

inline bool IOVDbFolder::timeStamp() const { return m_timestamp; }

inline bool IOVDbFolder::tagOverride() const { return m_tagoverride; }

inline bool IOVDbFolder::noOverride() const { return m_notagoverride; }

inline bool IOVDbFolder::retrieved() const { return m_retrieved; }

inline IOVDbNamespace::FolderType IOVDbFolder::folderType() const 
{return m_foldertype;}

inline bool IOVDbFolder::readMeta() const { return (m_metacon!=0); }

inline bool IOVDbFolder::writeMeta() const { return m_writemeta; }

inline bool IOVDbFolder::fromMetaDataOnly() const { return m_fromMetaDataOnly; }

inline bool IOVDbFolder::extensible() const { return m_extensible; }

inline bool IOVDbFolder::dropped() const { return m_dropped; }

inline const std::string& IOVDbFolder::joTag() const { return m_jotag; }

inline const std::string& IOVDbFolder::resolvedTag() const { return m_tag; }

inline const std::string& IOVDbFolder::eventStore() const 
{ return m_eventstore; }

inline CLID IOVDbFolder::clid() const { return m_clid; }

inline unsigned long long IOVDbFolder::bytesRead() const 
{ return m_nbytesread; }

inline float IOVDbFolder::readTime() const 
{ return m_readtime; }

inline IOVRange IOVDbFolder::currentRange() const { return m_currange; }

inline bool IOVDbFolder::cacheValid(const cool::ValidityKey reftime) const {
  const auto & [cacheStart, cacheStop]=m_iovs.getCacheBounds();
  return ((reftime>cacheStart) and (reftime<cacheStop));
}

inline void IOVDbFolder::setDropped(const bool dropped) { m_dropped=dropped; }

#endif //  IOVDBSVC_IOVDBFOLDER_H

