/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventSelectionTools/TreeManager.h"

#include "TFile.h"

#include <iostream>

#include "TopEventSelectionTools/MsgCategory.h"
using namespace TopEventSelectionTools;

namespace top {
  TreeManager::TreeManager() :
    m_name("NULL"),
    m_tree(nullptr) {
  }

  TreeManager::TreeManager(const std::string& name, TFile* outputFile, const int nEventAutoFlush,
                           const int basketSizePrimitive, const int basketSizeVector) :
    m_name(name),
    m_tree(nullptr) {
    outputFile->cd();
    m_tree = new TTree(name.c_str(), "tree");
    m_tree->SetAutoFlush(nEventAutoFlush);
    m_tree->SetAutoSave(0);
    m_basketSizePrimitive = basketSizePrimitive;
    m_basketSizeVector = basketSizeVector;
  }

  TreeManager::TreeManager(const TreeManager&& rhs) :
    m_name(std::move(rhs.m_name)),
    m_tree(std::move(rhs.m_tree)),
    m_outputVarPointers(std::move(rhs.m_outputVarPointers)) {
  }

  void TreeManager::initialize(const std::string& name, TFile* outputFile, const int nEventAutoFlush,
                               const int basketSizePrimitive, const int basketSizeVector) {
    if (m_tree) {
      ATH_MSG_WARNING("Tried to call initialize, but tree is already created. Doing nothing.");
      return;
    }

    outputFile->cd();
    m_tree = new TTree(name.c_str(), "tree");
    m_tree->SetAutoFlush(nEventAutoFlush);
    m_tree->SetAutoSave(0);
    m_basketSizePrimitive = basketSizePrimitive;
    m_basketSizeVector = basketSizeVector;
  }

  void TreeManager::fill() {
    m_tree->Fill();
  }

  const std::string& TreeManager::name() const {
    return m_name;
  }

  const char* TreeManager::RootType(const char* typeid_type) {
    // Check that we received a reasonable input:
    if (strlen(typeid_type) != 1) {
      //    SLogger m_logger( "SCycleBaseNTuple" );
      //    REPORT_ERROR( "Received a complex object description: " << typeid_type );
      //    throw SError( "SCycleBaseNTuple::RootType received complex object "
      //                  "description", SError::StopExecution );
    }

    // Do the hard-coded translation:
    switch (typeid_type[ 0 ]) {
    case 'c':
      return "B";

      break;

    case 'h':
      return "b";

      break;

    case 's':
      return "S";

      break;

    case 't':
      return "s";

      break;

    case 'i':
      return "I";

      break;

    case 'j':
      return "i";

      break;

    case 'f':
      return "F";

      break;

    case 'd':
      return "D";

      break;

    case 'x':
      return "L";

      break;

    case 'y':
      return "l";

      break;

    case 'b':
      return "O";

      break;
    }

    return "";
  }

  const char* TreeManager::TypeidType(const char* root_type) {
    // Do the hard-coded translation:
    if (!strcmp(root_type, "Char_t")) {
      return "c";
    } else if (!strcmp(root_type, "UChar_t")) {
      return "h";
    } else if (!strcmp(root_type, "Short_t")) {
      return "s";
    } else if (!strcmp(root_type, "UShort_t")) {
      return "t";
    } else if (!strcmp(root_type, "Int_t")) {
      return "i";
    } else if (!strcmp(root_type, "UInt_t")) {
      return "j";
    } else if (!strcmp(root_type, "Float_t")) {
      return "f";
    } else if (!strcmp(root_type, "Double_t")) {
      return "d";
    } else if (!strcmp(root_type, "Long64_t")) {
      return "x";
    } else if (!strcmp(root_type, "ULong64_t")) {
      return "y";
    } else if (!strcmp(root_type, "Bool_t")) {
      return "b";
    }

    return "";
  }
}
