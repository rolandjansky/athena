// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AthenaROOTAccessDict.h,v 1.23 2008-04-18 02:52:56 ssnyder Exp $

/**
 * @file AthenaROOTAccess/AthenaROOTAccessDict.h
 * @author scott snyder
 * @date Apr 2007
 * @brief Dictionary generation header.
 */


#include "AthenaROOTAccess/DVCollectionProxy.h"
#include "AthenaROOTAccess/TBranchAlias.h"
#include "AthenaROOTAccess/TTreeBranchMap.h"
#include "AthenaROOTAccess/TTreeTrans.h"
#include "AthenaROOTAccess/TBranchTPConvert.h"
#include "AthenaROOTAccess/TChainROOTAccess.h"
#include "AthenaROOTAccess/addDVProxy.h"
#include "AthenaROOTAccess/ISetSGKey.h"
#include "AthenaROOTAccess/tpcnvLoad.h"
#include "AthenaROOTAccess/IARAFixup.h"
#include "AthenaROOTAccess/MuonAuxContainer_v1_fixup.h"
#include "AthenaROOTAccess/VertexAuxContainer_v1_fixup.h"
#include "AthenaROOTAccess/HIEventShapeContainer_v1_fixup.h"
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPCnvBase.h"

#include "AthContainers/DataVector.h"
/// Dummy to instantiate templates for dictionary generation.
struct dictdummy {
  DataVector<AthenaROOTAccess::DVCollectionProxyDummy> m_vdummy;
};

