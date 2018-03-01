/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
#define TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H 1

#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "xAODEgamma/ElectronFwd.h"
#include "xAODMuon/Muon.h"

#include <vector>
#include <map>

class ITrigGlobalEfficiencyCorrectionTool : public virtual CP::ISystematicsTool
{
public:
	ASG_TOOL_INTERFACE(ITrigGlobalEfficiencyCorrectionTool)

	virtual CP::CorrectionCode getEfficiencyScaleFactor(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiency(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) = 0;
	virtual CP::CorrectionCode getEfficiency(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) = 0;		
	
	virtual CP::CorrectionCode getEfficiencyScaleFactor(const std::vector<const xAOD::Electron*>& electrons,
			const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons,
			const std::vector<const xAOD::Muon*>& muons, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiency(const std::vector<const xAOD::Electron*>& electrons,
			const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc) = 0;
	virtual CP::CorrectionCode getEfficiency(unsigned runNumber, const std::vector<const xAOD::Electron*>& electrons,
			const std::vector<const xAOD::Muon*>& muons, double& efficiencyData, double& efficiencyMc) = 0;
};

#endif //> !TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
