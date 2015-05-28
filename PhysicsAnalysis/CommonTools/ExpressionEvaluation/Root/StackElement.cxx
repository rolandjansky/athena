/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <cmath>
#include <iostream>
#include <stdexcept>

// Local include(s):
#include "ExpressionEvaluation/StackElement.h"

namespace {

   template< typename T >
   std::ostream& operator <<( std::ostream& os, const std::vector< T >& vec ) {

      os << '[';
      for( std::size_t i = 0; i < vec.size(); ++i ) {
         os << vec[ i ];
         if( ( i + 1 ) < vec.size() ) {
            os << ", ";
         }
      }
      os << ']';

      return os;
   }

} // private namespace

namespace ExpressionParsing {

   StackElement::StackElement()
      : m_type( SE_UNK ),
        m_intVal( 0 ),
        m_doubleVal( 0 ),
        m_vecIntVal(),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

   }

   StackElement::StackElement( unsigned int val )
      : m_type( SE_INT ),
        m_intVal( val ), 
        m_doubleVal( 0 ),
        m_vecIntVal(),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

   }

   StackElement::StackElement( int val )
      : m_type( SE_INT ),
        m_intVal( val ),
        m_doubleVal( 0 ),
        m_vecIntVal(),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

   }

   StackElement::StackElement( double val )
      : m_type( SE_DOUBLE ),
        m_intVal( 0 ),
        m_doubleVal( val ),
        m_vecIntVal(),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

   }

   StackElement::StackElement( const std::vector< int >& val )
      : m_type( SE_VECINT ),
        m_intVal( 0 ),
        m_doubleVal( 0 ),
        m_vecIntVal( val ),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

      for( auto& value : m_vecIntVal ) {
         m_vecDoubleHelper.push_back( value );
      }
   }

   StackElement::StackElement( const std::vector< double >& val )
      : m_type( SE_VECDOUBLE ),
        m_intVal( 0 ),
        m_doubleVal( 0 ),
        m_vecIntVal(),
        m_vecDoubleVal( val ),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName(),
        m_proxyLoader( nullptr ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

      for( auto& value : m_vecDoubleVal ) {
         m_vecIntHelper.push_back( value );
      }
   }

   StackElement::StackElement( const std::string &val,
                               IProxyLoader* proxyLoader )
      : m_type( SE_UNK ),
        m_intVal( 0 ),
        m_doubleVal( 0 ),
        m_vecIntVal(),
        m_vecDoubleVal(),
        m_vecIntHelper(),
        m_vecDoubleHelper(),
        m_varName( val ),
        m_proxyLoader( proxyLoader ),
        m_variableType( IProxyLoader::VT_UNK ),
        m_determinedVariableType( false ) {

   }

   StackElement& StackElement::operator=( int rhs ) {

      m_type = SE_INT;
      m_intVal = rhs;
      m_doubleVal = 0;
      m_vecIntVal.clear();
      m_vecDoubleVal.clear();
      m_vecIntHelper.clear();
      m_vecDoubleHelper.clear();

      return *this;
   }

   StackElement& StackElement::operator=( double rhs ) {

      m_type = SE_DOUBLE;
      m_intVal = 0;
      m_doubleVal = rhs;
      m_vecIntVal.clear();
      m_vecDoubleVal.clear();
      m_vecIntHelper.clear();
      m_vecDoubleHelper.clear();

      return *this;
   }


   StackElement& StackElement::operator=( const std::vector< int >& rhs ) {

      m_type = SE_VECINT;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal = rhs;
      m_vecDoubleVal.clear();
      m_vecIntHelper.clear();
      m_vecDoubleHelper.clear();

      for( auto& value : m_vecIntVal ) {
         m_vecDoubleHelper.push_back( value );
      }

      return *this;
   }

   StackElement& StackElement::operator=( const std::vector< double >& rhs ) {

      m_type = SE_VECDOUBLE;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal.clear();
      m_vecDoubleVal = rhs;
      m_vecIntHelper.clear();
      m_vecDoubleHelper.clear();

      for( auto& value : m_vecDoubleVal ) {
         m_vecIntHelper.push_back( value );
      }

      return *this;
   }

   StackElement StackElement::operator!() const {

      // Create a copy of the object:
      StackElement temp( *this );

      // ...and modify its payload appropriately:
      switch( m_type ) {

      case SE_INT:
         temp.m_intVal = !( this->m_intVal );
         break;

      case SE_DOUBLE:
         temp.makeInt();
         temp.m_intVal = !( this->m_doubleVal );
         break;

      case SE_VECINT:
         for( std::size_t i = 0; i < temp.m_vecIntVal.size(); ++i ) {
            temp.m_vecIntVal[ i ] = !( temp.m_vecIntVal[ i ] );
         }
         break;

      case SE_VECDOUBLE:
         temp.makeInt();
         for( std::size_t i = 0; i < temp.m_vecIntVal.size(); ++i ) {
            temp.m_vecIntVal[ i ] = !( temp.m_vecDoubleVal[ i ] );
         }
         break;

      default:
         throw std::runtime_error( "! operator called on unknown "
                                   "variable type" );
         break;
      }

      // Return the negated object:
      return temp;
   }

   StackElement StackElement::operator-() const {

      // Create a copy of the object:
      StackElement temp( *this );

      // ...and modify its payload appropriately:
      switch( m_type ) {

      case SE_INT:
         temp.m_intVal = -( this->m_intVal );
         break;

      case SE_DOUBLE:
         temp.m_doubleVal = -( this->m_doubleVal );
         break;

      case SE_VECINT:
         for( std::size_t i = 0; i < temp.m_vecIntVal.size(); ++i ) {
            temp.m_vecIntVal[ i ] = -( temp.m_vecIntVal[ i ] );
         }
         break;

      case SE_VECDOUBLE:
         for( std::size_t i = 0; i < temp.m_vecDoubleVal.size(); ++i ) {
            temp.m_vecDoubleVal[ i ] = -( temp.m_vecDoubleVal[ i ] );
         }
         break;

      default:
         throw std::runtime_error( "- operator called on unknown "
                                   "variable type" );
         break;
      }

      // Return the inverted object:
      return temp;
   }

/// Helper macro implementing the assignment operator specialisations
#define IMPL_ASSIGN_OP( OP )                                            \
   template <>                                                          \
   StackElement& StackElement::operator OP( const StackElement& rhs ) { \
      makeVectorIfNecessary( rhs );                                     \
      makeDoubleIfNecessary( rhs );                                     \
      switch( m_type ) {                                                \
      case SE_INT:                                                      \
         m_intVal OP rhs.scalarValue< int >();                          \
         break;                                                         \
      case SE_DOUBLE:                                                   \
         m_doubleVal OP rhs.scalarValue< double >();                    \
         break;                                                         \
      case SE_VECINT:                                                   \
         *this OP rhs.vectorValue< int >( m_vecIntVal.size() );         \
         break;                                                         \
      case SE_VECDOUBLE:                                                \
         *this OP rhs.vectorValue< double >( m_vecDoubleVal.size() );   \
         break;                                                         \
      default:                                                          \
         throw std::runtime_error( "StackElement ill-defined in "       \
                                   #OP );                               \
         break;                                                         \
      }                                                                 \
      return *this;                                                     \
   }

   IMPL_ASSIGN_OP( -= )
   IMPL_ASSIGN_OP( += )
   IMPL_ASSIGN_OP( *= )
   IMPL_ASSIGN_OP( /= )

/// The macro is not needed anymore:
#undef IMPL_ASSIGN_OP

   StackElement::ElementType StackElement::getType() const {

      return m_type;
   }

   bool StackElement::isScalar() const {

      return ( ( m_type == SE_INT ) || ( m_type == SE_DOUBLE ) );
   }

   bool StackElement::isVector() const {

      return ( ( m_type == SE_VECINT ) || ( m_type == SE_VECDOUBLE ) );
   }

   bool StackElement::isProxy() const {

      return ( m_varName.length() > 0 );
   }

   int StackElement::asInt() const {

      if( m_type != SE_INT ) {
         throw std::runtime_error( "asInt() only valid for SE_INT" );
      }

      return scalarValue< int >();
   }

   bool StackElement::asBool() const {

      if( ! ( ( m_type == SE_INT ) || ( m_type == SE_DOUBLE ) ) ) {
         throw std::runtime_error( "asBool() only valid for non-vector types" );
      }

      return scalarValue< bool >();
   }

   template<>
   const std::vector< int >&
   StackElement::vectorValue( std::size_t sizeIfScalar ) const {

      switch( m_type ) {

      case SE_VECINT:
         return m_vecIntVal;
         break;

      case SE_VECDOUBLE:
         return m_vecIntHelper;
         break;

      case SE_INT:
         m_vecIntHelper.clear();
         m_vecIntHelper.resize( sizeIfScalar, m_intVal );
         return m_vecIntHelper;
         break;

      case SE_DOUBLE:
         m_vecIntHelper.clear();
         m_vecIntHelper.resize( sizeIfScalar,
                                static_cast< int >( m_doubleVal ) );
         return m_vecIntHelper;
         break;

      default:
         throw std::runtime_error( "(int) vectorValue(): Unsupported "
                                   "StackElement" );
         break;
      }
   }

   template<>
   const std::vector< double >&
   StackElement::vectorValue( size_t sizeIfScalar ) const {

      switch( m_type ) {

      case SE_VECINT:
         return m_vecDoubleHelper;
         break;

      case SE_VECDOUBLE:
         return m_vecDoubleVal;
         break;

      case SE_INT:
         m_vecDoubleHelper.clear();
         m_vecDoubleHelper.resize( sizeIfScalar,
                                   static_cast< double >( m_intVal ) );
         return m_vecDoubleHelper;
         break;

      case SE_DOUBLE:
         m_vecDoubleHelper.clear();
         m_vecDoubleHelper.resize( sizeIfScalar, m_doubleVal );
         return m_vecDoubleHelper;
         break;

      default:
         throw std::runtime_error( "(dbl) vectorValue(): Unsupported "
                                   "StackElement" );
         break;
      }
   }

   void StackElement::makeInt() {

      if( m_type == SE_DOUBLE ) {
         m_type = SE_INT;
         m_intVal = m_doubleVal;
      } else if( m_type == SE_VECDOUBLE ) {
         m_type = SE_VECINT;
         m_vecIntVal.clear();
         m_vecDoubleHelper.clear();
         for( auto& value : m_vecDoubleVal ) {
            m_vecIntVal.push_back( static_cast< int >( value ) );
            m_vecDoubleHelper.push_back( static_cast< int >( value ) );
         }
      }

      return;
   }

   void StackElement::makeDouble() {

      if( m_type == SE_INT ) {
         m_type = SE_DOUBLE;
         m_doubleVal = m_intVal;
      } else if( m_type == SE_VECINT ) {
         m_type = SE_VECDOUBLE;
         m_vecDoubleVal.clear();
         m_vecIntHelper.clear();
         for( auto& value : m_vecIntVal ) {
            m_vecDoubleVal.push_back( static_cast< double >( value ) );
            m_vecIntHelper.push_back( value );
         }
      }

      return;
   }

   void StackElement::makeVector( std::size_t n ) {

      if( isVector() ) {
         return;
      }

      if( m_type == SE_INT ) {
         m_type = SE_VECINT;
         m_vecIntVal.clear();
         m_vecIntVal.resize( n, m_intVal );
         m_vecDoubleHelper.clear();
         m_vecDoubleHelper.resize( n, static_cast< double >(  m_intVal ) );
      } else if( m_type == SE_DOUBLE ) {
         m_type = SE_VECDOUBLE;
         m_vecDoubleVal.clear();
         m_vecDoubleVal.resize( n, m_doubleVal );
         m_vecIntHelper.clear();
         m_vecIntHelper.resize( n, static_cast< int >( m_doubleVal ) );
      }

      return;
   }

   void StackElement::setValueFromProxy() {

      if( ! isProxy() ) {
         return;
      }

      if( ! m_determinedVariableType ) {
         m_variableType = m_proxyLoader->variableTypeFromString( m_varName );
         m_determinedVariableType = true;
      }

      switch( m_variableType ) {

      case IProxyLoader::VT_INT:
         m_type = SE_INT;
         m_intVal = m_proxyLoader->loadIntVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_DOUBLE:
         m_type = SE_DOUBLE;
         m_doubleVal = m_proxyLoader->loadDoubleVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_VECINT:
         m_type = SE_VECINT;
         m_vecIntVal = m_proxyLoader->loadVecIntVariableFromString( m_varName );
         for( auto& value : m_vecIntVal ) {
            m_vecDoubleHelper.push_back( static_cast< double >( value ) );
         }
         break;

      case IProxyLoader::VT_VECDOUBLE:
         m_type = SE_VECDOUBLE;
         m_vecDoubleVal =
            m_proxyLoader->loadVecDoubleVariableFromString( m_varName );
         for( auto& value : m_vecDoubleVal ) {
            m_vecIntHelper.push_back( static_cast< int >( value ) );
         }
         break;

      case IProxyLoader::VT_UNK:
      default:
         throw std::runtime_error( "Got VT_UNK - unknown identifier: " +
                                   m_varName );
         break;
      }

      return;
   }

   void StackElement::clearValueFromProxy() {

      if( ! isProxy() ) {
         return;
      }

      m_type = SE_UNK;
      m_intVal = 0;
      m_doubleVal = 0.;
      m_vecIntVal.clear();
      m_vecDoubleVal.clear();
      m_vecIntHelper.clear();
      m_vecDoubleHelper.clear();

      return;
   }

/// Helper function for implementing the binary comparison specialisations
#define IMPL_BINARY_COMPARISON_OP( OP, BASEOP )                         \
   template <>                                                          \
   StackElement StackElement::OP( const StackElement& other ) const {   \
      if( isScalar() && other.isScalar() ) {                            \
         if( ( m_type == SE_INT ) && ( other.m_type == SE_INT ) ) {     \
            return ( scalarValue< int >() BASEOP                        \
                     other.scalarValue< int >() );                      \
         } else {                                                       \
            return ( scalarValue< double >() BASEOP                     \
                     other.scalarValue< double >() );                   \
         }                                                              \
      } else if( isVector() && other.isVector() ) {                     \
         if( ( m_type == SE_VECINT ) &&                                 \
             ( other.m_type == SE_VECINT ) ) {                          \
            return ( this->OP( other.vectorValue< int >() ) );          \
         } else {                                                       \
            return ( this->OP( other.vectorValue< double >() ) );       \
         }                                                              \
      } else if( isVector() && other.isScalar() ) {                     \
         if( ( m_type == SE_VECINT ) && ( other.m_type == SE_INT ) ) {  \
            const std::size_t size = m_vecIntVal.size();                \
            return ( this->OP( other.vectorValue< int >( size ) ) );    \
         } else {                                                       \
            const std::size_t size = vectorValue< int >().size();       \
            return ( this->OP( other.vectorValue< double >( size ) ) ); \
         }                                                              \
      } else if( isScalar() && other.isVector() ) {                     \
         if( other.m_type == SE_VECINT ) {                              \
            return ( this->OP( other.vectorValue< int >() ) );          \
         } else {                                                       \
            return ( this->OP( other.vectorValue< double >() ) );       \
         }                                                              \
      } else {                                                          \
         throw std::runtime_error( "ERROR: Can't operate on SE_UNK "    \
                                   "StackElements");                    \
      }                                                                 \
   }

   IMPL_BINARY_COMPARISON_OP( _eq, == )
   IMPL_BINARY_COMPARISON_OP( _neq, != )
   IMPL_BINARY_COMPARISON_OP( _and, && )
   IMPL_BINARY_COMPARISON_OP( _or, || )
   IMPL_BINARY_COMPARISON_OP( _gt, > )
   IMPL_BINARY_COMPARISON_OP( _gte, >= )
   IMPL_BINARY_COMPARISON_OP( _lt, < )
   IMPL_BINARY_COMPARISON_OP( _lte, <= )

/// The macro is not needed anymore:
#undef IMPL_BINARY_COMPARISON_OP

   template <>
   StackElement StackElement::_pow( const StackElement& n ) const {

      if( n.isVector() ) {
         throw std::runtime_error( "Can't use vector as exponent" );
      } else {
         return _pow( n.scalarValue< double >() );
      }
   }

   StackElement StackElement::_sum() const {

      switch( m_type ) {

      case SE_INT:
         return m_intVal;
         break;

      case SE_DOUBLE:
         return m_doubleVal;
         break;

      case SE_VECINT:
         {
            int total = 0;
            for( int value : m_vecIntVal ) {
               total += value;
            }
            return total;
         }
         break;

      case SE_VECDOUBLE:
         {
            double total = 0.0;
            for( double value : m_vecDoubleVal ) {
               total += value;
            }
            return total;
         }
         break;

      default:
         return 0;
         break;
      }
   }

   StackElement StackElement::_count() const {

      switch( m_type ) {

      case SE_INT:
         return !!( m_intVal );
         break;

      case SE_DOUBLE:
         return !!( m_doubleVal );
         break;

      case SE_VECINT:
         {
            int total = 0;
            for( int value : m_vecIntVal ) {
               total += !!value;
            }
            return total;
         }
         break;

      case SE_VECDOUBLE:
         {
            int total = 0;
            for( double value : m_vecDoubleVal ) {
               total += !!value;
            }
            return total;
         }
         break;

      default:
         return 0;
         break;
      }
   }

   StackElement StackElement::_abs() const {

      switch( m_type ) {

      case SE_INT:
         return std::abs( m_intVal );
         break;

      case SE_DOUBLE:
         return std::abs( m_doubleVal );
         break;

      case SE_VECINT:
         {
            std::vector< int > temp;
            for( int value : m_vecIntVal ) {
               temp.push_back( std::abs( value ) );
            }
            return temp;
         }
         break;

      case SE_VECDOUBLE:
         {
            std::vector< double > temp;
            for( double value : m_vecDoubleVal ) {
               temp.push_back( std::abs( value ) );
            }
            return temp;
         }
         break;

      default:
         return 0;
         break;
      }
   }

/// Helper macro for implementing many of the mathematical functions
#define UNARY_MATH_FUNCTION( FUNC, BASEFUNC )                           \
   StackElement StackElement::FUNC() const {                            \
      StackElement temp( *this );                                       \
      temp.makeDouble();                                                \
      if( temp.m_type == SE_DOUBLE ) {                                  \
         temp.m_doubleVal = BASEFUNC( temp.m_doubleVal );               \
         return temp;                                                   \
      } else if( temp.m_type == SE_VECDOUBLE ) {                        \
         temp.m_vecIntHelper.clear();                                   \
         for( double& value : temp.m_vecDoubleVal ) {                   \
            value = BASEFUNC( value );                                  \
            temp.m_vecIntHelper.push_back( value );                     \
         }                                                              \
         return temp;                                                   \
      } else {                                                          \
         return 0;                                                      \
      }                                                                 \
   }

   UNARY_MATH_FUNCTION( _sqrt, sqrt )
   UNARY_MATH_FUNCTION( _cbrt, cbrt )
   UNARY_MATH_FUNCTION( _sin, sin )
   UNARY_MATH_FUNCTION( _cos, cos )
   UNARY_MATH_FUNCTION( _tan, tan )
   UNARY_MATH_FUNCTION( _asin, asin )
   UNARY_MATH_FUNCTION( _acos, acos )
   UNARY_MATH_FUNCTION( _atan, atan )
   UNARY_MATH_FUNCTION( _sinh, sinh )
   UNARY_MATH_FUNCTION( _cosh, cosh )
   UNARY_MATH_FUNCTION( _tanh, tanh )
   UNARY_MATH_FUNCTION( _asinh, asinh )
   UNARY_MATH_FUNCTION( _acosh, acosh )
   UNARY_MATH_FUNCTION( _atanh, atanh )
   UNARY_MATH_FUNCTION( _log, log )
   UNARY_MATH_FUNCTION( _exp, exp )

/// The macro is not needed anymore:
#undef UNARY_MATH_FUNCTION

   void StackElement::makeVectorIfNecessary( const StackElement& other ) {

      if( isVector() ) {
         return;
      }
      if( other.isVector() ) {
         makeVector( other.vectorValue< int >().size() );
      }

      return;
   }

   void StackElement::makeVectorIfNecessary(const std::vector< int >& other ) {

      if( isVector() ) {
         return;
      }
      makeVector( other.size() );

      return;
   }

   void
   StackElement::makeVectorIfNecessary( const std::vector< double >& other ) {

      if( isVector() ) {
         return;
      }
      makeVector( other.size() );

      return;
   }

   void StackElement::makeDoubleIfNecessary( const StackElement& other ) {

      if( ( m_type == SE_INT ) && ( other.m_type == SE_DOUBLE ) ) {
         makeDouble();
      } else if( ( m_type == SE_VECINT ) && ( other.m_type == SE_VECDOUBLE ) ) {
         makeDouble();
      }

      return;
   }

   void StackElement::makeDoubleIfNecessary( int ) {

      return;
   }

   void StackElement::makeDoubleIfNecessary( double ) {

      if( m_type == SE_INT ) {
         makeDouble();
      }

      return;
   }

   void StackElement::makeDoubleIfNecessary( const std::vector< int >& ) {

      return;
   }

   void StackElement::makeDoubleIfNecessary( const std::vector< double >& ) {

      if( m_type == SE_VECINT ) {
         makeDouble();
      }

      return;
   }

   void StackElement::ensureCompatible( const StackElement& other ) const {

      if( this->m_type != other.m_type ) {
         throw std::runtime_error( "Incompatible stack elements" );
      }

      return;
   }

   template<>
   void
   StackElement::ensureCompatibleVectors( const StackElement& other ) const {

      if( isScalar() ) {
         return;
      }

      const std::size_t ourlen = vectorValue< int >().size();
      if( ourlen != other.vectorValue< int >().size() ) {
         throw std::runtime_error( "Incompatible vectors - different length" );
      }
   }

   std::ostream &operator<<( std::ostream& os,
                             const StackElement& el ) {

      switch( el.m_type ) {

      case StackElement::SE_INT:
         os << "(int)" << el.m_intVal;
         break;

      case StackElement::SE_DOUBLE:
         os << "(double)" << el.m_doubleVal;
         break;

      case StackElement::SE_VECINT:
         os << "(vec<int>)" << el.m_vecIntVal;
         break;

      case StackElement::SE_VECDOUBLE:
         os << "(vec<double>)" << el.m_vecDoubleVal;
         break;

      case StackElement::SE_UNK:
         os << "SE_UNK";
         break;

      default:
         break;
      }

      return os;
   }

} // namespace ExpressionParsing
