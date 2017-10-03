/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileDetDescrRegion.cxx,v 1.4 2003-10-27 16:12:25 solodkov Exp $
//<version>	$Name: not supported by cvs2svn $

//  **********************************************
//  * Auto-created from schaffer's code.
//  * ~berd
//  **********************************************

//<<<<<< INCLUDES                                                       >>>>>>

#include "TileDetDescr/TileDetDescrRegion.h"

#include "AtlasDetDescr/AtlasDetectorID.h"

#include <iostream>
#include <iomanip>

//<<<<<< PRIVATE DEFINES                                                >>>>>>
//<<<<<< PRIVATE CONSTANTS                                              >>>>>>
//<<<<<< PRIVATE TYPES                                                  >>>>>>
//<<<<<< PRIVATE VARIABLE DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC VARIABLE DEFINITIONS                                    >>>>>>
//<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>
//<<<<<< PRIVATE FUNCTION DEFINITIONS                                   >>>>>>
//<<<<<< PUBLIC FUNCTION DEFINITIONS                                    >>>>>>
//<<<<<< MEMBER FUNCTION DEFINITIONS                                    >>>>>>

Identifier
TileDetDescrRegion::identify() const
{
    return m_id;
}

void 			
TileDetDescrRegion::print		() const
{
    
    AtlasDetectorID id;
    std::cout << std::endl << " TileDetDescrRegion print: " 
	      << std::endl << std::endl;
    
    id.print(m_id);
    m_descriptor->print();
}
