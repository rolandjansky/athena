/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 
 ***************************************************************************/

//<doc><file>	$Id: ITk_Strip_RDO_Collection.h,v 1.9 2007-05-28 12:47:23 kgnanvo Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_ITK_STRIP_RDO_COLLECTION_H
# define INDETRAWDATA_ITK_STRIP_RDO_COLLECTION_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/SCT1_RawData.h"
#include "InDetRawData/SCT_TB03_RawData.h"
#include "InDetRawData/SCT_TB04_RawData.h"
#include "InDetRawData/SCT3_RawData.h"
#include "InDetRawData/ITk_Strip_RDORawData.h"
class MsgStream;

typedef InDetRawDataCollection< ITk_Strip_RDORawData >
	ITk_Strip_RDO_Collection; 

CLASS_DEF(ITk_Strip_RDO_Collection, 1081737352, 1)

/**Overload of << operator for MsgStream for debug output*/
MsgStream& operator << ( MsgStream& sl, const ITk_Strip_RDO_Collection& coll);

/**Overload of << operator for std::ostream for debug output*/ 
std::ostream& operator << ( std::ostream& sl, const ITk_Strip_RDO_Collection& coll);


typedef InDetRawDataCollection< SCT_TB03_RawData >
	SCT_RDO_TB03_Collection; 

CLASS_DEF(SCT_RDO_TB03_Collection, 1234089640, 1)

typedef InDetRawDataCollection< SCT_TB04_RawData >
	SCT_RDO_TB04_Collection; 

CLASS_DEF(SCT_RDO_TB04_Collection, 1278514121 , 1)

typedef InDetRawDataCollection< SCT3_RawData >
	SCT3_RDO_Collection; 

CLASS_DEF(SCT3_RDO_Collection, 1339040427 , 1)

// Class needed only for persistency - we add CLASS_DEF only to
// instantiate the template.
// typedef  DataVector<InDetRawDataCollection< SCT1_RawData > > SCT_RDO_vector; 

// CLASS_DEF( SCT_RDO_vector, 1241152074, 1 )

// typedef  DataVector<const InDetRawDataCollection< SCT1_RawData > > SCT_RDO_constvector; 

//CLASS_DEF( SCT_RDO_constvector, 1159798379, 1 )

#endif // INDETRAWDATA_ITK_STRIP_RDO_COLLECTION_H
