/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: FakeProxy.cxx 487276 2012-03-08 11:04:56Z krasznaa $

// ROOT include(s):
#include <TClass.h>
#include <TROOT.h>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "FakeProxy.h"

namespace D3PD {

   /**
    * The constructor figures out how the variable will need to be
    * cleared later on event by event. It issues a warning if it
    * didn't manage to do so. (It's not an error condition not to
    * be able to clear the variable...)
    *
    * @param object Pointer to the object the proxy is to take posession of
    * @param ti The type info of the object
    */
   FakeProxy::FakeProxy( void* object, const std::type_info& ti )
      : m_clearType( ZERO ), m_object( object ), m_type( ti ),
        m_clear(), m_dict( 0 ) {

      // Handle the case when it's an object that we received:
      m_dict = getClass( m_type );
      if( m_dict ) {
         // Check if there is a "clear" function defined for this object:
         TMethodCall method( m_dict, "clear", "" );
         if( method.IsValid() ) {
            m_clearType = CLEAR;
            m_clear = method;
         }
      }
   }

   /**
    * The destructor tries to free the memory allocated by the object
    * in the correct way.
    */
   FakeProxy::~FakeProxy() {

      if( m_dict && m_object ) {
         m_dict->Destructor( m_object );
      } else if( m_object ) {
         deletePrimitive( m_object, m_type );
      }
   }

   /**
    * This function also uses the dictionary of the object to try to
    * clear it. For primitive types we don't do anything for now...
    */
   void FakeProxy::clear() {

      // Execute the clear function if it is defined:
      if( m_clearType == CLEAR ) {
         m_clear.Execute( m_object );
      }

      return;
   }

   /**
    * I took this implementation basically from D3PD::RootD3PD, as I
    * needed the same functionality here as well.
    *
    * @param ti The type info of the type in question
    * @returns A dictionary for the type if found, or a zero
    *          pointer if it's not
    */
   ::TClass* FakeProxy::getClass( const std::type_info& ti ) {

      TClass* cls = gROOT->GetClass( ti );
      if( ! cls ) {
         cls = gROOT->GetClass( System::typeinfoName( ti ).c_str() );
      }

      if( ! cls ) {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::DEBUG, "D3PD::FakeProxy" )
            << "Can't find class for typeinfo " << ti.name();
      }

      return cls;
   }

   /**
    * This function is used to create a new primitive type in memory.
    *
    * @param ti The type info for the primitive type
    * @returns A pointer to the newly created primitive, or a null
    *          pointer if the type is not known.
    */
   void* FakeProxy::newPrimitive( const std::type_info& ti ) {

      if( ti == typeid( bool ) ) {
         return new bool;
      } else if ( ti == typeid( char ) ) {
         return new char;
      } else if ( ti == typeid( short ) ) {
         return new short;
      } else if ( ti == typeid( unsigned short ) ) {
         return new unsigned short;
      } else if ( ti == typeid( int ) ) {
         return new int;
      } else if ( ti == typeid( unsigned int ) ) {
         return new unsigned int;
      } else if ( ti == typeid( long ) ) {
         return new long;
      } else if ( ti == typeid( unsigned long ) ) {
         return new unsigned long;
      } else if ( ti == typeid( unsigned long long ) ) {
         return new unsigned long long;
      } else if ( ti == typeid( float ) ) {
         return new float;
      } else if ( ti == typeid( double ) ) {
         return new double;
      }

      REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "D3PD::FakeProxy" )
         << "Type '" << ti.name() << "' is not a known primitive";

      return 0;
   }

   /**
    * This function is used to delete previously created primitive types
    * from memory.
    *
    * @param ptr Pointer to the primitive object
    * @param ti The type info for the primitive type
    */
   void FakeProxy::deletePrimitive( void* ptr, const std::type_info& ti ) {

      if( ti == typeid( bool ) ) {
         delete reinterpret_cast< bool* >( ptr );
      } else if ( ti == typeid( char ) ) {
         delete reinterpret_cast< char* >( ptr );
      } else if ( ti == typeid( short ) ) {
         delete reinterpret_cast< short* >( ptr );
      } else if ( ti == typeid( unsigned short ) ) {
         delete reinterpret_cast< unsigned short* >( ptr );
      } else if ( ti == typeid( int ) ) {
         delete reinterpret_cast< int* >( ptr );
      } else if ( ti == typeid( unsigned int ) ) {
         delete reinterpret_cast< unsigned int* >( ptr );
      } else if ( ti == typeid( long ) ) {
         delete reinterpret_cast< long* >( ptr );
      } else if ( ti == typeid( unsigned long ) ) {
         delete reinterpret_cast< unsigned long* >( ptr );
      } else if ( ti == typeid( unsigned long long ) ) {
         delete reinterpret_cast< unsigned long long* >( ptr );
      } else if ( ti == typeid( float ) ) {
         delete reinterpret_cast< float* >( ptr );
      } else if ( ti == typeid( double ) ) {
         delete reinterpret_cast< double* >( ptr );
      } else {
         REPORT_MESSAGE_WITH_CONTEXT( MSG::WARNING, "D3PD::FakeProxy" )
            << "Couldn't delete type: " << ti.name() << "; There is a mistake "
            << "in the code!";
      }

      return;
   }

} // namespace D3PD
