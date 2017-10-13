/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: TRT_RDO_Collection.h,v 1.5 2006-09-27 06:31:46 andr Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_TRT_RDO_COLLECTION_H
# define INDETRAWDATA_TRT_RDO_COLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/TRT_LoLumRawData.h"
#include "InDetRawData/TRT_TB04_RawData.h"
class MsgStream;


typedef InDetRawDataCollection< TRT_RDORawData > TRT_RDO_Collection;

//Using 2539 historically defined in TRT_RDO_Container.h
CLASS_DEF( InDetRawDataCollection< TRT_RDORawData > , 2539 , 1 )

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const TRT_RDO_Collection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const TRT_RDO_Collection& coll);


// FIXME: this should go, as the LoLum collection did...
typedef InDetRawDataCollection< TRT_TB04_RawData >
	TRT_RDO_TB04_Collection; 

CLASS_DEF(TRT_RDO_TB04_Collection, 1172211349, 1)


#endif // INDETRAWDATA_TRT_RDO_COLLECTION_H
