// This file is -*- C++ -*-
/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIJETREC_HIJETCONSTITUENTMODIFIER_H
#define HIJETREC_HIJETCONSTITUENTMODIFIER_H
 ///////////////////////////////////////////////////////
 /// \class HIJetConstituentModifier
 /// \author R.Longo - riccardo.longo@cern.ch
 /// \date May 2020
 ///
 /// Tool to subtract final HIEventShape from constituents
 /// on the fly (w/o modifying clusters)
 ///////////////////////////////////////////////////////

#include "JetRec/JetModifierBase.h"
#include "xAODCaloEvent/CaloClusterContainer.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIJetConstituentModifier : public JetModifierBase {

   ASG_TOOL_CLASS(HIJetConstituentModifier, IJetModifier);

 public:

  // Constructor from tool name.
  HIJetConstituentModifier(const std::string& myname);

  virtual StatusCode initialize() override;
  // Inherited method to modify a jet.
  virtual int modifyJet(xAOD::Jet& jet) const;

 private:
   /// \brief Name of input cluster container
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clusterKey { this, "ClusterKey", "ClusterKey", "Name of the input Cluster Container"};

};

#endif
