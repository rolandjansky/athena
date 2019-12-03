// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/versions/CLinks_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKS_V1_H
#define DATAMODELTESTDATACOMMON_CLINKS_V1_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthContainers/AuxElement.h"
#include "AthenaKernel/BaseInfo.h"


namespace DMTest {


/**
 * @brief For testing thinning of xAOD objects.
 *
 * A CLinks object holds both an ElementLink and a vector<ElementLink>
 * to CVec objects.
 */
class CLinks_v1
  : public SG::AuxElement
{
public:
  // Get/set the single link.
  const ElementLink<CVec>& link() const;
  void setLink (const ElementLink<CVec>& l);

  // Get/set the link vector.
  const std::vector<ElementLink<CVec> >& links() const;
  void setLinks (const std::vector<ElementLink<CVec> >& l);
  void setLinks (std::vector<ElementLink<CVec> >&& l);
};


} // namespace DMTest


SG_BASE (DMTest::CLinks_v1, SG::AuxElement);


#endif // not DATAMODELTESTDATACOMMON_CLINKS_V1_H
