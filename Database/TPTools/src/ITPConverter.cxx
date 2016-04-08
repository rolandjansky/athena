/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TPTools/ITPConverter.h"
#include "GaudiKernel/MsgStream.h"
#include <stdexcept>

static
void errorHandler()
{
   static char *ignore_error = getenv("ATHENA_TP_IGNORE_NOT_FOUND");
   if( ignore_error )  return;
   static char *do_abort = getenv("ATHENA_TP_DIE_ON_NOT_FOUND");
   if( do_abort )  abort();
   throw std::runtime_error( "TP converter not found" );
}
   

void
ITPConverter::converterNotFound( const std::type_info& converterType,
				 ITPConverter *c,
				 const std::string& typeName,
				 MsgStream& log )
{
   log << MSG::ERROR << ">>>>>> in parent TP converter " << typeid(*this).name()
       << ": could not find matching TP converter for type " << typeName << endreq;
   if( c ) {
      log << MSG::ERROR << " - found incompatible converter of type "
	  << typeid(*c).name() << endreq;
   }
   log << MSG::ERROR << "  Converter handle type was " << converterType.name() << endreq;
   errorHandler();
}

   

void
ITPConverter::converterNotFound( unsigned typeID,
				 ITPConverter *c,
				 const std::string& reqCnvTypeName,
				 MsgStream& log )
{
   log << MSG::ERROR << ">>>>>> in parent TP converter " <<  typeid(*this).name()
       << "   requested TP converter for TP type ID " << typeID << " not found " << endreq;
   if( c ) {
      log << MSG::ERROR << " - found converter " <<  typeid(*c).name()
          << " for " << c->transientTInfo().name()
          << " with an incompatible base type " << c->transBaseTInfo().name()
          << endreq;
   }
   log << MSG::ERROR << "  Converter handle type was " << reqCnvTypeName <<  endreq;
   errorHandler();
}

   
