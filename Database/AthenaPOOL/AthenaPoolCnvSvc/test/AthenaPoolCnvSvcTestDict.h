// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthenaPoolCnvSvcTestDict.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2016
 * @brief 
 */


#ifndef ATHENAPOOLCNVSVC_ATHENAPOOLCNVSVCTESTDICT_H
#define ATHENAPOOLCNVSVC_ATHENAPOOLCNVSVCTESTDICT_H


#include "AthContainers/DataVector.h"
#include "AthContainers/ViewVector.h"
#include "AthContainers/AuxElement.h"
#include "AthContainers/AuxStoreInternal.h"
#include "AthLinks/ElementLink.h"
#include "AthenaKernel/CLASS_DEF.h"
#include <vector>


namespace AthenaPoolCnvSvcTest {


class X
{
public:
  X(int a) : m_a(a) {}
  int m_a;
};
typedef DataVector<X> XCont;

class X_p1
{
public:
  X_p1(int a) : m_a(a) {}
  int m_a;
};
class XCont_p1
{
public:
  std::vector<X_p1> m_v;
};


class X_p2
{
public:
  X_p2(int a) : m_a(a) {}
  int m_a;
};
class XCont_p2
{
public:
  std::vector<X_p2> m_v;
};


struct Y_v1
  : public SG::AuxElement
{
  Y_v1(int a) : m_a(a) {}
  int m_a;
};
struct Y_v2
  : public SG::AuxElement
{
  Y_v2(int a) : m_a(a) {}
  ~Y_v2() {}
  int m_a;
};


struct YAuxCont_v1
  : public SG::AuxStoreInternal
{
};


struct YAuxCont_v2
  : public SG::AuxStoreInternal
{
};


} // namespace AthenaPoolCnvSvcTest


CLASS_DEF(DataVector<AthenaPoolCnvSvcTest::Y_v1>, 524263040, 1)
CLASS_DEF(DataVector<AthenaPoolCnvSvcTest::Y_v2>, 524263041, 1)
CLASS_DEF(ViewVector<DataVector<AthenaPoolCnvSvcTest::Y_v1> >, 524263042, 1)
CLASS_DEF(ViewVector<DataVector<AthenaPoolCnvSvcTest::Y_v2> >, 524263043, 1)
CLASS_DEF(AthenaPoolCnvSvcTest::YAuxCont_v1, 524263044, 1)
CLASS_DEF(AthenaPoolCnvSvcTest::YAuxCont_v2, 524263045, 1)
CLASS_DEF(AthenaPoolCnvSvcTest::XCont, 524263046, 1)


namespace AthenaPoolCnvSvcTest {


typedef std::vector<ElementLink<DataVector<Y_v1> > > YCont_v1_pers2;
typedef std::vector<ElementLink<DataVector<Y_v2> > > YCont_v2_pers2;



} // namespace AthenaPoolCnvSvcTest


#endif // not ATHENAPOOLCNVSVC_ATHENAPOOLCNVSVCTESTDICT_H
