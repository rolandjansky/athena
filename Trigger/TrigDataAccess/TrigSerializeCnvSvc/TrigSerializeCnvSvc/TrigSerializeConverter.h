// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGSERIALIZECONVERTER_H
#define TRIGSERIALIZECONVERTER_H

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"

#include "SGTools/ClassID_traits.h"
#include "SGTools/StorableConversions.h"
#include "StoreGate/StoreGateSvc.h"

#include "TrigSerializeCnvSvc/TrigStreamAddress.h"
#include "TrigSerializeCnvSvc/ITrigSerConvHelper.h"
#include "TrigSerializeCnvSvc/TrigSerializeCnvSvc.h"

#include "AthContainers/normalizedTypeinfoName.h"

#include <memory>


//fwd
template< class Cnv >
class CnvFactory;

/***
 * @brief TrigSerializeConverter is a templated converter which implements
 *    conversions from transient to persistent representation (and back) 
 *    used by TrigSerializeCnvSvc to serialize and deserialize Trigger objects  
 *
 * @author Jiri Masik Jiri.Masik@cern.ch
 * @author Lukas Heinrich lukas.heinrich@cern.ch
 */

namespace SG{class AuxElement;}
namespace SG{class IAuxStore;}


template<typename T>
  struct isxAOD {
  template<typename C>
  constexpr static typename std::enable_if<std::is_base_of<SG::AuxElement,typename std::pointer_traits<typename C::value_type>::element_type>::value,bool>::type isxAODVector(C*){return true;}
  template<typename C>
  constexpr static bool isxAODVector(...){return false;}
  template<typename C>
  constexpr static typename std::enable_if<std::is_base_of<SG::IAuxStore,C>::value,bool>::type isAuxVector(C*){return true;}
  template<typename C>
  constexpr static bool isAuxVector(...){return false;}
  static const bool value = isxAODVector<T>(0) || isAuxVector<T>(0);
};

template< typename DATA >
class TrigSerializeConverter : public Converter {

public:
  static const bool typeIsxAOD = isxAOD<DATA>::value;
  
   // per->tran
   StatusCode createObj( IOpaqueAddress* iAddr, DataObject *& pO ) {

      const std::string clname = ClassID_traits< DATA >::typeName();

      std::string normalized = SG::normalizedTypeinfoName( typeid(DATA) ); // normalized string representation (matching the names used in the root dictionary)


      void *ptr( 0 );
      if( m_log->level() <= MSG::DEBUG ) {
	*m_log << MSG::DEBUG << "In createObj for : " << clname << " normalized to " << normalized << endreq;
      }

      StatusCode sc = m_convHelper->createObj( normalized, iAddr, ptr, typeIsxAOD);
      if( sc.isFailure() ){
         *m_log << MSG::WARNING << "m_convHelper->createObj failed for "
                << clname << endreq;
      }

      DATA *nObj = ( DATA* ) ptr;
      pO = SG::asStorable( nObj );

      if( m_log->level() <= MSG::DEBUG ) {
         *m_log << MSG::DEBUG << "IOpaq: " << iAddr 
                << " created nObj: " << nObj << endreq;
      }

      
      

      if( m_sgsvc && nObj ){
         TrigStreamAddress *addr = dynamic_cast< TrigStreamAddress* >( iAddr );
         if( addr ) {
            if( m_sgsvc->contains< DATA >( addr->sgkey() ) ) {
	      //std::cout << "NOT OVERWRITING! ptr " << nObj << " key: " << addr->sgkey() << std::endl;
	      sc = m_sgsvc->overwrite( nObj, addr->sgkey(), false );
            } else {
               sc = m_sgsvc->record( nObj, addr->sgkey() );
            }
            if( sc.isFailure() ) {
               *m_log << MSG::ERROR << "SG::record failed for " << addr->sgkey()
                      <<  endreq;
            } else if( m_log->level() <= MSG::DEBUG ) {
               *m_log << MSG::DEBUG << "SG::record key: "
                      << addr->sgkey() << " class: " << clname << endreq;
            }
         } else {
            *m_log << MSG::WARNING << "createObj cast failed" << endreq;
         }
      } else if( m_log->level() <= MSG::DEBUG ) {
         *m_log << MSG::DEBUG << "did not put an object into SG" << endreq;
      }

      return sc;
   }
  
   // tran->per
   StatusCode createRep( DataObject* pObj, IOpaqueAddress*& pAddr ) {

      // const std::string clname = ClassID_traits<DATA>::typeName(); // old style, does not contain the _vX of the actual class type

      // const std::type_info& typeId = ClassID_traits<DATA>::typeId();  // damn, this line just does not compile

      std::string classname = SG::normalizedTypeinfoName( typeid(DATA) ); // normalized string representation (matching the names used in the root dictionary)

      DATA *d( 0 );
      SG::fromStorable( pObj, d );
      if( m_log->level() <= MSG::DEBUG ) {
         *m_log << MSG::DEBUG << "My createRep for " << classID() << " "
                << classname << endreq;
      }
      void *serptr = d;
      //
      std::vector< uint32_t > ser;

      StatusCode sc = m_convHelper->createRep( classname, serptr, ser, typeIsxAOD);
      if( m_log->level() <= MSG::DEBUG ) {
         *m_log << MSG::DEBUG << "convHelper " << ser.size() << endreq;
      }

      TrigStreamAddress *addr = new TrigStreamAddress( classID(), classname, "" );
      addr->add( ser );

      pAddr = addr;

      return sc;
   }

   StatusCode initialize(){

      if( m_log ) {
         m_log->setLevel( msgSvc()->outputLevel( "TrigSerializeConverter" ) );
      } else {
         return StatusCode::FAILURE;
      }

      StatusCode sc = m_convHelper.retrieve();
      if( ! sc.isSuccess() ) {
         if( m_log->level() <= MSG::DEBUG ) {
            *m_log << MSG::DEBUG << m_convHelper << "not retrieved" << endreq;
         }
      } else {
         if( m_log->level() <= MSG::DEBUG ) {
            *m_log << MSG::DEBUG << m_convHelper << "m_serializer retrieved"
                   << endreq;
         }
         if( ! m_convHelper->initialize().isSuccess() ) {
            if( m_log->level() <= MSG::DEBUG ) {
               *m_log << MSG::DEBUG << m_convHelper << " not initialized"
                      << endreq;
            }
         }
      }

      StatusCode scsg = m_sgHandle.retrieve();
      if (scsg.isFailure())
         *m_log << MSG::ERROR << "cannot access SG" << endreq;
      else {
         m_sgsvc = m_sgHandle.operator->();
      }
      return sc;
   }

   static const CLID& classID() { return ClassID_traits<DATA>::ID(); }

   virtual long int repSvcType() const;
   static long int storageType();

protected:
   friend class CnvFactory< TrigSerializeConverter< DATA > >;

   TrigSerializeConverter( ISvcLocator* svcloc )
      : Converter( storageType(), classID(), svcloc ),
        m_convHelper( "TrigSerializeConvHelper/TrigSerializeConvHelper" ),
        m_sgHandle( "StoreGateSvc", "TrigSerializeConverter" ),
        m_log( 0 ), m_sgsvc( 0 ) {

      m_log = new MsgStream( msgSvc(), "TrigSerializeConverter" );
   }

   ~TrigSerializeConverter(){
      delete m_log;
   }

private:
   ToolHandle< ITrigSerializeConvHelper >   m_convHelper;
   ServiceHandle< StoreGateSvc >  m_sgHandle;
   MsgStream *m_log;
   StoreGateSvc  *m_sgsvc;
};


template <typename DATA>
long int TrigSerializeConverter<DATA>::storageType() {
   return TrigSerializeCnvSvc::storageType();
}
template <typename DATA>
long int TrigSerializeConverter<DATA>::repSvcType() const {
   return storageType();
}


#endif // TRIGSERIALIZECONVERTER_H
