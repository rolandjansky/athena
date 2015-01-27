/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * MuonTPPlots.cxx
 *
 *  Created on: Aug 31, 2014
 *      Author: goblirsc
 */

#include "MuonTPTools/MuonTPPlotTool.h"

MuonTPPlotTool::MuonTPPlotTool(std::string name)
	  : AsgTool(name){
	  declareProperty("EfficiencyFlag",        m_efficiencyFlag = "ZmmTP");
}

StatusCode MuonTPPlotTool::RegisterPlots (ToolHandleArray<IMuonTPSelectionTool> & probeTools, ToolHandleArray<IMuonTPEfficiencyTool> & effTools){


	  // Create histograms
	  for(auto tpSelTool : probeTools) {
	    std::string cfdirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag();
	    m_TPcutFlowPlots[cfdirs] = AddCutFlowPlots(cfdirs+"/CutFlows");
        for (auto plot :  m_TPcutFlowPlots[cfdirs]){
            plot->initialize();
            tpSelTool->AddCutFlowHist(plot);    // show this plot to the selection tool, which can then internally fill it
        }
	    for(auto tpEffTool : effTools) {
	      std::string dirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag()+"/"+tpEffTool->efficiencyFlag();
	      if (m_probeTPEffPlots.find(dirs) ==m_probeTPEffPlots.end()){
	    	  m_probeTPEffPlots[dirs] = AddPlots(dirs+"/Probes", false);
	    	  m_matchTPEffPlots[dirs] = AddPlots(dirs+"/Matches", true);
	    	  m_effTPEffPlots[dirs] = AddPlots(dirs+"/Efficiency", true);

	    	  // now we can initialize everything
	    	  for (auto plot :  m_probeTPEffPlots[dirs]){
	    		  plot->initialize();
	    	  }
	    	  for (auto plot :  m_matchTPEffPlots[dirs]){
	    		  plot->initialize();
	    	  }
	    	  for (auto plot :  m_effTPEffPlots[dirs]){
	    		  plot->initialize();
	    	  }
	      }
	    }
	  }
  return StatusCode::SUCCESS;

}
std::vector<MuonTPEfficiencyPlotBase*> MuonTPPlotTool::AddPlots(std::string , bool ){
	// here, you can add any MuonTPEfficiencyPlotBase* based plot you like to the output!
	// in this base class, we don't add anything
	return std::vector<MuonTPEfficiencyPlotBase*>(0);

}

std::vector<MuonTPCutFlowBase*> MuonTPPlotTool::AddCutFlowPlots(std::string){
    // here, you can add any MuonTPCutFlowBase* based plot you like to the output!
    // in this base class, we don't add anything
    return std::vector<MuonTPCutFlowBase*>(0);

}

 // fill the histos
 void MuonTPPlotTool::fill(Probe& probe, ToolHandle <IMuonTPSelectionTool> & tpSelTool, ToolHandle <IMuonTPEfficiencyTool> eff_tool){

     std::string dirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag()+"/"+eff_tool->efficiencyFlag();
     for (auto plot : m_probeTPEffPlots[dirs]){
    	 plot->fill(probe);
     }
     for (auto plot : m_matchTPEffPlots[dirs]){
    	 plot->fill(probe);
     }
 }

 // fill the cut flow histos
 void MuonTPPlotTool::fillCutFlow(std::string stage, double weight, ToolHandle <IMuonTPSelectionTool> & tpSelTool){

     std::string dirs = m_efficiencyFlag+"/"+tpSelTool->efficiencyFlag();
     for (auto plot : m_TPcutFlowPlots[dirs]){
         plot->fill(stage, weight);
     }
 }
  /// retrieve booked histograms
  std::vector<HistData> MuonTPPlotTool::retrieveBookedHistograms(){

	  std::vector<HistData> histData;
	  for(auto plots : m_probeTPEffPlots) {
		  for (auto hist : plots.second){
			  std::vector<HistData> histDataTmp = hist->retrieveBookedHistograms();
			  histData.insert(histData.end(), histDataTmp.begin(), histDataTmp.end());
		  }
	  }
	  for(auto plots : m_matchTPEffPlots) {
		  for (auto hist : plots.second){
			  std::vector<HistData> histDataTmp = hist->retrieveBookedHistograms();
			  histData.insert(histData.end(), histDataTmp.begin(), histDataTmp.end());
		  }
	  }
	  for(auto plots : m_effTPEffPlots) {
		  for (auto hist : plots.second){
			  std::vector<HistData> histDataTmp = hist->retrieveBookedHistograms();
			  histData.insert(histData.end(), histDataTmp.begin(), histDataTmp.end());
		  }
	  }
	  for (auto plots: m_TPcutFlowPlots) {
          for (auto hist : plots.second){
              std::vector<HistData> histDataTmp = hist->retrieveBookedHistograms();
              histData.insert(histData.end(), histDataTmp.begin(), histDataTmp.end());
          }
	  }

	  return histData;
  }
void  MuonTPPlotTool::CalcEff(void){

	for(auto deniter : m_probeTPEffPlots) {
		std::vector<MuonTPEfficiencyPlotBase*> denplots = deniter.second;
		std::vector<MuonTPEfficiencyPlotBase*> matchplots = m_matchTPEffPlots[deniter.first];
		std::vector<MuonTPEfficiencyPlotBase*> effplots = m_effTPEffPlots[deniter.first];
		for (size_t i = 0; i < denplots.size();++i){
			effplots[i]->EffiDivide(denplots[i],matchplots[i]);
		}
	}
}


  MuonTPPlotTool::~MuonTPPlotTool(){

	  for(auto plots : m_probeTPEffPlots){
		  for (auto plot : plots.second){
			  delete plot;
		  }
	  }
	  for(auto plots : m_matchTPEffPlots){
		  for (auto plot : plots.second){
			  delete plot;
		  }
	  }
      for(auto plots : m_effTPEffPlots){
          for (auto plot : plots.second){
              delete plot;
          }
      }
      for(auto plots : m_TPcutFlowPlots){
          for (auto plot : plots.second){
              delete plot;
          }
      }
	  m_probeTPEffPlots.clear();
	  m_matchTPEffPlots.clear();
	  m_effTPEffPlots.clear();
	  m_TPcutFlowPlots.clear();
  }






