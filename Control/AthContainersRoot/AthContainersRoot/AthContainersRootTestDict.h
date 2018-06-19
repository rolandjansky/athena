// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainersRoot/AthContainersRootTestDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2016
 * @brief For AthContainersRoot unit tests.
 */


#ifndef ATHCONTAINERSROOT_ATHCONTAINERSROOTTESTDICT_H
#define ATHCONTAINERSROOT_ATHCONTAINERSROOTTESTDICT_H


#include "AthContainersRoot/test/Foo.h"
#include "AthContainers/tools/AuxTypeVectorFactory.h"
#include "AthLinks/ElementLink.h"
#include <vector>


namespace AthContainersRootTest {


class Bar {};
class Baz {};


} // namespace AthContainersRootTest


template class std::vector<AthContainersRootTest::Foo*>;
template class ElementLink<std::vector<AthContainersRootTest::Foo*> >;
template class std::vector<ElementLink<std::vector<AthContainersRootTest::Foo*> > >;
template class std::vector<std::vector<ElementLink<std::vector<AthContainersRootTest::Foo*> > > >;

template class std::vector<AthContainersRootTest::Foo>;
template class ElementLink<std::vector<AthContainersRootTest::Foo> >;
template class std::vector<ElementLink<std::vector<AthContainersRootTest::Foo> > >;
template class std::vector<std::vector<ElementLink<std::vector<AthContainersRootTest::Foo> > > >;

namespace SG {
template class AuxTypeVectorFactory<AthContainersRootTest::Foo>;
}


#endif // not ATHCONTAINERSROOT_ATHCONTAINERSROOTTESTDICT_H
