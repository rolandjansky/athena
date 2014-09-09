// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: addDVProxy.cxx,v 1.2 2008-06-04 14:52:59 ssnyder Exp $

/**
 * @file AthenaROOTAccess/src/addDVProxy.cxx
 * @author scott snyder
 * @date Apr 2008
 * @brief Helper: Set up collection proxy for a @c DataVector class.
 */


#include "AthenaROOTAccess/addDVProxy.h"
#include "AthenaROOTAccess/DVCollectionProxy.h"
#include "TClass.h"
#include "TBaseClass.h"
#include "TList.h"
#include <string>


namespace AthenaROOTAccess {


/**
 * @brief Helper: Find the element type of a @c DataVector class.
 * @param cl The @c TClass to test.
 * @return The name of the element type if @a cl is a DV class, or empty.
 *
 * If @a cl is a @c DataVector class, or derives from one, then we return
 * the name of the element type (including a trailing `*').  Otherwise,
 * we return an empty string.
 */
std::string find_dv_elt (TClass* cl)
{
  // Is this a DV class?
  std::string elt;
  const char* name = cl->GetName();
  if (strncmp (name, "DataVector<", 11) == 0) {
    elt = std::string(name+11, strlen(name)-11-1) + "*";
    return elt;
  }

  // Scan base classes too.
  TIter next (cl->GetListOfBases());
  while (TBaseClass* bcl = dynamic_cast<TBaseClass*>(next())) {
    TClass* cc = bcl->GetClassPointer();
    elt = find_dv_elt (cc);
    if (!elt.empty())
      break;
  }
  return elt;
}


/**
 * @brief Helper: Set up collection proxy for a @c DataVector class.
 * @param cl The class to test.
 *
 * If @a cl is a @c DataVector class, or derives from one, then we attach
 * an appropriate Root collection proxy to it.  This is required for Root
 * to recognize the class as a collection in, eg, @c TTree::Draw.
 */
void addDVProxy::add (TClass* cl)
{
  if (!cl) return;
  std::string eltname = find_dv_elt (cl);
  if (eltname.empty())
    return;

  DVCollectionProxy prox (eltname.c_str(), cl->GetName());
  cl->CopyCollectionProxy (prox);
}


} // namespace AthenaROOTAccess
