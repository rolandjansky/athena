// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TDestructor.h 599851 2014-06-02 12:32:51Z krasznaa $
#ifndef XAODROOTACCESS_TOOLS_TDESTRUCTOR_H
#define XAODROOTACCESS_TOOLS_TDESTRUCTOR_H

namespace xAOD {

   /// Base class for the templated destructor types
   ///
   /// When recording an object into the transient store that doesn't
   /// have a dictionary available, we still need a way of deleting this
   /// object correctly when the store is cleared. That is done by using an
   /// object of this type on the object's pointer.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 599851 $
   /// $Date: 2014-06-02 14:32:51 +0200 (Mon, 02 Jun 2014) $
   ///
   class TVirtualDestructor {

   public:
      /// Virtual destructor (just realising the pun...), to make vtable happy
      virtual ~TVirtualDestructor() {}

      /// Function that deletes an object of a given type from memory
      virtual void destruct( void* ptr ) const = 0;

   }; // class TVirtualDestructor

   /// Implementation of the destructor base class for a given type
   ///
   /// This template implementation is used to delete objects of any given
   /// type from memory, for which we don't have a dictionary.
   ///
   /// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
   ///
   /// $Revision: 599851 $
   /// $Date: 2014-06-02 14:32:51 +0200 (Mon, 02 Jun 2014) $
   ///
   template< class T >
   class TDestructor : public TVirtualDestructor {

   public:
      /// Function that deletes an object of this type from memory
      virtual void destruct( void* ptr ) const;

   }; // class TDestructor

} // namespace xAOD

// Include the code's implementation:
#include "TDestructor.icc"

#endif // XAODROOTACCESS_TOOLS_TDESTRUCTOR_H
