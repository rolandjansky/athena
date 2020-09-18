/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TPTools/ITPConverter.h"
#include "GaudiKernel/MsgStream.h"
#include <stdexcept>

static
void errorHandler()
{
   static const char *const ignore_error = getenv("ATHENA_TP_IGNORE_NOT_FOUND");
   if( ignore_error )  return;
   static const char * const do_abort = getenv("ATHENA_TP_DIE_ON_NOT_FOUND");
   if( do_abort )  abort();
   throw std::runtime_error( "TP converter not found" );
}
   

void
ITPConverter::converterNotFound( const std::type_info& converterType,
				 ITPConverter *c,
				 const std::string& typeName,
				 MsgStream& log ) const
{
   log << MSG::ERROR << ">>>>>> in parent TP converter " << typeid(*this).name()
       << ": could not find matching TP converter for type " << typeName << endmsg;
   if( c ) {
      log << MSG::ERROR << " - found incompatible converter of type "
	  << typeid(*c).name() << endmsg;
   }
   log << MSG::ERROR << "  Converter handle type was " << converterType.name() << endmsg;
   errorHandler();
}

   

void
ITPConverter::converterNotFound( unsigned typeID,
				 ITPConverter *c,
				 const std::string& reqCnvTypeName,
				 MsgStream& log ) const
{
   log << MSG::ERROR << ">>>>>> in parent TP converter " <<  typeid(*this).name()
       << "   requested TP converter for TP type ID " << typeID << " not found " << endmsg;
   if( c ) {
      log << MSG::ERROR << " - found converter " <<  typeid(*c).name()
          << " for " << c->transientTInfo().name()
          << " with an incompatible base type " << c->transBaseTInfo().name()
          << endmsg;
   }
   log << MSG::ERROR << "  Converter handle type was " << reqCnvTypeName <<  endmsg;
   errorHandler();
}

   
