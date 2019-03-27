// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file  RootUtils/PyROOTTypePatch.h
 * @author scott snyder
 * @date Mar 2019
 * @brief Work around pyroot problem with __pair_base.
 */

#ifndef ROOTUTILS_PYROOTTYPEPATCH_H
#define ROOTUTILS_PYROOTTYPEPATCH_H


#include "CxxUtils/checker_macros.h"


class TClass;


namespace RootUtils {


/**
 * @brief Work around pyroot problem with __pair_base.
 *
 * In gcc8, std::pair derives from the (empty) base class std::__pair_base.
 * When ROOT makes dictionaries, it likes to strip std:: qualifiers.
 * When it then tries to look up cling information for a TClass,
 * if the lookup fails, it tries again with the std:: reinserted
 * for all STL classes (with TClassEdit::InsertStd()).
 * However, that doesn't work for __pair_base, because it's not a class
 * that ROOT knows about.  The upshot is that we see errors like:
 *
 *   Error in <TClass::LoadClassInfo>: no interpreter information for class __pair_base<unsigned int,string> is available even though it has a TClass initialization routine.
 *
 * We work around this by hooking into ROOT's TClass creation.
 * If we see a std::pair class, we erase the base class information
 * from its TClass.
 */
class ATLAS_NOT_THREAD_SAFE PyROOTTypePatch
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


#endif // not ROOTUTILS_PYROOTTYPEPATCH_H
