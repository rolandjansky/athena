/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TestInDetIds.h,v 1.7 2006-04-27 11:27:54 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef SRC_TESTINDETIDS_H
# define SRC_TESTINDETIDS_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/StatusCode.h"
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

class Identifier;
class ExpandedIdentifier;
class Range;
class MultiRange;
class IdDictManager;
class SiliconID;
class IMessageSvc;
class IChronoStatSvc;
class PixelID;
class SCTID;
class TRT_ID;
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 ** This class groups together the Identifier dictionary tests for the
 ** InnerDetector.
 **/

class TestInDetIds 
{
public:
    TestInDetIds();
    TestInDetIds(const IdDictManager* idDictMgr, 
		 const PixelID*       pixIDHelper,
		 const SCT_ID*        sctIDHelper,
		 const TRT_ID*        trtIDHelper, 
                 const SiliconID*     siliconID,
                 IMessageSvc*         msgSvc,
                 IChronoStatSvc*      chronoStSvc);
    
    StatusCode  testPixelIds            (void) const;
    StatusCode  testPixelDetElem        (void) const;
    StatusCode  testSCTIds              (void) const;
    StatusCode  testSCTDetElem          (void) const;
    StatusCode  testSCTNeighbors        (void) const;
    StatusCode  testSiliconIds          (void) const;
    StatusCode  testTRTDetElem          (void) const;
    StatusCode  testTRTDetElemSL        (void) const;
    StatusCode  testTRTStraw            (void) const;
    StatusCode  testMinMax              (void) const;

private:
    // Create expanded id from Identifier - temporary method
    int                 get_expanded_id (const Identifier& old_id,
                                         ExpandedIdentifier& new_id) const;

    // Create expanded id from Identifier - temporary method
    int                 get_id          (const ExpandedIdentifier& old_id,
                                         Identifier& new_id) const;

    const std::string&  name    (void) const;

    std::string                          m_name;
    const IdDictManager*                 m_idDictMgr;
    const PixelID*                       m_pixIDHelper;
    const SCT_ID*                        m_sctIDHelper;
    const TRT_ID*                        m_trtIDHelper; 
    const SiliconID*                     m_siliconID;
    IMessageSvc*                         m_msgSvc;
    IChronoStatSvc*                      m_chronoStSvc; 
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_TESTINDETIDS_H

