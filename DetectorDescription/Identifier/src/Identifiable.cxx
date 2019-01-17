/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Identifier Package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: Identifiable.cxx,v 1.4 2002-09-20 11:40:51 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifiable.h"
#include "Identifier/IdentifierHash.h"
#include "Identifier/IdHelper.h"

// default implementation
IdentifierHash	Identifiable::identifyHash() const
{
    IdentifierHash result;
    return (result);
}

// default implementation
const IdHelper* 
Identifiable::getHelper() const
{
    return (0);
}

