/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/


#ifndef SRC_TESTLARIDS_H
# define SRC_TESTLARIDS_H

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
class LArEM_ID;
class LArHEC_ID;
class LArFCAL_ID;
class LArMiniFCAL_ID;
class LArOnlineID;          // <-- added
class IMessageSvc;
class IChronoStatSvc;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 ** This class groups together the Identifier dictionary tests for the
 ** InnerDetector.
 **/

class TestLArIds 
{
public:
    TestLArIds();
    TestLArIds(const IdDictManager* idDictMgr, 
               const LArEM_ID* emHelper,
               const LArHEC_ID* hecHelper,
               const LArFCAL_ID* fcalHelper,
               const LArMiniFCAL_ID* miniFcalHelper,
               const LArOnlineID* laronlineHelper,      
               IMessageSvc*     msgSvc,
               IChronoStatSvc*  chronoStSvc);
    
    StatusCode  testEmIds               (void) const;
    StatusCode  testEmNeighbors         (void) const;
    StatusCode  testHecIds              (void) const;
    StatusCode  testHecNeighbors        (void) const;
    StatusCode  testFcalIds             (void) const;
    StatusCode  testMiniFcalIds         (void) const;
    StatusCode  testFcalNeighbors       (void) const;
    StatusCode  testLaronlineIds        (void) const;


private:
    const std::string& name    (void) const;
    std::string                         m_name;
    const IdDictManager*                m_idDictMgr;
    const LArEM_ID*                     m_emHelper;
    const LArHEC_ID*                    m_hecHelper;
    const LArFCAL_ID*                   m_fcalHelper;
    const LArMiniFCAL_ID*               m_miniFcalHelper;
    const LArOnlineID*                  m_laronlineHelper;
    IMessageSvc*                        m_msgSvc;
    IChronoStatSvc*                     m_chronoStSvc; 
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_TESTLARIDS_H

