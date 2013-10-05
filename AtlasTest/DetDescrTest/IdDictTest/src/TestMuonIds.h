/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TestMuonIds.h,v 1.4 2005-04-28 17:07:13 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef SRC_TESTMUONIDS_H
# define SRC_TESTMUONIDS_H

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
class StoreGateSvc;
class IMessageSvc;
class IChronoStatSvc;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 ** This class groups together the Identifier dictionary tests for the
 ** InnerDetector.
 **/

class TestMuonIds 
{
public:
    TestMuonIds();
    TestMuonIds(const IdDictManager* idDictMgr, 
                StoreGateSvc*   detStore,
		IMessageSvc* 	msgSvc,
		IChronoStatSvc*  chronoStSvc);
    
    StatusCode 	testMDTIds		(void) const;
    StatusCode 	testMDTDetElem		(void) const;
    StatusCode 	testMDTNeighbors	(void) const;

    StatusCode 	testCSCIds		(void) const;
    StatusCode 	testCSCDetElem		(void) const;
    StatusCode 	testCSCNeighbors	(void) const;

    StatusCode 	testRPCIds		(void) const;
    StatusCode 	testRPCDetElem		(void) const;
    StatusCode 	testRPCNeighbors	(void) const;

    StatusCode 	testTGCIds		(void) const;
    StatusCode 	testTGCDetElem		(void) const;
    StatusCode 	testTGCNeighbors	(void) const;

    const std::string& 	name	(void) const;

    std::string				m_name;
    const IdDictManager* 		m_idDictMgr;
    StoreGateSvc*                       m_detStore;
    IMessageSvc* 			m_msgSvc;
    IChronoStatSvc*   			m_chronoStSvc; 
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_TESTMUONIDS_H

