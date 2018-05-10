/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/** @file AthenaConverterTLPExtension.cxx
 *  @brief Extensions to AthenaConverter used by TP separation
 *  @author Marcin.Nowak@cern.ch
 **/

#include "TPTools/TopLevelTPCnvBase.h"
#include "TPTools/AthenaConverterTLPExtension.h"
#include "AthenaPoolUtilities/TPCnvTokenList_p1.h"
#include "PersistentDataModel/Token.h"
#include "CxxUtils/no_sanitize_undefined.h"

#include <stdexcept>
#include <sstream>
#include <iostream>
using namespace std;


unsigned short
AthenaConverterTLPExtension::getTPCnvID NO_SANITIZE_UNDEFINED () {
   return getTopLevelTPCnv()->getConverterID();
}


bool AthenaConverterTLPExtension::registerExtendingCnv NO_SANITIZE_UNDEFINED ( AthenaConverterTLPExtension *extending_converter )
{
   // check if all converter IDs are unique
   int	extendingTPLCnvID = extending_converter->getTPCnvID();
   if( extendingTPLCnvID == this->getTPCnvID()
       || m_extendingConverters.find( extendingTPLCnvID ) != m_extendingConverters.end() ) {
      ostringstream error;
      error << "Conflicting TP converter IDs when registering extending Athena Converter. ID=" << extendingTPLCnvID;
      throw runtime_error( error.str() );
   }
   AthenaConverterTLPExtension *cloned_converter;
   if( ! extending_converter->needsCloning() ) {
      cloned_converter = extending_converter;
      // cout << " TPCNVINFO: TP converter not cloned CLID=" << cnv->name() << endl;
   } else {
      // get a private converter copy
      cloned_converter = extending_converter->clone();
      if( !cloned_converter ) {
         ostringstream error;
         error << "Extending TP converter neeeds to define clone() method. Converter: " << extending_converter->name();
         throw runtime_error( error.str() );
      } 
      //cout << " TPCNVINFO: Cloned extending TP converter " << " Converter CLID=" << extending_converter->name() << endl;
      m_clonedExtendingCnvs.push_back( cloned_converter );
   }
   cloned_converter->wasClonedFrom( extending_converter );
   m_extendingConverters[ extendingTPLCnvID ] = cloned_converter;
   cloned_converter->getTopLevelTPCnv()->addTPConvertersTo( this->getTopLevelTPCnv() );
   return true;
}


void AthenaConverterTLPExtension::usingTPCnvForReading( void *cnv )
{
   usingTPCnvForReading( *reinterpret_cast<TopLevelTPCnvBase*>( cnv ) );
}


void AthenaConverterTLPExtension::usingTPCnvForReading( TopLevelTPCnvBase &baseTLPcnv )
{
   m_TLCnvForReading = &baseTLPcnv;

   if( getTopLevelTPCnv() != m_TLCnvForReading ) {
      // we are reading an old class version
      if( m_extendingConverters.size()
	  && m_extCnvMapMap.find( m_TLCnvForReading ) == m_extCnvMapMap.end() ) {
         // it is first time we use this TLP converter (for the old version)
         extCnvMap_t *newCnvMap = new extCnvMap_t;
         m_extCnvMapMap[ m_TLCnvForReading ] = newCnvMap;

         // copy clones of all extending converters to a new converter map 
         for( extCnvMap_t::const_iterator cnvI = m_extendingConverters.begin();
              cnvI != m_extendingConverters.end(); cnvI++ ) {
            AthenaConverterTLPExtension *cloned_converter = cnvI->second->clone();
            if( !cloned_converter ) {
               // basically panicking, this is not a runtime exception but an incomplete converter implementation
               ostringstream error;
               error << " TPCNVINFO: ERROR!  This extending TP converter does not define clone() method. "  << " Converter: " << cnvI->second->name();
               cerr << error.str() << endl;
               throw runtime_error( error.str() );
            }
            m_clonedExtendingCnvs.push_back( cloned_converter );
            cloned_converter->wasClonedFrom( cnvI->second );
            (*newCnvMap)[ cloned_converter->getTPCnvID() ] = cloned_converter;
            // this would be only necessary for writing, but we don't write old versions
            //cloned_converter->getTopLevelTPCnv()->addTPConvertersTo( this->getTopLevelTPCnv() );
         }
      }
   }
}



void AthenaConverterTLPExtension::writeExtendingObjects( void *baseObj, const std::string& key )
{
   TPCnvTokenList_p1 	*token_list =  getTopLevelTPCnv()->getTokenListVarFrom( baseObj );
   if( !token_list )
      throw std::runtime_error("Adding token to TP persistent object failed - getTokenListVar() returned NULL");
   
   for( const auto& cnv : m_extendingConverters ) {
      const Token *tok =  cnv.second->writeObject( key );
      if( tok !=0 ) {
         token_list->addToken( cnv.second->getTPCnvID(), tok->toString() );
         delete tok;
      }
   }
   /*
     for( extCnvMap_t::const_iterator cnv = m_extendingConverters.begin();
	cnv != m_extendingConverters.end(); cnv++ )
   {
      std::string token = cnv->second->writeObject( key );
      if( token.size() )
	 token_list->addToken(  cnv->second->getTPCnvID(), token );
   }
   */
}


void AthenaConverterTLPExtension::readExtendingObjects NO_SANITIZE_UNDEFINED ( void *baseObj )
{
   // m_TLCnvForReading may be 0 if reading pre-TP object
   if( m_TLCnvForReading ) {
      TPCnvTokenList_p1 	*tokens = m_TLCnvForReading->getTokenListVarFrom( baseObj );
      if( tokens && tokens->size() ) {
         // find which extending converter "set" to use
         extCnvMap_t *extendingConverters;
	 if( getTopLevelTPCnv() == m_TLCnvForReading ) {
	    // use the newest TP converters
	    extendingConverters = &m_extendingConverters;
	 } else {
	    extCnvMapMap_t::const_iterator extCnvMapIter = m_extCnvMapMap.find(m_TLCnvForReading);
	    if( extCnvMapIter == m_extCnvMapMap.end() ) {
	       throw std::runtime_error("Extending TP converter not available");
	    }
	    extendingConverters = extCnvMapIter->second;
	 }
	 // load the remaining "pieces" of this object
	 for( TPCnvTokenList_p1::const_iterator it = tokens->begin(); it != tokens->end(); it++ ) {
            if( it->token().size() < 36 ) {
               ostringstream err;
               err << "Corrupted Token in the list of extensions. Token='" << it->token() << "'"
                   << " CnvID=" << it->converterID() << ", Token list size=" << tokens->size();
 	       throw std::runtime_error(err.str());
            }              
	    extCnvMap_t::const_iterator cnv = extendingConverters->find( it->converterID() );
	    if( cnv == extendingConverters->end() ) {
               ostringstream err;
               err << "Extending TP converter not loaded! "
                   << "missing ExtCnvID=" << it->converterID()
                   << " corresponding POOL token: " << it->token();
	       throw std::runtime_error(err.str());
	    }
//	    std::cout << "--->>---  readExtendingObjects: "
//		      << " converter ID=" <<  it->converterID()
//		      << " token=" << it->token() << ", AP converter=" << cnv->second << std::endl;

	    cnv->second->readObject( it->token() );
	    TopLevelTPCnvBase *extTPCnv = cnv->second->getTopLevelTPCnvForReading();
	    extTPCnv->addTPConvertersForReadingTo( m_TLCnvForReading );
	 }
      }
   }
}




void AthenaConverterTLPExtension::deletePersistentObjects NO_SANITIZE_UNDEFINED ()
{
   for( extCnvMap_t::const_iterator cnv = m_extendingConverters.begin();
	cnv != m_extendingConverters.end(); cnv++ ) {
      cnv->second->deletePersistentObjects();
   }
   for( size_t c = 0; c < m_clonedExtendingCnvs.size(); c++ )
      m_clonedExtendingCnvs[c]->deletePersistentObjects();

   getTopLevelTPCnv()->deleteTLPersObject();
   // Can not do that - sometimes the converter is an automatic variable that is gone already
   //if( m_TLCnvForReading ) m_TLCnvForReading->deleteTLPersObject();
}



AthenaConverterTLPExtension::~AthenaConverterTLPExtension()
{
   // cout << "Deleting " << m_clonedExtendingCnvs.size() << " extending converters" << endl;
   for( size_t c = 0; c < m_clonedExtendingCnvs.size(); c++ )
      delete m_clonedExtendingCnvs[c];
   for( extCnvMapMap_t::const_iterator mmiter = m_extCnvMapMap.begin();
        mmiter != m_extCnvMapMap.end(); mmiter++ ) {
      delete mmiter->second;
   }
   
}
