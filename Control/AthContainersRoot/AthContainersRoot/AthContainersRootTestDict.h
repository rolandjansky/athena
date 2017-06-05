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


#include "AthContainers/tools/AuxTypeVectorFactory.h"


namespace AthContainersRootTest {


class Foo {};
class Bar {};
class Baz {};


} // namespace AthContainersRootTest


namespace SG {
template class AuxTypeVectorFactory<AthContainersRootTest::Foo>;
}


#endif // not ATHCONTAINERSROOT_ATHCONTAINERSROOTTESTDICT_H
