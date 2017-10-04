/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: SCT_RDO_Container.h,v 1.6 2008-01-05 18:53:00 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_SCT_RDO_CONTAINER_H
# define INDETRAWDATA_SCT_RDO_CONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/SCT_RDO_Collection.h" // Needed to pick up CLID.

typedef InDetRawDataContainer<InDetRawDataCollection<SCT_RDORawData> > 
	SCT_RDO_Container; 

CLASS_DEF(SCT_RDO_Container,2541,1)


#endif // INDETRAWDATA_SCT_RDO_CONTAINER_H
