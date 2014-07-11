/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**


@mainpage

The package includes converters for the following managers: CaloDetDescrManager, CaloTTDescrManager.

1. CaloMgrDetDescrCnv. This is a converter which builds Calo DD elements and descriptors and
ultimately stores them into CaloDetDescrManager. The LAr Calo DD elements and descriptors
are built using LArReadoutGeometry objects and the Tile Calo DD elements and descriptors are
simply copied from TileDetDescrManager to CaloDetDescrManager, because these objects are created
by TileGeoModel.

2. CaloTTMgrDetDescrCnv. This class derives from  DetDescrConverter which is a converter of the DetDescrCnvSvc. 
This converter creates a manager object and adds descriptors and detector elements to the manager. This objects are either created
or accessed from the detector store.

--------------------------------
  REQUIREMENTS 
--------------------------------

@include requirements
@htmlinclude used_packages.html 

*/
