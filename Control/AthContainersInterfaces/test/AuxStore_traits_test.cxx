/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AuxStore_traits_test.cxx 627017 2014-11-07 21:46:42Z ssnyder $
/**
 * @file AthContainersInterfaces/test/AuxStore_traits_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression test for AuxStore_traits.
 */


#undef NDEBUG
#include "AthContainersInterfaces/AuxStore_traits.h"
#include "AthContainersInterfaces/CLASS_AUXSTORE.h"
#include <iostream>
#include <vector>


void whichClass (const SG::NoAuxStore*) { std::cout << "SG::NoAuxStore "; }
void whichClass (const SG::IAuxStore*) { std::cout << "SG::IAuxStore "; }
void whichClass (const SG::IConstAuxStore*) { std::cout << "SG::IConstAuxStore "; }



template <class DOBJ>
void testit (const std::string& name)
{
  typedef SG::AuxStore_traits<DOBJ> traits;
  std::cout << name << " ";
  whichClass ((typename traits::type*)0);
  whichClass ((typename traits::const_type*)0);
  std::cout << traits::typeName() << " " << traits::const_typeName() << "\n";
}


template <class T>
struct DV
{
  typedef T base_value_type;
};


struct D : public SG::IAuxElement {};


struct W {};
CLASS_AUXSTORE(W)

struct X {};
struct Y {};
struct Z {};
CLASS_AUXSTORE3(X, Y, Z)


void whichClass (const Y*) { std::cout << "Y "; }
void whichClass (const Z*) { std::cout << "Z "; }


int main()
{
  testit<int> ("int");
  testit<std::vector<int> > ("vector<int>");
  testit<DV<int> > ("DV<int>");
  testit<D> ("D");
  testit<DV<D> > ("DV<D>");
  testit<W> ("W");
  testit<X> ("X");
  testit<W*> ("W*");
  testit<X*> ("X*");

  return 0;
}
