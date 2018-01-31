// contact: jmaurer@cern.ch

#ifdef XAOD_STANDALONE
	#include "xAODRootAccess/Init.h"
	#include "xAODRootAccess/TStore.h"
#endif
#include "AsgTools/AnaToolHandle.h"
#include "TriggerAnalysisInterfaces/ITrigGlobalEfficiencyCorrectionTool.h"
#include "EgammaAnalysisInterfaces/IAsgElectronEfficiencyCorrectionTool.h"
#include "MuonAnalysisInterfaces/IMuonTriggerScaleFactors.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "PATInterfaces/ISystematicsTool.h"

#include <random>
#include <functional>
#include <algorithm>
#include <initializer_list>

#define MSGSOURCE "TrigGlobEffCorrValidation"

class SimpleElectronEfficiencyCorrectionTool : virtual public IAsgElectronEfficiencyCorrectionTool, public asg::AsgTool
{
	ASG_TOOL_CLASS(SimpleElectronEfficiencyCorrectionTool, IAsgElectronEfficiencyCorrectionTool)
	std::function<double(float)> m_func;
public:
	SimpleElectronEfficiencyCorrectionTool(const std::string& name, double eff = 0.8) : AsgTool(name), m_func([=](float){return eff;}) {}
	SimpleElectronEfficiencyCorrectionTool(const std::string& name, const std::function<double(float)>& func) : AsgTool(name), m_func(func) {}
	virtual ~SimpleElectronEfficiencyCorrectionTool() {}
	virtual CP::CorrectionCode getEfficiencyScaleFactor(const xAOD::Electron& electron, double& efficiencyScaleFactor) const override
	{
		efficiencyScaleFactor = m_func(electron.pt());
		return CP::CorrectionCode::Ok;
	}
	virtual CP::CorrectionCode applyEfficiencyScaleFactor(const xAOD::Electron&) const override { return CP::CorrectionCode::Ok; }
	virtual bool isAffectedBySystematic(const CP::SystematicVariation&) const override { return false; }
	virtual CP::SystematicSet affectingSystematics() const override { return CP::SystematicSet(); }
	virtual CP::SystematicSet recommendedSystematics() const override { return CP::SystematicSet(); }
	virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet&) override { return CP::SystematicCode::Ok; }
	virtual int systUncorrVariationIndex( const xAOD::Electron &) const override { return 0; }
};

class SimpleMuonTriggerScaleFactors : public CP::IMuonTriggerScaleFactors, public asg::AsgTool 
{
	ASG_TOOL_CLASS(SimpleMuonTriggerScaleFactors, CP::IMuonTriggerScaleFactors)
	std::map<std::string, std::function<double(float)>> m_efficiencies;
public:
	SimpleMuonTriggerScaleFactors(const std::string& name, const std::map<std::string,std::function<double(float)>>& efficiencies) : AsgTool(name)
	{
		for(auto& kv : efficiencies)
		{
			if(kv.first[0] != 'm') continue;
			std::string leg;
			for(char c : kv.first)
			{
				if(c=='u' && leg.back()=='m') leg.replace(leg.end()-1, leg.end(), "HLT_mu");
				else leg.push_back(c);
			}
			m_efficiencies.emplace(leg, kv.second);
		}
	}
	virtual ~SimpleMuonTriggerScaleFactors() {}
	virtual StatusCode initialize(void) override { return StatusCode::SUCCESS; }
	virtual CP::CorrectionCode getTriggerScaleFactor(const xAOD::MuonContainer&, Double_t&, const std::string&) override { return CP::CorrectionCode::Ok; }
	virtual CP::CorrectionCode getTriggerEfficiency(const xAOD::Muon& muon, Double_t& efficiency, const std::string& trig, Bool_t) override
	{
		auto itr = m_efficiencies.find(trig);
		if(itr != m_efficiencies.end())
		{
			efficiency = itr->second(muon.pt());
			return CP::CorrectionCode::Ok;
		}
		return CP::CorrectionCode::Error;
	}
    virtual bool isAffectedBySystematic(const CP::SystematicVariation&) const override { return false; }
	virtual CP::SystematicSet affectingSystematics() const override { return CP::SystematicSet(); }
	virtual CP::SystematicSet recommendedSystematics() const override { return CP::SystematicSet(); }
	virtual CP::SystematicCode applySystematicVariation(const CP::SystematicSet&) override { return CP::SystematicCode::Ok; }
};

struct Config
{
	std::string testName;
	std::string triggers = "";
	unsigned minLeptons = 1;
	unsigned maxLeptons = 5;
	std::vector<float> leptonPtValues;
	std::map<std::string, std::function<double(float)>> efficiencies;
	std::function<bool(const std::vector<const xAOD::Electron*>&,const std::vector<const xAOD::Muon*>&)> eventSelection = nullptr;
	double expectedEfficiency = -1.;
	double expectedEfficiencyTolerance = 1e-6;
	bool debug = false;
	
	Config(const char* name) : testName(name) { leptonPtValues = {30e4f}; }
	Config& setTriggers(const std::string& t) { triggers = t; return *this; }
	Config& setLeptonPDF(unsigned nmin, unsigned nmax, std::initializer_list<float> ptvalues)
	{
		minLeptons = nmin;
		maxLeptons = nmax;
		leptonPtValues = ptvalues;
		return *this;
	}
	Config& setEfficiency(const std::string& leg, double eff)
	{
		efficiencies.emplace(leg, [=](float){return eff;});
		return *this;
	}
	Config& setEfficiency(const std::string& leg, std::function<double(float)>&& eff)
	{
		efficiencies.emplace(leg, eff);
		return *this;
	}
	Config& setEventSelection(const decltype(eventSelection)& sel) { eventSelection = sel; return *this; }
	Config& setDebug() { debug = true; return *this; };
	Config& setExpectedEfficiency(double eff, double tolerance)
	{
		expectedEfficiency = eff;
		expectedEfficiencyTolerance = tolerance;
		return *this;
	}
};

xAOD::ElectronContainer* electronContainer = nullptr;
xAOD::MuonContainer* muonContainer = nullptr;
bool quiet = false, fast = false;

template<class Lepton>
inline unsigned count(const std::vector<const Lepton*>& leptons, float ptmin, float ptmax = 1e12f)
{
	return std::count_if(leptons.cbegin(), leptons.cend(), [=](const Lepton* l){return l->pt()>=ptmin && l->pt()<ptmax;});
}

bool run_test(const Config& cfg, int toy_to_debug = -1);

using namespace asg::msgUserCode;

int main(int argc, char* argv[])
{
	const std::string flagQuiet("--quiet"), flagFast("--fast");
	for(int i=1;i<argc;++i)
	{
		if(argv[i] == flagQuiet) quiet = true;
		else if(argv[i] == flagFast) fast = true;
	}
	
#ifdef XAOD_STANDALONE
	StatusCode::enableFailure();
	ANA_CHECK (xAOD::Init());
	xAOD::TStore store;
#endif
	CP::CorrectionCode::enableFailure();
	
	electronContainer = new xAOD::ElectronContainer();
	ANA_CHECK(store.record(electronContainer, "Electrons"));
	auto electronContainerAux = new xAOD::ElectronAuxContainer();
	ANA_CHECK(store.record(electronContainerAux, "ElectronsAux"));
	electronContainer->setStore(electronContainerAux);
	for(int i=0;i<10;++i)
	{
		auto el = new xAOD::Electron();
		electronContainer->push_back(el);
	}
	
	muonContainer = new xAOD::MuonContainer();
	ANA_CHECK(store.record(muonContainer, "Muons"));
	auto muonContainerAux = new xAOD::MuonAuxContainer();
	ANA_CHECK(store.record(muonContainerAux, "MuonsAux"));
	muonContainer->setStore(muonContainerAux);
	for(int i=0;i<10;++i)
	{
		auto mu = new xAOD::Muon();
		muonContainer->push_back(mu);
	}
	
	using VE = const std::vector<const xAOD::Electron*>&;
	using VM = const std::vector<const xAOD::Muon*>&;
	
	ANA_CHECK(run_test(Config("1L")
		.setTriggers("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50")
		.setLeptonPDF(1, 1, {30e3f})
		.setEfficiency("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", 0.60)
		.setEfficiency("mu20_iloose_L1MU15_OR_mu50", 0.60)
		.setExpectedEfficiency(0.60, 1e-6)
	));
	
	ANA_CHECK(run_test(Config("1L (multilepton)")
		.setTriggers("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50")
		.setLeptonPDF(1, 4, {30e3f})
		.setEfficiency("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", 0.40)
		.setEfficiency("mu20_iloose_L1MU15_OR_mu50", 0.40)
	));

	ANA_CHECK(run_test(Config("2e")
		.setTriggers("2e12_lhloose_L12EM10VH")
		.setLeptonPDF(2, 3, {30e3f})
		.setEfficiency("e12_lhloose_L1EM10VH", 0.50)
		.setEventSelection([](VE ve, VM) { return count(ve,13e3f)>1; })
	));
	
	ANA_CHECK(run_test(Config("emu")
		.setTriggers("e17_lhloose_mu14")
		.setLeptonPDF(2, 3, {16e3f, 20e3f})
		.setEfficiency("e17_lhloose", 0.60)
		.setEfficiency("mu14", 0.60)
		.setEventSelection([](VE ve, VM vm) { return count(ve,18e3f)>0&&count(vm,15e3f)>0; })
	));
	
	ANA_CHECK(run_test(Config("2mu")
		.setTriggers("mu18_mu8noL1")
		.setLeptonPDF(2, 3, {11e3f, 21e3f})
		.setEfficiency("mu18", 0.60)
		.setEfficiency("mu8noL1", 0.70)
		.setEventSelection([](VE, VM vm) { return count(vm,19e3f)>0&&count(vm,10e3f)>1; })
	));
	
	ANA_CHECK(run_test(Config("2e||emu||2mu")
		.setTriggers("2e12_lhloose_L12EM10VH || e17_lhloose_mu14 || mu18_mu8noL1")
		.setLeptonPDF(2, 4, {10e3f, 16e3f, 20e3f})
		.setEfficiency("e12_lhloose_L1EM10VH", 0.50)
		.setEfficiency("e17_lhloose", 0.60)
		.setEfficiency("mu18", 0.60)
		.setEfficiency("mu14", 0.60)
		.setEfficiency("mu8noL1", 0.70)
		.setEventSelection([](VE ve, VM vm) { return count(ve,13e3f)>1 
			|| (count(ve,18e3f)>0&&count(vm,15e3f)>0) || (count(vm,19e3f)>0&&count(vm,10e3f)>1); })
	));
	
	ANA_CHECK(run_test(Config("2e||emu||2mu||1e||1mu")
		.setTriggers("2e12_lhloose_L12EM10VH || e17_lhloose_mu14 || mu18_mu8noL1 || e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15_OR_mu50")
		.setLeptonPDF(1, 4, {10e3f, 16e3f, 20e3f, 30e3f})
		.setEfficiency("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", 0.40)
		.setEfficiency("mu20_iloose_L1MU15_OR_mu50", 0.40)
		.setEfficiency("e12_lhloose_L1EM10VH", 0.50)
		.setEfficiency("e17_lhloose", 0.60)
		.setEfficiency("mu18", 0.60)
		.setEfficiency("mu14", 0.60)
		.setEfficiency("mu8noL1", 0.70)
		.setEventSelection([](VE ve, VM vm) { return count(ve,13e3f)>1 
			|| (count(ve,18e3f)>0&&count(vm,15e3f)>0) || (count(vm,19e3f)>0&&count(vm,10e3f)>1)
			|| count(ve,25e3f)>0 || count(vm,21.5e3f)>0; })
	));
	
	ANA_CHECK(run_test(Config("2e_e")
		.setTriggers("e17_lhloose_2e9_lhloose")
		.setLeptonPDF(3, 5, {12e3f, 20e3f})
		.setEfficiency("e17_lhloose", 0.60)
		.setEfficiency("e9_lhloose", 0.70)
		.setEventSelection([](VE ve, VM) { return (count(ve,18e3f)>0&&count(ve,10e3f)>2); })
	));
	
	ANA_CHECK(run_test(Config("3mu")
		.setTriggers("3mu6")
		.setLeptonPDF(3, 5, {10e3f})
		.setEfficiency("mu6", 0.70)
		.setEventSelection([](VE, VM vm) { return count(vm,7e3f)>2; })
	));
	
	ANA_CHECK(run_test(Config("2e_mu||e_2mu")
		.setTriggers("e12_lhloose_2mu10 || 2e12_lhloose_mu10")
		.setLeptonPDF(3, 5, {14e3f})
		.setEfficiency("e12_lhloose", 0.50)
		.setEfficiency("mu10", 0.60)
		.setEventSelection([](VE ve, VM vm) { return (count(ve,13e3f)>1&&count(vm,11e3f)>0) || (count(ve,13e3f)>0&&count(vm,11e3f)>1); })
	));

	// not a full test of the function since 2mu10 completely shadows 2mu14, but it validates at least part of the logic
	ANA_CHECK(run_test(Config("2mu||2mu||mu")
		.setTriggers("2mu10 || 2mu14 || mu24_iloose_L1MU15")
		.setLeptonPDF(1, 4, {13e3f, 16e3f, 30e3f})
		.setEfficiency("mu10", 0.70)
		.setEfficiency("mu14", 0.60)
		.setEfficiency("mu24_iloose_L1MU15", 0.30)
		.setEventSelection([](VE, VM vm) { return (count(vm,11e3f)>1) || (count(vm,25.5e3f)>1) ; })
	));
	
	ANA_CHECK(run_test(Config("mu_mu||2mu||mu")
		.setTriggers("mu18_mu8noL1 || 2mu14 || mu24_iloose_L1MU15")
		.setLeptonPDF(1, 4, {11e3f, 16e3f, 20e3f, 30e3f})
		.setEfficiency("mu8noL1", 0.80)
		.setEfficiency("mu14", 0.70)
		.setEfficiency("mu18", 0.60)
		.setEfficiency("mu24_iloose_L1MU15", 0.30)
		.setEventSelection([](VE, VM vm) { return (count(vm,25.5e3f)>0) || (count(vm,10e3f)>1&&count(vm,19e3f)>0) || (count(vm,15e3f)>1) ; })
	));
	
	ANA_CHECK(run_test(Config("2e||mu_mu||2mu||emu||emu||e||mu")
		.setTriggers("2e12_lhloose_L12EM10VH || mu18_mu8noL1 || 2mu14 || e17_lhloose_mu14 || e7_lhmedium_mu24 || e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose || mu20_iloose_L1MU15")
		.setLeptonPDF(2, 2, {11e3f, 13e3f, 16e3f, 20e3f, 30e3f})
		.setEfficiency("mu8noL1", 0.80)
		.setEfficiency("mu14", 0.70)
		.setEfficiency("mu18", 0.60)
		.setEfficiency("mu20_iloose_L1MU15", 0.30)
		.setEfficiency("mu24", 0.37)
		.setEfficiency("e7_lhmedium", 0.53)
		.setEfficiency("e12_lhloose_L1EM10VH", 0.73)
		.setEfficiency("e17_lhloose", 0.67)
		.setEfficiency("e24_lhmedium_L1EM20VH_OR_e60_lhmedium_OR_e120_lhloose", 0.42)
		.setEventSelection([](VE ve, VM vm) { return (count(vm,25.5e3f)>0) || (count(ve,25e3f)>0) || (count(ve,13e3f)>1) || (count(vm,15e3f)>1)
			|| (count(vm,10e3f)>1&&count(vm,19e3f)>0) ||  (count(ve,18e3f)>0&&count(vm,15e3f)>0) || (count(ve,8e3f)>0&&count(vm,25.5e3f)>0); })
	));
	
	//Info(MSGSOURCE, "Boost version: %i", BOOST_VERSION);
	return 0;
}


bool configure(asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool>& tool, ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>& electronEffToolsHandles, ToolHandleArray<IAsgElectronEfficiencyCorrectionTool>& electronSFToolsHandles, ToolHandleArray<CP::IMuonTriggerScaleFactors>& muonToolsHandles,
	const std::string& triggers, const std::map<std::string, std::string>& legsPerTool, unsigned long nToys, bool debug)
{
	ANA_CHECK(tool.setProperty("ElectronEfficiencyTools", electronEffToolsHandles));
	ANA_CHECK(tool.setProperty("ElectronScaleFactorTools", electronSFToolsHandles));
	ANA_CHECK(tool.setProperty("ListOfLegsPerTool", legsPerTool));
	ANA_CHECK(tool.setProperty("MuonTools", muonToolsHandles));
	ANA_CHECK(tool.setProperty("TriggerCombination2015", triggers));
	ANA_CHECK(tool.setProperty("NumberOfToys", nToys));
	ANA_CHECK(tool.setProperty("UseInternalSeed", true));
	ANA_CHECK(tool.setProperty("OutputLevel", debug? MSG::DEBUG : quiet? MSG::WARNING : MSG::INFO));
	ANA_CHECK(tool.initialize());
	return true;
}

bool run_test(const Config& cfg, int toy_to_debug)
{
	if(!quiet) Info(MSGSOURCE, "Running test %s", cfg.testName.c_str());
	const int nToysPerEvent = (fast? 250 : 500), nToysPerTest = (fast? 200 : 2000), nToySamples = 10;
	std::vector<SimpleElectronEfficiencyCorrectionTool*> electronTools;
	ToolHandleArray<IAsgElectronEfficiencyCorrectionTool> electronEffToolsHandles, electronSFToolsHandles;
	std::map<std::string,std::string> legsPerTool;
	for(auto& kv : cfg.efficiencies)
	{
		if(kv.first[0]=='e')
		{
			electronTools.emplace_back(new SimpleElectronEfficiencyCorrectionTool("EFF-"+kv.first, kv.second));
			legsPerTool.emplace(electronTools.back()->name(), kv.first);
			electronEffToolsHandles.emplace_back(electronTools.back());
			electronTools.emplace_back(new SimpleElectronEfficiencyCorrectionTool("SF-"+kv.first, 1.));
			legsPerTool.emplace(electronTools.back()->name(), kv.first);
			electronSFToolsHandles.emplace_back(electronTools.back());
		}
	}
	std::vector<SimpleMuonTriggerScaleFactors*> muonTools;
	muonTools.emplace_back(new SimpleMuonTriggerScaleFactors("EFF-muons", cfg.efficiencies));
	ToolHandleArray<CP::IMuonTriggerScaleFactors> muonToolsHandles = {muonTools.back()};
	std::string suffix = ((toy_to_debug>=0)? "-debug" : "");
	asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> trigGlobTool("TrigGlobalEfficiencyCorrectionTool/trigGlobTool" + suffix);
	asg::AnaToolHandle<ITrigGlobalEfficiencyCorrectionTool> trigGlobTool_toys("TrigGlobalEfficiencyCorrectionTool/trigGlobTool_toys" + suffix);
	bool debug = cfg.debug || (toy_to_debug>=0);
	ANA_CHECK(configure(trigGlobTool, electronEffToolsHandles, electronSFToolsHandles, muonToolsHandles, cfg.triggers, legsPerTool, 0, debug));
	ANA_CHECK(configure(trigGlobTool_toys, electronEffToolsHandles, electronSFToolsHandles, muonToolsHandles, cfg.triggers, legsPerTool, nToysPerEvent, debug));
	std::default_random_engine rdm;
	std::uniform_int_distribution<unsigned> nleptonsPdf(cfg.minLeptons, cfg.maxLeptons);
	std::bernoulli_distribution flavourPdf;
	std::uniform_int_distribution<unsigned> ptIndexPdf(0, cfg.leptonPtValues.size()-1);
	std::vector<const xAOD::Electron*> electrons;
	std::vector<const xAOD::Muon*> muons;
	double sum_eff = 0., sum_eff_toys[nToySamples];
	std::fill(std::begin(sum_eff_toys), std::end(sum_eff_toys), 0.);
	for(int toy=0; toy<nToysPerTest; ++toy)
	{
		do
		{
			electrons.clear();
			muons.clear();
			unsigned nLeptons = nleptonsPdf(rdm);
			for(unsigned index=0;index<nLeptons;++index)
			{
				float pt = cfg.leptonPtValues[ptIndexPdf(rdm)];
				if(flavourPdf(rdm))
				{
					auto electron = electronContainer->at(electrons.size());
					electrons.push_back(electron);
					electron->setP4(pt, 0.f, 0.f, 0.511f);
				}
				else
				{
					auto muon = muonContainer->at(muons.size());
					muons.push_back(muon);
					muon->setP4(pt, 0.f, 0.f);
				}
			}
		}
		while(cfg.eventSelection && !cfg.eventSelection(electrons, muons));
		if(toy_to_debug>=0 && toy!=toy_to_debug) continue;
		
		double dummy, eff = 0., eff_toys;
		const int runNumber = 281130;
		if(trigGlobTool->getEfficiency(runNumber, electrons, muons, eff, dummy) != CP::CorrectionCode::Ok)
		{
			if(toy_to_debug < 0)
			{
				Info(MSGSOURCE, "Running %s test again for this toy event, with the debug flag set", cfg.testName.c_str());
				run_test(cfg, toy);
			}
			return false;
		}
		sum_eff += eff;
		for(int spl=0;spl<nToySamples;++spl)
		{
			eff_toys = 0.;
			if(trigGlobTool_toys->getEfficiency(runNumber, electrons, muons, eff_toys, dummy) != CP::CorrectionCode::Ok)
			{
				if(toy_to_debug < 0)
				{
					Info(MSGSOURCE, "Running %s test again for this toy event, with the debug flag set", cfg.testName.c_str());
					run_test(cfg, toy);
					return false;
				}
				return false;
			}
			sum_eff_toys[spl] += eff_toys;
		}
		if(toy == toy_to_debug) return true;
	}
	if(cfg.expectedEfficiency > 0.)
	{
		double eff = sum_eff / nToysPerTest;
		if(fabs(eff - cfg.expectedEfficiency) > cfg.expectedEfficiencyTolerance)
		{
			return false;
		}
	}
	double eff = sum_eff/nToysPerTest, eff_toys = 0., toys_rms = 0.;
	for(double sum : sum_eff_toys) eff_toys += sum / nToysPerTest;
	eff_toys /= nToySamples;
	for(double sum : sum_eff_toys) toys_rms += pow(sum/nToysPerTest - eff_toys, 2);
	toys_rms = sqrt(toys_rms / (nToySamples-1));
	double  sigma = fabs(eff-eff_toys) / toys_rms;
	if(!quiet) Info(MSGSOURCE, "Efficiency: %f, toys: %f (signif. = %.1f sigma)", eff, eff_toys, sigma);
	if(sigma >= 3.)
	{
		Error(MSGSOURCE, "The difference is too large");
		return false;
	}
	else if(sigma >= 2.) Warning(MSGSOURCE, "The difference isn't small");
	for(auto tool : electronTools) delete tool;
	for(auto tool : muonTools) delete tool;
	return true;
}
