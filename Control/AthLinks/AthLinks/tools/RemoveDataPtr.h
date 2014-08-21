/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHLINKS_TOOLS_REMOVEDATAPTR_H
# define ATHLINKS_TOOLS_REMOVEDATAPTR_H
/***************************************************************************
 helper struct to treat pointers and DataPtr uniformly
 -----------------------------------------------------
 ATLAS Collaboration
 ***************************************************************************/

// $Id: RemoveDataPtr.h,v 1.1 2003-04-07 23:58:32 calaf Exp $

//<<<<<< INCLUDES                                                       >>>>>>

#include "AthLinks/DataPtr.h"

//namespace type_tools {
  template <typename T>
  struct RemoveDataPtr {
    typedef T type;
  };
  template <typename T>
  struct RemoveDataPtr<DataPtr<T> > {
    typedef typename DataPtr<T>::DataPtr_type* type;
  };
//}

#endif // ATHLINKS_TOOLS_REMOVEDATAPTR_H
