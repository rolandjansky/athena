// This file's extension implies that it's C, but it's really -*- C++ -*-.
/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
/**
 * @file xAODEventFormatCnv/test/xAODEventFormatCnvTestDict.h
 * @author frank berghaus <fberghaus@anl.gov>
 * @date Nov 2020
 * @brief Dictionary generation header
 */

#ifndef XAODEVENTFORMATCNVTESTDICT_H
#define XAODEVENTFORMATCNVTESTDICT_H


#include "xAODEventFormatCnv/test/A.h"
#include "xAODEventFormatCnv/test/AVec.h"
#include "xAODEventFormatCnv/test/AAuxContainer.h"
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"


template class SG::AuxElement::ConstAccessor<ElementLink<xAODMakerTest::AVec> >;

#endif  // XAODEVENTFORMATCNVTESTDICT_H
