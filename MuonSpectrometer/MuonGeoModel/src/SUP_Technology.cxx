/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 stiffening aluminum bars holding together RPC and DED components
 ----------------------------------------------------------------
 Copyright (C) 2003 by ATLAS Collaboration
 ***************************************************************************/

//<doc><file>	$Id: SUP_Technology.cxx,v 1.1 2008-07-31 10:57:55 dquarrie Exp $
//<version>	$Name: not supported by cvs2svn $

//<<<<<< INCLUDES                                                       >>>>>>

#include "MuonGeoModel/SUP_Technology.h"

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

namespace MuonGM {

SUP::SUP(std::string s)
    :Technology(s)
{
    totalThickness = 0.;
}
} // namespace MuonGM
