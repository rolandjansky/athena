/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// CalibrationDataInterfaceTester.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "CalibrationDataInterface/CalibrationDataInterfaceTester.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"
#include "JetTagInfo/TruthInfo.h"
#include <algorithm>
#include <utility>
#include <cctype>

using std::string;
using Analysis::CalibrationDataInterfaceTool;

//================ Constructor =================================================

Analysis::CalibrationDataInterfaceTester::CalibrationDataInterfaceTester(const std::string& name, ISvcLocator* pSvcLocator)
  :   AthAlgorithm(name, pSvcLocator)
{
  //  template for property declaration
  declareProperty("JetCollection",          m_jetCollection,          "name of the jet collection to be used");
  declareProperty("Tagger",                 m_tagger,                 "name of the tagging algorithm to be used");
  declareProperty("OperatingPoint",         m_operatingPoint,         "name of the tagging algorithm operating point to be used");
  declareProperty("CalibrationUncertainty", m_calibrationUncertainty, "calibration uncertainty result");
  declareProperty("CalibrationInterface",   m_calibrationInterface,   "interface tool instance name");
}

//================ Destructor =================================================

Analysis::CalibrationDataInterfaceTester::~CalibrationDataInterfaceTester()
{}


//================ Initialisation =================================================

StatusCode Analysis::CalibrationDataInterfaceTester::initialize()
{
  // convert the "calibration uncertainty" to lowercase, to limit the comparisons
//   for (int l = 0; l < m_calibrationUncertainty.size(); ++l) {
//     m_calibrationUncertainty[l] = tolower(m_calibrationUncertainty[l]);
//   }

  StatusCode sc = m_calibrationInterface.retrieve();
  if (sc.isFailure()) {
    msg() << MSG::FATAL << "initialize() in " << name() << ": unable to retrieve "
          << "calibration interface tool!" << endmsg;
    return sc;
  }

  std::transform(m_calibrationUncertainty.begin(), m_calibrationUncertainty.end(),
		 m_calibrationUncertainty.begin(), tolower);
  if (m_calibrationUncertainty.find("total") != string::npos) {
    m_uncertaintyType = CalibrationDataInterfaceTool::Total;
  } else if (m_calibrationUncertainty.find("syst") != string::npos) {
    m_uncertaintyType = CalibrationDataInterfaceTool::Systematic;
  } else if (m_calibrationUncertainty.find("stat") != string::npos) {
    m_uncertaintyType = CalibrationDataInterfaceTool::Statistical;
  } else {
    m_uncertaintyType = CalibrationDataInterfaceTool::None;
  }
  
  msg() << MSG::INFO << "initialize() successful in " << name() << endmsg;
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Analysis::CalibrationDataInterfaceTester::finalize()
{
  // Code entered here will be executed once at the end of the program run.
  return StatusCode::SUCCESS;
}

//================ Execution ====================================================

StatusCode Analysis::CalibrationDataInterfaceTester::execute()
{
  // Code entered here will be executed once per event

  // retrieve the desired jet collection
  const JetCollection* jets;
  ATH_CHECK( evtStore()->retrieve(jets, m_jetCollection) );

  int njtag = (*jets).size();
  msg() << MSG::INFO << "JetCollection " << m_jetCollection
	<< " found with " << njtag << " jets." << endmsg;

  // instead of (*jets).begin() etc.?
  JetCollection::const_iterator jetItr = jets->begin();
  JetCollection::const_iterator jetEnd = jets->end();
  for (; jetItr != jetEnd; ++jetItr) {
    // --- get btagging weight for the tagger under consideration
    double weight = (*jetItr)->getFlavourTagWeight(m_tagger);

    // --- get the true label of the jet from MC Truth:
    const Analysis::TruthInfo* mcinfo = (*jetItr)->tagInfo<Analysis::TruthInfo>("TruthInfo");
    if (! mcinfo) {
      msg() << MSG::VERBOSE << "could not find TruthInfo for matching jet" << endmsg;
      continue;
    }
    string label = mcinfo->jetTruthLabel();

    std::pair<double, double> sfResult =
      m_calibrationInterface->getScaleFactor(**jetItr, label, m_operatingPoint, m_uncertaintyType);
    std::pair<double, double> effResult =
      m_calibrationInterface->getEfficiency(**jetItr, label, m_operatingPoint, m_uncertaintyType);

    msg() << MSG::VERBOSE << "Jet with " << m_tagger << " weight: " << weight;
    msg() << MSG::VERBOSE << " label: " << label;
    msg() << MSG::VERBOSE << " SF (unc.): " << sfResult.first
	  << "(" << sfResult.second << ")";
    msg() << MSG::VERBOSE << " eff (unc.): " << effResult.first
	  << "(" << effResult.second << ")" << endmsg;

  }

  return StatusCode::SUCCESS;
}

//============================================================================================

