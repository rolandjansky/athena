/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 global functions and stuff ...
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: GlobalUtilities.h,v 1.1.1.1 2009-02-23 21:17:26 dwright Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_GLOBALUTILITIES_H
# define MUONGEOMODEL_GLOBALUTILITIES_H

//<<<<<< INCLUDES                                                       >>>>>>
#include <string>

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>


namespace MuonGM 
{
    std::string buildString(int i, int ncha);
    int strtoint(std::string str, unsigned int istart, unsigned int length);
    int stationPhiTGC(std::string stName, int fi, int zi_input, std::string geometry_version);
    
    
}

#endif // MUONGEOMODEL_GLOBALUTILITIES_H
