/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H
#define ISOLATIONCORRECTION_ISOLATIONCORRECTIONTOOL_H

#include "IsolationCorrections/IIsolationCorrectionTool.h"
#include "IsolationCorrections/IsolationCorrection.h"
//
#include "AsgDataHandles/ReadHandleKey.h"
#include "AsgDataHandles/ReadHandle.h"
#include "AsgMessaging/AsgMessaging.h"
#include "AsgTools/AsgMetadataTool.h"
#include "AsgTools/AsgTool.h"
//
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventShape/EventShape.h"
//
#include "TGraph.h"

namespace CP {

class IsolationCorrectionTool
  : virtual public IIsolationCorrectionTool
  , public asg::AsgMetadataTool
{
  // Create a proper constructor for Athena
  ASG_TOOL_CLASS3(IsolationCorrectionTool,
                  IIsolationCorrectionTool,
                  CP::ISystematicsTool,
                  CP::IReentrantSystematicsTool)

public:
  IsolationCorrectionTool(const std::string& name);
  virtual ~IsolationCorrectionTool();

  virtual StatusCode initialize() override final;

  // Apply correction to a modifyable Egamma object
  virtual CP::CorrectionCode applyCorrection(xAOD::Egamma&) override final;

  // This helps to correct for the (wrong) leakage at the analysis level
  virtual CP::CorrectionCode CorrectLeakage(xAOD::Egamma&) override final;

  // Create a corrected copy from a const Egamma object
  virtual CP::CorrectionCode correctedCopy(const xAOD::Egamma&,
                                           xAOD::Egamma*&) override final;

  // systematics
  // Which systematics have an effect on the tool's behaviour?
  virtual CP::SystematicSet affectingSystematics() const override final;
  // Is the tool affected by a specific systematic?
  virtual bool isAffectedBySystematic(
    const CP::SystematicVariation& systematic) const override final;
  // Systematics to be used for physics analysis
  virtual CP::SystematicSet recommendedSystematics() const override final;
  // Use specific systematic
  virtual StatusCode applySystematicVariation(
    const CP::SystematicSet& systConfig) override final;

  virtual float GetPtCorrectedIsolation(const xAOD::Egamma&,
                                        xAOD::Iso::IsolationType) override;
  virtual float GetPtCorrection(const xAOD::Egamma&,
                                xAOD::Iso::IsolationType) const override final;
  virtual float GetDDCorrection(const xAOD::Egamma&,
                                xAOD::Iso::IsolationType) override final;
  void Print() { m_isol_corr->Print(); };

private:
  float GetEtaPointing(const xAOD::Egamma* input)
  {
    return m_isol_corr->GetEtaPointing(input);
  };

  SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey{ this,
                                                     "EventInfoKey",
                                                     "EventInfo",
                                                     "" };
  SG::ReadHandleKey<xAOD::EventShape> m_centralEventShapeKey{
    this,
    "CentralEventShapeKey",
    "TopoClusterIsoCentralEventShape",
    ""
  };

  SG::ReadHandleKey<xAOD::EventShape> m_forwardEventShapeKey{
    this,
    "ForwardEventShape",
    "TopoClusterIsoForwardEventShape",
    ""
  };

  std::string m_corr_file;
  std::string m_corr_ddshift_file;
  std::string m_corr_ddsmearing_file;
  IsolationCorrection* m_isol_corr;
  std::string m_tool_ver_str;

  std::string m_corr_etaEDParPU_file;
  std::string m_corr_etaEDPar_mc_corr_file;
  std::map<xAOD::Iso::IsolationType, std::unique_ptr<TGraph>>
    m_map_isotype_zetaPU;
  std::map<xAOD::Iso::IsolationType, std::unique_ptr<TGraph>>
    m_map_isotype_zeta_mc_corr;
  std::string m_ddVersion;
  bool m_apply_SC_leak_corr;
  bool m_apply_etaEDParPU_corr;
  bool m_apply_etaEDParPU_mc_corr;
  bool m_is_mc;
  bool m_AFII_corr;
  bool m_apply_dd{}, m_apply_ddDefault;
  bool m_correct_etcone;
  bool m_trouble_categories;
  bool m_useLogLogFit;
  bool m_forcePartType;

  // For systematcis
  CP::SystematicVariation m_systDDonoff;
};

}

#endif
