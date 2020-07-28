// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ATHLINKS_ELEMENTLINKBASE_H
#define ATHLINKS_ELEMENTLINKBASE_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

/**
 *  @short A base class holding the container independent part of ElementLink
 *
 *         For technical reasons it's a good idea to define a common base
 *         class for the ElementLink classes, it helps with ROOT I/O immensely.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 567807 $
 * $Date: 2013-10-30 10:30:16 +0100 (Wed, 30 Oct 2013) $
 */
class ElementLinkBase {
private:
  /// Value to mark a default index
  static constexpr uint32_t INVALID = static_cast<uint32_t> (-1);
public:
   /// Default constructor
   ElementLinkBase( uint32_t key = 0, uint32_t index = INVALID );
   /// Destructor
   virtual ~ElementLinkBase() {}

   /// Get the key/identifier of the referenced container
   uint32_t persKey() const;
   /// Set the key/identifier of the referenced container
   void setPersKey( uint32_t key );

   /// Get the index of the referenced object in the container
   uint32_t persIndex() const;
   /// Set the index of the referenced object in the container
   void setPersIndex( uint32_t index );

   /// Reset the object's internal cache
   virtual bool toTransient() = 0;

   /**
    * @brief Is the link in the default state
    *
    * A link in the default state was purposefully constructed as a null link
    */
   bool isDefault() const;

protected:
   /// Key/identifier of the referenced container
   uint32_t m_persKey;
   /// Index of the referenced object in the container
   uint32_t m_persIndex;

   /// A transient variable signalling when direct I/O is happening
   ///
   /// This flag is used exclusively by the ElementLink<>::toTransient()
   /// function to know when it should take the persistent data stored in this
   /// object into account. That function may be called in normal P->T
   /// conversions as well, in which case using the data from this class would
   /// just reset the object to its default state.
   ///
   bool m_isDirectIO;

}; // class ElementLinkBase

#endif // ATHLINKS_ELEMENTLINKBASE_H
