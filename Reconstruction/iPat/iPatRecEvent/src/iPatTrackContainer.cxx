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

//<<<<<< INCLUDES                                                       >>>>>>

#ifdef HAVE_NEW_IOSTREAMS
  #include <iomanip>
  #include <iostream>
#else
  #include <iomanip.h>
  #include <iostream.h>
#endif
#include "iPatRecEvent/iPatTrackContainer.h"

//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

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
