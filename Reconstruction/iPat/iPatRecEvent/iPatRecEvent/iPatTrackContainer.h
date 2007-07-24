/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatTrackContainer contains the Track data objects
 found by the pattern recognition.
 ---------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatTrackContainer.h,v 1.5 2003-05-27 13:36:58 pop Exp $

#ifndef IPATRECEVENT_IPATTRACKCONTAINER_H
# define IPATRECEVENT_IPATTRACKCONTAINER_H

//<<<<<< INCLUDES                                                       >>>>>>

#ifndef CLIDSVC_CLASSDEF_H
# include "CLIDSvc/CLASS_DEF.h"
#endif
#include "DataModel/DataVector.h"
#include "iPatTrack/Track.h"

class iPatTrackContainer:public DataVector<Track>
{

public:
    // constructor/destructor
    iPatTrackContainer();
    ~iPatTrackContainer();

    // print track details
    void	        	print (void) const;
};

CLASS_DEF(iPatTrackContainer, 3151, 1)

#endif // IPATRECEVENT_IPATTRACKCONTAINER_H
