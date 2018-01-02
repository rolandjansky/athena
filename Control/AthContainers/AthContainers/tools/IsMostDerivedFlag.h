// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/tools/IsMostDerivedFlag.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2013
 * @brief Holder for the is-most-derived flag.
 */


#ifndef ATHCONTAINERS_ISMOSTDERIVEDFLAG_H
#define ATHCONTAINERS_ISMOSTDERIVEDFLAG_H


namespace SG {


/**
 * @brief Holder for the is-most-derived flag.
 *
 * This acts like a bool, except that when the class containing it
 * gets copied via a copy/move constructor, it is initialized to false.
 * This allows us to maintain the flags when the containers have
 * only implicit copy constructors for the derived classes.  After
 * copy construction, the flags will get set correctly by the
 * first call to @c testInsert.
 */
class IsMostDerivedFlag
{
public:
  IsMostDerivedFlag() : m_flag(false) {}
  IsMostDerivedFlag(bool flag) : m_flag(flag) {}
  IsMostDerivedFlag(const IsMostDerivedFlag&) : m_flag(false) {}
  IsMostDerivedFlag(IsMostDerivedFlag&&) : m_flag(false) {}
  operator bool() const { return m_flag; }
  IsMostDerivedFlag& operator= (const IsMostDerivedFlag&) { return *this; }
  IsMostDerivedFlag& operator= (bool flag) { m_flag = flag; return *this;}

private:
  bool m_flag;
};


} // namespace SG


#endif // not ATHCONTAINERS_ISMOSTDERIVEDFLAG_H
