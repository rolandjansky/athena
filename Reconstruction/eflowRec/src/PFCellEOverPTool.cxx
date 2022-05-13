/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "eflowRec/eflowEEtaBinnedParameters.h"
#include "eflowRec/PFCellEOverPTool.h"
#include "eflowRec/eflowCaloRegions.h"

#include "GaudiKernel/SystemOfUnits.h"

#include <nlohmann/json.hpp>

#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>

PFCellEOverPTool::PFCellEOverPTool(const std::string& type,const std::string& name,const IInterface* parent) : IEFlowCellEOverPTool( type, name, parent) {

  declareInterface<IEFlowCellEOverPTool>(this);

  m_eBinValues.push_back(  1.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 3.5*Gaudi::Units::GeV );
  m_eBinValues.push_back( 10.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 25.0*Gaudi::Units::GeV );
  m_eBinValues.push_back( 32.5*Gaudi::Units::GeV );
  m_eBinValues.push_back( 40.0*Gaudi::Units::GeV );

  for (double x = 0.0; x < 2.0; x+=0.5) {
    m_etaBinBounds.push_back(x);
  }
  m_etaBinBounds.push_back(2.5);

}

StatusCode PFCellEOverPTool::initialize(){

  return StatusCode::SUCCESS;
}


std::string_view PFCellEOverPTool::eBinsToString(const E_BINS& e_bins) const {

  switch(e_bins){
    case E001bin : return "E001bin";
    case E003point5bin: return "E003point5bin";
    case E010bin: return "E010bin";
    case E020bin: return "E020bin";
    case E032point5bin: return "E032point5bin";
    case E040bin: return "E040bin";
    default: throw std::invalid_argument("Please choose a valid E_BINS value");
  }
}

std::string_view PFCellEOverPTool::etaBinsToString(const ETA_BINS& eta_bins) const {

  switch(eta_bins){
    case eta050bin : return "eta050bin";
    case eta100bin : return "eta100bin";
    case eta150bin : return "eta150bin";
    case eta250bin : return "eta250bin";
    case eta350bin : return "eta350bin";
    case eta450bin : return "eta450bin";
    default: throw std::invalid_argument("Please choose a valid ETA_BINS value");
  }
}


StatusCode PFCellEOverPTool::fillBinnedParameters(eflowEEtaBinnedParameters *binnedParameters) const {

  if (binnedParameters) {

    binnedParameters->initialise(m_eBinValues, m_etaBinBounds);
             
    std::ifstream inputFile_eoverp(m_referenceFileLocation+"EOverP.json");
    nlohmann::json json_eoverp;
    inputFile_eoverp >> json_eoverp;  

    std::ifstream inputFile_cellOrdering(m_referenceFileLocation+"CellOrdering.json");
    nlohmann::json json_cellOrdering;
    inputFile_cellOrdering >> json_cellOrdering;

    //Loop over energy, eta and first int bins
    //For each combination we set the e/p mean and width from the json file values
    for (auto thisEBin : m_e_bins){
      //convert string_view to string so we can combine with "" later on.
      std::string currentEBin(eBinsToString(thisEBin));
      for (auto thisEtaBin : m_eta_bins){
        //convert string_view to string so we can combine with "" later on.
        std::string currentEtaBin(etaBinsToString(thisEtaBin));
        for (auto thisFirstIntRegionBin : m_firstInt_bins){
          std::string currentFirstIntBin = eflowFirstIntRegions::name(thisFirstIntRegionBin);
          std::string eOverPBin = currentEBin+"_"+currentEtaBin+"_"+currentFirstIntBin;
          if (json_eoverp.find(eOverPBin+"_mean") != json_eoverp.end()){
            binnedParameters->setFudgeMean(thisEBin,thisEtaBin,thisFirstIntRegionBin,json_eoverp[eOverPBin+"_mean"]);
          }
          if (json_eoverp.find(eOverPBin+"_sigma") != json_eoverp.end()){
            binnedParameters->setFudgeStdDev(thisEBin,thisEtaBin,thisFirstIntRegionBin,json_eoverp[eOverPBin+"_sigma"]);
          }
          for (auto thisLayerBin : m_layerBins){
            std::string currentLayerBin = eflowCalo::name(thisLayerBin);            
            std::string cellOrderingBin = currentEBin+"_"+currentEtaBin+"_"+currentFirstIntBin+"-"+currentLayerBin;
            if (json_cellOrdering.find(cellOrderingBin+"_norm1") != json_cellOrdering.end()){
              binnedParameters->setShapeParam(thisEBin,thisEtaBin,thisFirstIntRegionBin,thisLayerBin,NORM1,json_cellOrdering[cellOrderingBin+"_norm1"]);
            }
            if (json_cellOrdering.find(cellOrderingBin+"_sigma1") != json_cellOrdering.end()){
              binnedParameters->setShapeParam(thisEBin,thisEtaBin,thisFirstIntRegionBin,thisLayerBin,WIDTH1,json_cellOrdering[cellOrderingBin+"_sigma1"]);
            }
            if (json_cellOrdering.find(cellOrderingBin+"_norm2") != json_cellOrdering.end()){
              binnedParameters->setShapeParam(thisEBin,thisEtaBin,thisFirstIntRegionBin,thisLayerBin,NORM2,json_cellOrdering[cellOrderingBin+"_norm2"]);
            }
            if (json_cellOrdering.find(cellOrderingBin+"_sigma2") != json_cellOrdering.end()){
              binnedParameters->setShapeParam(thisEBin,thisEtaBin,thisFirstIntRegionBin,thisLayerBin,WIDTH2,json_cellOrdering[cellOrderingBin+"_sigma2"]);
            }            
          }
        }
      }
    }    
  }

  return StatusCode::SUCCESS;

}

StatusCode PFCellEOverPTool::finalize(){
  return StatusCode::SUCCESS;
}
