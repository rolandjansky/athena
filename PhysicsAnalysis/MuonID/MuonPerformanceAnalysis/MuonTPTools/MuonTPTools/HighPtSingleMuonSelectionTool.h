/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * HighPtSingleMuonSelectionTool.h
 *
 *  Created on: Jun 18, 2015
 *      Author: goblirsc<at>CERN.CH
 */

#ifndef HIGHPTSINGLEMUONSELECTIONTOOL_H_
#define HIGHPTSINGLEMUONSELECTIONTOOL_H_

/// This is the implementation of the IMuonTPSelectionTool for the tag and probe
/// efficiency measurement using dimuon resonances (JPsi and Z).

#include "MuonTPTools/MuonTPSelectionTool.h"
#include "MuonTPTools/IMuonTPExtrapolationTool.h"
#include "IsolationSelection/IIsolationSelectionTool.h"
#include "AsgTools/AsgTool.h"

class HighPtSingleMuonSelectionTool:  public MuonTPSelectionTool{
      ASG_TOOL_CLASS(HighPtSingleMuonSelectionTool, IMuonTPSelectionTool)
public:
      HighPtSingleMuonSelectionTool(std::string myname);

    virtual StatusCode initialize();

    /// Select Probes
    ProbeContainer* selectProbes(const xAOD::MuonContainer*, const xAOD::IParticleContainer*) const;

    /// Get Efficiency Flag
    std::string efficiencyFlag() {return m_efficiencyFlag;}

    bool PassProbeKinematics (const xAOD::IParticle* probe) const;

    bool PassProbeIPCuts(const xAOD::IParticle* probe) const;
    bool PassTagIPCuts(const xAOD::Muon* tag) const;

    bool PassProbeIsoCuts (const xAOD::IParticle* probe) const;
    bool PassTagIsoCuts (const xAOD::Muon* tag) const;

private:

    bool m_probe_ID_hits;
    
    bool m_probe_iso;

    double m_probe_d0;
    double m_probe_d0Sign;
    double m_probe_z0;

    ToolHandle<CP::IIsolationSelectionTool> m_probe_isotool;
};


#endif /* HIGHPTSINGLEMUONSELECTIONTOOL_H_ */
