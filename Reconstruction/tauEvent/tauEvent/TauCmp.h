/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCmp.h
///
///Declaration of Tau comparison predicates
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess
///
/// Created by Thomas Burgess on 2009-06-04.

#ifndef __TauCmp_h__
#define __TauCmp_h__

#include <vector>
#include <algorithm>

namespace Analysis {
    namespace TauCmp {
        ///Comparison predicate
        ///
        /// Note: put specialisations for all types using cmp in TauCmp.cxx
        ///
        ///@param a left hand side argument
        ///@param b right hand side argument
        ///@return true if equal
        template <typename T> bool cmp(const T& a, const T &b);

        ///Compare vectors using cmp predicate
        ///@param a left hand side argument
        ///@param b right hand side argument
        ///@return true if equal
        template <typename T> bool vcmp(
                const std::vector<T> &a, const std::vector<T> &b)
        {
            return equal(a.begin(),a.end(),b.begin(),cmp<T>);
        }

        ///Compare vectors using vcmp predicate
        ///@param a left hand side argument
        ///@param b right hand side argument
        ///@return true if equal
        template <typename T> bool vvcmp(
                const std::vector<std::vector<T> > &a,
                const std::vector<std::vector<T> > &b) 
        {
            return equal(a.begin(),a.end(),b.begin(),vcmp<T>);
        }   
    }
}
#endif //__TauCmp_h__
