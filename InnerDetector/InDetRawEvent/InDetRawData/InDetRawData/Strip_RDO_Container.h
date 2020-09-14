/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 
 ***************************************************************************/

//<doc><file>	$Id: Strip_RDO_Container.h,v 1.6 2008-01-05 18:53:00 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_STRIP_RDO_CONTAINER_H
# define INDETRAWDATA_STRIP_RDO_CONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/Strip_RDORawData.h"
#include "InDetRawData/Strip_RDO_Collection.h" // Needed to pick up CLID.

typedef InDetRawDataContainer<InDetRawDataCollection<Strip_RDORawData> > 
	Strip_RDO_Container; 

CLASS_DEF(Strip_RDO_Container,286325,1)


#endif // INDETRAWDATA_STRIP_RDO_CONTAINER_H
