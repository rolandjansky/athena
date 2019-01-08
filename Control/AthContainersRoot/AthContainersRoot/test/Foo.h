// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file AthContainersRoot/Foo.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2018
 * @brief 
 */


#ifndef ATHCONTAINERSROOT_FOO_H
#define ATHCONTAINERSROOT_FOO_H


#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


namespace AthContainersRootTest {


class Foo
{
public:
  Foo(int the_x=0) : x(the_x) {}
  int x;
  bool operator== (const Foo& other) const { return x == other.x; }
  bool operator!= (const Foo& other) const { return x != other.x; }
};


} // namespace AthContainersRootTest


CLASS_DEF( std::vector<AthContainersRootTest::Foo*>, 1323163190, 0 )
CLASS_DEF( std::vector<AthContainersRootTest::Foo>,  1323163191, 0 )

#endif // not ATHCONTAINERSROOT_FOO_H
