/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOMGT_ITAGINFOMGR_H
# define EVENTINFOMGT_ITAGINFOMGR_H
/**
 * @file TagInfoMgr.h
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a private TagInfo object 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "CxxUtils/checker_macros.h"
#include "GaudiKernel/Service.h"

class MsgStream;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class ITagInfoMgr
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object with current tag values.
 *
 *  The tags to fill the TagInfo object may come from a variety of
 *  sources: i.e. the tags specified by the GeoModelSvc and IOVDbSvc,
 *  or the tags stored in the incoming EventInfo object, or the
 *  TagInfo object itself may be in the IOV DB. The exact combination
 *  is controlled by job options.
 *
 *  This interface also provides a 'input' method to allow client to
 *  add in their own tags:
 *     
 *  virtual StatusCode addTag(const std::string& tagName, 
 *                            const std::string& tagValue) = 0;
 *
 *  The full set of tags in TagInfo are copied each event into
 *  EventInfo (specifically EventType) to be written out in the event
 *  stream. Currently, the tags are NOT written to the IOV DB via the
 *  TagInfoMgr. 
 *
 *  Some clients need to know when detector description tags change
 *  so that they may update their descriptions, such as the geometry
 *  description. The TagInfo object will change in this case. So
 *  clients that need to know when tags change should register themselves
 *  by calling ITagInfoMgr::addListener(listener*) method.
 *  These clients need to implement the ITagInfoMgr::Listener interface and
 *  ITagInfoMgr::Listener::tagInfoUpdated() method which will be called
 *  when the TagInfo changes.
 *
 */
class ITagInfoMgr : virtual public IInterface
{
public:
    /// @name typedefs: (a copy from TagInfo, to avoid dependencies)
    //@{
    typedef     std::pair<std::string, std::string>  NameTagPair;
    typedef     std::vector<NameTagPair>             NameTagPairVec;
    //@}


    /// Listener interface class that client who want to be notified about TagInfo
    /// update should implement (and then register themselves calliing addListener())
    class Listener {
    public:
       virtual void tagInfoUpdated() = 0;
    };

    /// Add a Listener to the notification list for TagInfo changes
    virtual void              addListener(Listener* listener) = 0;

    /// Remove a Listener from the notification list for TagInfo changes
    virtual void              removeListener(Listener* listener) = 0;
      

    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    /// Method to allow clients to add in tags as: tag name, tag value
    virtual StatusCode        addTag(const std::string& tagName, 
				     const std::string& tagValue) = 0;

    /// Method to allow clients to remove a tag which may have come in
    /// on the input
    virtual StatusCode        removeTagFromInput(const std::string& tagName) = 0;

    /// Find tag by its name, return by value - empty string if not found
    virtual std::string       findTag ATLAS_CHECK_THREAD_SAFETY (const std::string & name) const = 0;

    /// Find INPUT tag by its name, return by value - empty string if not found
    virtual std::string       findInputTag ATLAS_CHECK_THREAD_SAFETY (const std::string& name) const = 0;

    /// Return a vector with all current input tags
    virtual NameTagPairVec    getInputTags() const = 0;

    /// Dump the content of the current TagInfo to std::string for debug
    virtual std::string       dumpTagInfoToStr() const = 0;

    /// Printout method
    virtual void              printTags(MsgStream& log) const = 0;
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline const InterfaceID& 
ITagInfoMgr::interfaceID() 
{
    static const InterfaceID IID_ITagInfoMgr("ITagInfoMgr", 1 , 0); 
    return IID_ITagInfoMgr; 
}

#endif // EVENTINFOMGT_ITAGINFOMGR_H
