// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/CLinksAOD.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of AOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSAOD_H
#define DATAMODELTESTDATACOMMON_CLINKSAOD_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"


namespace DMTest {


/**
 * @brief For testing thinning of AOD objects.
 *
 * A CLinks object holds both a vector<ElementLink> and an ElementLinkVector
 * to CVec objects.
 */
class CLinksAOD
{
public:
  // Get/set the vector<ElementLink>.
  const std::vector<ElementLink<CVec> >& vel() const;
  void setVel (const std::vector<ElementLink<CVec> >& vel);
  void setVel (std::vector<ElementLink<CVec> >&& vel);

  // Get/set the ElementLinkVector.
  const ElementLinkVector<CVec>& elv() const;
  ElementLinkVector<CVec>& elv();
  void setElv (const ElementLinkVector<CVec>& elv);


private:
  std::vector<ElementLink<CVec> > m_vel;
  ElementLinkVector<CVec> m_elv;
};


} // namespace DMTest


#include "xAODCore/CLASS_DEF.h"
CLASS_DEF (DMTest::CLinksAOD, 28651380, 1)


#endif // not DATAMODELTESTDATACOMMON_CLINKSAOD_H
