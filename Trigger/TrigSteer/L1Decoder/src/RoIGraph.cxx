/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
* Author: Piotr Sarna
*/

#include <thread>

#include <vector>
#include <fstream>
#include <stdexcept>
#include <string>
#include <tuple>

#include "CxxUtils/make_unique.h"
#include "L1Decoder/TrigIdentifiers.h"
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigSteeringEvent/TrigRoiDescriptorCollection.h"
#include "TrigSteeringEvent/Lvl1Result.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "TRandom1.h"

#include "./RoIGraph.h"

RoIGraph::RoIGraph(const std::string& name, ISvcLocator* pSvcLocator)
	: AthAlgorithm(name, pSvcLocator),
	m_trigRoIs("OutputRoIs"),
	m_recEMTauRoIs("OutputRecEMTauRoIs"),
	m_decisions("OutputDecisions"),
	m_decisionsAux("OutputDecisionsAux."),
	//m_view("View"),
	m_configSvc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name)
{
	// outputs
	declareProperty("OutputRoIs", m_trigRoIs, "Name of the RoIs object produced by the unpacker");
	declareProperty("OutputRecEMTauRoIs", m_recEMTauRoIs, "Name of the RoIs object produced by the unpacker");
	declareProperty("OutputDecisions", m_decisions, "Name of the decisions object (wiht links to specific RoIs)");
	declareProperty("OutputDecisionsAux", m_decisionsAux, "Name of the decisions object (wiht links to specific RoIs) - aux");

	// input
	declareProperty("InputFilename", m_inputFilename, "FakeROI input filename");
	declareProperty("MonitoredTool", m_monitoredTool, "Monitored variables tool");

	// views
	//declareProperty("View", m_view, "Name of the generated view");

	// services
	declareProperty("LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");

}

StatusCode RoIGraph::initialize() {
	CHECK(m_configSvc.retrieve());

	return StatusCode::SUCCESS;
}

StatusCode RoIGraph::beginRun() {
	if (m_inputFilename.empty()) {
		ATH_MSG_WARNING("Provided InputFilename parameter is empty");
		return StatusCode::RECOVERABLE;
	}

	try
	{
		m_currentRowNumber = 0;
		m_inputData = parseInputFile();
	}
	catch (const std::exception& ex)
	{
		ATH_MSG_WARNING(ex.what());
		return StatusCode::RECOVERABLE;
	}

	ATH_MSG_DEBUG("Input file parsed successfully. Data:");

	for (auto& fragment : m_inputData) {
		for (auto& fakeRoI : fragment) {
			const char* const delim = ", ";
			std::ostringstream debugPassedTresholdIDs;
			std::copy(fakeRoI.passedThresholdIDs.begin(), fakeRoI.passedThresholdIDs.end(), std::ostream_iterator<std::string>(debugPassedTresholdIDs, delim));

			ATH_MSG_DEBUG("Eta: " + std::to_string(fakeRoI.eta) +
				", Phi: " + std::to_string(fakeRoI.phi) +
				", Word: " + std::to_string(fakeRoI.word) +
				", PassedThresholdIDs: " + debugPassedTresholdIDs.str());
		}
	}

	using namespace TrigConf;
	const ThresholdConfig* thresholdConfig = m_configSvc->thresholdConfig();
	for (auto caloType : std::vector<L1DataDef::TriggerType>{ L1DataDef::EM/*, L1DataDef::TAU*/ }) {
		for (TriggerThreshold * th : thresholdConfig->getThresholdVector(caloType)) {
			if (th != nullptr)
				ATH_MSG_DEBUG("Found threshold in the configuration: " << th->name() << " of ID: " << HLTUtils::string2hash(th->name(), "TE"));
			m_emtauThresholds.push_back(th);
		}
	}

	return StatusCode::SUCCESS;
}

void multiThreadTest1(ToolHandle<MonitoredTool> tool, const std::string& aHistogramName, unsigned aNumberOfIterations, FillPolicy aPolicy) {
	Monitored<double> monitoredValue = tool->create_monitored<double>(aHistogramName, {}, aPolicy);

	TRandom1 gRandom;

	for (unsigned i = 0; i < aNumberOfIterations; ++i) {
		monitoredValue = gRandom.Rndm();
	}
}

void multiThreadTest2(ToolHandle<MonitoredTool> tool, const std::string& aHistogramName, unsigned aNumberOfIterations, FillPolicy aPolicy) {
	TRandom1 gRandom;

	for (unsigned i = 0; i < aNumberOfIterations; ++i) {
		tool->create_monitored<double>(aHistogramName, {}, aPolicy) = gRandom.Rndm();
	}
}

StatusCode RoIGraph::execute() {
	// create view (BEN: for now don't do it until we've got the scheduling going)
	// *m_view = CxxUtils::make_unique< std::vector<SG::View*>>();
	// IProxyDict * view = new SG::View("L1CaloDecoderView");
	// m_view->push_back(view);
	IProxyDict * view = 0;

	// redirect handles to that view
	CHECK(m_trigRoIs.setStore(view));
	CHECK(m_recEMTauRoIs.setStore(view));
	CHECK(m_decisions.setStore(view));
	CHECK(m_decisionsAux.setStore(view));

	// define output
	m_trigRoIs = CxxUtils::make_unique< TrigRoiDescriptorCollection >();
	m_recEMTauRoIs = CxxUtils::make_unique< DataVector<LVL1::RecEmTauRoI> >();

	m_decisions = CxxUtils::make_unique< xAOD::TrigCompositeContainer >();
	m_decisionsAux = CxxUtils::make_unique< xAOD::TrigCompositeAuxContainer>();
	m_decisions->setStore(m_decisionsAux.ptr());

	for (auto& fakeRoI : m_inputData[m_currentRowNumber]) {
		auto recRoI = new LVL1::RecEmTauRoI(fakeRoI.word, &m_emtauThresholds);
		m_recEMTauRoIs->push_back(recRoI);

		auto trigRoI = new TrigRoiDescriptor(fakeRoI.word, 0u, 0u,
			fakeRoI.eta, fakeRoI.eta - 0.1, fakeRoI.eta + 0.1,
			fakeRoI.phi, fakeRoI.phi - 0.1, fakeRoI.phi + 0.1);
		m_trigRoIs->push_back(trigRoI);


		ATH_MSG_DEBUG("RoI word: 0x" << MSG::hex << std::setw(8) << fakeRoI.word << ", threshold pattern " << MSG::dec);
		std::vector<TriggerElementID> passedThresholdIDs;
		for (const auto& th : fakeRoI.passedThresholdIDs) {
			passedThresholdIDs.push_back(TrigConf::HLTUtils::string2hash(th, "TE"));
			ATH_MSG_DEBUG("Threshold " << th << " id: " << MSG::hex << passedThresholdIDs.back() << MSG::dec);
		}

		xAOD::TrigComposite * decision = new xAOD::TrigComposite();
		m_decisions->push_back(decision);
		decision->setDetail("Passed", passedThresholdIDs);
		decision->setObjectLink("initialRoI", ElementLink<TrigRoiDescriptorCollection>(m_trigRoIs.name(), m_trigRoIs->size() - 1));
		decision->setObjectLink("initialRecRoI", ElementLink<DataVector<LVL1::RecEmTauRoI>>(m_recEMTauRoIs.name(), m_recEMTauRoIs->size() - 1));
	}

	{
		Monitored<double> phi = m_monitoredTool->create_monitored<double>("phi", 15);

		ATH_MSG_DEBUG("phi value " << phi);

		phi = 2;
		phi.fill();

		ATH_MSG_DEBUG("phi value " << phi);

		phi = 7;
		phi = 17;

		ATH_MSG_DEBUG("phi value " << phi);
	}

	{
		const unsigned threadsNo = 800;
		const unsigned numberOfIterations = 50000;
		std::thread* t[threadsNo];

		for (unsigned i = 0; i < threadsNo; i += 4) {
			t[i + 0] = new std::thread(multiThreadTest1, m_monitoredTool, "theta", numberOfIterations, FillOnAssign);
			t[i + 1] = new std::thread(multiThreadTest2, m_monitoredTool, "theta", numberOfIterations, FillOnAssign);
			t[i + 2] = new std::thread(multiThreadTest1, m_monitoredTool, "delta", numberOfIterations, FillOnDestruction);
			t[i + 3] = new std::thread(multiThreadTest2, m_monitoredTool, "delta", numberOfIterations, FillOnDestruction);
		}

		for (unsigned i = 0; i < threadsNo; ++i) {
			t[i]->join();
			delete t[i];
		}
	}

	return StatusCode::SUCCESS;
}

StatusCode RoIGraph::finalize() {
	return StatusCode::SUCCESS;
}

std::vector<std::vector<RoIGraph::SingleFakeRoI>> RoIGraph::parseInputFile() {
	const char* whiteSpaceChars = " \t\n\r\f\v";
	auto rtrim = [whiteSpaceChars](std::string& s) { s.erase(s.find_last_not_of(whiteSpaceChars) + 1); };
	auto ltrim = [whiteSpaceChars](std::string& s) { s.erase(0, s.find_first_not_of(whiteSpaceChars)); };
	auto trim = [&rtrim, &ltrim](std::string& s) { ltrim(s); rtrim(s); };

	std::vector<std::vector<RoIGraph::SingleFakeRoI>> result;
	unsigned lineNumber = 0;
	std::string line;

	std::ifstream inputFile(m_inputFilename);

	if (!inputFile.good())
	{
		throw std::invalid_argument("File " + m_inputFilename + "does not exists or is corrupted");
	}

	while (getline(inputFile, line))
	{
		trim(line);

		if (line.empty()) continue;

		result.push_back(parseInputFileLine(line, ++lineNumber));
	}

	if (result.size() == 0) {
		throw std::invalid_argument("File " + m_inputFilename + " does not contain any RoI");
	}

	return result;
}

std::vector<RoIGraph::SingleFakeRoI> RoIGraph::parseInputFileLine(const std::string& line, unsigned lineNumber) {
	std::vector<RoIGraph::SingleFakeRoI> result;
	unsigned roiNumber = 0;
	std::stringstream inputLine(line);
	std::string roi;

	while (getline(inputLine, roi, ';'))
	{
		result.push_back(parseInputRoI(roi, lineNumber, ++roiNumber));
	}

	return result;
}

RoIGraph::SingleFakeRoI RoIGraph::parseInputRoI(const std::string& roi, unsigned lineNumber, unsigned roiNumber) {
	RoIGraph::SingleFakeRoI result;
	std::stringstream inputRoi(roi);
	std::string roiElement;
	unsigned roiElementNumber = 0;

	std::vector<std::function<void(const std::string&)>> fillResultFunctors;
	fillResultFunctors.push_back([&](const std::string& eta) { result.eta = stod(eta); });
	fillResultFunctors.push_back([&](const std::string& phi) { result.phi = stod(phi); });
	fillResultFunctors.push_back([&](const std::string& word) { result.word = stoul(word); });
	fillResultFunctors.push_back([&](const std::string& passedThresholdID) { result.passedThresholdIDs.push_back(passedThresholdID); });

	while (getline(inputRoi, roiElement, ','))
	{
		if (roiElementNumber > 2) { roiElementNumber = 3; }

		try
		{
			fillResultFunctors[roiElementNumber++](roiElement);
		}
		catch (const std::exception& ex)
		{
			throw std::invalid_argument("Exception encoruated while parsing line " + std::to_string(lineNumber) + ", roi " + std::to_string(roiNumber) + ". Exception: " + ex.what());
		}
	}

	return result;
}
