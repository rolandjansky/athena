// $Id: TProperty.cxx 612908 2014-08-21 16:19:03Z krasznaa $

// Local include(s):
#include "AsgTools/TProperty.h"

////////////////////////////////////////////////////////////////////////////////
//
//               Implementation of the setFrom specialisations
//

/// Helper macro for implementing the setFrom functions
#define TRY_TYPE( TYPE )                                    \
   do {                                                     \
      const TProperty< TYPE >* prop =                       \
         dynamic_cast< const TProperty< TYPE >* >( &rhs );  \
      if( prop && prop->pointer() ) {                       \
         *m_ptr = *( prop->pointer() );                     \
         return 0;                                          \
      }                                                     \
   } while( 0 )

template<>
int TProperty< float >::setFrom( const Property& rhs ) {

   // Check that we have a valid pointer:
   if( ! this->pointer() ) {
      return 1;
   }

   // Try some compatible types:
   TRY_TYPE( float );
   TRY_TYPE( double );
   TRY_TYPE( int );

   // Apparently none of them succeeded:
   return 1;
}

//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//       Implementation of the createProperty specialisations/overloads
//

Property* createProperty( const bool& rval ) {
   return new TProperty< bool >( const_cast< bool& >( rval ), Property::BOOL );
}

Property* createProperty( const int& rval ) {
   return new TProperty< int >( const_cast< int& >( rval ), Property::INT );
}

Property* createProperty( const float& rval ) {
   return new TProperty< float >( const_cast< float& >( rval ),
                                  Property::FLOAT );
}

Property* createProperty( const double& rval ) {
   return new TProperty< double >( const_cast< double& >( rval ),
                                   Property::DOUBLE );
}

Property* createProperty( const std::string& rval ) {
   return new TProperty< std::string >( const_cast< std::string& >( rval ),
                                        Property::STRING );
}

Property* createProperty( const std::vector< int >& rval ) {
   typedef Property::IntVector vecInt_t;
   return new TProperty< vecInt_t >( const_cast< vecInt_t& >( rval ),
                                     Property::INTVECTOR );
}

Property* createProperty( const std::vector< float >& rval ) {
   typedef Property::FloatVector vecFloat_t;
   return new TProperty< vecFloat_t >( const_cast< vecFloat_t& >( rval ),
                                       Property::FLOATVECTOR );
}

Property* createProperty( const std::vector< std::string >& rval ) {
   typedef Property::StringVector vecString_t;
   return new TProperty< vecString_t >( const_cast< vecString_t& >( rval ),
                                        Property::STRINGVECTOR );
}

//
////////////////////////////////////////////////////////////////////////////////
