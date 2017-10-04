/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 Tile Calorimeter detector description package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TileDetDescrRegion.h,v 1.9 2007-11-06 15:20:19 baranov Exp $
//<version>	$Name: not supported by cvs2svn $

//  **********************************************
//  * Auto-created from schaffer's code.
//  * ~berd
//  **********************************************

#ifndef TILEDETDESCR_TILEDETDESCRREGION_H
#define TILEDETDESCR_TILEDETDESCRREGION_H

//<<<<<< INCLUDES                                                       >>>>>>

#include "Identifier/Identifiable.h"

#include "TileDetDescr/TileDetDescriptor.h"

//<<<<<< PUBLIC DEFINES                                                 >>>>>>
//<<<<<< PUBLIC CONSTANTS                                               >>>>>>
//<<<<<< PUBLIC TYPES                                                   >>>>>>
//<<<<<< PUBLIC VARIABLES                                               >>>>>>
//<<<<<< PUBLIC FUNCTIONS                                               >>>>>>
//<<<<<< CLASS DECTILEATIONS                                            >>>>>>

/**
    @section s1 Class TileDetDescrRegion
  
    This class provides an interface to detector description
    information for the regions (sub-detectors) of the  Tile calorimeter. 
    Each instance of an TileDetDescrRegion corresponds to the description for a SINGLE region.
  
    @section s2 Information available:
  
    Identifier: Each TileDetDescrRegion has an Identifier which
    uniquely identifies to which region it corresponds, i.e. which
    sub-detector and positive/negative half, (See classes Tile_ID, ... in TileDetDescr/TileID.h for
    more info.).
  
    Print: A general print method is available.
  
    @section s3 Object ownership:
  
    Clients are NOT responsible for deleting the objects connected
    to the pointers received for DetectorPosition or TileDetDescriptor
    objects.
*/
class TileDetDescrRegion : public Identifiable
{
public:
    
    typedef TileDetDescriptor		descriptor_type;

    TileDetDescrRegion(void);
    TileDetDescrRegion(const Identifier& id, 
		      const descriptor_type* descriptor);
    
// 		      const DetectorPosition* position);


    virtual Identifier		identify	() const;
    
    const descriptor_type* 	descriptor	() const;
    
//     const DetectorPosition* 	position	() const;

    void 			print		() const;

private:
    
    Identifier			m_id;
    
    const descriptor_type* 	m_descriptor;
    
//     const DetectorPosition* 	m_position;
    
};



//<<<<<< INLINE PUBLIC FUNCTIONS                                        >>>>>>
//<<<<<< INLINE MEMBER FUNCTIONS                                        >>>>>>

inline
TileDetDescrRegion::TileDetDescrRegion(void)
    :
    m_descriptor(0)
// ,
//     m_position(0)
{}

inline
TileDetDescrRegion::TileDetDescrRegion(const Identifier& id, 
	     const descriptor_type* descriptor)
// ,
// 	     const DetectorPosition* position) 
    :
    m_id(id),
    m_descriptor(descriptor)
// ,
//     m_position(position)
{}

inline const TileDetDescrRegion::descriptor_type* 	
TileDetDescrRegion::descriptor	() const
{
    return m_descriptor;
}

// inline const DetectorPosition* 	
// TileDetDescrRegion::position	() const
// {
//     return m_position;
// }



#endif // TILEDETDESCR_TILEDETDESCRREGION_H
