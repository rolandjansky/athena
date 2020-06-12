/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIEVENTUTILS_HIPILEUPTOOL_H
#define HIEVENTUTILS_HIPILEUPTOOL_H

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

    virtual StatusCode initialize() override;

    virtual void print() const override;
    void write(TFile* fOut) const;

    double get_efficiency(const xAOD::HIEventShapeContainer&, double ) const;
    double get_purity(const xAOD::HIEventShapeContainer&, double& );
    bool   is_pileup(const xAOD::HIEventShapeContainer&, const xAOD::ZdcModuleContainer& ) const override;

    virtual const asg::AcceptInfo& getAcceptInfo(const xAOD::HIEventShapeContainer& evShCont, const xAOD::ZdcModuleContainer& ZdcCont) const override;

  private:

    double get_et(const xAOD::HIEventShapeContainer& evShCont) const;
    double get_nNeutrons(const xAOD::ZdcModuleContainer& ZdcCont) const;

    //R.Longo - 12/10/2019 - Replacing PATCore/TAccept (inherited from 21.0 HI-equalization)
    asg::AcceptInfo m_accept;

    std::vector<double> m_etCutAndPurity;
    std::string m_fname;

    double m_etThreshold;
    double m_purityCut;
    bool m_setup;
    //Average energy deposited in the ZDC by neutrons
    const double m_npeak = 2510.;

    TH2D* m_hEvents;
    TH2D* m_hPileUp;
    TH1D* m_hCut;
    TH1D* m_hEff;
    TH1D* m_hPurity;
  };
}
#endif
