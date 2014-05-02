/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module.cm

//## begin module.cp preserve=no
//## end module.cp

//## Module: VDetectorNode; Pseudo Package body
//## Subsystem: GeoModelKernel
//## Source file: L:\GEO\GeoModelKernel\VDetectorNode.cxx


//## begin module.additionalIncludes preserve=no
//## end module.additionalIncludes

//## begin module.includes preserve=yes
//## end module.includes

// VDetectorNode
#include "GeoModelKernel/VDetectorNode.h"

//## begin module.additionalDeclarations preserve=yes
//## end module.additionalDeclarations


// Class VDetectorNode 

VDetectorNode::VDetectorNode()
  //## begin VDetectorNode::VDetectorNode%.hasinit preserve=no
  //## end VDetectorNode::VDetectorNode%.hasinit
  //## begin VDetectorNode::VDetectorNode%.initialization preserve=yes
  //## end VDetectorNode::VDetectorNode%.initialization
{
  //## begin VDetectorNode::VDetectorNode%.body preserve=yes
  //## end VDetectorNode::VDetectorNode%.body
}


VDetectorNode::~VDetectorNode()
{
  //## begin VDetectorNode::~VDetectorNode%.body preserve=yes
  //## end VDetectorNode::~VDetectorNode%.body
}



//## Other Operations (implementation)
const GeoVDetectorManager * VDetectorNode::getDetectorManager() const
{
  //## begin VDetectorNode::getDetectorManager%1043616483.body preserve=yes
	return this;
  //## end VDetectorNode::getDetectorManager%1043616483.body
}


// Additional Declarations
//## begin VDetectorNode.declarations preserve=yes
//## end VDetectorNode.declarations




