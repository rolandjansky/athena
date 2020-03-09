/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/CLinksAuxContainer_v1.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#include "DataModelTestDataCommon/versions/CLinksAuxContainer_v1.h"


namespace DMTest {


CLinksAuxContainer_v1::CLinksAuxContainer_v1()
  : xAOD::AuxContainerBase()
{
  AUX_VARIABLE (link);
  AUX_VARIABLE (links);
}


} // namespace DMTest
