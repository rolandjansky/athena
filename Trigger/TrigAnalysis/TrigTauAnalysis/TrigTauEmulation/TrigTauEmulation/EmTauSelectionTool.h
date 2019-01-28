/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// vim: ts=2 sw=2
#ifndef EMTAUSELECTIONTOOL_H
#define EMTAUSELECTIONTOOL_H

// Framework includes
#include "AsgTools/AsgTool.h"

// Local includes
#include "TrigTauEmulation/IEmTauSelectionTool.h"
#include "TrigTauEmulation/Level1SelectionTool.h"

class EmTauSelectionTool : public virtual IEmTauSelectionTool, public Level1SelectionTool
{
  ASG_TOOL_CLASS(EmTauSelectionTool, IEmTauSelectionTool)

  public:

    // Default Constructor 
    EmTauSelectionTool(const std::string& name);

    // Copy Constructor 
    EmTauSelectionTool(const EmTauSelectionTool& other);
    EmTauSelectionTool& operator=(const EmTauSelectionTool& other) = delete;
    
    // Destructor
    virtual ~EmTauSelectionTool() {};

    // Tool initialization
    virtual StatusCode initialize() override;

    virtual const asg::AcceptInfo& getAcceptInfo() const override;

    // Get the decision for a specific EmTauRoI
    virtual asg::AcceptData accept(const xAOD::EmTauRoI& l1tau) const override;


  private:

    asg::AcceptInfo m_accept;

    double m_roi_pt_cut;
    double m_roi_eta_cut;

    double m_iso_slope;
    double m_iso_offset;
    double m_iso_thresh;
    double m_iso_min;

    double m_had_leak_slope;
    double m_had_leak_offset;
    double m_had_leak_thresh;
    double m_had_leak_min;

    bool m_use_had_core;
    bool m_use_emclus;

  protected:

    bool pass_isolation(const xAOD::EmTauRoI& l1tau) const;
    bool pass_hadronic_leakage(const xAOD::EmTauRoI& l1tau) const;

};
#endif
