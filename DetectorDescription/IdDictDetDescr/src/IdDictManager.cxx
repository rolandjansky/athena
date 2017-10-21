/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDictDetDescr package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IdDictManager.cxx,v 1.3 2003-09-22 07:01:22 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDictDetDescr/IdDictManager.h"
#include "Identifier/IdHelper.h"
#include "IdDict/IdDictDefs.h"


//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

IdDictManager::IdDictManager()
    :
    m_mgr(0)
{}

IdDictManager::IdDictManager(const IdDictMgr& mgr)
    :
    m_mgr(&mgr)
{}


IdDictManager::~IdDictManager()
{}

const IdDictMgr*
IdDictManager::manager			(void) const
{
    return (m_mgr);
}


int
IdDictManager::initializeHelper	(IdHelper& helper) const
{
    if (m_mgr) return (helper.initialize_from_dictionary(*m_mgr));
    return (1); // otherwise error
}


