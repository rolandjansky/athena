/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// contact: jmaurer@cern.ch

#ifndef TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
#define TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H 1

#include "PATInterfaces/ISystematicsTool.h"
#include "PATInterfaces/CorrectionCode.h"
#include "xAODEgamma/Electron.h"
#include "xAODMuon/Muon.h"
#include "xAODEgamma/Photon.h"

#include <vector>
#include <type_traits>

class ITrigGlobalEfficiencyCorrectionTool : public virtual CP::ISystematicsTool
{
public:
	ASG_TOOL_INTERFACE(ITrigGlobalEfficiencyCorrectionTool)
	
	template<typename Arg> static constexpr bool validArgs(unsigned nTrailingDoubles);
	template<typename Arg1, typename Arg2, typename... OtherArgs> static constexpr bool validArgs(unsigned nTrailingDoubles);

	virtual CP::CorrectionCode getEfficiencyScaleFactor(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiency(const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) = 0;
	virtual CP::CorrectionCode checkTriggerMatching(bool& matched, const std::vector<const xAOD::IParticle*>& particles) = 0;
	
	/// Alternatively, the list of particles can be supplied via one or several vectors of xAOD::Electron*/Muon*/Photon*
	/// The generic signature is getEfficiencyScaleFactor((const) vector<(const)Type1*>&, ..., (const) vector<(const)TypeN*>&, double& efficiencyScaleFactor)
	/// e.g. getEfficiencyScaleFactor(electrons, muons, sf);
	///      getEfficiencyScaleFactor(photons, sf);
	///      getEfficiencyScaleFactor(muons, electrons, photons, sf);
	/// don't forget the last argument(s)! (scale factor for getEfficiencyScaleFactor(), data and MC efficiencies for getEfficiency())
	template<typename... Args>
	auto getEfficiencyScaleFactor(Args&... args) -> std::enable_if_t<validArgs<Args...>(1), CP::CorrectionCode>;
	template<typename... Args>
	auto getEfficiency(Args&... args) -> std::enable_if_t<validArgs<Args...>(2), CP::CorrectionCode>;
	template<typename... Args>
	auto checkTriggerMatching(bool& matched, Args&... args) -> std::enable_if_t<validArgs<Args...>(0), CP::CorrectionCode>;
	
	/// This will fill the 'triggers' argument with the names of the triggers relevant for the current run number, among those specified in the tool configuration
	virtual CP::CorrectionCode getRelevantTriggers(std::vector<std::string>& triggers) = 0;
	
	/// This utility function provides the number of legs for the specified trigger
	virtual CP::CorrectionCode countTriggerLegs(const std::string& trigger, std::size_t& numberOfLegs) = 0;
	
	/// These should in principle not be used (except by unit tests), as the CP tools require the EventInfo decoration "RandomRunNumber" to be present 
	virtual CP::CorrectionCode getEfficiencyScaleFactor(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyScaleFactor) = 0;
	virtual CP::CorrectionCode getEfficiency(unsigned runNumber, const std::vector<const xAOD::IParticle*>& particles, double& efficiencyData, double& efficiencyMc) = 0;	
	
protected:
	double* handleArg(double& arg, std::vector<const xAOD::IParticle*>&) { return &arg; }
	template<typename P>  double* handleArg(const std::vector<P>& arg, std::vector<const xAOD::IParticle*>& particles)
	{
		for(auto ptr : arg) particles.push_back(static_cast<const xAOD::IParticle*>(ptr));
		return nullptr;
	}
};

template<typename Arg>
constexpr bool ITrigGlobalEfficiencyCorrectionTool::validArgs(unsigned nTrailingDoubles)
{
	if(std::is_same<double, Arg>::value) return (nTrailingDoubles==1);
	using P = std::remove_cv_t<Arg>;
	return std::is_same<P, std::vector<xAOD::Electron*>>::value
		|| std::is_same<P, std::vector<const xAOD::Electron*>>::value
		|| std::is_same<P, std::vector<xAOD::Muon*>>::value
		|| std::is_same<P, std::vector<const xAOD::Muon*>>::value
		|| std::is_same<P, std::vector<xAOD::Photon*>>::value
		|| std::is_same<P, std::vector<const xAOD::Photon*>>::value;
}

template<typename Arg1, typename Arg2, typename... OtherArgs>
constexpr bool ITrigGlobalEfficiencyCorrectionTool::validArgs(unsigned nTrailingDoubles)
{
	bool xs [] = { std::is_same<OtherArgs, double>::value..., true };
	for(bool x : xs) if(!x) return validArgs<Arg1>(0) && validArgs<Arg2, OtherArgs...>(nTrailingDoubles);
	unsigned nTD = sizeof...(OtherArgs);
	if(nTD == nTrailingDoubles) return validArgs<Arg1>(0) && validArgs<Arg2>(0);
	if(nTD == nTrailingDoubles-1) return validArgs<Arg1>(0) && std::is_same<Arg2, double>::value;
	if(nTD == nTrailingDoubles-2) return std::is_same<Arg1, double>::value && std::is_same<Arg2, double>::value;
	return false;
}

template<typename... Args>
auto ITrigGlobalEfficiencyCorrectionTool::getEfficiencyScaleFactor(Args&... args)
	-> std::enable_if_t<validArgs<Args...>(1), CP::CorrectionCode>
{
	std::vector<const xAOD::IParticle*> particles;
	double* sf[] = { nullptr, handleArg(args, particles)... };
	return getEfficiencyScaleFactor(particles, *sf[sizeof...(Args)]);
}

template<typename... Args>
auto ITrigGlobalEfficiencyCorrectionTool::getEfficiency(Args&... args)
	-> std::enable_if_t<validArgs<Args...>(2), CP::CorrectionCode>
{
	std::vector<const xAOD::IParticle*> particles;
	double* eff[] = { nullptr, handleArg(args, particles)... };
	return getEfficiency(particles, *eff[sizeof...(Args)-1], *eff[sizeof...(Args)]);
}

template<typename... Args>
auto ITrigGlobalEfficiencyCorrectionTool::checkTriggerMatching(bool& matched, Args&... args)
	-> std::enable_if_t<validArgs<Args...>(0), CP::CorrectionCode>
{
	std::vector<const xAOD::IParticle*> particles;
	double* eff[] __attribute__((unused)) = { nullptr, handleArg(args, particles)... };
	return checkTriggerMatching(matched, particles);
}

#endif //> !TRIGGERANALYSISINTERFACES_ITRIGGLOBALEFFICIENCYCORRECTIONTOOL_H
