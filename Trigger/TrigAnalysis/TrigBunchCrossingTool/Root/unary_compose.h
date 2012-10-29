// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: unary_compose.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_UNARY_COMPOSE_H
#define TRIGBUNCHCROSSINGTOOL_UNARY_COMPOSE_H

namespace Trig {

   /**
    *  @short Function adaptor for combining function objects
    *
    *         I got this function adaptor from the STL documentation, because
    *         it doesn't seem to be present in recent versions of GCC.
    *
    *         What it does is that if you call it with two unary functions
    *         "x1( X )" and "x2( Y )", this function object evaluates internally
    *         as
    *
    *         <code>
    *         x1( x2( X ) )
    *         </code>
    *
    *         This can be a very useful thing to have when using the STL functionals
    *         and algorithms.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   template< class UnaryFunction1,
             class UnaryFunction2 >
   class unary_compose {

   public:
      unary_compose( UnaryFunction1 ufn1, UnaryFunction2 ufn2 )
         : m_ufn1( ufn1 ), m_ufn2( ufn2 ) {}

      typename UnaryFunction1::result_type
      operator()( const typename UnaryFunction2::argument_type& x ) {
         return m_ufn1( m_ufn2( x ) );
      }

   private:
      UnaryFunction1 m_ufn1;
      UnaryFunction2 m_ufn2;

   }; // class unary_compose

   /**
    *  @short Helper function for creating unary_compose function objects
    *
    *         This function is here because when you call it, you don't need
    *         to explicitly specify the template parameter types. When you
    *         create a unary_compose object directly, you do have to.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   template< class UnaryFunction1,
             class UnaryFunction2 >
   unary_compose< UnaryFunction1, UnaryFunction2 >
   compose1( UnaryFunction1 ufn1, UnaryFunction2 ufn2 ) {

      return unary_compose< UnaryFunction1,
                            UnaryFunction2 >( ufn1, ufn2 );
   }

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_UNARY_COMPOSE_H
