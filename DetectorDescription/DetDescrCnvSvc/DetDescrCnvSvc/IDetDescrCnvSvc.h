/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Detector description conversion service package
 -----------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: IDetDescrCnvSvc.h,v 1.1 2002-07-10 13:44:41 schaffer Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef DETDESCRCNVSVC_IDETDESCRCNVSVC_H
# define DETDESCRCNVSVC_IDETDESCRCNVSVC_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/StatusCode.h"
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>

static const InterfaceID IID_IDetDescrCnvSvc(340,0,0) ;

//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

class IDetDescrCnvSvc : virtual public IConversionSvc
{

public:
    
    virtual StatusCode addToDetStore(const CLID& clid, std::string& name) = 0;

};

//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

#endif // DETDESCRCNVSVC_IDETDESCRCNVSVC_H

