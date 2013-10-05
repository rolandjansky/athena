/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDict Test package
 -----------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: TestCaloIds.h,v 1.9 2004-02-16 17:34:53 fledroit Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef SRC_TESTCALOIDS_H
# define SRC_TESTCALOIDS_H

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
class CaloIdManager;
class CaloLVL1_ID;
class CaloDM_ID;
class CaloCell_ID;
class IMessageSvc;
class IChronoStatSvc;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 ** This class groups together the Identifier dictionary tests for the
 ** Calorimeter trigger tower.
 **/

class TestCaloIds 
{
public:
    TestCaloIds();
    TestCaloIds(const IdDictManager* idDictMgr, 
		const CaloLVL1_ID* lvl1Helper,
		const CaloDM_ID* dmHelper,
		const CaloCell_ID* cellHelper,
		IMessageSvc* 	msgSvc,
		IChronoStatSvc*  chronoStSvc);
    
    StatusCode  testLArDM_Ids           (void) const;
    StatusCode  testTileDM_Ids          (void) const;
    StatusCode  testCaloCell_Ids        (void) const;
    StatusCode  testLvl1Ids             (void) const;
    StatusCode  testLvl1Neighbors       (void) const;

private:

    const std::string& 	name	(void) const;

    std::string				m_name;
    const IdDictManager* 		m_idDictMgr;
    const CaloLVL1_ID* 	                m_lvl1Helper;
    const CaloDM_ID* 	                m_dmHelper;
    const CaloCell_ID* 	                m_cellHelper;
    IMessageSvc* 			m_msgSvc;
    IChronoStatSvc*   			m_chronoStSvc; 
};


//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // SRC_TESTCALOIDS_H
