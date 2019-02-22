/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// IOVDbFolder.h
// helper class for IOVDbSvc managing folder access
// Richard Hawkings, started 24/11/08
#ifndef IOVDBSVC_IOVDBFOLDER_H
#define IOVDBSVC_IOVDBFOLDER_H

#include<string>
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

class MsgStream;
class IOVDbConn;
class IOVMetaDataContainer;
class IOpaqueAddress;
class IAddressCreator;
class StoreGateSvc;
class IIOVDbMetaDataTool;
class CondAttrListCollection;

class IOVDbFolder {
public:
  IOVDbFolder(IOVDbConn* conn,const IOVDbParser& folderprop, MsgStream* msg,
              IClassIDSvc* clidsvc,const bool checkglock);
  ~IOVDbFolder();
  // enum for folder types
  enum FolderType {
    AttrList=0,
    AttrListColl,
    PoolRef,
    PoolRefColl,
    CoraCool,
    CoolVector
  };

  // access methods to various internal information
  const std::string& folderName() const;
  const std::string& key() const;
  IOVDbConn* conn() const;
  bool multiVersion() const;
  bool timeStamp() const;
  bool tagOverride() const;
  bool retrieved() const;
  bool noOverride() const;
  FolderType folderType() const;
  bool readMeta() const;
  bool writeMeta() const;
  // read from meta data only, otherwise ignore folder
  bool fromMetaDataOnly() const;
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
                  IOpaqueAddress*& address,
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
  // convert COOL ValidityKey to IOVTime, taking into account m_timestamp
  IOVTime makeTime(const cool::ValidityKey key);
  // convert two ValidityKeys into IOVRange, taking into account m_timestamp
  IOVRange makeRange(const cool::ValidityKey since, 
                     const cool::ValidityKey until);
  // make a channel ID from its string representation, use defchan if empty
  cool::ChannelId makeChannel(const std::string& strval,
                              const cool::ChannelId defchan);
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
  // add type information to calculate size of attributeList
  void addType(const std::string& type,const unsigned int ielm);
  // setup shared AttributeListSpecification cache
  void setSharedSpec(const coral::AttributeList& atrlist);
  // calculate bytesize of given attributeList, using cached typeinfo
  void countSize(const coral::AttributeList& atrlist);
  // return if channel is selected, compensating for COOL channel seln bug
  bool isChanSelected(const cool::ChannelId chan);
  // strip leading/trailing spaces from given string
  std::string spaceStrip(const std::string& input) const;
  // add this IOV to cache, including channel counting if over edge of cache
  void addIOVtoCache(cool::ValidityKey since, cool::ValidityKey until);

  // cache update for online mode
  void specialCacheUpdate(CoraCoolObjectPtr obj,
                          const ServiceHandle<IIOVSvc>& iovSvc);

  void specialCacheUpdate(const cool::IObject& obj,
                          const ServiceHandle<IIOVSvc>& iovSvc);

  MsgStream*           m_log;
  StoreGateSvc*        p_detStore;     // pointer to detector store
  IClassIDSvc*         p_clidSvc;      // pointer to CLID service
  IIOVDbMetaDataTool*  p_metaDataTool; // pointer to metadata tool (writing)
  IOVDbConn*           m_conn;         // pointer to corresponding IOVDbConn object (=0 FLMD)
  std::string m_foldername; // COOL foldername
  std::string m_key;   // SG key where data is loaded (unique)
  bool m_multiversion; // is folder multiversion
  bool m_timestamp;    // is folder indexed by timestamp (else runLB)
  bool m_tagoverride;  // is tag reset from override (needed for FLMD)
  bool m_notagoverride;// tag must not be overridden from input file
  bool m_writemeta;    // is writing to metadata
  bool m_fromMetaDataOnly; // to be read from metadata only
  bool m_named;        // folder has named channels
  bool m_iovoverridden;// folder has IOV override
  bool m_jokey;        // folder has non-default key from joboptions
  bool m_dropped;      // data object was dropped from SG
  bool m_autocache;    // indicates if cache length was automatically set
  bool m_checklock;    // indicates if global tags should be checked locked
  cool::ValidityKey m_iovoverride; // validity key to use
  FolderType        m_foldertype; // type of data in folder (enum)
  const IOVMetaDataContainer* m_metacon; // metadata container (=0 if not FLMD)

  cool::ValidityKey m_cachelength; // length of cache
  cool::ValidityKey m_cachestart;  // lowest time valid in cache
  cool::ValidityKey m_cachestop;   // 1+highest time valid in cache
  int m_cachehint; // cachehint value (set initial size to Nxchan)
  int m_cacheinc;  // number of cache increments performed

  cool::ChannelSelection m_chansel; // COOL channel selection
  typedef std::pair<cool::ChannelId,cool::ChannelId> ChanRange;
  std::vector<ChanRange> m_chanrange; // explicit list of channel ranges
  // channel range list needed to work around COOL bug 42708 which does not
  // properly select channels in non-contiguous ranges
  std::string m_jotag; // raw tag found on job options
  std::string m_tag;   // resolved tag actually used for lookup
  std::string m_typename; // typename, read from folder description
  std::string m_eventstore; // associated event store name
  std::string m_cachepar; // folder cache parameter read from jobopt/desc
  std::string m_addrheader; // address header string from folder description
  CLID m_clid;         // CLID, read from folder description or ClassIDSvc

  // read statistcs
  typedef std::pair<unsigned int, unsigned int> VarSizePair;
  std::vector<VarSizePair> m_varfields;
  unsigned int m_fixedfields;
  unsigned int m_ndbread; // number of times data read from DB
  unsigned int m_ncacheread; // number of times data read from cache
  unsigned int m_nobjread; // number of objects read from DB
  unsigned long long m_nbytesread; // number of bytes read from DB
  float m_readtime; // time spent reading data from COOL (in loadcache)

  // channel number and names (latter only filled for 'named' folders)
  unsigned int m_nchan;
  std::vector<cool::ChannelId> m_channums;
  std::vector<std::string> m_channames;

  // current range and validity flag
  bool m_retrieved;
  IOVRange m_currange;

  // COOL data cache and limits
  coral::AttributeListSpecification* m_cachespec;
  cool::ValidityKey m_boundmin;
  cool::ValidityKey m_boundmax;
  int m_nboundmin;   // Number of chans NOT in cache for min
  int m_nboundmax;   // Number of chans NOT in cache for max
  std::vector<cool::ValidityKey> m_cachesince;
  std::vector<cool::ValidityKey> m_cacheuntil;
  std::vector<cool::ChannelId> m_cachechan;
  std::vector<coral::AttributeList> m_cacheattr;
  std::vector<unsigned int> m_cacheccstart;
  std::vector<unsigned int> m_cacheccend;
};

inline const std::string& IOVDbFolder::folderName() const {return m_foldername;}
inline const std::string& IOVDbFolder::key() const { return m_key;}

inline IOVDbConn* IOVDbFolder::conn() const { return m_conn;}

inline bool IOVDbFolder::multiVersion() const { return m_multiversion; }

inline bool IOVDbFolder::timeStamp() const { return m_timestamp; }

inline bool IOVDbFolder::tagOverride() const { return m_tagoverride; }

inline bool IOVDbFolder::noOverride() const { return m_notagoverride; }

inline bool IOVDbFolder::retrieved() const { return m_retrieved; }

inline IOVDbFolder::FolderType IOVDbFolder::folderType() const 
{return m_foldertype;}

inline bool IOVDbFolder::readMeta() const { return (m_metacon!=0); }

inline bool IOVDbFolder::writeMeta() const { return m_writemeta; }

inline bool IOVDbFolder::fromMetaDataOnly() const { return m_fromMetaDataOnly; }

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
  return (reftime>=m_cachestart && reftime<m_cachestop);
}

inline void IOVDbFolder::setDropped(const bool dropped) { m_dropped=dropped; }

#endif //  IOVDBSVC_IOVDBFOLDER_H

