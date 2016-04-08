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
#include "DataModelTestDataCommon/C.h"
#include "DataModelTestDataCommon/CVec.h"
#include "DataModelTestDataCommon/CVecWithData.h"
#include "DataModelTestDataCommon/CView.h"
#include "DataModelTestDataCommon/CAuxContainer.h"
#include "DataModelTestDataCommon/CTrigAuxContainer.h"
#include "DataModelTestDataCommon/CInfoAuxContainer.h"
#include "DataModelTestDataCommon/setConverterLibrary.h"
#include "AthLinks/ElementLink.h"


namespace DataModelTestDataCommonDict {

std::vector<DMTest::B> xa;
DMTest::CView::Pers_t xb;
std::vector<ElementLink<DMTest::BAuxVec> > xc;

}

template class SG::AuxElement::ConstAccessor<ElementLink<DMTest::CVec> >;

#endif // not DATAMODELTESTDATACOMMONDICT_H
