/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// HIPileupTool.h

#ifndef HIPileupTool_H
#define HIPileupTool_H

// David Adams.
// January 2014
//
// This is a simple ASG dual-use tool intended as an
// example for tool developers.

#include "HIEventUtils/IHIPileupTool.h"
#include "AsgTools/AsgTool.h"
#include "AsgTools/ToolHandle.h"
#include "xAODForward/ZdcModuleContainer.h"
#include "xAODHIEvent/HIEventShapeContainer.h"

class TH1D;
class TH2D;
class TFile;
namespace HI
{

class HIPileupTool : 
public asg::AsgTool, virtual public HI::IHIPileupTool {
ASG_TOOL_CLASS(HIPileupTool, IHIPileupTool)

public:

  HIPileupTool( const std::string& myname="HIPileupTool" );
  ~HIPileupTool();

  virtual StatusCode initialize();

  virtual void print() const;
  void write(TFile* fOut) const;

  // Display the message recorded as property "Message"
  //virtual int talk() const;

  double get_efficiency(const xAOD::HIEventShapeContainer&, double& );
  double get_purity(const xAOD::HIEventShapeContainer&, double& );
  bool   is_pileup(const xAOD::HIEventShapeContainer&, const xAOD::ZdcModuleContainer& ) const;
  //bool   is_pileup(const xAOD::HIEventShapeContainer&, double ZdcE);
  mutable Root::TAccept m_accept; //!
  virtual const Root::TAccept& getTAccept(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const;
  //virtual const Root::TAccept& accept() const { return getTAccept(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont); }

private:

  double get_et(const xAOD::HIEventShapeContainer& evShCont) const;
  double get_nNeutrons(const xAOD::ZdcModuleContainer& ZdcCont) const;
  //double get_nNeutrons(double ZdcE);

  std::string m_msg;
  std::vector<double> m_etCutAndPurity;
  std::string m_fname;

  double m_etThreshold;
  double m_purityCut;
  bool m_setup;
  double m_npeak;

  TH2D* m_hEvents;
  TH2D* m_hPileUp;
  TH1D* m_hCut;
  TH1D* m_hEff;
  TH1D* m_hPurity;
};
}
#endif
