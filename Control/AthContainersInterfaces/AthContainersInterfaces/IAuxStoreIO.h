// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxStoreIO.h 584348 2014-02-20 09:21:15Z krasznaa $
/**
 * @file AthContainersInterfaces/IAuxStoreIO.h
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @date Aug, 2013
 * @brief Interface providing I/O for a generic auxiliary store.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXSTOREIO_H
#define ATHCONTAINERSINTERFACES_IAUXSTOREIO_H


#include "AthContainersInterfaces/AuxTypes.h"
#include <typeinfo>
#include <set>


namespace SG {


/**
 *  @brief Interface providing I/O for a generic auxiliary store
 *
 *         In order to read/write the properties stored in a generic
 *         auxiliary store object, this interface needs to be used.
 *         It allows the infrastructure code to inspect the contents
 *         of the object, and access it in a way that allows the
 *         information to be passed to the persistency system.
 *
 * @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
 * @author Marcin Nowak         <Marcin.Nowak@cern.ch>
 *
 * $Revision: 584348 $
 * $Date: 2014-02-20 10:21:15 +0100 (Thu, 20 Feb 2014) $
 */
class IAuxStoreIO
{
public:
  /// Destructor.
  virtual ~IAuxStoreIO() {}


  /**
   * @brief Return a pointer to the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   *
   * This will usually be a pointer to a @c std::vector; however, it may
   * be something different for a standalone object.
   *
   * Returns 0 and reports an error if the requested aux data item
   * does not exist.
   */
  virtual const void* getIOData (SG::auxid_t auxid) const = 0;


  /**
   * @brief Return the type of the data to be stored for one aux data item.
   * @param auxid The identifier of the desired aux data item.
   *
   * For an aux data item of type @c T, this will usually be
   * @c std::vector<T>.  For standalone objects, however, it will
   * usually be @c T; and @c std::vector<char> will be used instead
   * of @c std::vector<bool>.
   *
   * Returns 0 if the requested aux data item does not exist.
   */
  virtual const std::type_info* getIOType (SG::auxid_t auxid) const = 0;


  /**
   * @brief Get the list of all dynamically created variables.
   */
  virtual const SG::auxid_set_t& getDynamicAuxIDs() const = 0;


   /**
    * @brief Get a list of dynamic variables that need to be written out.
    */
  virtual SG::auxid_set_t getSelectedAuxIDs() const {
     // default all are selected
     return getDynamicAuxIDs();
  }
  
}; // class IAuxStoreIO


} // namespace SG


#ifndef XAOD_STANDALONE
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( SG::IAuxStoreIO, 55879233, 1 )
#endif


#endif // not ATHCONTAINERSINTERFACES_IAUXSTOREIO_H
