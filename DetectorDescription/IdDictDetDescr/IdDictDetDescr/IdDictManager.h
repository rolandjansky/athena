/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 IdDictDetDescr package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IdDictManager.h,v 1.4 2003-09-22 07:01:22 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef IDDICTDETDESCR_IDDICTMANAGER_H
# define IDDICTDETDESCR_IDDICTMANAGER_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "IdDict/IdDictDefs.h"
#include "CLIDSvc/CLASS_DEF.h"

//<<<<<< PUBLIC TYPES                                                   >>>>>>

class IdHelper;
class IdDictDictionary;
class IdDictMgr;

//<<<<<< CLASS DECLARATIONS                                             >>>>>>

/**
 *  IdDictManager is the interface to identifier dictionaries. This
 *  class inherits most of its functionality from IdDictMgr and adds
 *  the capabilities to be an object in the detector store.
 */

class IdDictManager
{
public:
    IdDictManager();
    IdDictManager(const IdDictMgr& mgr);
    ~IdDictManager();

    //IdDictDictionary* 	find_dictionary (const std::string& name) const;  

    const IdDictMgr*	manager			(void) const;

    /// Return value: 0 OK, >0 error
    int                 initializeHelper        (IdHelper& helper) const;

private:
    const IdDictMgr* 	m_mgr;
};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

//using the macros below we can assign an identifier (and a version)
//This is required and checked at compile time when you try to record/retrieve
CLASS_DEF(IdDictManager, 2411, 1)

#endif // IDDICTDETDESCR_IDDICTMANAGER_H
