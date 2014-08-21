// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: IAuxElement.h 591267 2014-04-04 10:12:14Z krasznaa $
/**
 * @file AthContainersInterfaces/IAuxElement.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Flag that a class may have auxiliary data associated with it.
 */


#ifndef ATHCONTAINERSINTERFACES_IAUXELEMENT_H
#define ATHCONTAINERSINTERFACES_IAUXELEMENT_H


namespace SG {


/**
 * @brief Flag that a class may have auxiliary data associated with it.
 *
 * This is an empty class, existing only to flag that a class may
 * have auxiliary data associated with it.  @c SG::AuxElement derives
 * from this, but one should use this class for tests rather
 * than @c SG::AuxElement to avoid dependency issues.
 *
 * This is an `interface' in the sense that it identifies a group of classes,
 * but it defines no functionality itself.  In fact, this class should
 * not have a virtual table.
 */
class IAuxElement
{
public:
   IAuxElement() : m_dummy( 0 ) { ++m_dummy; }
private:
   int m_dummy; ///< A temporary variable needed to make ROOT I/O work...
};


} // namespace SG


#endif // not ATHCONTAINERSINTERFACES_IAUXELEMENT_H
