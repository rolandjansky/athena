// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//
#ifndef XAODROOTACCESS_LINKDEF_H
#define XAODROOTACCESS_LINKDEF_H

// Local include(s):
#include "xAODRootAccess/tools/xAODTEventBranch.h"
#include "xAODRootAccess/tools/xAODTEventTree.h"
#include "xAODRootAccess/tools/xAODTMetaBranch.h"
#include "xAODRootAccess/tools/xAODTMetaTree.h"
#include "xAODRootAccess/tools/TTransTrees.h"
#include "xAODRootAccess/tools/TReturnCode.h"
#include "xAODRootAccess/tools/TFileMerger.h"
#include "xAODRootAccess/tools/TFileChecker.h"
#include "xAODRootAccess/MakeTransientTree.h"
#include "xAODRootAccess/Init.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ namespace xAOD;
#pragma link C++ class xAODTEventBranch;
#pragma link C++ class xAODTEventTree;
#pragma link C++ class xAODTMetaBranch;
#pragma link C++ class xAODTMetaTree;
#pragma link C++ class xAOD::TTransTrees;
#pragma link C++ class xAOD::TReturnCode;
#pragma link C++ class xAOD::TFileMerger;
#pragma link C++ class xAOD::TFileChecker;
#pragma link C++ function xAOD::MakeTransientTrees(TFile*,const char*);
#pragma link C++ function xAOD::ClearTransientTrees;
#pragma link C++ function xAOD::MakeTransientTree(TFile*,const char*);
#pragma link C++ function xAOD::MakeTransientTree(TChain*);
#pragma link C++ function xAOD::ClearTransientTree;
#pragma link C++ function xAOD::MakeTransientMetaTree(TFile*);
#pragma link C++ function xAOD::MakeTransientMetaTree(TChain*);
#pragma link C++ function xAOD::ClearTransientMetaTree;
#pragma link C++ function xAOD::Init(const char*);
#pragma link C++ function xAOD::SetMessageSourceWidth(size_t);

#endif // __CINT__
#endif // XAODROOTACCESS_LINKDEF_H
