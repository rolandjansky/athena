// this file is -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
     @class HIJetSignificanceTool
     @brief Modified by R.Longo on May 2020 to include new treatment for JetDecorations
*/

#ifndef HIJETREC_HIJETSIGNIFICANCETOOL_H
#define HIJETREC_HIJETSIGNIFICANCETOOL_H

#include <string>

#include "StoreGate/WriteDecorHandleKey.h"
#include "JetInterface/IJetDecorator.h"
#include "AsgTools/AsgTool.h"

#include "AsgTools/ToolHandle.h"


class HIJetSignificanceTool: public asg::AsgTool,
                             virtual public IJetDecorator
{
  ASG_TOOL_CLASS0(HIJetSignificanceTool)

 public:
  HIJetSignificanceTool(const std::string & name);

  virtual StatusCode initialize() override;

  //The modifyJet function has to be replaced by decorate
  //virtual int modifyJet(xAOD::Jet& ) const ;
  virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

 private:

 //New set of keys for decorations now needed + jet container name to initialize them automatically

 SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetSignificanceKey { this, "SignificanceKey", "SIGNIFICANCE", "Key for significance Jet attribute"};
 SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetCellSignificanceKey { this, "CellSignificanceKey", "CELL_SIGNIFICANCE", "Key for cell significance Jet attribute"};
 SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetCellSigSamplingKey { this, "CellSigSamplingKey", "CELL_SIG_SAMPLING", "Key for cell significance sampling Jet attribute"};
 SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetNMaxSigTileKey { this, "NMaxSigTileKey", "N_MAX_SIG_TILE", "Key for N max sig tile Jet attribute"};
 SG::WriteDecorHandleKey< xAOD::JetContainer > m_jetSignificanceTileKey { this, "SignificanceTileKey", "SIGNIFICANCE_TILE", "Key for significance tile Jet attribute"};

 Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "SG key for the input jet container"};

};

#endif
