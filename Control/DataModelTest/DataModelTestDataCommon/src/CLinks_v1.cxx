/*
   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 */
/**
 * @file DataModelTestDataCommon/src/CLinks_v1.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Aug, 2019
 * @brief For testing thinning of xAOD objects.
 */


#include "DataModelTestDataCommon/versions/CLinks_v1.h"
#include "xAODCore/AuxStoreAccessorMacros.h"


namespace DMTest {


AUXSTORE_OBJECT_SETTER_AND_GETTER (CLinks_v1, ElementLink<CVec>,
                                   link,  setLink)
AUXSTORE_OBJECT_SETTER_AND_GETTER (CLinks_v1, std::vector<ElementLink<CVec> >,
                                   links,  setLinks)
AUXSTORE_OBJECT_MOVE (CLinks_v1, std::vector<ElementLink<CVec> >,
                      links,  setLinks)


} // namespace DMTest

