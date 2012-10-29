// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: binary_compose.h 457114 2011-09-05 09:35:49Z krasznaa $
#ifndef TRIGBUNCHCROSSINGTOOL_BINARY_COMPOSE_H
#define TRIGBUNCHCROSSINGTOOL_BINARY_COMPOSE_H

namespace Trig {

   /**
    *  @short Function adaptor for combining function objects
    *
    *         I got this function adaptor from the STL documentation, because
    *         it doesn't seem to be present in recent versions of GCC.
    *
    *         What it does is that if you call it with a binary function
    *         "bfn( X, Y )", and two unary functions "x1( X )" and "x2( X )",
    *         then internally it will evaluate as
    *
    *         <code>
    *         bfn( x1( X ), x2( X ) )
    *         </code>
    *
    *         The concept may be a bit tricky, but have a look at the STL
    *         documentation...
    *
    *         This class is not actually used by the code anymore, but I didn't
    *         just want to throw it out, as it might come handy later on.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   template< class BinaryFunction,
             class UnaryFunction1,
             class UnaryFunction2 >
   class binary_compose {

   public:
      binary_compose( BinaryFunction bfn, UnaryFunction1 ufn1,
                      UnaryFunction2 ufn2 )
         : m_bfn( bfn ), m_ufn1( ufn1 ), m_ufn2( ufn2 ) {}

      typename BinaryFunction::result_type
      operator()( const typename UnaryFunction1::argument_type& x ) {
         return m_bfn( m_ufn1( x ), m_ufn2( x ) );
      }

   private:
      BinaryFunction m_bfn;
      UnaryFunction1 m_ufn1;
      UnaryFunction2 m_ufn2;
   }; // class binary_compose

   /**
    *  @short Helper function for creating binary_compose function objects
    *
    *         This function is here because when you call it, you don't need
    *         to explicitly specify the template parameter types. When you
    *         create a binary_compose object directly, you do have to.
    *
    * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
    *
    * $Revision: 457114 $
    * $Date: 2011-09-05 11:35:49 +0200 (Mon, 05 Sep 2011) $
    */
   template< class BinaryFunction,
             class UnaryFunction1,
             class UnaryFunction2 >
   binary_compose< BinaryFunction, UnaryFunction1, UnaryFunction2 >
   compose2( BinaryFunction bfn, UnaryFunction1 ufn1, UnaryFunction2 ufn2 ) {

      return binary_compose< BinaryFunction, UnaryFunction1,
                             UnaryFunction2 >( bfn, ufn1, ufn2 );
   }

} // namespace Trig

#endif // TRIGBUNCHCROSSINGTOOL_BINARY_COMPOSE_H
