// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: DataModelTestDataCommonDict.h,v 1.2 2005-12-01 19:07:55 ssnyder Exp $

/**
 * @file  DataModelTestDataCommon/DataModelTestDataCommonDict.h
 * @author snyder@bnl.gov
 * @date Nov 2005
 * @brief Dictionary generation header.
 */

#ifndef DATAMODELTESTDATACOMMONDICT_H
#define DATAMODELTESTDATACOMMONDICT_H


#include "DataModelTestDataCommon/B.h"
#include "DataModelTestDataCommon/D.h"
#include "DataModelTestDataCommon/BAux.h"
#include "DataModelTestDataCommon/BAuxStandalone.h"
#include "DataModelTestDataCommon/BAuxVec.h"
#include "DataModelTestDataCommon/versions/C_v1.h"
#include "DataModelTestDataCommon/versions/CVec_v1.h"
#include "DataModelTestDataCommon/versions/CAuxContainer_v1.h"
#include "DataModelTestDataCommon/versions/CTrigAuxContainer_v1.h"
#include "DataModelTestDataCommon/versions/CInfoAuxContainer_v1.h"
#include "DataModelTestDataCommon/setConverterLibrary.h"
#include "AthLinks/ElementLink.h"


namespace DataModelTestDataCommonDict {

std::vector<DMTest::B> xa;
std::vector<ElementLink<DMTest::CVec_v1> > xb;

}

template class SG::AuxElement::ConstAccessor<ElementLink<DMTest::CVec_v1> >;


#endif // not DATAMODELTESTDATACOMMONDICT_H
