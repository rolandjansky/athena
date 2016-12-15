// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: StdHackGenerator.h,v 1.4 2008-03-20 15:30:00 ssnyder Exp $

/**
 * @file  RootUtils/StdHackGenerator.h
 * @author scott snyder
 * @date Oct 2007
 * @brief Work around inconsistent use of @c std:: and spaces.
 */

#ifndef ROOTUTILS_STDHACKGENERATOR_H
#define ROOTUTILS_STDHACKGENERATOR_H

#include "TClassGenerator.h"


namespace RootUtils {


/**
 * @brief Work around inconsistent use of @c std:: and spaces.
 *
 * Sometimes, when Athena is requesting a class to autoload (via @c GetClass),
 * the name that's requested omits some of the @c std:: qualifications
 * in front of the STL names.  One usually finds, though, that the
 * autoload database contains the full @c std::-qualified names.  Thus,
 * the autoload fails.
 *
 * For example, we may be asked to load a class with a name like
 *@code
 *   Navigable<foo,vector<...> >
 @endcode
 * but what's in the rootmap file is
 *@code
 *   Navigable<foo,std::vector<...> >
 @endcode
 *
 * Root does provide a way to hook into class autoloading ---
 * a @c TClassGenerator, which is called when autoloading fails.
 * Thus, we create an autoloader such that when autoloading fails,
 * it tries again with any missing @c std:: qualifications reinserted.
 *
 * With 5.18, we run into a similar problem with pairs of names like
 * __gnu_cxx::__normal_iterator<HepMC::GenParticle*const*,vector<HepMC::GenParticle*> >
 * and
 * __gnu_cxx::__normal_iterator<HepMC::GenParticle* const*,vector<HepMC::GenParticle*> >
 *
 * We do a fixup for this case too.
 *
 * ??? FIXME: Find out why the bad names are being requested
 * in the first place.
 *
 * There is yet another dictionary pathology which has sometimes been observed.
 * This was seen with vector<vector<unsigned int> > and TMVA.
 * Suppose we first load the reflex dictionary for this type.
 * The TClass will point at a G__ClassInfo that has the ISCPPCOMPILED
 * bit set.
 * Suppose we then load a library such as TMVA which references this
 * class via a rootcint dictionary.
 * In this case, the class name we have will have the `allocator'
 * arguments filled in.  The Cint dictionary initialization code
 * will get a tagnum for this class; because the name is different
 * than before, we'll get a different tagnum.
 * The Cint dictionary initialization code will then call
 * TCint::UpdateClassInfo via a hook.  This will then resolve the
 * class name back to the short one.  It will see that the tagnum
 * for the TClass is changing.  As a result, the G__ClassInfo for the
 * TClass will be overwritten with the new tagnum.  The ISCPPCOMPILED
 * property will also be cleared.  This means that when we try
 * to create one of these vectors, we won't call the constructor,
 * we'll just get the result of calling new for a block of the
 * proper size --- and it won't be zero-initialized.
 *
 * So, what we do is to interpose ourselves in the UpdateClassInfo hook.
 * If we're replacing a ISCPPCOMPILED class with one which isn't,
 * and the new class had a name that was edited, then we don't allow
 * the change.
 */
class StdHackGenerator
  : public TClassGenerator
{
public:
  // un-hide this method...
  using TClassGenerator::GetClass;

  /**
   * @brief Initialize.
   *        Create an instance of this class and register it as a generator.
   */
  static void initialize();


  /**
   * @brief Look up a class by name.
   * @param classname The name of the class to find.
   * @param load If true, enable autoloading.
   */
  virtual TClass *GetClass(const char* classname, Bool_t load);


  /**
   * @brief Look up a class by @c type_info.
   * @param typeinfo The @c type_info of the class to find.
   * @param load If true, enable autoloading.
   */
  virtual TClass *GetClass(const std::type_info& typeinfo, Bool_t load);


  /**
   * @brief Look up a class by name.
   * @param classname The name of the class to find.
   * @param load If true, enable autoloading.
   * @param silent If true, suppress messages.
   */
  virtual TClass *GetClass(const char* classname,
                           Bool_t load,
                           Bool_t silent);


  /**
   * @brief Look up a class by @c type_info.
   * @param typeinfo The @c type_info of the class to find.
   * @param load If true, enable autoloading.
   * @param silent If true, suppress messages.
   */
  virtual TClass *GetClass(const std::type_info& typeinfo,
                           Bool_t load,
                           Bool_t silent);

  ClassDef (RootUtils::StdHackGenerator, 1);
};


} // namespace RootUtils


#endif // not ROOTUTILS_STDHACKGENERATOR_H
