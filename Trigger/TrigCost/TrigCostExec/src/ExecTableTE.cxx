/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>

// ROOT
#include "TCanvas.h"
#include "TDirectory.h"
#include "TH1.h"
#include "TH2.h"
// Local
#include "TrigCostBase/Factory.h"
#include "TrigCostBase/UtilTrig.h"
#include "TrigCostBase/HistMan.h"

// Local
#include "TrigCostExec/ExecCount.h"
#include "TrigCostExec/ExecTableTE.h"

REGISTER_ANP_OBJECT(ExecClient,ExecTableTE)

using namespace std;



//--------------------------------------------------------------------------------------      
Anp::ExecTableTE::ExecTableTE()
  :fDir(0),
   fCanvas(0),
   fDebug(false),
   fLevel(0),
   fMakeHist(false),
   fTimeout(0)
{
}

//--------------------------------------------------------------------------------------      
Anp::ExecTableTE::~ExecTableTE()
{
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableTE::SetReg(const Registry &reg)
{
  //
  // Copy configuration
  //
  fReg = reg;
  
  //
  // Read configuration
  //
  reg.Get("Debug",      fDebug);
  reg.Get("Level",      fLevel);
  reg.Get("Timeout",    fTimeout);
  reg.Get("HistOpt",    fHistOpt);
  reg.Get("DirName",    fDirName);
  reg.Get("MakeHist",   fMakeHist);
  reg.Get("ClientLink", fClientLink);

  if(fMakeHist) {
    fCanvas = new TCanvas(GetClientName().c_str(), "", 650, 400);
  }

  std::vector<std::string> StringVec;
  Anp::Registry xml_reg = Anp::ParseXml("$TestArea/Trigger/TrigCost/TrigCostExec/data/Combined_Trigger.xml" , true);
  std::string trigstring;

  while(xml_reg.KeyExists("trigger")) {

    Registry range_reg;
    xml_reg.Get("trigger", range_reg);
    xml_reg.RemoveKey("trigger");
    while(range_reg.KeyExists("sequence")) {
 	  range_reg.Get("sequence", trigstring);
 	  range_reg.RemoveKey("sequence");
	  StringVec.push_back(trigstring);
          if(fDebug){
             std::cout << "Combined trigger: " << trigstring << std::endl;
          }
    }
    fCombinedTriggers.push_back(StringVec);
    StringVec.clear();
  }

  fPassCount.clear();
  std::string CombTrigName("Comb");  

  for(unsigned int i = 0; i < fCombinedTriggers.size(); ++i){
    fPassCount.push_back(0);
    for(unsigned int i = 0; i < StringVec.size(); ++i){
	CombTrigName += StringVec[i];
        if(fDebug) std::cout << StringVec[i] << std::endl;
    }
  }

}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableTE::SetDir(TDirectory *dir)
{
  //
  // Get directory and create histograms
  //
  if(fDebug) cout << "ExecTableTE:SetDir - fDirName: " << fDirName << endl; 
  if(!fMakeHist) return;

  fDir = Anp::GetDir(dir, fDirName);
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableTE::ProcessAlg(const Exec::AlgCall &alg)
{
  if(fDebug){
    std::cout << " algorithm: " << alg.alg_conf->getName() << " in sequence " << alg.seq_conf->getName() << "in chain: " <<    alg.chn_conf->getName() << " Ran on: " << std::endl; 
    for(unsigned int i = 0; i < alg.roi_data.size(); ++i){
      std::cout << setw(10);
      std::cout << alg.roi_data[i].roi_id; 
      //if(alg.roi_data[i].isPassed) std::cout << " passed";
      //else std::cout << " did not pass";
      std::cout << endl;
    }
 }

    fTEs.push_back(alg);
/* 
    ExecClient *client = GetChnClient(*alg.chn_conf);
    client->ProcessAlg(alg);
cd 
    fActiveClients.push_back(client);
*/
} 

//--------------------------------------------------------------------------------------      
void Anp::ExecTableTE::FinishEvent(const Exec::Event &/*event*/)
{
  //
  // Find the client pointer for run and lb; then process AlgCalls
  //

    for(unsigned int i  = 0; i <  fCombinedTriggers.size(); ++i){
      vector<bool> PassVec(fCombinedTriggers[i].size(),false);
      bool CombTrigPassed; 
      CombTrigPassed = true;
      for(unsigned int j = 0; j < fTEs.size(); ++j) {
          for(unsigned int k = 0; k < fTEs[j].roi_data.size(); ++k){
		
              if(!fTEs[j].roi_data[k].isPassed) continue;
	      //	std::cout << "roi " << k << " Seq " << fTEs[j].seq_conf->getName() << " passed " << std::endl;
	      for(unsigned int h = 0; h < fCombinedTriggers[i].size(); ++h){
		//std::cout << " checking: " << fCombinedTriggers[i][h] << std::endl;
	      	if(/*fTEs[j].seq_conf->getName().compare(fCombinedTriggers[i][h]) == 0 ||*/ fTEs[j].chn_conf->getName().compare(fCombinedTriggers[i][h]) == 0/*string::npos*/)  { 
			PassVec[h] = true;

//			std::cout << " Sequence passed: " << fTEs[j].seq_conf->getName() << "!" << std::endl;
//			std::cout << " Compared to : " << fCombinedTriggers[i][h] << "!" << std::endl;
		}
 	      }
          }
      }

      for(unsigned int j = 0; j < PassVec.size(); ++j){
      	CombTrigPassed = CombTrigPassed && PassVec[j];
        if(fDebug) std::cout << "Trigger: " <<  fCombinedTriggers[i][j] << " " << CombTrigPassed << std::endl;
      }
      if(CombTrigPassed) fPassCount[i]++;

    }

//  std::cout << " fSimplecounter: " << fSimplecounter << std::endl;
//  std::cout << " PassCount: " << fPassCount[1] << std::endl;
  fTEs.clear();
}

const Anp::Exec::Count& Anp::ExecTableTE::GetCount() const
{
  return fCount;
}

//--------------------------------------------------------------------------------------      
void Anp::ExecTableTE::WriteExec(HtmlNode &anode, HtmlMenu &/*hmenu*/, const Exec::Summary &/*sum*/)
{
 //
  // Write out results
  //
  ostream &os = anode.GetCont();

  os << "Predictions for combined triggers from the sequences 1 to N " << endl;
//	 std::cout << " WriteExec fPassCount: " << fPassCount[1] << endl; 

  os << "<script src=\"sorttable.js\"></script>" << endl	
     << "<p style=\"font-family:times; font-size:11px;\">" << endl
     << "<table border=\"1\" cellpadding=\"2\" cellspacing=\"0\" class=\"sortable\">" << endl
     << "<tr>";

  unsigned int largestSize = 0;
  for(unsigned int i = 0; i <fCombinedTriggers.size(); ++i){
    if(fDebug) std::cout << " CombinedTriggers size: " << fCombinedTriggers[i].size() <<std::endl;
    if(fCombinedTriggers[i].size() > largestSize) largestSize = fCombinedTriggers[i].size();
  }

  	
  for(unsigned int i = 0; i <largestSize; ++i){
    os << "<th> Sequence " << 1+i << " </th>" << endl;
  }
    os << "<th> Counts </th>";
  

  //
  // Count all sequences.
  //

  for(unsigned int i = 0; i <fCombinedTriggers.size(); ++i){
      
          //if(!PassVec[i]) continue;

    // Write HTML table entries
    //
      os << "<tr>" 
         << "<td align=\"left\" style=\"word-wrap:break-word\">";
         for(unsigned int j = 0; j <fCombinedTriggers[i].size(); ++j){
           os << fCombinedTriggers[i][j]<< "<td align=\"center\"> ";
	 }
      os << fPassCount[i]               << " </td>";

      os << "</tr>" << endl;
    }
  os << "</table>" << endl;
}


//--------------------------------------------------------------------------------------      
