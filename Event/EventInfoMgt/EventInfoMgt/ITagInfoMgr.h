/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef EVENTINFOMGT_ITAGINFOMGR_H
# define EVENTINFOMGT_ITAGINFOMGR_H
/**
 * @file TagInfoMgr.h
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object in the Detector
 *  Store with current tag values. 
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: ITagInfoMgr.h,v 1.4 2009-04-29 07:47:39 schaffer Exp $
 */

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IOVSvcDefs.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "AthenaKernel/IAddressProvider.h"
#include "AthenaKernel/IOVSvcDefs.h"


//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;
class IGeoModelSvc;
class TagInfo;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 * @class ITagInfoMgr
 *
 * @brief This is a Athena service which manages detector description
 *  tag information. It maintains a TagInfo object in the Detector
 *  Store with current tag values. 
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
 *  clients that need to know when tags change should set up a
 *  callback to the TagInfo object. This is done with regFcn of the
 *  StoreGate service (for the DetectorStore). The method 
 *
 *  virtual std::string& tagInfoKey() = 0;
 *
 *  provides access to the SG key of TagInfo which is needed to
 *  register a data handle for the callback.
 *
 */
class ITagInfoMgr : virtual public IInterface
{
public:

    /// Retrieve interface ID
    static const InterfaceID& interfaceID();

    /// Method to allow clients to add in tags as: tag name, tag value
    virtual StatusCode        addTag(const std::string& tagName, 
				     const std::string& tagValue) = 0;

    /// Method to allow clients to remove a tag which may have come in
    /// on the input
    virtual StatusCode        removeTagFromInput(const std::string& tagName) = 0;

    /// Method to allow clients to access the TagInfo object key.
    virtual std::string&      tagInfoKey() = 0;

    /// callback from IOVSvc - only used as test of callback
    virtual StatusCode        checkTagInfo(IOVSVC_CALLBACK_ARGS) = 0;

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
