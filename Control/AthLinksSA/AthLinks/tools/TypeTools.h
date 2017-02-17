// Dear emacs, this is -*- c++ -*-
// $Id: TypeTools.h 572309 2013-11-25 16:22:38Z krasznaa $
#ifndef ATHLINKS_TOOLS_TYPETOOLS_H
#define ATHLINKS_TOOLS_TYPETOOLS_H

namespace TypeTools {

   /// Trait class defining how ElementLink should handle a container
   ///
   /// This type trait is used to make sure that ElementLink handles DataVector
   /// containers in a reasonable way. The code is taken directly from
   /// AthenaKernel/tools/type_tools.h.
   ///
   template< typename T >
   struct Copy {
      /// Type of the elements in the container
      typedef T type;
      /// Constant reference to an element in the container
      typedef const T& const_reference;
      /// Constant pointer to an element in the container
      typedef const T* const_pointer;
   };

   /// Specialisation for containers storing pointers
   ///
   template< typename T >
   struct Copy< T* > {
      /// Type of the elements in the container
      typedef const T* type;
      /// Constant reference to an element in the container
      typedef const T* const_reference;
      /// Constant pointer to an element in the container
      typedef const T* const* const_pointer;
   };

} // namespace TypeTools

#endif // ATHLINKS_TOOLS_TYPETOOLS_H
