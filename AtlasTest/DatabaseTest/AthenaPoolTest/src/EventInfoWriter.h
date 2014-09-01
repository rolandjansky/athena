/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SRC_EVENTINFOWRITER_H
# define SRC_EVENTINFOWRITER_H

/**
 * @file EventInfoWriter.h
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, writes extra tags.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 *
 * $Id: EventInfoWriter.h,v 1.6 2009-04-29 07:49:45 schaffer Exp $
 *
 */

/**
 * @class EventInfoWriter
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, writes extra tags.
 *
 */


//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaKernel/IOVSvcDefs.h"
#include "AthenaBaseComps/AthAlgorithm.h"

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class StoreGateSvc;
class ITagInfoMgr;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class EventInfoWriter : public AthAlgorithm
{
public:

    /// Algorithm constructor
    EventInfoWriter(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize();

    /// Algorithm execute once per event
    virtual StatusCode execute(); 

    /// Algorithm finalize at end of job
    virtual StatusCode finalize();
  
    /// Callback method for TagInfo access
    StatusCode         checkTagInfo(IOVSVC_CALLBACK_ARGS);

    ///////////////////////////////////////////////////////////////////
    // Private methods:
    ///////////////////////////////////////////////////////////////////
private:

    /// Avoid use of default constructor
    EventInfoWriter();

    /// Avoid use of copy constructor
    EventInfoWriter(const EventInfoWriter&);

    /// Avoid use of copy operator
    EventInfoWriter &operator=(const EventInfoWriter&);
  
    ///////////////////////////////////////////////////////////////////
    // Private data:
    ///////////////////////////////////////////////////////////////////
private:

    /// Add in tags to TagInfoMgr
    StatusCode fillTagInfo    () const;

    /// For testing only: add in dummy tags
    BooleanProperty m_createDummyTags;

    /// For testing only: remove input dummy tags
    BooleanProperty m_removeDummyTags;

    /// For testing only: add in dummy tags which may override
    /// existing tags
    BooleanProperty m_createDummyOverrideTags;

    /// Access to the TagInfoMgr
    ITagInfoMgr*    m_tagInfoMgr;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_EVENTINFOWRITER_H
