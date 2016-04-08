/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//## begin module%1.5%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.5%.codegen_version

//## begin module%3CD1266D02D3.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3CD1266D02D3.cm

//## begin module%3CD1266D02D3.cp preserve=no
//## end module%3CD1266D02D3.cp

//## Module: RCBase%3CD1266D02D3; Pseudo Package body
//## Source file: /home/atlas/GEO/GeoModelKernel/RCBase.cxx

//## begin module%3CD1266D02D3.additionalIncludes preserve=no
//## end module%3CD1266D02D3.additionalIncludes

//## begin module%3CD1266D02D3.includes preserve=yes
#include <exception>
//## end module%3CD1266D02D3.includes

// RCBase
#include "GeoModelKernel/RCBase.h"
//## begin module%3CD1266D02D3.additionalDeclarations preserve=yes
//## end module%3CD1266D02D3.additionalDeclarations


// Class RCBase 

RCBase::RCBase()
  //## begin RCBase::RCBase%3CD1266D02D3_const.hasinit preserve=no
      : m_count(0)
  //## end RCBase::RCBase%3CD1266D02D3_const.hasinit
  //## begin RCBase::RCBase%3CD1266D02D3_const.initialization preserve=yes
  //## end RCBase::RCBase%3CD1266D02D3_const.initialization
{
  //## begin RCBase::RCBase%3CD1266D02D3_const.body preserve=yes
  //## end RCBase::RCBase%3CD1266D02D3_const.body
}


RCBase::~RCBase()
{
  //## begin RCBase::~RCBase%3CD1266D02D3_dest.body preserve=yes
  //## end RCBase::~RCBase%3CD1266D02D3_dest.body
}



//## Other Operations (implementation)
void RCBase::ref () const
{
  //## begin RCBase::ref%3CD12A80000C.body preserve=yes
  m_count++;
  //## end RCBase::ref%3CD12A80000C.body
}

void RCBase::unref () const
{
  //## begin RCBase::unref%3CD12A840120.body preserve=yes
  if (!m_count)
    {

    }
  else
    {
      m_count--;
      if (!m_count)
	delete this;
    }
  //## end RCBase::unref%3CD12A840120.body
}

unsigned int RCBase::refCount () const
{
  //## begin RCBase::refCount%3CD523F40158.body preserve=yes
  return m_count;
  //## end RCBase::refCount%3CD523F40158.body
}

// Additional Declarations
  //## begin RCBase%3CD1266D02D3.declarations preserve=yes
  //## end RCBase%3CD1266D02D3.declarations

//## begin module%3CD1266D02D3.epilog preserve=yes
//## end module%3CD1266D02D3.epilog
