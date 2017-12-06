/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
 InDetRawData package
 -----------------------------------------
 ***************************************************************************/

//<doc><file>	$Id: PixelRDO_Container.h,v 1.5 2008-01-05 18:52:59 ssnyder Exp $
//<version>	$Name: not supported by cvs2svn $

#ifndef INDETRAWDATA_PIXELRDO_CONTAINER_H
# define INDETRAWDATA_PIXELRDO_CONTAINER_H

#include "CLIDSvc/CLASS_DEF.h"
#include "InDetRawData/InDetRawDataContainer.h"
#include "InDetRawData/InDetRawDataCollection.h"
#include "InDetRawData/PixelRDORawData.h"
#include "InDetRawData/PixelRDO_Collection.h" // Needed to pick up CLID.
#include "AthLinks/DeclareIndexingPolicy.h"
#include "EventContainers/IIdentifiableCont.h"

typedef InDetRawDataContainer<InDetRawDataCollection<PixelRDORawData> > 
PixelRDO_Container; 

typedef EventContainers::IdentifiableCache< InDetRawDataCollection<PixelRDORawData> > PixelRDO_Cache;

CLASS_DEF(PixelRDO_Container,2540,1)
CONTAINER_IS_IDENTCONT(PixelRDO_Container)

CLASS_DEF( PixelRDO_Cache , 24943700 , 1 )

typedef EventContainers::IIdentifiableCont<InDetRawDataCollection<PixelRDORawData>> IPixelRDO_Container;//Interface only, do not place in storegate

#endif // INDETRAWDATA_PIXELRDO_CONTAINER_H
