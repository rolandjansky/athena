/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDD34600238.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDD34600238.cm

//## begin module%3CDD34600238.cp preserve=no
//## end module%3CDD34600238.cp

//## Module: GeoGraphNode%3CDD34600238; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoGraphNode.cxx

//## begin module%3CDD34600238.additionalIncludes preserve=no
//## end module%3CDD34600238.additionalIncludes

//## begin module%3CDD34600238.includes preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3CDD34600238.includes

// GeoGraphNode
#include "GeoModelKernel/GeoGraphNode.h"
//## begin module%3CDD34600238.additionalDeclarations preserve=yes
//## end module%3CDD34600238.additionalDeclarations


// Class GeoGraphNode 

GeoGraphNode::GeoGraphNode ()
  //## begin GeoGraphNode::GeoGraphNode%3CDD42320389.hasinit preserve=no
  //## end GeoGraphNode::GeoGraphNode%3CDD42320389.hasinit
  //## begin GeoGraphNode::GeoGraphNode%3CDD42320389.initialization preserve=yes
  //## end GeoGraphNode::GeoGraphNode%3CDD42320389.initialization
{
  //## begin GeoGraphNode::GeoGraphNode%3CDD42320389.body preserve=yes
  //## end GeoGraphNode::GeoGraphNode%3CDD42320389.body
}


GeoGraphNode::~GeoGraphNode()
{
  //## begin GeoGraphNode::~GeoGraphNode%3CDD34600238_dest.body preserve=yes
  //## end GeoGraphNode::~GeoGraphNode%3CDD34600238_dest.body
}



//## Other Operations (implementation)
void GeoGraphNode::exec (GeoNodeAction *action) const
{
  //## begin GeoGraphNode::exec%3CDF54C40260.body preserve=yes
  action->handleNode (this);
  //## end GeoGraphNode::exec%3CDF54C40260.body
}

void GeoGraphNode::dockTo (GeoVPhysVol* )
{
  //## begin GeoGraphNode::dockTo%3CE3D83D025E.body preserve=yes
  //## end GeoGraphNode::dockTo%3CE3D83D025E.body
}

// Additional Declarations
  //## begin GeoGraphNode%3CDD34600238.declarations preserve=yes
  //## end GeoGraphNode%3CDD34600238.declarations

//## begin module%3CDD34600238.epilog preserve=yes
//## end module%3CDD34600238.epilog
