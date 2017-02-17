// Dear emacs, this is -*- c++ -*-
// $Id: ElementLinkVectorBase.h 567807 2013-10-30 09:30:16Z krasznaa $
#ifndef ATHLINKS_ELEMENTLINKVECTORBASE_H
#define ATHLINKS_ELEMENTLINKVECTORBASE_H

// System include(s):
extern "C" {
#   include <stdint.h>
}
#include <vector>

/**
 *  @short Base class holding the container independent part of ElementLinkVector
 *
 *         This class holds the ROOT persistifiable part of the
 *         ElementLinkVector<T> class. In the simplest form imaginable.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 567807 $
 * $Date: 2013-10-30 10:30:16 +0100 (Wed, 30 Oct 2013) $
 */
class ElementLinkVectorBase {

public:
   /// Default constructor
   ElementLinkVectorBase( const std::vector< uint32_t >& keys =
                          std::vector< uint32_t >(),
                          const std::vector< uint32_t >& indices =
                          std::vector< uint32_t >() );
   /// Virtual destructor, to make vtable happy...
   virtual ~ElementLinkVectorBase() {}

   /// Get the keys/identifiers of the referenced containers
   const std::vector< uint32_t >& persKeys() const;
   /// Set the keys/identifiers of the referenced containers
   void setPersKeys( const std::vector< uint32_t >& keys );

   /// Get the indices of the referenced objects in the containers
   const std::vector< uint32_t >& persIndices() const;
   /// Set the indices of the referenced objects in the containers
   void setPersIndices( const std::vector< uint32_t >& indices );

   /// Reset the object's internal cache
   virtual bool toTransient() = 0;

protected:
   /// Keys/identifiers of the referenced containers
   std::vector< uint32_t > m_persKeys;
   /// Indices of the referenced objects in the containers
   std::vector< uint32_t > m_persIndices;

   /// A transient variable signalling when direct I/O is happening
   ///
   /// This flag is used exclusively by the ElementLinkVector<>::toTransient()
   /// function to know when it should take the persistent data stored in this
   /// object into account. That function is called in normal P->T conversions
   /// as well, in which case using the data from this class would just reset
   /// the object to its default state.
   ///
   bool m_isDirectIO;

}; // class ElementLinkVectorBase

#endif // ATHLINKS_ELEMENTLINKVECTORBASE_H
