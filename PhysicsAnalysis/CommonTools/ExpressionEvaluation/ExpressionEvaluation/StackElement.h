// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef EXPRESSIONEVALUATION_STACKELEMENT_H
#define EXPRESSIONEVALUATION_STACKELEMENT_H

// System include(s):
#include <iosfwd>
#include <vector>
#include <string>
#include <atomic>

// Local include(s):
#include "ExpressionEvaluation/IProxyLoader.h"

/// Namespace holding all the expression evaluation code
///
/// For convenience reasons all the code of the package is put into this
/// namespace.
///
/// @author Thomas Gillam (thomas.gillam@cern.ch)
///
/// $Revision$
/// $Date$
///
namespace ExpressionParsing {

   // Forward declaration(s):
   class IProxyLoader;

   /// Class describing a single element in a text expression
   ///
   /// A vector of such objects is used to describe a full text expression.
   /// As such, it's able to perform all the operations that can be described
   /// in a text expression.
   /// Most operations are destructive i.e. vector content will be modified and
   /// moved on operations. This is to reduce the number of vector copy operations.
   /// Thus, if the content should be preserved, a copy needs to be created and
   /// the operation should be applied to the copy.
   ///
   /// @author Thomas Gillam (thomas.gillam@cern.ch)
   ///
   /// $Revision$
   /// $Date$
   ///
   class StackElement {

      /// Declare the print operator as a friend of the class
      friend std::ostream& operator<<( std::ostream& os,
                                       const StackElement& el );

   public:
      /// Type of the data held by this object
      enum ElementType {
         SE_UNK       = 0, ///< The type is not known, or not applicable
         SE_INT       = 1, ///< The type is an integer
         SE_DOUBLE    = 2, ///< The type is a double
         SE_VECINT    = 3, ///< The type is a vector of integers
         SE_VECDOUBLE = 4  ///< The type is a vector of doubles
      };

      /// @name Constructor definitions
      /// @{

      /// Default constructor
      StackElement() {};
      /// Constructor creating the object with an unsigned integer value
      StackElement( unsigned int val ) : m_type( SE_INT ), m_intVal(static_cast<unsigned int>(val)) {}
      /// Constructor creating the object with a signed integer value
      StackElement( int val )  : m_type( SE_INT ), m_intVal(val) {}
      /// Constructor creating the object with a double value
      StackElement( double val ) : m_type( SE_DOUBLE ), m_doubleVal( val ) {}
      /// Constructor creating the object with a vector of integers value
      StackElement( const std::vector< int >& val ) : m_type( SE_VECINT ), m_vecIntVal(val) {}
      /// Constructor creating the object with a vector of doubles value
      StackElement( const std::vector< double >& val ) : m_type( SE_VECDOUBLE ), m_vecDoubleVal( val ) {}
      /// Constructor creating the object with a vector of integers value
      StackElement( std::vector< int >&& val ) : m_type( SE_VECINT ), m_vecIntVal(std::move(val)) {}
      /// Constructor creating the object with a vector of doubles value
      StackElement( std::vector< double >&& val ) : m_type( SE_VECDOUBLE ), m_vecDoubleVal( std::move(val) ) {}
      /// Constructor creating the object from a text expression
      StackElement( const std::string& val, IProxyLoader* proxyLoader ) : /*m_type( SE_UNK ), */m_varName( val ), m_proxyLoader( proxyLoader ) {}
;
      StackElement(const StackElement &a);
      StackElement(StackElement &&a);
      /// @}

      /// @name Assignment operators
      /// @{

      /// Operator assigning an integer value to the object
      StackElement& operator=( int rhs );
      /// Operator assigning a double value to the object
      StackElement& operator=( double rhs );
      /// Operator assigning a vector of integers to the object
      StackElement& operator=( const std::vector< int >& rhs );
      /// Operator assigning a vector of doubles to the object
      StackElement& operator=( const std::vector< double >& rhs );
      /// Operator assigning a vector of integers to the object
      StackElement& operator=( std::vector< int >&& rhs );
      /// Operator assigning a vector of doubles to the object
      StackElement& operator=( std::vector< double >&& rhs );

      StackElement& operator=( StackElement&& rhs );
      /// @}

      /// @name Comparison and logical operators
      /// @{

      /// NOT operator
      StackElement operator!();
      /// Inverse operator
      StackElement operator-();

      /// @}

      /// @name Modifier operators
      /// @{

      /// Subtract a scalar type from the object
      StackElement& operator-=( StackElement& rhs );
      template< typename T >
      StackElement& operator-=( const T& rhs );
      /// Subtract a vector type from the object
      template< typename T >
      StackElement& operator-=( const std::vector< T >& rhs );
      /// Add a scalar type to the object
      StackElement& operator+=( StackElement& rhs );
      template< typename T >
      StackElement& operator+=( const T& rhs );
      /// Add a vector type to the object
      template< typename T >
      StackElement& operator+=( const std::vector< T >& rhs );
      /// Multiply the object by a scalar type
      StackElement& operator*=( StackElement& rhs );
      template< typename T >
      StackElement& operator*=( const T& rhs );
      /// Multiply the object by a vector type
      template< typename T >
      StackElement& operator*=( const std::vector< T >& rhs );
      /// Divide the object by a scalar type
      StackElement& operator/=( StackElement& rhs );
      template< typename T >
      StackElement& operator/=( const T& rhs );
      /// Divide the object by a vector type
      template< typename T >
      StackElement& operator/=( const std::vector< T >& rhs );

      /// Operator subtracting a value from the object
      StackElement operator-( StackElement& rhs );
      template< typename T >
      StackElement operator-( const T& rhs );
      /// Operator adding a value to the object
      StackElement operator+( StackElement& rhs );
      template< typename T >
      StackElement operator+( const T& rhs );
      /// Operator multiplying the object by a value
      StackElement operator*( StackElement& rhs );
      template< typename T >
      StackElement operator*( const T& rhs );
      /// Operator dividing the object by a value
      StackElement operator/( StackElement& rhs );
      template< typename T >
      StackElement operator/( const T& rhs );

      /// @}

      /// @name Accessor functions
      /// @{

      /// Get the variable type of the object
      ElementType getType() const;
      /// Check if the object describes a scalar value
      bool isScalar() const;
      /// Check if the object describes a vector value
      bool isVector() const;
      /// Check if the object takes its value from a StoreGate object
      bool isProxy() const;

      /// Get the internal value of the object as an integer
      int asInt() const;
      /// Get the internal value of the object as a boolean
      bool asBool() const;

      /// Evaluate the value of the object into the requested scalar type
      template< typename T >
      T scalarValue() const;
      /// Evaluate the value of the object into the requested vector type
      template< typename T >
      std::vector< T > vectorValue( std::size_t sizeIfScalar = 0 );

      /// @}

      /// @name Modifier functions
      /// @{

      /// Function constructing internal integer values in case double values
      /// were given to the object initially
      void makeInt();
      /// Function constructing internal double values in case integer values
      /// were given to the object initially
      void makeDouble();
      /// Function converting a possibly scalar value into a vector
      void makeVector( std::size_t n );

      /// Set the internal variables of the object based on the objects in SG
      StackElement valueFromProxy() const;
      /// @}

      /// @name Internal functions evaluating binary comparisons
      /// @{
   private:
      template <class T_CompHelper>
      StackElement _comparisonOp(StackElement &other, T_CompHelper comp_helper);
   public:
      StackElement _eq(StackElement &other);
      StackElement _neq(StackElement &other);
      StackElement _and(StackElement &other);
      StackElement _or(StackElement &other);
      StackElement _gt(StackElement &other);
      StackElement _gte(StackElement &other);
      StackElement _lt(StackElement &other);
      StackElement _lte(StackElement &other);

      /// @}

      /// @name Internal functions evaluating mathematical operations
      /// @{

      /// Function raising the object's value to the n'th power
      template< typename T >
      StackElement _pow( const T& n );
      /// Function calculating a sum value
      StackElement _sum();
      /// Function counting elements
      StackElement _count();
      /// Function taking the absolute value of the object
      StackElement _abs();
      /// Function taking the square root of the object
      StackElement _sqrt();
      /// Function taking the cubic root of the object
      StackElement _cbrt();
      /// Function taking the sinus value of the object
      StackElement _sin();
      /// Function taking the cosine value of the object
      StackElement _cos();
      /// Function taking the tangent value of the object
      StackElement _tan();
      /// Function taking the arc sinus value of the object
      StackElement _asin();
      /// Function taking the arc cosine value of the object
      StackElement _acos();
      /// Function taking the arc tangent value of the object
      StackElement _atan();
      /// Function taking the sinus hyperbolic value of the object
      StackElement _sinh();
      /// Function taking the cosine hyperbolic value of the object
      StackElement _cosh();
      /// Function taking the tangent hyperbolic value of the object
      StackElement _tanh();
      /// Function taking the arc sinus hyperbolic value of the object
      StackElement _asinh();
      /// Function taking the arc cosine hyperbolic value of the object
      StackElement _acosh();
      /// Function taking the arc tangent hyperbolic value of the object
      StackElement _atanh();
      /// Function taking the logarithm of the object
      StackElement _log();
      /// Function taking the natural exponent of the object
      StackElement _exp();

      /// @}

   private:
      /// @name Internal operations on the object
      /// @{

      void makeVectorIfNecessary( const StackElement& other );
      void makeVectorIfNecessary( const std::vector< int >& other );
      void makeVectorIfNecessary( const std::vector< double >& other );

      void makeDoubleIfNecessary( const StackElement& other );
      void makeDoubleIfNecessary( int other );
      void makeDoubleIfNecessary( double other );
      void makeDoubleIfNecessary( const std::vector< int >& other );
      void makeDoubleIfNecessary( const std::vector< double >& other );

      void ensureCompatible( const StackElement& other ) const;
      template< typename T >
      void ensureCompatibleVectors( const T& other ) const;
      template< typename T >
      void ensureCompatibleVectors( const std::vector< T >& other ) const;

      size_t size() const;
      /// @}
 
      /// The type of the variable held by the object
      ElementType m_type = SE_UNK;

      /// The value of the object represented as an integer
      int m_intVal = 0;
      /// The value of the object represented as a double
      double m_doubleVal = 0.;
      /// The value of the object represented as a vector of integers
      std::vector< int > m_vecIntVal;
      /// The value of the object represented as a vector of doubles
      std::vector< double > m_vecDoubleVal;

      /// The name/definition of the variable
      std::string m_varName;
      /// Loader for the described variable
      IProxyLoader* m_proxyLoader = nullptr;
      /// Type of the variable provided by the proxy loader
      mutable std::atomic<IProxyLoader::VariableType> m_variableType = IProxyLoader::VT_UNK;
      /// Internal flag showing whether the type of the variable was already
      /// determined
      mutable std::atomic<bool> m_determinedVariableType = false;
      mutable std::atomic<bool> m_moved = false;

   }; // class StackElement

   /// Declare an output operator for StackElement objects
   std::ostream& operator<<( std::ostream& os, const StackElement& el );

} // namespace ExpressionParsing

/// Include the implementation of the template functions:
#include "ExpressionEvaluation/StackElement.icc"

#endif // EXPRESSIONEVALUATION_STACKELEMENT_H
