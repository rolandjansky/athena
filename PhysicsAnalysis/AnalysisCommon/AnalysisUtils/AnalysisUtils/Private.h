/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ANALYSISUTILS_PRIVATE_H
#define ANALYSISUTILS_PRIVATE_H

/**
   private classes

   @author Tadashi Maeno
*/

namespace AnalysisUtils {

  namespace Private {
    
    // remove "const"
    template <class T> struct NonConst
    {
      typedef T Type;
    };
    
    template <class T> struct NonConst<const T>
    {
      typedef T Type;
    };

    // remove "pointer"
    template <class T> struct NonPointer
    {
      typedef T Type;
    };
    
    template <class T> struct NonPointer<T *>
    {
      typedef T Type;
    };

  } // end of Private
}

#endif
