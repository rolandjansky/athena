/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// TrigT1CaloCalibUtils includes
#include "L1CaloDumpRampData.h"

#include <fstream> // std::ofstream
#include <ios> // std::dec, std::hex
#include <sstream>

#include <TF1.h>
#include <TFile.h>
#include <TGraphErrors.h>

#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "CxxUtils/make_unique.h"
#include "TrigT1CaloCalibConditions/L1CaloRampDataContainer.h"

L1CaloDumpRampData::L1CaloDumpRampData(const std::string& name, ISvcLocator* pSvcLocator)
  : AthHistogramAlgorithm(name, pSvcLocator)
{
  //declareProperty( "Property", m_nProperty ); //example property declaration
}


L1CaloDumpRampData::~L1CaloDumpRampData() {}


StatusCode L1CaloDumpRampData::initialize()
{
  ATH_MSG_INFO("Initializing " << name() << "...");

  return StatusCode::SUCCESS;
}

namespace {
void writeChannel(std::ofstream& S, uint32_t cool, double slope, double offset, double chi)
{
  S << "<Channel coolid='0x" << std::hex << cool
    << std::dec << "' slope='" << slope
    << "' offset='" << offset
    << "' chi2='" << chi << "/>\n";
}

std::string getName(uint32_t cool)
{
  std::ostringstream S;
  S << "0x" << std::hex << cool;
  return std::string(S.str());
}

std::string getTitle(uint32_t cool)
{
  return "Ramp " + getName(cool);
}

} // anonymous namespace

StatusCode L1CaloDumpRampData::finalize()
{
  using CxxUtils::make_unique;
  ATH_MSG_INFO("Finalizing " << name() << "...");

  const L1CaloRampDataContainer* rampDataContainer = nullptr;
  CHECK_RECOVERABLE(detStore()->retrieve(rampDataContainer, "/L1CaloRampMaker/L1CaloRampDataContainer"));

  const CondAttrListCollection* energyScanResults = nullptr;
  CHECK_RECOVERABLE(detStore()->retrieve(energyScanResults, "/TRIGGER/L1Calo/V1/Results/EnergyScanResults"));

  std::ofstream xmlFile("rampdata.xml");
  xmlFile << "<?xml version='1.0' encoding='utf-8'?>\n"
          << "<TriggerTowerDecoratorData>\n" 
          << "<default name='slope' />\n";

  std::unique_ptr<TGraphErrors> graph_temp = make_unique<TGraphErrors>();
  std::unique_ptr<TF1> func = make_unique<TF1>("func", "pol1", 0., 255.);
  for(auto rampDataIt : *rampDataContainer) {
    auto coolId = rampDataIt.first;
    const L1CaloRampData& rampData = rampDataIt.second;
    ATH_MSG_DEBUG("Processing RampData for 0x" << std::hex << coolId << std::dec);

    // create a TGraphErrors of the energy steps
    graph_temp->SetNameTitle(getName(coolId).c_str(), getTitle(coolId).c_str());
    TGraphErrors* graph = static_cast<TGraphErrors*>(this->bookGetPointer(*graph_temp));

    unsigned int nSteps = rampData.getNSteps();
    graph->Set(nSteps);
    for(unsigned int iStep = 0; iStep < nSteps; ++iStep) {
      auto dataPoint = rampData.getStep(iStep);
      if(!dataPoint) {
        ATH_MSG_WARNING("Recieved nullptr for step " << iStep
                        << " and coolId 0x" << std::hex << coolId << std::dec);
        continue;
      }

      if(iStep) {
        // avoid problems due to saturation effects
        auto* prevStep = rampData.getStep(iStep - 1);
        if(prevStep && prevStep->second.mean() > dataPoint->second.mean()) continue;
      }

      graph->SetPoint(iStep, dataPoint->second.mean(), dataPoint->first.mean());
      graph->SetPointError(iStep, dataPoint->second.rms(), dataPoint->first.rms());
    }

    // overlay the fit function
    auto attrListIt = energyScanResults->chanAttrListPair(coolId);
    if(attrListIt != energyScanResults->end()) {
      auto attrList = attrListIt->second;
      double slope = attrList["Slope"].data<double>();
      double offset = attrList["Offset"].data<double>();
      double chi2 = attrList["Chi2"].data<double>();

      func->SetParameters(offset, slope);
      graph->GetListOfFunctions()->Add(func->Clone(("func" + getName(coolId)).c_str()));

      writeChannel(xmlFile, coolId, slope, offset, chi2);
    } else {
      ATH_MSG_WARNING("No database entry for 0x" << std::hex
                      << coolId << std::dec);
    }
  }

  xmlFile << "</TriggerTowerDecoratorData>\n";

  return StatusCode::SUCCESS;
}

StatusCode L1CaloDumpRampData::execute()
{  
  return StatusCode::SUCCESS;
}
