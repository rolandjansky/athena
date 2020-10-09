/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 
 ***************************************************************************/

//<doc><file>	$Id: ITk_Strip_RDO_Container.h,v 1.6 2008-01-05 18:53:00 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_ITK_STRIP_RDO_CONTAINER_H
# define INDETRAWDATA_ITK_STRIP_RDO_CONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/ITk_Strip_RDORawData.h"
#include "InDetRawData/ITk_Strip_RDO_Collection.h" // Needed to pick up CLID.

typedef InDetRawDataContainer<InDetRawDataCollection<ITk_Strip_RDORawData> > 
	ITk_Strip_RDO_Container; 

CLASS_DEF(ITk_Strip_RDO_Container,1282214950,1)


#endif // INDETRAWDATA_ITK_STRIP_RDO_CONTAINER_H
