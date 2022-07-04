/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENAPOOLTEST_EVENTINFOWRITER_H
#define ATHENAPOOLTEST_EVENTINFOWRITER_H

/**
 * @file EventInfoWriter.h
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, writes extra tags.
 *
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */

/**
 * @class EventInfoWriter
 *
 * @brief Test Algorithm for checking TagInfoMgr and the storing of
 * geometry and conditions tags in EventInfo, writes extra tags.
 *
 */


//<<<<<< INCLUDES                                                       >>>>>>

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/ReadHandleKey.h"
#include "xAODEventInfo/EventInfo.h"

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class ITagInfoMgr;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class EventInfoWriter : public AthAlgorithm
{
public:

    /// Algorithm constructor
    EventInfoWriter(const std::string &name,ISvcLocator *pSvcLocator);

    /// Algorithm initialize at begin of job
    virtual StatusCode initialize() override;

    /// Algorithm execute once per event
    virtual StatusCode execute() override;

    /// Algorithm finalize at end of job
    virtual StatusCode finalize() override;
  
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
    StatusCode fillTagInfo    ();

    /// For testing only: add in dummy tags
    BooleanProperty m_createDummyTags;

    /// For testing only: remove input dummy tags
    BooleanProperty m_removeDummyTags;

    /// For testing only: add in dummy tags which may override
    /// existing tags
    BooleanProperty m_createDummyOverrideTags;

    /// Access to the TagInfoMgr
    ITagInfoMgr*    m_tagInfoMgr;

    /// SG key for Event Info
    SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{this, "EventInfo", "EventInfo", "EventInfo name"};

    bool m_setDummyTags = false;

};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_EVENTINFOWRITER_H
