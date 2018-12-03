/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 stiffening aluminum bars holding together RPC and DED components
 ----------------------------------------------------------------
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
    :Technology(s), alFlangeThickness(0.), alHorFlangeLength(0.),
     alVerFlangeLength(0.), alVerProfileThickness(0.), alHorProfileThickness(0.),
     largeVerClearance(0.), smallVerClearance(0.), HorClearance(0.), xAMDB0(0.),
     yAMDB0(0.), zAMDB0(0.), bottomsizewrtAMDB0(0.), topsizewrtAMDB0(0.),
     alThickness(0.), length(0.), sup3Square(0.), sup3VerticalThickness(0.),
     sup3CentralLength(0.), sup3OuterLength(0.)
{
    totalThickness = 0.;
}
} // namespace MuonGM
