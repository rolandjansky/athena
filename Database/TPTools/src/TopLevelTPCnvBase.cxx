/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TPTools/TopLevelTPCnvBase.h"

#include <stdexcept>

   
void TopLevelTPCnvBase::addTPConverter( ITPConverter *converter )
{
   addTPConverterForWriting( converter );
   addTPConverterForReading( converter );
}


   
void TopLevelTPCnvBase::addTPConverterForReading( ITPConverter *converter )
{
   // for reading, converters are selected based on their typeID value
   // which comes from the Ref
   // Ref contains top level converter ID + local type ID
   TPObjRef::typeID_t	full_typeID( getConverterID(), ++m_typeIDCount );
   m_convIdMap[ full_typeID.value() ] = converter;

//   std::cout << " **addTPConverter - adding converter for " << converter->transientTInfo().name() << " CnvID=" << full_typeID.value() << std::endl;
   converter->setTopConverter( this, full_typeID );
}



void TopLevelTPCnvBase::addTPConverterForWriting( ITPConverter *converter )
{
   // add converter to the map keyed by type_info
   // this map is used to find a converter when writing
   const  std::type_info &info( converter->transientTInfo() );
   if( m_converters.findConverter( info ) ) {
      // adding a second converter for the same type will mess up things
      const std::string	error("Duplicate TP converter for class ");
      throw std::runtime_error( error +  info.name() );
   }
   m_converters.addConverter( converter, info );
}

  
void TopLevelTPCnvBase::addExtTPConverterForReading( ITPConverter *converter )
{
   // for reading, converters are selected based on our own typeID value
   // which comes from the Ref
   // Ref contains top level converter ID + local type ID
   m_convIdMap[ converter->typeID().value() ] = converter;

//   std::cout << " **addExtTPConverterForReading - adding converter for " << converter->transientTInfo().name() << " CnvID=" << converter->typeID().value() << std::endl;
   converter->setRuntimeTopConverter( this );
}


/// copy all TP converters to another topLevel conveter 
void	TopLevelTPCnvBase::addTPConvertersTo( TopLevelTPCnvBase *cnv )
{
   // loop over all registered converters 
   for( convIdMap_t::const_iterator itr = m_convIdMap.begin();
	itr != m_convIdMap.end(); itr++ ) {
      // check if the converter is in the list of R/W converters
      if( m_converters.findConverter( itr->second->transientTInfo() ) == itr->second ) {
         // this is a R/W converter, add it for writing too
         cnv->addTPConverterForWriting( itr->second );
      }
      // add the convterter for reading
      cnv->addExtTPConverterForReading( itr->second );
   }
   // prevent attempts to add the same converters again
   cnv->rememberConverter( this );
}


void	TopLevelTPCnvBase::addTPConvertersForReadingTo( TopLevelTPCnvBase *cnv )
{
   // check if we have done this already, and if so then exit
   if( cnv->hasConvertersFrom( this ) ) {
      return;
   }
   // copy all TP converters to another topLevel conveter 
   for( convIdMap_t::const_iterator itr = m_convIdMap.begin();
	itr != m_convIdMap.end(); itr++ ) {
      cnv->addExtTPConverterForReading( itr->second );
   }
   // prevent attempts to add the same converters again
   cnv->rememberConverter( this );
}

   
