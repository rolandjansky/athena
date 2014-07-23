/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AthenaRootConverterHandle.h"



AthenaRootConverterHandle::AthenaRootConverterHandle(
   T_AthenaRootConverterBase *converter) :
	m_oldClass(0),
	m_oldObj(0),
	m_converter(converter)
{
}


AthenaRootConverterHandle::~AthenaRootConverterHandle()
{
   // not deleting the converter here - could be under user management
}


#include <iostream>

void AthenaRootConverterHandle::ReadBuffer(TBuffer& b, void* obj, Version_t version, UInt_t R__s, UInt_t R__c)
{
   if (m_oldObj == 0) {
      m_oldObj = m_converter->InstantiateOld();
      m_oldClass = gROOT->GetClass( m_converter->TypeIDOld() );
   }
   if( !m_oldClass ) {
      std::cerr << "-- AthenaRootConverterHandle::ReadBuffer - ERROR - Class name:"
	       <<  m_converter->ClassName() << ", typeid:" << m_converter->TypeIDOld().name()
	       << "; can NOT read the object - failed to get TClass (possibly unknown to dictionary)"
	       << std::endl;
      return;
   }
//   std::cout << "-- AthenaRootConverterHandle::ReadBuffer - Class name:"
//     <<  m_converter->ClassName() << ", m_oldClass=" << m_oldClass
//	     << ", typeid:" << m_converter->TypeIDOld().name() << std::endl;
   m_oldClass->ReadBuffer(b, m_oldObj, version, R__s, R__c);
   m_converter->Convert(m_oldObj, obj);
}
