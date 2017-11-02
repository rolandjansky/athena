// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ELVec.h,v 1.4 2008-11-21 06:31:28 ssnyder Exp $

/**
 * @file  DataModelTestDataWrite/ELVec.h
 * @author snyder@bnl.gov
 * @date May 2007
 * @brief Class used for testing new @c ElementLink.
 */

#ifndef DATAMODELTESTDATAWRITE_ELVEC_H
#define DATAMODELTESTDATAWRITE_ELVEC_H


#include "DataModelTestDataWrite/BVec.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"
#include "AthLinks/DataLink.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "DataModelAthenaPool/ElementLink_p3.h"
#include "DataModelAthenaPool/DataLink_p1.h"
#include "AthenaKernel/CLASS_DEF.h"


namespace DMTest {


/**
 * @brief For testing @c ElementLink.
 */
struct ELVec
{
  std::vector<ElementLink<BVec> > m_el;
  std::vector<DataLink<BVec> > m_dl;
  ElementLinkVector<BVec> m_elv;

  ElementLinkVector<BVec> m_elv2;    // Transient
  ElementLinkIntVector_p1 m_elv2_p;

  std::vector<ElementLink<BVec> > m_el2; // Transient
  std::vector<ElementLinkInt_p3> m_el2_p;

  std::vector<DataLink<BVec> > m_dl2; // Transient
  std::vector<DataLink_p1> m_dl2_p;
};


}


CLASS_DEF (DMTest::ELVec, 9639, 1)


#endif // not DATAMODELTESTDATAWRITE_ELVEC_H
