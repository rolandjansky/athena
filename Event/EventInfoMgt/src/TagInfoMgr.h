//Dear emacs, this is -*-c++-*-
/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOMGT_TAGINFOMGR_H
# define EVENTINFOMGT_TAGINFOMGR_H
/**
 * @file TagInfoMgr.h
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object with current tag values. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "EventInfoMgt/ITagInfoMgr.h"
#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthService.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaKernel/IOVRange.h"
#include "AthenaKernel/IIOVDbSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "EventInfo/TagInfo.h"
#include "IOVDbMetaDataTools/IIOVDbMetaDataTool.h"
#include "AthenaKernel/IAddressProvider.h"

#include <map>
#include <set>
#include <shared_mutex>

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;
class CondAttrListCollection;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class TagInfoMgr
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object in the Detector
 *  Store with current tag values.
 *
 *  The tags to fill the TagInfo object may come from a variety of
 *  sources: i.e. the tags specified by the GeoModelSvc and IOVDbSvc,
 *  or the tags stored in the incoming EventInfo object, or the
 *  TagInfo object itself may be in the IOV DB. The TagInfoMgr itself
 *  only adds in tags from the incoming event. Others are added by
 *  external clients.
 *
 *  The interface provides a 'input' method to allow client to add in
 *  their own tags:
 *     
 *  virtual StatusCode addTag(const std::string& tagName, 
 *                            const std::string& tagValue) = 0;
 *
 *  The full set of tags in TagInfo are copied each event into
 *  EventInfo (specifically EventType) to be written out in the event
 *  stream. Currently, the tags are NOT written to the IOV DB via the
 *  TagInfoMgr. 
 *
 */
class TagInfoMgr : virtual public AthService,
                   virtual public ITagInfoMgr,
                   virtual public IAddressProvider,
                   virtual public IIncidentListener
{
public:

    /// Constructor with parameters:
    TagInfoMgr(const std::string &name,ISvcLocator *pSvcLocator);
    ~TagInfoMgr();

    /// @name Basic service methods
    //@{
    virtual StatusCode   initialize() override;
    virtual StatusCode   start() override;
    virtual StatusCode   finalize() override;
  
    /// Query the interfaces.
    virtual StatusCode   queryInterface( const InterfaceID& riid, void** ppvInterface ) override;
    //@}

    /// @name Interface methods
    //@{
    /// @name Method to allow clients add in tags as: tag name, tag value
    virtual StatusCode   addTag(const std::string& tagName, 
                              const std::string& tagValue) override;

    /// Method to allow clients to remove a tag which may have come in
    /// on the input
    virtual StatusCode   removeTagFromInput(const std::string& tagName) override;

    /// callback from IOVSvc - only used as test of callback
    StatusCode           checkTagInfo(IOVSVC_CALLBACK_ARGS);

    /// Find tag by name, return by value
    virtual std::string  findTag ATLAS_CHECK_THREAD_SAFETY (const std::string & name) const override final;

    /// Find tag by its name - for input tags, return by value
    virtual std::string  findInputTag ATLAS_CHECK_THREAD_SAFETY (const std::string& name) const override final;

    /// Return a vector with all current input tags
    virtual NameTagPairVec getInputTags() const override final;

    /// Dump the content of the current TagInfo to std::string for debug
    virtual std::string  dumpTagInfoToStr() const override final;

    /// Printout method
    virtual void         printTags(MsgStream& log) const override final;

    /// Add a Listener to the notification list for TagInfo changes
    virtual void         addListener(Listener* listener) override final;

    /// Remove a Listener from the notification list for TagInfo changes
    virtual void         removeListener(Listener* listener) override final;
    //@}
   
    /// @name TagInfo management methods:
    //@{
    void                 handle(const Incident& incident) override final;

    /// Update Tags when input tags (/TagInfo in-file metadata) change
    StatusCode           updateTagInfo();

    /// Notify all listeners that the Tags were updated.
    void                 notifyListeners() const;
    /// Callback from IOVSvc used to notifyListeners at the right time 
    StatusCode           iovCallback(IOVSVC_CALLBACK_ARGS);

    // Fake IAddressProvider interface implementation - allows to make TagInfoMgr
    // a proxy provider and attach it to an object with an IOV callback
    // MN: non-functional, hoipefully temporaru
    using IAddressProvider::tadList, IAddressProvider::tadListIterator;

    virtual StatusCode preLoadAddresses( StoreID::type storeID, tadList& tlist ) override;

    virtual StatusCode updateAddress(StoreID::type storeID, SG::TransientAddress* tad,
                                     const EventContext& ctx) override;
    //@}


    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:
    /// Allow the factory class access to the constructor
    template <class TYPE> class SvcFactory;  /// forward declaration
    friend class SvcFactory<TagInfoMgr>;

    TagInfoMgr();
    TagInfoMgr(const TagInfoMgr&);
    TagInfoMgr &operator=(const TagInfoMgr&);
    StatusCode getRunNumber (unsigned int& runNumber);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:


    /// @name TagInfo management methods:
    //@{
    StatusCode fillTagInfo(const CondAttrListCollection* tagInfoCond);
    StatusCode fillMetaData(const CondAttrListCollection* tagInfoCond);
    //@}

    /// Extra tags/values pairs added in my jobOptions
    Gaudi::Property<std::map<std::string,std::string> >
        m_extraTagValuePairs { this, "ExtraTagValuePairs", {}, "key/value pairs to be added", "mapMergeNoReplace<T,T>" };

    /// Extra tags/values pairs added in via interface
    std::map<std::string,std::string> m_extraTagValuePairsViaInterface;

    /// Extra tags to be removed
    std::set<std::string>          m_tagsToBeRemoved;

    /// The event store
    ServiceHandle<StoreGateSvc>    m_storeGate { this, "StoreGateSvc", "StoreGateSvc" };

    /// The detector store
    ServiceHandle<StoreGateSvc>    m_detStore { this, "DetectorStore", "DetectorStore" };

    /// Access to IOVDbSvc interface - used to register callback
    ServiceHandle<IIOVDbSvc>       m_iovDbSvc { this, "IOVDbSvc", "IOVDbSvc" };

    /// Access to iov meta data tool
    PublicToolHandle<IIOVDbMetaDataTool> m_metaDataTool { this, "IOVDbMetaDataTool", "IOVDbMetaDataTool" };

    /// Flag to identify the first BeginRun incident
    bool                           m_isFirstBeginRun { true };

    /// conditionsRun from the first BeginRun incident (HLT)
    EventIDBase::number_type       m_conditionsRun { EventIDBase::UNDEFNUM };

    /// current run number from BeginRun incident
    EventIDBase::number_type       m_currentRun { EventIDBase::UNDEFNUM };

    /// IOVRange of last TagInfo added to the file meta data
    IOVRange                       m_lastIOVRange { IOVRange(IOVTime(), IOVTime()) };

    /// The Tags
    TagInfo                        m_tagInfo;

    /// List of listeners notified when the TagInfo changed
    std::set< Listener* >          m_listeners;

    /// mutex to protect internal data in MT
    mutable std::shared_mutex      m_mutex   ATLAS_THREAD_SAFE;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // EVENTINFOMGT_TAGINFOMGR_H
