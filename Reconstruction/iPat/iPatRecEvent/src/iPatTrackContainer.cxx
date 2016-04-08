/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 iPatTrackContainer contains the Track data objects
 found by the pattern recognition.
 ---------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: iPatTrackContainer.cxx,v 1.2 2002-08-30 12:39:32 pop Exp $


#include <iomanip>
#include <iostream>
#include "iPatRecEvent/iPatTrackContainer.h"


iPatTrackContainer::iPatTrackContainer()
    :	DataVector<Track>	()
{}

iPatTrackContainer::~iPatTrackContainer()
{
//      std::cout << " iPatTrackContainer::destructor  tracks->size() "
//  	      << this->size() << std::endl;
}

//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

void
iPatTrackContainer::print (void) const
{
    for (DataVector<Track>::const_iterator t = this->begin();
	 t != this->end();
	 ++t)
    {
	(**t).print_parameters();
    }
}
