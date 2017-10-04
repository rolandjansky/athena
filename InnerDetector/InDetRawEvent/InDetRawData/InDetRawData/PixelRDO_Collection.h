/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: PixelRDO_Collection.h,v 1.7 2007-05-22 16:14:42 hirsch Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_PIXELRDO_COLLECTION_H
# define INDETRAWDATA_PIXELRDO_COLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/Pixel1RawData.h"
#include "InDetRawData/PixelTB04RawData.h"
class MsgStream;

typedef InDetRawDataCollection< PixelRDORawData > PixelRDO_Collection; 
CLASS_DEF(PixelRDO_Collection, 2534, 1)

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const PixelRDO_Collection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const PixelRDO_Collection& coll);


typedef InDetRawDataCollection< PixelTB04RawData > PixelRDO_TB04_Collection; 
CLASS_DEF(PixelRDO_TB04_Collection, 1264657413, 1)

typedef InDetRawDataCollection< Pixel1RawData >	PixelRDO_1_Collection; 
CLASS_DEF(PixelRDO_1_Collection, 1263352945, 1)

// Class needed only for persistency - we add CLASS_DEF only to
// instantiate the template.
// typedef  DataVector<InDetRawDataCollection< Pixel1RawData > > PixelRDO_vector; 

// CLASS_DEF( PixelRDO_vector, 1324952031 , 1 )

// typedef  DataVector<const InDetRawDataCollection< Pixel1RawData > > PixelRDO_constvector; 

// CLASS_DEF( PixelRDO_constvector, 1152271469 , 1 )

#endif // INDETRAWDATA_PIXELRDO_COLLECTION_H
