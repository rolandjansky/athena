/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CDF51210227.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CDF51210227.cm

//## begin module%3CDF51210227.cp preserve=no
//## end module%3CDF51210227.cp

//## Module: GeoVolumeAction%3CDF51210227; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoVolumeAction.cxx

//## begin module%3CDF51210227.additionalIncludes preserve=no
//## end module%3CDF51210227.additionalIncludes

//## begin module%3CDF51210227.includes preserve=yes
//## end module%3CDF51210227.includes

// GeoVolumeAction
#include "GeoModelKernel/GeoVolumeAction.h"
//## begin module%3CDF51210227.additionalDeclarations preserve=yes
//## end module%3CDF51210227.additionalDeclarations


// Class GeoVolumeAction 

GeoVolumeAction::GeoVolumeAction (Type type)
  //## begin GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.hasinit preserve=no
      : m_type(type),
        m_terminate(false)
  //## end GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.hasinit
  //## begin GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.initialization preserve=yes
  //## end GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.initialization
{
  //## begin GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.body preserve=yes
  //## end GeoVolumeAction::GeoVolumeAction%3CE234DF02D0.body
}


GeoVolumeAction::~GeoVolumeAction()
{
  //## begin GeoVolumeAction::~GeoVolumeAction%3CDF51210227_dest.body preserve=yes
  // Do nothing.     
  //## end GeoVolumeAction::~GeoVolumeAction%3CDF51210227_dest.body
}



//## Other Operations (implementation)
void GeoVolumeAction::handleVPhysVol (const GeoVPhysVol *)
{
  //## begin GeoVolumeAction::handleVPhysVol%3CE2033201E1.body preserve=yes
  // Do nothing.     
  //## end GeoVolumeAction::handleVPhysVol%3CE2033201E1.body
}

void GeoVolumeAction::terminate ()
{
  //## begin GeoVolumeAction::terminate%3CE2046003DA.body preserve=yes
  m_terminate = true;
  //## end GeoVolumeAction::terminate%3CE2046003DA.body
}

bool GeoVolumeAction::shouldTerminate () const
{
  //## begin GeoVolumeAction::shouldTerminate%3CE20461001A.body preserve=yes
  return m_terminate;
  //## end GeoVolumeAction::shouldTerminate%3CE20461001A.body
}

const GeoTraversalState* GeoVolumeAction::getState () const
{
  //## begin GeoVolumeAction::getState%3CE23D830046.body preserve=yes
  return &m_traversalState;
  //## end GeoVolumeAction::getState%3CE23D830046.body
}

GeoTraversalState* GeoVolumeAction::getState ()
{
  //## begin GeoVolumeAction::getState%3CE204D803A0.body preserve=yes
  return &m_traversalState;
  //## end GeoVolumeAction::getState%3CE204D803A0.body
}

GeoVolumeAction::Type GeoVolumeAction::getType () const
{
  //## begin GeoVolumeAction::getType%3CE23408014B.body preserve=yes
  return m_type;
  //## end GeoVolumeAction::getType%3CE23408014B.body
}

// Additional Declarations
  //## begin GeoVolumeAction%3CDF51210227.declarations preserve=yes
  //## end GeoVolumeAction%3CDF51210227.declarations

//## begin module%3CDF51210227.epilog preserve=yes
//## end module%3CDF51210227.epilog
