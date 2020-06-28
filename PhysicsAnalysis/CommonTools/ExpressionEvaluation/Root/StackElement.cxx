/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$

// System include(s):
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cassert>

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


   StackElement& StackElement::operator=( int rhs ) {

      m_type = SE_INT;
      m_intVal = rhs;
      m_doubleVal = 0;
      m_vecIntVal.clear();
      m_vecDoubleVal.clear();
      m_moved = false;

      return *this;
   }

   StackElement& StackElement::operator=( double rhs ) {

      m_type = SE_DOUBLE;
      m_intVal = 0;
      m_doubleVal = rhs;
      m_vecIntVal.clear();
      m_vecDoubleVal.clear();
      m_moved = false;

      return *this;
   }


   StackElement& StackElement::operator=( const std::vector< int >& rhs ) {

      m_type = SE_VECINT;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal = rhs;
      m_vecDoubleVal.clear();
      m_moved = false;

      return *this;
   }
   StackElement& StackElement::operator=( std::vector< int >&& rhs ) {

      m_type = SE_VECINT;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal = std::move(rhs);
      m_vecDoubleVal.clear();
      m_moved = false;

      return *this;
   }
   StackElement& StackElement::operator=( const std::vector< double >& rhs ) {

      m_type = SE_VECDOUBLE;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal.clear();
      m_vecDoubleVal = rhs;
      m_moved = false;

      return *this;
   }

   StackElement& StackElement::operator=( std::vector< double >&& rhs ) {

      m_type = SE_VECDOUBLE;
      m_intVal = 0;
      m_doubleVal = 0;
      m_vecIntVal.clear();
      m_vecDoubleVal = std::move(rhs);
      m_moved = false;

      return *this;
   }

   StackElement StackElement::operator!() {

      // Create a copy of the object:
      if (this->m_moved) throw std::logic_error("Content already moved");
      StackElement temp( std::move(*this) );
      this->m_moved=true;
      // ...and modify its payload appropriately:
      switch( m_type ) {

      case SE_INT:
         temp.m_intVal = !( temp.m_intVal );
         break;

      case SE_DOUBLE:
         //         temp.makeInt();
         temp.m_intVal = !( temp.m_doubleVal );
         break;

      case SE_VECINT:
         for( std::size_t i = 0; i < temp.m_vecIntVal.size(); ++i ) {
            temp.m_vecIntVal[ i ] = !( temp.m_vecIntVal[ i ] );
         }
         break;

      case SE_VECDOUBLE:
         //         temp.makeInt();
         temp.m_vecIntVal.resize(temp.m_vecDoubleVal.size());
         for( std::size_t i = 0; i < temp.m_vecDoubleVal.size(); ++i ) {
            temp.m_vecIntVal[ i ] = !( temp.m_vecDoubleVal[ i ] ); // @TODO use >0. ?
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

   StackElement StackElement::operator-() {

      // Create a copy of the object:
      if (this->m_moved) throw std::logic_error("Content already moved");
      StackElement temp( std::move(*this) );
      this->m_moved=true;

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
   StackElement& StackElement::operator OP( StackElement& rhs ) {       \
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
   std::vector< int >
   StackElement::vectorValue( std::size_t sizeIfScalar )  {

      if (this->m_moved) throw std::logic_error("Content already moved");
      switch( m_type ) {

      case SE_VECINT:
         this->m_moved=true;
         return std::move(m_vecIntVal);
         break;

      case SE_VECDOUBLE: {
         std::vector<int> ret(m_vecDoubleVal.size());
         std::transform (m_vecDoubleVal.begin(), m_vecDoubleVal.end(), ret.begin(), [](const double  &a) -> int { return static_cast<int>(a);});
         break;
      }
      case SE_INT: {
         return std::vector<int>( sizeIfScalar, m_intVal );
         break;
      }
      case SE_DOUBLE:{
         return std::vector<int>( sizeIfScalar,static_cast< int >( m_doubleVal ) );
         break;
      }
      default:
         throw std::runtime_error( "(int) vectorValue(): Unsupported "
                                   "StackElement" );
         break;
      }
      return std::vector<int>();
   }

   template<>
   std::vector< double >
   StackElement::vectorValue( size_t sizeIfScalar ) {

      if (this->m_moved) throw std::logic_error("Content already moved");
      switch( m_type ) {

      case SE_VECINT: {
         std::vector<double> ret(m_vecIntVal.size());
         std::transform (m_vecIntVal.begin(), m_vecIntVal.end(), ret.begin(), [](int a) -> double { return a;});
         return ret;
         break;
      }
      case SE_VECDOUBLE:
         m_moved=true;
         return std::move(m_vecDoubleVal);
         break;

      case SE_INT: {
         return std::vector<double>(sizeIfScalar,static_cast< double >( m_intVal ) );
         break;
      }
      case SE_DOUBLE: {
         return std::vector<double>(sizeIfScalar, m_doubleVal );
         break;
      }
      default:
         throw std::runtime_error( "(dbl) vectorValue(): Unsupported "
                                   "StackElement" );
         break;
      }
      return std::vector<double>();
   }

   void StackElement::makeInt() {

      if( m_type == SE_DOUBLE ) {
         m_type = SE_INT;
         m_intVal = m_doubleVal;
      } else if( m_type == SE_VECDOUBLE ) {
         m_type = SE_VECINT;
         m_vecIntVal.clear();
         for( auto& value : m_vecDoubleVal ) {
            m_vecIntVal.push_back( static_cast< int >( value ) );
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
         for( auto& value : m_vecIntVal ) {
            m_vecDoubleVal.push_back( static_cast< double >( value ) );
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
      } else if( m_type == SE_DOUBLE ) {
         m_type = SE_VECDOUBLE;
         m_vecDoubleVal.clear();
         m_vecDoubleVal.resize( n, m_doubleVal );
      }

      return;
   }

   StackElement StackElement::valueFromProxy() const {

      StackElement tmp;
      if( ! isProxy() ) {
         return tmp;
      }

      if( ! m_determinedVariableType ) {
         m_variableType = m_proxyLoader->variableTypeFromString( m_varName );
         if ( m_variableType != IProxyLoader::VT_VECEMPTY) {
            m_determinedVariableType = true;
         }
      }

      switch( m_variableType ) {

      case IProxyLoader::VT_INT:
         tmp = m_proxyLoader->loadIntVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_DOUBLE:
         tmp = m_proxyLoader->loadDoubleVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_VECINT:
         tmp = m_proxyLoader->loadVecIntVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_VECDOUBLE:
         tmp = m_proxyLoader->loadVecDoubleVariableFromString( m_varName );
         break;

      case IProxyLoader::VT_VECEMPTY:
         tmp=std::vector<double>();
         break;
      case IProxyLoader::VT_UNK:
      default:
         throw std::runtime_error( "Got VT_UNK - unknown identifier: " +
                                   m_varName );
         break;
      }

      return tmp;
   }


   template <class T_CompHelper, class T>
   std::vector<int> compareVector(std::vector<T> &&a, std::vector<T> &&b,T_CompHelper helper)
   {
      std::vector<int> ret;
      ret.resize(a.size());
      assert( a.size() == b.size() );
      for (std::size_t idx=0; idx < a.size(); ++idx) {
         ret[idx] = helper.compare(a[idx],b[idx]);
      }
      return ret;
   }
   template <class T_CompHelper, class T>
   std::vector<int> compareVectorAlt(const std::vector<T> &&a, std::vector<T> &&b,T_CompHelper helper)
   {
      return compareVectorAlt(a,b,helper);
   }

   template <class T_CompHelper>
   std::vector<int> compareVector(std::vector<int> &&a, const std::vector<int> &&b,T_CompHelper helper)
   {
      assert( a.size() == b.size() );
      for (std::size_t idx=0; idx < a.size(); ++idx) {
         a[idx] = helper.compare(a[idx],b[idx]);
      }
      return std::vector<int>(std::move(a));
   }

   template <class T_CompHelper>
   std::vector<int> compareVectorAlt(const std::vector<int> &&a, std::vector<int> &&b,T_CompHelper helper)
   {
      assert( a.size() == b.size() );
      for (std::size_t idx=0; idx < a.size(); ++idx) {
         b[idx] = helper.compare(a[idx],b[idx]);
      }
      return std::vector<int>(std::move(b));
   }

   template <class T_CompHelper>
   StackElement StackElement::_comparisonOp(StackElement &other, T_CompHelper comp_helper)
   {
      if (m_type ==SE_UNK || other.m_type==SE_UNK) {
         throw std::runtime_error( "ERROR: Can't operate on SE_UNK "
                                   "StackElements");
      }
      if( isScalar() && other.isScalar() ) {
         if( m_type == SE_INT  && other.m_type == SE_INT ) {
            return comp_helper.compare(scalarValue< int >(),other.scalarValue< int >() );
         } else {
            return comp_helper.compare(scalarValue< double >(),other.scalarValue< double >() );
         }
      } else {
         if (isVector() && other.isVector() && this->size() != other.size()) {
            throw std::runtime_error( "Incompatible vectors - different length" );
         }
         std::size_t the_size = ( m_type == SE_VECINT || m_type==SE_VECDOUBLE) ? this->size() : other.size();
         if(    ( m_type == SE_VECINT)
                && ( other.m_type == SE_VECINT || other.m_type == SE_INT )) {
            return compareVector( this->vectorValue<int>(the_size) , other.vectorValue< int >(the_size), comp_helper );
         }
         else if( m_type == SE_INT && other.m_type == SE_VECINT) {
            return compareVectorAlt( this->vectorValue<int>(the_size) , other.vectorValue< int >(the_size), comp_helper );
         }
         else {
            return compareVector( this->vectorValue<double>(the_size) , other.vectorValue< double >(the_size), comp_helper );
         }
      }
   }

   class Helper_eq  { public: template <class T> int compare(const T &a, const T &b) { return a == b; } };
   class Helper_neq { public: template <class T> int compare(const T &a, const T &b) { return a != b; } };
   class Helper_and { public: template <class T> int compare(const T &a, const T &b) { return a && b; } };
   class Helper_or  { public: template <class T> int compare(const T &a, const T &b) { return a || b; } };
   class Helper_gt  { public: template <class T> int compare(const T &a, const T &b) { return a >  b; } };
   class Helper_gte { public: template <class T> int compare(const T &a, const T &b) { return a >= b; } };
   class Helper_lt  { public: template <class T> int compare(const T &a, const T &b) { return a <  b; } };
   class Helper_lte { public: template <class T> int compare(const T &a, const T &b) { return a <= b; } };

   StackElement StackElement::_eq(StackElement &b)  { return _comparisonOp(b, Helper_eq()); }
   StackElement StackElement::_neq(StackElement &b) { return _comparisonOp(b, Helper_neq()); }
   StackElement StackElement::_and(StackElement &b) { return _comparisonOp(b, Helper_and()); }
   StackElement StackElement::_or(StackElement &b)  { return _comparisonOp(b, Helper_or()); }
   StackElement StackElement::_gt(StackElement &b)  { return _comparisonOp(b, Helper_gt()); }
   StackElement StackElement::_gte(StackElement &b) { return _comparisonOp(b, Helper_gte()); }
   StackElement StackElement::_lt(StackElement &b)  { return _comparisonOp(b, Helper_lt()); }
   StackElement StackElement::_lte(StackElement &b) { return _comparisonOp(b, Helper_lte()); }


   template <>
   StackElement StackElement::_pow( const StackElement& n ) {

      if( n.isVector() ) {
         throw std::runtime_error( "Can't use vector as exponent" );
      } else {
         return _pow( n.scalarValue< double >() );
      }
   }

   StackElement StackElement::_sum() {

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

   StackElement StackElement::_count()  {

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

   StackElement StackElement::_abs() {
      if (this->m_moved) throw std::logic_error("Content already moved");

      StackElement temp(std::move(*this));
      this->m_moved=true;
      switch( m_type ) {

      case SE_INT:
         m_intVal = std::abs( temp.m_intVal );
         break;

      case SE_DOUBLE:
         m_doubleVal = std::abs( temp.m_doubleVal );
         break;

      case SE_VECINT:
         {
            for( int &value : temp.m_vecIntVal ) {
               value = std::abs( value );
            }
         }
         break;

      case SE_VECDOUBLE:
         {
            for( double &value : temp.m_vecDoubleVal ) {
               value = std::abs( value );
            }
         }
         break;

      default:
         // @throw exception ?
         break;
      }
      return temp;
   }

/// Helper macro for implementing many of the mathematical functions
#define UNARY_MATH_FUNCTION( FUNC, BASEFUNC )                           \
   StackElement StackElement::FUNC()  {                                 \
      if (this->m_moved) throw std::logic_error("Content already moved");\
      StackElement temp( std::move(*this) );                            \
      this->m_moved=true;                                               \
      temp.makeDouble();                                                \
      if( temp.m_type == SE_DOUBLE ) {                                  \
         temp.m_doubleVal = BASEFUNC( temp.m_doubleVal );               \
         return temp;                                                   \
      } else if( temp.m_type == SE_VECDOUBLE ) {                        \
         for( double& value : temp.m_vecDoubleVal ) {                   \
            value = BASEFUNC( value );                                  \
         }                                                              \
         return temp;                                                   \
      } else {                                                          \
         /* @TODO throw exception */                                    \
         return temp;                                                   \
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
         makeVector( other.size() );
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

      const std::size_t ourlen = size();
      if( ourlen != other.size() ) {
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
