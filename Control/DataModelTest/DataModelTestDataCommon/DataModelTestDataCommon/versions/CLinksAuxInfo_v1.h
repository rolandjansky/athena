// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/versions/CLinksAuxInfo_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#ifndef DATAMODELTESTDATACOMMON_CLINKSAUXINFO_V1_H
#define DATAMODELTESTDATACOMMON_CLINKSAUXINFO_V1_H


#include "DataModelTestDataCommon/CVec.h"
#include "AthLinks/ElementLink.h"
#include "xAODCore/AuxInfoBase.h"


namespace DMTest {


/**
 * @brief For testing thinning of xAOD objects (standalone store).
 *
 * A CLinks object holds both an ElementLink and a vector<ElementLink>
 * to CVec objects.
 */
class CLinksAuxInfo_v1
  : public xAOD::AuxInfoBase
{
public:
  CLinksAuxInfo_v1();


private:
  ElementLink<CVec> link;
  std::vector<ElementLink<CVec> > links;
};


} // namespace DMTest


SG_BASE (DMTest::CLinksAuxInfo_v1, xAOD::AuxInfoBase);


#endif // not DATAMODELTESTDATACOMMON_CLINKSAUXINFO_V1_H
