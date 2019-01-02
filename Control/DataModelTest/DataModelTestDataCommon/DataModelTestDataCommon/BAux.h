// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file DataModelTestDataCommon/BAux.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2014
 * @brief Class used for testing aux data reading/writing.
 */


#ifndef DATAMODELTESTDATACOMMON_BAUX_H
#define DATAMODELTESTDATACOMMON_BAUX_H


#include "AthContainers/AuxElement.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


struct BAux
  : public SG::AuxElement
{
  BAux (int b = 0) : m_x (b) {}
  int m_x;
};


} // namespace DMTest


CLASS_DEF (DMTest::BAux, 9702, 1)


#endif // not DATAMODELTESTDATACOMMON_BAUX_H
