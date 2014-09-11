// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ScanForAbstract.h,v 1.3 2007-10-30 16:49:30 ssnyder Exp $

/**
 * @file  RootUtils/ScanForAbstract.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Work around Root problems with virtual bases.
 */

#ifndef ROOTUTILS_SCANFORABSTRACT_H
#define ROOTUTILS_SCANFORABSTRACT_H


class TClass;


namespace RootUtils {


/**
 * @brief Work around root problems with virtual bases.
 *
 * Root's @c TClass does not support virtual bases.
 *
 * Cint does in fact support virtual bases.  If a base is virtual,
 * then a flag is set; and in the location that would normally hold
 * the offset of the base class in the parent, we instead have
 * a pointer to a function which will compute this offset,
 * given an object instance.
 *
 * Root's @c TRealData structure does not, however, know about this.
 * It will happily use what it gets from Cint as a structure offset.
 * This will almost certainly lead to crashes later on.
 *
 * Since @c TRealData is what the I/O is based on, this is a problem.
 *
 * Fixing @c TClass/@c TRealData to properly represent virtual bases is
 * not trivial.  However, we observe that where such virtual bases
 * occur in the data model, they do not contain data members.
 *
 * Thus, we can work around the problem by finding all `abstract'
 * classes before @c MakeRealData runs on them, and filling @c fRealData
 * with an empty @c TList.  This then prevents @c MakeRealData from running,
 * thus preventing the crash.
 *
 * We identify classes as `abstract' by first looking for names
 * that start with `I' followed by an uppercase letter.
 * Then we test to see that it has the abstract property set,
 * and that it has no data members.
 *
 * On a call to @c initialize(), @c ScanForAbstract will scan all currently
 * known classes, and patch up any `abstract' ones it finds.  It will
 * further hook itself into class creation, so that it will automatically
 * scan any classes defined in the future.
 */
class ScanForAbstract
{
public:
  /**
   * @brief Initialize the workaround.
   * Scan all known classes, and also hook into class creation
   * so that we'll scan all future classes.
   */
  static void initialize();


  /**
   * @brief Scan a single class.
   * @param cls The class to scan.
   */
  static void scan_one (TClass* cls);
};


} // namespace RootUtils


#endif // not ROOTUTILS_SCANFORABSTRACT_H
