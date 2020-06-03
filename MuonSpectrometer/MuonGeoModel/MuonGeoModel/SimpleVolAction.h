/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 My GeoVolumeAction: crosses a PhysVol to see the shape and materials
 --------------------------------------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SimpleVolAction.h,v 1.3 2006-04-10 17:12:17 stefspa Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef MUONGEOMODEL_SIMPLEVOLACTION_H
# define MUONGEOMODEL_SIMPLEVOLACTION_H

//<<<<<< INCLUDES                                                       >>>>>>
#include "GeoModelKernel/GeoVolumeAction.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECLARATIONS                                             >>>>>>

namespace MuonGM {

class SimpleVolAction :public GeoVolumeAction
{
public:
    //constructor
    SimpleVolAction ();

    //desctructor
    ~SimpleVolAction ();

    //Action routine
    void handleVPhysVol (const GeoVPhysVol *);
    
//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>
};

} // namespace MuonGM

#endif // MUONGEOMODEL_SIMPLEVOLACTION_H
