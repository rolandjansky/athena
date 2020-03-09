// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataWriteCnv/CLinksAOD_p1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief Persistent class for AOD CLinks container.  Used for thinning tests.
 */


#ifndef DATAMODELTESTDATAWRITECNV_CLINKSAOD_P1_H
#define DATAMODELTESTDATAWRITECNV_CLINKSAOD_P1_H


#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include <vector>


namespace DMTest {


/**
 * @brief Persistent class for AOD CLinks container.  Used for thinning tests.
 */
class CLinksAOD_p1
{
public:
  std::vector<ElementLinkInt_p3> m_vel;
  ElementLinkIntVector_p1 m_elv;
};


} // namespace DMTest


#endif // not DATAMODELTESTDATAWRITECNV_CLINKSAOD_P1_H
