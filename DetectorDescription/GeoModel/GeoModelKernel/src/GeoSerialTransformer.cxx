/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3D1F73570100.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D1F73570100.cm

//## begin module%3D1F73570100.cp preserve=no
//## end module%3D1F73570100.cp

//## Module: GeoSerialTransformer%3D1F73570100; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/GeoSerialTransformer.cxx

//## begin module%3D1F73570100.additionalIncludes preserve=no
//## end module%3D1F73570100.additionalIncludes

//## begin module%3D1F73570100.includes preserve=yes
//## end module%3D1F73570100.includes

// GeoSerialTransformer
#include "GeoModelKernel/GeoSerialTransformer.h"
//## begin module%3D1F73570100.additionalDeclarations preserve=yes
#include "GeoModelKernel/GeoNodeAction.h"
//## end module%3D1F73570100.additionalDeclarations


// Class GeoSerialTransformer 

GeoSerialTransformer::GeoSerialTransformer (const GeoVPhysVol *volume, const GeoXF::Function *func, unsigned int copies)
  //## begin GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.hasinit preserve=no
  //## end GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.hasinit
  //## begin GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.initialization preserve=yes
  :
m_nCopies (copies),
m_function (func->clone ()),
m_physVol (volume)
  //## end GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.initialization
{
  //## begin GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.body preserve=yes
  m_physVol->ref ();
  //## end GeoSerialTransformer::GeoSerialTransformer%3D1F75650201.body
}


GeoSerialTransformer::~GeoSerialTransformer()
{
  //## begin GeoSerialTransformer::~GeoSerialTransformer%3D1F73570100_dest.body preserve=yes
  m_physVol->unref ();
  delete m_function;
  //## end GeoSerialTransformer::~GeoSerialTransformer%3D1F73570100_dest.body
}



//## Other Operations (implementation)
void GeoSerialTransformer::exec (GeoNodeAction *action) const
{
  //## begin GeoSerialTransformer::exec%3D1F753D031C.body preserve=yes
  action->handleSerialTransformer (this);
  //## end GeoSerialTransformer::exec%3D1F753D031C.body
}

const GeoXF::Function * GeoSerialTransformer::getFunction () const
{
  //## begin GeoSerialTransformer::getFunction%3FD60E350091.body preserve=yes
  return m_function;
  //## end GeoSerialTransformer::getFunction%3FD60E350091.body
}

// Additional Declarations
  //## begin GeoSerialTransformer%3D1F73570100.declarations preserve=yes
  //## end GeoSerialTransformer%3D1F73570100.declarations

//## begin module%3D1F73570100.epilog preserve=yes
//## end module%3D1F73570100.epilog
