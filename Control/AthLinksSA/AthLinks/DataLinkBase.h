// Dear emacs, this is -*- c++ -*-
/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */
#ifndef ATHLINKS_DATALINKBASE_H
#define ATHLINKS_DATALINKBASE_H

// System include(s):
extern "C" {
#   include <stdint.h>
}

/**
 *  @file AthLinksSA/AthLinks/DataLinkBase.h
 *  @short A base class holding the object independent part of DataLink
 *
 *         To implement efficient ROOT I/O for DataLink<T> classes, they all
 *         store their persistent information in this common base class,
 *         and DataLink<T> just holds transient variables.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 *
 * $Revision: 602641 $
 * $Date: 2014-06-19 15:05:08 +0200 (Thu, 19 Jun 2014) $
 */
class DataLinkBase {

public:
   /// Default constructor
   DataLinkBase( uint32_t key = 0 );
   /// Destructor
   virtual ~DataLinkBase() {}

   /// Get the key/identifier of the referenced container
   uint32_t persKey() const;
   /// Set the key/identifier of the referenced container
   void setPersKey( uint32_t key );

   /// Test if the link is in the default state
   bool isDefault() const;

   /// Clear the link (make it null)
   void clear();

   /// Reset the object's internal cache
   virtual bool toTransient() = 0;

protected:
   /// Key/identifier of the referenced object
   uint32_t m_persKey;

   /// A transient variable signalling when direct I/O is happening
   ///
   /// This flag is used exclusively by the DataLink<>::toTransient()
   /// function to know when it should take the persistent data stored in this
   /// object into account. That function may be called in normal P->T
   /// conversions as well, in which case using the data from this class would
   /// just reset the object to its default state.
   ///
   bool m_isDirectIO;

}; // class DataLinkBase

#endif // ATHLINKS_DATALINKBASE_H
