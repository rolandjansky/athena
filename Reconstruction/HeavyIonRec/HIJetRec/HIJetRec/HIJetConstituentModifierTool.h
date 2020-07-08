// This file is -*- C++ -*-
/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
 */

#ifndef HIJETREC_HIJETCONSTITUENTMODIFIERTOOL_H
#define HIJETREC_HIJETCONSTITUENTMODIFIERTOOL_H
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
#include "HIJetRec/IHISubtractorTool.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"

class HIJetConstituentModifierTool : public JetModifierBase {

   ASG_TOOL_CLASS(HIJetConstituentModifierTool, IJetModifier);

 public:

  // Constructor from tool name.
  HIJetConstituentModifierTool(const std::string& myname);

  virtual StatusCode initialize() override;
  // Inherited method to modify a jet.
  virtual int modifyJet(xAOD::Jet& jet) const override;

 private:
   /// \brief Name of input cluster container
  SG::ReadHandleKey< xAOD::CaloClusterContainer > m_clusterKey { this, "ClusterKey", "ClusterKey", "Name of the input Cluster Container"};
  /// \brief handle to IHISubtractorTool that determines the subtracted kinematics for each constituent
  ToolHandle<IHISubtractorTool> m_subtractorTool { this, "Subtractor", "HIJetSubtractorToolBase", "" };
  /// |brief boolean switch to drive the JetScale settings after constituents are added 
  Gaudi::Property< bool > m_originCorrection { this, "ApplyOriginCorrection", false, "Apply Origin Correction boolean switch"};

};

#endif
